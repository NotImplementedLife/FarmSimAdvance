#pragma once

extern short BLD_SMALL_PLOT   [];
extern short BLD_MEDIUM_PLOT  [];
extern short BLD_LARGE_PLOT   [];
extern short BLD_SAND_ROAD    [];
extern short BLD_CHICKEN_COOP [];

extern char COL_1x1_16[];
extern char COL_2x2_32[];
extern char COL_3x3_48[];
extern char COL_4x4_80[];


void init_buildings_gfx();


class Building
{
private:
	const short* res_gfx;
	int px_width, px_height;
	int rows_count, cols_count;	
	const char* collision_matrix;
public:
	Building(const short* res_gfx, int px_width, int px_height, int rows_count, int cols_count, const char* collision_matrix);
	Building(const short* res_gfx); // and let the code choose the right settings
	
	const short* get_res_gfx() const;
	
	int get_px_width() const;
	int get_px_height() const;
	
	int get_rows_count() const;
	int get_cols_count() const;
	
	const char* get_collision_matrix() const;
	
	#define BLD_VALID (0<<0)
	#define BLD_INVALID (1<<0)
	#define BLD_BITMAP (0<<1)
	#define BLD_TILES (1<<1)	
		
	void copy_gfx(int src_x, int src_y, int src_width, int src_height, void* dest, int dest_stride, int dest_x, int dest_y, int flags = BLD_BITMAP | BLD_VALID) const;
	
	bool can_be_placed_on(const char* map, int stride, int row, int col) const;	
	
	virtual ~Building() = default;
};


#include <Astralbrew>

class BuildingSprite : public Astralbrew::Entity::Sprite
{
private:	
	Astralbrew::Memory::Address vram_addr;
	const Building* building;
	Sprite* auxiliary[3] = {nullptr, nullptr, nullptr};
	Astralbrew::Memory::Address aux_vram[3];	
	bool invalid_placement = false;
	void draw_vram() const;
	void clear_vram(void* addr) const;
public:
	BuildingSprite(const Building* building);
	
	void update(const Astralbrew::World::Camera* camera = nullptr);
	
	void set_placement_validity(bool valid);
	
	const Building* get_building() const;
	
	int px_width() const;
	int px_height() const;
	
	~BuildingSprite();
};