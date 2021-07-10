#ifndef GT_GO_ENEMY_H
#define GT_GO_ENEMY_H

#include "gt_game_object.h"
#include "bn_vector.h"

namespace gt
{
    class go_enemy: public game_object
    {
        public:
            virtual void do_damage(int amount) override;
        protected:
            int _health;
            bn::vector<Def,4> _droppable_items;
            bn::fixed _drop_chance = 0.25;
            virtual void _die()=0;
    };
}

#endif