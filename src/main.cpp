
#include <QApplication>

#include "mainwindow.h"
#include "GUILogger.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow(nullptr);
    ohmcomm::Logger::LOGGER.reset(new GUILogger(&mainWindow));
    mainWindow.show();
    
    QApplication::connect(&app, SIGNAL(lastWindowClosed()), &mainWindow, SLOT(shutdownCommunication()));
    
    return app.exec();
}