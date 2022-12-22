#include <Astralbrew>

#include "map.h"
#include <stdio.h>

#include "building.hpp"
#include "icon_sprite.hpp"
#include "cursor_sprite.hpp"
#include "digit_sprite.hpp"

using namespace Astralbrew::World;
using namespace Astralbrew;
using namespace Astralbrew::Objects;

#define LCD_FRAME_SELECT (1 << 4)

#include "border_48_73_bin.h"

#include "metamap.hpp"

#include "timer.hpp"

class MainScene : public Scene
{	
private:			
	char* row = (char*)(new short[120]);
	
	Camera camera = Camera(0,0);
	
	inline static constexpr int cam_left = 264;
	inline static constexpr int cam_top = 208;
	inline static constexpr int cam_width = 1136;
	inline static constexpr int cam_height = 512;

	Cursor* cursor;
	
	Farm farm;
	TimerProcessor timer_processor = TimerProcessor(&farm);	

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
		init_buildings_gfx();			
		
		Video::setMode(4);				
		Video::bgInit(2, Video::RotS256x256, Video::Pal8bit, 0, 0);
		Video::bgSetPriority(2,3);
		
		Video::objEnable1D();
		
		camera.set_x(336);
		camera.set_y(156);
		
		map_x = cam_left-120+camera.get_x();
		map_y = cam_top -80 +camera.get_y();
		display(map_x,map_y,0,160);		
		
		flip_page();
		
		dmaCopy(ROA_map_pal, BG_PALETTE, ROA_map_pal_len);			
		dmaCopy(ROA_map_pal, SPRITE_PALETTE, ROA_map_pal_len);
		SPRITE_PALETTE[0xCF] = Drawing::Colors::Red;
		
		IconSprite::init_gfx();			
		DigitSprite::init_gfx();
		
		cursor = new Cursor();			

		farm.set_metamap(&metamap);	
		
		wheat_icon->set_position(236,6);
		wheat_icon->update_position(nullptr);
		
		egg_icon->set_position(236,16);
		egg_icon->update_position(nullptr);
		
