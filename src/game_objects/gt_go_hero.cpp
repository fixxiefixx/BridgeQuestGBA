#include "gt_go_hero.h"
#include "gt_globals.h"
#include "bn_sprite_items_hero.h"
#include "bn_keypad.h"
#include "bn_sprite_animate_actions.h"
#include "bn_array.h"
#include "bn_fixed_point.h"
#include "bn_sound_items.h"
#include "bn_math.h"
#include "gt_go_blood_splash.h"
#include "gt_util.h"
#include "bn_music.h"
#include "bn_blending_actions.h"
#include "bn_string_view.h"
#include "bn_music_items.h"
#include "bn_log.h"
#include "bn_rect_window_actions.h"
#include "bn_display.h"
#include "bn_sram.h"

namespace
{
    bn::sprite_animate_action<6> _create_animation_walk_down(bn::sprite_ptr sprite)
    {
        return bn::create_sprite_animate_action_forever(
            sprite, 4, bn::sprite_items::hero.tiles_item(), 15, 16, 17, 18, 19, 20);
    }

    bn::sprite_animate_action<6> _create_animation_walk_right(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 4, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 3>{{30, 31, 32}});
    }

    bn::sprite_animate_action<6> _create_animation_walk_up(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 4, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 6>{{21, 22, 23, 24, 25, 26}});
    }

    bn::sprite_animate_action<6> _create_animation_walk_shield_down(bn::sprite_ptr sprite)
    {
        return bn::create_sprite_animate_action_forever(
            sprite, 8, bn::sprite_items::hero.tiles_item(), 48, 49, 50, 51, 52, 53);
    }

    bn::sprite_animate_action<6> _create_animation_walk_shield_right(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 6>{{60, 61, 62, 63, 64, 65}});
    }

    bn::sprite_animate_action<6> _create_animation_walk_shield_up(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 6>{{54, 55, 56, 57, 58, 59}});
    }

    bn::sprite_animate_action<6> _create_animation_stand_up(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 5>{{5, 6, 7, 8, 9}});
    }

    bn::sprite_animate_action<6> _create_animation_stand_right(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 5>{{10, 11, 12, 13, 14}});
    }

    bn::sprite_animate_action<6> _create_animation_stand_down(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 5>{{0, 1, 2, 3, 4}});
    }

    bn::sprite_animate_action<6> _create_animation_stand_shield_up(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 2>{{59,59}});
    }

    bn::sprite_animate_action<6> _create_animation_stand_shield_right(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 2>{{65,65}});
    }

    bn::sprite_animate_action<6> _create_animation_stand_shield_down(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::forever(sprite, 8, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 2>{{53,53}});
    }

    bn::sprite_animate_action<6> _create_animation_attack_down(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::once(sprite, 4, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 3>{{34, 35, 36}});
    }

    bn::sprite_animate_action<6> _create_animation_attack_up(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::once(sprite, 4, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 3>{{38, 39, 40}});
    }

    bn::sprite_animate_action<6> _create_animation_attack_right(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::once(sprite, 4, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 3>{{42, 43, 44}});
    }

    bn::sprite_animate_action<6> _create_animation_damaged(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::once(sprite, 22, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 2>{{66, 66}});
    }

    bn::sprite_animate_action<6> _create_animation_death(bn::sprite_ptr sprite)
    {
        return bn::sprite_animate_action<6>::once(sprite, 22, bn::sprite_items::hero.tiles_item(), bn::array<uint16_t, 2>{{67, 67}});
    }

}

namespace gt
{
    go_hero::go_hero(const bn::camera_ptr &camera) : _hero_sprite(bn::sprite_items::hero.create_sprite(0, 0)),
                                                     _action(_create_animation_stand_down(_hero_sprite))                                                  
    {
        _local_collision_rect=bn::optional<bn::fixed_rect>(bn::fixed_rect(0, 8, 10, 10));
        _hero_sprite.set_camera(camera);
        _current_direction = Direction::DOWN;
        _current_state = Movestate::STAND;
        _movement_action_needs_rebuild=false;
        _holding_shield=false;
        _blinkframes=0;
        velocity=bn::fixed_point(0,0);
    }

