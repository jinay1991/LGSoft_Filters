/**
 *  inverse.hpp
 *   
 *  Contains class definition for InverseFilter
 *
 *  Created by Jinay Patel
 *  License - GPL v2
 */
 
#ifndef __INVERSE_HPP__
#define __INVERSE_HPP__

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <math.h>

class InverseFilter
{
    private:
        float sigma;
        size_t ksize;
        float *inv_filter;
        int reflect(int M, int x);
        void ConvolutionNxN(const uint8_t *input, const float *kernel, uint8_t *output, int width, int height, int ksize);
        void createInvFilter(const float *filter, float **inv_filter, int width, int hight);
        void createFilter(float **kernel);
    public:
        InverseFilter();
        void GaussianFilter(const uint8_t *input, uint8_t *output, int width, int height);
        int Laplace(const uint8_t *input, uint8_t *output, int width, int height);
        int Inverse(const uint8_t *input, uint8_t *output, int width, int height);
};

#endif //__INVERSE_HPP__