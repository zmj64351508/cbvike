CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(cbvike)

SET(VERSION_SERIES 0)
SET(VERSION_MAJOR 2)
SET(VERSION_MINOR 1)

set (WX_VERSION "2.8")
set (wxWidgets_CONFIG_OPTIONS --version=${WX_VERSION})

FIND_PACKAGE(wxWidgets REQUIRED)

INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(/usr/include/codeblocks)
INCLUDE_DIRECTORIES(/usr/include/codeblocks/wxscintilla/include)

ADD_DEFINITIONS(-D __WXGTK__)
ADD_DEFINITIONS(-D __linux__)
ADD_DEFINITIONS(-std=c++11)

ADD_LIBRARY(cbVike SHARED
    cbvike.cpp
    cbvike.h
    cbvike_plugin.cpp
    cbvike_plugin.h
    debugging.h
    vicmd.cpp
    vifunc.cpp
    vifunc.h
    vike.cbp
    vicmd.h
)

ADD_CUSTOM_COMMAND(TARGET cbVike POST_BUILD
    COMMAND zip ARGS ${PROJECT_BINARY_DIR}/cbVike.zip manifest.xml
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

SET(CMAKE_INSTALL_PREFIX /usr)
#INSTALL(TARGETS cbVike DESTINATION lib/codeblocks/plugins)
INSTALL(FILES ${PROJECT_BINARY_DIR}/cbVike.zip DESTINATION share/codeblocks)
