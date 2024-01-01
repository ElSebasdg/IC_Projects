cmake_minimum_required(VERSION 2.8)
project(opencv_projects)
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(image_encoder image_encoder.cpp)
target_link_libraries( image_encoder ${OpenCV_LIBS} )

add_executable(image_decoder image_decoder.cpp)
target_link_libraries( image_decoder ${OpenCV_LIBS} )

add_executable(intra_encoder intra_encoder.cpp)
target_link_libraries( intra_encoder ${OpenCV_LIBS} )

add_executable(intra_decoder intra_decoder.cpp)
target_link_libraries( intra_decoder ${OpenCV_LIBS} )




