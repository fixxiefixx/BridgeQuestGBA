#include "gt_go_chest.h"
#include "bn_sprite_items_chest.h"
#include "bn_sprite_builder.h"
#include "gt_globals.h"
#include "gt_util.h"
#include "bn_string_view.h"
#include "bn_format.h"

namespace
{
    bn::sprite_ptr _create_sprite()
    {
        bn::sprite_builder builder(bn::sprite_items::chest);
        return builder.release_build();
    }
}

namespace gt
{
    go_chest::go_chest(bn::camera_ptr& camera, bn::fixed_point position, Def name, Def item):
    _sprite(_create_sprite()),
    _item(item),
    _name(name)
    {
        _sprite.set_camera(camera);
        _sprite.set_z_order(1000);
        _sprite.set_blending_enabled(true);
        _sprite.set_position(position);
        _local_collision_rect=bn::fixed_rect(0, 0, 16, 16);
        _is_interactive=true;
        if(globals::save.used_object(_name))
        {
            _open=true;
            _sprite.set_tiles(bn::sprite_items::chest.tiles_item(),1);
        }
    }

    bn::optional<bn::fixed_point> go_chest::get_pos()
    {
        return  bn::optional<bn::fixed_point>(_sprite.position());
    }

    void go_chest::interact()
    {
        if(_open)
        {
            globals::gui->textbox("The chest is empty.");
        }
        else
        {
            
            bn::vector<bn::string_view,4> options;
            options.push_back("Yes");
            options.push_back("No");
            int choice = globals::gui->textbox("Do you want to open the chest?",options);
            if(choice==0)
            {
                _open=true;
                globals::save.use_object(_name);
                _sprite.set_tiles(bn::sprite_items::chest.tiles_item(),1);
                globals::gui->textbox(bn::format<64>("You opened the chest.\nYou got {}.",itemdef_to_stringview(_item)));

                globals::hero->collect_item(_item);
            }
        }
    }

    void go_chest::update()
    {
        /*if(globals::hero->intersects(this))
        {
            bn::fixed_point me_to_hero=(*(globals::hero->get_pos()))-_sprite.position();
           globals::hero->velocity=normalize(me_to_hero);
                
            
        }*/
    }
}