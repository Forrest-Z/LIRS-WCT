cmake_minimum_required(VERSION 3.5)
project(lirs_wct_gui)

###################
## Preprocessing ##
###################

## Decalare a build type
set(CMAKE_BUILD_TYPE Release)

## Compile as C++11
add_compile_options(-std=c++11)

## Tell Cmake where QT libraries, includes, tool and etc.
list(APPEND CMAKE_PREFIX_PATH "/opt/qt512/")

## Add the current source and build directories to the include path
set(CMAKE_INCLUDE_CURRENT_DIR ON)
## Process using Qt moc preprocessor
set(CMAKE_AUTOMOC ON)

## Find Qt5 libraries
find_package(Qt5 REQUIRED
    COMPONENTS
      Widgets
      Core
      Gui
)

## Grab project files
file(GLOB SOURCES src/*.cpp)
file(GLOB QT_UI ui/*.ui)
file(GLOB QT_MOC include/mainwindow.h)
#file(GLOB QT_RESOURCES resources/*.qrc)
file(GLOB_RECURSE QT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/*.cpp)

## QT wrappers
QT5_WRAP_UI(QT_UI_HPP ${QT_UI})
QT5_WRAP_CPP(QT_MOC_HPP ${QT_MOC})

## Specify additional locations of header files
include_directories(
    include
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

###########
## Build ##
###########

## Generate the executable file
add_executable(${PROJECT_NAME} ${QT_SOURCES} ${QT_RESOURCES_CPP} ${QT_UI_HPP} ${QT_MOC_HPP})

## Link libraries to the executable
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Gui Qt5::Core)
