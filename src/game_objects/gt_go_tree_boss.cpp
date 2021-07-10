#include "gt_go_tree_boss.h"
#include "bn_sprite_items_tree_boss.h"
#include "bn_sprite_items_woodboss.h"
#include "bn_sprite_items_bullet.h"
#include "bn_sprite_builder.h"
#include "gt_globals.h"
#include "gt_util.h"
#include "bn_sound_items.h"
#include "gt_go_blocking_log.h"
#include "bn_music_items.h"
#include "gt_go_smoke_explosion.h"
#include "gt_go_fire_bullet.h"
#include "gt_go_bat.h"
#include "gt_go_tree_boss_minion.h"

namespace
{
    bn::sprite_ptr _create_sprite()
    {
        bn::sprite_builder builder(bn::sprite_items::woodboss);
        return builder.release_build();
    }

    bn::sprite_animate_action<2> _create_animation_idle(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<2>::forever(sprite, 8, bn::sprite_items::woodboss.tiles_item(), bn::array<uint16_t, 2>{{0, 1}});
    }

    bn::sprite_animate_action<2> _create_animation_spit(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<2>::once(sprite, 8, bn::sprite_items::woodboss.tiles_item(), bn::array<uint16_t, 2>{{2, 1}});
    }

    bn::sprite_animate_action<2> _create_animation_dying(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<2>::once(sprite, 8, bn::sprite_items::woodboss.tiles_item(), bn::array<uint16_t, 2>{{3, 3}});
    }

}

namespace gt
{
    go_tree_boss::go_tree_boss(bn::camera_ptr& camera, bn::fixed_point position):
    _sprite(_create_sprite()),
    _action(_create_animation_idle(_sprite)),
    _movestate(Movestate::disabled),
    _velocity(0,0),
    _start_pos(position)
    {
        _health=10;
        _idle_time=0;
        _blinkframes=0;
        _dying_counter=10;
        _angry=false;
        _sprite.set_position(position);
        _sprite.set_camera(camera);
        _sprite.set_visible(false);
        _local_collision_rect=bn::fixed_rect(0,8,16,16);

        
    }

    void go_tree_boss::update_hero_collision()
    {
        if(globals::hero->is_vulnerable() && globals::hero->intersects(this))
        {
            /*if(globals::hero->is_blocking(_sprite.position()))
            {
                bn::sound_items::shield_bounce.play(1);
                
            }
            else*/
            {
                globals::hero->do_damage(1);
            }

            bn::fixed_point hero_to_me = _sprite.position() - (*(globals::hero->get_pos()));
            globals::hero->velocity=normalize(hero_to_me)*-4;
            
            _movestate=Movestate::idle;
            _idle_time=30;
            
            //_velocity=normalize(hero_to_me)*2;
            
        }
    }

    void go_tree_boss::update_disabled()
    {
        if(!globals::save.collected_wood)
            return;
        if(globals::save.tree_boss_defeated)
            return;
        
        bn::fixed_point me_to_hero=(*globals::hero->get_pos()) - _sprite.position();
        if(aprox_length(me_to_hero)<bn::fixed(60))
        {
            _sprite.set_visible(true);
            _movestate=Movestate::idle;
            _idle_time=2*60;
            globals::music->play(&bn::music_items::absolu,0.5);
            globals::go_manager->add_object(new go_smoke_explosion(*(globals::camera),_sprite.position()));
            globals::hero->velocity=normalize(me_to_hero)*4;

            bn::vector<gt::game_object*,4> logs = globals::go_manager->get_objects_of_type(Def::blocking_log);
            for(int i=0,limit=logs.size();i<limit;i++)
            {
                go_blocking_log *log =(go_blocking_log*) logs[i];
                log->set_enabled(true);
            }
            logs = globals::go_manager->get_objects_of_type(Def::blocking_log_vertical);
            for(int i=0,limit=logs.size();i<limit;i++)
            {
                go_blocking_log *log =(go_blocking_log*) logs[i];
                log->set_enabled(true);
            }
            //spawn_minions();
            
        }
    }

    void go_tree_boss::update_idle()
    {
        if(--_idle_time<=0)
        {
            if(_angry)
            {
                _movestate=Movestate::go_to_start;
            }
            else
            {
                _action=_create_animation_idle(_sprite);
                _movestate=Movestate::follow_player;
            }

            
        }
    }

    void go_tree_boss::update_follow_player()
    {
        bn::fixed_point me_to_hero=(*globals::hero->get_pos()) - _sprite.position();
        _velocity = normalize(me_to_hero)*bn::fixed(_health>3? 0.25 : 0.35);
        _sprite.set_position(_sprite.position()+_velocity);
    }

    void go_tree_boss::update_go_to_start()
    {
        _sprite.set_position(move_towards(_sprite.position(),_start_pos,1));
        bn::fixed_point me_to_start=_start_pos-_sprite.position();
        if(me_to_start.x().right_shift_integer()==0 && me_to_start.y().right_shift_integer()==0)
        {
            _movestate=Movestate::fire_bullets;
            _bullets_fired=0;
            _idle_time=0;
            
        }
    }

