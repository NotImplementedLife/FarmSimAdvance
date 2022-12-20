#include "metamap.hpp"

#include "border_48_73_bin.h"

Metamap::Metamap()
{
	char* src = (char*)border_48_73_bin;
	char* dst = (char*)map_metadata;
	for(int i=0;i<73*48;i++) *(dst++)=*(src++);
}

char* Metamap::get_buffer() const { return map_metadata; }

bool Metamap::can_place(const Building* building, int row, int col) const
{
	bool row_shift = (row % 2 == 1);
	
	const char* tl = map_metadata+row*stride+col;	
	const char* cm = building->get_collision_matrix();
	
	for(int r=0;r<building->get_rows_count();r++)
	{
		for(int c=0;c<building->get_cols_count();c++)
		{
			if(*(cm++)!=0 && tl[c+((r&1)&&row_shift)]!=0)
				return false;
		}
		tl+=stride;
	}			
	return true;
}

Building* Metamap::building_at(int x, int y) const
{	
	int r = (32 * y + 64 * x / 3 - 256) >> 9;
    int c = (32 * y - 64 * x / 3 + 256) >> 9;
	
	int rr = r+c;
	int cc = (r-c)/2;
	
	return bld_addr[rr*stride + cc];	
}

void Metamap::place(Building* building, int row, int col)
{
	bool row_shift = (row % 2 == 1);
	
	char* tl = map_metadata+row*stride+col;	
	Building* *bld_tl = bld_addr + row*stride+col;
	const char* cm = building->get_collision_matrix();	
	
	int min1r = -1;
	int max1r = -1;
	
	for(int r=0;r<building->get_rows_count();r++)
	{
		for(int c=0;c<building->get_cols_count();c++)
		{			
			if(*(cm++)!=0)
			{
				if(min1r<0) min1r = r;
				if(r>max1r) max1r = r;
				int pos = c+((r&1)&&row_shift);
				tl[pos]=1;
								
				bld_tl[pos] = building;
			}
		}
		tl+=stride;
		bld_tl+=stride;
	}				
	
	int mid_r = min1r + ((max1r-min1r)/2);
	
	assert(mid_r>=0);
	
	mid_r+=row;
	
	int x = col*24 + (row&1)*12;
	int y = (row/2)*16 + (row&1)*8;	
	buildings[mid_r].push_back({building, x, y,  building->get_px_width(), building->get_px_height()});
}

namespace
{
	int clamp(int x, int a, int b) 
	{
		if(x<a) return a;
		if(x>b) return b;
		return x;
	}
}

void Metamap::draw_buildings(int x, int y, int w, int h, short* dest, int dest_stride) const
{	
	assert(x%2==0);
	assert(w%2==0);
	
	int x0 = x-24;
	int y0 = y-building_max_height/2;	
	
	int r = (32 * y0 + 64 * x0 / 3 - 256) >> 9;
    int c = (32 * y0 - 64 * x0 / 3 + 256) >> 9;
	
	int r0 = clamp(r+c, 0, height-1);	
	
	int x1 = x+w;
	int y1 = y+h+3*building_max_height/4;
	
	r = (32 * y1 + 64 * x1 / 3 - 256) >> 9;
    c = (32 * y1 - 64 * x1 / 3 + 256) >> 9;	
	int r1 = clamp(r+c, 0, height-1);	
		
	for(int ir=r0;ir<=r1;ir++)
	{
		for(int i=0;i<buildings[ir].size();i++)
		{						
			const Record rec = buildings[ir][i];
			if(rec.x+rec.width < x || rec.x >= x+w) continue;
			if(rec.y+rec.height < y || rec.y >= y+h) continue;
			
			if(rec.y-y >= 160) continue;								
			
			const Building* building = rec.building;						
			
			short* dst = dest + (rec.y-y)*dest_stride/2 + (rec.x - x)/2;
			
			for(int iy=0;iy<building->get_px_height();iy++)
			{				
				if(y>rec.y+iy || rec.y+iy>=y+h) 
				{
					dst+=dest_stride/2;
					continue;
				}
				for(int ix=0;ix<building->get_px_width()/2;ix++)
				{					
					if(x>rec.x+2*ix || rec.x+2*ix>=x+w) 
					{						
						continue;
					}					
					short src = building->get_res_gfx()[iy*building->get_px_width()/2+ix];
					short tmp = dst[ix];
					
					if(src&(0x00FF)) tmp = (tmp&0xFF00)|(src&0x00FF);
					if(src&(0xFF00)) tmp = (tmp&0x00FF)|(src&0xFF00);
					
					dst[ix] = tmp;
				}				
				dst+=dest_stride/2;
			}
		}
	}	
}

void Metamap::remove(const Building* building)
{	
	if(building==nullptr) return;
	int r0 = -1;
	int r1 = 0;
	char* tl = map_metadata;	
	Building* *bld_tl = bld_addr;
	for(int r=0;r<height;r++)
	{
		for(int c=0;c<stride;c++)
		{
			if(bld_tl[c] == building)
			{
				tl[c]=0;							
				bld_tl[c] = nullptr;
				if(r0<0) r0 = r;
				r1 = r;
			}
		}
		tl += stride;
		bld_tl += stride;
	}
	
	bool found = false;
	for(int r=r0;r<=r1 && !found;r++)
	{
		for(int i=0;i<buildings[r].size();i++)
		{
			if(buildings[r][i].building == building)
			{
				buildings[r].remove_at(i);
				break;
			}
		}		
	}	
}

Metamap::~Metamap() 
{ 
	delete[] map_metadata; 
	delete[] bld_addr;
	delete[] buildings;
}

void Metamap::update_crops(Building* building)
{	
	Record* rec = nullptr;
	for(int r=0;r<=72 && !rec;r++)
	{
		for(int i=0;i<buildings[r].size();i++)
		{
			if(buildings[r][i].building == building)
			{
				rec = &buildings[r][i];				
				break;
			}
		}		
	}	
	if(rec == nullptr) return;
	
	if(!(building->is_empty_plot() || building->is_crops_growing() || building->is_crops_ready()))
	{		
		return;
	}

	Astralbrew::Point<short> rpos = rec->building->crops_next_stage();
	rec->x+=rpos.x;
	rec->y+=rpos.y;
	rec->width = rec->building->get_px_width();
	rec->height = rec->building->get_px_height();	
}