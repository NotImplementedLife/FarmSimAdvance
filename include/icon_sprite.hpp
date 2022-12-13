#pragma once

#include <Astralbrew>

class IconSprite : public Astralbrew::Entity::Sprite
{
private:
	Astralbrew::Memory::Address vram_addr;
	int icon_id;
public:
	IconSprite(int icon_id);
	
	static void init_gfx();	
};