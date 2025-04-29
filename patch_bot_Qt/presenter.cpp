#include <iostream>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QRect>
#include <QRectF>
#include <QDebug>
#include <QString>
#include <QPushButton>
#include <QScrollBar>
#include <QVBoxLayout>
#include <string>
#include <cstring>
#include <QString>
#include <QRegularExpression>
#include <QTimer>
#include <QChar>
#include <QThread>
#include <QPoint>
#include <QHash>
#include <QVector>
#include <QPair>
#include <unordered_map>
#include <algorithm>
#include <cmath>


#include "map.h"
#include "presenter.h"
#include "tile.h"
#include "ui_view.h"
#include "dialog.h"
#include "patch_bot.h"
#include "door.h"
#include "ki_gegner.h"
#include "djkstra.h"

/////////////////////////////////////////////////////////////
//
//
//  Class: presenter
//
//
/////////////////////////////////////////////////////////////

// Class presenter as Main Window user interface
// uses map class data to generate colony map of images
// user feedback on buttons used to update colony map ( robots and tiles )
// colony map can be changed by clicking other colony button connecting the dialog class Window
// arrow buttons input next direction of patch bot onto text box with their cooresponding repetitions
// when start button is clicked game starts using input directions, patch bot moves around the map
// using various functions each move is varified and validated
// auto-run button uses QTimer to move patch bot automatically
presenter::presenter(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::presenter)
{
    // initialise ui
    ui->setupUi(this);
    // initialise scene
    scene = new QGraphicsScene(this);
    // initialise dialog
    dialog = new Dialog(this);
    // initialise timer
    timer = new QTimer(this);

    // set scene
    ui->graphicsView->setScene(scene);

    // set start-up format
    ui->group_1->setEnabled(true);
    ui->select_colony->setEnabled(true);
    ui->stop->setEnabled(false);
    ui->auto_run->setEnabled(false);
    ui->single_step->setEnabled(false);
    ui->cancel->setEnabled(false);

    // initialise default map
    try{
        curr_map = map::read_file("koloniekarten/Lithium-Stollen 13-A.txt");
        ui->graphicsView->setSceneRect(0,0, curr_map.width * presenter::presenter_tile.tile_size, curr_map.height * presenter::presenter_tile.tile_size);
        update_view_port();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }

    /*
    // click pop-up notification
    connect(ui->up, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->down, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->left, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->right, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->wait, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->start, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->stop, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->delete_2, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->hold, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->auto_run, &QPushButton::clicked, this, &presenter::click_me);
    connect(ui->next_step, &QPushButton::clicked, this, &presenter::click_me);
    */

    // connect buttons to slots
    // connect(ui->start, &QPushButton::clicked, this, &presenter::on_start_clicked);
    //mconnect(ui->stop, &QPushButton::clicked, this, &presenter::on_stop_clicked);
    // update view port according to size of window

    connect(ui->graphicsView->verticalScrollBar(), &QScrollBar::valueChanged, this, &presenter::update_view_port);
    connect(ui->graphicsView->verticalScrollBar(), &QScrollBar::rangeChanged, this, &presenter::update_view_port);
    connect(ui->graphicsView->horizontalScrollBar(), &QScrollBar::valueChanged, this, &presenter::update_view_port);
    connect(ui->graphicsView->horizontalScrollBar(), &QScrollBar::rangeChanged, this, &presenter::update_view_port);
    // connect timer with single-step slot
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&presenter::on_single_step_clicked));
    // connect dialog select button with presenter update colony slot
    connect(dialog, SIGNAL(on_select_clicked_signal(bool)), this, SLOT(update_colony(bool)));

    ui->comboBox->setStyleSheet("QComboBox { background: rgb(12, 131, 139); color: rgb(255,255,255); }");
    ui->up->setStyleSheet("QPushButton { background: rgb(21, 199, 212); color: rgb(255,255,255); }");
    ui->down->setStyleSheet("QPushButton { background: rgb(21, 199, 212); color: rgb(255,255,255); }");
    ui->left->setStyleSheet("QPushButton { background: rgb(21, 199, 212); color: rgb(255,255,255); }");
    ui->right->setStyleSheet("QPushButton { background: rgb(21, 199, 212); color: rgb(255,255,255); }");
    ui->wait->setStyleSheet("QPushButton { background: rgb(12, 131, 139); color: rgb(255,255,255); }");
    ui->start->setStyleSheet("QPushButton { background: rgb(12, 131, 139); color: rgb(255,255,255); }");
    ui->auto_run->setStyleSheet("QPushButton { background: rgb(12, 131, 139); color: rgb(255,255,255); }");
    ui->cancel->setStyleSheet("QPushButton { background: rgb(12, 131, 139); color: rgb(255,255,255); }");
    ui->delete_2->setStyleSheet("QPushButton { background: rgb(12, 131, 139); color: rgb(255,255,255); }");
    ui->single_step->setStyleSheet("QPushButton { background: rgb(12, 131, 139); color: rgb(255,255,255); }");
    ui->stop->setStyleSheet("QPushButton { background: rgb(12, 131, 139); color: rgb(255,255,255); }");
    ui->graphicsView->setStyleSheet("QGraphicsView { background-color: rgb(12, 131, 139); }");
    ui->select_colony->setStyleSheet("QPushButton { background: rgb(12, 131, 139); color: rgb(255,255,255); }");
    ui->current_colony->setStyleSheet("QLabel { background-color: rgb(220,220,220); }");
    ui->frame->setStyleSheet("QFrame { background: rgb(220,220,220); }");
    ui->textfield->setStyleSheet("QPlainTextEdit { background: rgb(175,238,238); }");
    ui->group_1->setStyleSheet("QGroupBox { background-color: rgb(192,192,192); }");
    ui->group_2->setStyleSheet("QGroupBox { background-color: rgb(192,192,192); }");
    ui->graphicsView->horizontalScrollBar()->setStyleSheet("QScrollBar { background: rgb(21, 199, 212);}");
    ui->graphicsView->verticalScrollBar()->setStyleSheet("QScrollBar { background: rgb(21, 199, 212);}");



    // initialise dropdown box
    for(int i = 1; i <= 10; i++)
    {
        if(i == 10)
        {
            ui->comboBox->addItem(" X ");
        } else {
            ui->comboBox->addItem( QString::number(i) + " x");
        }
    }

}

