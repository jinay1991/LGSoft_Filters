/**
 *  laplace.cpp
 *
 *  Contains method for LaplaceFilter Class and Implementation for Laplace Filter
 *  (File also included Unit Test _disabled with compiler switch UNIT_TEST)
 *
 *  Created by Jinay Patel
 *  License - GPL v2
 */

#include "laplace.hpp"

LaplaceFilter::LaplaceFilter()
{
    ksize = 3;
}

LaplaceFilter::LaplaceFilter(size_t ksize_)
{
    ksize = ksize_;
}

int LaplaceFilter::reflect(int M, int x)
{
    if (x < 0)
        return -x - 1;
    if (x >= M)
        return 2 * M - x - 1;
    return x;
}
void LaplaceFilter::ConvolutionNxN(const uint8_t *input, const int *kernel, uint8_t *output, int width, int height, int ksize)
{
    int kH = ksize / 2;
    int kW = ksize / 2;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            int dot = 0;
            int sum = 0;
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
            output[h * width + w] = (uint8_t) std::max(0, std::min(255, (int)dot/((sum==0)?1:sum)));
        }
    }
}
void LaplaceFilter::DerivativeXY(const uint8_t *input, uint8_t **deltaX, uint8_t **deltaY, int order, int width, int height)
{
    *deltaX = new uint8_t[width * height];
    *deltaY = new uint8_t[width * height];
    uint8_t *temp = new uint8_t[width * height];

    memcpy(temp, input, sizeof(uint8_t) * width * height);
    memset(*deltaX, 0, sizeof(uint8_t) * width * height);
    memset(*deltaY, 0, sizeof(uint8_t) * width * height);

    int gx[] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1};
    int gy[] = {
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
int LaplaceFilter::Laplace(const uint8_t *input, uint8_t *output, int width, int height)
{
    int w, h;

    if (ksize == 1)
    {
        std::cout << "ksize == 1, run Laplace with Pre-defined filter matrix 3x3." << std::endl;
        int filter[9] = {0, 1, 0,
                         1, -4, 1,
                         0, 1, 0};
        ConvolutionNxN(input, filter, output, width, height, 3);
    }
    else
    {
        uint8_t *deltaX = NULL;
        uint8_t *deltaY = NULL;
        DerivativeXY(input, &deltaX, &deltaY, 2, width, height);
        for (h = 0; h < height; h++)
        {
            for (w = 0; w < width; w++)
            {
                output[h * width + w] = (deltaX[h * width + w] + deltaY[h * width + w]);
            }
        }
        delete[] deltaX;
        delete[] deltaY;
    }
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
class laplaceFilter : public ::testing::TestWithParam<size_t>
{
};
TEST_P(laplaceFilter, ksize)
{
    char cwd[1024];
    std::string dirpath;
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        dirpath = std::string(cwd);
    std::cout << dirpath << std::endl;
    cv::Mat input = cv::imread(dirpath + "/data/lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    ASSERT_NE(input.rows, 0) << "input dimensions: " << input.size() << std::endl;
    ASSERT_NE(input.cols, 0) << "input dimensions: " << input.size() << std::endl;
    cv::Mat output = cv::Mat::zeros(input.size(), CV_8UC1);

    size_t ksize = GetParam();
    // target algorithm results
    LaplaceFilter l(ksize);
    int result = l.Laplace(input.data, output.data, input.cols, input.rows);

    EXPECT_EQ(result, 0);

#ifdef COMPARE_RESULTS
    // reference algorithm results
    cv::Mat ocvResult;
    cv::Laplacian(input, ocvResult, CV_8U, ksize);

    // finding difference between reference and target algorithms, to evaluate.
    cv::Mat diffImage;
    cv::compare(output, ocvResult, diffImage, cv::CMP_NE);
    int diffCount = cv::countNonZero(diffImage);
    EXPECT_EQ(diffCount, 0);
    if (diffCount > 0)
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
INSTANTIATE_TEST_CASE_P(Filters, laplaceFilter, ::testing::Values(1, 3));
#endif