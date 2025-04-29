#include <iostream>
#include <charconv>
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <istream>
#include <QString>
#include <unordered_map>

#include "map.h"
#include "image.h"
#include "presenter.h"


/////////////////////////////////////////////////////////////
//
//
//  Class: map
//
//
/////////////////////////////////////////////////////////////

// Class map combines all other classes to be used by the presenter ui
// Main objective is reading text files to create 2D vectors of enums for robots and tiles
// Also used to find image of each object, validate map parameters and output generated 2D vectors
// locations of door enums are also recorded


// static method to read text file and create new map object with 2D vector of robot and tile enums
// saves initial coordinates of patch bot, height and width of map, number of goals found and checks whether colony map is valid
// stores door objects found with their locations and enum
map map::read_file(const std::string& location)
{
    // Create map object
    map new_map;
    // Create environment object
    tile map_env;

    // Temperory 1D ENUM Vectors saves input line of string as ENUMS
    std::vector<tile::tile_enum> temp_tile_enum_vec;
    std::vector<robot::robot_enum> temp_robot_enum_vec;
    std::ifstream samplefile;

    int row = 0;				// width
    int column = 0;				// height
    int count = 0;				// count of each line / Row
    bool start = false;         // initialise start found to false
    bool goal = false;          // initialise goal found to false

    samplefile.open(location);			// Opens file in given location

    if (samplefile.is_open()) {			// Check if file open
        std::string in_str;				// Read in line of string from text file

        while (std::getline(samplefile, in_str)) {
            if (count == 0) {			// 1st line of text file = row
                // convert string to int and set map width
                row = map::valid_map_para(in_str, count);
                new_map.width = row;
            }
            else if (count == 1) {	    // 2nd line of text file = height
                // convert string to int and set map height
                column = map::valid_map_para(in_str, count);
                new_map.height = column;
            }
            else {
                // Compare input string with Table of elements
                // Save cooresponding ENUM to ENUM vector
                if (in_str.size() != row) {
                    //  throw exception if size of row is invalid
                    throw std::runtime_error("Error map::read_file(..) : Invalid size of row ! ");
                    return new_map;
                }
                else {
                    // text file of each colony-map row loop
                    for (int i = 0; i < in_str.size(); i++)
                    {
                        // create temperory enum to be added onto vector of enums
                        robot::robot_enum temp_robot_enum;
                        tile::tile_enum temp_tile_enum;
                        // compare elements on unordered_map to find character
                        std::unordered_map<char, tile::tile_enum>
                            ::const_iterator element = map_env.tile_table.find(in_str[i]);
                        if (element == map_env.tile_table.end()) {
                            // throw exception if element not found in table
                            throw std::runtime_error("Error map::read_file(..) : Invalid character ! ");
                            return new_map;
                        }
                        else {
                            // assign current tile character and initialise robot enum to no_bot
                            temp_tile_enum = element->second;
                            temp_robot_enum = robot::robot_enum::no_bot;
                            if (element->first == 'p') {
                                if (start) {
                                    // if many Patchbots available, throw exception
                                    throw std::runtime_error("Error map::read_file(..) : Too Many Starts ! ");
                                    return new_map;
                                }
                                else {
                                    start = true;
                                    // if Patchbot found, set robot enum to patch_bot and set its coordinates
                                    temp_robot_enum = robot::robot_enum::patch_bot;
                                    new_map.patch_x_coor = i;
                                    new_map.patch_y_coor = count - 2;
                                }
                           }
                            // check if Patchbot goal available
                            else if (element->first == 'P') {
                                goal = true;
                                new_map.num_goal++;
                            }
                            else if (isdigit(element->first)) {
                                // if character is a number (robot) find robot type on unordered map and change robot status accordingly
                                robot exe_robot;
                                for (int i = 1; i < 8; i++) {
                                    if (char(element->first - '0') == i) {
                                        temp_robot_enum = exe_robot.get_robot(i);
                                        break;
                                    }
                                }
                            }
                            // add created enum to temperory vector of enums
                            temp_tile_enum_vec.push_back(temp_tile_enum);
                            temp_robot_enum_vec.push_back(temp_robot_enum);
                        }
                    }
                }
                // add created temperory vector of enums to 2D vector of enums
                new_map.tile_vec.push_back(temp_tile_enum_vec);
                new_map.robot_vec.push_back(temp_robot_enum_vec);
                // clear temperory vector of enums
                temp_tile_enum_vec.clear();
                temp_robot_enum_vec.clear();
             //   temp_tile_cost_vec.clear();
            }
            // increment to next line of text file ( height )
            count++;
            // if end of text file reached and no start or goal available, throw exception
            if (count == column + 2 && !start) {
                throw std::runtime_error("Error map::read_file(..) : No PatchBot Start Available !");
                return new_map;
            }
            else if (count == column + 2 && !goal) {
                throw std::runtime_error("Error map::read_file(..) : No PatchBot Goal Available !");
                return new_map;
            }
        }
        // if invalid size of height, throw exception
        if (count != column + 2)
        {
            throw std::runtime_error("Error map::read_file(..) : Invalid size of column !");
        }
        // close file
        samplefile.close();
    }
    else {
        // if file not available, throw exception
        throw std::runtime_error("Error map::read_file(..) : File not found!");
        return new_map;
    }
    return new_map;
}

// check whether character is a number
// used to validate map parameters
int map::valid_map_para(std::string in_str, int count) {
    for (int i = 0; i < in_str.size(); i++)
    {
        if (!isdigit(in_str[i])) {
            throw std::runtime_error("Error map::valid_map_para(..) : Invalid Height Input");
            return 0;
        }
    }
    return stoi(in_str);
}

// returns image of tile enum
QString map::get_tile_image(int x, int y)
{
    if(x > this->width || x < 0){
        throw std::runtime_error("Error map::get_tile_image(..) : Invalid Width Input");
        return QString();
    } else if(y > this->height || y < 0){
        throw std::runtime_error("Error map::get_tile_image(..) : Invalid Height Input");
        return QString();
    }
    // using enum value find coorespinding image link in un_ordered map
    // return link
    std::unordered_map<tile::tile_enum, QString>
        ::const_iterator element = map_image.image_table_tile.find(this->tile_vec[y][x]);
    return element->second;
}

// returns image of robot enum
QString map::get_robot_image(robot::robot_enum robo)
{   
    // using enum value find coorespinding image link in un_ordered map
    // return link
    std::unordered_map<robot::robot_enum,  QString>
        ::const_iterator element = map_image.image_table_robot.find(robo);
    return element->second;
}
