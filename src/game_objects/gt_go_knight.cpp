#include "gt_go_knight.h"
#include "bn_sprite_items_knight.h"
#include "bn_sprite_builder.h"
#include "gt_globals.h"
#include "bn_sound_items.h"
#include "gt_util.h"
#include "gt_go_smoke_explosion.h"

namespace
{
    [[nodiscard]] bn::sprite_ptr _create_sprite()
    {
        bn::sprite_builder builder(bn::sprite_items::knight);
        return builder.release_build();
    }

    bn::sprite_animate_action<6> _create_animation_stand_down(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 5>{{0, 1, 2, 3, 4}});
    }

    bn::sprite_animate_action<6> _create_animation_stand_up(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 5>{{5, 6, 7, 8, 9}});
    }

    bn::sprite_animate_action<6> _create_animation_stand_right(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 5>{{10, 11, 12, 13, 14}});
    }

    bn::sprite_animate_action<6> _create_animation_walk_down(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 4, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 6>{{15, 16, 17, 18, 19, 20}});
    }

    bn::sprite_animate_action<6> _create_animation_walk_up(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 4, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 6>{{21, 22, 23, 24, 25, 26}});
    }

    bn::sprite_animate_action<6> _create_animation_walk_right(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 4, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 6>{{27, 28, 29, 30, 31, 32}});
    }

    bn::sprite_animate_action<6> _create_animation_attention_down(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::once(sprite, 4, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 5>{{33, 34, 35, 36, 37}});
    }

    bn::sprite_animate_action<6> _create_animation_attention_up(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::once(sprite, 4, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 5>{{38, 39, 40, 41, 42}});
    }

    bn::sprite_animate_action<6> _create_animation_attention_right(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::once(sprite, 4, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 5>{{43, 44, 45, 46, 47}});
    }

    bn::sprite_animate_action<6> _create_animation_walk_down_fighting(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 4, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 6>{{48, 49, 50, 51, 52, 53}});
    }

    bn::sprite_animate_action<6> _create_animation_walk_up_fighting(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 4, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 6>{{54, 55, 56, 57, 58, 59}});
    }

    bn::sprite_animate_action<6> _create_animation_walk_right_fighting(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 4, bn::sprite_items::knight.tiles_item(), bn::array<uint16_t, 6>{{60, 61, 62, 63, 64, 65}});
    }

}

namespace gt
{
    go_knight::go_knight(const bn::camera_ptr& camera):
    _sprite(_create_sprite()),
    _velocity(0,0),
    _action(_create_animation_walk_down(_sprite))
    {
        _state=Movestate::STAND;
        _sprite.set_camera(camera);
        _direction = Direction::DOWN;
        int dirturns=globals::random->get()%4;
        for(int i=0;i<dirturns;i++)
        {
            turn_right();
        }
        _walk_circles_time=globals::random->get()%(10*60)+4*60;
        _local_collision_rect=bn::fixed_rect(0, 10, 10, 10);
        _follow_hero_time=0;
        _health = 3;
        _blinkframes=0;
        _droppable_items.push_back(Def::heart);
    }

    bn::fixed_point go_knight::forward()
    {
        switch(_direction)
        {
            default:
            case Direction::DOWN:return bn::fixed_point(0,1);
            case Direction::UP:return bn::fixed_point(0,-1);
            case Direction::LEFT:return bn::fixed_point(-1,0);
            case Direction::RIGHT:return bn::fixed_point(1,0);
        }
    }

    void go_knight::turn_to_direction(Direction dir)
    {
        if(_direction != dir)
        {
            _direction=dir;
            switch(_direction)
            {
                default:
                case Direction::DOWN:
                {
                    _direction=Direction::DOWN;
                    _action=_create_animation_walk_down(_sprite);
                    _sprite.set_horizontal_flip(false);
                }break;
                case Direction::LEFT:
                {
                    _direction=Direction::LEFT;
                    _action=_create_animation_walk_right(_sprite);
                    _sprite.set_horizontal_flip(true);
                }break;
                case Direction::UP:
                {
                    _direction=Direction::UP;
                    _action=_create_animation_walk_up(_sprite);
                    _sprite.set_horizontal_flip(false);
                }break;
                case Direction::RIGHT:
                {
                    _direction=Direction::RIGHT;
                    _action=_create_animation_walk_right(_sprite);
                    _sprite.set_horizontal_flip(false);
                }break;
            }
        }
    }

