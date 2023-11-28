cmake_minimum_required(VERSION 2.8)
project(opencv_projects)
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( color_channel color_channel.cpp)
target_link_libraries( color_channel ${OpenCV_LIBS} )

add_executable(ex2 ex2.cpp)
target_link_libraries( ex2 ${OpenCV_LIBS} )

# add_executable(image_encoder image_encoder.cpp)
# target_link_libraries( image_encoder ${OpenCV_LIBS} )

# add_executable(image_decoder image_decoder.cpp)
# target_link_libraries( image_decoder ${OpenCV_LIBS} )

add_executable(golombTest golombTest.cpp golomb.h)
target_link_libraries(golombTest ${OpenCV_LIBS})
