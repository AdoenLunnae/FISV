cmake_minimum_required (VERSION 3.0)
project (fsivlbp)
set(CMAKE_CXX_FLAGS "-std=c++11 -g")
find_package(OpenCV REQUIRED)

LINK_LIBRARIES(${OpenCV_LIBS})
include_directories("include/")

add_executable(lbpdesc src/main.cpp src/lbp.cpp)
