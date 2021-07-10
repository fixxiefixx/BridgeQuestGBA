#include "gt_map_info.h"

namespace gt
{
    map_info::map_info(Def _name,bn::regular_bg_item _bg_item, const uint8_t* _mapobjects_data,
                const simple_fixed_rect *_area_data, uint16_t _area_count,
                const sign *_sign_data, uint16_t _sign_count, const warp *_warps, uint16_t _warp_count,
                const objinfo *_objinfos, uint16_t _objinfo_count):
                bg_item(_bg_item)
    {
        name=_name;
        mapobjects_data=_mapobjects_data;
        area_data=_area_data;
        area_count=_area_count;
        sign_data=_sign_data;
        sign_count=_sign_count;
        warps=_warps;
        warp_count=_warp_count;
        objinfos=_objinfos;
        objinfo_count=_objinfo_count;
    }
}