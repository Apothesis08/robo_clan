#ifndef PRESENTER_H
#define PRESENTER_H

#include <iostream>
#include <string>
#include <QDialog>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsGridLayout>
#include <QGraphicsItem>
#include <QGroupBox>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QtCore>
#include <QTimer>

#include "map.h"
#include "dialog.h"
#include "patch_bot.h"
#include "door.h"
#include "ki_gegner.h"
#include "djkstra.h"


//QT_BEGIN_NAMESPACE
namespace Ui {
class presenter;
}
//QT_END_NAMESPACE

// Class presenter as Main Window user interface
// uses map class data to generate colony map of images
// user feedback on buttons used to update colony map ( robots and tiles )
// colony map can be changed by clicking other colony button connecting the dialog class Window
// arrow buttons input next direction of patch bot onto text box with their cooresponding repetitions
// when start button is clicked game starts using input directions, patch bot moves around the map while interecting with other elements
// using various functions each move is varified and validated
// auto-run button uses QTimer to move patch bot automatically
class presenter : public QMainWindow
{
    Q_OBJECT

public:
    presenter(QWidget *parent = nullptr);
    ~presenter();
    // instanciate current map object for accessibility
    map curr_map;
    // save current map starting state to use at new game run
    map start_map;
    // stores current state of doors, open, close and duration of open
    QHash<QPoint, door> door_vec;

protected:
    // clicked notification pop-up window
    void click_me();
    // void on_repetition_dropdown_index_changed(int index);
    // void update_simulation_area(const QPixmap& new_frame);
    // void display_message(const std::string& text);
    // given x and y coordinates of enums, print cooresponding image of enum
    void execute_images(int y_start, int y_end, int x_start, int x_end);
    // update displayed colony map
    void update_view_port();
    // update label of selected colony map and initialise colony map
    void update_label();

public slots:
    // other colony button clicked, display dialog Window
    void on_select_colony_clicked();
    // execute update_label and update_view_port
    void update_colony(bool);
    // update dead bots duration
    void check_dead_bot();
    // update map for DIGGER and dead bots
    void update_move_hash();

private slots:
    // up button clicked, send 'O' to plain_text_update
    void on_up_clicked();
    // left button clicked, send 'L' to plain_text_update
    void on_left_clicked();
    // right button clicked, send 'R' to plain_text_update
    void on_right_clicked();
    // down button clicked, send 'U' to plain_text_update
    void on_down_clicked();
    // wait button clicked, send 'W' to plain_text_update
    void on_wait_clicked();
    // delete button clicked, remove last character of text box
    void on_delete_2_clicked();
    // validate input directions and their respective repetitions to update text box
    void plain_text_update(QString dir);
    // start button clicked, execute game and set movement buttons, text box and other colony button to read-only
    void on_start_clicked();
    // cancel button clicked, terminate game and enable movement buttons, text box and other colony button
    void on_cancel_clicked();
    // auto-run button clicked, start timer and update colony map with patchbot movements and update single-step button
    void on_auto_run_clicked();
    // single-step button clicked, for each step update colony map with patchbot movements
    void on_single_step_clicked();
    // stop button clicked, stop timer and enable single-step button
    void on_stop_clicked();

private:
    Ui::presenter *ui;       // ui object
    QGraphicsScene *scene;   // scene object
    Dialog *dialog;          // dialog object
    QTimer *timer;           // timer object

    QString text;            // current text of text box
    QString starting_text;   // initial text of text box

    QPoint my_patch_bot;     // save current real time point of PATCH BOT
    bool patch_delay;        // delay when PATCH BOT on alien grass

    tile presenter_tile;     // access tile values

    int c_box_index;         // current index of drop down box
    QHash<QPoint, robot::robot_enum> robot_point;       // created vector of coordinates of robots
    QHash<QPoint, ki_gegner_att> gegner_hash;           // contains up-to-date gegner robot location and their respective updated attributes (from class ki_gegner_att)

    QHash<QPoint, std::tuple<QPoint, int>> relation;    // QHash data structure to store parent of each accessable point on map and shortest distance to patch bot
    QVector<QPair<QPoint, int>> dstr_unvisited_vec;     // QVector data structure of points to visit next, distance increases incremently (lowest to highest)
    QHash<QPoint, int> dstr_visited;                    // QHash data structure of parent points already visited

    std::vector<std::vector<robot::robot_enum>> p_robot_vec; // 2D vector of robot enums
    std::vector<std::vector<tile::tile_enum>> p_tile_vec;    // 2D vector of tile enums
    std::vector<robot> p_dead_bot_vec;                       // vector of dead robots


};
#endif // PRESENTER_H