// presenter constructor
presenter::~presenter()
{
    delete ui;
}

// pop up clicked notification
void presenter::click_me()
{
    QPushButton* click_sender = qobject_cast<QPushButton*>(sender());
    QString button_name = click_sender->objectName();
    QMessageBox::information(
        nullptr,
        tr(" Button Clicked "),
        button_name, QMessageBox::Ok | QMessageBox::Cancel);
}

/* update simulation area
void presenter::update_simulation_area(const QPixmap& new_frame)
{
}

  display message
void presenter::display_message(const std::string& text)
{
}
*/

// update view port of graphics view
void presenter::update_view_port()
{
    // refresh scene
    scene->clear();
    // check if current map is available
    if(curr_map.tile_vec.empty())
    {
        QMessageBox::information(this, tr("Error "), tr("Colony map is not available"), QMessageBox::Ok);
        return;
    }
    // initialise graphics view dimensions
    const QRect view_port(0, 0, ui->graphicsView->viewport()->width(),
                          ui->graphicsView->viewport()->height());
    // corrolate view port to scene
    const QRectF visible_area = ui->graphicsView->mapToScene(view_port).boundingRect();
    // initialise starting coordinates
    int x_start = static_cast<int>(visible_area.x()) / presenter::presenter_tile.tile_size;
    int y_start = static_cast<int>(visible_area.y()) / presenter::presenter_tile.tile_size;
    int x_end = static_cast<int>(visible_area.x() + visible_area.width()) / presenter::presenter_tile.tile_size + 1;
    int y_end = static_cast<int>(visible_area.y() +  visible_area.height()) / presenter::presenter_tile.tile_size + 1;

    // correct border values
    if (x_start < 0){
        x_start = 0;
    }
    if (y_start < 0){
        y_start = 0;
    }
    if (x_end > curr_map.width) x_end = curr_map.width;
    if (y_end > curr_map.height) y_end = curr_map.height;

    // print icon of each colony map coordinate
    presenter::execute_images(y_start, y_end, x_start, x_end);
}