    bool go_hero::check_sign()
    {
        bn::fixed_point check_sign_pos= _hero_sprite.position()+ bn::fixed_point(0,8) + (forward()*10);
        bn::optional<sign> m_sign = globals::current_map->get_sign_at(check_sign_pos);
        if(m_sign)
        {
            globals::gui->textbox((*m_sign).text());
            return true;
        }
        return false;
    }

    bool go_hero::check_interactive()
    {
        bn::fixed_point check_interactive_pos= _hero_sprite.position()+ bn::fixed_point(0,8) + (forward()*10);
        bn::optional<game_object*> go = globals::go_manager->get_interactive_at(check_interactive_pos);
        if(go)
        {
            (*go)->interact();
            return true;
        }
        return false;
    }

    void go_hero::attack()
    {
        bn::sound_items::sword.play(1);
        switch(_current_direction)
        {
            default:
            case Direction::DOWN:
            {
                _hero_sprite.set_horizontal_flip(false);
                _action=_create_animation_attack_down(_hero_sprite);
            }break;
            case Direction::LEFT:
            {
                _hero_sprite.set_horizontal_flip(true);
                _action=_create_animation_attack_right(_hero_sprite);
            }break;
            case Direction::RIGHT:
            {
                _hero_sprite.set_horizontal_flip(false);
                _action=_create_animation_attack_right(_hero_sprite);
            }break;
            case Direction::UP:
            {
                _hero_sprite.set_horizontal_flip(false);
                _action=_create_animation_attack_up(_hero_sprite);
            }break;
        }
        _current_state=Movestate::ATTACK;
        _holding_shield=false;
    }

    bn::fixed_rect go_hero::get_sword_rect()
    {
        return bn::fixed_rect(_hero_sprite.position()+bn::fixed_point(0,8)+forward()*10,
            bn::fixed_size(16,16));
    }

    void go_hero::update_attack_state()
    {
        bn::fixed_rect sword_rect=get_sword_rect();
        bn::vector<game_object*,10> colliding_objects = globals::go_manager->get_colliding_objects(sword_rect,this);
        for(int i = 0, limit = colliding_objects.size();i<limit;++i)
        {
            game_object *obj=colliding_objects[i];
            obj->do_damage(1);
        }

        if(_action.done())
        {
            _current_state=Movestate::STAND;
            _movement_action_needs_rebuild=true;
        }
    }

    void go_hero::move_and_slide()
    {
        bool noclip = /*bn::keypad::l_held()*/false;
        bn::fixed_point testPos=_hero_sprite.position()+bn::fixed_point(velocity.x(),0);
        if(noclip || !check_map_collision(testPos))
            _hero_sprite.set_x(testPos.x());
        else
            velocity.set_x(0);
        
        testPos=_hero_sprite.position()+bn::fixed_point(0,velocity.y());
        if(noclip || !check_map_collision(testPos))
            _hero_sprite.set_y(testPos.y());
        else
            velocity.set_y(0);
    }

