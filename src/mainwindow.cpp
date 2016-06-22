#include "mainwindow.h"
#include "moc_mainwindow.cpp"
#include "ui_mainwindow.h"

#include "Parameters.h"
#include "sip/SIPConfiguration.h"
#include "rtp/ParticipantDatabase.h"
#include "rtp/RTCPData.h"
#include "Statistics.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), updateTimer(parent), ui(new Ui::MainWindow), config(nullptr), ohmComm(nullptr), portValidator(0, UINT16_MAX)
{
    ui->setupUi(this);
    
    //initialize models, must be after setting up GUI
    libraryModel.reset(new AudioLibraryModel(ui->audioLibraryComboBox));
    inputDeviceModel.reset(new AudioDeviceModel(true, ui->inputAudioDeviceComboBox));
    outputDeviceModel.reset(new AudioDeviceModel(false, ui->outputAudioDeviceComboBox));
    formatsModel.reset(new FormatsModel(ui->formatsListView));
    participantsModel.reset(new ParticipantModel(ui->participantsListView));
    
    initSetupView();
    initConnectionView();
    initLogView();
    
    //update participant details every second
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateParticipantDetails()));
    updateTimer.start(1000);
}

//FIXME crashes on exit (on destruction of Logger) since this window is the logger and is destroyed before
//-> move logger out of here!

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSetupView()
{
    //local user
    const std::string userName = ohmcomm::Utility::getUserName();
    const std::string deviceName = ohmcomm::Utility::getDomainName();
    ui->userNameLineEdit->setText(QString(userName.data()));
    ui->hostNameLineEdit->setText(QString(deviceName.data()));
    ui->localSIPPortLineEdit->setText(QString::number(5060));
    ui->localSIPPortLineEdit->setValidator(&portValidator);
    ui->localDataPortLineEdit->setText(QString::number(ohmcomm::DEFAULT_NETWORK_PORT));
    ui->localDataPortLineEdit->setValidator(&portValidator);
    
    //remote
    ui->remotePortLineEdit->setText(QString::number(5060));
    ui->remotePortLineEdit->setValidator(&portValidator);
    
    //audio
    ui->audioLibraryComboBox->setModel(libraryModel.get());
    ui->inputAudioDeviceComboBox->setModel(inputDeviceModel.get());
    ui->outputAudioDeviceComboBox->setModel(outputDeviceModel.get());
    ui->formatsListView->setModel(formatsModel.get());
    ui->formatsListView->selectAll();
    //update available devices on library change
    connect(ui->audioLibraryComboBox, SIGNAL(currentIndexChanged(QString)), inputDeviceModel.get(), SLOT(onUpdateLibrary(QString)));
    connect(ui->audioLibraryComboBox, SIGNAL(currentIndexChanged(QString)), outputDeviceModel.get(), SLOT(onUpdateLibrary(QString)));
    
    ui->errorMessageField->setStyleSheet("QLabel { color: red; }");
    //other
    ohmcomm::Logger::LOGGER.reset(this);
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectRemote()));
    connect(ui->disconnectButton, SIGNAL(clicked()), this, SLOT(shutdownCommunication()));
    
    connect(this, SIGNAL(appendLog(QString)), ui->logView, SLOT(append(QString)));
}

void MainWindow::initConnectionView()
{
    ui->participantsListView->setModel(participantsModel.get());
    connect(participantsModel.get(), SIGNAL(participantSelected(uint32_t)), this, SLOT(updateParticipantInfo(uint32_t)));
}

void MainWindow::initLogView()
{
    connect(ui->clearLogButton, SIGNAL(clicked()), this, SLOT(clearLog()));
}

std::wostream& MainWindow::write(const LogLevel level)
{
    switch(level)
    {
        case ohmcomm::Logger::DEBUG:
            logStream << "<font color='gray'>"; 
            break;
        case ohmcomm::Logger::INFO:
            logStream << "<font color='black'>"; 
            break;
        case ohmcomm::Logger::WARNING:
            logStream << "<font color='orange'>"; 
            break;
        case ohmcomm::Logger::ERROR:
            logStream << "<font color='red'>"; 
            break;
    }
    return logStream;
}

