#ifndef GT_GO_BED_H
#define GT_GO_BED_H

#include "bn_fixed_rect.h"
#include "gt_game_object.h"

namespace gt
{
    class go_bed : public game_object
    {
        public:
            go_bed(bn::fixed_rect rect);
            virtual bn::optional<bn::fixed_point> get_pos() override;
            virtual void interact() override;
        private:
            bn::fixed_rect _rect;
    };
}

#endif