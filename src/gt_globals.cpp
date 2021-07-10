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
    gt::game_object_manager *go_manager = nullptr;
    gt::map *current_map = nullptr;
    gt::go_hero *hero = nullptr;
    bn::random *random = nullptr;
    bn::camera_ptr *camera = nullptr;
    gt::following_camera *followcam;
    gt::gui *gui;
    bool scene_running = false;
    gt::music_manager *music=nullptr;
    gt::savegame save;
    bool to_main_menu=false;
}