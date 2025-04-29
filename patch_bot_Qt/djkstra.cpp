#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QPoint>
#include <tuple>

#include "map.h"
#include "djkstra.h"
#include "presenter.h"

/////////////////////////////////////////////////////////////
//
//
//  Class: djkstra
//
//
/////////////////////////////////////////////////////////////

// Class djkstra used to find path of current gegner bot to PATCH BOT
// mainly used for robots with legs. i.e. FOLLOWER, HUNTER, and SNIFFER

// execute path finding algorithem
void djkstra::djkstra_map_gen(const map& curr_map, QVector<QPair<QPoint, int>>& djkstr_unvisited_vec,
                              QHash<QPoint, int>& djkstr_visited,
                              QHash<QPoint, std::tuple<QPoint, int>>& djkstra_relation,
                              const QPoint& patchy, const QHash<QPoint, robot::robot_enum>& robot_point){

    // clear previous stored data
    djkstra::relation.clear();
    djkstra::dstr_visited.clear();
    djkstra::dstr_unvisited_vec.clear();
    // update robot locations on map in robot vector

    djkstra::dstra_map = curr_map;

    int temp_dist;
    QPoint curr_point;
    // add to unvisited vector

    djkstra::dstr_unvisited_vec.append(QPair(patchy,0));
    // set initial point to patch bot point
    curr_point = patchy;
    // while unvisited vector is not empty, run
    while (!djkstra::dstr_unvisited_vec.empty()){
        // take the first element in unvisited vector of point and set to current point
        curr_point = djkstra::dstr_unvisited_vec[djkstra::dstr_unvisited_vec.size()-1].first;

        std::vector<QPoint> dir;
        // add possible movements to direction vector
        QPoint S(curr_point.x(),curr_point.y()+1);
        QPoint N(curr_point.x(),curr_point.y()-1);
        QPoint E(curr_point.x()-1,curr_point.y());
        QPoint W(curr_point.x()+1,curr_point.y());

        dir.push_back(S);
        dir.push_back(N);
        dir.push_back(E);
        dir.push_back(W);

        // increment distance of parent point for child point
        temp_dist = djkstra::dstr_unvisited_vec[  djkstra::dstr_unvisited_vec.size()-1].second + 1;

        // insert points, if available, in unordered map with current point as parent and relative distance
        try{
            djkstra::djkstra_update_relation(dir, curr_point, temp_dist, robot_point);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    djkstr_unvisited_vec = djkstra::dstr_unvisited_vec;
    djkstr_visited = djkstra::dstr_visited;
    djkstra_relation = djkstra::relation;

    /*
    if(!robot_point.empty()){
        // print robot points and distance to patch bot with qDebug()
        for (auto i = robot_point.cbegin(), end = robot_point.cend(); i != end; ++i){
            QPoint robot_coor(i.key().x(),i.key().y());
            qDebug() << robot_coor << "distance to patch bot:" << djkstra::dstr_visited.value(robot_coor);
            // print path of robots with qDebug()
            //QPoint map_point;
            //QPoint temp_point;
            // temp_point = robot_coor;
            // int map_dist = INT_MAX;
            // while(map_dist != 0){
            //     std::tie(map_point, map_dist) =   djkstra::relation.value(temp_point);
            //     qDebug() << temp_point << "<-" << map_point << map_dist ;
            //     temp_point = map_point;
            // }
        }
    }*/
}

// update branches of each cell, parent and child, with regards to shorter distance
void djkstra::djkstra_update_relation(const std::vector<QPoint>& dir, QPoint parent, int curr_dist
                                      , const QHash<QPoint, robot::robot_enum>& robot_point){

    int map_dist;
    int temp_dist;
    QPoint map_point;
    tile::tile_enum curr_tile;
    bool robot;

    // for each direction point (north, south, east, west)
    // check if valid and update unvisited point vector and  djkstra::relation QHash
    for (int i = 0; i < dir.size(); ++i){
        QPoint curr_point(dir[i].x(),dir[i].y());
        if(!(dir[i].y() < 0) && !(dir[i].x() < 0)
            && !(dir[i].x() >  djkstra::dstra_map.width - 1)
            && !(dir[i].y() >  djkstra::dstra_map.height - 1)){
            curr_tile = djkstra::dstra_map.tile_vec[dir[i].y()][dir[i].x()];
            robot = robot_point.contains(curr_point);
        } else {
            continue;
        }
        // if point is not available in  djkstra::relation and not available in visited QHash, then point has not been visited yet
        // validate point and add to  djkstra::relation and unvisited
        if(!(djkstra::relation.contains(dir[i])) && !(djkstra::dstr_visited.contains(dir[i]))){
            if(!(dir[i].y() < 0) && !(dir[i].x() < 0)
                && !(dir[i].x() >  djkstra::dstra_map.width - 1) && !(dir[i].y() >  djkstra::dstra_map.height - 1)
                && curr_tile != tile::wand_BetonGrau && curr_tile != tile::gefahr_Abgrunde
                && curr_tile != tile::gefahr_Wasser && curr_tile != tile::wand_FelsBraun
                && curr_tile != tile::punkte_Hauptserver && curr_tile != tile::hind_Geheimgange)
            {
                // if point is Alien grass, hind_Schrotter, Blue door or Red door.
                // increase distance by 1

                if(curr_tile == tile::hind_Aliengras || curr_tile == tile::hind_Schrotter
                    || curr_tile == tile::tur_Blau || curr_tile == tile::tur_Rot
                    || robot == true)
                {
                    temp_dist = curr_dist + 1 ;
                } else {
                    temp_dist = curr_dist;
                }
                int index = 0;
                // find point in unvisted vector to update
                for (const auto& vec_pair : djkstra::dstr_unvisited_vec) {
                    if(temp_dist > vec_pair.second){
                          djkstra::dstr_unvisited_vec.insert(djkstra::dstr_unvisited_vec.begin() + index,
                                                           QPair(dir[i], temp_dist));
                        break;
                    }
                    index++;
                }
                  djkstra::relation.insert(dir[i], std::make_tuple( parent , temp_dist ));
            }
            // if point has already been visited, check if new path is shorter and update new parent and new distance
        } else if(  djkstra::relation.contains(dir[i])) {

            std::tie(map_point, map_dist) =   djkstra::relation.value(dir[i]);

            if(curr_tile == tile::hind_Aliengras || curr_tile == tile::hind_Schrotter
                || curr_tile == tile::tur_Blau || curr_tile == tile::tur_Rot
                || robot == true)
            {
                temp_dist = curr_dist + 1 ;
            } else {
                temp_dist = curr_dist;
            }

            if(temp_dist < map_dist){
                int index = 0;
                // find point in unvisted vector to update
                for (const auto& vec_pair :   djkstra::dstr_unvisited_vec) {
                    if(vec_pair.first == dir[i]){
                          djkstra::dstr_unvisited_vec.erase(djkstra::dstr_unvisited_vec.begin()+index);
                          djkstra::dstr_unvisited_vec.insert(djkstra::dstr_unvisited_vec.begin()+index,
                                                             QPair(dir[i], temp_dist));
                        break;
                    }
                    index++;
                }
                  djkstra::relation.insert(dir[i], std::make_tuple( parent , temp_dist ));
            }
        }
    }
    // save parent to visited QHash
      djkstra::dstr_visited.insert(parent, djkstra::dstr_unvisited_vec[djkstra::dstr_unvisited_vec.size()-1].second);
    // remove parent from unvisited vector
      djkstra::dstr_unvisited_vec.pop_back();
}

// find other robots on map and save current coordinates to vector
void djkstra::track_other_bot(const map& curr_map, QHash<QPoint, robot::robot_enum>& robot_point){

    // initialise robot point vector
    robot_point.clear();

    for(int y = 0; y < curr_map.height; y++)
    {
        for(int x = 0; x < curr_map.width; x++)
        {
            QPoint cost_coor(x,y);

            if(curr_map.robot_vec[y][x] != robot::no_bot){
                robot_point.insert(cost_coor,curr_map.robot_vec[y][x]);
            }
        }
    }
}
