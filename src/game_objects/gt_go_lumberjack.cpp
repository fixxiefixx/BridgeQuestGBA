#include "gt_go_lumberjack.h"
#include "bn_sprite_items_lumberjack.h"
#include "bn_sprite_builder.h"
#include "bn_vector.h"
#include "gt_globals.h"
#include "gt_util.h"

namespace
{
    bn::sprite_ptr _create_sprite()
    {
        bn::sprite_builder builder(bn::sprite_items::lumberjack);
        return builder.release_build();
    }
}

namespace gt
{
    go_lumberjack::go_lumberjack(bn::camera_ptr& camera, bn::fixed_point position):
    _sprite(_create_sprite())
    {
        _sprite.set_camera(camera);
        _sprite.set_position(position);
        _sprite.set_z_order(-_sprite.y().right_shift_integer());
        _sprite.set_blending_enabled(true);
        _local_collision_rect=bn::fixed_rect(0, 8, 16, 16);
        _is_interactive=true;
    }

    bn::optional<bn::fixed_point> go_lumberjack::get_pos()
    {
        return  bn::optional<bn::fixed_point>(_sprite.position());
    }

    void go_lumberjack::interact()
    {
        globals::gui->textbox("I chopped too much wood.\nThe spirit of the forest is angry now.\nYou can take the wood in the chest,\nbut if I where you, I would better not\ntouch the wood because it's cursed.");
    }

    void go_lumberjack::update()
    {
        if(globals::hero->intersects(this))
        {
            bn::fixed_point me_to_hero=(*(globals::hero->get_pos()))-_sprite.position();
           globals::hero->velocity=normalize(me_to_hero);
                
            
        }
    }

}