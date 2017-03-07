/**
 *  main.cpp
 *   
 *  Contains main() function
 *
 *  Created by Jinay Patel
 *  License - GPL v2
 */

#include "gtest/gtest.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}