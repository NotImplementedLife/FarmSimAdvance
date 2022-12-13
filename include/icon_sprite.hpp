#pragma once

#include <Astralbrew>

class IconSprite : public Astralbrew::Entity::Sprite
{
private:
	Astralbrew::Memory::Address vram_addr;
	int icon_id;
public:
	IconSprite(int icon_id);
	
	static void init_gfx(int menu_type = MENU_BUILDING);	
	
	int get_icon_id() const;
	
	inline static constexpr int MENU_BUILDING = 0;
	
	inline static constexpr int u = 16;

	~IconSprite() = default;
};