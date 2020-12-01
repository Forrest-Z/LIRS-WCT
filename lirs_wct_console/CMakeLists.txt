cmake_minimum_required(VERSION 3.5)
project(lirs_wct_console)

###################
## Preprocessing ##
###################

## Compile as C++11
set(CMAKE_CXX_STANDARD 11)
## Decalare a build type
set(CMAKE_BUILD_TYPE Release)

## Hide warning output of the ImageMagick library
add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16)
add_definitions(-DMAGICKCORE_HDRI_ENABLE=0)

## Find libraries
find_package(Eigen3 REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(Boost 1.58.0 REQUIRED
    COMPONENTS
      system
      filesystem
      program_options
)
find_package(ImageMagick REQUIRED
    COMPONENTS
      Magick++
)

## Specify additional locations of header files
include_directories(
    SYSTEM # fix eigen warnings
    include
    ${EIGEN3_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${ImageMagick_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

###########
## Build ##
###########

## Generate the executable file
add_executable(${PROJECT_NAME} src/main.cpp src/png2stl.cpp 
        src/heightmap.cpp src/libtrix.cpp
        src/utils.cpp
        src/png2dae.cpp
        src/world_creator.cpp
        src/script_generator.cpp
        src/ModelLoader.cpp src/Mesh.cpp
)

## Link libraries to the executable
target_link_libraries(${PROJECT_NAME}
    ${ASSIMP_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ImageMagick_LIBRARIES}
)
