#include <Astralbrew>

#include "map.h"
#include <stdio.h>

#include "building.hpp"

using namespace Astralbrew::World;
using namespace Astralbrew;
using namespace Astralbrew::Objects;

#define LCD_FRAME_SELECT (1 << 4)

#include "border_48_73_bin.h"

class MainScene : public Scene
{	
private:			
	char* row = (char*)(new short[120]);
	
	Camera camera = Camera(0,0);
	
	inline static constexpr int cam_left = 264;
	inline static constexpr int cam_top = 208;
	inline static constexpr int cam_width = 1136;
	inline static constexpr int cam_height = 512;	
	

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
	
	static bool in_range(int x, int a, int b) { return a<=x && x<b; }
	
	bool dx0 = false;
	bool dy0 = false;
	
	void move_steps(int dx, int dy, int r0, int r1, bool update_map_coords=false)
	{
		if(dx<0) dx=-8/2; else if(dx>0) dx=8/2;
		if(dy<0) dy=-6; else if(dy>0) dy=6;		
		
		if(update_map_coords)
		{			
			if(in_range(camera.get_x()+2*dx, 0, cam_width))
			{				
				camera.move(2*dx, 0);
				map_x+=2*dx;
				dx0 = false;
			}
			else dx = 0, dx0 = true;
			if(in_range(camera.get_y()+dy,0,cam_height))
			{
				camera.move(0, dy);
				map_y+=dy;
				dy0 = false;
			}
			else dy = 0, dy0 = true;;
		}
		else
		{
			if(dx0)
				dx = 0;
			if(dy0)
				dy = 0;			
		}
		
		short* src = (short*)FRONT;
		short* dst = (short*)BACK;
		
		for(int y=r0;y<r1;y++)
		{
			int srcy = y+dy;
			if(srcy<0 || srcy>=160)
			{
				display(map_x,map_y,y,y+1);
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
	
	int map_x=0, map_y=0;
public:
	virtual void init() override
	{					
		init_buildings_gfx();	
		init_map_metadata();
		
		Video::setMode(4);				
		Video::bgInit(2, Video::RotS256x256, Video::Pal8bit, 0, 0);
		Video::bgSetPriority(2,3);
		
		Video::objEnable1D();
		
		map_x = cam_left-120+camera.get_x();
		map_y = cam_top -80 +camera.get_y();
		display(map_x,map_y,0,160);		

		
		building_sprite = new BuildingSprite(new Building(BLD_CHICKEN_COOP));
		building_sprite->set_position(120,80);
		flip_page();
		
		dmaCopy(ROA_map_pal, BG_PALETTE, ROA_map_pal_len);			
		dmaCopy(ROA_map_pal, SPRITE_PALETTE, ROA_map_pal_len);		
		SPRITE_PALETTE[0xBF] = Drawing::Colors::Red;
	}	
	
	int frame_cnt = 0;
	
	
	
	BuildingSprite* building_sprite = nullptr;	
	
	void adjust_building_sprite_pos()
	{		
		assert(building_sprite!=nullptr);
		int bx = map_x + 120 - building_sprite->px_width()/2;
		int by = map_y + 80 - building_sprite->px_height()/2;
		
		int r = (by/8+bx/12)/2;
		int c = (by/8-bx/12)/2;
		
		int rr = r+c;
		int cc = (r-c)/2;		
		
		// FATAL_ERROR(sf24(cc).to_string());
		
		bx = cc*24 + (rr&1)*12;
		by = (rr/2)*16 + (rr&1)*8;
		
		bx = bx - map_x + building_sprite->px_width()/2;		
		by = by - map_y + building_sprite->px_height()/2;				
		
		building_sprite->set_position(bx,by);
		
		bool valid = building_sprite->get_building()->can_be_placed_on(map_metadata, 48, rr-26, cc-11);
		building_sprite->set_placement_validity(valid);
	}
	
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
				
				if(building_sprite!=nullptr) 
				{
					adjust_building_sprite_pos();
					building_sprite->update();				
				}				
				OamPool::deploy();
				break;
			}
		}						
		frame_cnt++; frame_cnt&=3;		

		
	}	
	
	int mvx = 0;
	int mvy = 0;
	
	void process_movement(int keys)
	{
		if(keys & KEY_DOWN) {			
			mvy=1;
			mvx=1; 			
		}
		else if(keys & KEY_UP) {			
			mvy=-1;
			mvx=-1;			
		}
		if(keys & KEY_LEFT) {						
			mvy=1;
			mvx=-1;			
		}
		else if(keys & KEY_RIGHT) {			
			mvx=1;
			mvy=-1;			
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
		delete[] map_metadata;
	}
	
private:
	void drawBuilding(const Building* building, int x, int y)
	{				
		building->copy_gfx(0,0,building->get_px_width(), building->get_px_height(), BACK, 240, x, y);
	}
	
private:
	char* map_metadata = new char[73*48];

	void init_map_metadata()
	{
		int* src = (int*)border_48_73_bin;
		int* dst = (int*)map_metadata;
		for(int i=0;i<73*48/4;i++) *(dst++)=*(src++);
	}
	
	
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
};

astralbrew_launch_with_splash(MainScene);

