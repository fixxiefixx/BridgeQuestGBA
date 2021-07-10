#ifndef GT_GO_SMOKE_EXPLOSION_H
#define GT_GO_SMOKE_EXPLOSION_H

#include "bn_sprite_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_camera_ptr.h"
#include "gt_game_object.h"

namespace gt
{
    class go_smoke_explosion : public game_object
    {
        public:
            go_smoke_explosion(const bn::camera_ptr& camera, bn::fixed_point pos);
            virtual void update() override;
        private:
            bn::sprite_ptr _sprite;
            bn::sprite_animate_action<5> _action;
    };
}

#endif