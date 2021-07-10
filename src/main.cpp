#define BN_CFG_MEMORY_MAX_EWRAM_ALLOC_ITEMS 64

#include "bn_core.h"
#include "bn_log.h"
#include "bn_fixed.h"
#include "bn_fixed_rect.h"
#include "bn_keypad.h"
#include "bn_colors.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_memory.h"
#include "bn_blending_actions.h"
#include "bn_random.h"

#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_regular_bg_items_title.h"
#include "bn_regular_bg_items_gbajam_splash.h"
#include "bn_regular_bg_items_overworld.h"
#include "bn_sprite_items_hero.h"
#include "bn_string_view.h"
#include "bn_sram.h"

#include "gt_go_hero.h"
#include "gt_go_blob.h"
#include "gt_go_bat.h"
#include "gt_go_knight.h"
#include "gt_game_object_manager.h"
#include "gt_globals.h"
#include "gt_map.h"
#include "gt_following_camera.h"
#include "gt_simple_fixed_rect.h"
#include "maps.h"
#include "gt_gui.h"
#include "bn_bg_palettes.h"


void gbajam_splash_scene()
{
    bool abort=false;
    bn::regular_bg_ptr splash_bg=bn::regular_bg_items::gbajam_splash.create_bg(0,0);
    splash_bg.set_blending_enabled(true);
    bn::blending::set_fade_alpha(1);
    
    bn::blending_fade_alpha_to_action action(60,0);
    while(!action.done() && !abort)
    {
        action.update();
        bn::core::update();
        if(bn::keypad::start_pressed())
            abort=true;
    }

    int framesToWait=60*2;
    while(framesToWait-- && !abort)
    {
        bn::core::update();
        if(bn::keypad::start_pressed())
            abort=true;
    }

    action=bn::blending_fade_alpha_to_action(60,1);
    while(!action.done() && !abort)
    {
        action.update();
        bn::core::update();
        if(bn::keypad::start_pressed())
            abort=true;
    }
    bn::blending::set_fade_alpha(0);
}

void scene()
{
    
    gt::globals::music->play(&bn::music_items::forgottenone,0.5);

    bn::camera_ptr camera = bn::camera_ptr::create(0, 0);
    gt::globals::camera=&camera;
	gt::following_camera followcam(camera);
    gt::globals::followcam=&followcam;

    

    const int min_x = -1024/2 + 240/2;
    const int min_y = -1024/2 + 160/2;
    const int max_x = 1024/2 - 240/2-1;
    const int max_y = 1024/2 - 160/2-1;

    bn::rect cam_limits=bn::rect(0, 0,1024-240,1024-160);
    BN_LOG("min_x ",min_x);
    BN_LOG("cam_limits_left ",cam_limits.left());

    followcam.set_limits(cam_limits);

    BN_LOG("health: ",gt::globals::save.health);

    gt::gui gui=gt::gui();
    gt::globals::gui=&gui;
    //gui.set_max_health(4);
    //gui.set_health(gui.max_health());
    gui.set_max_health(gt::globals::save.max_health);
    gui.set_health(gt::globals::save.health);
    gui.set_collected_wood(gt::globals::save.collected_wood);

    gt::game_object_manager go_manager;
    gt::globals::go_manager = &go_manager;

    gt::go_hero *hero = new gt::go_hero(camera);
    gt::globals::hero = hero;
    go_manager.add_object(hero);

    const gt::map_info *m_info=nullptr;
    for(int i=0;i<mapinfo_count;i++)
    {
        const gt::map_info *info = &mapinfos[i];
        if(info->name==gt::globals::save.current_warp.dest_map)
        {
            m_info=info;
            break;
        }
    }

    BN_ASSERT(m_info!=nullptr,"warp destination map not found");
    const gt::warp *dest_warp=nullptr;
    for(int i=0;i<m_info->warp_count;i++)
    {
        const gt::warp *warp = &m_info->warps[i];
        if(gt::globals::save.current_warp.dest_warp==warp->name)
        {
            dest_warp=warp;
            break;
        }
    }

    BN_ASSERT(dest_warp!=nullptr,"destination warp not found");

    hero->set_pos(dest_warp->position + bn::fixed_point(0,-8));
    hero->set_direction((gt::go_hero::Direction)gt::globals::save.current_warp.dest_rotation);
    
    followcam.set_target(hero);
    

    gt::map overworld_map(camera, *m_info);
    gt::globals::current_map = &overworld_map;
    hero->update();
    followcam.update();

    gt::globals::current_map->bg().set_blending_enabled(true);
    bn::blending::set_fade_alpha(1);
    
    bn::blending_fade_alpha_to_action action(15,0);
    while(!action.done())
    {
        action.update();
        bn::core::update();
    }
    gt::globals::current_map->bg().set_blending_enabled(false);
    
    gt::globals::scene_running=true;
    while(gt::globals::scene_running)
    {

        /*if(bn::keypad::select_pressed())
        {
            BN_LOG("CPU usage: ",bn::core::current_cpu_usage()*100);
        }

        if(bn::keypad::start_pressed())
        {
            gui.textbox("hello world");
        }*/

        go_manager.update();
        overworld_map.update();
        followcam.update();
        bn::core::update();
    }

    gt::globals::save.max_health=gui.max_health();
    
    if(gui.health()==0)
    {
        gt::globals::save.health=gui.max_health();
    }
    else
    {
        gt::globals::save.health=gui.health();
    }
    
    
    //go_manager.destroy_all_except(nullptr);
    //go_manager.update();
}

