#include "gt_go_bat.h"
#include "bn_sprite_items_bat.h"
#include "bn_sprite_builder.h"
#include "gt_globals.h"
#include "bn_math.h"
#include "gt_util.h"
#include "gt_go_smoke_explosion.h"
#include "bn_sound_items.h"

namespace
{
    [[nodiscard]] bn::sprite_ptr _create_sprite()
    {
        bn::sprite_builder builder(bn::sprite_items::bat);
        return builder.release_build();
    }

    bn::sprite_animate_action<6> _create_animation_fly(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 4, bn::sprite_items::bat.tiles_item(), bn::array<uint16_t, 2>{{0, 1}});
    }

}

namespace gt
{
    go_bat::go_bat(const bn::camera_ptr& camera):
    _sprite(_create_sprite()),
    _velocity(0,0),
    _action(_create_animation_fly(_sprite))
    {
        _sprite.set_z_order(-1000);
        _local_collision_rect=bn::fixed_rect(0, 0, 10, 10);
        _state=Movestate::CIRCLES;
        _sprite.set_camera(camera);
        _fly_angle=0;
        _fly_direction=false;
        _fly_time=globals::random->get() %50 + 30;
        _health = 2;
        _blinkframes=0;
        _droppable_items.push_back(Def::heart);
    }

    void go_bat::update_hero_collision()
    {
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
           globals::hero->velocity=normalize(_velocity)*4;
                
            bn::fixed_point hero_to_me = _sprite.position() - (*(globals::hero->get_pos()));
            _velocity=normalize(hero_to_me)*2;
            
        }
    }



    void go_bat::update_circles()
    {
        _fly_angle+=_fly_direction?0.01f:-0.01f;
        bn::fixed speed=bn::fixed(1);
        bn::fixed_point target_velocity=bn::fixed_point(bn::sin(_fly_angle)* speed,bn::cos(_fly_angle)*speed);
        _velocity=move_towards(_velocity,target_velocity,0.2f);
        _sprite.set_position(_sprite.position()+_velocity);

        if(_fly_time--==0)
        {
            _fly_time=globals::random->get() %20 + 120;
            _state=Movestate::ATTACK;
            bn::sound_items::bat.play(1);
        }
    }

    void go_bat::update_attack()
    {
        bn::fixed speed=bn::fixed(1.5);
        bn::fixed_point me_to_hero=(*globals::hero->get_pos()) - _sprite.position();
        bn::fixed_point target_velocity = normalize(me_to_hero)*speed;
        _velocity=move_towards(_velocity,target_velocity,0.07f);
        _sprite.set_position(_sprite.position()+_velocity);

        if(_fly_time--==0)
        {
            _fly_time=globals::random->get() %240 + 120;
            _fly_direction = globals::random->get()%2;
            _state=Movestate::CIRCLES;
        }
    }

    void go_bat::update()
    {
        switch(_state)
        {
            default:
            case Movestate::CIRCLES:
            {
                update_circles();
            }break;
            case Movestate::ATTACK:
            {
                update_attack();
            }break;
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
    }

    void go_bat::_die()
    {
        if(!globals::go_manager->is_full())
            globals::go_manager->add_object(new go_smoke_explosion(*(globals::camera),_sprite.position()));
        globals::go_manager->destroy_object(this);
    }

    bn::optional<bn::fixed_point> go_bat::get_pos()
    {
        return bn::optional(_sprite.position());
    }

    void go_bat::do_damage(int amount)
    {
        if(_blinkframes>0)
            return;
        go_enemy::do_damage(amount);
        if(_health>0)
        {
            bn::fixed_point hero_to_me = _sprite.position() - (*(globals::hero->get_pos()));
            _velocity=normalize(hero_to_me)*3;
            _blinkframes=30;
            bn::sound_items::bat.play(1);
            /*if(!globals::go_manager->is_full())
                globals::go_manager->add_object(new go_blood_splash(*(globals::camera),_sprite.position()));*/
        }
    }

    void go_bat::set_pos(bn::fixed_point pos)
    {
        _sprite.set_position(pos);
    }

}