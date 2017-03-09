/**
 *  median.cpp
 *
 *  Contains method for MedianFilter Class and Implementation for Median Filter
 *  (File also included Unit Test _disabled with compiler switch UNIT_TEST)
 *
 *  Created by Jinay Patel
 *  License - GPL v2
 */

#include "median.hpp"

MedianFilter::MedianFilter()
{
    ksize = 3;
}

MedianFilter::MedianFilter(size_t ksize_)
{
    ksize = ksize_;
}
int MedianFilter::reflect(int M, int x)
{
    if (x < 0)
        return -x - 1;
    if (x >= M)
        return 2 * M - x - 1;
    return x;
}
int MedianFilter::Median(const uint8_t *input, uint8_t *output, int width, int height)
{
    int w, h;
    int size = ksize * ksize;
    int kH = ksize / 2;
    int kW = ksize / 2;
    int k_h, k_w;

    uint32_t *avg = new uint32_t[size];

    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            memset(avg, 0, sizeof(uint32_t) * size);

            for (k_h = -kH; k_h <= kH; k_h++)
            {
                for (k_w = -kW; k_w <= kW; k_w++)
                {
                    int cur_h = reflect(height, h - k_h);
                    int cur_w = reflect(width, w - k_w);

                    avg[(kH + k_h) * ksize + (kW + k_w)] = (uint32_t)input[cur_h * width + cur_w];
                }
            }
            std::sort(&avg[0], &avg[size]);
            uint32_t median = 0;
            if (size % 2)
                median = avg[size / 2];
            else
                median = (avg[(size / 2) - 1] + avg[size / 2]) / 2;
            output[h * width + w] = (uint8_t)median;
        }
    }
    delete[] avg;
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
class medianFilter : public ::testing::TestWithParam<size_t>
{
};
TEST_P(medianFilter, ksize)
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

    // target algorithm results
    MedianFilter m(ksize);
    int result = m.Median(input.data, output.data, input.cols, input.rows);
    
    EXPECT_EQ(result, 0);

#ifdef COMPARE_RESULTS
    // reference algorithm results
    cv::Mat ocvResult;
    cv::medianBlur(input, ocvResult, ksize);

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
INSTANTIATE_TEST_CASE_P(Filters, medianFilter, ::testing::Values(3));
#endif