#include "farm.hpp"

int Farm::get_eggs_count() const {return eggs_count; }
int Farm::get_seeds_count() const { return seeds_count; }
int Farm::get_wheat_count() const { return wheat_count; }
int Farm::get_balance() const { return balance; }
Metamap* Farm::get_metamap() const { return metamap; }
void Farm::set_metamap(Metamap* metamap) {this->metamap = metamap;}

bool Farm::use_wheat(int quant)
{
	if(quant>wheat_count)
		return false;	
	wheat_count-=quant;	
	return true;	
}

void Farm::add_wheat(int quant)
{
	wheat_count+=quant;	
}

bool Farm::use_eggs(int quant)
{
	if(quant>eggs_count)
		return false;	
	eggs_count-=quant;	
	return true;		
}

void Farm::add_eggs(int quant)
{
	eggs_count+=quant;
}