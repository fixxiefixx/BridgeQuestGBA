#ifndef GT_GO_COLLECTABLE_H
#define GT_GO_COLLECTABLE_H

#include "gt_game_object.h"
#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"

namespace gt
{
    class go_collectable : public game_object
    {
        public:
            go_collectable(const bn::camera_ptr& camera, bn::fixed_point pos, Def item);
            virtual void update() override;
            virtual bn::optional<bn::fixed_point> get_pos() override;
        private:
            bn::sprite_ptr _sprite;
            bn::sprite_ptr _shadow_sprite;
            Def _item;
            int _time=0;
            bn::fixed _down_speed;
            bn::fixed _down_pos;
            void update_hero_collision();
    };
}

#endif