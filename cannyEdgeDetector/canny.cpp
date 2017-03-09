/**
 *  canny.cpp
 *
 *  Contains method for CannyEdgeDetector Class and Implementation for Canny Edge Detector
 *  (File also included Unit Test _disabled with compiler switch UNIT_TEST)
 *
 *  Created by Jinay Patel
 *  License - GPL v2
 */

#include "canny.hpp"
CannyEdgeDetector::CannyEdgeDetector()
{
    sigma = 0.4f;
    ksize = 3;
    tlow = 100;
    thigh = 200;
}
CannyEdgeDetector::CannyEdgeDetector(size_t ksize_, float sigma_, int tlow_, int thigh_)
{
    sigma = sigma_;
    ksize = ksize_;
    tlow = tlow_;
    thigh = thigh_;
}
/* gaussianFilter:
  * http://www.songho.ca/dsp/cannyedge/cannyedge.html
  * determine size of kernel (odd #)
  * 0.0 <= sigma < 0.5 : 3
  * 0.5 <= sigma < 1.0 : 5
  * 1.0 <= sigma < 1.5 : 7
  * 1.5 <= sigma < 2.0 : 9
  * 2.0 <= sigma < 2.5 : 11
  * 2.5 <= sigma < 3.0 : 13 ...
  * kernelSize = 2 * int(2*sigma) + 3;
  */
void CannyEdgeDetector::createFilter(float **kernel)
{
    // set standard deviation to 1.0
    if (sigma == 0)
        sigma = 1.0;
    if (ksize == 0)
        ksize = 2 * (int)(2 * sigma) + 3;
    float r, s = 2.0 * sigma * sigma;

    std::cout << "ksize: " << ksize << " sigma: " << sigma << std::endl;

    if (*kernel == NULL)
        *kernel = new float[ksize * ksize];

    // sum is for normalization
    float sum = 0.0;
    int kH = (int)ksize / 2;
    int kW = (int)ksize / 2;
    // generate NxN kernel
    for (int h = -kH; h <= kH; h++)
    {
        for (int w = -kW; w <= kW; w++)
        {
            r = sqrt(w * w + h * h);

            (*kernel)[(h + kH) * ksize + (w + kW)] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += (*kernel)[(h + kH) * ksize + (w + kW)];
        }
    }

    // normalize the Kernel
    for (int h = 0; h < (int)ksize; ++h)
        for (int w = 0; w < (int)ksize; ++w)
            (*kernel)[h * ksize + w] /= sum;
}

int CannyEdgeDetector::reflect(int M, int x)
{
    if (x < 0)
        return -x - 1;
    if (x >= M)
        return 2 * M - x - 1;
    return x;
}
void CannyEdgeDetector::ConvolutionNxN(const uint8_t *input, const float *kernel, uint8_t *output, int width, int height, int ksize)
{
    int kH = ksize / 2;
    int kW = ksize / 2;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            float dot = 0;
            float sum = 0;
            for (int k_h = -kH; k_h <= kH; k_h++)
            {
                for (int k_w = -kW; k_w <= kW; k_w++)
                {
                    int cur_h = reflect(height, h - k_h);
                    int cur_w = reflect(width, w - k_w);

                    dot += input[cur_h * width + cur_w] * kernel[(k_h + kH) * ksize + (k_w + kW)];
                    sum += kernel[(k_h + kH) * ksize + (k_w + kW)];
                }
            }
            output[h * width + w] = (uint8_t)std::max(0, std::min(255, (int)(dot/((sum==0)?1:sum))));
        }
    }
}
void CannyEdgeDetector::GaussianFilter(const uint8_t *input, uint8_t *output, int width, int height)
{
    float *GaussianMatrix = NULL;
    createFilter(&GaussianMatrix);
    ConvolutionNxN(input, GaussianMatrix, output, width, height, 3);
    delete[] GaussianMatrix;
}

