#ifndef GT_GO_HERO_H
#define GT_GO_HERO_H

#include "bn_sprite_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_rect.h"
#include "gt_game_object.h"

namespace gt
{
    class go_hero : public game_object
    {
        public:
            enum class Direction
            {
                DOWN,
                LEFT,
                UP,
                RIGHT
            };
            go_hero(const bn::camera_ptr& camera);
            virtual void update() override;
            virtual bn::optional<bn::fixed_point> get_pos() override;
            void set_pos(bn::fixed_point pos);
            Direction direction();
            void set_direction(Direction dir);
            bn::fixed_point forward();
            virtual void do_damage(int amount) override;
            bool is_position_blockable(bn::fixed_point pos);
            bool is_blocking(bn::fixed_point pos);
            bn::fixed_point velocity;
            void set_blending_enabled(bool value);
            bool blending_enabled();
            bool is_vulnerable();
            void collect_item(Def item);

        private:
            

            enum class Movestate
            {
                STAND,
                WALK,
                ATTACK,
                DAMAGED
            };

            bn::sprite_ptr _hero_sprite;
            bn::sprite_animate_action<6> _action;
            void update_stand_walk_state();
            void update_attack_state();
            void update_damaged_state();
            bool check_sign();
            bool check_interactive();
            void attack();
            void move_and_slide();
            bn::fixed_rect get_sword_rect();
            Direction position_to_direction(bn::fixed_point point);

            
            Direction _current_direction;
            Movestate _current_state;
            bool _movement_action_needs_rebuild;
            bool _holding_shield;
            uint8_t _blinkframes;
            
    };
}

#endif