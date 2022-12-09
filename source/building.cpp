#include "building.hpp"

__attribute__((section(".ewram.bld")))
short BLD_SMALL_PLOT   [ 24 * 16 / 2];
__attribute__((section(".ewram.bld")))
short BLD_MEDIUM_PLOT  [ 72 * 32 / 2];
__attribute__((section(".ewram.bld")))
short BLD_LARGE_PLOT   [ 72 * 48 / 2];
__attribute__((section(".ewram.bld")))
short BLD_SAND_ROAD    [ 24 * 16 / 2];
__attribute__((section(".ewram.bld")))
short BLD_CHICKEN_COOP [120 * 80 / 2];


#include "map.h"
void extract_from_map(short* dest, int x, int y,int w, int h)
{
	w/=2;
	short* gfx = (short*)((int)ROA_map_gfx+2424*y+x);
	for(int iy=0;iy<h;iy++)
	{
		for(int ix=0;ix<w;ix++)
		{
			*(dest++) = gfx[ix];
		}
		gfx += 2424 / sizeof(short);
	}
}

void init_buildings_gfx()
{
	extract_from_map(BLD_SMALL_PLOT  , 1776, 80,  24, 16);
	extract_from_map(BLD_MEDIUM_PLOT , 1812, 72,  72, 32);
	extract_from_map(BLD_LARGE_PLOT  , 1896, 64,  72, 48);
	extract_from_map(BLD_SAND_ROAD   , 1992, 80,  24, 16);
	extract_from_map(BLD_CHICKEN_COOP, 2076, 56, 120, 80);
}