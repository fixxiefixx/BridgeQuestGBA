#ifndef GT_GO_FIRE_BULLET_H
#define GT_GO_FIRE_BULLET_H

#include "gt_game_object.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"

namespace gt
{
    class go_fire_bullet : public game_object
    {
        public:
            go_fire_bullet(bn::camera_ptr& camera, bn::fixed_point position, bn::fixed_point velocity);
            virtual void update() override;
        private:
            bn::sprite_ptr _sprite;
            bn::sprite_animate_action<4> _action;
            bn::fixed_point _velocity;
            int _time=0;
            void update_hero_collision();
    };
}

#endif