    void go_tree_boss::throw_bullet(bn::fixed_point pos, bn::fixed_point velocity)
    {
        bn::camera_ptr cam=*_sprite.camera();
        go_fire_bullet *bullet=new go_fire_bullet(cam,pos,velocity);
        globals::go_manager->add_object(bullet);
    }

    void go_tree_boss::update_fire_bullets()
    {
        int bullet_count=_health <=3?16:8;

        if(_bullets_fired>=bullet_count || globals::go_manager->is_full())
        {
            _angry=false;
            _movestate=Movestate::idle;
            _idle_time=60;

        }
        else
        {
            if(--_idle_time<=0)
            {
                _action=_create_animation_spit(_sprite);
                bn::sound_items::fire_bullet.play(1);
                _idle_time = _health <=3 ? 20 : 30;
                bn::fixed_point me_to_hero=*globals::hero->get_pos() - _sprite.position();

                bool spawned_minions=false;
                if(_bullets_fired==bullet_count-3)
                {
                    bn::vector<game_object*,4> minion_vec=globals::go_manager->get_objects_of_type(Def::tree_boss_minion);
                    for(int i=0,limit=minion_vec.size();i<limit;i++)
                    {
                        ((go_tree_boss_minion*)minion_vec[i])->follow_player();
                    }

                    if(minion_vec.size()==0)
                    {
                        bn::sound_items::collect.play(1);
                        spawn_minions();
                        spawned_minions=true;
                    }
                }
                if(!spawned_minions)
                {
                    if(_health<=2 && _bullets_fired==bullet_count-1)
                    {
                        bn::camera_ptr cam=*_sprite.camera();
                        go_bat *bat=new go_bat(cam);
                        bat->set_pos(_sprite.position());
                        globals::go_manager->add_object(bat);
                    }else
                    {
                        throw_bullet(_sprite.position(),normalize(me_to_hero)*2);
                    }
                }
                
                _bullets_fired++;
            }
        }
    }

    void go_tree_boss::update_dying()
    {
        if(_idle_time--<=0)
        {
            if(_dying_counter--<=0)
            {
                globals::go_manager->add_object(new go_smoke_explosion(*(globals::camera),_sprite.position()));
                globals::save.tree_boss_defeated=true;
                globals::music->play(&bn::music_items::forgottenone,0.5);
                globals::go_manager->destroy_object(this);

                bn::vector<gt::game_object*,4> logs = globals::go_manager->get_objects_of_type(Def::blocking_log);
                for(int i=0,limit=logs.size();i<limit;i++)
                {
                    go_blocking_log *log =(go_blocking_log*) logs[i];
                    log->set_enabled(false);
                }
                logs = globals::go_manager->get_objects_of_type(Def::blocking_log_vertical);
                for(int i=0,limit=logs.size();i<limit;i++)
                {
                    go_blocking_log *log =(go_blocking_log*) logs[i];
                    log->set_enabled(false);
                }
                bn::vector<game_object*,4> minion_vec=globals::go_manager->get_objects_of_type(Def::tree_boss_minion);
                for(int i=0,limit=minion_vec.size();i<limit;i++)
                {
                    ((go_tree_boss_minion*)minion_vec[i])->follow_player();
                }
            }else
            {
                _idle_time=15;
                go_smoke_explosion* explo = new go_smoke_explosion(*globals::camera,_sprite.position()+bn::fixed_point((globals::random->get()%32)-16,(globals::random->get()%32)-16));
                globals::go_manager->add_object(explo);
                if(_dying_counter==5)
                    _blinkframes=1000;
            }
        }
    }

    void go_tree_boss::update()
    {
        switch(_movestate)
        {
            default:
            case Movestate::disabled:
            {
                update_disabled();
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
            case Movestate::go_to_start:
            {
                update_go_to_start();
                update_hero_collision();
            }break;
            case Movestate::fire_bullets:
            {
                update_fire_bullets();
                update_hero_collision();
            }break;
            case Movestate::dying:
            {
                update_dying();
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
        _sprite.set_z_order(-_sprite.y().right_shift_integer());
        if(!_action.done())
            _action.update();
    }

    void go_tree_boss::_die()
    {
        _movestate=Movestate::dying;
        _idle_time=15;
        _action=_create_animation_dying(_sprite);
    }

    void go_tree_boss::do_damage(int amount)
    {
        if(_blinkframes>0 || _movestate == Movestate::disabled)
            return;
        go_enemy::do_damage(amount);
        if(_health>0)
        {
            _blinkframes=120;
            _movestate=Movestate::idle;
            _idle_time=60;
            _angry=true;
        }
    }

    bn::optional<bn::fixed_point> go_tree_boss::get_pos()
    {
        return bn::optional(_sprite.position());
    }

    void go_tree_boss::spawn_minions()
    {
        int count=_health<=5?4:2;
        
        for(int i=0;i<count;i++)
            {
                go_tree_boss_minion* minion = new go_tree_boss_minion(*globals::camera,_sprite.position(),this,i*(count==2?180:90));
                globals::go_manager->add_object(minion);
            }
    }

}