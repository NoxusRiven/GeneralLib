#pragma once

#include "raylib.h"

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>

namespace Collision
{
	struct Size
	{
		float width;
		float height;
	};

    class Box
    {
    private:
        Rectangle _bounds;
        bool _solid;

    public:
        Box(Rectangle rect, bool isSolid);

        Rectangle get_bounds() { return _bounds; }
        void set_x(int newX) { _bounds.x = newX; }
        void set_y(int newY) { _bounds.y = newY; }

        bool get_solid() { return _solid; }
    };



	struct BoxTypes
	{
		std::vector<std::shared_ptr<Box>> static_boxes;
		std::vector<std::shared_ptr<Box>> dynamic_boxes;
    };

	class SectorGrid
	{
	private:
		std::map<std::pair<int, int>, BoxTypes> _sectors;
		std::unordered_map<std::shared_ptr<Box>, std::vector<std::pair<int,int>>> _object_to_sectors;

        int _sector_size;


	public:
		SectorGrid(Size map_size, BoxTypes objects, int sector_size);
        BoxTypes get_boxes_from_sector(std::shared_ptr<Box> box);

	};

	class CollisionManager
	{

	private:
		// map of pair and 2 vectors
		// pair: x,y of sector ; SectorBoxes: container for static and dynamic box vectors

		Size _map_size;
        SectorGrid _sector_grid;

	public:
		
		CollisionManager(Size map_size, BoxTypes boxes);

		bool check_box_collision(Box& box1, Box& box2);
		bool handle_solid_box_collision(Box& movingBox, Box& box);

		std::vector<std::shared_ptr<Box>> check_entity_collision(Box& entity);

		//testing
		Rectangle handle_solid_box_colli_rect(Box& movingBox, Box& box);


		//for efficient collisions with sectors
		//CollisionManager(Vector2 map_size, std::vector<std::shared_ptr<Box>>& static_b, std::vector<std::shared_ptr<Box>>& dynamic_b, int hitbox_cell_size);
		std::vector<std::shared_ptr<Box>> check_entity_collision_g(Box& entity);
	};
	
}


