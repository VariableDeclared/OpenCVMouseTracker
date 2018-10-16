cmake_minimum_required(VERSION 3.0)

if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

project( ComputerVision )
find_package( OpenCV REQUIRED )
add_executable( ComputerVision main.cpp )
target_link_libraries( ComputerVision ${OpenCV_LIBS} )

