/**
 *  laplace.hpp
 *   
 *  Contains class definition for LaplaceFilter
 *
 *  Created by Jinay Patel
 *  License - GPL v2
 */
 
#ifndef __LAPLACE_HPP__
#define __LAPLACE_HPP__

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <algorithm>

class LaplaceFilter
{
    private:
        size_t ksize;
        int reflect(int M, int x);
        void ConvolutionNxN(const uint8_t *input, const int *kernel, uint8_t *output, int width, int height, int ksize);
        void DerivativeXY(const uint8_t *input, uint8_t **deltaX, uint8_t **deltaY, int order, int width, int height);
    public:
        LaplaceFilter();
        LaplaceFilter(size_t ksize);
        int Laplace(const uint8_t *input, uint8_t *output, int width, int height);
};

#endif //__LAPLACE_HPP__