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
void CannyEdgeDetector::Convolution(const uint8_t *x, const uint8_t *h, uint8_t *y, int width, int height)
{
    int w, h;

    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            y[h * width + w] = x[h * width + w] * h[(height - h) * width + (width - w)];
        }
    }
}
void CannyEdgeDetector::GaussianFilter(const uint8_t *input, uint8_t *output, int width, int height)
{
    int w, h;

    int GaussianMatrix[] = {
        2,  4,  5,  4,  2,
        4,  9, 12,  9,  4,
        5, 12, 15, 12,  5,
        4,  9, 12,  9,  4,
        2,  4,  5,  4,  2
    };
    int inv_K = 159;

    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            output[ h * width + w ] = Convolution(input[h * width + w], 
        }
    }
}
void CannyEdgeDetector::Canny(int m)
{
    int gx[] = {
        -1, 0, 1,
        -2, 0, 2,
        -3, 0, 3
    };
    int gy[] = {
        -1, -2, -3,
         0,  0,  0,
         1,  2,  3
    }

    // perform Gaussian Noise removal to remove extra noise
}


// --------------------------------------------------------------
//      U N I T    T E S T
// --------------------------------------------------------------
#if UNIT_TEST
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "gtest/gtest.h"
class cannyDetector: public ::testing::TestWithParam<size_t>{};
TEST_P(cannyDetector, DISABLED_ksize)
{
    cv::Mat input = cv::imread("/Users/jinay/workspace/git-repo/LGSoft_Filters/data/apple.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat output = cv::Mat::zeros(input.size(), CV_8UC1);

    size_t ksize = GetParam();
    CannyEdgeDetector c;
    c.Canny(ksize);
    
    EXPECT_EQ(0,0);
}
INSTANTIATE_TEST_CASE_P(Features, cannyDetector, ::testing::Values(8,3));
#endif