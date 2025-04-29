#include <QThread>
#include <QDebug>

#include "map.h"
#include "ki_gegner.h"
#include "presenter.h"

/////////////////////////////////////////////////////////////
//
//
//  Class: ki_gegner
//
//
/////////////////////////////////////////////////////////////
// class KI_GEGNER used to implement KI algorithm to change states of gegner robot objects generated from ki_gegner_att class
// each robot state is checked and updated once at each call of the "gegner_bot_ki" function
// BUGGER robots are rotated and moved when walls are not found
// PUSHER, DIGGER, and SWIMMER are moved vertically or horizontally depending of position of PATCH BOT
// Robots do not move when blocked and states are changed
// Next move coordinates are validated with regards to current map
// QHASH ROBOT_HASH contains up-to-date gegner robot location and their respective robot enums of all robots
// QHASH GEGNER_HASH contains up-to-date gegner robot location and their respective updated attributes (from class ki_gegner_att)


// non danger bots dont work well
// danger bots dont open doors
// only hunter bot effectively destroys patch bot

// main AI function used to change states of each gegner robot and update position
QHash<QPoint, std::tuple<QPoint, int>> ki_gegner::gegner_bot_ki(QHash<QPoint, robot::robot_enum>& robot_hash, map& ki_map,
                         QHash<QPoint, ki_gegner_att>& gegner_hash,
                         QHash<QPoint, door> &d_vec, QPoint& patch_bot_old,
                         QHash<QPoint, std::tuple<QPoint, int>>& relation_hash){
    // initialise failed
    ki_gegner::failed = false;
    // save old version of robot_hash, which contains coordinates of all robots
    QHash<QPoint, robot::robot_enum> robot_point = robot_hash;
    ki_gegner::relation = relation_hash;
    ki_gegner::my_patch_bot_coor = patch_bot_old;
    // save old version of map (includes environment and changes to wand_FelsBraun to hind_schrotter)
    // gegner_hash for loop iterates for each gegner robot
    djkstra curr_path;

    qDebug() << "-----------------------------------------------------------------------------";

    for (auto i = gegner_hash.cbegin(), end = gegner_hash.cend(); i != end; ++i){

        // if failed end run
        if(ki_gegner::failed){
            break;
        }

        // initialise next move validator
        ki_gegner::north = false;
        ki_gegner::south = false;
        ki_gegner::east  = false;
        ki_gegner::west  = false;

        // save object of current gegner robot
        curr_ki = gegner_hash.value(i.key());
        // save initial coordinate of current gegner robot8
        QPoint initial_point = i.key();
        int x = curr_ki.current_point.x();
        int y = curr_ki.current_point.y();

        if(curr_ki.reached){
            temp_relation.clear();
            curr_path.djkstra_map_gen(ki_map, dstr_unvisited_vec,
                                      dstr_visited, temp_relation,
                                      my_patch_bot_coor, robot_point);
            relation = temp_relation;
        }

        if(gegner_hash.size() < 10){
            qDebug() << ki_gegner::my_robot_id(curr_ki) << "STATE : " + ki_gegner::my_robot_state(curr_ki) ;
        }


        // if current gegner robot has already been checked and re-entered due to changes from another gegner robot, skip
        if(curr_ki.checked == true){
            continue;
        }
        // set checked to true for current gegner robot being gegner_bots_kid
        curr_ki.checked = true;

        if(curr_ki.robot_id == robot::patch_bot){
            continue;
        }

        // determine whether to delay or not, with regards to whether current gegner robot has wheels or legs
        if(curr_ki.wheels == true && ki_map.tile_vec[y][x] == tile::hind_Aliengras && curr_ki.delay == false){
            curr_ki.delay = true;
            ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
            continue;
        } else if(curr_ki.legs == true && ki_map.tile_vec[y][x] == tile::hind_Schrotter && curr_ki.delay == false){
            curr_ki.delay = true;
            ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
            continue;
        } else if(curr_ki.delay == true){
            curr_ki.delay = false;
            ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
        }

        // if current gegner robot is BUGGER
        if(curr_ki.robot_id == robot::bot_Bugger){
            curr_ki.next_point = curr_ki.N;
            // find if EAST coordinate of current is a wall or not
            bool wall_east = ki_gegner::coor_is_wall(ki_map, curr_ki.E);
            // find if next coordinate of current is a wall or not
            bool wall_next = ki_gegner::coor_is_wall(ki_map, curr_ki.next_point);
            // if state is "FOLLOW_WALL"
            if(curr_ki.current_state == ki_gegner_att::state::follow_wall){
                    if(wall_east){                          // if EAST is wall
                            if (wall_next){                 // if next point is wall
                                // change state to "FOLLOW_WALL"
                                curr_ki.current_state = ki_gegner_att::state::follow_wall;
                                ki_gegner::save_rotate_ki(gegner_hash, curr_ki, curr_ki.current_point);
                            // if another robot is at next move point and start point != next point
                            } else if (robot_point.contains(curr_ki.next_point)
                                       && curr_ki.next_point != curr_ki.start_point){
                                curr_ki.delay = true;
                                continue;                   // go to next iteration
                            // if no robot at next point and start point != next point
                            } else if(!(robot_point.contains(curr_ki.next_point))
                                       && curr_ki.next_point != curr_ki.start_point){
                                // move BUGGER gegner robot to next move position
                                ki_gegner::robot_refresh_ki(robot_point, curr_ki.current_point,
                                                            curr_ki.next_point, curr_ki.robot_id, gegner_hash, ki_map);
                            }
                    // if EAST position is not wall or start point == next point
                    } else if(!(wall_east) || curr_ki.next_point == curr_ki.next_point){
                            // change state to "FIND_ANOTHER_WALL"
                            curr_ki.current_state = ki_gegner_att::state::find_another_wall;
                            ki_gegner::save_rotate_ki(gegner_hash, curr_ki, curr_ki.current_point);
                    }
            // if state is "FIND_ANOTHER_WALL"
            } else if(curr_ki.current_state == ki_gegner_att::state::find_another_wall){
                    if (!(wall_next)){                      // if next point is not wall
                            if(robot_point.contains(curr_ki.next_point)){   // if robot at next point
                                continue;                   // go to next iteration
                            } else {                        // if no robot at next point
                                // move BUGGER gegner robot to next move position
                                ki_gegner::robot_refresh_ki(robot_point, curr_ki.current_point,
                                                            curr_ki.next_point, curr_ki.robot_id, gegner_hash, ki_map);
                            }
                    } else if ((wall_next)){                // if next point is wall
                        // change state to "FOLLOW_WALL"
                        curr_ki.current_state = ki_gegner_att::state::follow_wall;
                        ki_gegner::save_rotate_ki(gegner_hash, curr_ki, curr_ki.current_point);
                    }
            }
        // if current gegner robot is PUSHER, DIGGER, or SWIMMER
        } else if(curr_ki.robot_id == robot::bot_Pusher
                   || curr_ki.robot_id == robot::bot_Digger
                   || curr_ki.robot_id == robot::bot_Swimmer){
            ki_gegner_att::state opposite_state = find_state(curr_ki);
            ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
            if(ki_gegner::valid_point(curr_ki.next_point, ki_map, curr_ki.robot_id)){
                // find next tile enum
                auto& next_tile_coor = ki_map.tile_vec[curr_ki.next_point.y()][curr_ki.next_point.x()];
                // if EAST or WEST close to patch_bot OR next move position is wall
                // OR next move position is another robot AND gegner robot is blocked
                if(ki_gegner::robot_blocked(curr_ki, ki_map, robot_point)
                    || ki_gegner::patchy_coor_reached(curr_ki.current_state, curr_ki, curr_ki.next_point)
                    || next_tile_coor == tile::wand_BetonGrau
                    || next_tile_coor == tile::hind_Geheimgange
                    || (robot_point.contains(curr_ki.next_point)
                        && ki_gegner::robot_blocked(curr_ki, ki_map, robot_point))){
                    // change state to "MOVE_VERTICALLY"
                    curr_ki.current_state = opposite_state;
                    try{
                        // save changes
                        ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                    } catch (const std::exception& e) {
                        std::cerr << e.what() << std::endl;
                    }
                    // go to next iteration
                    continue;
                }
                QPoint dir;
                // find the second next point
                if(ki_gegner::east){
                    QPoint E(curr_ki.next_point.x()-1,curr_ki.next_point.y());
                    dir = E;
                } else if(ki_gegner::west){
                    QPoint W(curr_ki.next_point.x()+1,curr_ki.next_point.y());
                    dir = W;
                } else if(ki_gegner::north){
                    QPoint N(curr_ki.next_point.x(),curr_ki.next_point.y()-1);
                    dir = N;
                } else if(ki_gegner::south){
                    QPoint S(curr_ki.next_point.x(),curr_ki.next_point.y()+1);
                    dir = S;
                }
                // if next tile is gefahr_Abgrund
                if(next_tile_coor == tile::gefahr_Abgrunde){
                    // change state to destroy
                    curr_ki.current_state = ki_gegner_att::state::destroy;
                    try{
                        // save changes
                        ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                        ki_gegner::robot_refresh_ki(robot_point, curr_ki.current_point, curr_ki.next_point,
                                                    curr_ki.robot_id, gegner_hash, ki_map);
                        continue;
                    } catch (const std::exception& e) {
                        std::cerr << e.what() << std::endl;
                    }
                }
                // find if next point is door and open
                try{
                    if((next_tile_coor == tile::tur_Blau || next_tile_coor == tile::tur_Rot)
                        && curr_ki.delay == false){
                        curr_ki.delay = true;
                        open_door(curr_ki.next_point, d_vec, ki_map);
                        ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                        continue;
                    }
                    // find robot in front and move it
                    if(!(ki_gegner::robot_blocked(curr_ki, ki_map, robot_point))
                        && robot_point.contains(curr_ki.next_point)){
                            robot::robot_enum next_robot = robot_point.value(curr_ki.next_point);
                        if(!(ki_gegner::check_before_move(curr_ki.robot_id, next_tile_coor, next_robot))){
                            if(!ki_gegner::valid_point(dir, ki_map, curr_ki.robot_id)){
                                continue;
                            }
                            ki_gegner::robot_refresh_ki(robot_point, curr_ki.next_point,
                                                        dir, next_robot, gegner_hash, ki_map);
                        }
                     }
                    // move current robot
                    ki_gegner::robot_refresh_ki(robot_point, curr_ki.current_point, curr_ki.next_point,
                                                 curr_ki.robot_id, gegner_hash, ki_map);
                } catch (const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
            } else {
                // set opposite state to current state
                curr_ki.current_state = opposite_state;
                try{
                    // save changes
                    ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                } catch (const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
            }
        // if current gegner robot is FOLLOWER, HUNTER or SNIFFER
        } else if(curr_ki.robot_id == robot::bot_Follower
                   || curr_ki.robot_id == robot::bot_Hunter
                   || curr_ki.robot_id == robot::bot_Sniffer){
            // save initial point
            initial_point = curr_ki.current_point;
            // check if vision of PATCH BOT is blocked
            if(ki_gegner::bresenham_path(curr_ki.current_point,
                                          ki_gegner::my_patch_bot_coor, ki_map, robot_point)){
                curr_ki.visible = true;
            } else {
                curr_ki.visible = false;
            }
            // check if path is blocked
            if(relation.contains(curr_ki.current_point)){
                curr_ki.reachable = true;
            } else {
                curr_ki.reachable = false;
            }
            // save changes
            ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
            // if current state is WAITING
            if(curr_ki.current_state == ki_gegner_att::state::waiting){
                if(curr_ki.reachable && curr_ki.robot_id == robot::bot_Sniffer){
                    curr_ki.current_state = ki_gegner_att::state::follow_path;
                    // save changes
                    ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                } else if(curr_ki.reachable && curr_ki.visible){
                    curr_ki.current_state = ki_gegner_att::state::follow_path;
                    if(curr_ki.robot_id == robot::bot_Hunter){
                        // generate and save new path
                        curr_path.djkstra_map_gen(ki_map, dstr_unvisited_vec,
                                                  dstr_visited, relation,
                                                  my_patch_bot_coor, robot_point);
                    }
                }
                // save changes
                ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
            // if current state is FOLLOW_PATH
            } else if(curr_ki.current_state == ki_gegner_att::state::follow_path){
                if(curr_ki.reachable && curr_ki.robot_id == robot::bot_Sniffer){
                    // if gegner robot not moved go to next iteration
                    if(!ki_gegner::bot_leg_update(relation, curr_ki, robot_point, ki_map, gegner_hash, d_vec)){
                        continue;
                    }
                } else if(curr_ki.reachable && curr_ki.visible && curr_ki.robot_id == robot::bot_Follower){
                    // if gegner robot not moved go to next iteration
                    if(!ki_gegner::bot_leg_update(relation, curr_ki, robot_point, ki_map, gegner_hash, d_vec)){
                        continue;
                    }
                } else if(curr_ki.reachable && curr_ki.visible && curr_ki.robot_id == robot::bot_Hunter){
                    // if gegner robot not moved go to next iteration
                    curr_path.djkstra_map_gen(ki_map, dstr_unvisited_vec,
                                              dstr_visited, relation,
                                              my_patch_bot_coor, robot_point);
                    if(!ki_gegner::bot_leg_update(relation, curr_ki, robot_point, ki_map, gegner_hash, d_vec)){
                        ki_gegner::save_ki(gegner_hash, curr_ki, curr_ki.current_point);
                        continue;
                    }
                    if(!ki_gegner::bot_leg_update(relation, curr_ki, robot_point, ki_map, gegner_hash, d_vec)){
                        ki_gegner::save_ki(gegner_hash, curr_ki, curr_ki.current_point);
                        continue;
                    }
                } else {
                    if(!curr_ki.reachable && curr_ki.robot_id == robot::bot_Sniffer){
                        curr_ki.current_state = ki_gegner_att::state::waiting;
                        // save changes
                        ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                    } else if(curr_ki.robot_id == robot::bot_Follower && (!curr_ki.reachable || !curr_ki.visible)){
                        curr_ki.current_state = ki_gegner_att::state::waiting;
                        // save changes
                        ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                    } else if(curr_ki.robot_id == robot::bot_Hunter && (!curr_ki.reachable || !curr_ki.visible)){
                        curr_ki.current_state = ki_gegner_att::state::hunt;
                        // save changes
                        ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                    }
                }
            // if current state is HUNT
            } else if(curr_ki.current_state == ki_gegner_att::state::hunt){
                // if current gegner robot reached PATCHBOT or end of path, set state to waiting
                if(curr_ki.reached){
                    curr_ki.current_state = ki_gegner_att::state::waiting;
                    // save changes
                    ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                } else if(!curr_ki.reachable || !curr_ki.visible){
                    // execute move twice here
                    // if gegner robot not moved go to next iteration
                    if(!ki_gegner::bot_leg_update(relation, curr_ki, robot_point, ki_map, gegner_hash, d_vec)){
                        ki_gegner::save_ki(gegner_hash, curr_ki, curr_ki.current_point);
                        continue;
                    }
                    if(!ki_gegner::bot_leg_update(relation, curr_ki, robot_point, ki_map, gegner_hash, d_vec)){
                        ki_gegner::save_ki(gegner_hash, curr_ki, curr_ki.current_point);
                        continue;
                    }
                } else if(curr_ki.reachable && curr_ki.visible){
                    curr_path.djkstra_map_gen(ki_map, dstr_unvisited_vec,
                                              dstr_visited, relation,
                                              my_patch_bot_coor, robot_point);
                    curr_ki.current_state = ki_gegner_att::state::follow_path;
                    // save changes
                    ki_gegner::save_ki(gegner_hash, curr_ki, initial_point);
                    // generate and save new path
                    curr_path.djkstra_map_gen(ki_map, dstr_unvisited_vec,
                                              dstr_visited, relation,
                                              my_patch_bot_coor, robot_point);
                }
            }
        }
    }

    if(ki_gegner::failed){
        ki_map.failed = true;
        return relation;
    }

    // reset checked gegner robots
    for (auto i = gegner_hash.cbegin(), end = gegner_hash.cend(); i != end; ++i){
        if(i.value().checked == true){
            i.value().checked = false;
        }
    }

    // update previous versions of variables
    patch_bot_old = ki_gegner::my_patch_bot_coor;
    robot_hash = robot_point;
    relation_hash = ki_gegner::relation;

    return relation;
}

// move gegner bots with legs, with regards to saved Relation djkstra path and position of other bots
bool ki_gegner::bot_leg_update(QHash<QPoint, std::tuple<QPoint, int>>& djkstra_relation,
                          ki_gegner_att& gegner, QHash<QPoint, robot::robot_enum>& robot_point,
                          map& ki_map, QHash<QPoint, ki_gegner_att>& gegner_hash,
                          QHash<QPoint, door> &d_vec){

    // save second next point and find if robot can be moved or not
    ki_gegner::reached(djkstra_relation, gegner, robot_point);
    tile::tile_enum next_tile_coor = ki_map.tile_vec[gegner.next_point.y()][gegner.next_point.x()];
    djkstra ki_path;
    if(gegner.reached){
        return false;
    }
    // check if PATCHBOT has been reached, setting failed = true ends current run
    if(gegner.next_point == ki_gegner::my_patch_bot_coor){
        gegner.reached = true;
        ki_gegner::failed = true;
        return false;
    // check for doors and open them
    } else if((next_tile_coor == tile::tur_Blau || next_tile_coor == tile::tur_Rot)
        && gegner.delay == false){
        gegner.delay = true;
        ki_gegner::open_door(gegner.next_point, d_vec, ki_map);
        ki_gegner::save_ki(gegner_hash, gegner, gegner.current_point);
        // return false to insert delay for next move
        return false;
     // if move is true then move
    } else if(gegner.move == true && gegner.reached == false){
        // after moving gegner bot in front, move self
        ki_gegner::robot_refresh_ki(robot_point, gegner.current_point, gegner.next_point,
                                    gegner.robot_id, gegner_hash, ki_map);
        // assign new current position
        curr_ki = gegner_hash.value(curr_ki.next_point);
    } else if(gegner.move == false && gegner.reached == false
               && gegner.robot_id == robot::bot_Hunter){
        ki_path.djkstra_map_gen(ki_map, dstr_unvisited_vec,
                                  dstr_visited, djkstra_relation,
                                  my_patch_bot_coor, robot_point);
    }
    return true;
}

// Using Relation djkstra path, find whether robot should move or not. Returns the second next point to use when needed
void ki_gegner::reached(QHash<QPoint, std::tuple<QPoint, int>>& djkstra_relation,
                          ki_gegner_att& gegner, const QHash<QPoint, robot::robot_enum>& robot_list){
    QPoint init_point;
    // set initial robot point
    QPoint next;
    int next_dist;

    // set initial point
    init_point = gegner.current_point;
    // if RELATION djkstra path is not empty
    if(!(djkstra_relation.empty())){
        // find next position and distance
        std::tie(next, next_dist) = djkstra_relation.value(init_point);
        gegner.next_point = next;
        // if next point is PATCH BOT, set reached to true. Set failed to true to end current run
        if(gegner.next_point == ki_gegner::my_patch_bot_coor){
            gegner.reached = true;
            ki_gegner::failed = true;
            return;
        }
        // if next dist is > 0, final dest isnt reached yet
        if(next_dist > 0){
            // initialising reached to false
            gegner.reached = false;
            // if no robot infront, then move
            if(!(robot_list.contains(next))){
                gegner.move = true;
            // if robot in front and second next point is end of path, wait
            } else {
                gegner.move = false;
            }
            return;
        // if next_dist = 0, then end of path has been reached
        } else if(next_dist == 0){
            gegner.reached = true;
            return;
        }
    }
    gegner.reached = true;
    return;

}

// find and determine the state of robot with regards to its position and PATCH BOT position
ki_gegner_att::state ki_gegner::find_state(ki_gegner_att& gegner){
    ki_gegner_att::state temp_state = ki_gegner_att::state::destroy;
    if(gegner.current_state == ki_gegner_att::state::move_horizontally){
        // if patchbot is left
        if(ki_gegner::my_patch_bot_coor.x() <= gegner.E.x()){
            // set next move position to EAST
            ki_gegner::east = true;
            gegner.next_point = gegner.E;
            // if patchbot is right
        } else {
            // set next move position to WEST
            ki_gegner::west = true;
            gegner.next_point = gegner.W;
        }
        temp_state = ki_gegner_att::state::move_vertically;
    } else if(gegner.current_state == ki_gegner_att::state::move_vertically){
        if(ki_gegner::my_patch_bot_coor.y() <= gegner.N.y()){
            // set next move position to NORTH
            ki_gegner::north = true;
            gegner.next_point = gegner.N;
            // if patchbot is down
        } else {
            // set next move position to SOUTH
            ki_gegner::south = true;
            gegner.next_point = gegner.S;
        }
        temp_state = ki_gegner_att::state::move_horizontally;
    }

    return temp_state;
}

// checks whether robot is DIGGER and move is valid
bool ki_gegner::check_before_move(robot::robot_enum robot_id, tile::tile_enum next_tile,
                                  robot::robot_enum next_robot){
    if(robot_id != robot::bot_Digger &&  next_tile == tile::wand_FelsBraun){
        // go to next iteration
        return true;
        // if current gegner robot is DIGGER and next gegner robot is also DIGGER
    } else if(robot_id == robot::bot_Digger && next_robot != robot::bot_Digger
               &&  next_tile == tile::wand_FelsBraun){
        // go to next iteration
        return true;
    }
    return false;
}

// finds current robot enum and returns it as string
QString ki_gegner::my_robot_id(const ki_gegner_att& curr_id){
    QString id;
    if(curr_id.robot_id == robot::bot_Bugger){
        id = "BUGGER";
    } else if(curr_id.robot_id == robot::bot_Pusher){
        id = "PUSHER";
    } else if(curr_id.robot_id == robot::bot_Digger){
        id = "DIGGER";
    } else if(curr_id.robot_id == robot::bot_Swimmer){
        id = "SWIMMER";
    } else if(curr_id.robot_id == robot::bot_Follower){
        id = "FOLLOWER";
    } else if(curr_id.robot_id == robot::bot_Hunter){
        id = "HUNTER";
    } else if(curr_id.robot_id == robot::bot_Sniffer){
        id = "SNIFFER";
    } else if(curr_id.robot_id == robot::patch_bot){
        id = "PATCHBOT";
    }

    return id;
}

// finds current robot state and returns it as string
QString ki_gegner::my_robot_state(const ki_gegner_att& curr_id){
    QString state;
    if(curr_id.current_state == ki_gegner_att::state::follow_path){
        state = "FOLLOW_PATH" ;
    } else if(curr_id.current_state == ki_gegner_att::state::hunt){
        state = "HUNT" ;
    } else if(curr_id.current_state == ki_gegner_att::state::destroy){
        state = "DESTROY" ;
    } else if(curr_id.current_state == ki_gegner_att::state::move_horizontally){
        state = "MOVE_HORIZONTALLY" ;
    } else if(curr_id.current_state == ki_gegner_att::state::move_vertically){
        state = "MOVE_VERTICALLY" ;
    } else if(curr_id.current_state == ki_gegner_att::state::find_another_wall){
        state = "FIND_ANOTHER_WALL" ;
    } else if(curr_id.current_state == ki_gegner_att::state::follow_wall){
        state = "FOLLOW_WALL" ;
    } else if(curr_id.current_state == ki_gegner_att::state::hero){
        state = "SAVING THE DAY" ;
    } else if(curr_id.current_state == ki_gegner_att::state::waiting){
        state = "WAITING" ;
    }
    return state;
}

// execute refresh_points and update QHash robot_point which contains real time coordinate of robots and their enum
void ki_gegner::robot_refresh_ki(QHash<QPoint, robot::robot_enum>& robot_list,
                                 const QPoint& curr_point, const QPoint& next_point,
                                 robot::robot_enum robot_id, QHash<QPoint, ki_gegner_att>& gegner_h,
                                 const map& ki_map){
    try{
        if(robot_id == robot::patch_bot){
            // set next point to self NORTH
            ki_gegner::my_patch_bot_coor = next_point;
        }
        robot_list.remove(curr_point);
        robot_list.insert(next_point, robot_id);
        refresh_points(curr_point, next_point, gegner_h, ki_map);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

// execute rotate and save function
void ki_gegner::save_rotate_ki(QHash<QPoint, ki_gegner_att>& gegner_hash,
                               ki_gegner_att& gegner, const QPoint& init_point){

    if(gegner.current_state == ki_gegner_att::state::follow_wall){
        // set start position
        gegner.start_point = gegner.current_point;
    }
    // save changes
    ki_gegner::save_ki(gegner_hash, gegner, init_point);
    // rotate BUGGER gegner robot
    ki_gegner::rotate(gegner.current_point, gegner_hash);
}

// removes old version of gegner robot and inserts new version of gegner robot in gegner_hash
void ki_gegner::save_ki(QHash<QPoint, ki_gegner_att>& gegner_hash,
                        const ki_gegner_att& gegner, const QPoint& init_point){
    gegner_hash.remove(init_point);
    gegner_hash.insert(gegner.current_point, gegner);
}

// initial function of checking if robot is blocked, while using the next direction of robot, call snd_robot_blocked
bool ki_gegner::robot_blocked(ki_gegner_att& gegner, const map& ki_map,
                              const QHash<QPoint, robot::robot_enum>& robot_hash){
    // intialise blocked as true
    bool blocked = true;
    // check if next point with map borders
    try{
        if(gegner.next_point.y() >= 0 && gegner.next_point.x() >= 0
            && gegner.next_point.x() <= ki_map.width - 1 && gegner.next_point.y() <= ki_map.height - 1){
            // if next point is NORTH
            if(gegner.next_point == gegner.N){
                // set second next NORTH point
                QPoint snd_dir_point(gegner.N.x(),gegner.N.y()-1);
                // find if second next point is blocked
                blocked = ki_gegner::snd_robot_blocked(gegner.N, snd_dir_point,
                                                       ki_map, robot_hash, gegner.robot_id);
                return blocked;
                // if next point is WEST
            } else if(gegner.next_point == gegner.W ){
                // set second next WEST point
                QPoint snd_dir_point(gegner.W.x()+1,gegner.W.y());
                // find if second next point is blocked
                blocked = ki_gegner::snd_robot_blocked(gegner.W, snd_dir_point,
                                                       ki_map, robot_hash, gegner.robot_id);
                return blocked;
                // if next point is SOUTH
            } else if(gegner.next_point == gegner.S ){
                // set second next SOUTH point
                QPoint snd_dir_point(gegner.S.x(),gegner.S.y()+1);
                // find if second next point is blocked
                blocked = ki_gegner::snd_robot_blocked(gegner.S, snd_dir_point,
                                                       ki_map, robot_hash, gegner.robot_id);
                return blocked;
                // if next point is EAST
            } else if(gegner.next_point == gegner.E ){
                // set second next EAST point
                QPoint snd_dir_point(gegner.E.x()-1,gegner.E.y());
                // find if second next point is blocked
                blocked = ki_gegner::snd_robot_blocked(gegner.E, snd_dir_point,
                                                       ki_map, robot_hash, gegner.robot_id);
                return blocked;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return blocked;
}

// checks whether robot is blocked, using QHash of up-to-date coordinates of all robots and environment
bool ki_gegner::snd_robot_blocked(QPoint& next, QPoint& snd_next, const map& ki_map,
                                  const QHash<QPoint, robot::robot_enum>& robot_hash,
                                  robot::robot_enum robot_id){
    // set next robot enum
    robot::robot_enum next_robot = robot_hash.value(next);
    // check if second next point with map borders
    if(snd_next.y() >= 0 && snd_next.x() >= 0 && snd_next.x() <= ki_map.width - 1
                                            && snd_next.y() <= ki_map.height - 1){
        // find tile enum
        auto& tile_coor = ki_map.tile_vec[next.y()][next.x()];
        // find second tile enum
        auto& snd_tile_point = ki_map.tile_vec[snd_next.y()][snd_next.x()];
        // if second next tile is wall and robot in front
        if(snd_tile_point == tile::wand_BetonGrau && robot_hash.contains(next)){
            return true; // blocked
        // if robot in front of the next robot
        } else if(robot_hash.contains(snd_next) && robot_hash.contains(next)){
            return true; // blocked
        // if robot in front and second next tile is wall
        } else if(robot_hash.contains(next) && snd_tile_point == tile::wand_FelsBraun){
            return true; // blocked
            // if next point is wall
        } else if(tile_coor == tile::wand_FelsBraun && robot_id != robot::bot_Digger){
            return true; // blocked
            // robot in front and no robot after it and second tile is valid
        } else if(tile_coor == tile::hind_Geheimgange){
            return true; // blocked
            // robot in front and no robot after it and second tile is valid
        } else if(snd_tile_point == tile::punkte_Hauptserver && robot_hash.contains(next)){
            return true; // blocked
            // robot in front and no robot after it and second tile is valid
        }else if(tile_coor == tile::punkte_Hauptserver){
            return true; // blocked
            // robot in front and no robot after it and second tile is valid
        } else if(snd_tile_point == tile::hind_Geheimgange && next_robot == robot::patch_bot){
            return false; // not blocked
            // robot in front and no robot after it and second tile is valid
        } else if(tile_coor == tile::wand_BetonGrau){
            return true; // blocked
        // robot in front and no robot after it and second tile is valid
        } else if(robot_hash.contains(next) && !robot_hash.contains(snd_next) &&
                   (snd_tile_point != tile::punkte_Hauptserver || snd_tile_point != tile::hind_Geheimgange
                    || (snd_tile_point == tile::wand_FelsBraun && next_robot == robot::bot_Digger))){
            return false; // not blocked
        // if secont next tile is gefahr_Abgrunde and robot in front
        }else if(snd_tile_point == tile::gefahr_Abgrunde && robot_hash.contains(next)){
            return false; // not blocked
        // if second next tile is Water and robot in front
        } else if(snd_tile_point == tile::gefahr_Wasser && robot_hash.contains(next)){
            return false; // not blocked
        }
    }
    return false; // return not blocked
}

// with regards to current state of gegner robots and coordiantes of robot,
// if a coordinate is close to patch bot's coordinate move either vertically or horizontally
bool ki_gegner::patchy_coor_reached(ki_gegner_att::state& curr_state,
                                    const ki_gegner_att& curr_ki, QPoint& next_point){
    // intialise reached as false
    bool p_b_reached = false;
    // if current state is "MOVE_HORIZONTALLY"
    if(curr_state == ki_gegner_att::state::move_horizontally){
        // if x coordinate is with map borders
        if((curr_ki.current_point.x() >= ki_gegner::my_patch_bot_coor.x()
             && curr_ki.current_point.x() <= ki_gegner::my_patch_bot_coor.x() )){
            // if patchbot is up
            if(ki_gegner::my_patch_bot_coor.y() <= curr_ki.N.y()){
                // set next point to NORTH
                next_point = curr_ki.N;
                // if patchbot is down
            } else {
                //  set next point to SOUTH
                next_point = curr_ki.S;
            }
            // set current state to "MOVE_VERTICALLY"
            curr_state = ki_gegner_att::state::move_vertically;
            // set reached to true
            p_b_reached = true;
        } else{
            // if patchbot is left
            if(ki_gegner::my_patch_bot_coor.x() <= curr_ki.E.x()){
                // set next point to EAST
                next_point = curr_ki.E;
                // if patchbot is right
            } else {
                // set next point to WEST
                next_point = curr_ki.W;
            }
        }
    // if current state is "MOVE_VERTICALLY"
    } else if(curr_state == ki_gegner_att::state::move_vertically){
        // if y coordinate is within map borders
        if((curr_ki.current_point.y() >= ki_gegner::my_patch_bot_coor.y()
             && curr_ki.current_point.y() <= ki_gegner::my_patch_bot_coor.y())){
            // if patchbot is left
            if(ki_gegner::my_patch_bot_coor.x() <= curr_ki.E.x()){
                // set next point to EAST
                next_point = curr_ki.E;
                // if patchbot is right
            } else {
                next_point = curr_ki.W;
            }
            // set current state to "MOVE_HORIZONTALLY"
            curr_state = ki_gegner_att::state::move_horizontally;
            // set reached to true
            p_b_reached = true;
        } else{
            // if patchbot is up
            if(ki_gegner::my_patch_bot_coor.y() <= curr_ki.N.y()){
                // set next point to NORTH
                next_point = curr_ki.N;
                // if patchbot is down
            } else {
                // set next point to SOUTH
                next_point = curr_ki.S;
            }
        }
    }
    return p_b_reached;

}

// checks whether input coordinate is a wall, used for BUGGER robot
bool ki_gegner::coor_is_wall(const map& ki_map, const QPoint& dir){
    // initialise wall to false
    bool wall = false;
    if(dir.y() >= 0 && dir.x() >= 0
        && dir.x() <= ki_map.width - 1 && dir.y() <= ki_map.height - 1){
        auto& wall_tile_coor = ki_map.tile_vec[dir.y()][dir.x()];
        // check if wall for BUGGER
        if(wall_tile_coor == tile::gefahr_Wasser || wall_tile_coor == tile::gefahr_Abgrunde
            || wall_tile_coor == tile::punkte_Hauptserver || wall_tile_coor == tile::hind_Geheimgange
            || wall_tile_coor == tile::tuer_automatisch_offen || wall_tile_coor == tile::tuer_manuell_offen
            || wall_tile_coor == tile::tur_Blau || wall_tile_coor == tile::tur_Rot
            || wall_tile_coor == tile::wand_BetonGrau || wall_tile_coor == tile::wand_FelsBraun){
            wall = true;
        }
    }

    return wall;
}

// rotate robot at "old" coordinate clockwise
void ki_gegner::rotate(const QPoint& old, QHash<QPoint, ki_gegner_att> &gegner_hash){
    // initialise gegner robot object
    ki_gegner_att curr_gegner;
    // save current gegner robot object
    curr_gegner = gegner_hash.value(old);
    curr_gegner.current_point = gegner_hash.value(old).current_point;
    // rotate coordinates clockwise
    curr_gegner.S = gegner_hash.value(old).E;
    curr_gegner.N = gegner_hash.value(old).W;
    curr_gegner.E = gegner_hash.value(old).N;
    curr_gegner.W = gegner_hash.value(old).S;
    // BUGGER gegner robot always moves forward, hence next move coordinate is new NORTH coordinate
    curr_gegner.next_point = curr_gegner.N;
    // update gegner object in QHash gegner_hash
    gegner_hash.remove(old);
    gegner_hash.insert(curr_gegner.current_point, curr_gegner);
}

// find initial location of robots on map and save current coordinates with generated ki_gegner_att object
QHash<QPoint, ki_gegner_att> ki_gegner::locate_gegner(const QHash<QPoint, robot::robot_enum>& robot_hash,
                                                      const map& ki_map){
    // create temp ki_gegner_att object
    ki_gegner_att temp_gegner;
    // create temp gegner_hash
    QHash<QPoint, ki_gegner_att> gegner_hash;

    // create ki_gegner_att objects for selected robots
    for (auto i = robot_hash.cbegin(), end = robot_hash.cend(); i != end; ++i){
            // save current robot enum
            robot::robot_enum bot_enum = i.value();
                QPoint coor = i.key();
                int x = coor.x();
                int y = coor.y();
                // create gegner object
                temp_gegner.robot_id = bot_enum;
                temp_gegner.current_point = i.key();
                // set axis coordinates
                temp_gegner.S = QPoint(x,y+1);
                temp_gegner.N = QPoint(x,y-1);
                temp_gegner.E = QPoint(x-1,y);
                temp_gegner.W = QPoint(x+1,y);
                // if gegner robot is DIGGER, SWIMMER, or PUSHER
                if(bot_enum == robot::bot_Bugger){
                    // initialise current state to "FOLLOW_WALL"
                    temp_gegner.current_state = ki_gegner_att::state::follow_wall;
                    // wheels is false and legs false
                    temp_gegner.wheels = false;
                    temp_gegner.legs = false;
                    // initialise next point to self north direction
                    if(valid_point(temp_gegner.N, ki_map, temp_gegner.robot_id) == true){
                        temp_gegner.next_point = temp_gegner.N;
                    }
                    // initialise start point to starting position
                    temp_gegner.start_point = temp_gegner.current_point;
                } else if(temp_gegner.robot_id == robot::bot_Digger
                    || temp_gegner.robot_id == robot::bot_Swimmer
                    || temp_gegner.robot_id == robot::bot_Pusher){
                    // otherwise initialise current state to "MOVE_HORIZONTALLY"
                    temp_gegner.current_state = ki_gegner_att::state::move_horizontally;
                    // wheels is true and legs false
                    temp_gegner.wheels = true;
                    temp_gegner.legs = false;
                    // initialise next point to any valid axis coordinate
                    if(valid_point(temp_gegner.S, ki_map, temp_gegner.robot_id) == true){
                        temp_gegner.next_point = temp_gegner.S;
                    } else if(valid_point(temp_gegner.N, ki_map, temp_gegner.robot_id) == true){
                        temp_gegner.next_point = temp_gegner.N;
                    } else if(valid_point(temp_gegner.E, ki_map, temp_gegner.robot_id) == true){
                        temp_gegner.next_point = temp_gegner.E;
                    } else if(valid_point(temp_gegner.W, ki_map, temp_gegner.robot_id) == true){
                        temp_gegner.next_point = temp_gegner.W;
                    }
                // if gegner robot is HUNTER, SNIFFER or FOLLOWER
                } else if (temp_gegner.robot_id == robot::bot_Hunter
                           || temp_gegner.robot_id == robot::bot_Sniffer
                           || temp_gegner.robot_id == robot::bot_Follower){
                    // otherwise initialise current state to "MOVE_HORIZONTALLY"
                    temp_gegner.current_state = ki_gegner_att::state::waiting;
                    // wheels is false and legs true
                    temp_gegner.wheels = false;
                    temp_gegner.legs = true;
                } else if (temp_gegner.robot_id == robot::patch_bot){
                    // otherwise initialise current state to "MOVE_HORIZONTALLY"
                    temp_gegner.current_state = ki_gegner_att::state::hero;
                    // wheels is false and legs true
                    temp_gegner.wheels = true;
                    temp_gegner.legs = false;
                }
                // initialise checked and delay to false
                temp_gegner.checked = false;
                temp_gegner.delay = false;
                temp_gegner.visible = false;
                temp_gegner.reachable = false;
                temp_gegner.reached = false;
                temp_gegner.move = false;

                // add gegner robot object to temperory QHash gegner hash
                gegner_hash.insert(temp_gegner.current_point, temp_gegner);
    }
    // return created temperory QHash gegner hash
    return gegner_hash;
}

// check if input coordinate is valid with regards to map size
bool ki_gegner::valid_point(const QPoint& curr_coor, const map& ki_map, robot::robot_enum my_id){
    // if point within map borders
    bool valid = false;
    if(curr_coor.y() >= 0 && curr_coor.x() >= 0
        && curr_coor.x() <= ki_map.width - 1 && curr_coor.y() <= ki_map.height - 1){
        valid = true;
            // if tile of point is not wall
        if((ki_map.tile_vec[curr_coor.y()][curr_coor.x()] == tile::wand_FelsBraun && my_id != robot::bot_Digger)
                || (ki_map.tile_vec[curr_coor.y()][curr_coor.x()] == tile::hind_Geheimgange
                && my_id != robot::patch_bot)){
                    valid = false;
            }
        if(ki_map.tile_vec[curr_coor.y()][curr_coor.x()] == tile::wand_BetonGrau){
                valid = false;
            }
    }
    // otherwise return false
    return valid;
}

// provide keys to doors making the cooresponding door open
void ki_gegner::open_door(const QPoint& latest, QHash<QPoint, door> &d_vec, const map& ki_map){
    // if next point is door, add key to door vector
    if (ki_map.tile_vec[latest.y()][latest.x()] == tile::tur_Blau){
        door temp_door = d_vec.value(latest);
        temp_door.door_key = true;
        d_vec.remove(latest);
        d_vec.insert(latest, temp_door);
    } else if(ki_map.tile_vec[latest.y()][latest.x()] == tile::tur_Rot){
        door temp_door = d_vec.value(latest);
        temp_door.door_key = true;
        d_vec.remove(latest);
        d_vec.insert(latest, temp_door);
    }
}

// creates an entire new version of current gegner robot (ki_gegner_att)
// and inserts new version of gegner robot in gegner_hash
void ki_gegner::refresh_points(const QPoint &old, const QPoint &latest,
                               QHash<QPoint, ki_gegner_att> &gegner_hash, const map& ki_map){
    // create new gegner robot object
    ki_gegner_att curr_gegner;
    // save old gegner robot attributes
    curr_gegner = gegner_hash.value(old);

    if(valid_point(latest, ki_map, curr_gegner.robot_id) == false){
        return;
    }
    // find change in coordinates
    int dx = latest.x() - gegner_hash.value(old).current_point.x();
    int dy = latest.y() - gegner_hash.value(old).current_point.y();
    // set change to axis coordinates
    curr_gegner.S = QPoint(gegner_hash.value(old).S.x()+dx,gegner_hash.value(old).S.y()+dy);
    curr_gegner.N = QPoint(gegner_hash.value(old).N.x()+dx,gegner_hash.value(old).N.y()+dy);
    curr_gegner.E = QPoint(gegner_hash.value(old).E.x()+dx,gegner_hash.value(old).E.y()+dy);
    curr_gegner.W = QPoint(gegner_hash.value(old).W.x()+dx,gegner_hash.value(old).W.y()+dy);
    // if current gegner robot is BUGGER ki_gegner::my_patch_bot_coor
    if(gegner_hash.value(old).robot_id == robot::bot_Bugger){
        // set next point to self NORTH

        if(valid_point(curr_gegner.N, ki_map, curr_gegner.robot_id) == true){
            curr_gegner.next_point = curr_gegner.N;
        }
    } else {
        // otherwise set change to next point
        QPoint next(gegner_hash.value(old).next_point.x()+dx,gegner_hash.value(old).next_point.y()+dy);
        if(valid_point(next, ki_map, curr_gegner.robot_id) == true){
            curr_gegner.next_point = QPoint(gegner_hash.value(old).next_point.x()+dx,
                                              gegner_hash.value(old).next_point.y()+dy);
        }
    }
    // set checked true because gegner robot has moved
    curr_gegner.checked = true;

    // set current point to new point
    curr_gegner.current_point = latest;

    gegner_hash.remove(old);
    gegner_hash.insert(latest, curr_gegner);

}

// executes bresenham algorithm to find if patch bot is visible to current gegner bot
bool ki_gegner::bresenham_path(const QPoint& start, const QPoint& dest, const map& ki_map,
                               const QHash<QPoint, robot::robot_enum>& robot_hash){
    // find difference of x and y coordinates
    int dx = abs(dest.x()-start.x());
    int dy = abs(dest.y()-start.y());

    int set_start_x;
    int set_start_y;

    std::vector<QPoint> error_vec;
    // find type of slope and which quadrant form
    if(start.x() < dest.x()){
        set_start_x = 1;
    } else {
        set_start_x = -1;
    }
    if(start.y() < dest.y()){
        set_start_y = 1;
    } else {
        set_start_y = -1;
    }

    // initialise error value
    int error = dx - dy;
    // set current point
    QPoint path = start;

    while (true) {
        // push back generated points to error vector
        error_vec.push_back(path);
        // if destination reached, end
        if (path.x() == dest.x() && path.y() == dest.y()){
            break;
        }
        // generate sub error value
        int temp_error = 2 * error;
        // find next error value with regards to destination and slope
        if (temp_error > -dy) {
            error = error - dy;
            path.setX(path.x() + set_start_x);
        }
        if (temp_error < dx) {
            error = error + dx;
            path.setY(path.y() + set_start_y);
        }
        // if robot at current point and robot is not PATCHBOT and current point != start point, view is blocked
        if((robot_hash.contains(path) && robot_hash.value(path) != robot::patch_bot
             && path != start)
            // if wall at current point, view is blocked
            || ki_map.tile_vec[path.y()][path.x()] == tile::wand_BetonGrau
            || ki_map.tile_vec[path.y()][path.x()] == tile::wand_FelsBraun){
            //qDebug() << "View blocked";
            // return false for blocked view
            return false;
        }
    }
    // otherwise view is not blocked, path generated stored in error vector can be printed
    return true;

}
