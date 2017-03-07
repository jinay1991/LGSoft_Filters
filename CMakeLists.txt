# -------------------------------------------------------------------------------------------
#   CMakeLists.txt
#
#   Build Support File.
#
#   Created by Jinay Patel
#   License - GPL v2
# -------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)

project(LGSoft_Filters)

# set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -g -O0")
set(CMAKE_C_FLAGS "-Wall -g -O0")

# set compiler options such as compiler switch options
option(BUILD_TEST "Build Unit Test applications." ON)

set(SOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/medianFilter/median.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/cannyEdgeDetector/canny.cpp
)

if(BUILD_TEST)
    # find dependent packages
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/)
    # set Source files + gtest source
    set(SOURCE ${SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/gtest-all.cc)
    # add compiler define to code
    add_definitions(-DUNIT_TEST)
    add_executable(testApp ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCE})
    target_link_libraries(testApp ${OpenCV_LIBS} )
    install(TARGETS testApp DESTINATION bin)
else()
    add_library(filter SHARED ${SOURCE})
    install(TARGETS filter DESTINATION lib)
endif()

install(DIRECTORY data DESTINATION .)