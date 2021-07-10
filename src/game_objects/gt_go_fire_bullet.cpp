#include "gt_go_fire_bullet.h"
#include "bn_sprite_items_fireball.h"
#include "bn_sprite_builder.h"
#include "bn_sound_items.h"
#include "gt_globals.h"


namespace
{
    bn::sprite_ptr _create_sprite(bn::camera_ptr& camera, bn::fixed_point position)
    {
        bn::sprite_builder builder(bn::sprite_items::fireball);
        builder.set_camera(camera);
        builder.set_position(position);
        builder.set_z_order(-1000);
        return builder.release_build();
    }

    bn::sprite_animate_action<4> _create_animation_idle(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<4>::forever(sprite, 4, bn::sprite_items::fireball.tiles_item(), bn::array<uint16_t, 4>{{0, 1,2,3}});
    }

}

namespace gt
{
    go_fire_bullet::go_fire_bullet(bn::camera_ptr& camera, bn::fixed_point position, bn::fixed_point velocity):
    _sprite(_create_sprite(camera, position)),
    _action(_create_animation_idle(_sprite)),
    _velocity(velocity)
    {
        
    }

    void go_fire_bullet::update_hero_collision()
    {
        
            bn::fixed_rect rect=bn::fixed_rect(_sprite.position(),bn::fixed_size(16,16));
            if(rect.contains(*globals::hero->get_pos()))
            {
                if(!globals::hero->is_blocking(_sprite.position()))
                {
                    if(globals::hero->is_vulnerable())
                        globals::hero->do_damage(1);
                }
                else
                    bn::sound_items::shield_bounce.play(1);
                globals::go_manager->destroy_object(this);
            }
            
    }

    void go_fire_bullet::update()
    {
        _sprite.set_position(_sprite.position() + _velocity);
        update_hero_collision();
        if(_time++>60)
        {
            globals::go_manager->destroy_object(this);
        }

        if(!_action.done())
            _action.update();
    }

}