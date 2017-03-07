/**
 *  canny.hpp
 *   
 *  Contains class definition for CannyEdgeDetector
 *
 *  Created by Jinay Patel
 *  License - GPL v2
 */

#ifndef __CANNY_HPP__
#define __CANNY_HPP__

#include <iostream>
#include <stdint.h>

class CannyEdgeDetector
{
    private:
        void Convolution(const uint8_t *x, const uint8_t *h, uint8_t *y, int width, int height);
    
    public:
        CannyEdgeDetector()
        {
            std::cout << "CannyEdgeDetector()" << std::endl;
        }
        void Canny(int m);
};

#endif //__CANNY_HPP__