std::wostream& MainWindow::end(std::wostream& stream)
{
    stream << "</font>" << '\0' << std::endl;
    std::wstring data = dynamic_cast<std::wstringstream&>(stream).str();
    appendLog(QString::fromStdWString(data));
    stream.clear();
    dynamic_cast<std::wstringstream&>(stream).str(std::wstring());
    return stream;
}

void MainWindow::shutdownCommunication()
{
    //FIXME crashes with floating point error
    if(ohmComm.get() != nullptr)
    {
        ui->participantsListView->selectionModel()->clearSelection();
        if(ohmComm->isRunning())
        {
            ohmComm->stopAudioThreads();
            //TODO wait until everything is stopped
            std::stringstream stream;
            ohmcomm::Statistics::printStatistics(stream);
            ui->logView->append(QString(stream.str().data()));
            ohmComm.reset(nullptr);
        }
        ohmcomm::Statistics::resetStatistics();
        ui->setupTab->setEnabled(true);
        ui->tabWidget->setCurrentIndex(0);
    }
}

void MainWindow::connectRemote()
{
    //make sure, all required fields are set
    if(ui->userNameLineEdit->text().isEmpty())
    {
        ui->errorMessageField->setText("Username can't be empty!");
        return;
    }
    if(ui->hostNameLineEdit->text().isEmpty())
    {
        ui->errorMessageField->setText("Hostname can't be empty!");
        return;
    }
    if(ui->localSIPPortLineEdit->text().isEmpty())
    {
        ui->errorMessageField->setText("SIP port can't be empty!");
        return;
    }
    if(ui->localDataPortLineEdit->text().isEmpty())
    {
        ui->errorMessageField->setText("Data port can't be empty!");
        return;
    }
    if(ui->addressLineEdit->text().isEmpty())
    {
        ui->errorMessageField->setText("Remote address can't be empty!");
        return;
    }
    if(ui->remotePortLineEdit->text().isEmpty())
    {
        ui->errorMessageField->setText("Remote port can't be empty!");
        return;
    }
    if(!ui->formatsListView->selectionModel()->hasSelection())
    {
        ui->errorMessageField->setText("At least a single codec must be selected!");
        return;
    }
    
    //log selected config
    ui->logView->append("\nUsing configuration:");
    ui->logView->append(QString("User '%1', host '%2'").arg(ui->userNameLineEdit->text()).arg(ui->hostNameLineEdit->text()));
    ui->logView->append(QString("Local SIP port %1, local data port %2, remote address '%3', remote SIP port %4").
        arg(ui->localSIPPortLineEdit->text()).arg(ui->localDataPortLineEdit->text()).arg(ui->addressLineEdit->text()).arg(ui->remotePortLineEdit->text()));
    ui->logView->append(QString("Audio library '%1', input device '%2', output device '%3', DTX %4, FEC %5, codecs: %6").
        arg(ui->audioLibraryComboBox->currentText()).arg(ui->inputAudioDeviceComboBox->currentText()).arg(ui->outputAudioDeviceComboBox->currentText()).
        arg((ui->dtxCheckBox->isChecked() ? "enabled" : "disabled")).arg((ui->fecCheckBox->isChecked() ? "enabled" : "disabled")).
        arg(ui->formatsListView->selectionModel()->selectedIndexes().size()));
    
    //write configuration mode
    ohmcomm::NetworkConfiguration sipConfig{};
    sipConfig.localPort = ui->localSIPPortLineEdit->text().toUShort();
    sipConfig.remoteIPAddress = ohmcomm::Utility::getAddressForHostName(ui->addressLineEdit->text().toStdString());
    sipConfig.remotePort = ui->remotePortLineEdit->text().toUShort();
    ohmcomm::Parameters params({}, {});
    params.setParameter(ohmcomm::Parameters::AUDIO_HANDLER, ui->audioLibraryComboBox->currentText().toStdString());
    params.setParameter(ohmcomm::Parameters::INPUT_DEVICE, std::to_string(ui->inputAudioDeviceComboBox->currentIndex()));
    params.setParameter(ohmcomm::Parameters::OUTPUT_DEVICE, std::to_string(ui->outputAudioDeviceComboBox->currentIndex()));
    params.setParameter(ohmcomm::Parameters::REMOTE_ADDRESS, sipConfig.remoteIPAddress);
    params.setParameter(ohmcomm::Parameters::LOCAL_PORT, ui->localDataPortLineEdit->text().toStdString());
    
    params.setParameter(ohmcomm::Parameters::ENABLE_DTX, (ui->dtxCheckBox->isChecked() ? "1" : "0"));
    params.setParameter(ohmcomm::Parameters::ENABLE_FEC, (ui->fecCheckBox->isChecked() ? "1" : "0"));
    
    params.setParameter(ohmcomm::Parameters::SDES_CNAME, ui->hostNameLineEdit->text().toStdString());
    params.setParameter(ohmcomm::Parameters::SDES_NAME, ui->hostNameLineEdit->text().toStdString());
    
    config.reset(new ohmcomm::sip::SIPConfiguration(params, sipConfig));
    
    //try to establish connection
    ohmComm.reset(new ohmcomm::OHMComm(config.get()));
    if(!ohmComm->isConfigurationDone(true))
    {
        ohmcomm::error("GUI") << "Failed to configure OHMComm!" << ohmcomm::endl;
        ohmComm.reset();
        ui->errorMessageField->setText("Error configuring OHMComm, see the log for details!");
        return;
    }
    try
    {
        ohmComm->startAudioThreads();
    }
    catch(const std::exception& err)
    {
        ohmcomm::error("GUI") << err.what() << ohmcomm::endl;
        ohmComm.reset();
        ui->errorMessageField->setText("Error starting OHMComm, see the log for details!");
        return;
    }
    
    connect(ui->participantsListView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), participantsModel.get(), SLOT(fireParticipantSelected(const QModelIndex&, const QModelIndex&)));
    //switch to participants-tab
    ui->tabWidget->setCurrentIndex(1);
    
    //disable all configuration until disconnect
    ui->setupTab->setEnabled(false);
}

