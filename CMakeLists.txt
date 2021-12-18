cmake_minimum_required(VERSION 3.15)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_NAME nprog_project)
project(${PROJECT_NAME} C CXX) # project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

#! Options
set(WARNINGS_AS_ERRORS ON)
set(ENABLE_PVS_STUDIO OFF)

#! ENABLE_SANITIZERS is the option for the test builds!
#  Definitely enable it while developing.
#  Disable it for the production builds and before submitting for grading!
set(ENABLE_SANITIZERS OFF)

#! Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

#! Build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

find_package(
  Qt5
  COMPONENTS Core Widgets
  REQUIRED
)

#! Include headers
include_directories(include)

#! Project source compilation
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/cpu_info.cpp
    src/cpu.cpp
    src/functions.cpp
    src/memory_info.cpp
    src/mainwindow.cpp

    include/functions.h
    include/cpu.h
    include/cpu_info.h
    include/memory_info.h
    include/mainwindow.h

    )

#! Add external packages
#find_package(Boost 1.71.0 REQUIRED COMPONENTS program_options regex)
#include_directories(${Boost_INCLUDE_DIR})
#target_link_libraries(${PROJECT_NAME} Boost::program_options)

#! set output directory (bin)
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#####################################
# Define ALL_TARGETS variable to use in PVS and Sanitizers
set (ALL_TARGETS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC
    Qt5::Core
    Qt5::Widgets
    )

# Include fixed CMake configuration
# include(cmake/config.cmake)

