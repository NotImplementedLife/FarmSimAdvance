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
public:
	
	
	virtual ~Building() = default;
	
	
};