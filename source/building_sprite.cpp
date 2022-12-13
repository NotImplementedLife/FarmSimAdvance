#include "building.hpp"

using namespace Astralbrew::Entity;
using namespace Astralbrew::Objects;

BuildingSprite::BuildingSprite(const Building* building) : Sprite(ObjSize::SIZE_64x64, ObjBitDepth::_8bit, 1)
{
	this->building=building;
	int w = building->get_px_width();
	int h = building->get_px_height();
		
	short* vram = (short*)0x06014000;
	vram_addr.set_value(vram);
	
	get_attribute()->set_priority(0);
	set_position(120,80);	
	
	if(w<=64 && h<=64)
	{						
		this->set_anchor(128,128);
		
		this->get_visual()->set_frame(0, &vram_addr);
		this->get_visual()->set_crt_gfx(0);		
				
		building->copy_gfx(0, 0, w, h, vram, 64, (64-w)/2, (64-h)/2, BLD_TILES | invalid_placement);
	}
	else if(w<=128 && h<=64)
	{
		this->set_anchor(0,128);
		this->get_visual()->set_frame(0, &vram_addr);
		this->get_visual()->set_crt_gfx(0);		
		
		building->copy_gfx(w/2, 0, w/2, h, vram, 64, 0, (64-h)/2, BLD_TILES | invalid_placement);
		
		auxiliary[0] = new Sprite(ObjSize::SIZE_64x64, ObjBitDepth::_8bit, 1);
		aux_vram[0].set_value(vram+64*64/2);
		auxiliary[0]->set_anchor(255,128);
		auxiliary[0]->get_visual()->set_frame(0, &aux_vram[0]);
		auxiliary[0]->get_visual()->set_crt_gfx(0);		
		building->copy_gfx(0, 0, w/2, h, aux_vram[0].get_value(), 64, 64-w/2, (64-h)/2, BLD_TILES | invalid_placement);
	}		
	else if(w<=64 && h<=128) FATAL_ERROR("Vertical rect not supported");
	else if(w<=128 && h<=128)
	{
		int opth = ((128-h)/2)/8;		
		opth = 8*opth*32;
		
		this->set_anchor(0,0);
		this->get_visual()->set_frame(0, &vram_addr);
		this->get_visual()->set_crt_gfx(0);				
		building->copy_gfx(w/2, h/2, w/2, h/2, vram, 64, 0, 0, BLD_TILES | invalid_placement);		
		
		auxiliary[0] = new Sprite(ObjSize::SIZE_64x64, ObjBitDepth::_8bit, 1);
		aux_vram[0].set_value(vram + 64*64/sizeof(short) - opth);
		auxiliary[0]->set_anchor(0,255);
		auxiliary[0]->get_visual()->set_frame(0, &aux_vram[0]);
		auxiliary[0]->get_visual()->set_crt_gfx(0);		
		building->copy_gfx(w/2, 0, w/2, h/2, aux_vram[0].get_value(), 64, 0, 64-h/2, BLD_TILES | invalid_placement);				
		
		auxiliary[1] = new Sprite(ObjSize::SIZE_64x64, ObjBitDepth::_8bit, 1);
		aux_vram[1].set_value(vram+2*64*64/sizeof(short) - opth);
		auxiliary[1]->set_anchor(255,0);
		auxiliary[1]->get_visual()->set_frame(0, &aux_vram[1]);
		auxiliary[1]->get_visual()->set_crt_gfx(0);		
		building->copy_gfx(0, h/2, w/2, h/2, aux_vram[1].get_value(), 64, 64-w/2, 0, BLD_TILES | invalid_placement);
				
		
		auxiliary[2] = new Sprite(ObjSize::SIZE_64x64, ObjBitDepth::_8bit, 1);
		aux_vram[2].set_value(vram+3*64*64/sizeof(short) - 2*opth);
		auxiliary[2]->set_anchor(255,255);
		auxiliary[2]->get_visual()->set_frame(0, &aux_vram[2]);
		auxiliary[2]->get_visual()->set_crt_gfx(0);		
		building->copy_gfx(0, 0, w/2, h/2, aux_vram[2].get_value(), 64, 64-w/2, 64-h/2, BLD_TILES | invalid_placement);
				
	}
}

