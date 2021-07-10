#include "gt_go_blocking_log.h"
#include "bn_sprite_items_blocking_log.h"
#include "bn_sprite_items_broken_bridge.h"
#include "bn_sprite_builder.h"
#include "gt_util.h"
#include "gt_globals.h"

namespace
{
    bn::sprite_ptr _create_sprite(Def type)
    {
        switch(type)
        {
            default:
            case Def::blocking_log:
            case Def::blocking_log_vertical:
            {
                bn::sprite_builder builder(bn::sprite_items::blocking_log);
                return builder.release_build();
            }break;
            case Def::broken_bridge:
            {
                bn::sprite_builder builder(bn::sprite_items::broken_bridge);
                return builder.release_build();
            }
        }
        
    }
}

namespace gt
{
    go_blocking_log::go_blocking_log(bn::camera_ptr& camera, bn::fixed_point position, Def type):
    _sprite(_create_sprite(type))
    {
        _type=type;
        _sprite.set_blending_enabled(true);
        _sprite.set_camera(camera);
        _sprite.set_position(position);
        
        switch(type)
        {
            default:
            case Def::blocking_log:
            {
                _vertical=false;
                _local_collision_rect=bn::fixed_rect(0,0,32,16);
                _sprite.set_visible(false);
                _sprite.set_z_order(-_sprite.y().right_shift_integer());
            }break;
            case Def::blocking_log_vertical:
            {
                _vertical =true;
                _sprite.set_rotation_angle(90);
                _local_collision_rect=bn::fixed_rect(0,0,16,32);
                _sprite.set_visible(false);
                _sprite.set_z_order(-_sprite.y().right_shift_integer());
            }break;
            case Def::broken_bridge:
            {
                _vertical =true;
                _local_collision_rect=bn::fixed_rect(0,0,16,32);
                _sprite.set_z_order(1000);
                _sprite.set_visible(!globals::save.repaired_bridge);
            }break;
        }

    }

    bn::optional<bn::fixed_point> go_blocking_log::get_pos()
    {
        return  bn::optional<bn::fixed_point>(_sprite.position());
    }

    void go_blocking_log::update()
    {
        if(!_sprite.visible())
            return;
        if(globals::hero->intersects(this))
        {
            bn::fixed_point me_to_hero=(*(globals::hero->get_pos()))-_sprite.position();
            if(_vertical)
            {
                if(me_to_hero.x()>bn::fixed(0))
                {
                    globals::hero->velocity.set_x(1);
                }
                else
                {
                    globals::hero->velocity.set_x(-1);
                }
            }
            else
            {
                if(me_to_hero.y()>bn::fixed(0))
                {
                    globals::hero->velocity.set_y(1);
                }
                else
                {
                    globals::hero->velocity.set_y(-1);
                }
            }
                
            
        }
    }

    void go_blocking_log::set_enabled(bool enabled)
    {
        _sprite.set_visible(enabled);
    }

    bool go_blocking_log::enabled()
    {
        return _sprite.visible();
    }

    Def go_blocking_log::type()
    {
        return _type;
    }

}