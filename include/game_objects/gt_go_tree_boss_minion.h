#ifndef GT_GO_TREE_BOSS_MINION
#define GT_GO_TREE_BOSS_MINION

#include "gt_go_enemy.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "gt_go_tree_boss.h"

namespace gt
{
    class go_tree_boss_minion : public go_enemy
    {
        public:
            go_tree_boss_minion(const bn::camera_ptr& camera, bn::fixed_point pos, go_tree_boss* boss, bn::fixed direction);
            virtual void update() override;
            virtual bn::optional<bn::fixed_point> get_pos() override;
            virtual void do_damage(int amount) override;
            virtual Def type() override;
            void follow_player();
        protected:
            virtual void _die() override;
        private:
            enum class Movestate
            {
                mimic_wood,
                protect_boss,
                follow_player
            };
            bn::sprite_ptr _sprite;
            bn::sprite_animate_action<4> _action;
            Movestate _movestate=Movestate::protect_boss;
            bn::fixed _protect_boss_direction=0;
            go_tree_boss* _boss;
            int _blinkframes=0;
            bn::fixed_point _velocity;
            void update_hero_collision();
            void update_mimic_wood();
            void update_protect_boss();
            void update_follow_player();
    };
}

#endif