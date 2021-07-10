#include "gt_map.h"
#include "bn_log.h"
#include "gt_globals.h"
#include "gt_util.h"
#include "gt_go_knight.h"
#include "gt_go_bat.h"
#include "gt_go_chest.h"
#include "gt_go_bed.h"
#include "gt_go_lumberjack.h"
#include "gt_go_builder.h"
#include "gt_go_blocking_log.h"
#include "gt_go_tree_boss.h"
#include "gt_go_woodmimic.h"
#include "gt_warp.h"

namespace gt
{
    map::map(bn::camera_ptr camera, map_info m_info):
    _bg(_m_info.bg_item.create_bg(0,0)),
    _m_info(m_info)
    {
        //_m_info=m_info;
        _bg.set_camera(camera);
        _column_count = _m_info.bg_item.map_item().dimensions().width();
        _row_count = _m_info.bg_item.map_item().dimensions().height();
        _mapobjects_origin_offset = bn::point( (_column_count/2)*-8, (_row_count/2)*-8);
        bn::fixed_point start_pos = *(globals::hero->get_pos());
        bn::optional<simple_fixed_rect> area = get_area_at(start_pos);
        BN_ASSERT(area,"No start area found ",start_pos.x(),",",start_pos.y());
        _current_area=*area;
        apply_camera_bounds(_current_area.to_bn_fixed_rect());
        spawn_area_objects(*area);
        BN_LOG("map origin offset ",_mapobjects_origin_offset.x(),",",_mapobjects_origin_offset.y());
    }

    void map::apply_camera_bounds(bn::fixed_rect area)
    {
        globals::followcam->set_limits(bn::fixed_rect(area.x(),
            area.y(),
            area.width()- bn::fixed(240),
            area.height()- bn::fixed(160)));
    }

    uint8_t map::mapobject_at(bn::point tile_coord)
    {
        BN_ASSERT(tile_coord.x()>=0 && tile_coord.y()>=0 &&
            tile_coord.x() < _column_count && tile_coord.y() < _row_count,
            "tile_coord out of range ",tile_coord.x());
        int index = (tile_coord.y()*_column_count)+tile_coord.x();
        return _m_info.mapobjects_data[index];
    }

    bn::point map::position_to_tile_coord(bn::fixed_point pos)
    {
        return bn::point((pos.x().right_shift_integer()-_mapobjects_origin_offset.x())/8,
         (pos.y().right_shift_integer()-_mapobjects_origin_offset.y())/8);
    }

    uint8_t map::position_to_mapobject(bn::fixed_point pos)
    {
        return mapobject_at(position_to_tile_coord(pos));
    }

    bool map::check_rect_collision(bn::fixed_rect rect)
    {

        bn::fixed_rect testrect(rect.position()-_mapobjects_origin_offset, rect.dimensions());

        for(bn::fixed i = testrect.left(); i < testrect.left() + testrect.width(); i += 8)
        for(bn::fixed j = testrect.top(); j < testrect.top() + testrect.height(); j += 8) {
           if(mapobject_at(bn::point(i.right_shift_integer()/8,j.right_shift_integer()/8))==1)
            return true;
       }
        return false;
    }

    bn::optional<simple_fixed_rect> map::get_area_at(bn::fixed_point pos)
    {
        for(int i=0;i<_m_info.area_count;i++)
        {
            const simple_fixed_rect area = _m_info.area_data[i];
            if(area.intersects(pos))
            {
                return bn::optional<simple_fixed_rect>(area);
            }

        }
        return bn::optional<simple_fixed_rect>();
    }

    bn::optional<sign> map::get_sign_at(bn::fixed_point pos)
    {
        for(int i=0;i<_m_info.sign_count;i++)
        {
            sign m_sign = _m_info.sign_data[i];
            if(m_sign.rect().intersects(pos))
            {
                return bn::optional<sign>(m_sign);
            }
        }
        return bn::optional<sign>();
    }

    void map::do_camera_transition(bn::fixed_point oldPos, bn::fixed_point newPos)
    {
        bn::fixed_point pos=oldPos;
        while(pos != newPos)
        {
            pos=move_towards(pos,newPos,5);
            globals::camera->set_position(pos);
            bn::core::update();
        }
    }

