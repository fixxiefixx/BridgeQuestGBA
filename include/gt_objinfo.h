#ifndef GT_OBJINFO_H
#define GT_OBJINFO_H

#include "definitions.h"
#include "bn_fixed_rect.h"

namespace gt
{
    class objinfo
    {
        public:
            objinfo(bn::fixed_rect _rect, Def _name, Def _typ, Def _item);
            bn::fixed_rect rect;
            Def name;
            Def type;
            Def item;

    };
}

#endif