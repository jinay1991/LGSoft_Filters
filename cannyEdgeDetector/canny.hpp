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
        size_t kernel[2];

        void Convolution(const uint8_t *x, const double *h_, uint8_t *y, int kWidth, int kHeight, int width, int height);
        void GaussianFilter(const uint8_t *input, uint8_t *output, int width, int height);
    public:
        CannyEdgeDetector();
        CannyEdgeDetector(size_t ksize);
        void Canny(const uint8_t *input, uint8_t *output, int width, int height);
};

#endif //__CANNY_HPP__