    void go_knight::turn_right()
    {
        switch(_direction)
        {
            default:
            case Direction::DOWN:
            {
                turn_to_direction(Direction::LEFT);
            }break;
            case Direction::LEFT:
            {
                turn_to_direction(Direction::UP);
            }break;
            case Direction::UP:
            {
                turn_to_direction(Direction::RIGHT);
            }break;
            case Direction::RIGHT:
            {
                turn_to_direction(Direction::DOWN);
            }break;
        }
        _walk_circles_time=globals::random->get()%(5*60)+2*60;
    }


    bool go_knight::update_out_of_bounds_check()
    {
        simple_fixed_rect area = globals::current_map->area();
        bn::fixed_point pos = _sprite.position();
        if(pos.x()<area.x())
        {
            turn_to_direction(Direction::RIGHT);
            return true;
        }
        if(pos.y()<area.y())
        {
            turn_to_direction(Direction::DOWN);
            return true;
        }
        if(pos.x()>=area.x()+area.width())
        {
            turn_to_direction(Direction::LEFT);
            return true;
        }
        if(pos.y()>=area.y()+area.height())
        {
            turn_to_direction(Direction::UP);
            return true;
        }
        return false;
    }

    void go_knight::update_walk_in_circles()
    {
        update_out_of_bounds_check();
        bn::fixed_point movement = forward()* bn::fixed(0.75f);
        _velocity=move_towards(_velocity,movement,0.1f);
        if(--_walk_circles_time<=0 || move_and_slide())
        {
            turn_right();
        }
        check_can_see_hero();
    }

    void go_knight::update_hero_collision()
    {
        if(_blinkframes>0)
            return;
        
        if(globals::hero->intersects(this))
        {
            if(globals::hero->is_blocking(_sprite.position()))
            {
                bn::sound_items::shield_bounce.play(1);
            }
            else
            {
                globals::hero->do_damage(1);
            }
            bn::fixed_point hero_to_me = _sprite.position() - (*(globals::hero->get_pos()));
            _velocity=normalize(hero_to_me);
            globals::hero->velocity=_velocity*-4;
                
            
            
        }
    }

    void go_knight::update_see_hero()
    {
        if(_action.done())
        {
            _follow_hero_time=globals::random->get()%(60*4)+(60*3);
            _state=Movestate::FOLLOW_HERO;
            _direction=Direction::NONE;
        }
    }

    void go_knight::update_follow_hero()
    {
        bn::fixed_point me_to_hero = (*(globals::hero->get_pos())) - _sprite.position();
        Direction new_direction = position_to_direction(me_to_hero);
        if(_direction != new_direction)
        {
            _direction=new_direction;
            switch(_direction)
            {
                default:
                case Direction::DOWN:
                {
                    _sprite.set_horizontal_flip(false);
                    _action=_create_animation_walk_down_fighting(_sprite);
                }break;
                case Direction::LEFT:
                {
                    _sprite.set_horizontal_flip(true);
                    _action=_create_animation_walk_right_fighting(_sprite);
                }break;
                case Direction::UP:
                {
                    _sprite.set_horizontal_flip(false);
                    _action=_create_animation_walk_up_fighting(_sprite);
                }break;
                case Direction::RIGHT:
                {
                    _sprite.set_horizontal_flip(false);
                    _action=_create_animation_walk_right_fighting(_sprite);
                }break;
            }
        }
        bn::fixed_point movement = normalize(me_to_hero)* bn::fixed(0.9);
        _velocity=move_towards(_velocity,movement,0.1f);
        move_and_slide();
        if(--_follow_hero_time<=0)
        {
            _state=Movestate::WALK_IN_CIRCLES;
            turn_right();
        }
    }