    void go_hero::update_stand_walk_state()
    {
        bn::fixed_point movement = bn::fixed_point(0, 0);
        Direction new_direction = _current_direction;
        Movestate new_state = Movestate::STAND;

        bool new_holding_shield=bn::keypad::b_held();
        bn::fixed speed = new_holding_shield ? 0.5f : 1;

        if(gt::globals::scene_running)
        {
            if (bn::keypad::left_held())
            {
                //if (!check_map_collision(_hero_sprite.position() + bn::fixed_point(-speed, 0)))
                movement.set_x(movement.x() - speed);
                if(!new_holding_shield)
                    new_direction = Direction::LEFT;
                new_state = Movestate::WALK;
            }
            if (bn::keypad::right_held())
            {
                //if (!check_map_collision(_hero_sprite.position() + bn::fixed_point(speed, 0)))
                movement.set_x(movement.x() + speed);
                if(!new_holding_shield)
                    new_direction = Direction::RIGHT;
                new_state = Movestate::WALK;
            }

            if (bn::keypad::up_held())
            {
                //if (!check_map_collision(_hero_sprite.position() + bn::fixed_point(0, -speed)))
                movement.set_y(movement.y() - speed);
                if(!new_holding_shield)
                    new_direction = Direction::UP;
                new_state = Movestate::WALK;
            }
            if (bn::keypad::down_held())
            {
                //if (!check_map_collision(_hero_sprite.position() + bn::fixed_point(0, speed)))
                movement.set_y(movement.y() + speed);
                if(!new_holding_shield)
                    new_direction = Direction::DOWN;
                new_state = Movestate::WALK;
            }
        }
        if (new_direction != _current_direction || new_state != _current_state || 
        new_holding_shield != _holding_shield || _movement_action_needs_rebuild)
        {
            bool new_walking = new_state == Movestate::WALK;
            switch (new_direction)
            {
                default:
                case Direction::DOWN:
                {
                    _hero_sprite.set_horizontal_flip(false);
                    if(new_walking)
                    {
                        if(new_holding_shield)
                             _action = _create_animation_walk_shield_down(_hero_sprite);
                        else
                            _action = _create_animation_walk_down(_hero_sprite);
                    }
                    else
                    {
                        if(new_holding_shield)
                            _action =  _create_animation_stand_shield_down(_hero_sprite);
                        else
                            _action =  _create_animation_stand_down(_hero_sprite);
                    }
                }break;
                case Direction::LEFT:
                {
                    _hero_sprite.set_horizontal_flip(true);
                     if(new_walking)
                    {
                        if(new_holding_shield)
                             _action = _create_animation_walk_shield_right(_hero_sprite);
                        else
                            _action = _create_animation_walk_right(_hero_sprite);
                    }
                    else
                    {
                        if(new_holding_shield)
                            _action =  _create_animation_stand_shield_right(_hero_sprite);
                        else
                            _action =  _create_animation_stand_right(_hero_sprite);
                    }
                }break;
                case Direction::UP:
                {
                    _hero_sprite.set_horizontal_flip(false);
                     if(new_walking)
                    {
                        if(new_holding_shield)
                             _action = _create_animation_walk_shield_up(_hero_sprite);
                        else
                            _action = _create_animation_walk_up(_hero_sprite);
                    }
                    else
                    {
                        if(new_holding_shield)
                            _action =  _create_animation_stand_shield_up(_hero_sprite);
                        else
                            _action =  _create_animation_stand_up(_hero_sprite);
                    }
                }break;
                case Direction::RIGHT:
                {
                    _hero_sprite.set_horizontal_flip(false);
                     if(new_walking)
                    {
                        if(new_holding_shield)
                             _action = _create_animation_walk_shield_right(_hero_sprite);
                        else
                            _action = _create_animation_walk_right(_hero_sprite);
                    }
                    else
                    {
                        if(new_holding_shield)
                            _action =  _create_animation_stand_shield_right(_hero_sprite);
                        else
                            _action =  _create_animation_stand_right(_hero_sprite);
                    }
                }break;
            }
            _current_direction = new_direction;
            _current_state = new_state;
            _holding_shield = new_holding_shield;
            _movement_action_needs_rebuild=false;
        }

        velocity=move_towards(velocity,movement,0.5f);


        int16_t prev_tile_x=_hero_sprite.x().right_shift_integer()/8;
        int16_t prev_tile_y=_hero_sprite.y().right_shift_integer()/8;
        move_and_slide();
        int16_t post_tile_x=_hero_sprite.x().right_shift_integer()/8;
        int16_t post_tile_y=_hero_sprite.y().right_shift_integer()/8;

        if(prev_tile_x != post_tile_x || prev_tile_y != post_tile_y)
        {
            uint8_t mapobj = globals::current_map->position_to_mapobject(_hero_sprite.position()+bn::fixed_point(0,8));
            if(mapobj==4)//warp
            {
                bn::optional<warp> opt_warp = globals::current_map->get_nearest_warp_at(_hero_sprite.position());
                if(opt_warp)
                {
                    
                    /*bn::window win=bn::window::outside();
                    win.set_show_bg(globals::current_map->bg(),false);
                    bn::rect_window internal_window = bn::rect_window::internal();
                    internal_window.set_boundaries(-160/2, -240/2, 160/2, 240/2);
                    bn::rect_window_move_top_to_action move_top_action(internal_window, 30, 0);
                    bn::rect_window_move_left_to_action move_left_action(internal_window, 30, 0);
                    bn::rect_window_move_bottom_to_action move_bottom_action(internal_window, 30, 0);
                    bn::rect_window_move_right_to_action move_right_action(internal_window, 30, 0);

                    while(!move_right_action.done())
                    {
                        move_top_action.update();
                        move_left_action.update();
                        move_bottom_action.update();
                        move_right_action.update();
                        bn::core::update();
                    }
                    internal_window.set_boundaries(0, 0, 0, 0);*/

                    _hero_sprite.set_blending_enabled(true);
                    globals::current_map->bg().set_blending_enabled(true);
                    bn::blending::set_fade_alpha(0);

                    bn::blending_fade_alpha_to_action action(15,1);
                    while(!action.done())
                    {
                        action.update();
                        bn::core::update();
                    }

                    globals::save.current_warp=*opt_warp;
                    globals::scene_running=false;
                }
            }
        }

        //bn::fixed_point new_pos = _hero_sprite.position() + movement;
        //_hero_sprite.set_position(new_pos);

        if(bn::keypad::a_pressed())
        {
            if(!check_interactive())
                if(!check_sign())
                    attack();
        }

        if(bn::keypad::start_pressed())
        {
            bool in_menu=true;
            while(in_menu)
            {
                bn::vector<bn::string_view,4> options;
                options.push_back("Continue");
                options.push_back("Save game");
                options.push_back("Back to main menu");
                int option = globals::gui->textbox("Game paused.",options);
                if(option == 1)
                {
                    globals::save.health=globals::gui->health();
                    bn::sram::write(globals::save);
                    globals::gui->textbox("Game saved.");
                    in_menu=false;
                }
                else if(option == 2)
                {
                    options.clear();
                    options.push_back("No");
                    options.push_back("Ok");
                    option=globals::gui->textbox("Are you sure?\nUnsaved progress will be lost.",options);
                    if(option==1)
                    {
                        _hero_sprite.set_blending_enabled(true);
                        globals::current_map->bg().set_blending_enabled(true);
                        bn::blending_fade_alpha_to_action action(30,1);
                        while(!action.done())
                        {
                            action.update();
                            bn::core::update();
                        }

                        globals::to_main_menu=true;
                        globals::scene_running=false;
                        in_menu=false;
                    }
                }
                else if(option == 0)
                {
                    in_menu=false;
                }
            }
        }
    }

