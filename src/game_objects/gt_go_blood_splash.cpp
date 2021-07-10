#include "gt_go_blood_splash.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_items_blood_splash.h"
#include "bn_sound_items.h"
#include "gt_globals.h"

namespace
{
    [[nodiscard]] bn::sprite_ptr _create_sprite()
    {
        bn::sprite_builder builder(bn::sprite_items::blood_splash);
        return builder.release_build();
    }

    bn::sprite_animate_action<7> _create_animation(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<7>::once(sprite, 4, bn::sprite_items::blood_splash.tiles_item(), bn::array<uint16_t, 7>{{0, 1,2,3,4,5,6}});
    }

}

namespace gt
{
    go_blood_splash::go_blood_splash(const bn::camera_ptr& camera, bn::fixed_point pos):
        _sprite(_create_sprite()),
        _action(_create_animation(_sprite))
        {
            _sprite.set_camera(camera);
            _sprite.set_position(pos);
            _sprite.set_z_order(-1000);
            bn::sound_items::squisch.play(1);
        }
    
    void go_blood_splash::update()
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