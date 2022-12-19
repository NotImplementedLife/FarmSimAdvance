#include "cursor_sprite.hpp"

using namespace Astralbrew;
using namespace Astralbrew::Entity;
using namespace Astralbrew::Objects;

#include "cursor.h"

Cursor::Cursor() : Sprite(ObjSize::SIZE_16x16, ObjBitDepth::_4bit, 1)
{
	get_attribute()->set_palette_number(0xE);
	
	vram_addr.set_value((void*)(0x06017C00));
	
	this->get_visual()->set_frame(0, &vram_addr);
	this->get_visual()->set_crt_gfx(0);		
	
	set_anchor(128,128);
	
	get_attribute()->set_priority(0);		
	
	dmaCopy(ROA_icons4pal, &SPRITE_PALETTE[0xE0], ROA_icons4pal_len);
	
	set_carrot();
}

void Cursor::init_gfx(int mode)
{
	const short* src = ROA_icons4 + mode * 64;
	short* dst = (short*)0x06017C00;
	for(int i=0;i<64;i++)
		*(dst++)=*(src++);
}

void Cursor::set_hand()
{
	init_gfx(0);
	set_position(124,120);
}
	
void Cursor::set_carrot()
{
	init_gfx(1);
	set_position(128,88);
}

void Cursor::hide()
{
	get_attribute()->hide();
}

void Cursor::show()
{
	get_attribute()->show();
}