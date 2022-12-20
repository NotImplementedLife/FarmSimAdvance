#pragma once

#include "metamap.hpp"

class Farm
{
private:
	int eggs_count = 0;
	int seeds_count = 10;
	int wheat_count = 0;
	int balance = 100;
	Metamap* metamap;
public:
	
	int get_eggs_count() const;
	int get_seeds_count() const;
	int get_wheat_count() const;
	int get_balance() const;
	Metamap* get_metamap() const;
	
	void set_metamap(Metamap* metamap);
};