    void go_knight::update()
    {
        switch(_state)
        {
            default:
            case Movestate::WALK_IN_CIRCLES:
            {
                update_walk_in_circles();
            }break;
            case Movestate::SEE_HERO:
            {
                update_see_hero();
            }break;
            case Movestate::FOLLOW_HERO:
            {
                update_follow_hero();
            }
        }
        update_hero_collision();

        if(_blinkframes>0)
        {
            _blinkframes--;
            if(_blinkframes==0)
            {
                _sprite.set_visible(true);
            }
            else
            {
                _sprite.set_visible((_blinkframes/2)%2);
            }
        }

        if(!_action.done())
            _action.update();
        
        _sprite.set_z_order(-_sprite.y().right_shift_integer());
    }

    void go_knight::set_pos(bn::fixed_point pos)
    {
        _sprite.set_position(pos);
    }

    void go_knight::_die()
    {
        if(!globals::go_manager->is_full())
            globals::go_manager->add_object(new go_smoke_explosion(*(globals::camera),_sprite.position()));
        globals::go_manager->destroy_object(this);
    }

    void go_knight::do_damage(int amount)
    {
        if(_blinkframes>0)
            return;
        go_enemy::do_damage(amount);
        if(_health>0)
        {
            bn::fixed_point hero_to_me = _sprite.position() - (*(globals::hero->get_pos()));
            _velocity=normalize(hero_to_me)*3;
            _blinkframes=30;
            //bn::sound_items::bat.play(1);
            /*if(!globals::go_manager->is_full())
                globals::go_manager->add_object(new go_blood_splash(*(globals::camera),_sprite.position()));*/
        }
    }

    bn::optional<bn::fixed_point> go_knight::get_pos()
    {
        return bn::optional(_sprite.position());
    }

    bool go_knight::move_and_slide()
    {
        bool collided=false;
        bn::fixed_point testPos=_sprite.position()+bn::fixed_point(_velocity.x(),0);
        if(!check_map_collision(testPos))
            _sprite.set_x(testPos.x());
        else
        {
            _velocity.set_x(0);
            collided=true;
        }
        
        testPos=_sprite.position()+bn::fixed_point(0,_velocity.y());
        if(!check_map_collision(testPos))
            _sprite.set_y(testPos.y());
        else
        {
            _velocity.set_y(0);
            collided=true;
        }
        return collided;
    }

    bool go_knight::check_can_see_hero()
    {
        bn::fixed see_distance=75;
        bn::fixed_point center=_sprite.position() + (forward()*(see_distance/2));
        bn::fixed_size extends=bn::fixed_size(see_distance,see_distance);
        bn::fixed_rect view_rect=bn::fixed_rect(center,extends);
        bool can_see_hero = view_rect.contains(*globals::hero->get_pos());
        if(can_see_hero)
        {
            switch(_direction)
            {
                default:
                case Direction::DOWN:
                {
                    _sprite.set_horizontal_flip(false);
                    _action=_create_animation_attention_down(_sprite);
                }break;
                case Direction::LEFT:
                {
                    _sprite.set_horizontal_flip(true);
                    _action=_create_animation_attention_right(_sprite);
                }break;
                case Direction::UP:
                {
                    _sprite.set_horizontal_flip(false);
                    _action=_create_animation_attention_up(_sprite);
                }break;
                case Direction::RIGHT:
                {
                    _sprite.set_horizontal_flip(false);
                    _action=_create_animation_attention_right(_sprite);
                }break;
            }
            _state=Movestate::SEE_HERO;
        }
        return can_see_hero;
    }

    go_knight::Direction go_knight::position_to_direction(bn::fixed_point point)
    {
        if(bn::abs(point.x())>bn::abs(point.y()))
        {
            if(point.x()>bn::fixed(0))
                return Direction::RIGHT;
            else
                return Direction::LEFT;
        }
        else
        {
            if(point.y()>bn::fixed(0))
                return Direction::DOWN;
            else
                return Direction::UP;
        }
    }

}