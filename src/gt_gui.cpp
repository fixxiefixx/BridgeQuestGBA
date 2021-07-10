#include "gt_gui.h"
#include "bn_sprite_items_heart.h"
#include "bn_sprite_items_wood.h"
#include "bn_sprite_items_cursor.h"
#include "bn_sprite_builder.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_items_textbox.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_font.h"
#include "bn_keypad.h"
#include "variable_8x8_sprite_font.h"
#include "bn_sound_items.h"
#include "bn_log.h"

namespace
{
    [[nodiscard]] bn::sprite_ptr _create_sprite_heart()
    {
        bn::sprite_builder builder(bn::sprite_items::heart,1);
        return builder.release_build();
    }

    [[nodiscard]] bn::sprite_ptr _create_sprite_wood()
    {
        bn::sprite_builder builder(bn::sprite_items::wood,0);
        builder.set_z_order(-1001);
        builder.set_position((240-16)/2,(-160+16)/2);
        return builder.release_build();
    }
}

namespace gt
{
    gui::gui():
    _wood_sprite(_create_sprite_wood())
    {
        _max_health=0;
        _health=0;
        _wood_sprite.set_visible(false);
    }

    void gui::update()
    {

    }

    void gui::set_health(uint8_t health)
    {
        BN_LOG("set health ",health);
        BN_ASSERT(health>=0 && health <= _hearts.max_size(),"health out of range");
        BN_ASSERT(health<=_max_health,"health over max_health");
        if(_health!=health)
        {
            for(int i=_health;i < health;i++)
            {
                bn::sprite_ptr *sprite=&_hearts.at(i);
                sprite->set_tiles(bn::sprite_items::heart.tiles_item(),0);
            }

            for(int i=health;i<_max_health;i++)
            {
                bn::sprite_ptr *sprite=&_hearts.at(i);
                sprite->set_tiles(bn::sprite_items::heart.tiles_item(),1);
            }
            _health=health;
        }
    }

    uint8_t gui::health()
    {
        return _health;
    }

    void gui::set_max_health(uint8_t max_health)
    {
        BN_ASSERT(max_health >= 0 && max_health <= _hearts.max_size(),"max_health out of range");
        if(max_health != _max_health)
        {
            for(int i=_max_health;i<max_health;i++)
            {
                bn::sprite_ptr sprite=_create_sprite_heart();
                sprite.set_z_order(-1001);
                sprite.set_position(i*8-(120-6),-((160/2)-6));
                _hearts.push_back(sprite);
            }
            _max_health=max_health;
        }
    }

    uint8_t gui::max_health()
    {
        return _max_health;
    }

    int gui::textbox(const bn::string_view& text, bn::vector<bn::string_view,4> options)
    {
        bn::regular_bg_ptr bg_textbox=bn::regular_bg_items::textbox.create_bg(0,0);
        bg_textbox.set_priority(1);
        bn::vector<bn::sprite_ptr, 32> text_sprites;
        bn::sprite_text_generator small_variable_text_generator(variable_8x8_sprite_font);
        small_variable_text_generator.set_bg_priority(1);
        small_variable_text_generator.set_left_alignment();
        bn::core::update();

        bn::fixed y_options_start=0;

        for(int len=1, limit=text.length();len<=limit;len++)
        {
            text_sprites.clear();
            bn::fixed y_pos=35;
            //int len = text.length();
            int start=0;
            for(int i=0;i<len;i++)
            {
                if(text.at(i)=='\n')
                {
                    int sublen=i-start;
                    if(sublen>0)
                    {
                        bn::string_view subText=text.substr(start,sublen);
                        small_variable_text_generator.generate(-115, y_pos, subText, text_sprites);
                    }
                    y_pos+=8;
                    start=i+1;
                }
                if(i==len-1 && i-start>0)
                {
                    bn::string_view subText=text.substr(start,i-start+1);
                    small_variable_text_generator.generate(-115, y_pos, subText, text_sprites);
                }
            }
            char lastChar=text.at(len-1);
            if(lastChar!=' ' && lastChar!='\n')
                bn::sound_items::shortbeep.play(1);

            for(int i=0;i<3;i++)
            {
                if(bn::keypad::a_pressed() || bn::keypad::b_pressed() || bn::keypad::start_pressed())
                {
                    len=limit-1;
                }
                bn::core::update();
            }
            y_options_start=y_pos+8;
        }

        for(int i=0,limit=options.size();i<limit;i++)
        {
            small_variable_text_generator.generate(-115+8, y_options_start+8*i, options[i], text_sprites);
        }

        bn::optional<bn::sprite_ptr> cursor_spr=options.size()>0?bn::optional<bn::sprite_ptr>(
            bn::sprite_items::cursor.create_sprite(bn::fixed_point(-115+2, y_options_start))):
            bn::optional<bn::sprite_ptr>();
        if(cursor_spr)
        {
            cursor_spr->set_bg_priority(1);
        }
        int selection_index=0;

        bn::core::update();
        while(!bn::keypad::a_pressed() && !bn::keypad::start_pressed())
        {
            if(cursor_spr)
            {
                if(bn::keypad::down_pressed())
                {
                    selection_index++;
                    if(selection_index>=options.size())
                    {
                        selection_index=0;
                    }
                    cursor_spr->set_position(cursor_spr->x(), y_options_start+selection_index*8);
                }
                if(bn::keypad::up_pressed())
                {
                    selection_index--;
                    if(selection_index<0)
                    {
                        selection_index=options.size()-1;
                    }
                    cursor_spr->set_position(cursor_spr->x(), y_options_start+selection_index*8);
                }
            }
            bn::core::update();
        }

        bn::core::update();
        return selection_index;
    }

    void gui::textbox(const bn::string_view& text)
    {
        textbox(text,bn::vector<bn::string_view,4>());
    }

    void gui::set_collected_wood(bool has_wood)
    {
        _wood_sprite.set_visible(has_wood);
    }

    bool gui::collected_wood()
    {
        return _wood_sprite.visible();
    }

}