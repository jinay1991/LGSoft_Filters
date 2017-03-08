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
#include <string.h>
#include <algorithm>
#include <math.h>

class CannyEdgeDetector
{
    private:
        size_t ksize;
        float sigma;
        void createFilter(float **kernel);
        int reflect(int M, int x);
        void ConvolutionNxN(const uint8_t *input, const float *kernel, uint8_t *output, int width, int height, int ksize);
        void GaussianFilter(const uint8_t *input, uint8_t *output, int width, int height);
        void DerivativeXY(const uint8_t *input, uint8_t **deltaX, uint8_t **deltaY, int order, int width, int height);
        void MagnitudeXY(const uint8_t *deltaX, const uint8_t *deltaY, uint8_t **magnitudeXY, int width, int height);
        void NonMaximalSuppression(const uint8_t *magnitude, const uint8_t *deltaX, const uint8_t *deltaY, int width, int height, uint8_t **nonmaxsup);
        void Hysteresis(const uint8_t *nonmaxsup, uint8_t *output, int lowThresh, int highThresh, int width, int height);

    public:
        CannyEdgeDetector();
        CannyEdgeDetector(size_t ksize, float s);

        void Canny(const uint8_t *input, uint8_t *output, int width, int height);
};

#endif //__CANNY_HPP__