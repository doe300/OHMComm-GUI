# OHMComm-GUI
A lightweight GUI for the [OHMComm](https://github.com/doe300/OHMComm)-project based on Qt 5.

[![GitHub license](https://img.shields.io/github/license/doe300/OHMComm-GUI.svg)](https://github.com/doe300/OHMComm-GUI/blob/master/LICENSE)
[![Release](https://img.shields.io/github/tag/doe300/OHMComm-GUI.svg)](https://github.com/doe300/OHMComm-GUI/releases/latest)

For a full list of features, see [OHMComm](https://github.com/doe300/OHMComm).

## Build it

#### Under Linux/Mac OS
You will need a compiler with full C++11 support and cmake in version 2.8.12 or higher.
To build the GUI, you first need to compile the [OHMComm-library](https://github.com/doe300/OHMComm) 
and have [Qt5](https://www.qt.io/) installed, inclusive development headers.

	$ cd <project-directory>
	$ cmake -G "Unix Makefiles" ./CMakeLists.txt
	$ make OHMCommGUI		# To build the executable

This will build the executable into `<project-directory>/build/` by default.