void CannyEdgeDetector::DerivativeXY(const uint8_t *input, uint8_t **deltaX, uint8_t **deltaY, int order, int width, int height)
{
    *deltaX = new uint8_t[width * height];
    *deltaY = new uint8_t[width * height];
    uint8_t *temp = new uint8_t[width * height];

    memcpy(temp, input, sizeof(uint8_t) * width * height);
    memset(*deltaX, 0, sizeof(uint8_t) * width * height);
    memset(*deltaY, 0, sizeof(uint8_t) * width * height);

    float gx[] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1};
    float gy[] = {
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1};
    for (int o = 0; o < order; o++)
    {
        ConvolutionNxN(temp, gx, *deltaX, width, height, 3);
        memcpy(temp, *deltaX, sizeof(uint8_t) * width * height);
    }
    memcpy(temp, input, sizeof(uint8_t) * width * height);
    for (int o = 0; o < order; o++)
    {
        ConvolutionNxN(input, gy, *deltaY, width, height, 3);
        memcpy(temp, *deltaY, sizeof(uint8_t) * width * height);
    }

    delete[] temp;
}
void CannyEdgeDetector::MagnitudeXY(const uint8_t *deltaX, const uint8_t *deltaY, uint8_t **magnitudeXY, int width, int height)
{
    int w, h;

    *magnitudeXY = new uint8_t[width * height];

    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            (*magnitudeXY)[h * width + w] = (uint8_t)hypot(deltaX[h * width + w], deltaY[h * width + w]);
        }
    }
}
void CannyEdgeDetector::NonMaximalSuppression(const uint8_t *magnitude, const uint8_t *deltaX, const uint8_t *deltaY, int width, int height, uint8_t **nonmaxsup)
{
    int w, h;

    *nonmaxsup = new uint8_t[width * height];
    memset(*nonmaxsup, 0, sizeof(uint8_t) * width * height);

    for (h = 1; h < height - 1; h++)
    {
        for (w = 1; w < width - 1; w++)
        {
            int pos = h * width + w;

            float dir = (float)(fmod(atan2(deltaY[pos], deltaX[pos]) + 3.14, 3.14) / 3.14) * 8;

            int ne = (h - 1) * width + (w + 1);
            int sw = (h + 1) * width + (w - 1);

            int nn = (h - 1) * width + w;
            int ss = (h + 1) * width + w;

            int nw = (h - 1) * width + (w - 1);
            int se = (h + 1) * width + (w + 1);

            int ee = h * width + (w + 1);
            int ww = h * width + (w - 1);

            int center = pos;

            if (((dir <= 1 || dir > 7) && magnitude[center] > magnitude[ee] && magnitude[center] > magnitude[ww]) || //   0 degree
                ((dir > 1 || dir <= 3) && magnitude[center] > magnitude[nw] && magnitude[center] > magnitude[se]) || //  45 degree
                ((dir > 3 || dir <= 5) && magnitude[center] > magnitude[nn] && magnitude[center] > magnitude[ss]) || //  90 degree
                ((dir > 5 || dir <= 7) && magnitude[center] > magnitude[ne] && magnitude[center] > magnitude[sw]))   // 135 degree
                (*nonmaxsup)[center] = magnitude[center];
            else
                (*nonmaxsup)[center] = 0;
        }
    }
    delete[] deltaX;
    delete[] deltaY;
    delete[] magnitude;
}
void CannyEdgeDetector::DoubleThresholding(const uint8_t *nonmaxsup, uint8_t *output, int tmin, int tmax, int width, int height)
{
    int w, h;
    uint8_t *detected_edge = new uint8_t[width * height];
    memset(detected_edge, 0, sizeof(uint8_t) * width * height);
    memset(output, 0, sizeof(uint8_t) * width * height);

    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            int center = h * width + w;

            if (nonmaxsup[center] > tmax)
                output[center] = (uint8_t)255;
            else if (nonmaxsup[center] < tmin)
                output[center] = (uint8_t)0;
            else
            {
                // output[center] = (uint8_t) 128;

                bool anyHigh = false;
                bool anyBetween = false;
                for (int k_h = h - 1; k_h < h + 1; k_h++)
                {
                    for (int k_w = w - 1; k_w < w + 1; k_w++)
                    {
                        if (k_w < 0 || k_h < 0 || k_w > width || k_h > height)
                            continue;

                        if (output[k_h * width + k_w] > tmax)
                        {
                            output[h * width + w] = (uint8_t)255;
                            anyHigh = true;
                            break;
                        }
                        else if (output[k_h * width + k_w] < tmax && output[k_h * width + k_w] >= tmin)
                            anyBetween = true;
                    }
                    if (anyHigh)
                        break;
                }
                if (anyBetween && !anyHigh)
                {
                    for (int k_h = h - 2; k_h < h + 2; k_h++)
                    {
                        for (int k_w = w - 2; k_w < w + 2; k_w++)
                        {
                            if (k_w < 0 || k_h < 0 || k_w > width || k_h > height)
                                continue;
                            if (output[k_h * width + k_w] > tmax)
                            {
                                output[h * width + w] = (uint8_t)255;
                                anyHigh = true;
                                break;
                            }
                        }
                        if (anyHigh)
                            break;
                    }
                }
                if (!anyHigh)
                    output[h * width + w] = 0;
            }
        }
    }

    delete[] detected_edge;
    delete[] nonmaxsup;
}
int CannyEdgeDetector::Canny(const uint8_t *input, uint8_t *output, int width, int height)
{
    uint8_t *deltaX = NULL;
    uint8_t *deltaY = NULL;
    uint8_t *magnitude = NULL;
    uint8_t *nms = NULL;

    GaussianFilter(input, output, width, height);

    DerivativeXY(output, &deltaX, &deltaY, 1, width, height);

    MagnitudeXY(deltaX, deltaY, &magnitude, width, height);

    NonMaximalSuppression(magnitude, deltaX, deltaY, width, height, &nms);

    DoubleThresholding(nms, output, tlow, thigh, width, height);

    return 0;
}