    void go_hero::update_damaged_state()
    {
        if(_action.done())
        {
            _current_state=Movestate::STAND;
            _movement_action_needs_rebuild=true;
            _blinkframes = 60;
        }else
        {
            velocity=move_towards(velocity,bn::fixed_point(0,0),0.5);
            move_and_slide();
        }
    }

    void go_hero::update()
    {

        switch (_current_state)
        {
            default:
            case Movestate::STAND:
            case Movestate::WALK:
            {
                update_stand_walk_state();
            }break;
            case Movestate::ATTACK:
            {
                update_attack_state();
            }break;
            case Movestate::DAMAGED:
            {
                update_damaged_state();
            }break;
        }

        if(_blinkframes>0)
        {
            _blinkframes--;
            if(_blinkframes==0)
            {
                _hero_sprite.set_visible(true);
            }
            else
            {
                _hero_sprite.set_visible((_blinkframes/2)%2);
            }
        }

        if(!_action.done())
            _action.update();
        
        _hero_sprite.set_z_order(-_hero_sprite.y().right_shift_integer());
    }

    bn::optional<bn::fixed_point> go_hero::get_pos()
    {
        return bn::optional<bn::fixed_point>(_hero_sprite.position());
    }

    void go_hero::set_pos(bn::fixed_point pos)
    {
        _hero_sprite.set_position(pos);
    }


