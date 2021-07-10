#include "gt_go_blob.h"
#include "bn_sprite_items_blob.h"
#include "bn_sprite_builder.h"
#include "bn_log.h"

namespace
{
    [[nodiscard]] bn::sprite_ptr _create_sprite()
    {
        bn::sprite_builder builder(bn::sprite_items::blob);
        return builder.release_build();
    }
}

namespace gt
{
    go_blob::go_blob(const bn::camera_ptr& camera):
    blob_sprite(_create_sprite())
    {
        blob_sprite.set_camera(camera);
    }

    void go_blob::update()
    {
        bn::fixed_point movement=bn::fixed_point(0,0);
        
        switch(test2)
        {
            default:
            case 0:movement=bn::fixed_point(1,0);break;
            case 1:movement=bn::fixed_point(0,1);break;
            case 2:movement=bn::fixed_point(-1,0);break;
            case 3:movement=bn::fixed_point(0,-1);break;
        }

        if(test++>=64)
        {
            test=0;
            test2++;
            if(test2>=4)
            {
                test2=0;
                destroy();
            }
        }
        blob_sprite.set_position(blob_sprite.position()+movement);
    }

}