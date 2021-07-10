#ifndef GT_GUI_H
#define GT_GUI_H

#include "bn_core.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"

namespace gt
{
    class gui
    {
        public:
            gui();
            void update();
            void set_health(uint8_t health);
            uint8_t health();
            void set_max_health(uint8_t max_health);
            uint8_t max_health();
            void textbox(const bn::string_view& text);
            int textbox(const bn::string_view& text, bn::vector<bn::string_view,4> options);
            void set_collected_wood(bool has_wood);
            bool collected_wood();
        private:
            uint8_t _health;
            uint8_t _max_health;
            bn::vector<bn::sprite_ptr,6> _hearts;
            bn::sprite_ptr _wood_sprite;
    };
}

#endif