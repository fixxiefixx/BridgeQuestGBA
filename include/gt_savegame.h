#ifndef GT_SAVEGAME_H
#define GT_SAVEGAME_H

#include "bn_core.h"
#include "bn_array.h"
#include "definitions.h"
#include "gt_go_hero.h"
#include "gt_warp.h"

namespace gt
{
    struct savegame
    {
            int magic_number=1111;
            uint8_t health=4;
            uint8_t max_health=4;
            Def mapname=Def::hero_house;
            Def warpname=Def::hero_house_indoor;
            go_hero::Direction direction=go_hero::Direction::UP;
            Def used_objects[16];
            uint8_t used_objects_count = 0;
            warp current_warp=warp(Def::null,Def::indoor,Def::hero_house_indoor,bn::fixed_point(-400,488),0);
            bool collected_wood=false;
            bool tree_boss_defeated=false;
            bool repaired_bridge=false;
            bool used_object(Def objname);
            void use_object(Def objname);
            bool is_valid();
    };
}

#endif