void BuildingSprite::clear_vram(void* addr) const
{	
	short* dest = (short*)addr;	
	for(int i=0;i<64*64/2;i++)
		asm volatile(
			"\n PUSH {r0, r1}"				
			"\n MOV r1, %0"
			"\n MOV r0, %1"
			"\n STRH r0, [r1]"
			"\n POP {r0, r1}"
				:
				: "r" (dest+i), "r" (0));
}

void BuildingSprite::draw_vram() const
{
	int w = building->get_px_width();
	int h = building->get_px_height();
	if(w<=64 && h<=64)
	{		
		building->copy_gfx(0, 0, w, h, vram_addr.get_value(), 64, (64-w)/2, (64-h)/2, BLD_TILES | invalid_placement);
	}
	else if(w<=128 && h<=64)
	{			
		building->copy_gfx(w/2, 0, w/2, h, vram_addr.get_value(), 64, 0, (64-h)/2, BLD_TILES | invalid_placement);		
		building->copy_gfx(0, 0, w/2, h, aux_vram[0].get_value(), 64, 64-w/2, (64-h)/2, BLD_TILES | invalid_placement);
	}
	else if(w<=128 && h<=128)
	{			
		building->copy_gfx(w/2, h/2, w/2, h/2, vram_addr.get_value(), 64, 0, 0, BLD_TILES | invalid_placement);
		building->copy_gfx(w/2, 0, w/2, h/2, aux_vram[0].get_value(), 64, 0, 64-h/2, BLD_TILES | invalid_placement);	
		building->copy_gfx(0, h/2, w/2, h/2, aux_vram[1].get_value(), 64, 64-w/2, 0, BLD_TILES | invalid_placement);
		building->copy_gfx(0, 0, w/2, h/2, aux_vram[2].get_value(), 64, 64-w/2, 64-h/2, BLD_TILES | invalid_placement);
	}
}

void BuildingSprite::set_placement_validity(bool valid)
{
	if(invalid_placement== !valid) return;
	invalid_placement = !valid;
	draw_vram();
}

const Building* BuildingSprite::get_building() const { return building;}

void BuildingSprite::update(const Astralbrew::World::Camera* camera)
{		
	update_visual();
	update_position(camera);		
		
	for(int i=0,dx,dy;i<3;i++)
	{
		if(auxiliary[i]==nullptr)		
			continue;	
		dx = dy = 0;
		if(this->get_anchor().x==0 && auxiliary[i]->get_anchor().x==255)
			dx = -1;
		if(this->get_anchor().y==0 && auxiliary[i]->get_anchor().y==255)
			dy = -1;
		auxiliary[i]->set_position(this->pos_x()+dx, this->pos_y()+dy);
		auxiliary[i]->update_position(camera);
		auxiliary[i]->update_visual();
	}
}

int BuildingSprite::px_width() const { return building->get_px_width(); }
int BuildingSprite::px_height() const { return building->get_px_height(); }

bool BuildingSprite::is_valid_placed() const { return !invalid_placement; }

void BuildingSprite::wipe_vram()
{
	const int zero = 0;
	short* vram = (short*)0x06014000;
	for(int i=0;i<0x3400/2;i++) 
	{		
		__asm("STRH %[_0], [%[dest_map]]"				
				:
				: [dest_map] "r" (vram), [_0] "r" (zero));    
		vram++;
	}
	
}

BuildingSprite::~BuildingSprite()
{
	for(int i=0;i<3;i++)
		delete auxiliary[i];
	if(auto_clean)
		delete building;	
	wipe_vram();
}