#include "mainwindow.h"
#include "moc_mainwindow.cpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), ohmComm(nullptr), formatsModel(ui->formatsListView), participantsModel(ui->participantsTableView)
{
    ui->setupUi(this);
    
    ohmcomm::Logger::LOGGER.reset(this);
    initData();
}    

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initData()
{
    ui->formatsListView->setModel(&formatsModel);
    ui->participantsTableView->setModel(&participantsModel);
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
