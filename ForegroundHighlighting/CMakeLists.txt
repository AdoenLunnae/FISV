CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Foreground_Highlighting)
FIND_PACKAGE(OpenCV 	REQUIRED )
include_directories ("${OpenCV_INCLUDE_DIRS}")

LINK_LIBRARIES(${OpenCV_LIBS} ${XRAND})

add_executable(rectangle foregroundRectangle.cpp)
add_executable(polygon foregroundPolygon.cpp)
