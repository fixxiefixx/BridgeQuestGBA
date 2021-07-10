#include "gt_go_bed.h"
#include "gt_globals.h"
#include "bn_blending.h"
#include "bn_blending_actions.h"
#include "bn_sound_items.h"

namespace gt
{
    go_bed::go_bed(bn::fixed_rect rect)
    {
        _local_collision_rect = rect;
        _is_interactive=true;
    }

    bn::optional<bn::fixed_point> go_bed::get_pos()
    {
        return bn::optional<bn::fixed_point>(bn::fixed_point(0,0));
    }

    void go_bed::interact()
    {
        bn::vector<bn::string_view,4> options;
        options.push_back("Yes");
        options.push_back("No");
        int option=globals::gui->textbox("Do you want to take a nap?",options);
        if(option==0)
        {
            globals::hero->set_blending_enabled(true);
            globals::current_map->bg().set_blending_enabled(true);
            bn::blending::set_fade_alpha(0);

            {
                bn::blending_fade_alpha_to_action action(30,1);
                while(!action.done())
                {
                    action.update();
                    bn::core::update();
                }
            }

            for(int i=0;i<30;i++)
                {
                    bn::core::update();
                }

            while(globals::gui->health()<globals::gui->max_health())
            {
                bn::sound_items::collect.play(1);
                globals::gui->set_health(globals::gui->health()+1);
                for(int i=0;i<20;i++)
                {
                    bn::core::update();
                }
            }

            {
                bn::blending_fade_alpha_to_action action(30,0);
                while(!action.done())
                {
                    action.update();
                    bn::core::update();
                }
            }
            globals::hero->set_blending_enabled(false);
        }
    }

}