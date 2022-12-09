#include <Astralbrew>

#include "map.h"
#include <stdio.h>

#include "building.hpp"

using namespace Astralbrew::World;
using namespace Astralbrew;

#define LCD_FRAME_SELECT (1 << 4)

class MainScene : public Scene
{
private:
	MODE5_LINE* BACK = MODE5_BB;		
	MODE5_LINE* FRONT = MODE5_FB;
	
	void flip_page()
	{		
		REG_DISPCNT ^= LCD_FRAME_SELECT;
		if (REG_DISPCNT & LCD_FRAME_SELECT)
		{
			BACK = MODE5_FB;			
			FRONT = MODE5_BB;
		}
		else
		{	
			BACK = MODE5_BB;
			FRONT = MODE5_FB;			
		}		
	}		
private:		
	int scrollX = 0, scrollY = 0;
	
	char* row;

	void display(int x0, int y0, int r0, int r1)
	{				
		char* gfx = (char*)ROA_map_gfx+2424*(y0+r0)+x0;
		for(int y=r0;y<r1;y++)
		{
			char* line = gfx;
			for(int x=0;x<240;x++)
			{								
				row[x]=line[x];
			}
			char* vrow = (char*)((int)BACK+240*y);
						
			for(int i=0;i<120;i++)
				((short*)vrow)[i] = ((short*)row)[i];
			gfx+=2424;			
		}					
	}
	
	void move_steps(int dx, int dy, int r0, int r1, bool update_map_coords=false)
	{
		if(dx<0) dx=-8/2; else if(dx>0) dx=8/2;
		if(dy<0) dy=-6; else if(dy>0) dy=6;		
		
		if(update_map_coords)
		{
			map_x+=2*dx;
			map_y+=dy;
		}
		
		short* src = (short*)FRONT;
		short* dst = (short*)BACK;
		
		for(int y=r0;y<r1;y++)
		{
			int srcy = y+dy;
			if(srcy<0 || srcy>=160)
			{
				display(map_x,map_y,y,y+1);
				/*for(int x=0;x<120;x++)
				{
					
				}*/
			}
			else
			{
				for(int x=0;x<120;x++)
				{			
					int srcx = x+dx;
					if(srcx<0 || srcx>=120)
					{
						short* gfx = (short*)((int)ROA_map_gfx+2424*(map_y+y)+map_x+2*x);
						dst[120*y+x] = gfx[0];
					}
					else
					{
						dst[120*y+x] = src[120*srcy+srcx];
					}
				}
			}
		}		
		
	}		
	
	void move_view(int dx, int dy)
	{
		
	}		
	
	int map_x=0, map_y=0;
public:
	virtual void init() override
	{			
		row = (char*)(new short[120]);
		
		Video::setMode(4);		
		Video::bgInit(2, Video::RotS256x256, Video::Pal8bit, 0, 0);
		
		map_x=264;
		map_y=208;
		display(map_x,map_y,0,160);		
		flip_page();
		
		dmaCopy(ROA_map_pal, BG_PALETTE, ROA_map_pal_len);		

		init_buildings_gfx();
	}	
	
	int frame_cnt = 0;
	
	virtual void frame() override
	{		
		switch(frame_cnt)
		{
			case 0: 
				move_steps(mvx,mvy, 0,60, true);
				break;
			case 1:
				move_steps(mvx,mvy, 60,120);
				break;
			case 2:
				move_steps(mvx,mvy, 120, 160);
				break;
			case 3:	
			{
				mvx=mvy=0;
				flip_page();
				break;
			}
		}						
		frame_cnt++; frame_cnt&=3;			
	}

	int dx = 8;
	int dy = 6;
	
	int mvx = 0;
	int mvy = 0;
	
	void process_movement(int keys)
	{
		if(keys & KEY_DOWN) {			
			mvy=dy;
		}
		else if(keys & KEY_UP) {			
			mvy=-dy;
		}
		if(keys & KEY_LEFT) {			
			mvx=-dx;
		}
		else if(keys & KEY_RIGHT) {			
			mvx=dx;
		}	
	}
	
	int metaframe_keys_down = 0;
	int metaframe_keys_held = 0;
	
	virtual void on_key_down(int keys)
	{
		metaframe_keys_down |= keys;
		if(frame_cnt==0)
		{
			process_movement(metaframe_keys_down);
			metaframe_keys_down=0;
		}
	}
	
	virtual void on_key_held(int keys)
	{	
		metaframe_keys_held |= keys;
		if(frame_cnt==0)
		{
			process_movement(metaframe_keys_held);
			metaframe_keys_held=0;
		}
	}
	
	~MainScene() 
	{
		delete[] row;
	}
};

astralbrew_launch_with_splash(MainScene);

