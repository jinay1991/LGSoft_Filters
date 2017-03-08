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
    kernel[0] = kernel[1] = 5;
}
CannyEdgeDetector::CannyEdgeDetector(size_t ksize)
{
    kernel[0] = kernel[1] = ksize;
}
void CannyEdgeDetector::Convolution(const uint8_t *x, const double *h_, uint8_t *y, int kWidth, int kHeight, int width, int height)
{
    int k_w, k_h;
    int kH = kHeight / 2;
    int kW = kWidth / 2;
    int w, h;
    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            double sum = 0;
            double dot = 0;
            for (k_h = -kH; k_h <= kH; k_h++)
            {
                for (k_w = -kW; k_w <= kW; k_w++)
                {
                    int cur_h = std::max(0, h + k_h);
                    int cur_w = std::max(0, w + k_w);
                    cur_h = std::min(height, cur_h);
                    cur_w = std::min(width, cur_w);
                    dot += (double) x[cur_h * width + cur_w] * h_[(kHeight - k_h) * kWidth + (kWidth - k_w)];
                    sum += h_[(kHeight - k_h) * kWidth + (kWidth - k_w)];
                }
            }
            // std::cout << dot << "/" << sum << " ";
            y[h * width + w] = (uint8_t) (dot / sum);
        }
        std::cout << std::endl;
    }
}
void CannyEdgeDetector::GaussianFilter(const uint8_t *input, uint8_t *output, int width, int height)
{
    int w, h;

    uint8_t temp[] = {
        2,  4,  5,  4,  2,
        4,  9, 12,  9,  4,
        5, 12, 15, 12,  5,
        4,  9, 12,  9,  4,
        2,  4,  5,  4,  2
    };
    double *GaussianMatrix = new double[5 * 5];
    memset(GaussianMatrix, 0, sizeof(double) * 5 * 5);
    double inv_K = 159;
    for (h = 0; h < 5; h++)
        for (w = 0; w < 5; w++)
            GaussianMatrix[h * width + w] = (double) temp[h * width + w] / inv_K;
    for (h = 0; h < 5; h++)
    {
        for (w = 0; w < 5; w++)
            std::cout << GaussianMatrix[h * width + w] << " ";
        std::cout << std::endl;
    }
    std::cout << "GaussianMatrix...prepared? Yes" << std::endl;
    Convolution(input, GaussianMatrix, output, 5, 5, width, height);

    delete[] GaussianMatrix;
}
void CannyEdgeDetector::Canny(const uint8_t *input, uint8_t *output, int width, int height)
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
    };

    // perform Gaussian Noise removal to remove extra noise
    GaussianFilter(input, output, width, height);
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
TEST_P(cannyDetector, ksize)
{
    cv::Mat input = cv::imread("/Users/jinay/workspace/git-repo/LGSoft_Filters/data/apple.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat output = cv::Mat::zeros(input.size(), CV_8UC1);

    size_t ksize = GetParam();
    CannyEdgeDetector c(ksize);
    c.Canny(input.data, output.data, input.cols, input.rows);
    
    // reference algorithm results
    cv::Mat ocvResult;
    cv::GaussianBlur(input, ocvResult, cv::Size(ksize, ksize), 0);

    // finding difference between reference and target algorithms, to evaluate.
    cv::Mat diffImage;
    cv::compare(output, ocvResult, diffImage, cv::CMP_NE);
    int diffCount = cv::countNonZero(diffImage);
#ifndef _NDEBUG
    cv::imshow("inImage", input);
    cv::imshow("refImage", ocvResult);
    cv::imshow("outImage", output);
    cv::imshow("diffImage", diffImage);
    cv::waitKey(0);
#endif
    
    EXPECT_EQ(diffCount, 0);
}
INSTANTIATE_TEST_CASE_P(Features, cannyDetector, ::testing::Values(5));
#endif