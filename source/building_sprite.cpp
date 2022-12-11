#include "building.hpp"

using namespace Astralbrew::Entity;
using namespace Astralbrew::Objects;

BuildingSprite::BuildingSprite(const Building* building) : Sprite(ObjSize::SIZE_64x64, ObjBitDepth::_8bit, 1)
{
	int w = building->get_px_width();
	int h = building->get_px_height();
		
	short* vram = (short*)0x06014000;
	vram_addr.set_value(vram);
	
	get_attribute()->set_priority(0);
	set_position(120,80);
	
	if(w<=64 && h<=64)
	{						
		set_anchor(128,128);
		
		get_visual()->set_frame(0, &vram_addr);
		get_visual()->set_crt_gfx(0);		
		
		building->copy_gfx(0, 0, w, h, vram, 64, (64-w)/2, (64-h)/2, true);
	}		
}


BuildingSprite::~BuildingSprite()
{
	for(int i=0;i<3;i++)
		delete auxiliary[i];
	
}