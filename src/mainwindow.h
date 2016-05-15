#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <sstream>

#include <QMainWindow>

#include "OHMComm.h"
#include "Logger.h"
#include "Utility.h"

#include "formatsmodel.h"
#include "participantmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, private ohmcomm::Logger
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    std::wostream& write(const LogLevel level) override;
    std::wostream& end(std::wostream& stream) override;
    
private:
    Ui::MainWindow *ui;
    std::unique_ptr<ohmcomm::OHMComm> ohmComm;
    std::wstringstream logStream;
    
    void initData();
    
    FormatsModel formatsModel;
    ParticipantModel participantsModel;
};

#endif // MAINWINDOW_H
