#include "gt_sign.h"

namespace gt
{
    sign::sign(simple_fixed_rect rect, bn::string_view text)
    {
        _rect=rect;
        _text=text;
    }

    simple_fixed_rect sign::rect()
    {
        return _rect;
    }

    bn::string_view sign::text()
    {
        return _text;
    }
}