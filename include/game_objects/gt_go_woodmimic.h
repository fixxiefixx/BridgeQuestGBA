#ifndef GT_GO_WOODMIMIC_H
#define GT_GO_WOODMIMIC_H

#include "gt_go_enemy.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"

namespace gt
{
    class go_woodmimic : public go_enemy
    {
        public:
            go_woodmimic(const bn::camera_ptr& camera, bn::fixed_point pos);
            virtual void update() override;
            virtual bn::optional<bn::fixed_point> get_pos() override;
            virtual void do_damage(int amount) override;
            virtual Def type() override;
        protected:
            virtual void _die() override;
        private:
            bn::sprite_ptr _sprite;
            bn::sprite_animate_action<4> _action;
            enum class Movestate{
                mimicwood,
                waking,
                idle,
                follow_player
            };
            Movestate _movestate;
            bn::fixed_point _velocity;
            int _blinkframes=0;
            int _idle_time=0;
            void update_hero_collision();
            void update_mimicwood();
            void update_waking();
            void update_idle();
            void update_follow_player();
            bool move_and_slide();
    };
}

#endif