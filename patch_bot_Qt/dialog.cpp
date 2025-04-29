#include <iostream>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include <QtCore/QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QPushButton>
#include <QListWidgetItem>
#include <QMessageBox>
#include <unordered_map>

#include "presenter.h"
#include "dialog.h"
#include "ui_dialog.h"

/////////////////////////////////////////////////////////////
//
//
//  Class: dialog
//
//
/////////////////////////////////////////////////////////////

// Class Dialog window to display list of all maps, which can be selected to change as current map
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    // initialise ui
    ui->setupUi(this);
    // sort Colonies using QDir
    QDir mDir("koloniekarten");
    // find Colony links ending with ".txt"
    QStringList filter;
    filter << "*.txt";
    // set filter
    mDir.setNameFilters(filter);

    // common link name set to QRegularExpression to be removed
    static QRegularExpression pre_link("D:/Qt_code/Projects/build-patch_bot_Qt"
                                       "-Desktop_Qt_6_6_0_MinGW_64_bit-Debug/koloniekarten/");
    static QRegularExpression sur_link(".txt");

    // add each found directory to unordered_map of Colonies and their cooresponding Colony file links
    foreach(QFileInfo mitm, mDir.entryInfoList())
    {
        QString link = mitm.absoluteFilePath();
        // remove QRegularExpression of links
        link.remove(pre_link);
        link.remove(sur_link);
        // enter colony and colony link to unordered_map of colonies
        map_link[link] = QString{mitm.absoluteFilePath()};
        // add item to list widget
        ui->listWidget->addItem(link);
    }
}

// Dialog constructor
Dialog::~Dialog()
{
    delete ui;
}


// sets selected colony as current colony
void Dialog::on_listWidget_currentRowChanged(int currentRow)
{
    row_selected = currentRow;
}

// finds link of selected colony and its updates selected_colony_link
void Dialog::on_listWidget_itemClicked()
{   // sets selected colony to current item
    this->selected_colony = ui->listWidget->currentItem()->text();
    // finds cooresponding colony map link
    std::unordered_map<QString, QString>
        ::const_iterator element = map_link.find(selected_colony);
    this->selected_colony_link = element->second;
}

// select button in Dialog window clicked
void Dialog::on_select_clicked(){
    // if no colony currently selected, notify user
    if(selected_colony.isEmpty()){
        QMessageBox::information(
            nullptr,
            tr(" Error "),
            tr(" No File Selected! "), QMessageBox::Close);
        return;
    }

    // send signal of button clicked, used to update label of colony on presenter Window
    // and update map to selected colony map
    emit on_select_clicked_signal(true);
}




void Dialog::on_listWidget_doubleClicked(const QModelIndex &index)
{
    emit on_select_clicked_signal(true);
}

