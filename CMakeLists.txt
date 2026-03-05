cmake_minimum_required(VERSION 3.5)
project(ROS2_Automatic)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

add_executable(ROS2_Automatic Automation.cpp)

target_link_libraries(ROS2_Automatic Qt5::Widgets Qt5::Network)