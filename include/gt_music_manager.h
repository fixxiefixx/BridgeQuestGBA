#ifndef GT_MUSIC_MANAGER_H
#define GT_MUSIC_MANAGER_H

#include "bn_vector.h"
#include "bn_unordered_map.h"
#include "bn_music_item.h"
#include "bn_optional.h"

namespace gt
{
    class music_manager
    {
        public:
            music_manager();
            void play(const bn::music_item *item,bn::fixed volume);
            bool is_playing();
            void stop();
        private:
            const bn::music_item* _current_music_item;
    };
}

#endif