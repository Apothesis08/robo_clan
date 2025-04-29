#include <QDebug>
#include <QString>
#include <QChar>
#include <QThread>
#include <tuple>

#include "map.h"
#include "patch_bot.h"
#include "presenter.h"

/////////////////////////////////////////////////////////////
//
//
//  Class: patch_bot
//
//
/////////////////////////////////////////////////////////////

void patch_bot::get_map_update(bool& patch_delay, QString& text, map& patch,
                               QHash<QPoint, robot::robot_enum>& gegner_point,
                               QHash<QPoint, door> &d_vec, QHash<QPoint, ki_gegner_att> &gegner_vec,
                               QPoint& patch_bot_old){

    patch_bot::patch_bot_coor = patch_bot_old;
    patch_bot::p_map = patch;
    patch_bot::updated_text = text;
    patch_bot::door_vec = d_vec;
    patch_bot::robot_point = gegner_point;
    patch_bot::gegner_patch_hash = gegner_vec;
    patch_bot::delay = patch_delay;

    try{
        update_location();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    patch_bot_old = patch_bot::patch_bot_coor;
    patch = patch_bot::p_map;
    text = patch_bot::updated_text;
    d_vec = patch_bot::door_vec;
    gegner_point = patch_bot::robot_point;
    gegner_vec = patch_bot::gegner_patch_hash;
    patch_delay = patch_bot::delay;
}

// update location of patch bot (main function)
void patch_bot::update_location()
{

    if(patch_bot::updated_text == ""){
        patch_bot::p_map.completed = true;
        return;
    }

    // initialise current direction of patchbot
    patch_bot::up = false;
    patch_bot::down = false;
    patch_bot::left = false;
    patch_bot::right = false;
    // initialise status of X (repeat until obstacle)
    patch_bot::recur = false;

    // depending on the initial character of text box, update current direction of patchbot
    try{
        if(patch_bot::updated_text[0] == 'R'){
            patch_bot::right = true;
            if(patch_bot::updated_text[1].isDigit()){
                patch_bot::rounds = QString(patch_bot::updated_text[1]).toInt();
            } else if(!patch_bot::updated_text[1].isDigit()){
                patch_bot::rounds = 10;
                patch_bot::recur = true;
            }

            patch_bot::next_y = patch_bot::patch_bot_coor.y();
            patch_bot::next_x = patch_bot::patch_bot_coor.x()+1;
            QPoint curr_next_coor(patch_bot::next_x,patch_bot::next_y);
            patch_bot::next = curr_next_coor;
            update_patch_bot();
        } else if(patch_bot::updated_text[0] == 'L'){
            patch_bot::left = true;
            if(patch_bot::updated_text[1].isDigit()){
                patch_bot::rounds = QString(patch_bot::updated_text[1]).toInt();
            } else if(!patch_bot::updated_text[1].isDigit()){
                patch_bot::rounds = 10;
                patch_bot::recur = true;
            }
            patch_bot::next_y = patch_bot::patch_bot_coor.y();
            patch_bot::next_x = patch_bot::patch_bot_coor.x()-1;
            QPoint curr_next_coor(patch_bot::next_x,patch_bot::next_y);
            patch_bot::next = curr_next_coor;
            update_patch_bot();
        } else if(patch_bot::updated_text[0] == 'U'){
            patch_bot::down = true;
            if(patch_bot::updated_text[1].isDigit()){
                patch_bot::rounds = QString(patch_bot::updated_text[1]).toInt();
            } else if(!patch_bot::updated_text[1].isDigit()){
                patch_bot::rounds = 10;
                patch_bot::recur = true;
            }
            patch_bot::next_y = patch_bot::patch_bot_coor.y()+1;
            patch_bot::next_x = patch_bot::patch_bot_coor.x();
            QPoint curr_next_coor(patch_bot::next_x,patch_bot::next_y);
            patch_bot::next = curr_next_coor;
            update_patch_bot();
        } else if(patch_bot::updated_text[0] == 'O'){
            patch_bot::up = true;
            if(patch_bot::updated_text[1].isDigit()){
                patch_bot::rounds = QString(patch_bot::updated_text[1]).toInt();
            } else if(!patch_bot::updated_text[1].isDigit()){
                patch_bot::rounds = 10;
                patch_bot::recur = true;
            }
            patch_bot::next_y = patch_bot::patch_bot_coor.y()-1;
            patch_bot::next_x = patch_bot::patch_bot_coor.x();
            QPoint curr_next_coor(patch_bot::next_x,patch_bot::next_y);
            patch_bot::next = curr_next_coor;
            update_patch_bot();
        } else if(patch_bot::updated_text[0] == 'W'){
            if(patch_bot::updated_text[1].isDigit()){
                patch_bot::rounds = QString(patch_bot::updated_text[1]).toInt();
                wait();
                if(patch_bot::rounds>1){
                    patch_bot::rounds = patch_bot::rounds - 1;
                    patch_bot::updated_text.replace(1, 1, QString::number(rounds));
                } else {
                    patch_bot::updated_text.remove(0,2);
                }
                qDebug() << patch_bot::updated_text << "y:" << patch_bot::patch_bot_coor.y()
                         << "x:" << patch_bot::patch_bot_coor.x();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}


// delay by 1 second
void patch_bot::wait(){
    //QThread::msleep(1000);
    //qDebug() << "waiting.." ;
}

// validates next patch bot move, pushes enemy bot if possible and also  with regards to "repeat until obstacle"
void patch_bot::update_patch_bot(){

    // initialise obstacle status
    patch_bot::hind = false;
    auto& curr_tile = patch_bot::p_map.tile_vec[patch_bot::next_y][patch_bot::next_x];
    // validate next move of patch bot is within colony map bounds
    if (patch_bot::next_y < 0 || (patch_bot::next_y > patch_bot::p_map.height - 1)
        || patch_bot::next_x < 0 || (patch_bot::next_x > patch_bot::p_map.width - 1)){
        patch_bot::recur = false;
        update_rounds();
        return;
    }
    // check direction of patch bot, if enemy bot is present and no obstacle behind it, move both bots 1 step forward
    if (patch_bot::robot_point.contains(next) && left){
        patch_bot::second_next_y = patch_bot::next_y;
        patch_bot::second_next_x = patch_bot::next_x-1;
        QPoint curr_snd_next_coor(patch_bot::second_next_x,patch_bot::second_next_y);
        patch_bot::snd_next = curr_snd_next_coor;
        update_push_bot();
    } else if (patch_bot::robot_point.contains(next) && down){
        patch_bot::second_next_y = patch_bot::next_y+1;
        patch_bot::second_next_x = patch_bot::next_x;
        QPoint curr_snd_next_coor(patch_bot::second_next_x,patch_bot::second_next_y);
        patch_bot::snd_next = curr_snd_next_coor;
        update_push_bot();
    } else if (patch_bot::robot_point.contains(next) && up){
        patch_bot::second_next_y = patch_bot::next_y-1;
        patch_bot::second_next_x = patch_bot::next_x;
        QPoint curr_snd_next_coor(patch_bot::second_next_x,patch_bot::second_next_y);
        patch_bot::snd_next = curr_snd_next_coor;
        update_push_bot();
    } else if (patch_bot::robot_point.contains(next) && right){
        patch_bot::second_next_y = patch_bot::next_y;
        patch_bot::second_next_x = patch_bot::next_x+1;
        QPoint curr_snd_next_coor(patch_bot::second_next_x,patch_bot::second_next_y);
        patch_bot::snd_next = curr_snd_next_coor;
        update_push_bot();
        // check if next move is valid (without obstacles)
    } else if ((curr_tile == tile::boden_Klar
                ||curr_tile == tile::boden_GrunPatch
                ||curr_tile == tile::boden_RotPatch
                ||curr_tile == tile::hind_Schrotter
                ||curr_tile == tile::hind_Geheimgange
                ||curr_tile == tile::tuer_manuell_offen
                ||curr_tile == tile::hind_Aliengras)
               && !(patch_bot::robot_point.contains(patch_bot::next))){
        update_patch_bot_icon();
        // if patch bot encounters danger, notify mission failed
    } else if (curr_tile == tile::gefahr_Abgrunde){
        patch_bot::hind = true;
        update_patch_bot_icon();
        return;
        // if patch bot encounters danger, notify mission failed
    }else if (curr_tile == tile::gefahr_Wasser){
        patch_bot::hind = true;
        update_patch_bot_icon();
        return;
        // if patch bot encounters alien grass, delay 1 second
    } else if (curr_tile == tile::tur_Blau){
        door temp_door = patch_bot::door_vec.value(patch_bot::next);
        temp_door.door_key = true;
        patch_bot::door_vec.remove(patch_bot::next);
        patch_bot::door_vec.insert(patch_bot::next, temp_door);
        update_rounds();
        // if patch bot reaches main server, notify mission success
    } else if (curr_tile == tile::punkte_Hauptserver){
        patch_bot::p_map.robot_vec[patch_bot::patch_bot_coor.y()][patch_bot::patch_bot_coor.x()] = robot::patch_bot;
        patch_bot::p_map.success = true;
        return;
    } else {
        qDebug() << "Obstacle in front";
        patch_bot::recur = false;
        update_rounds();

        return;
    }
}

void patch_bot::update_push_bot(){
    // initialise obstacle status
    patch_bot::hind = false;
    robot::robot_enum next_robot_type;
    auto& second_next_tile_type = patch_bot::p_map.tile_vec[patch_bot::second_next_y][patch_bot::second_next_x];
    next_robot_type = patch_bot::robot_point.value(patch_bot::next);

    ki_gegner temp_ki;

    // validate next move of patch bot is within colony map bounds
    if (patch_bot::second_next_y < 0 || (patch_bot::second_next_y > patch_bot::p_map.height - 1)
        || patch_bot::second_next_x < 0 || (patch_bot::second_next_x > patch_bot::p_map.width - 1)){
        patch_bot::recur = false;
        update_rounds();
        return;
    }

    if(!(patch_bot::robot_point.contains(patch_bot::snd_next)) && patch_bot::robot_point.contains(patch_bot::next)
        && second_next_tile_type != tile::wand_BetonGrau && second_next_tile_type != tile::punkte_Hauptserver
        && second_next_tile_type != tile::hind_Geheimgange && second_next_tile_type != tile::tur_Blau
        && second_next_tile_type != tile::tur_Rot){
        if(second_next_tile_type == tile::wand_FelsBraun && next_robot_type == robot::bot_Digger){
            patch_bot::robot_point.insert(patch_bot::snd_next, next_robot_type);
            patch_bot::robot_point.remove(patch_bot::next);
            temp_ki.refresh_points(patch_bot::next, patch_bot::snd_next,
                                   patch_bot::gegner_patch_hash, patch_bot::p_map);
            update_patch_bot_icon();
        } else if(second_next_tile_type != tile::wand_FelsBraun){
            patch_bot::robot_point.insert(patch_bot::snd_next, next_robot_type);
            patch_bot::robot_point.remove(patch_bot::next);
            temp_ki.refresh_points(patch_bot::next, patch_bot::snd_next,
                                   patch_bot::gegner_patch_hash, patch_bot::p_map);
            update_patch_bot_icon();
        } else {
            qDebug() << "Obstacle in front";
            patch_bot::recur = false;
            update_rounds();
            return;
        }
    } else {
        qDebug() << "Obstacle in front";
        patch_bot::recur = false;
        update_rounds();
        return;
    }
}

// if patch bot encounters danger, update icon to tomb stone
void patch_bot::update_patch_bot_icon(){

    ki_gegner curr_ki;

    if(patch_bot::p_map.tile_vec[patch_bot::patch_bot_coor.y()][patch_bot::patch_bot_coor.x()] ==
            tile::hind_Aliengras && patch_bot::delay == false){
        patch_bot::delay = true;
        qDebug() << "PATCH_BOT move delayed by Alien Grass" ;

    } else {
        patch_bot::delay = false;
        if(patch_bot::hind){
            patch_bot::p_map.failed = true;
            robot new_dead_bot;
            new_dead_bot.robot_id = patch_bot::robot_point.value(patch_bot::next);
            new_dead_bot.x_coor = patch_bot::next_x;
            new_dead_bot.y_coor = patch_bot::next_y;
            new_dead_bot.dead_bot_duration = 0;
            patch_bot::p_map.dead_bot_vec.push_back(new_dead_bot);
        }
        if(patch_bot::robot_point.value(patch_bot::patch_bot_coor) == robot::robot_enum::patch_bot){
            patch_bot::robot_point.remove(patch_bot::patch_bot_coor);
            patch_bot::robot_point.insert(patch_bot::next, robot::robot_enum::patch_bot);
            curr_ki.refresh_points(patch_bot::patch_bot_coor, patch_bot::next,
                                   patch_bot::gegner_patch_hash, patch_bot::p_map);
            patch_bot::patch_bot_coor = patch_bot::next;

        }
        update_rounds();
    }

}

// update number of direction repetitions with regards to "repeat until obstacle"
void patch_bot::update_rounds(){
    if(patch_bot::rounds == 10 && patch_bot::recur){
        qDebug() << "No Obstacle Found..";
    } else if(patch_bot::rounds == 10 && !patch_bot::recur){
        patch_bot::updated_text.remove(0,2);
        patch_bot::update_location();
        return;
    } else if(patch_bot::rounds > 1 && patch_bot::rounds < 10 && !patch_bot::recur){
        patch_bot::rounds--;
        patch_bot::updated_text.replace(1, 1, QString::number(rounds));
    } else {
        patch_bot::rounds--;
        patch_bot::updated_text.remove(0,2);
    }
    //ui->textfield->setPlainText(patch_bot::updated_text);
    qDebug() << patch_bot::updated_text << "y:" << patch_bot::patch_bot_coor.y()
             << "x:" << patch_bot::patch_bot_coor.x();
    // update_view_port();
}
