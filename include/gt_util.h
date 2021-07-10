#ifndef GT_UTIL_H
#define GT_UTIL_H

#include "bn_fixed_point.h"
#include "bn_math.h"
#include "bn_string_view.h"
#include "definitions.h"

namespace gt
{
    extern bn::fixed dot_product(bn::fixed_point a, bn::fixed_point b);

    extern bn::fixed length_sqr(bn::fixed_point a);

    extern bn::fixed length(bn::fixed_point a);

    extern bn::fixed aprox_length(bn::fixed_point a);

    extern bn::fixed_point normalize(bn::fixed_point a);

    extern bn::fixed_point move_towards(bn::fixed_point current, bn::fixed_point target, bn::fixed maxDistanceDelta);

    extern bn::string_view itemdef_to_stringview(Def item);
}

#endif