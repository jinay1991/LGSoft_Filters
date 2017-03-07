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
    kernel[0] = 3;
    kernel[1] = 3;
}

MedianFilter::MedianFilter(size_t ksize)
{
    kernel[0] = ksize;
    kernel[1] = ksize;
}

int MedianFilter::Median(const uint8_t *input, uint8_t *output, int width, int height)
{
    int w, h;
    int size = kernel[0] * kernel[1];
    int kH = kernel[0] / 2;
    int kW = kernel[1] / 2;
    int k_h, k_w;

    uint32_t* avg = new uint32_t[size];
    
    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            memset(avg, 0, sizeof(uint32_t) * size);
            for (k_h = -kH; k_h <= kH; k_h++)
            {
                for (k_w = -kW; k_w <= kW; k_w++)
                {
                    int cur_w = std::max(0, w + k_w);
                    int cur_h = std::max(0, h + k_h);
                    cur_w = std::min(width, cur_w);
                    cur_h = std::min(height, cur_h);
                    avg[(kH + k_h) * kernel[0] + (kW + k_w)] = (uint32_t) input[cur_h * width + cur_w];
                }
            }
            
            std::sort(&avg[0], &avg[size]);
            uint32_t median = 0;
            if (size % 2)
                median = avg[size / 2];
            else
                median = (avg[(size/2) - 1] + avg[size/2]) / 2;
            output[h * width + w] = (uint8_t) median;
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
class medianFilter: public ::testing::TestWithParam<size_t>{};
TEST_P(medianFilter, ksize)
{
    cv::Mat input = cv::imread("/Users/jinay/workspace/git-repo/LGSoft_Filters/data/lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    ASSERT_NE(input.rows, 0) << "input dimensions: " << input.size() << std::endl;
    ASSERT_NE(input.cols, 0) << "input dimensions: " << input.size() << std::endl;;
    cv::Mat output = cv::Mat::zeros(input.size(), CV_8UC1);
    size_t ksize = GetParam();

    // target algorithm results
    MedianFilter m(ksize);
    m.Median(input.data, output.data, input.cols, input.rows);

    // reference algorithm results
    cv::Mat ocvResult;
    cv::medianBlur(input, ocvResult, ksize);

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
INSTANTIATE_TEST_CASE_P(Filters, medianFilter, ::testing::Values(15));
#endif 