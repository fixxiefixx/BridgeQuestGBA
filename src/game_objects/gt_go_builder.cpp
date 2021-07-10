#include "gt_go_builder.h"
#include "bn_sprite_items_builder.h"
#include "bn_sprite_builder.h"
#include "bn_vector.h"
#include "gt_globals.h"
#include "gt_util.h"

namespace
{
    bn::sprite_ptr _create_sprite()
    {
        bn::sprite_builder builder(bn::sprite_items::builder);
        return builder.release_build();
    }
}

namespace gt
{
    go_builder::go_builder(bn::camera_ptr& camera, bn::fixed_point position):
    _sprite(_create_sprite())
    {
        _sprite.set_camera(camera);
        _sprite.set_position(position);
        _sprite.set_z_order(-_sprite.y().right_shift_integer());
        _sprite.set_blending_enabled(true);
        _local_collision_rect=bn::fixed_rect(0, 8, 16, 16);
        _is_interactive=true;
        if(globals::save.current_warp.dest_map==Def::overworld)
        {
            if(!globals::save.repaired_bridge)
                _sprite.set_visible(false);
        }

        if(globals::save.current_warp.dest_map==Def::indoor)
        {
            if(globals::save.repaired_bridge)
                _sprite.set_visible(false);
        }
    }

    bn::optional<bn::fixed_point> go_builder::get_pos()
    {
        return  bn::optional<bn::fixed_point>(_sprite.position());
    }

    void go_builder::interact()
    {
        if(!_sprite.visible())
            return;

        if(globals::save.current_warp.dest_map==Def::indoor)
        {
            if(globals::save.repaired_bridge)
                globals::gui->textbox("I will repair the bridge. You can go ahead.\nI will take a shortcuit.");
            else
            {
                if(globals::save.collected_wood)
                {
                    bn::vector<bn::string_view, 4> options;
                    options.push_back("Yes");
                    options.push_back("No");
                    int option=globals::gui->textbox("Oh you have wood.\nWith that wood I can repair the bridge.\nDo you want to give me the wood?",options);
                    if(option==0)
                    {
                        globals::gui->set_collected_wood(false);
                        globals::save.collected_wood=false;
                        globals::save.repaired_bridge=true;
                        globals::gui->textbox("Oh thanks, I will start repairing the bridge now.");
                    }else
                    {
                        globals::gui->textbox("Ok, you can visit me every time\nand ask me to repair the bridge.");
                    }
                }
                else
                {
                    globals::gui->textbox("Hello, I'm the builder.\nDo you know the broken bridge in the north?\nI could repair the bridge if I had wood.\nUnfortunately the forest is cursed\nand monsters live in it.");
                    globals::gui->textbox("You better dont go in there\nif you dont want to die.");
                }
                
            }
        }else
        {
            globals::gui->textbox("I repaired the bridge.");
        }
    }

    void go_builder::update()
    {
        if(!_sprite.visible())
            return;

        if(globals::hero->intersects(this))
        {
            bn::fixed_point me_to_hero=(*(globals::hero->get_pos()))-_sprite.position();
           globals::hero->velocity=normalize(me_to_hero);
                
            
        }
    }

}