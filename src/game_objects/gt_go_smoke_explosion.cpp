#include "gt_go_smoke_explosion.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_items_smoke_explosion.h"
#include "bn_sound_items.h"
#include "gt_globals.h"

namespace
{
    [[nodiscard]] bn::sprite_ptr _create_sprite()
    {
        bn::sprite_builder builder(bn::sprite_items::smoke_explosion);
        return builder.release_build();
    }

    bn::sprite_animate_action<5> _create_animation(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<5>::once(sprite, 4, bn::sprite_items::smoke_explosion.tiles_item(), bn::array<uint16_t, 5>{{0, 1,2,3,4}});
    }

}

namespace gt
{
    go_smoke_explosion::go_smoke_explosion(const bn::camera_ptr& camera, bn::fixed_point pos):
        _sprite(_create_sprite()),
        _action(_create_animation(_sprite))
        {
            _sprite.set_camera(camera);
            _sprite.set_position(pos);
            _sprite.set_z_order(-1000);
            bn::sound_items::smoke_explosion.play(1);
        }
    
    void go_smoke_explosion::update()
    {
        if(_action.done())
        {
            globals::go_manager->destroy_object(this);
        }
        else
        {
            _action.update();
        }
    }
}