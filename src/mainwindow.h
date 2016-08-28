#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <sstream>

#include <QMainWindow>
#include <QIntValidator>

#include "OHMComm.h"
#include "Utility.h"

#include "audiolibrarymodel.h"
#include "formatsmodel.h"
#include "participantmodel.h"
#include "audiodevicemodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
Q_SIGNALS:
    void appendLog(QString string);

public Q_SLOTS:
    void shutdownCommunication();
    void openLink(const QString& link);
    
private:
    uint32_t lastSSRC;
    QTimer updateTimer;
    Ui::MainWindow *ui;
    std::unique_ptr<ohmcomm::OHMComm> ohmComm;
    const QIntValidator portValidator;
    
    void initSetupView();
    void initConnectionView();
    void initLogView();
    void initStatusBar();
    
    std::unique_ptr<AudioLibraryModel> libraryModel;
    std::unique_ptr<AudioDeviceModel> inputDeviceModel;
    std::unique_ptr<AudioDeviceModel> outputDeviceModel;
    std::unique_ptr<FormatsModel> formatsModel;
    std::unique_ptr<ParticipantModel> participantsModel;
    
    void resetParticipantInfo();
    
private Q_SLOTS:
    void connectRemote();
    void clearLog();
    void updateParticipantInfo(uint32_t participantSSRC);
    void updateParticipantDetails();
};

#endif // MAINWINDOW_H
