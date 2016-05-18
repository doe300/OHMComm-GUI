
#include <QApplication>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow(nullptr);
    mainWindow.show();
    
    QApplication::connect(&app, SIGNAL(lastWindowClosed()), &mainWindow, SLOT(shutdownCommunication()));
    
    return app.exec();
}