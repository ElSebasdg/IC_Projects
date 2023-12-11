cmake_minimum_required(VERSION 2.8)
project(opencv_projects)
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(image_encoder image_encoder.cpp)
target_link_libraries( image_encoder ${OpenCV_LIBS} )

add_executable(image_decoder image_decoder.cpp)
target_link_libraries( image_decoder ${OpenCV_LIBS} )




