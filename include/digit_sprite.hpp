#pragma once

#include <Astralbrew>

class DigitSprite : public Astralbrew::Entity::Sprite
{
private:
	Astralbrew::Memory::Address vram_addr;
public:
	DigitSprite(int digit, int pal);
	
	void set_digit(int digit);
	static void init_gfx();
	~DigitSprite() = default;
};