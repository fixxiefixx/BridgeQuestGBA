#include "gt_simple_fixed_rect.h"

namespace gt
{
    simple_fixed_rect::simple_fixed_rect(bn::fixed x, bn::fixed y, bn::fixed width, bn::fixed height):
    _x(x),
    _y(y),
    _width(width),
    _height(height)
    {

    }

    simple_fixed_rect::simple_fixed_rect(bn::fixed_rect rect):
    _x(rect.left()),
    _y(rect.top()),
    _width(rect.width()),
    _height(rect.height())
    {

    }

    simple_fixed_rect::simple_fixed_rect():
    _x(0),
    _y(0),
    _width(0),
    _height(0)
    {

    }

    bn::fixed simple_fixed_rect::x()
    {
        return _x;
    }

    bn::fixed simple_fixed_rect::y()
    {
        return _y;
    }

    bn::fixed simple_fixed_rect::width()
    {
        return _width;
    }

    bn::fixed simple_fixed_rect::height()
    {
        return _height;
    }

    bool simple_fixed_rect::intersects(bn::fixed_point pos) const
    {
        if(pos.x()<_x)
            return false;
        if(pos.y()<_y)
            return false;
        if(pos.x()>=_x+_width)
            return false;
        if(pos.y()>=_y+_height)
            return false;
        return true;
    }

    bn::fixed_rect simple_fixed_rect::to_bn_fixed_rect()
    {
        return bn::fixed_rect(_x + _width / 2, _y + _height / 2, _width, _height);
    }

}