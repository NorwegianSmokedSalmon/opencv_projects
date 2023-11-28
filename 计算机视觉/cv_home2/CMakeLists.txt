cmake_minimum_required(VERSION 3.2)
project(ellipse)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS}) 

add_executable(ellipse ellipse.cpp)
target_link_libraries(ellipse ${OpenCV_LIBS})

