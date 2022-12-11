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


Building::Building(const short* res_gfx, int px_width, int px_height, int rows_count, int cols_count)
{
	this->res_gfx = res_gfx;
	this->px_width = px_width;
	this->px_height = px_height;
	this->rows_count = rows_count;
	this->cols_count = cols_count;
}

const short* Building::get_res_gfx() const { return res_gfx;}

int Building::get_px_width() const { return px_width;}
int Building::get_px_height() const {return px_height;}

int Building::get_rows_count() const {return rows_count;}
int Building::get_cols_count() const {return cols_count;}

using namespace Astralbrew;


void Building::copy_gfx(int src_x, int src_y, int src_width, int src_height, void* dest, int dest_stride, int dest_x, int dest_y, bool as_tiles) const
{
	assert(src_x%2==0);
	assert(src_width%2==0);
	assert(dest_stride%8==0);
	assert(src_x+src_width<=px_width);
	assert(src_y+src_height<=px_height);	
	assert(dest_x + src_width <= dest_stride);
	assert(dest_x%2==0);
	
	int w = min(src_width, dest_stride);	
	short* dst = (short*)((int)dest + dest_y*dest_stride+dest_x);
	const short* src = (const short*)((int)res_gfx + px_width*src_y + src_x);
	
	
	for(int iy=0;iy<src_height;iy++)
	{
		for(int ix=0;ix<w/2;ix++)
		{
			short t =dst[ix];
			short s = *(src++);
			if((s&0x00FF)!=0) t = (t&0xFF00)|(s&0x00FF);
			if((s&0xFF00)!=0) t = (t&0x00FF)|(s&0xFF00);				
			dst[ix]=t;
		}
		src = src-w/2+px_width/2;
		dst+=dest_stride/2;
	}
	
	if(as_tiles)
	{
		int tmp[2];
		int* buf = (int*)dest;
		const int c = dest_stride/8;
		const int r = 8;
		
		short* perm = new short[r*c];
		for(int i=0;i<r*c-1;i++)
			perm[i] = c*i%(r*c-1);
		perm[r*c-1] = r*c-1;		
		
		int ny = (dest_y + src_height+7)/8;
		for(int k=0;k<ny;k++)
		{			
			for(int i=0;i<r*c-1;i++) perm[i]&=0x7fff;
			
			for(int i=0;i<r*c-1;i++)
			{
				if(perm[i]&0x8000) continue;		
				perm[i]|=0x8000;
				
				tmp[0] = buf[2*i], tmp[1] = buf[2*i+1];	// tmp = at(i)
				int j=i, next;			
				do
				{		
					next = perm[j]&0x7fff;
					if(next==i) 
					{
						buf[2*j]=tmp[0], buf[2*j+1]=tmp[1]; //at(j)=tmp
					}
					else 
					{
						buf[2*j]=buf[2*next], buf[2*j+1]=buf[2*next+1]; //at(j) = at(perm(j))
					}
					
					perm[j]|=0x8000;
				}
				while((j = next)!=i);							
			}	
			buf += 2*(r*c);
		}		
		
		delete[] perm;
	}
}