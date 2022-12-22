#include "digit_sprite.hpp"

using namespace Astralbrew::Objects;
using namespace Astralbrew::Entity;

DigitSprite::DigitSprite(int digit, int pal) : Sprite(ObjSize::SIZE_8x8, ObjBitDepth::_4bit, 1)
{
	get_attribute()->set_priority(0);
	get_attribute()->set_palette_number(pal);
	
	vram_addr.set_value((void*)(0x06017C80 + 32*digit));
	
	this->get_visual()->set_frame(0, &vram_addr);
	this->get_visual()->set_crt_gfx(0);		
	
	set_anchor(128,128);	
	
	update_visual();
	update_position(nullptr);
}

#include "digits.h"

void DigitSprite::init_gfx()
{
	const short* src = ROA_digits4;
	short* dst = (short*)0x06017C80;
	for(int i=0;i<ROA_digits4_len;i++)
		*(dst++)=*(src++);
	
	dmaCopy(ROA_digits4pal, &SPRITE_PALETTE[0xC0], ROA_digits4pal_len-2);
	dmaCopy(ROA_digits4pal, &SPRITE_PALETTE[0xF0], ROA_digits4pal_len-2);
}