		for(int i=0;i<7;i++)
		{
			wheat_no[i] = new DigitSprite(0, 0xC);
			wheat_no[i]->set_position(228-6*i, 6);
			wheat_no[i]->update_position(nullptr);
			
			eggs_no[i] = new DigitSprite(0, 0xC);
			eggs_no[i]->set_position(228-6*i, 16);
			eggs_no[i]->update_position(nullptr);
		}			
	}	
	
	DigitSprite* wheat_icon = new DigitSprite(10, 0xC);
	DigitSprite* egg_icon = new DigitSprite(11, 0xF);
	
	DigitSprite* wheat_no[7];
	DigitSprite* eggs_no[7];
	
	
	int frame_cnt = 0;	
	
	BuildingSprite* building_sprite = nullptr;	
	
	Metamap metamap;
	
	int building_place_r;
	int building_place_c;
	
	void adjust_building_sprite_pos()
	{		
		assert(building_sprite!=nullptr);
		int bx = map_x + 120 - building_sprite->px_width()/2;
		int by = map_y + 80 - building_sprite->px_height()/2;
				
		int r = (32 * by + 64 * bx / 3 - 256) >> 9;
        int c = (32 * by - 64 * bx / 3 + 256) >> 9;
		
		int rr = r+c;
		int cc = (r-c)/2;		
				
		
		bx = cc*24 + (rr&1)*12;
		by = (rr/2)*16 + (rr&1)*8;
		
		bx = bx - map_x + building_sprite->px_width()/2;		
		by = by - map_y + building_sprite->px_height()/2;				
	
		building_place_r = rr-26;
		building_place_c = cc-11;
		
		building_sprite->set_position(bx,by);		
		bool valid = metamap.can_place(building_sprite->get_building(), building_place_r, building_place_c);		
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
				if(draw_building_scheduled)
				{
					if(draw_refresh)
					{
						display(map_x, map_y, 0, 160);
						metamap.draw_buildings(map_x-cam_left, map_y-cam_top, 240, 160, (short*)BACK, 240);
						draw_refresh = false;
					}
					else
					{
						metamap.draw_buildings(draw_building_x, draw_building_y, draw_building_w, draw_building_h, (short*)BACK, 240);
					}
					draw_building_scheduled = false;
				}
				break;
			case 3:	
			{				
				mvx=mvy=0;
				flip_page();
				
				if(mode==MODE_SELECT)
				{
					int timer_result = timer_processor.update();
					if(timer_result & TimerFlags::FORCE_REDRAW)
					{
						draw_refresh = true;
						draw_building_scheduled = true;
					}
				}
				
				if(building_sprite!=nullptr) 
				{
					adjust_building_sprite_pos();
					building_sprite->update();				
				}								
				for(int i=0;i<4;i++)
				{
					if(icons[i])
					{
						icons[i]->update_visual();
						icons[i]->update_position(nullptr);
					}
				}
				
				cursor->update_visual();
				cursor->update_position(nullptr);
				
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
	
	inline static constexpr int ACTION_FREE_PLOT = 1<<8;
	inline static constexpr int ACTION_CROPS_GROWING = 2<<8;
	inline static constexpr int ACTION_CHICKEN_FEED = 3<<8;
	inline static constexpr int ACTION_REMOVE_ONLY = 4<<8;
	
	Building*  sel_building = nullptr;
	
	void process_keys_down(int keys)
	{
		switch(mode)
		{
			case MODE_SELECT:
				if(process_movement(keys))
					break;
				else if(keys & KEY_L)
				{
					launch_menu(IconSprite::MENU_BUILDING);					
				}
				else if(keys & KEY_A)
				{
					int x = map_x - cam_left + 120;
					int y = map_y - cam_top + 80;
					sel_building = metamap.building_at(x,y);
					if(sel_building != nullptr)
					{									
						if(sel_building->is_empty_plot())
						{							
							launch_menu(IconSprite::MENU_ACTIONS | ACTION_FREE_PLOT);
						}
						else if(sel_building->is_crops_growing())
						{
							if(!sel_building->is_watered())
							{
								launch_menu(IconSprite::MENU_ACTIONS | ACTION_CROPS_GROWING);
							}
							else
							{
								launch_menu(IconSprite::MENU_ACTIONS | ACTION_REMOVE_ONLY);
							}
						}
						else if(sel_building->is_crops_ready())
						{
							metamap.update(sel_building);	// return to empty plot
							draw_refresh = true;
							draw_building_scheduled = true;			
							sel_building->set_watered(false);
						}
						else if(sel_building->is_chicken_coop())
						{
							if(!sel_building->is_feed())
							{
								launch_menu(IconSprite::MENU_ACTIONS | ACTION_CHICKEN_FEED);
							}
							else
							{
								launch_menu(IconSprite::MENU_ACTIONS | ACTION_REMOVE_ONLY);
							}
						}
						else if(sel_building->is_chicken_coop_ready())
						{
							metamap.update(sel_building);
							draw_refresh = true;
							draw_building_scheduled = true;			
							sel_building->set_feed(false);
						}
					}
				}
				break;
			case MODE_PLACE_BUILDING:
				if(process_movement(keys))
					break;
				else if(keys & KEY_A)
				{
					if(!(metaframe_keys_held & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)))
						building_place_confirm();
				}
				else if(keys & KEY_B)
				{
					building_place_cancel();
				}
				break;
			case MODE_MENU:
				if(keys & KEY_RIGHT)
				{
					menu_right();
				}
				else if(keys & KEY_LEFT)
				{
					menu_left();
				}
				else if(keys & KEY_B)
				{
					menu_cancel();
				}
				else if(keys & KEY_A)
				{
					menu_select();
				}
				break;
		}		
	}
	
	void process_keys_held(int keys)
	{
		switch(mode)
		{
			case MODE_SELECT:
				if(process_movement(keys))
					break;
			case MODE_PLACE_BUILDING:
				if(process_movement(keys))
					break;
		}		
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
	
	~MainScene() 
	{
		delete[] row;		
		
		delete wheat_icon;
		delete egg_icon;
	}
	
private:	
	IconSprite* icons[4] = {nullptr, nullptr, nullptr, nullptr};
	
	int crt_icon_id = 0;
	
	int menu_type;

	void launch_menu(int menu_type)
	{
		this->menu_type = menu_type;
		IconSprite::init_gfx(menu_type & 0xFF);		
		if((menu_type & 0xFF) == IconSprite::MENU_ACTIONS)
		{			
			int k=0;
			switch(menu_type & 0xFF00)
			{
				case ACTION_FREE_PLOT:
					icons[k++] = new IconSprite(0);			
					icons[k++] = new IconSprite(3);
					icons[k-1]->set_position(icons[k-1]->pos_x()-k*40,icons[k-1]->pos_y());					
					break;
				case ACTION_CROPS_GROWING:
					icons[k++] = new IconSprite(1);
					icons[k-1]->set_position(icons[k-1]->pos_x()-40,icons[k-1]->pos_y());
					icons[k++] = new IconSprite(3);
					icons[k-1]->set_position(icons[k-1]->pos_x()-k*40,icons[k-1]->pos_y());					
					break;
				case ACTION_CHICKEN_FEED:
					icons[k++] = new IconSprite(2);
					icons[k-1]->set_position(icons[k-1]->pos_x()-80,icons[k-1]->pos_y());
					icons[k++] = new IconSprite(3);
					icons[k-1]->set_position(icons[k-1]->pos_x()-k*40,icons[k-1]->pos_y());					
					break;
				case ACTION_REMOVE_ONLY:
					icons[k++] = new IconSprite(3);
					icons[k-1]->set_position(icons[k-1]->pos_x()-120,icons[k-1]->pos_y());			
					break;
			}			
		}
		else
		{
			for(int i=0;i<4;i++)
			{
				icons[i] = new IconSprite(i);
			}
		}
		crt_icon_id = 0;
		schedule_task(&menu_move_task);
		
		cursor->set_hand();
		set_mode(MODE_MENU);
	}
	
	void menu_left()
	{
		if(crt_icon_id==0) return;
		if(icons[crt_icon_id-1]==nullptr) return;
		crt_icon_id--;
		menu_move_task.to_right();
	}
	
	void menu_right()
	{
		if(crt_icon_id==3) return;
		if(icons[crt_icon_id+1]==nullptr) return;
		crt_icon_id++;
		menu_move_task.to_left();
	}
	
	void menu_cancel()
	{
		unschedule_task(&menu_move_task);
		menu_move_task.set_counter(-1);
				
		for(int i=0;i<4;i++)
		{
			delete icons[i];
			icons[i] = nullptr;
		}		
		
		cursor->set_carrot();
		set_mode(MODE_SELECT);
	}
	
	inline static constexpr short* menu_opt_bld[4] = { BLD_SMALL_PLOT, BLD_MEDIUM_PLOT, BLD_LARGE_PLOT, BLD_CHICKEN_COOP };
	//inline static constexpr short* menu_opt_bld[4] = { BLD_LARGE_WHEAT_0, BLD_LARGE_WHEAT_1, BLD_LARGE_WHEAT_2, BLD_LARGE_WHEAT_3 };
	
	void menu_select()
	{
		if((menu_type & 0xFF) == IconSprite::MENU_BUILDING)
		{			
			menu_cancel();
			building_place_start(new Building(menu_opt_bld[crt_icon_id]));
		}
		if((menu_type & 0xFF) == IconSprite::MENU_ACTIONS)
		{
			int icon_id = icons[crt_icon_id]->get_icon_id();
			menu_cancel();
			switch(icon_id)
			{
				case 3: // REMOVE
					metamap.remove(sel_building);		
					timer_processor.remove_building(sel_building);
					// force redraw				
					draw_refresh = true;
					draw_building_scheduled = true;							
					
					break;
				case 0: // PLANT SEEDS				
					if(sel_building->is_empty_plot())
					{
						metamap.update(sel_building);					
						draw_refresh = true;
						draw_building_scheduled = true;		
						timer_processor.add_timer(new CropsTimer(sel_building));	
					}
					break;
				case 1: // WATER PLANTS
					if(sel_building->is_crops_growing())
					{						
						sel_building->set_watered();
					}
					break;
				case 2:
					if(sel_building->is_chicken_coop())
					{						
						sel_building->set_feed();
						timer_processor.add_timer(new ChickenTimer(sel_building));
					}
					break;
				default:
					FATAL_ERROR(sf24(icon_id).to_string());
			}
		}
	}
private:
	void building_place_start(Building* building)
	{
		building_sprite = new BuildingSprite(building);
		building_sprite->set_position(120,80);
		set_mode(MODE_PLACE_BUILDING);
	}
	
	void building_place_confirm()
	{
		if(!building_sprite->is_valid_placed()) 
			return;
		
		Building* building = building_sprite->get_building();
		
		metamap.place(building, building_place_r, building_place_c);
						
		draw_building_x = map_x-cam_left;
		draw_building_y = map_y-cam_top;
		draw_building_w = 240;
		draw_building_h = 160;
		draw_refresh = false;
		draw_building_scheduled = true;		
				
		building_place_cancel(true);
		building_place_start(new Building(building));
	}
	
	bool draw_building_scheduled = false;
	bool draw_refresh = false;
	int draw_building_x;
	int draw_building_y;
	int draw_building_w;
	int draw_building_h;	
	
	void building_place_cancel(bool success = false)
	{
		if(!success)
		{
			delete building_sprite->get_building();
		}
		delete building_sprite;
		building_sprite = nullptr;		
		set_mode(MODE_SELECT);
	}
	
private:
	
	
private:
	int mode = MODE_SELECT;
	
	inline static constexpr int MODE_SELECT = 0;
	inline static constexpr int MODE_PLACE_BUILDING = 1;
	inline static constexpr int MODE_MENU = 2;
	
	int bak_mode;
	void suspend_mode()
	{
		bak_mode = mode;
		mode = -1;
	}
	
	void restore_mode()
	{
		mode = bak_mode;
	}
	
	
	void set_mode(int _mode)
	{
		mode = _mode;
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
private:

	class MenuOptionMoveTask : public ScheduledTask
	{		
	public:
		MainScene* scene = nullptr;
		int direction = 1;
		MenuOptionMoveTask(MainScene* scene) : ScheduledTask(1,-1), scene(scene) { }		
		
		void to_left() { direction = -2; set_counter(20); scene->suspend_mode(); }
		void to_right() { direction = 2; set_counter(20); scene->suspend_mode(); }
		
		void action() override
		{
			for(int i=0;i<4;i++)
			{
				if(scene->icons[i])
					scene->icons[i]->set_position(scene->icons[i]->pos_x()+direction, scene->icons[i]->pos_y());
			}
			if(get_counter()==1) 
			{
				scene->restore_mode();				
			}
		}
				
	};
	
	MenuOptionMoveTask menu_move_task = MenuOptionMoveTask(this);
};

astralbrew_launch_with_splash(MainScene);

