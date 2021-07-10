#ifndef GT_GAME_OBJECT_H
#define GT_GAME_OBJECT_H

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_fixed_rect.h"
#include "definitions.h"


namespace gt
{

    class game_object
    {
        public:
            virtual ~game_object() = default;
            virtual void update();
            virtual bn::optional<bn::fixed_point> get_pos();
            virtual bn::optional<bn::fixed_rect> get_collision_rect();
            virtual void do_damage(int amount);
            virtual void interact();
            bool is_destroyed();
            void destroy();
            void mark_as_destroyed();
            bool check_map_collision(bn::fixed_point pos);
            bool intersects(game_object *other);
            bool is_interactive();
            bool intersects(const bn::fixed_point &pos);
            virtual Def type();

        protected:
            game_object() = default;
            bn::optional<bn::fixed_rect> _local_collision_rect;
            bool _is_interactive = false;
        
        private:
            bool _is_destroyed = false;
    };
}

#endif