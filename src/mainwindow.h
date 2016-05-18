#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <sstream>

#include <QMainWindow>
#include <QIntValidator>

#include "config/ConfigurationMode.h"
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
    
Q_SIGNALS:
    void appendLog(QString string);

public Q_SLOTS:
    void shutdownCommunication();
    
private:
    uint32_t lastSSRC;
    QTimer updateTimer;
    Ui::MainWindow *ui;
    std::shared_ptr<ohmcomm::ConfigurationMode> config;
    std::unique_ptr<ohmcomm::OHMComm> ohmComm;
    std::wstringstream logStream;
    const QIntValidator portValidator;
    
    void initSetupView();
    void initConnectionView();
    void initLogView();
    
    std::unique_ptr<AudioLibraryModel> libraryModel;
    std::unique_ptr<AudioDeviceModel> inputDeviceModel;
    std::unique_ptr<AudioDeviceModel> outputDeviceModel;
    std::unique_ptr<FormatsModel> formatsModel;
    std::unique_ptr<ParticipantModel> participantsModel;
    
private Q_SLOTS:
    void connectRemote();
    void clearLog();
    void updateParticipantInfo(uint32_t participantSSRC);
    void updateParticipantDetails();
};

#endif // MAINWINDOW_H
