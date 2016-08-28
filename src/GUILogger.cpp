/* 
 * File:   GUILogger.cpp
 * Author: doe300
 * 
 * Created on August 28, 2016, 1:00 PM
 */

#include "GUILogger.h"

GUILogger::GUILogger(MainWindow* gui) : Logger(), gui(gui)
{
    
}

std::wostream& GUILogger::write(const LogLevel level)
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

std::wostream& GUILogger::end(std::wostream& stream)
{
    stream << "</font>" << '\0' << std::endl;
    std::wstring data = dynamic_cast<std::wstringstream&>(stream).str();
    gui->appendLog(QString::fromStdWString(data));
    stream.clear();
    dynamic_cast<std::wstringstream&>(stream).str(std::wstring());
    return stream;
}
