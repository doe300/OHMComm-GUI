/* 
 * File:   GUILogger.h
 * Author: doe300
 *
 * Created on August 28, 2016, 1:00 PM
 */

#ifndef GUILOGGER_H
#define GUILOGGER_H

#include "Logger.h"
#include "mainwindow.h"

class GUILogger : public ohmcomm::Logger
{
public:
    GUILogger(MainWindow* gui);
    
    std::wostream& write(const LogLevel level) override;
    std::wostream& end(std::wostream& stream) override;

    MainWindow* gui;
private:
    std::wstringstream logStream;
};

#endif /* GUILOGGER_H */

