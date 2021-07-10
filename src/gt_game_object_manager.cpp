#include "bn_log.h"
#include "gt_game_object_manager.h"

namespace gt
{
    void game_object_manager::update()
    {
        

        //Destroy objects
        while(game_objects_to_destroy.size()>0)
        {
            game_object *obj=game_objects_to_destroy[game_objects_to_destroy.size()-1];
            game_objects_to_destroy.pop_back();
            for(int i = 0, limit = game_objects.size();i<limit;++i)
            {
                if(game_objects[i]==obj)
                {
                    if(i != limit-1)
                    {
                        game_objects[i]=game_objects[limit-1];
                    }
                    game_objects.pop_back();
                    break;
                }
            }
            delete obj;
        }

        //Update objects
        for(int i = 0, limit = game_objects.size();i<limit;++i)
        {
            game_objects[i]->update();
        }
    }

    void game_object_manager::add_object(gt::game_object* obj)
    {
        //BN_LOG("adding game_object to vector");
        game_objects.push_back(obj);
        //BN_LOG("game objects: ",game_objects.size());
    }

    void game_object_manager::destroy_object(gt::game_object* obj)
    {
        if(!obj->is_destroyed())
        {
            obj->mark_as_destroyed();
            game_objects_to_destroy.push_back(obj);
        }
    }

    void game_object_manager::destroy_all_except(gt::game_object* obj)
    {
        for(int i = 0, limit = game_objects.size();i<limit;++i)
        {
            if(game_objects[i] != obj)
            {
                destroy_object(game_objects[i]);
            }
        }
    }

    bool game_object_manager::is_full()
    {
        return game_objects.full();
    }

    bn::vector<gt::game_object*,5> game_object_manager::get_colliding_objects(bn::fixed_rect rect, gt::game_object* obj_to_ignore)
    {
        bn::vector<gt::game_object*,5> colliding_objects;
        for(int i = 0, limit = game_objects.size();i<limit;++i)
        {
            gt::game_object *go = game_objects[i];
            if(go == obj_to_ignore)
                continue;
            bn::optional<bn::fixed_rect> obj_rect = go->get_collision_rect();
            if(!obj_rect)
                continue;
            if(rect.intersects(*obj_rect))
                colliding_objects.push_back(go);
            if(colliding_objects.full())
                break;
        }
        return colliding_objects;
    }

    bn::optional<game_object*> game_object_manager::get_interactive_at(const bn::fixed_point &pos)
    {
        for(int i = 0, limit = game_objects.size();i<limit;++i)
        {
            gt::game_object *go = game_objects[i];
            if(go->is_interactive())
            {
                if(go->intersects(pos))
                    return bn::optional<game_object*>(go);
            }
        }
        return bn::optional<game_object*>();
    }

    bn::vector<gt::game_object*,4> game_object_manager::get_objects_of_type(Def type)
    {
        bn::vector<gt::game_object*,4> type_objects;
        for(int i = 0, limit = game_objects.size();i<limit;++i)
        {
            gt::game_object *go = game_objects[i];
            if(go->type() == type)
            {
                type_objects.push_back(go);
                if(type_objects.full())
                    break;
            }
        }
        return type_objects;
    }

    game_object_manager::~game_object_manager()
    {
        for(int i = 0, limit = game_objects.size();i<limit;++i)
        {
            delete game_objects[i];
        }
        game_objects.clear();
    }

}