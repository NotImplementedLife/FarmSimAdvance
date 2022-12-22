#include <Astralbrew>

#include "map_title.h"
#include <stdlib.h>

using namespace Astralbrew::World;
using namespace Astralbrew;
using namespace Astralbrew::Objects;
using namespace Astralbrew::Entity;

#define LCD_FRAME_SELECT (1 << 4)

#include "border_48_73_bin.h"
#include "logo.h"

#include "metamap.hpp"

#include "main.hpp"

class TitleScene : public Scene
{	
private:			
	char* row = (char*)(new short[120]);
	
	Camera camera = Camera(0,0);
	
	inline static constexpr int cam_left = 300;
	inline static constexpr int cam_top = 250;
	inline static constexpr int cam_width = 1000;
	inline static constexpr int cam_height = 420;	

	void display(int x0, int y0, int r0, int r1)
	{						
		char* gfx = (char*)ROA_map_title_gfx+1728*(y0+r0)+x0;
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
			gfx+=1728;			
		}							
	}	
	
	
	inline static constexpr int dirx[] = { -1,  0,   1, 1, 1, 0, -1, -1};
	inline static constexpr int diry[] = { -1, -1,  -1, 0, 1, 1,  1, 0};
	int dirk = 0;
	
	static bool in_range(int x, int a, int b) { return a<=x && x<b; }
	
	bool dx0 = false;
	bool dy0 = false;
	
	void move_steps(int dx, int dy, int r0, int r1, bool update_map_coords=false)
	{
		if(dx<0) dx=-6/2; else if(dx>0) dx=6/2;
		if(dy<0) dy=-4; else if(dy>0) dy=4;		
		
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
			else dy = 0, dy0 = true;
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
						short* gfx = (short*)((int)ROA_map_title_gfx+1728*(map_y+y)+map_x+2*x);
						dst[120*y+x] = gfx[0];						
					}
					else
					{
						dst[120*y+x] = src[120*srcy+srcx];
					}
				}
			}
		}
		
		if(dy<0 && r0==0)
		{
			metamap.draw_buildings(map_x-cam_left, map_y-cam_top, 240, -dy, (short*)BACK, 240);
		}
		else if(dy>0 && r1==160)
		{
			metamap.draw_buildings(map_x-cam_left, map_y-cam_top+160-dy, 240, dy, ((short*)BACK)+120*(160-dy), 240);
		}
		
		if(dx<0)
		{
			metamap.draw_buildings(map_x-cam_left, map_y-cam_top+r0, -2*dx, r1-r0, ((short*)BACK)+120*r0, 240);
		}
		else if(dx>0)
		{
			metamap.draw_buildings(map_x-cam_left+240-2*dx, map_y-cam_top+r0, 2*dx, r1-r0, ((short*)BACK)+120*r0+120-dx, 240);
		}
	}	
	
	int map_x=0, map_y=0;
public:	
	
	virtual void init() override
	{									
		
		Video::setMode(4);				
		Video::bgInit(2, Video::RotS256x256, Video::Pal8bit, 0, 0);
		Video::bgSetPriority(2,3);
		
		Video::objEnable1D();
		
		camera.set_x(500);
		camera.set_y(300);
		
		map_x = cam_left-120+camera.get_x();
		map_y = cam_top -80 +camera.get_y();
		display(map_x,map_y,0,160);		
		
		flip_page();
		
		dmaCopy(ROA_map_title_pal, BG_PALETTE, ROA_map_title_pal_len);
		
		dmaCopy(ROA_logo4, (void*)(0x06014000), ROA_logo4_len);
		
		dmaCopy(ROA_logo4pal, SPRITE_PALETTE, ROA_logo4pal_len);
		
		logo1.get_attribute()->set_priority(0);
		logo1.get_attribute()->set_palette_number(0);
		
		vram1.set_value((void*)(0x06014000));
		
		logo1.get_visual()->set_frame(0, &vram1);
		logo1.get_visual()->set_crt_gfx(0);		
		
		logo1.set_anchor(255,128);	
		logo1.set_position(119,80);
		
		logo1.update_visual();
		logo1.update_position(nullptr);
		
		logo2.get_attribute()->set_priority(0);
		logo2.get_attribute()->set_palette_number(0);
		
		vram2.set_value((void*)(0x06014800));
		
		logo2.get_visual()->set_frame(0, &vram2);
		logo2.get_visual()->set_crt_gfx(0);		
		
		logo2.set_anchor(0,128);	
		logo2.set_position(120,80);
		
		logo2.update_visual();
		logo2.update_position(nullptr);
	}	
	
	Memory::Address vram1;
	Memory::Address vram2;
	
	Sprite logo1 = Sprite(ObjSize::SIZE_64x64, ObjBitDepth::_4bit, 1);
	Sprite logo2 = Sprite(ObjSize::SIZE_64x64, ObjBitDepth::_4bit, 1);
		
	
	Metamap metamap;
	
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
				
				mvx = dirx[dirk];
				mvy = diry[dirk];
				
				if(rand()%30==0) 
				{
					dirk++;
					dirk%=9;
				}
				
				flip_page();				
								
				OamPool::deploy();
				break;
			}
		}						
		frame_cnt++; frame_cnt&=3;		
	}	
	
	int mvx = 0;
	int mvy = 0;
	
	bool process_movement(int keys)
	{
		if(keys & KEY_DOWN) {			
			mvy=1;
			mvx=1; 			
			return true;
		}
		else if(keys & KEY_UP) {			
			mvy=-1;
			mvx=-1;		
			return true;
		}
		if(keys & KEY_LEFT) {						
			mvy=1;
			mvx=-1;		
			return true;
		}
		else if(keys & KEY_RIGHT) {			
			mvx=1;
			mvy=-1;		
			return true;
		}			
		return false;
	}	

	void process_keys_down(int keys)
	{		
		if(process_movement(keys))
			return;
		
		if(keys & KEY_START)
		{
			close()->next(new MainScene());
		}
	}
	
	void process_keys_held(int keys)
	{		
		if(process_movement(keys))
			return;				
	}
	
	int metaframe_keys_down = 0;
	int metaframe_keys_held = 0;
	
	virtual void on_key_down(int keys)
	{
		metaframe_keys_down |= keys;
		if(frame_cnt==0)
		{
			process_keys_down(metaframe_keys_down);
			metaframe_keys_down=0;
		}		
	}
	
	virtual void on_key_held(int keys)
	{	
		metaframe_keys_held |= keys;
		if(frame_cnt==0)
		{
			process_keys_held(metaframe_keys_held);
			metaframe_keys_held=0;
		}
	}
	
	~TitleScene() 
	{
		delete[] row;				
		
		const int zero = 0;
		for(short* buff = (short*)0x06000000; buff!=(short*)0x06018000;buff++)
		{
			__asm("STRH %[_0], [%[dest]]"				
				:
				: [dest] "r" (buff), [_0] "r" (zero));     
		}
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

astralbrew_launch_with_splash(TitleScene);