#include "icon_sprite.hpp"

#include "buildingsicn4.h"

using namespace Astralbrew::Entity;
using namespace Astralbrew::Objects;

IconSprite::IconSprite(int icon_id) : Sprite(ObjSize::SIZE_32x32, ObjBitDepth::_4bit, 1)
{
	get_attribute()->set_palette_number(0xD);	
	this->icon_id = icon_id;
	vram_addr.set_value((void*)(0x06017400 + 32*32/2 * icon_id));
	
	this->get_visual()->set_frame(0, &vram_addr);
	this->get_visual()->set_crt_gfx(0);		
	
	set_anchor(128,128);
	
	get_attribute()->set_priority(0);	
	set_position(120 + 40*icon_id,140);		
}
	
void IconSprite::init_gfx(int menu_type)
{
	if(menu_type == MENU_BUILDING)
	{
		dmaCopy(ROA_buildingsicn4pal, &SPRITE_PALETTE[0xD0], ROA_buildingsicn4pal_len);	
		dmaCopy(ROA_buildingsicn4, (void*)0x06017400, ROA_buildingsicn4_len);	
	}
}

int IconSprite::get_icon_id() const { return icon_id; }