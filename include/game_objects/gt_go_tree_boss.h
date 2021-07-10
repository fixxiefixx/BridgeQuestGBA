#ifndef GT_GO_TREE_BOSS_H
#define GT_GO_TREE_BOSS_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_core.h"
#include "bn_fixed_point.h"
#include "gt_game_object.h"
#include "gt_go_enemy.h"

namespace gt
{
    class go_tree_boss : public go_enemy
    {
        public:
            go_tree_boss(bn::camera_ptr& camera, bn::fixed_point position);
            virtual void update() override;
            virtual bn::optional<bn::fixed_point> get_pos() override;
            virtual void do_damage(int amount) override;
        protected:
             virtual void _die() override;
        private:
            enum class Movestate{
                disabled,
                idle,
                follow_player,
                go_to_start,
                fire_bullets,
                dying
            };
            Movestate _movestate;
            int _idle_time;
            bn::sprite_ptr _sprite;
            bn::fixed_point _velocity;
            bn::sprite_animate_action<2> _action;
            int _blinkframes;
            int _dying_counter;
            bn::fixed_point _start_pos;
            bool _angry;
            int _bullets_fired;
            void update_disabled();
            void update_idle();
            void update_follow_player();
            void update_go_to_start();
            void update_hero_collision();
            void throw_bullet(bn::fixed_point pos, bn::fixed_point velocity);
            void update_fire_bullets();
            void update_dying();
            void spawn_minions();
    };
}

#endif