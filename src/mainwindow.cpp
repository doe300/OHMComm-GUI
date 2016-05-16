#include "mainwindow.h"
#include "moc_mainwindow.cpp"
#include "ui_mainwindow.h"

#include "rtp/RTCPHeader.h"
#include "rtp/ParticipantDatabase.h"
#include "rtp/RTCPData.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), ohmComm(nullptr)
{
    ui->setupUi(this);
    
    //initialize models, must be after setting up GUI
    libraryModel.reset(new AudioLibraryModel(ui->audioLibraryComboBox));
    inputDeviceModel.reset(new AudioDeviceModel(true, ui->inputAudioDeviceComboBox));
    outputDeviceModel.reset(new AudioDeviceModel(false, ui->outputAudioDeviceComboBox));
    formatsModel.reset(new FormatsModel(ui->formatsListView));
    participantsModel.reset(new ParticipantModel(ui->participantsTableView));
    
    initSetupView();
    initConnectionView();
    initLogView();
}    

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
    ui->localDataPortLineEdit->setText(QString::number(ohmcomm::DEFAULT_NETWORK_PORT));
    
    //remote
    ui->remotePortLineEdit->setText(QString::number(5060));
    
    //audio
    ui->audioLibraryComboBox->setModel(libraryModel.get());
    ui->inputAudioDeviceComboBox->setModel(inputDeviceModel.get());
    ui->outputAudioDeviceComboBox->setModel(outputDeviceModel.get());
    ui->formatsListView->setModel(formatsModel.get());
    ui->formatsListView->selectAll();
    //update available devices on library change
    connect(ui->audioLibraryComboBox, SIGNAL(currentIndexChanged(QString)), inputDeviceModel.get(), SLOT(onUpdateLibrary(QString)));
    connect(ui->audioLibraryComboBox, SIGNAL(currentIndexChanged(QString)), outputDeviceModel.get(), SLOT(onUpdateLibrary(QString)));
    
    //other
    ohmcomm::Logger::LOGGER.reset(this);
    connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(connectRemote(bool)));
}

void MainWindow::initConnectionView()
{
    ui->participantsTableView->setModel(participantsModel.get());
    connect(participantsModel.get(), SIGNAL(participantSelected(uint32_t)), this, SLOT(updateParticipantInfo(uint32_t)));
}

void MainWindow::initLogView()
{
    connect(ui->clearLogButton, SIGNAL(clicked(bool)), this, SLOT(clearLog(bool)));
}


std::wostream& MainWindow::write(const LogLevel level)
{
    switch(level)
    {
        case ohmcomm::Logger::DEBUG:
            logStream << "[D]"; 
            break;
        case ohmcomm::Logger::INFO:
            logStream << "[I]"; 
            break;
        case ohmcomm::Logger::WARNING:
            logStream << "[W]"; 
            break;
        case ohmcomm::Logger::ERROR:
            logStream << "[E]"; 
            break;
    }
    return logStream;
}

std::wostream& MainWindow::end(std::wostream& stream)
{
    std::wstring data = dynamic_cast<std::wstringstream&>(stream).str();
    ui->logView->append(QString::fromWCharArray(data.data()));
    dynamic_cast<std::wstringstream&>(stream).str((wchar_t*)"");
    return logStream;
}

void MainWindow::connectRemote(bool clicked)
{
    if(!clicked)return;
    //TODO log selected config
    //TODO try to establish connection
    //TODO change to participants-tab
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::clearLog(bool clicked)
{
    if(!clicked)return;
    ui->logView->clear();
}

static std::string getSourceDescription(const ohmcomm::rtp::RTCPData* data, const ohmcomm::rtp::RTCPSourceDescriptionType type)
{
    for(const ohmcomm::rtp::SourceDescription& sdes : data->sourceDescriptions)
    {
        if(sdes.type == type)
            return sdes.value;
    }
    return "";
}

void MainWindow::updateParticipantInfo(uint32_t participantSSRC)
{
    const auto remoteIt = ohmcomm::rtp::ParticipantDatabase::getAllRemoteParticipants().find(participantSSRC);

    ui->participantJitterField->setText(QString("%1 ms").arg((*remoteIt).second.interarrivalJitter, 0, 'f', 3));
    ui->participantPackagesField->setText(QString("%1 (%2 lost)").arg((*remoteIt).second.totalPackages).arg((*remoteIt).second.packagesLost));
    ui->participantDataField->setText(QString("%1 Bytes total").arg((*remoteIt).second.totalBytes));
    if((*remoteIt).second.rtcpData.get() != nullptr)
    {
        ui->participantUserField->setText(QString(getSourceDescription((*remoteIt).second.rtcpData.get(), ohmcomm::rtp::RTCP_SOURCE_NAME).data()));
        ui->participantAddressField->setText(QString(getSourceDescription((*remoteIt).second.rtcpData.get(), ohmcomm::rtp::RTCP_SOURCE_CNAME).data()));
        ui->participantInfoField->setText(QString(getSourceDescription((*remoteIt).second.rtcpData.get(), ohmcomm::rtp::RTCP_SOURCE_TOOL).data()));
    }
    else
    {
        ui->participantUserField->setText("-");
        ui->participantAddressField->setText("-");
        ui->participantInfoField->setText("-");
    }
}