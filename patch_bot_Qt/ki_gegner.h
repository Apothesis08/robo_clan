#ifndef KI_GEGNER_H
#define KI_GEGNER_H

#include <unordered_map>
#include <QThread>

#include "map.h"
#include "ki_gegner_att.h"

// class KI_GEGNER used to implement KI algorithm to change states of gegner robot objects generated from ki_gegner_att class
// each robot state is checked and updated once at each call of the "validate" function
// BUGGER robots are rotated and moved when walls are not found
// PUSHER, DIGGER, and SWIMMER are moved vertically or horizontally depending of position of PATCH BOT
// Robots do not move when blocked and states are changed
// Next move coordinates are validated with regards to current map
// QHASH ROBOT_HASH contains up-to-date gegner robot location and their respective robot enums of all robots
// QHASH GEGNER_HASH contains up-to-date gegner robot location and their respective attributes (from class ki_gegner_att)

class ki_gegner {

public:
    // rotate robot at "old" coordinate clockwise
    void rotate(const QPoint &old, QHash<QPoint, ki_gegner_att>& gegner_hash);

    // main AI funtion used to change states of each gegner robot and update position
    QHash<QPoint, std::tuple<QPoint, int> > gegner_bot_ki(QHash<QPoint, robot::robot_enum> &robot_hash,
                  map &ki_map, QHash<QPoint, ki_gegner_att> &gegner_hash,
                  QHash<QPoint, door>& d_vec, QPoint& patch_bot_old,
                  QHash<QPoint, std::tuple<QPoint, int> > &relation_hash);

    // find initial location of robots on map and save current coordinates with generated ki_gegner_att object
    QHash<QPoint, ki_gegner_att> locate_gegner(const QHash<QPoint, robot::robot_enum> &robot_hash, const map &ki_map);

    // check if input coordinate is valid with regards to map size
    bool valid_point(const QPoint &curr_coor, const map &ki_map,
                     robot::robot_enum my_id); // check methods should be const

    // initial function of checking if robot is blocked, while using the next direction of robot, call snd_robot_blocked
    bool robot_blocked(ki_gegner_att& gegner, const map &ki_map,
                       const QHash<QPoint, robot::robot_enum> &robot_hash);

    // checks whether robot is blocked, using QHash of up-to-date coordinates of all robots and environment
    bool snd_robot_blocked(QPoint& curr, QPoint& next, const map &ki_map,
                           const QHash<QPoint, robot::robot_enum> &robot_hash,
                           robot::robot_enum robot_id);

    // with regards to current state of gegner robots and coordiantes of robot, if a coordinate is close to patch bot's coordinate move either vertically or horizontally
    bool patchy_coor_reached(ki_gegner_att::state& curr_state,
                             const ki_gegner_att &curr_ki, QPoint& next_point);

    // checks whether input coordinate is a wall, used for BUGGER robot
    bool coor_is_wall(const map& ki_map, const QPoint& dir);

    // creates an entire new version of current gegner robot (ki_gegner_att) and inserts new version of gegner robot in gegner_hash
    // used when robots are pushed or moved
    void refresh_points(const QPoint& old, const QPoint& latest,
                        QHash<QPoint, ki_gegner_att>& gegner_hash, const map &ki_map);

    // removes old version of gegner robot and inserts new version of gegner robot in gegner_hash
    // used when states or other minor attributes of gegner robots are changed
    void save_ki(QHash<QPoint, ki_gegner_att>& gegner_hash, const ki_gegner_att& gegner,
                 const QPoint& init_point);

    // provide keys to doors making the cooresponding door open
    void open_door(const QPoint &latest, QHash<QPoint, door> &d_vec, const map &ki_map);

    // execute rotate and save function
    void save_rotate_ki(QHash<QPoint, ki_gegner_att>& gegner_hash, ki_gegner_att& gegner,
                        const QPoint& init_point);

    // execute refresh_points and update QHash robot_point which contains real time coordinate of robots and their enum
    void robot_refresh_ki(QHash<QPoint, robot::robot_enum> &robot_list, const QPoint& curr_point,
                          const QPoint& next_point, robot::robot_enum robot_id,
                          QHash<QPoint, ki_gegner_att>& gegner_h, const map& ki_map);

    // checks whether robot is DIGGER and move is valid
    bool check_before_move(robot::robot_enum robot_id, tile::tile_enum next_tile,
                           robot::robot_enum next_robot);

    // NO NEED REFERENCES FOR PRIMITIVE TYES SUCH AS ENUMS

    // find and determine the state of robot with regards to its position and PATCH BOT position
    ki_gegner_att::state find_state(ki_gegner_att &gegner);

    // executes bresenham algorithm to find if patch bot is visible to current gegner bot
    bool bresenham_path(const QPoint& start, const QPoint& dest, const map& ki_map,
                                   const QHash<QPoint, robot::robot_enum>& robot_hash);

    // using Relation djkstra path, finds whether robot should move or not. Returns the second next point to use when needed
    void reached(QHash<QPoint, std::tuple<QPoint, int>>& djkstra_relation, ki_gegner_att& gegner
                              , const QHash<QPoint, robot::robot_enum> &robot_list);

    // move gegner bots with legs, with regards to saved Relation djkstra path and position of other bots
    bool bot_leg_update(QHash<QPoint, std::tuple<QPoint, int>>& djkstra_relation,
                              ki_gegner_att& gegner, QHash<QPoint, robot::robot_enum> &robot_point,
                              map& ki_map, QHash<QPoint, ki_gegner_att>& gegner_hash, QHash<QPoint, door> &d_vec);

    // finds current robot enum and returns it as string
    QString my_robot_id(const ki_gegner_att& curr_id);

    // finds current robot state and returns it as string
    QString my_robot_state(const ki_gegner_att& curr_id);



private:
    // gegner robot object
    ki_gegner_att curr_ki;

    // next direction of movement
    bool north;
    bool south;
    bool east;
    bool west;

    // used to find if PATCH BOT destroyed
    bool failed;

    QHash<QPoint, std::tuple<QPoint, int>> relation;    // QHash data structure to store parent of each accessable point on map and shortest distance to patch bot
    QHash<QPoint, std::tuple<QPoint, int>> temp_relation; // to store temperory paths
    QVector<QPair<QPoint, int>> dstr_unvisited_vec;     // QVector data structure of points to visit next, distance increases incremently (lowest to highest)
    QHash<QPoint, int> dstr_visited;                    // QHash data structure of parent points already visited

    // save current real time point of PATCH BOT
    QPoint my_patch_bot_coor;

};


#endif // KI_GEGNER_H
