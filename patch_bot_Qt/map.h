#ifndef MAP_H
#define MAP_H

#include <unordered_map>
#include <vector>
#include <string>
#include <QDebug>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QPixmap>
#include <QtCore/QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>

#include "robot.h"
#include "tile.h"
#include "image.h"

class door;

// Class map combines robot, tile and image classes to be used by the presenter ui
// Main objective is reading text files to create 2D vectors of enums for robots and tiles
// Also used to find image of each object, validate map parameters and output generated 2D vectors
// locations of door enums are also recorded
class map
{
public:
    int height;              // height of map
    int width;               // width of map
    int num_goal;            // number of goals
    int patch_x_coor;        // x coordinate of patchbot
    int patch_y_coor;        // y coordinate of patchbot
    bool success;            // main server reached by patchbot
    bool completed;          // input commands from user completed
    bool failed;             // patchbot failed mission (died or captured)

    std::vector<std::vector<robot::robot_enum>> robot_vec; // 2D vector of robot enums
    std::vector<std::vector<tile::tile_enum>> tile_vec;    // 2D vector of tile enums
    std::vector<robot> dead_bot_vec; // vector of dead robots

    // initialise variables
    map()
    {
        height = 0;
        width = 0;
        num_goal = 0;
    }

    // static method to retrieve enum image
    QString get_tile_image(int row, int column);

    // static method to retrieve enum image
    QString get_robot_image(robot::robot_enum robo);

    // static method to read text file and return 2D map vector
    static map read_file(const std::string& location);

    // static function to find if character is a number
    static int valid_map_para(std::string in_str, int count);

private:
    // image object to access images
    image map_image;

};

#endif // MAP_H
