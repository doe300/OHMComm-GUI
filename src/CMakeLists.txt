#Include headers in the project structure
file( GLOB HDRS "${PROJECT_SOURCE_DIR}/include/*.h")

#Include headers in the project settings (as search-path for header-files)
include_directories ("${PROJECT_SOURCE_DIR}/include")

#Include the source files
file( GLOB SRCS *.cpp */*.cpp )

#Build OHMComm
add_executable(OHMCommGUI ${SRCS} ${HDRS} ${UI_HEADERS})
target_link_libraries(OHMCommGUI OHMComm)

# Use the Widgets module from Qt 5.
target_link_libraries(OHMCommGUI Qt5::Widgets)

# Include RESTless API
target_link_libraries(OHMCommGUI restless)