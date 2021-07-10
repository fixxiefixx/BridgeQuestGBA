#ifndef GT_MAP_H
#define GT_MAP_H

#include "bn_core.h"
#include "bn_point.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_rect.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_camera_ptr.h"
#include "gt_simple_fixed_rect.h"
#include "gt_sign.h"
#include "gt_map_info.h"

namespace gt
{
    class map
    {
        public:
            map(bn::camera_ptr camera, map_info m_info);
            uint8_t mapobject_at(bn::point tile_coord);
            bn::point position_to_tile_coord(bn::fixed_point pos);
            uint8_t position_to_mapobject(bn::fixed_point pos);
            bool check_rect_collision(bn::fixed_rect rect);
            void update();
            simple_fixed_rect area();
            bn::regular_bg_ptr bg();
            bn::optional<sign> get_sign_at(bn::fixed_point pos);
            bn::optional<warp> get_nearest_warp_at(bn::fixed_point pos);

        private:
            map_info _m_info;
            int _column_count;
            int _row_count;
            bn::point _mapobjects_origin_offset;
            bn::regular_bg_ptr _bg;
            simple_fixed_rect _current_area;

            bn::optional<simple_fixed_rect> get_area_at(bn::fixed_point pos);
            void update_hero_area(bn::fixed_point hero_pos);
            void apply_camera_bounds(bn::fixed_rect area);
            void activate_area(simple_fixed_rect area);
            void do_camera_transition(bn::fixed_point oldPos, bn::fixed_point newPos);
            void spawn_area_objects(simple_fixed_rect area);
    };
}

#endif