// set image of tile and robot enums onto viewport
void presenter::execute_images(int y_start, int y_end, int x_start, int x_end)
{
    for(int y = y_start; y < y_end; y++)
    {
        for(int x = x_start; x < x_end; x++)
        {
            // print icon of tile enums
            try{
                QGraphicsPixmapItem* pm(scene->addPixmap(QPixmap(presenter::curr_map.get_tile_image(x,y))));
                pm->setPos(presenter::presenter_tile.tile_size * x, presenter::presenter_tile.tile_size * y);
            } catch (const std::exception& e) {
                QMessageBox::warning(this, tr("Error"), tr(e.what()));
            }
        }
    }

    // print path of each robot to patch_bot
    if (!(ui->select_colony->isEnabled())){

        for (auto i = presenter::robot_point.cbegin(), end = presenter::robot_point.cend(); i != end; ++i){
            QPoint map_point;
            QPoint temp_point;
            // set current robot point
            robot::robot_enum robot_id = i.value();
            QPoint robot_coor(i.key().x(),i.key().y());
            // set initial robot point
            temp_point = robot_coor;
            int map_dist = INT_MAX;
            // while distance to patchbot is not zero, find the next parent in QHash relation
            // next parent is closer to patch bot with shorter distance
            try{
                QGraphicsPixmapItem* pm(scene->addPixmap(QPixmap(curr_map.get_robot_image(i.value()))));
                pm->setPos(presenter::presenter_tile.tile_size * i.key().x(), presenter::presenter_tile.tile_size * i.key().y());
            } catch (const std::exception& e) {
                QMessageBox::warning(this, tr("Error"), tr(e.what()));
            }

            if(!(presenter::relation.empty()) &&
                (robot_id == robot::bot_Follower || robot_id == robot::bot_Sniffer || robot_id == robot::bot_Hunter)){
                try{
                    while(map_dist != 0){
                        // find parent point and current distance in QHash relation of current point
                        std::tie(map_point, map_dist) = presenter::relation.value(temp_point);
                        // with regards to direction, print icons

                        if(temp_point.y() >= y_start && temp_point.y() < y_end && temp_point.x() >= x_start && temp_point.x() < x_end){
                            QPoint S(temp_point.x(),temp_point.y()+1);
                            QPoint N(temp_point.x(),temp_point.y()-1);
                            QPoint E(temp_point.x()-1,temp_point.y());
                            QPoint W(temp_point.x()+1,temp_point.y());

                            if(S == map_point){
                                QGraphicsPixmapItem* pm(scene->addPixmap(QPixmap("pfeile/pfeil_unten.png")));
                                pm->setPos(presenter::presenter_tile.tile_size * temp_point.x(), presenter::presenter_tile.tile_size * temp_point.y());
                            } else if (N == map_point){
                                QGraphicsPixmapItem* pm(scene->addPixmap(QPixmap("pfeile/pfeil_oben.png")));
                                pm->setPos(presenter::presenter_tile.tile_size * temp_point.x(), presenter::presenter_tile.tile_size * temp_point.y());
                            } else if (E == map_point){
                                QGraphicsPixmapItem* pm(scene->addPixmap(QPixmap("pfeile/pfeil_links.png")));
                                pm->setPos(presenter::presenter_tile.tile_size * temp_point.x(), presenter::presenter_tile.tile_size * temp_point.y());
                            } else if (W == map_point){
                                QGraphicsPixmapItem* pm(scene->addPixmap(QPixmap("pfeile/pfeil_rechts.png")));
                                pm->setPos(presenter::presenter_tile.tile_size * temp_point.x(), presenter::presenter_tile.tile_size * temp_point.y());
                            }
                        }
                        // save next point as parent point and repeat
                        temp_point = map_point;
                    }
                } catch (const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
            }

            // print icon of dead bot
            if (!presenter::curr_map.dead_bot_vec.empty())
            {
                for(int i = 0; i < presenter::curr_map.dead_bot_vec.size() ; i++){
                    //if (curr_map.dead_bot_vec[i].y_coor == y && curr_map.dead_bot_vec[i].x_coor == x){
                    try{
                        QGraphicsPixmapItem* pm(scene->addPixmap(QPixmap("roboter/dead.png")));
                        pm->setPos(presenter::presenter_tile.tile_size * presenter::curr_map.dead_bot_vec[i].x_coor, presenter::presenter_tile.tile_size * curr_map.dead_bot_vec[i].y_coor);
                    } catch (const std::exception& e) {
                        QMessageBox::warning(this, tr("Error"), tr(e.what()));
                    }
                    // }
                }
            }

            try{
                QGraphicsPixmapItem* pm(scene->addPixmap(QPixmap(curr_map.get_robot_image(i.value()))));
                pm->setPos(presenter::presenter_tile.tile_size * i.key().x(), presenter::presenter_tile.tile_size * i.key().y());
            } catch (const std::exception& e) {
                QMessageBox::warning(this, tr("Error"), tr(e.what()));
            }
        }
    }




}

// update label according to selected colony
void presenter::update_label()
{
    ui->current_colony->setText("Current Colonie: " + presenter::dialog->selected_colony);
    //qDebug() << presenter::dialog->selected_item;
    try{
        curr_map = map::read_file(presenter::dialog->selected_colony_link.toStdString());
        ui->graphicsView->setSceneRect(0,0, curr_map.width * presenter::presenter_tile.tile_size, curr_map.height * presenter::presenter_tile.tile_size);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }
}

// other colony button clicked, display dialog Window
void presenter::on_select_colony_clicked()
{
    dialog->show();
}

// execute update_label and update_view_port
void presenter::update_colony(bool)
{
    try{
        update_label();
        update_view_port();
        ui->textfield->clear();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }

}

// up button clicked, send 'O' to plain_text_update
void presenter::on_up_clicked()
{
    try{
        plain_text_update("O");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }
}

// left button clicked, send 'L' to plain_text_update
void presenter::on_left_clicked()
{
    try{
        plain_text_update("L");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }
}

// right button clicked, send 'R' to plain_text_update
void presenter::on_right_clicked()
{
    try{
        plain_text_update("R");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }
}

// down button clicked, send 'U' to plain_text_update
void presenter::on_down_clicked()
{
    try{
        plain_text_update("U");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }
}

// wait button clicked, send 'W' to plain_text_update
void presenter::on_wait_clicked()
{
    try{
        plain_text_update("W");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }
}

// delete button clicked, remove last character of text box
void presenter::on_delete_2_clicked()
{
    text = ui->textfield->toPlainText();
    if(text == QString()){
        return;
    }
    text.chop(2);
    ui->textfield->setPlainText(text);
}

// validate input directions and their respective repetitions to update text box
void presenter::plain_text_update(QString dir)
{
    text = ui->textfield->toPlainText();

    c_box_index = ui->comboBox->currentIndex();

    if(ui->comboBox->currentIndex() < 10 && ui->comboBox->currentIndex() > 0){
        ui->textfield->setPlainText(text + dir + QString::number(c_box_index));
    } else if (ui->comboBox->currentIndex() == 10){
        if(dir[0] == 'W'){
            throw std::runtime_error("Invalid combination, please select another repetition.");
            return;
        }else{
            ui->textfield->setPlainText(text + dir + 'X');
        }
    } else {
        throw std::runtime_error("No repetition selected.");
        return;
    }
    text.clear();
}

// stop button clicked, stop timer and enable single-step button
void presenter::on_stop_clicked()
{
    ui->stop->setEnabled(false);
    ui->start->setEnabled(false);

    ui->cancel->setEnabled(true);
    ui->auto_run->setEnabled(true);
    ui->single_step->setEnabled(true);
    timer->stop();
    qDebug() << "timer stopped";
}

// start button clicked, execute game and set movement buttons,
// text box and other colony button to read-only
void presenter::on_start_clicked()
{
    door curr_door;
    ki_gegner curr_ki;
    djkstra curr_path;

    presenter::gegner_hash.clear();

    curr_path.track_other_bot(presenter::curr_map, presenter::robot_point);


    ui->group_1->setEnabled(false);
    ui->select_colony->setEnabled(false);
    ui->stop->setEnabled(false);
    ui->start->setEnabled(false);

    ui->textfield->setEnabled(true);
    ui->auto_run->setEnabled(true);
    ui->single_step->setEnabled(true);
    ui->cancel->setEnabled(true);

    // initialise game
    starting_text = ui->textfield->toPlainText();
    presenter::curr_map.completed = false;
    presenter::curr_map.failed = false;
    presenter::curr_map.success = false;
    presenter::patch_delay = false;
    // initialise door vector
    try{
    presenter::my_patch_bot = QPoint(presenter::curr_map.patch_x_coor,presenter::curr_map.patch_y_coor);
    curr_path.djkstra_map_gen(curr_map, dstr_unvisited_vec,
                              dstr_visited, relation,
                              my_patch_bot, robot_point);
    curr_door.locate_door(presenter::curr_map, presenter::door_vec);
    presenter::gegner_hash = curr_ki.locate_gegner(presenter::robot_point, presenter::curr_map);

    // generate paths of robots to patch bot
        update_view_port();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }
}

// cancel button clicked, terminate game and enable movement buttons,
// text box and other colony button
void presenter::on_cancel_clicked()
{
    // if no other colony is selected, reload default colony
    // otherwise reload current selected colony
    ui->textfield->setPlainText(starting_text);

    presenter::gegner_hash.clear();

    if (presenter::dialog->selected_colony == QString()){
        try{
            curr_map = map::read_file("koloniekarten/Lithium-Stollen 13-A.txt");
            ui->graphicsView->setSceneRect(0,0, curr_map.width * presenter::presenter_tile.tile_size, curr_map.height * presenter::presenter_tile.tile_size);
            update_view_port();
        } catch (const std::exception& e) {
            QMessageBox::warning(this, tr("Error"), tr(e.what()));
        }
    }else {
        try{
            update_label();
        } catch (const std::exception& e) {
            QMessageBox::warning(this, tr("Error"), tr(e.what()));
        }
    }

    // update status of buttons
    ui->stop->setEnabled(false);
    ui->auto_run->setEnabled(false);
    ui->single_step->setEnabled(false);
    ui->cancel->setEnabled(false);

    ui->group_1->setEnabled(true);
    ui->select_colony->setEnabled(true);
    ui->start->setEnabled(true);
    try{
        update_view_port();
        timer->stop();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }
}

// auto-run button clicked, start timer and
// update colony map with patchbot movements with single-step function
void presenter::on_auto_run_clicked()
{
    text = ui->textfield->toPlainText();

    ui->single_step->setEnabled(false);
    ui->start->setEnabled(false);
    ui->cancel->setEnabled(false);

    ui->stop->setEnabled(true);
    timer->start(300);

    qDebug() << "timer started";
}


// single-step button clicked,
// for each step update colony map with patchbot movements
void presenter::on_single_step_clicked()
{
    patch_bot curr_bot;
    door curr_door;
    ki_gegner curr_ki;


    // get text from text box
    text = ui->textfield->toPlainText();
    // update door changes from Class patch_bot
    presenter::update_move_hash();

    ui->start->setEnabled(false);

    // if timer active enable stop and disable other buttons
    // otherwise enable cancel and disable stop button
    if (timer->isActive()){
        ui->cancel->setEnabled(false);
        ui->stop->setEnabled(true);
    } else {
        ui->cancel->setEnabled(true);
        ui->stop->setEnabled(false);
    }

    // if text box is empty notify user of path entered is completed
    // update button status
    if(text == ""){
        QMessageBox::information(this, tr(" Notification "), tr("Path completed!"), QMessageBox::Ok);
        on_cancel_clicked();
        return;
    } else {
        // if text is not empty update movement of patchbot
        try{
            curr_bot.get_map_update(presenter::patch_delay, text, presenter::curr_map,
                                    presenter::robot_point, presenter::door_vec,
                                    presenter::gegner_hash, presenter::my_patch_bot);
            ui->textfield->setPlainText(text);
            presenter::relation = curr_ki.gegner_bot_ki(presenter::robot_point, presenter::curr_map,
                                                   presenter::gegner_hash, presenter::door_vec,
                                                   presenter::my_patch_bot, presenter::relation);
            curr_door.update_door(presenter::robot_point, presenter::curr_map, presenter::door_vec, presenter::my_patch_bot);
            if(presenter::curr_map.success){
                QMessageBox::information(this, tr(" Notification "),
                                         tr("Mission Completed, Main Server Repaired!"), QMessageBox::Ok);
                on_cancel_clicked();
                return;
            } else if(presenter::curr_map.failed){
                update_view_port();
                QMessageBox::information(this, tr(" Notification "), tr("Mission Failed!"), QMessageBox::Ok);
                on_cancel_clicked();
                return;
            } else if(presenter::curr_map.completed){
                QMessageBox::information(this, tr(" Notification "), tr("Path completed!"), QMessageBox::Ok);
                on_cancel_clicked();
                return;
            }
        } catch (const std::exception& e) {
            QMessageBox::warning(this, tr("Error"), tr(e.what()));
        }
    }

    // update patch bot changes to door vector
    // generate next path to patch bot from other robots
    // update view port
    try{
        presenter::update_move_hash();
        check_dead_bot();
        update_view_port();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"), tr(e.what()));
    }
    if(presenter::curr_map.failed){
        QMessageBox::information(this, tr(" Notification "), tr("Mission Failed!"), QMessageBox::Ok);
        update_view_port();
        on_cancel_clicked();
        return;
    }
}

// update map for DIGGER and dead bots
void presenter::update_move_hash(){

    for (auto i = presenter::robot_point.cbegin(), end = presenter::robot_point.cend(); i != end; ++i){
        // set current robot coordinate
        QPoint robot_coor(i.key().x(),i.key().y());
        // if current robot is DIGGER and tile is wand_FelsBraun
        if(presenter::curr_map.tile_vec[robot_coor.y()][robot_coor.x()] == tile::wand_FelsBraun
            && i.value() == robot::bot_Digger){
            // change tile to hind_Schrotter
            presenter::curr_map.tile_vec[robot_coor.y()][robot_coor.x()] = tile::hind_Schrotter;
        // if current robot is not SWIMMER and tile is water OR tile is gefahr_Abgrunde
        } else if((presenter::curr_map.tile_vec[robot_coor.y()][robot_coor.x()] == tile::gefahr_Wasser
                    && i.value() != robot::bot_Swimmer)
                   || (presenter::curr_map.tile_vec[robot_coor.y()][robot_coor.x()] == tile::gefahr_Abgrunde)){
            // create new dead bot object
            robot new_dead_bot;
            new_dead_bot.robot_id = i.value();
            new_dead_bot.x_coor = robot_coor.x();
            new_dead_bot.y_coor = robot_coor.y();
            new_dead_bot.dead_bot_duration = 0;
            // add new dead bot to dead_bot_vec
            presenter::curr_map.dead_bot_vec.push_back(new_dead_bot);
            // remove point from QHash robot_point and QHash gegner_hash
            if(new_dead_bot.robot_id == robot::patch_bot){
                presenter::curr_map.failed = true;
            }
            presenter::robot_point.remove(robot_coor);
            presenter::gegner_hash.remove(robot_coor);
        }
    }
}

// update dead bots duration
void presenter::check_dead_bot(){
    // if dead bots are not available return, otherwise continue
    if (presenter::curr_map.dead_bot_vec.empty()){
        return;
    }
    // for each available dead bot, increment duration
    // if duration longer than 5 patch bot moves, remove dead bot from vector
    for(int i = 0; i < presenter::curr_map.dead_bot_vec.size() ; i++){
        if(presenter::curr_map.dead_bot_vec[i].robot_id == robot::patch_bot){
            presenter::curr_map.failed = true;
            return;
        }
        presenter::curr_map.dead_bot_vec[i].dead_bot_duration++;
        qDebug() << presenter::curr_map.dead_bot_vec[i].dead_bot_duration;
        if(presenter::curr_map.dead_bot_vec[i].dead_bot_duration > 5)
        {
            qDebug() << presenter::curr_map.dead_bot_vec[i].robot_id;
            presenter::curr_map.dead_bot_vec.erase(presenter::curr_map.dead_bot_vec.begin()+i);
        }
    }
}
