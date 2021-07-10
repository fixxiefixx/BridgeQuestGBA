#ifndef GT_WARP_H
#define GT_WARP_H

#include "bn_core.h"
#include "bn_fixed_point.h"
#include "definitions.h"

namespace gt
{
    class warp
    {
        public:
            Def name;
            Def dest_map;
            Def dest_warp;
            bn::fixed_point position;
            uint8_t dest_rotation;
            warp(Def _name, Def _dest_map, Def _dest_warp, bn::fixed_point _position, uint8_t _dest_rotation);
    };
}

#endif