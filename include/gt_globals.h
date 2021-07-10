#ifndef GT_GLOBALS_H
#define GT_GLOBALS_H

#include "bn_random.h"
#include "gt_game_object_manager.h"
#include "gt_map.h"
#include "gt_go_hero.h"
#include "gt_following_camera.h"
#include "gt_gui.h"
#include "gt_music_manager.h"
#include "gt_savegame.h"

namespace gt::globals
{
    extern gt::game_object_manager *go_manager;
    extern gt::map *current_map;
    extern gt::go_hero *hero;
    extern bn::random *random;
    extern bn::camera_ptr *camera;
    extern gt::following_camera *followcam;
    extern gt::gui *gui;
    extern bool scene_running;
    extern gt::music_manager *music;
    extern gt::savegame save;
    extern bool to_main_menu;
}

#endif