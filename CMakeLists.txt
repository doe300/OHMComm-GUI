cmake_minimum_required (VERSION 2.8.12)

project (OHMComm-GUI)

####
# General configuration
####
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run uic automatically when needed.
set(CMAKE_AUTOUIC ON)
# Option to enable/disable test-program
option(BUILD_DEBUG "Build with debugging symbols. Otherwise build for performance" OFF)

# append usage of C++ to compiler flags, also optimize for speed and enable all warnings
if(NOT MSVC)
	# the following instruction causes warnings in visual studio, 
	# therefore enable it only when MSVC-compiler is not used
	if(BUILD_DEBUG)
		LIST(APPEND CMAKE_CXX_FLAGS "-std=c++0x -g3 -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -pthread")
	else()
		LIST(APPEND CMAKE_CXX_FLAGS "-std=c++0x -O3 -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -pthread")
	endif()
else()
	# visual studio specific flags
	set(CMAKE_CXX_FLAGS "/W3 /D_UNICODE /DUNICODE /DNOMINMAX /EHsc")
	add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

####
# Dependencies
####
# OHMComm - https://github.com/doe300/OHMComm
#Try to find precompiled library
find_library(OHMCOMM_LIBRARY NAMES OHMComm PATHS "../OHMComm/build")
if(OHMCOMM_LIBRARY)
	message(STATUS "OHMComm library found: " ${OHMCOMM_LIBRARY})
	#Add OHMComm to include directories
	link_directories("${PROJECT_SOURCE_DIR}/../OHMComm/build")
	include_directories("${PROJECT_SOURCE_DIR}/../OHMComm/include")
else()
	message(SEND_ERROR "No OHMComm library found")
endif()

# Qt - https://www.qt.io/
# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

####
# Main files
####
#build all from ./src into ./build
add_subdirectory(src build)

