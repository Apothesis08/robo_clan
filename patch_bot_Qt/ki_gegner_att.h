#ifndef KI_GEGNER_ATT_H
#define KI_GEGNER_ATT_H

// class KI_GEGNER_ATT contains attributes of gegner robots
// attributes such as: their current state, robot enum, starting position, next position, up down left right axis
// whether robot has legs or wheels, they are delayed or not and already validated or not
class ki_gegner_att{

public:
    // states of gegner robot
    enum state{
        // states of BUGGER gegner robot
        find_another_wall, follow_wall,
        // states of PUSHER, DIGGER, and SWIMMER
        move_horizontally, move_vertically,
        // state when gegner robot die
        destroy,
        // states of FOLLOWER, HUNTER, and SNIFFER
        waiting, follow_path, hunt,
        // state of PATCH_BOT
        hero
    };

    robot::robot_enum robot_id;     // current gegner robot enum
    mutable state current_state;    // current state of gegner robot

    mutable QPoint start_point;     // starting point of BUGGER robot
    mutable QPoint current_point;   // current point of gegner robot
    mutable QPoint next_point;      // next move coordinate of current gegner robot
    mutable QPoint N;               // north coordinate of gegner robot ( used when rotating and finding next move coordiante )
    mutable QPoint W;               // west coordinate of gegner robot ( used when rotating and finding next move coordiante )
    mutable QPoint E;               // east coordinate of gegner robot ( used when rotating and finding next move coordiante )
    mutable QPoint S;               // south coordinate of gegner robot ( used when rotating and finding next move coordiante )

    mutable bool legs;              // if gegner robot has legs = true
    mutable bool wheels;            // if gegner robot has wheels = true
    mutable bool checked;           // if gegner robot has been validated = true (prevent from checking again on same loop, gegner robots move only once)
    mutable bool delay;             // if gegner robot has wheels and on hind_Aliengras delay is true, or it has legs and on hind_Schrotter then delay is true
    mutable bool visible;           // if PATCH BOT in line of view
    mutable bool reachable;         // if path to PATCH BOT is clear
    mutable bool reached;           // if PATCH BOT or end of path is reached
    mutable bool move;              // if current gegner bot should move or not


};

#endif // KI_GEGNER_ATT_H
