[![Build Status](https://travis-ci.org/jinay1991/imagefilters.svg?branch=master)](https://travis-ci.org/jinay1991/imagefilters)

# imfilter
Sample Stand-alone Implementations for Median Filter, Laplace Filter, Inverse Filter and Canny Edge Detection

PRE_REQUISITES
--------------

1. OpenCV (2.4 or greater) -- Used for Unit Test Applications (to open/read/show images)
2. gcc

BUILD
-----

In order to build the project, please perform following commands

```
$ mkdir build
$ cd build
$ cmake ../
$ make 
```

There are few options which are customised for the CMake for this project. They are, 

```
    CMAKE_BUILD_TYPE    [Debug|Release]     Default: Release. Based on the value it receives certain features such as Debug Logs and Debug windows will turn on/off.
    STAND_ALONE         [ON|OFF]            Default: ON. Builds each filter Unit test in stand alone application or Combined APP.
```

USAGE
-----

There are different Stand alone apps for each of the filter.

```
$ ./testCanny
```
