#ifndef DOOR_H
#define DOOR_H

#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QThread>
#include <iostream>

//#include "tile.h"
#include "map.h"

//class map;

// Class Door to store enum of each door and their respective location
// Used to check whether robots and infront of door, to open and close with
// respect to the type of robot
class door
{

public:
    // door enum
    mutable tile::tile_enum door_id;

    // door coordinates
    mutable QPoint door_coor;
    // open duration of door, when greater than 10 door closes
    mutable int open_duration;
    // door key, when true door remains open
    mutable bool door_key;

    // find doors and add to door vector with respective points
    void locate_door(map &patch, QHash<QPoint, door> &d_vec);
    // checks and updates icon (open or closed) of door depending on the robot near it
    void update_door(QHash<QPoint, robot::robot_enum> &robot_hash, map &patch,
                     QHash<QPoint, door> &d_vec, QPoint &my_patch_bot);
    // update door according to color of door
    //void update_door_icon(int x, int y, int dx, int dy, int index);

};

#endif // DOOR_H

