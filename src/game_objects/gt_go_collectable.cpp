#include "gt_go_collectable.h"
#include "bn_sprite_items_shadow_small.h"
#include "bn_sprite_items_heart.h"
#include "bn_sprite_items_blob.h"
#include "bn_sprite_items_shadow_small.h"
#include "bn_sprite_builder.h"
#include "gt_globals.h"
#include "bn_sound_items.h"

#define __ go_collectable

namespace
{
    [[nodiscard]] bn::sprite_ptr _create_sprite_item(bn::fixed_point pos, bn::camera_ptr camera, Def item)
    {
        const bn::sprite_item* spr_item;

        switch(item)
        {
            default:
            {
                spr_item = &bn::sprite_items::blob;
            }break;
            case Def::heart:
            {
                spr_item =&bn::sprite_items::heart;
            }break;
        }

        bn::sprite_builder builder(*spr_item);
        builder.set_position(pos);
        builder.set_camera(camera);
        builder.set_z_order(-pos.y().right_shift_integer());
        return builder.release_build();
    }

    [[nodiscard]] bn::sprite_ptr _create_sprite_shadow(bn::fixed_point pos, bn::camera_ptr camera)
    {
        bn::sprite_builder builder(bn::sprite_items::shadow_small);
        builder.set_position(pos);
        builder.set_camera(camera);
        builder.set_z_order(1000);
        return builder.release_build();
    }

}

namespace gt
{
    __::__(const bn::camera_ptr& camera, bn::fixed_point pos, Def item):
    _sprite(_create_sprite_item(pos, camera, item)),
    _shadow_sprite(_create_sprite_shadow(pos, camera)),
    _down_speed(-3),
    _down_pos(0)
    {
        _local_collision_rect=bn::fixed_rect(0,0,8,8);
        _item=item;
    }

    void __::update_hero_collision()
    {
        if(_down_speed >= bn::fixed(-0.1) && globals::hero->intersects(this))
        {
            bn::sound_items::collect.play(1);
            globals::go_manager->destroy_object(this);
            globals::hero->collect_item(_item);
        }
    }

    void __::update()
    {
        _down_speed = _down_speed + bn::fixed(0.1);
        _down_pos = _down_pos + _down_speed;
        if(_down_pos > bn::fixed(0))
        {
            _down_pos=0;
            _down_speed = _down_speed * bn::fixed(-0.5);
        }
        _sprite.set_y(_shadow_sprite.y()+_down_pos);
        update_hero_collision();
        _shadow_sprite.set_visible(!_shadow_sprite.visible());
        if(_time++>=60*5)
        {
            globals::go_manager->destroy_object(this);
        }else
        {
            if(_time>=60*4)
            {
                _sprite.set_visible(!_sprite.visible());
            }
        }
    }

    bn::optional<bn::fixed_point> __::get_pos()
    {
        return bn::optional<bn::fixed_point>(_shadow_sprite.position());
    }

}