    bn::fixed_point go_hero::forward()
    {
        switch(_current_direction)
        {
            default:
            case Direction::DOWN:return bn::fixed_point(0,1);
            case Direction::UP:return bn::fixed_point(0,-1);
            case Direction::LEFT:return bn::fixed_point(-1,0);
            case Direction::RIGHT:return bn::fixed_point(1,0);
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

    void go_hero::do_damage(int amount)
    {
        if(_current_state == Movestate::DAMAGED || _blinkframes>0)
            return;
        
        if(globals::gui->health()>0)
        {
            globals::gui->set_health(globals::gui->health()-1);
            if(globals::gui->health()==0)
            {
                bn::music::stop();
                //bn::music::play(bn::music_items::sadness);
                gt::globals::music->play(&bn::music_items::sadness,1);
                _action=_create_animation_damaged(_hero_sprite);
                //globals::current_map->bg().set_visible(false);
                bn::sound_items::hero_attacked.play(1,0.75,0);
                _hero_sprite.set_blending_enabled(false);
                globals::current_map->bg().set_blending_enabled(true);
                bn::blending::set_fade_alpha(0);

                bn::blending_fade_alpha_to_action action(60,1);
                while(!action.done())
                {
                    action.update();
                    bn::core::update();
                }
                _action=_create_animation_death(_hero_sprite);
                if(!_action.done())
                    _action.update();
                bn::core::update();
                globals::gui->textbox("Oh no, You are death.\nThe evil has won.\nYou were the last hope for our kingdom.\n\n             GAME OVER");
                globals::scene_running=false;
            }else
            {
                bn::sound_items::hero_attacked.play(1);
                _current_state=Movestate::DAMAGED;
                _action=_create_animation_damaged(_hero_sprite);
            }
        }
    }

#pragma GCC diagnostic pop

    go_hero::Direction go_hero::position_to_direction(bn::fixed_point point)
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

    bool go_hero::is_position_blockable(bn::fixed_point pos)
    {
        bn::fixed_point me_to_pos = pos - _hero_sprite.position();
        Direction blocking_direction = position_to_direction(me_to_pos);
        return blocking_direction == _current_direction;
    }

    bool go_hero::is_blocking(bn::fixed_point pos)
    {
        if(!_holding_shield)
            return false;
        return is_position_blockable(pos);
    }

    go_hero::Direction go_hero::direction()
    {
        return _current_direction;
    }
            
    void go_hero::set_direction(Direction dir)
    {
        _current_direction=dir;
        _movement_action_needs_rebuild=true;
    }


    void go_hero::set_blending_enabled(bool value)
    {
        _hero_sprite.set_blending_enabled(value);
    }

    bool go_hero::blending_enabled()
    {
        return _hero_sprite.blending_enabled();
    }

    bool go_hero::is_vulnerable()
    {
        return _blinkframes<=0 && _current_state!=Movestate::DAMAGED;
    }

    void go_hero::collect_item(Def item)
    {
        switch(item)
        {
            default:
            case Def::null:
            {
                //Nothing
            }break;
            case Def::wood:
            {
                globals::save.collected_wood=true;
                globals::gui->set_collected_wood(true);
            }break;
            case Def::heart:
            {
                uint8_t new_healt = globals::gui->health()+1;
                if(new_healt <= globals::gui->max_health())
                {
                    globals::gui->set_health(new_healt);
                }
            }break;
        }
    }
}