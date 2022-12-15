#pragma once

#include <Astralbrew>

class Cursor : public Astralbrew::Entity::Sprite
{
private:
	Astralbrew::Memory::Address vram_addr;
public:
	Cursor();
	
	~Cursor() = default;
	
	static void init_gfx(int mode);
};