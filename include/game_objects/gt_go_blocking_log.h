#ifndef GT_GO_BLOCKING_LOG_H
#define GT_GO_BLOCKING_LOG_H

#include "gt_game_object.h"
#include "bn_fixed_point.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_ptr.h"

namespace gt
{
    class go_blocking_log : public game_object
    {
        public:
            go_blocking_log(bn::camera_ptr& camera, bn::fixed_point position, Def type);
            virtual bn::optional<bn::fixed_point> get_pos() override;
            virtual void update() override;
            void set_enabled(bool enabled);
            bool enabled();
            virtual Def type() override;
        private:
            bn::sprite_ptr _sprite;
            Def _type;
            bool _vertical;
    };
}

#endif