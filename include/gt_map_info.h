#ifndef GT_MAP_INFO_H
#define GT_MAP_INFO_H

#include "bn_regular_bg_item.h"
#include "gt_simple_fixed_rect.h"
#include "gt_sign.h"
#include "gt_warp.h"
#include "gt_objinfo.h"
#include "definitions.h"

namespace gt
{
    class map_info
    {
        public:
            Def name;
            bn::regular_bg_item bg_item;
            const uint8_t* mapobjects_data;
            const simple_fixed_rect *area_data;
            uint16_t area_count;
            const sign *sign_data;
            uint16_t sign_count;
            const warp *warps;
            uint16_t warp_count;
            const objinfo *objinfos;
            uint16_t objinfo_count;

            map_info(Def _name,bn::regular_bg_item _bg_item, const uint8_t* _mapobjects_data,
                const simple_fixed_rect *_area_data, uint16_t _area_count,
                const sign *_sign_data, uint16_t _sign_count, const warp *_warps, uint16_t _warp_count,
                const objinfo *_objinfos, uint16_t _objinfo_count);
    };
}

#endif