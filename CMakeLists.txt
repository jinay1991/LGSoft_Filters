cmake_minimum_required(VERSION 2.8)

project(imfilter)

file(GLOB SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

find_package(OpenCV REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(filter SHARED ${SOURCE})
install(TARGETS filter DESTINATION lib)

link_directories(${CMAKE_CURRENT_BINARY_DIR}/)
add_executable(testApp ${CMAKE_CURRENT_SOURCE_DIR}/testApp.cpp)
target_link_libraries(testApp ${OpenCV_LIBRARY} filter)
install(TARGETS testApp DESTINATION bin)