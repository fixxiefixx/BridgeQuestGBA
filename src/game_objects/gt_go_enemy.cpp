#include "gt_go_enemy.h"
#include "gt_globals.h"
#include "gt_go_collectable.h"
#include "bn_log.h"

namespace gt
{
    void go_enemy::do_damage(int amount)
    {
        if(_health<=0)
            return;
        _health-=amount;
        if(_health<=0)
        {
            if(_droppable_items.size()>0)
            {
                bn::fixed drop_rand = bn::fixed(globals::random->get()%100);
                if(drop_rand <= _drop_chance*bn::fixed(100))
                {
                    int item_index = globals::random->get()%_droppable_items.size();
                    Def item = _droppable_items[item_index];
                    
                    if(item != Def::null)
                    {
                        go_collectable* coll = new go_collectable(*globals::camera,*get_pos(),item);
                        globals::go_manager->add_object(coll);
                    }
                }
            }

            _die();
        }
    }
}