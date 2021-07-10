#include "gt_music_manager.h"
#include "bn_music_items.h"
#include "bn_music.h"
#include "bn_fixed.h"


namespace gt
{
    music_manager::music_manager()
    {
        _current_music_item=nullptr;
    }
            

    void music_manager::play(const bn::music_item *item, bn::fixed volume)
    {
        if(_current_music_item==item)
            return;

        if(bn::music::playing())
            bn::music::stop();

        bn::music::play(*item,volume);
        _current_music_item=item;
    }

    bool music_manager::is_playing()
    {
        return bn::music::playing();
    }
    
    void music_manager::stop()
    {
        if(bn::music::playing())
        {
            bn::music::stop();
            _current_music_item=nullptr;
        }
    }
}