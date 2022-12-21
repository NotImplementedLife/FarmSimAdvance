#include "timer.hpp"

Timer::Timer(Building* building, int ticks) : target_building(building), ticks_count(ticks)
{ }

int Timer::decrease_tick(Farm* farm)
{
	if(ticks_count==0) return 0;
	ticks_count--;	
	if(ticks_count==0)
	{
		return on_tick_end(farm);
	}	
	return 0;
}

bool Timer::done() const { return ticks_count==0; }

CropsTimer::CropsTimer(Building* building) : Timer(building, 20) { }

int CropsTimer::on_tick_end(Farm* farm) 
{
	no$log("crops");
	if(stage<3)
	{
		stage++;
		farm->get_metamap()->update_crops(target_building);
		
		ticks_count = stage==3 ? 0 : (target_building->is_watered() ? 10:20);
		
		return FORCE_REDRAW;
	}	
	return 0;
}

int ChickenTimer::on_tick_end(Farm* farm)
{
	return 0;
}

namespace
{
	unsigned long long __qrand_state = 0x12345679;
	unsigned int qrand()
	{
		__qrand_state *= 0xFCF35B35;
		return __qrand_state>>32;
	}
}

TimerProcessor::TimerProcessor(Farm* farm) : farm(farm) 
{
	
}

void TimerProcessor::add_timer(Timer* timer)
{
	active_timers.push_back(timer);
	update_freq();
}

void TimerProcessor::update_freq()
{
	if(active_timers.size()>=729)
		freq = 1;
	else if(active_timers.size()>=243)
		freq = 3;
	else if(active_timers.size()>=81)
		freq = 7;
	else if(active_timers.size()>=27)
		freq = 15;
	else if(active_timers.size()>=9)
		freq = 31;
	else if(active_timers.size()>=3)
		freq = 63;
	else
		freq = 127;
}

int TimerProcessor::update()
{
	int result = 0;
	if(active_timers.size()==0) return 0;
	if((qrand()&freq)<5)
	{				
		int i = qrand()%active_timers.size();
		Timer* timer = active_timers[i];		
		result = timer->decrease_tick(farm);
		if(timer->done()) 
		{
			active_timers.remove_at(i);
			delete timer;
			update_freq();
		}		
	}
	return result;
}

void TimerProcessor::remove_building(Building* building)
{
	for(int i=0;i<active_timers.size();i++)
	{
		Timer* timer = active_timers[i];
		if(timer->get_building()==building)
		{
			active_timers.remove_at(i);
			delete timer;			
			update_freq();
		}
	}	
}

Timer* TimerProcessor::get_timer(Building* building) const
{
	for(int i=0;i<active_timers.size();i++)
	{
		Timer* timer = active_timers[i];
		if(timer->get_building()==building)
		{			
			return timer;						
		}		
	}
	return nullptr;
}