#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <string>
#include <cstring>
#include <QRegularExpression>
#include <QTimer>
#include <QChar>
#include <QThread>
#include <tuple>

#include "map.h"
#include "door.h"
#include "presenter.h"

/////////////////////////////////////////////////////////////
//
//
//  Class: door
//
/////////////////////////////////////////////////////////////

// find doors and add to door vector with respective points
void door::locate_door(map& patch, QHash<QPoint, door> &d_vec){

    door temp_door;

    for(int y = 0; y < patch.height; y++)
    {
        for(int x = 0; x < patch.width; x++)
        {
            if (patch.tile_vec[y][x] == tile::tur_Blau || patch.tile_vec[y][x] == tile::tur_Rot){
                temp_door.door_id = patch.tile_vec[y][x];
                QPoint temp_coor(x,y);
                temp_door.door_coor = temp_coor;
                temp_door.door_key = false;
                temp_door.open_duration = 0;
                // add door object to vector of doors
                d_vec.insert(temp_coor, temp_door);
            }
        }
    }
}

// checks and updates icon (open or closed) of door depending on the robot near it
void door::update_door(QHash<QPoint, robot::robot_enum>& robot_hash, map& patch,
                       QHash<QPoint, door> &d_vec, QPoint& my_patch_bot){

    if(!d_vec.empty()){
        for(auto i = d_vec.cbegin(), end = d_vec.cend(); i != end; ++i){
            int x = i.key().x();
            int y = i.key().y();

            if(i.value().door_key == true)
            {
                if (patch.tile_vec[y][x] == tile::tur_Blau){
                    patch.tile_vec[y][x] = tile::tuer_manuell_offen;
                } else if (patch.tile_vec[y][x] == tile::tur_Rot){
                    patch.tile_vec[y][x] = tile::tuer_automatisch_offen;
                }
            }

            if(i.value().door_key == false){
                continue;
            } else {
                i.value().open_duration++;
                /*
                // short version to iterate directions
                // for(auto dir : std::vector<std::pair<int,int>>({{0, -1}, {0, 1}}))
                // update_door_icon(x,y, dir.first, dir.second);
                */
            }

            //qDebug() << d_vec[i].open_duration;

            if(robot_hash.contains(i.value().door_coor)){
                i.value().open_duration--;
            }

            /*
        //update_door_icon(x,y, 0, -1, i);
        //update_door_icon(x,y, 0, 1, i);
        //update_door_icon(x,y, -1, 0, i);
        //update_door_icon(x,y, 1, 0, i);
        */

            if(i.value().open_duration > 9)
            {
                if (patch.tile_vec[y][x] == tile::tuer_manuell_offen){
                    patch.tile_vec[y][x] = tile::tur_Blau;
                } else if (patch.tile_vec[y][x] == tile::tuer_automatisch_offen){
                    patch.tile_vec[y][x] = tile::tur_Rot;
                }
                i.value().open_duration = 0;
                i.value().door_key = false;
            }
        }
    } else {
        return;
    }
}

/*
// update door according to color of door
void door::update_door_icon(int x, int y, int dx, int dy, int index){

    int next_x = x + dx;
    int next_y = y + dy;
    auto& tile_type = door::d_map.tile_vec[y][x];

    if(door::d_map.robot_vec[next_y][next_x] == robot::no_bot
        && door::d_map.robot_vec[y][x] == robot::no_bot){
        return;
    }
}
*/
