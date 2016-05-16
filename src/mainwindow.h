#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <sstream>

#include <QMainWindow>

#include "OHMComm.h"
#include "Logger.h"
#include "Utility.h"

#include "audiolibrarymodel.h"
#include "formatsmodel.h"
#include "participantmodel.h"
#include "audiodevicemodel.h"

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
    
    void initSetupView();
    void initConnectionView();
    void initLogView();
    
    std::unique_ptr<AudioLibraryModel> libraryModel;
    std::unique_ptr<AudioDeviceModel> inputDeviceModel;
    std::unique_ptr<AudioDeviceModel> outputDeviceModel;
    std::unique_ptr<FormatsModel> formatsModel;
    std::unique_ptr<ParticipantModel> participantsModel;
    
private Q_SLOTS:
    void connectRemote(bool clicked);
    void clearLog(bool clicked);
    void updateParticipantInfo(uint32_t participantSSRC);
};

#endif // MAINWINDOW_H
