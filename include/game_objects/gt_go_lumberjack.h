#ifndef GT_GO_LUMBERJACK_H
#define GT_GO_LUMBERJACK_H

#include "gt_game_object.h"
#include "bn_fixed_point.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_ptr.h"

namespace gt
{
    class go_lumberjack : public game_object
    {
        public:
            go_lumberjack(bn::camera_ptr& camera, bn::fixed_point position);
            virtual bn::optional<bn::fixed_point> get_pos() override;
            virtual void interact() override;
            virtual void update() override;
        private:
            bn::sprite_ptr _sprite;
    };
}

#endif