#ifndef DJKSTRA_H
#define DJKSTRA_H

#include <unordered_map>
#include <QThread>
#include <QHash>
#include <QVector>

#include "map.h"

// Class djkstra used to find path of current gegner bot to PATCH BOT
// mainly used for robots with legs. i.e. FOLLOWER, HUNTER, and SNIFFER

class djkstra {

public:
    // update branches of each cell, parent and child, with regards to shorter distance
    void djkstra_update_relation(const std::vector<QPoint>& dir, QPoint parent, int curr_dist, const QHash<QPoint, robot::robot_enum> &robot_point);
    // execute path finding algorithem
    void djkstra_map_gen(const map &curr_map, QVector<QPair<QPoint, int>>& djkstr_unvisited_vec,
                         QHash<QPoint, int>& djkstr_visited,
                         QHash<QPoint, std::tuple<QPoint, int>>& djkstra_relation,
                         const QPoint &patchy, const QHash<QPoint, robot::robot_enum> &robot_point);
    // find other robots on map and save current coordinates to vector
    void track_other_bot(const map &curr_map, QHash<QPoint, robot::robot_enum> &robot_point);

private:

    QHash<QPoint, std::tuple<QPoint, int>> relation;    // QHash data structure to store parent of each accessable point on map and shortest distance to patch bot
    QVector<QPair<QPoint, int>> dstr_unvisited_vec;     // QVector data structure of points to visit next, distance increases incremently (lowest to highest)
    QHash<QPoint, int> dstr_visited;                    // QHash data structure of parent points already visited

    // save current real time point of PATCH BOT
    QPoint my_patch_bot;

    // save current map
    map dstra_map;

};


#endif // DJKSTRA_H