// --------------------------------------------------------------
//      U N I T    T E S T
// --------------------------------------------------------------
#if UNIT_TEST
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "gtest/gtest.h"
class cannyDetector : public ::testing::TestWithParam<size_t>
{
};
TEST_P(cannyDetector, ksize)
{
    char cwd[1024];
    std::string dirpath;
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        dirpath = std::string(cwd);
    cv::Mat input = cv::imread(dirpath + "/data/lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    ASSERT_NE(input.rows, 0) << "input dimensions: " << input.size() << std::endl;
    ASSERT_NE(input.cols, 0) << "input dimensions: " << input.size() << std::endl;
    cv::Mat output = cv::Mat::zeros(input.size(), CV_8UC1);

    size_t ksize = GetParam();
    int tlow = 60;
    int thigh = 120;
    CannyEdgeDetector c(ksize, 0.4, tlow, thigh);
    int result = c.Canny(input.data, output.data, input.cols, input.rows);

    EXPECT_EQ(result, 0);

#ifdef COMPARE_RESULTS
    // reference algorithm results
    cv::Mat ocvResult;
    cv::GaussianBlur(input, ocvResult, cv::Size(ksize, ksize), 0.4);
    cv::Canny(ocvResult, ocvResult, tlow, thigh, ksize);

    // finding difference between reference and target algorithms, to evaluate.
    cv::Mat diffImage;
    cv::compare(output, ocvResult, diffImage, cv::CMP_NE);
    int diffCount = cv::countNonZero(diffImage);
    EXPECT_EQ(diffCount, 0);
    if(diffCount > 0)
    {
        cv::imshow("refImage", ocvResult);
        cv::imshow("diffImage", diffImage);
    }
#endif
#ifndef _NDEBUG
    cv::imshow("inImage", input);
    cv::imshow("outImage", output);
    
    cv::waitKey(0);
#endif

}
INSTANTIATE_TEST_CASE_P(Features, cannyDetector, ::testing::Values(3));
#endif

// void CannyEdgeDetector::createGaussianMatrix1D(float sigma, float **kernel, int *windowsize)
// {
//     int i, center;
//     float x, fx, sum = 0.0;

//     *windowsize = 1 + 2 * ceil(2.5 * sigma);
//     center = (*windowsize) / 2;

//     if ((*kernel = (float *)calloc((*windowsize), sizeof(float))) == NULL)
//     {
//         std::cerr << "Error callocing the gaussian kernel ahhay." << std::endl;
//         return;
//     }

//     for (i = 0; i < (*windowsize); i++)
//     {
//         x = (float)(i - center);
//         fx = pow(2.71828, -0.5 * x * x / (sigma * sigma)) / (sigma * sqrt(6.2831853));
//         (*kernel)[i] = fx;
//         sum += fx;
//     }

//     for (i = 0; i < (*windowsize); i++)
//     {
//         if (sum != 0)
//             (*kernel)[i] /= sum;
//     }
// #ifndef _NDEBUG
//     std::cout << "The filter coefficients are:" << std::endl;
//     for (i = 0; i < (*windowsize); i++)
//         std::cout << "kernel[" << i << "] = " << (*kernel)[i] << std::endl;
// #endif
// }
// void CannyEdgeDetector::GaussianFilter(const uint8_t *input, uint8_t *output, int width, int height)
// {
//     // int w, h;

//     float *GaussianMatrix = NULL;
//     createFilter(sigma, &GaussianMatrix, ksize);
//     ConvolutionNxN(input, GaussianMatrix, output, width, height, 3);
//     delete[] GaussianMatrix;
//     // float *temp = new float[width * height];
//     // memset(temp, 0, sizeof(float) * width * height);

//     // //  x - direction
//     // for (h = 0; h < height; h++)
//     // {
//     //     for (w = 0; w < width; w++)
//     //     {
//     //         float dot = 0.0;
//     //         float sum = 0.0;
//     //         int ww = 0;
//     //         for (ww = -center; ww <= center; ww++)
//     //         {
//     //             if ((w + ww) >= 0 && (w + ww) < width)
//     //             {
//     //                 dot += (float)input[h * width + (w + ww)] * GaussianMatrix[center + ww];
//     //                 sum += (float)GaussianMatrix[center + ww];
//     //             }
//     //         }
//     //         temp[h * width + w] = dot;
//     //     }
//     // }

//     // // y - direction
//     // for (w = 0; w < width; w++)
//     // {
//     //     for (h = 0; h < height; h++)
//     //     {
//     //         float dot = 0.0;
//     //         float sum = 0.0;
//     //         int hh = 0;
//     //         for (hh = -center; hh <= center; hh++)
//     //         {
//     //             if ((h + hh) >= 0 && (h + hh) < height)
//     //             {
//     //                 dot += (float)temp[(h + hh) * width + w] * GaussianMatrix[center + hh];
//     //                 sum += (float)GaussianMatrix[center + hh];
//     //             }
//     //         }
//     //         output[h * width + w] = (uint8_t)(dot);
//     //     }
//     // }
//     // delete[] temp;
//     // delete[] GaussianMatrix;
// }
