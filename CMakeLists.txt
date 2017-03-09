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
set(CMAKE_CXX_FLAGS "-Wall")
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build Type - Release")
    add_definitions(-D_NDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build Type - Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -O0")
endif()
# set compiler options such as compiler switch options
option(BUILD_TEST "Build Unit Test applications." ON)

set(MEDIAN_UNIT ${CMAKE_CURRENT_SOURCE_DIR}/medianFilter/median.cpp ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/gtest-all.cc)
set(CANNY_UNIT ${CMAKE_CURRENT_SOURCE_DIR}/cannyEdgeDetector/canny.cpp ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/gtest-all.cc)
set(LAPLACE_UNIT ${CMAKE_CURRENT_SOURCE_DIR}/laplaceFilter/laplace.cpp ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/gtest-all.cc)

set(SOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/medianFilter/median.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/cannyEdgeDetector/canny.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/laplaceFilter/laplace.cpp
)

if(BUILD_TEST)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/apple.jpg ${CMAKE_CURRENT_BINARY_DIR}/data/apple.jpg COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/lena.jpg ${CMAKE_CURRENT_BINARY_DIR}/data/lena.jpg COPYONLY)
    # find dependent packages
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/)
    # set Source files + gtest source
    set(SOURCE ${SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/gtest-all.cc)
    # add compiler define to code
    add_definitions(-DUNIT_TEST)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DCOMPARE_RESULTS)
    endif()
    option(STAND_ALONE "build standalone apps" ON)
    if(STAND_ALONE)
        add_executable(testCanny ${CANNY_UNIT})
        add_executable(testLaplace ${LAPLACE_UNIT})
        add_executable(testMedian ${MEDIAN_UNIT})
        target_link_libraries(testCanny ${OpenCV_LIBS} pthread m)
        target_link_libraries(testLaplace ${OpenCV_LIBS} pthread m)
        target_link_libraries(testMedian  ${OpenCV_LIBS} pthread m)
        set(${EXE} testCanny testLaplace testMedian)
    elseif()
        add_executable(testApp ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCE})
        target_link_libraries(testApp ${OpenCV_LIBS} pthread m)
        set(${EXE} testApp)
    endif()
    install(TARGETS ${EXE} DESTINATION bin)
else()
    add_library(filter SHARED ${SOURCE})
    target_link_libraries(filter m)
    install(TARGETS filter DESTINATION lib)
endif()

install(DIRECTORY data DESTINATION .)