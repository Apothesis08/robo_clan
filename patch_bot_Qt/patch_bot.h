#ifndef PATCH_BOT_H
#define PATCH_BOT_H

#include <QDebug>
#include <QString>
#include <QThread>
#include <iostream>

#include "map.h"
#include "ki_gegner.h"

// class PATCH_BOT contains functions and attributes to move the PATCH_BOT
// with regards to the input "text" and current position of PATCH_BOT on map
// also considers the positions of other gegner robots and state of doors
class patch_bot
{

public:
    // used as main function to run entire class, such as moving PATCH BOT and updating doors, map and its current position, connecting point to the presenter class
    void get_map_update(bool &patch_delay, QString& text, map &patch,
                        QHash<QPoint, robot::robot_enum>& gegner_point,
                        QHash<QPoint, door> &d_vec, QHash<QPoint, ki_gegner_att> &gegner_vec,
                        QPoint &patch_bot_old);
    // update location of patch bot (main function)
    void update_location();
    // delay by 1 second
    void wait();
    // update number of direction repetitions with regards to "repeat until obstacle"
    void update_rounds();
    // validates next patch bot move, pushes enemy bot if possible and also  with regards to "repeat until obstacle"
    void update_patch_bot();
    // validates next patch bot move, pushes enemy bot if possible and also  with regards to "repeat until obstacle"
    void update_push_bot();
    // if patch bot encounters danger, update icon to tomb stone
    void update_patch_bot_icon();


private:
    QString updated_text;    // updated text of text box
    map p_map;               // store current map data
    QPoint patch_bot_coor;   // save current real time ppoint of PATCH BOT

    QHash<QPoint, door> door_vec; // vector of doors with respective coordinates
    int rounds;              // number of repetitions
    bool up;                 // if patch bot direction is up
    bool down;               // if patch bot direction is down
    bool left;               // if patch bot direction is left
    bool right;              // if patch bot direction is right
    bool hind;               // if next tile is hindernis
    bool recur;              // if "move until obstacle" is required
    bool delay;
    int next_y;              // save next y coordinate
    int next_x;              // save next x coordinate
    int second_next_y;       // save second next y coordinate
    int second_next_x;       // save second next x coordinate
    QPoint next;             // save next point of PATCH BOT
    QPoint snd_next;         // save second next point of PATCH BOT
    QHash<QPoint, robot::robot_enum> robot_point;    // QHASH robot_point contains up-to-date gegner robot location and their respective robot enums of all robots
    QHash<QPoint, ki_gegner_att> gegner_patch_hash;  // QHASH gegner_patch_hash contains up-to-date gegner robot location and their respective attributes (from class ki_gegner_att)
};


#endif // PATCH_BOT_H
