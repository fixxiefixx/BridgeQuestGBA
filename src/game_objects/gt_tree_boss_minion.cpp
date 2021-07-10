#include "gt_go_tree_boss_minion.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_items_tree_boss_minion.h"
#include "gt_globals.h"
#include "bn_sound_items.h"
#include "bn_math.h"
#include "gt_util.h"
#include "gt_go_smoke_explosion.h"
#include "bn_core.h"


#define __ go_tree_boss_minion

namespace
{

    [[nodiscard]] bn::sprite_ptr _create_sprite(bn::fixed_point pos, bn::camera_ptr camera)
    {
        bn::sprite_builder builder(bn::sprite_items::tree_boss_minion);
        builder.set_position(pos);
        builder.set_camera(camera);
        return builder.release_build();
    }

    bn::sprite_animate_action<4> _create_animation_idle(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<4>::forever(sprite, 4, bn::sprite_items::tree_boss_minion.tiles_item(), bn::array<uint16_t, 4>{{5, 6,7,8}});
    }

}

namespace gt
{
    __::__(const bn::camera_ptr& camera, bn::fixed_point pos, go_tree_boss* boss, bn::fixed direction):
    _sprite(_create_sprite(pos,camera)),
    _action(_create_animation_idle(_sprite)),
    _velocity(0,0)
    {
        _health=2;
        _boss=boss;
        _protect_boss_direction=direction;
        _local_collision_rect=bn::fixed_rect(0,0,8,8);
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

    void __::update_mimic_wood()
    {

    }

    void __::update_protect_boss()
    {
        _protect_boss_direction = _protect_boss_direction + bn::fixed(1.5);
        bn::fixed_point target = (*_boss->get_pos()) + bn::fixed_point(degrees_sin(_protect_boss_direction),degrees_cos(_protect_boss_direction)) * bn::fixed(35);
        _sprite.set_position(move_towards(_sprite.position(),target,1.25));
    }

    void __::update_follow_player()
    {
        bn::fixed_point me_to_hero = *globals::hero->get_pos() - _sprite.position();
        bn::fixed_point target_velocity=normalize(me_to_hero)*bn::fixed(0.5);
        _velocity=move_towards(_velocity,target_velocity,0.1);
        _sprite.set_position(_sprite.position()+_velocity);
    }

    void __::update()
    {
        switch(_movestate)
        {
            default:
            case Movestate::mimic_wood:
            {
                update_mimic_wood();
            }break;
            case Movestate::protect_boss:
            {
                update_protect_boss();
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
            follow_player();
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

    void __::follow_player()
    {
        _movestate=Movestate::follow_player;
    }
}