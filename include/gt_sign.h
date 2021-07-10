#ifndef GT_SIGN_H
#define GT_SIGN_H

#include "gt_simple_fixed_rect.h"
#include "bn_string_view.h"

namespace gt
{
    class sign
    {
        public:
            sign(simple_fixed_rect rect, bn::string_view text);
            simple_fixed_rect rect();
            bn::string_view text();
        private:
            simple_fixed_rect _rect;
            bn::string_view _text;
    };
}

#endif