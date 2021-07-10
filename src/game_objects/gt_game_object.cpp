#include "gt_game_object.h"
#include "gt_globals.h"

namespace gt{
    bool game_object::is_destroyed()
    {
        return _is_destroyed;
    }

    void game_object::destroy()
    {
        globals::go_manager->destroy_object(this);
    }

    void game_object::mark_as_destroyed()
    {
        _is_destroyed=true;
    }

    void game_object::update()
    {

    }

    bn::optional<bn::fixed_point> game_object::get_pos()
    {
        return bn::optional<bn::fixed_point>();
    }

    bn::optional<bn::fixed_rect> game_object::get_collision_rect()
    {
        if(_local_collision_rect)
        {
            bn::fixed_rect rect =*_local_collision_rect;
            bn::optional<bn::fixed_point> pos = get_pos();
            if(pos)
            {
                return bn::optional<bn::fixed_rect>(bn::fixed_rect(rect.position()+(*pos),rect.dimensions()));
            }
        }
        return bn::optional<bn::fixed_rect>();
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

    void game_object::do_damage(int amount)
    {

    }
#pragma GCC diagnostic pop
    
    bool game_object::check_map_collision(bn::fixed_point pos)
    {
        bn::fixed_rect test_box(bn::point(pos.x().right_shift_integer(), 
            pos.y().right_shift_integer()) + _local_collision_rect->position(), _local_collision_rect->dimensions());
        return gt::globals::current_map->check_rect_collision(test_box);
    }

    bool game_object::intersects(game_object *other)
    {
        bn::optional<bn::fixed_rect> myRect=get_collision_rect();
        if(!myRect)
            return false;
        bn::optional<bn::fixed_rect> otherRect=other->get_collision_rect();
        if(!otherRect)
            return false;
        return myRect->intersects(*otherRect);
    }

    bool game_object::intersects(const bn::fixed_point &pos)
    {
        bn::optional<bn::fixed_rect> myRect=get_collision_rect();
        if(!myRect)
            return false;
        return myRect->contains(pos);
    }

    bool game_object::is_interactive()
    {
        return _is_interactive;
    }

    void game_object::interact()
    {
        
    }

    Def game_object::type()
    {
        return Def::null;
    }
}