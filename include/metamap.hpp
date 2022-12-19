#pragma once
#include "building.hpp"

#include <Astralbrew>

class Metamap
{
private:
	struct Record
	{
		const Building* building;
		int x;
		int y;
		int width;
		int height;		
	};
private:
	inline static constexpr int height = 73;
	inline static constexpr int stride = 48;
	char* map_metadata = new char[height*stride];
	const Building* *bld_addr = new const Building*[height*stride];
	
	inline static constexpr int building_max_height = 80;
	
	Astralbrew::Vector<Record>* buildings = new Astralbrew::Vector<Record>[height];
		
public:
	Metamap();
	
	char* get_buffer() const;
	
	bool can_place(const Building* building, int row, int col) const;
		
	void place(const Building* building, int row, int col);
	
	void draw_buildings(int x, int y, int w, int h, short* dest, int dest_stride) const;
		
	const Building* building_at(int x, int y) const;
	
	void remove(const Building* building);
	
	~Metamap();
};