#include "icon_sprite.hpp"

#include "buildingsicn4.h"

using namespace Astralbrew::Entity;
using namespace Astralbrew::Objects;

IconSprite::IconSprite(int icon_id) : Sprite(ObjSize::SIZE_32x32, ObjBitDepth::_4bit, 1)
{
	get_attribute()->set_palette_number(0xC);	
	this->icon_id = icon_id;
	vram_addr.set_value((void*)(0x06017400 + 32*32/2 * icon_id));
	
	this->get_visual()->set_frame(0, &vram_addr);
	this->get_visual()->set_crt_gfx(0);		
	
	set_anchor(0,128);
	
	get_attribute()->set_priority(0);	
	set_position(40*icon_id+8, 120);	
}
	
void IconSprite::init_gfx()
{
	dmaCopy(ROA_buildingsicn4pal, &SPRITE_PALETTE[0xC0], ROA_buildingsicn4pal_len);	
	dmaCopy(ROA_buildingsicn4, (void*)0x06017400, ROA_buildingsicn4_len);	
}