    void map::spawn_area_objects(simple_fixed_rect area)
    {
        int x_start=(area.x().right_shift_integer()-_mapobjects_origin_offset.x())/8;
        int y_start=(area.y().right_shift_integer()-_mapobjects_origin_offset.y())/8;
        int x_limit = x_start + area.width().right_shift_integer()/8;
        int y_limit = y_start + area.height().right_shift_integer()/8;
        //BN_LOG("spawn_area_objects ",x_start," ",x_limit," ",y_start," ",y_limit);
        for(int x = x_start;x<x_limit;x++)
        {
            for(int y = y_start;y<y_limit;y++)
            {
                uint8_t blockType = mapobject_at(bn::point(x,y));
                switch(blockType)
                {
                    default:
                    case 0:
                    {
                        //Empty
                    }break;
                    case 2://KnightEnemy
                    {
                        go_knight *knight = new go_knight(*globals::camera);
                        knight->set_pos(bn::fixed_point(x*8,y*8)+_mapobjects_origin_offset);
                        globals::go_manager->add_object(knight);
                    }break;
                    case 3://BatEnemy
                    {
                        go_bat *bat = new go_bat(*globals::camera);
                        bat->set_pos(bn::fixed_point(x*8,y*8)+_mapobjects_origin_offset);
                        globals::go_manager->add_object(bat);
                    }break;
                }
            }
        }

        for(int i=0;i<_m_info.objinfo_count;i++)
        {
            objinfo obji=_m_info.objinfos[i];
            if(area.intersects(obji.rect.position()))
            {
                switch(obji.type)
                {
                    default:
                    case Def::null:
                    {
                        //Nothing
                    }break;
                    case Def::knight:
                    {
                        go_knight *knight = new go_knight(*globals::camera);
                        knight->set_pos(obji.rect.position());
                        globals::go_manager->add_object(knight);
                    }break;
                    case Def::bat:
                    {
                        go_bat *bat = new go_bat(*globals::camera);
                        bat->set_pos(obji.rect.position());
                        globals::go_manager->add_object(bat);
                    }break;
                    case Def::chest:
                    {
                        go_chest *chest = new go_chest(*globals::camera,obji.rect.position(),obji.name, obji.item);
                        globals::go_manager->add_object(chest);
                    }break;
                    case Def::bed:
                    {
                        go_bed *bed = new go_bed(obji.rect);
                        globals::go_manager->add_object(bed);
                    }break;
                    case Def::lumberjack:
                    {
                        go_lumberjack *lumberjack = new go_lumberjack(*globals::camera,obji.rect.position());
                        globals::go_manager->add_object(lumberjack);
                    }break;
                    case Def::builder:
                    {
                        go_builder *builder = new go_builder(*globals::camera,obji.rect.position());
                        globals::go_manager->add_object(builder);
                    }break;
                    case Def::blocking_log:
                    case Def::blocking_log_vertical:
                    case Def::broken_bridge:
                    {
                        go_blocking_log *blocking_log = new go_blocking_log(*globals::camera,obji.rect.position(),obji.type);
                        globals::go_manager->add_object(blocking_log);
                    }break;
                    case Def::tree_boss:
                    {
                        go_tree_boss *tree_boss = new go_tree_boss(*globals::camera,obji.rect.position());
                        globals::go_manager->add_object(tree_boss);
                    }break;
                    case Def::woodmimic:
                    {
                        go_woodmimic *woodmimic = new go_woodmimic(*globals::camera,obji.rect.position());
                        globals::go_manager->add_object(woodmimic);
                    }break;
                }
            }
        }
    }

    void map::activate_area(simple_fixed_rect area)
    {
        _current_area = area;
        bn::fixed_point old_cam_pos=globals::camera->position();
        apply_camera_bounds(area.to_bn_fixed_rect());
        bn::fixed_point new_cam_pos=globals::followcam->apply_limits(*globals::hero->get_pos());

        
        //globals::go_manager->update();
        //bn::core::update();
        globals::go_manager->destroy_all_except(globals::hero);
        spawn_area_objects(area);
        do_camera_transition(old_cam_pos, new_cam_pos);
        


    }

    void map::update_hero_area(bn::fixed_point hero_pos)
    {
        
        if(!_current_area.intersects(hero_pos))
        {
            bn::optional<simple_fixed_rect> new_area=get_area_at(hero_pos);
            BN_ASSERT(new_area,"No area found for hero");
            if(new_area)
                activate_area(*new_area);
            else
                globals::scene_running=false;
        }
    }

    void map::update()
    {
        update_hero_area(*globals::hero->get_pos());
    }

    simple_fixed_rect map::area()
    {
        return _current_area;
    }

    bn::regular_bg_ptr map::bg()
    {
        return _bg;
    }

    bn::optional<warp> map::get_nearest_warp_at(bn::fixed_point pos)
    {
        const warp *nearest_warp=nullptr;
        bn::fixed nearest_dist= bn::fixed(UINT16_MAX);
        for(int i=0;i<_m_info.warp_count;i++)
        {
            const warp *warp = &_m_info.warps[i];
            bn::fixed dist = aprox_length(pos-warp->position);
            if(dist < nearest_dist)
            {
                nearest_dist=dist;
                nearest_warp=warp;
            }
        }
        if(nearest_warp==nullptr)
        {
            return bn::optional<warp>();
        }else
        {
            return bn::optional<warp>(*nearest_warp);
        }
    }

    

}