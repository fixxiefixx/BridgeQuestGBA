#ifndef GT_GO_BAT_H
#define GT_GO_BAT_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_core.h"
#include "bn_fixed_point.h"
#include "gt_game_object.h"
#include "gt_go_enemy.h"


namespace gt
{
    class go_bat : public go_enemy
    {
        public:
            go_bat(const bn::camera_ptr& camera);
            virtual void update() override;
            virtual bn::optional<bn::fixed_point> get_pos() override;
            virtual void do_damage(int amount) override;
            void set_pos(bn::fixed_point pos);
        protected:
            virtual void _die() override;
        private:
            enum class Movestate
            {
                CIRCLES,
                ATTACK,
                FLEE
            };
            void update_circles();
            void update_hero_collision();
            void update_attack();
            bn::sprite_ptr _sprite;
            bn::fixed_point _velocity;
            bn::sprite_animate_action<6> _action;
            bn::fixed _fly_angle;
            bool _fly_direction;
            uint16_t _fly_time;
            Movestate _state;
            uint8_t _blinkframes;
    };

}

#endif