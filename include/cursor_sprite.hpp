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
	
	void set_hand();
	
	void set_carrot();
	
	void hide();
	
	void show();
};