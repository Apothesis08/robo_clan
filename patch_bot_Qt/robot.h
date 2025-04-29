#ifndef ROBOT_H
#define ROBOT_H

#include <unordered_map>

// Class robot stores enums of robots
// creates unordered_map of int and robot enums,
// used in map class to create 2D enum robot vector
class robot {

public:
    // enum of robots
    enum robot_enum {
        bot_Bugger = 1, bot_Pusher,
        bot_Digger, bot_Swimmer,
        bot_Follower, bot_Hunter,
        bot_Sniffer, no_bot, patch_bot
    };

    // enum of robot
    robot_enum robot_id;
    // dead bot coordinates
    int x_coor;
    int y_coor;
    // dead bot duration
    int dead_bot_duration;


    // unordered_map of robot enum
    std::unordered_map<int, robot_enum> robot_table;

    // initialise robot enum unordered_map
    robot()
    {
        robot_table[1] = robot_enum{ bot_Bugger };
        robot_table[2] = robot_enum{ bot_Pusher };
        robot_table[3] = robot_enum{ bot_Digger };
        robot_table[4] = robot_enum{ bot_Swimmer };
        robot_table[5] = robot_enum{ bot_Follower };
        robot_table[6] = robot_enum{ bot_Hunter };
        robot_table[7] = robot_enum{ bot_Sniffer };
        robot_table[8] = robot_enum{ no_bot };
        robot_table[9] = robot_enum{ patch_bot };
    }

    // retrieve robot value for enum
    robot_enum get_robot(int x)
    {
        robot_enum robot_id = no_bot;
        std::unordered_map<int, robot_enum>
            ::const_iterator element = robot_table.find(x);

        for (int i = 0; i < 9; i++) {
            if (element->first == x) {
                robot_id = element->second;
                break;
            }
        }
        return robot_id;
    }
};


#endif // ROBOT_H
