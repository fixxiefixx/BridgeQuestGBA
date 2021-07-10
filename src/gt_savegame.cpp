#include "gt_savegame.h"

namespace gt
{
    bool savegame::used_object(Def objname)
    {
        for(int i=0, limit = used_objects_count;i<limit;i++)
        {
            if(objname==used_objects[i])
                return true;
        }
        return false;
    }
    
    void savegame::use_object(Def objname)
    {
        if(!used_object(objname))
        {
            BN_ASSERT(used_objects_count<16,"used objects count overflow");
            used_objects_count++;
            used_objects[used_objects_count-1]=objname;
        }
    }

    bool savegame::is_valid()
    {
        return magic_number==1111;
    }
}