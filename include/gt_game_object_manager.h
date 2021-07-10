#ifndef GT_GAME_OBJECT_MANAGER_H
#define GT_GAME_OBJECT_MANAGER_H

#include "bn_vector.h"
#include "gt_game_object.h"

namespace gt
{
    class game_object_manager
    {
        public:
            void update();
            void add_object(gt::game_object* obj);
            void destroy_object(gt::game_object* obj);
            void destroy_all_except(gt::game_object* obj);
            bool is_full();
            bn::vector<gt::game_object*,5> get_colliding_objects(bn::fixed_rect rect, gt::game_object* obj_to_ignore);
            bn::optional<game_object*> get_interactive_at(const bn::fixed_point &pos);
            bn::vector<gt::game_object*,4> get_objects_of_type(Def type);
            ~game_object_manager();
        private:
            bn::vector<gt::game_object*, 28> game_objects;
            bn::vector<gt::game_object*, 28> game_objects_to_destroy;
    };
}

#endif