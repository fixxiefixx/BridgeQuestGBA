#ifndef GT_GO_KNIGHT_H
#define GT_GO_KNIGHT_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_core.h"
#include "bn_fixed_point.h"
#include "gt_game_object.h"
#include "gt_go_enemy.h"

namespace gt
{
    class go_knight : public go_enemy
    {
        public:
            go_knight(const bn::camera_ptr& camera);
            virtual void update() override;
            void set_pos(bn::fixed_point pos);
            virtual bn::optional<bn::fixed_point> get_pos() override;
            virtual void do_damage(int amount) override;
        protected:
            virtual void _die() override;
        private:
            enum class Movestate
            {
                STAND,
                WALK_IN_CIRCLES,
                SEE_HERO,
                FOLLOW_HERO,
                ATTACK
            };
            enum class Direction
            {
                NONE,
                DOWN,
                LEFT,
                UP,
                RIGHT
            };
            bn::sprite_ptr _sprite;
            bn::fixed_point _velocity;
            bn::sprite_animate_action<6> _action;
            Movestate _state;
            Direction _direction;
            int _follow_hero_time;
            int _walk_circles_time;
            int _blinkframes;
            void update_walk_in_circles();
            bn::fixed_point forward();
            void turn_right();
            void update_hero_collision();
            bool update_out_of_bounds_check();
            void turn_to_direction(Direction dir);
            bool move_and_slide();
            void update_follow_hero();
            void update_see_hero();
            bool check_can_see_hero();
            Direction position_to_direction(bn::fixed_point point);
    };
}


#endif