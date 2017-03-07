cmake_minimum_required(VERSION 2.8)

project(imfilter)

set(CMAKE_CXX_FLAGS "-Wall -g -O0")
set(CMAKE_C_FLAGS "-Wall -g -O0")

option(BUILD_TEST "Build Unit Test applications." ON)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/)

set(MEDIAN_FILTER ${CMAKE_CURRENT_SOURCE_DIR}/medianFilter/median.cpp)
set(CANNY_EDGE_DETECTOR ${CMAKE_CURRENT_SOURCE_DIR}/cannyEdgeDetector/canny.cpp)
set(GTEST ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/gtest-all.cc)

set(SOURCE ${MEDIAN_FILTER} ${CANNY_EDGE_DETECTOR} ${GTEST})

if(BUILD_TEST)
    add_definitions(-DUNIT_TEST)
    add_executable(testApp ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCE})
    target_link_libraries(testApp ${OpenCV_LIBS} )
    install(TARGETS testApp DESTINATION bin)
else()
    add_library(filter SHARED ${SOURCE})
    install(TARGETS filter DESTINATION lib)
endif()
install(DIRECTORY data DESTINATION .)