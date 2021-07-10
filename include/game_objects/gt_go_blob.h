#ifndef GT_GO_BLOB_H
#define GT_GO_BLOB_H

#include "bn_sprite_actions.h"
#include "gt_game_object.h"

namespace gt
{
    class go_blob : public game_object
    {
        public:
            go_blob(const bn::camera_ptr& camera);
            virtual void update() override;

        private:
            bn::sprite_ptr blob_sprite;
            int test=0;
            int test2=0;
    };
}

#endif