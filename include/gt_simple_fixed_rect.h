#ifndef GT_SIMPLE_FIXED_RECT_H
#define GT_SIMPLE_FIXED_RECT_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"

namespace gt
{
    class simple_fixed_rect
    {
        public:
            simple_fixed_rect(bn::fixed x, bn::fixed y, bn::fixed width, bn::fixed height);
            simple_fixed_rect(bn::fixed_rect rect);
            simple_fixed_rect();
            bn::fixed x();
            bn::fixed y();
            bn::fixed width();
            bn::fixed height();
            bool intersects(bn::fixed_point pos) const;
            bn::fixed_rect to_bn_fixed_rect();
        private:
            bn::fixed _x;
            bn::fixed _y;
            bn::fixed _width;
            bn::fixed _height;
    };
}



#endif