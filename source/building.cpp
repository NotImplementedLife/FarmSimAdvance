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


Building::Building(const short* res_gfx, int px_width, int px_height, int rows_count, int cols_count, const char* collision_matrix)
{
	this->res_gfx = res_gfx;
	this->px_width = px_width;
	this->px_height = px_height;
	this->rows_count = rows_count;
	this->cols_count = cols_count;
	this->collision_matrix = collision_matrix;
}

Building::Building(const short* res_gfx)
{
	this->res_gfx = res_gfx;
	if(res_gfx == BLD_SMALL_PLOT || res_gfx == BLD_SAND_ROAD)
	{
		px_width = 24, px_height = 16, rows_count = 1, cols_count = 1; collision_matrix=COL_1x1_16; return;
	}
	else if(res_gfx == BLD_MEDIUM_PLOT)	
	{
		px_width = 72, px_height = 32, rows_count = 3, cols_count = 3; collision_matrix=COL_2x2_32; return;
	}
	else if(res_gfx == BLD_LARGE_PLOT)	
	{
		px_width = 72, px_height = 48, rows_count = 5, cols_count = 3; collision_matrix=COL_3x3_48; return;
	}	
	else if(res_gfx == BLD_CHICKEN_COOP)	
	{
		px_width = 120, px_height = 80, rows_count = 9, cols_count = 5; collision_matrix=COL_4x4_80; return;
	}
	else
	{
		FATAL_ERROR("res_gfx unknown");
	}	
}

Building::Building(const Building* building)
{
	this->res_gfx = building->res_gfx;
	this->px_width = building->px_width;
	this->px_height = building->px_height;
	this->rows_count = building->rows_count;
	this->cols_count = building->cols_count;
	this->collision_matrix = building->collision_matrix;
}

const short* Building::get_res_gfx() const { return res_gfx;}

int Building::get_px_width() const { return px_width;}
int Building::get_px_height() const {return px_height;}

int Building::get_rows_count() const {return rows_count;}
int Building::get_cols_count() const {return cols_count;}

const char* Building::get_collision_matrix() const { return collision_matrix;}

using namespace Astralbrew;


void Building::copy_gfx(int src_x, int src_y, int src_width, int src_height, void* dest, int dest_stride, int dest_x, int dest_y, int flags) const
{
	assert(src_x%2==0);
	assert(src_width%2==0);
	assert(dest_stride%8==0);
	assert(src_x+src_width<=px_width);
	assert(src_y+src_height<=px_height);	
	assert(dest_x + src_width <= dest_stride);
	assert(dest_x%2==0);	
	
	bool as_tiles = flags & BLD_TILES;
	bool invalid = flags & BLD_INVALID;
	
	int w = min(src_width, dest_stride);	
	short* dst = (short*)((int)dest + dest_y*dest_stride+dest_x);
	const short* src = (const short*)((int)res_gfx + px_width*src_y + src_x);
	
	if(!as_tiles)
	{
		for(int iy=0;iy<src_height;iy++)
		{
			for(int ix=0;ix<w/2;ix++)
			{
				short t =dst[ix];
				short s = *(src++);			
							
				if(invalid && (iy&1))
				{
					if((s&0x00FF)!=0) s = (s&0xFF00)|0x00CF;
					if((s&0xFF00)!=0) s = (s&0x00FF)|0xCF00;
				}						
				if((s&0x00FF)!=0) t = (t&0xFF00)|(s&0x00FF);
				if((s&0xFF00)!=0) t = (t&0x00FF)|(s&0xFF00);										
				dst[ix]=t;
			}
			src = src-w/2+px_width/2;
			dst+=dest_stride/2;
		}	
	}	
	else //if(as_tiles)
	{		
		for(int iy=0;iy<src_height;iy++)
		{
			for(int ix=0;ix<w/2;ix++)
			{
				int y = dest_y + iy;
				int x = dest_x + 2*ix;
				int ty = y/8;
				int tx = x/8;
				x %= 8;
				y %= 8;
				int tstride = dest_stride/8;
				
				short* val = &(((short*)dest)[(tstride*ty+tx)*32 + 4*y + x/2]);
				
				short t = *val;
				short s = *(src++);			
				
				if(invalid && (iy&1))
				{
					if((s&0x00FF)!=0) s = (s&0xFF00)|0x00CF;
					if((s&0xFF00)!=0) s = (s&0x00FF)|0xCF00;
				}						
				if((s&0x00FF)!=0) t = (t&0xFF00)|(s&0x00FF);
				if((s&0xFF00)!=0) t = (t&0x00FF)|(s&0xFF00);										
				*val=t;
			}
			src = src-w/2+px_width/2;
			dst+=dest_stride/2;
		}			
	}
}

bool Building::can_be_placed_on(const int* map, int stride, int row, int col) const
{
	bool row_shift = (row % 2 == 1);
	
	const int* tl = map+row*stride+col;	
	const char* cm = collision_matrix;
	
	for(int r=0;r<rows_count;r++)
	{
		for(int c=0;c<cols_count;c++)
		{
			if(*(cm++)!=0 && tl[c+((r&1)&&row_shift)]!=0)
				return false;
		}
		tl+=stride;
	}			
	return true;
}