void MainWindow::clearLog()
{
    ui->logView->clear();
}

void MainWindow::updateParticipantInfo(uint32_t participantSSRC)
{
    lastSSRC = participantSSRC;
    const auto remoteIt = ohmcomm::rtp::ParticipantDatabase::getAllRemoteParticipants().find(participantSSRC);
    
    const auto duration = std::chrono::steady_clock::now() - (*remoteIt).second.firstPackageReceived;

    ui->participantDurationField->setText(QString("%2 s (%1 ms)").
            arg(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()).
            arg(std::chrono::duration_cast<std::chrono::seconds>(duration).count()));
    ui->participantJitterField->setText(QString("%1 ms").arg((*remoteIt).second.interarrivalJitter, 0, 'f', 3));
    ui->participantPackagesField->setText(QString("%1 (%2 lost)").arg((*remoteIt).second.totalPackages).arg((*remoteIt).second.packagesLost));
    ui->participantDataField->setText(QString("%2/s (%1/total)").
            arg(QString::fromStdString(ohmcomm::Utility::prettifyByteSize((*remoteIt).second.totalBytes))).
            arg(QString::fromStdString(ohmcomm::Utility::prettifyByteSize((*remoteIt).second.totalBytes / std::chrono::duration_cast<std::chrono::seconds>(duration).count()))));
    if((*remoteIt).second.rtcpData.get() != nullptr)
    {
        ui->participantUserField->setText(QString((*(*remoteIt).second.rtcpData)[ohmcomm::rtp::RTCP_SOURCE_NAME].data()));
        ui->participantAddressField->setText(QString((*(*remoteIt).second.rtcpData)[ohmcomm::rtp::RTCP_SOURCE_CNAME].data()));
        ui->participantInfoField->setText(QString((*(*remoteIt).second.rtcpData)[ohmcomm::rtp::RTCP_SOURCE_TOOL].data()));
    }
    else
    {
        ui->participantUserField->setText("-");
        ui->participantAddressField->setText("-");
        ui->participantInfoField->setText("-");
    }
    
    //TODO display format, sample rate, audio-config, ...
}

void MainWindow::updateParticipantDetails()
{
    if(ui->participantsListView->selectionModel() != nullptr)
    {
        const QModelIndexList selected = ui->participantsListView->selectionModel()->selectedIndexes();
        if(!selected.empty())
        {
            updateParticipantInfo(lastSSRC);
        }
    }
}
