#include "gt_go_woodmimic.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_items_tree_boss_minion.h"
#include "gt_globals.h"
#include "bn_sound_items.h"
#include "bn_math.h"
#include "gt_util.h"
#include "gt_go_smoke_explosion.h"
#include "bn_core.h"

#define __ go_woodmimic

namespace
{
    [[nodiscard]] bn::sprite_ptr _create_sprite(bn::fixed_point pos, bn::camera_ptr camera)
    {
        bn::sprite_builder builder(bn::sprite_items::tree_boss_minion);
        builder.set_position(pos);
        builder.set_camera(camera);
        builder.set_blending_enabled(true);
        return builder.release_build();
    }

    bn::sprite_animate_action<4> _create_animation_mimicwood(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<4>::once(sprite, 4, bn::sprite_items::tree_boss_minion.tiles_item(), bn::array<uint16_t, 2>{{0, 0}});
    }

    bn::sprite_animate_action<4> _create_animation_waking(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<4>::forever(sprite, 2, bn::sprite_items::tree_boss_minion.tiles_item(), bn::array<uint16_t, 4>{{1, 2, 3, 4}});
    }

    bn::sprite_animate_action<4> _create_animation_idle(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<4>::forever(sprite, 4, bn::sprite_items::tree_boss_minion.tiles_item(), bn::array<uint16_t, 4>{{5, 6, 7, 8}});
    }
}

namespace gt
{
    __::__(const bn::camera_ptr& camera, bn::fixed_point pos):
    _sprite(_create_sprite(pos,camera)),
    _action(_create_animation_mimicwood(_sprite)),
    _velocity(0,0)
    {
        _movestate=Movestate::mimicwood;
        _local_collision_rect=bn::fixed_rect(0,0,10,10);
        _health=2;
        _droppable_items.push_back(Def::heart);
    }

    void __::update_hero_collision()
    {
        if(globals::hero->is_vulnerable() && globals::hero->intersects(this))
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
            bn::fixed_point normalized=normalize(hero_to_me);
            globals::hero->velocity=normalized * -4;
            _velocity=normalized*2;
        }
    }

    void __::update_mimicwood()
    {
        bn::fixed_point me_to_hero = *globals::hero->get_pos() - _sprite.position();
        if(aprox_length(me_to_hero) < bn::fixed(64))
        {
            _movestate=Movestate::waking;
            _action=_create_animation_waking(_sprite);
            _idle_time=30;
        }
    }

    void __::update_waking()
    {
        if(_idle_time--<=0)
        {
            _idle_time=30;
            _movestate=Movestate::idle;
            _action=_create_animation_idle(_sprite);
        }
    }

    void __::update_idle()
    {
        if(_idle_time--<=0)
        {
            _movestate=Movestate::follow_player;
        }
    }

    void __::update_follow_player()
    {
        bn::fixed_point me_to_hero = *globals::hero->get_pos() - _sprite.position();
        bn::fixed_point target_velocity=normalize(me_to_hero)*bn::fixed(0.5);
        _velocity=move_towards(_velocity,target_velocity,0.2);
        move_and_slide();
    }


    void __::update()
    {
        switch(_movestate)
        {
            default:
            case Movestate::mimicwood:
            {
                update_mimicwood();
            }break;
            case Movestate::waking:
            {
                update_waking();
            }break;
            case Movestate::idle:
            {
                update_idle();
                update_hero_collision();
            }break;
            case Movestate::follow_player:
            {
                update_follow_player();
                update_hero_collision();
            }break;
        }

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

    bool __::move_and_slide()
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

    bn::optional<bn::fixed_point> __::get_pos()
    {
        return bn::optional<bn::fixed_point>(_sprite.position());
    }

    void __::do_damage(int amount)
    {
        if(_blinkframes>0)
            return;
        go_enemy::do_damage(amount);
        if(_health>0)
        {
            _blinkframes=30;
            bn::fixed_point hero_to_me = _sprite.position() - *globals::hero->get_pos();
            _velocity=normalize(hero_to_me)*2;
        }
    }

    void __::_die()
    {
        globals::go_manager->add_object(new go_smoke_explosion(*(globals::camera),_sprite.position()));
        globals::go_manager->destroy_object(this);
    }

    Def __::type()
    {
        return Def::tree_boss_minion;
    }

}