void main_menu()
{
    bn::regular_bg_ptr title_bg=bn::regular_bg_items::title.create_bg(0,-6*8);
    title_bg.set_blending_enabled(true);
    bn::blending::set_fade_alpha(1);
    {
        bn::blending_fade_alpha_to_action action(60,0);
        for(int i=title_bg.y().right_shift_integer(); i<=6*8; i++)
        {
            title_bg.set_y(i);
            for(int j=0;j<2;j++)
            {
                if(!action.done())
                    action.update();

                bn::core::update();
            }
            
        }
    }

    gt::savegame loaded_save;
    bn::sram::read(loaded_save);
    bool save_valid=loaded_save.is_valid();
    gt::gui gui;
    bool startgame=false;
    while(!startgame)
    {
        bn::vector<bn::string_view,4> options;
        if(save_valid)
            options.push_back("Continue");
        options.push_back("New game");
        int option = gui.textbox(" ",options);
        if(option==0 && save_valid)
        {
            gt::globals::save=loaded_save;
            startgame=true;
        }
        else if(option==1)
        {
            options.clear();
            options.push_back("No");
            options.push_back("Yes");
            int option2=gui.textbox("Your previous progress will be lost.\nDo you really want to start a new game?",options);
            if(option2==1)
            {
                gt::globals::save=gt::savegame();
                startgame=true;
            }
        }
        else
        {
            startgame=true;
        }
    }
    {
        bn::blending_fade_alpha_to_action action(30,1);
        while(!action.done())
        {
            action.update();
            bn::core::update();
        }
    }
}

int main()
{
    bn::core::init();
    BN_LOG("butano initalized");
    bn::random random=bn::random();
    gt::globals::random=&random;

    gt::music_manager music=gt::music_manager();
    gt::globals::music=&music;

    gbajam_splash_scene();

    //gt::warp start_warp=gt::warp(Def::null,Def::indoor,Def::hero_house_indoor,bn::fixed_point(-400,488),0);
    

    while(true)
    {
        //gt::globals::save.current_warp=start_warp;
        gt::globals::to_main_menu=false;
        music.play(&bn::music_items::easter,1);
        bn::bg_palettes::set_transparent_color(bn::color(0, 0, 0));
        main_menu();

        while(!gt::globals::to_main_menu)
        {
            //bn::core::update();
            scene();
            //bn::core::update();
        }
    }
}
