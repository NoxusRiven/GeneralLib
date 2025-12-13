#include "Collision.h"

namespace Collision
{
    /* ---------------------------- BOX OBJECT ----------------------------------- */
    Box::Box(Rectangle rect, bool isSolid) : 
        _bounds(rect), _solid(isSolid)
    {
    }


    /* ---------------------------- - SECTOR GRID OBJECT---------------------------------- - */
    SectorGrid::SectorGrid(Size map_size, BoxTypes objects, int sector_size) :
        _sector_size(sector_size)
    {
        for (auto box : objects.static_boxes) 
        {
            int minX = (int)(box->get_bounds().x / sector_size);
            int minY = (int)(box->get_bounds().y / sector_size);
            int maxX = (int)((box->get_bounds().x + box->get_bounds().width) / sector_size);
            int maxY = (int)((box->get_bounds().y + box->get_bounds().height) / sector_size);

            for (int sx = minX; sx <= maxX; sx++)
            {
                for (int sy = minY; sy <= maxY; sy++)
                {
                    _sectors[{sx, sy}].static_boxes.push_back(box);
                }
            }
        }

        //check for erros in this fors
        for (auto box : objects.dynamic_boxes) 
        {
            int minX = (int)(box->get_bounds().x / sector_size);
            int minY = (int)(box->get_bounds().y / sector_size);
            int maxX = (int)((box->get_bounds().x + box->get_bounds().width) / sector_size);
            int maxY = (int)((box->get_bounds().y + box->get_bounds().height) / sector_size);

            std::vector<std::pair<int,int>> sector_pos;

            for (int sx = minX; sx <= maxX; sx++) 
            {
                for (int sy = minY; sy <= maxY; sy++) 
                {
                    _sectors[{sx, sy}].dynamic_boxes.push_back(box);
                    sector_pos.push_back({ sx, sy });
                }
            }

            _object_to_sectors[box] = sector_pos;
        }

    }

    BoxTypes SectorGrid::get_boxes_from_sector(std::shared_ptr<Box> box)
    {
        std::vector<std::pair<int, int>>& sector_pos = _object_to_sectors[box];

        if (sector_pos.size() == 1)
        {
            return _sectors[sector_pos[0]];
        }
        else
        {
            BoxTypes boxes_in_sectors;
            for (auto pos : sector_pos)
            {
                BoxTypes& sector_boxes = _sectors[pos];
                boxes_in_sectors.static_boxes.insert( boxes_in_sectors.static_boxes.end(), sector_boxes.static_boxes.begin(), sector_boxes.static_boxes.end() );
                
                boxes_in_sectors.dynamic_boxes.insert( boxes_in_sectors.dynamic_boxes.end(), sector_boxes.dynamic_boxes.begin(), sector_boxes.dynamic_boxes.end() );
            }
            return boxes_in_sectors;
        }
    }


    /* ---------------------------- COLLISION MANAGER OBJECT ----------------------------------- */
    
    CollisionManager::CollisionManager(Size map_size, BoxTypes boxes) :
        _map_size(map_size), _sector_grid( _map_size, boxes , 100 )
    {
    }
    
    //check collision made for boxes 
    bool CollisionManager::check_box_collision(Box& box1, Box& box2)
    {
        return CheckCollisionRecs(box1.get_bounds(), box2.get_bounds());
    }

    /*
        Will always offset first box!
        Player or moving entity has to be in first parameter
    */
     //u have to check outside of that methode if there is collision
    bool CollisionManager::handle_solid_box_collision(Box& movingBox, Box& box)
    {
        if (!movingBox.get_solid() || !box.get_solid()) return false;

        //logic of collision handling breaks with small boxes, so ignore them, limit 4x4
        if((movingBox.get_bounds().width + movingBox.get_bounds().height) < 8 || 
           (box.get_bounds().width + box.get_bounds().height) < 8) return false;


        Rectangle intersection = GetCollisionRec(movingBox.get_bounds(), box.get_bounds());


        if (intersection.width >= intersection.height)
        {
            if (box.get_bounds().y < movingBox.get_bounds().y) // box is above movingBox
            {
                movingBox.set_y(movingBox.get_bounds().y + intersection.height);
            }
            else if (box.get_bounds().y > movingBox.get_bounds().y) // box is below movingBox
            {
                movingBox.set_y(movingBox.get_bounds().y - intersection.height);
            }
        }

        if (intersection.width <= intersection.height)
        {
            if (box.get_bounds().x < movingBox.get_bounds().x) // box is left of movingBox
            {
                movingBox.set_x(movingBox.get_bounds().x + intersection.height);
            }
            else if (box.get_bounds().x > movingBox.get_bounds().x) // box is right of movingBox
            {
                movingBox.set_x(movingBox.get_bounds().x - intersection.height);
            }
        }

        return true;
    }

   
    //u have to check outside of that methode if there is collision
    Rectangle CollisionManager::handle_solid_box_colli_rect(Box& movingBox, Box & box)
    {
       if (!movingBox.get_solid() || !box.get_solid()) return {0,0,0,0};

       //logic of collision handling breaks with small boxes, so ignore them, limit 4x4
       if((movingBox.get_bounds().width + movingBox.get_bounds().height) < 8 || 
          (box.get_bounds().width + box.get_bounds().height) < 8) return {0,0,0,0};




        Rectangle intersection = GetCollisionRec(movingBox.get_bounds(), box.get_bounds());

        if (intersection.width >= intersection.height)
        {
            if (box.get_bounds().y < movingBox.get_bounds().y) // box is above movingBox
            {
                movingBox.set_y(movingBox.get_bounds().y + intersection.height);
            }
            else if (box.get_bounds().y > movingBox.get_bounds().y) // box is below movingBox
            {
                movingBox.set_y(movingBox.get_bounds().y - intersection.height);
            }
        }

        if (intersection.width <= intersection.height)
        {
            if (box.get_bounds().x < movingBox.get_bounds().x) // box is left of movingBox
            {
                movingBox.set_x(movingBox.get_bounds().x + intersection.width);
            }
            else if (box.get_bounds().x > movingBox.get_bounds().x) // box is right of movingBox
            {
                movingBox.set_x(movingBox.get_bounds().x - intersection.width);
            }
        }

        return intersection;

    }

    std::vector<std::shared_ptr<Box>> CollisionManager::check_entity_collision(Box& entity)
    {
        std::vector<std::shared_ptr<Box>> boxesCollided;

       /* for (auto box : _boxes)
        {
            if (box.get() == &entity)
            {
                continue;
            }

            if (check_box_collision(entity, *box))
            {
                boxesCollided.push_back(box);
            }
        }*/

        BoxTypes boxes_in_sectors = _sector_grid.get_boxes_from_sector(std::make_shared<Box>(entity));
        auto& boxes = boxes_in_sectors.static_boxes;
        boxes.insert(boxes.end(), boxes_in_sectors.dynamic_boxes.begin(), boxes_in_sectors.dynamic_boxes.end());

        for(auto box : boxes)
        {
            if (box.get() == &entity)
            {
                continue;
            }
            if (check_box_collision(entity, *box))
            {
                boxesCollided.push_back(box);
            }
        }


        return boxesCollided;
    }




/*---------------------------------------------- CODE GRAVEYARD -------------------------------------------------------------*/
//stuff that might be usefull later but for now i abandoned that idea cuz it was over engeeniered
}









 