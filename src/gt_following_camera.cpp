#include "gt_following_camera.h"
#include "bn_math.h"

namespace gt
{
    following_camera::following_camera(bn::camera_ptr camera):
    _camera(camera),
    _limits(bn::point(-5000,-5000),bn::size(10000,10000))
    {

    }

    void following_camera::set_limits(bn::fixed_rect limits)
    {
        _limits = limits;
    }

    void following_camera::set_target(game_object *target)
    {
        _target = target;
    }

    bn::fixed_point following_camera::apply_limits(bn::fixed_point pos)
    {
        if(_limits.width()<=0)
        {
            pos.set_x(_limits.x());
        }
        else
        {
            pos.set_x(bn::max(pos.x(), bn::fixed(_limits.left())));
            pos.set_x(bn::min(pos.x(), bn::fixed(_limits.left()+_limits.width()-bn::fixed(1))));
        }
        if(_limits.height()<=0)
        {
            pos.set_y(_limits.y());
        }
        else
        {
            pos.set_y(bn::max(pos.y(), bn::fixed(_limits.top())));
            pos.set_y(bn::min(pos.y(), bn::fixed(_limits.top()+_limits.height()-bn::fixed(1))));
        }
        return pos;
    }

    void following_camera::update()
    {
        if(_target != nullptr)
        {
            bn::optional<bn::fixed_point> target_pos=_target->get_pos();
            if(target_pos)
            {
                bn::fixed_point pos = apply_limits(*target_pos);
                
                _camera.set_position(pos);
            }
        }
    }
}