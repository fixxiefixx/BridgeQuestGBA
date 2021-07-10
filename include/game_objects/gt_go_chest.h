#ifndef GT_GO_CHEST_H
#define GT_GO_CHEST_H

#include "gt_game_object.h"
#include "bn_fixed_point.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_ptr.h"
#include "definitions.h"

namespace gt
{
    class go_chest : public game_object
    {
        public:
            go_chest(bn::camera_ptr& camera, bn::fixed_point position, Def name, Def item);
            virtual bn::optional<bn::fixed_point> get_pos() override;
            virtual void interact() override;
            virtual void update() override;
        
        private:
            bn::sprite_ptr _sprite;
            bool _open=false;
            Def _name;
            Def _item;
    };
}

#endif