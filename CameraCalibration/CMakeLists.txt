CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Foreground_Highlighting)
FIND_PACKAGE(OpenCV 	REQUIRED )
include_directories ("${OpenCV_INCLUDE_DIRS}" "include/")

LINK_LIBRARIES(${OpenCV_LIBS} ${XRAND})

add_executable(calibrate src/calibrate.cpp)
add_executable(augReal src/augReal.cpp src/CmdLineParser.cpp)