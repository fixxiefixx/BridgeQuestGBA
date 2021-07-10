#include "gt_warp.h"

namespace gt
{
    warp::warp(Def _name, Def _dest_map, Def _dest_warp, bn::fixed_point _position, uint8_t _dest_rotation)
    {
        name=_name;
        dest_map=_dest_map;
        dest_warp=_dest_warp;
        position=_position;
        dest_rotation=_dest_rotation;
    }
}