#pragma once

#include <Astralbrew>
#include "building.hpp"
#include "farm.hpp"
#include "log.hpp"

class Timer
{
protected:
	Building* target_building;
	int ticks_count;	
public:
	Timer(Building* building, int ticks);
	
	int decrease_tick(Farm* farm);
	
	virtual int on_tick_end(Farm* farm) = 0;	
	
	inline Building* get_building() const {return target_building;}
	
	bool done() const;
	
	virtual ~Timer() = default;
};

class CropsTimer : public Timer
{
private:
	int stage = 0;
public:
	CropsTimer(Building* building);
	virtual int on_tick_end(Farm* farm) override;
};

enum TimerFlags
{
	FORCE_REDRAW = (1<<0)	
};

class ChickenTimer : public Timer
{
public:
	virtual int on_tick_end(Farm* farm) override;
};

class TimerProcessor
{
private:
	Astralbrew::Vector<Timer*> active_timers;
	Farm* farm;
	int freq = 127;
	void update_freq();
public:
	TimerProcessor(Farm* farm);
	void add_timer(Timer* timer);
	int update();	
	
	void remove_building(Building* building);
	~TimerProcessor() = default;
};