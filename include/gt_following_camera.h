#ifndef GT_FOLLOWING_CAMERA_H
#define GT_FOLLOWING_CAMERA_H

#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_rect.h"
#include "gt_game_object.h"

namespace gt
{
    class following_camera
    {
        public:
            following_camera(bn::camera_ptr camera);
            void set_limits(bn::fixed_rect limits);
            void set_target(game_object *target);
            void update();
            bn::fixed_point apply_limits(bn::fixed_point pos);
        private:
            bn::camera_ptr _camera;
            game_object *_target;
            bn::fixed_rect _limits;
    };
}

#endif