#include "farm.hpp"

int Farm::get_eggs_count() const {return eggs_count; }
int Farm::get_seeds_count() const { return seeds_count; }
int Farm::get_wheat_count() const { return wheat_count; }
int Farm::get_balance() const { return balance; }
Metamap* Farm::get_metamap() const { return metamap; }
void Farm::set_metamap(Metamap* metamap) {this->metamap = metamap;}