/* 
 * File:   helper.cpp
 * Author: doe300
 * 
 * Created on July 31, 2016, 1:59 PM
 */

#include "helper.h"
#include "configuration.h"
#include "Logger.h"

#include "restless.hpp"

// for a more readable name
using Http = asoni::Handle;

Helper::Helper()
{
}

std::string Helper::getLatestVersion()
{
    //https://api.github.com/repos/doe300/OHMComm/tags
    auto response = Http().get("https://api.github.com/repos/doe300/OHMComm/tags").exec();
    if(response.code != 200)
    {
        ohmcomm::warn("Helper") << "Failed to retrieve version, HTTP status code: " << response.code << ohmcomm::endl;
        return ohmcomm::OHMCOMM_VERSION;
    }
    
    std::string::size_type index = response.body.find("\"name\"");
    if(index != std::string::npos)
    {
        index = response.body.find("\"v", index) + 1;
        const std::string versionString = response.body.substr(index, response.body.find('"', index) - index);
        ohmcomm::info("Helper") << "Latest version: " << versionString.data() << ohmcomm::endl;
        return versionString;
    }
    
    ohmcomm::warn("Helper") << "Failed to read latest version-number!" << ohmcomm::endl;
    return ohmcomm::OHMCOMM_VERSION;
}