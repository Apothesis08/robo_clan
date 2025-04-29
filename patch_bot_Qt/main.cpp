#include <iostream>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include <QMenuBar>
#include <QtCore/QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QTimer>

#include "presenter.h"
#include "robot.h"
#include "tile.h"


/////////////////////////////////////////////////////////////
//
//
//  Class: main
//
//
/////////////////////////////////////////////////////////////

// Class main to execute program and display main Window
int main(int argc, char *argv[])
{    
    QApplication app(argc, argv);
    presenter w;
    w.setStyleSheet("QMainWindow { background: rgb(220,220,220); } ");
    // Set the background color using a stylesheet

    w.show();
    int output;
    try {
        output = app.exec();
    } catch (const std::bad_alloc &) {

        return EXIT_FAILURE; // exit application
    }
    return output;
}
