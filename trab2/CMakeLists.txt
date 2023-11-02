cmake_minimum_required(VERSION 2.8)
project(opencv_projects)
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( color_channel color_channel.cpp)
target_link_libraries( color_channel ${OpenCV_LIBS} )