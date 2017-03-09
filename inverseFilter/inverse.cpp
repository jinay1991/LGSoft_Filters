/**
 *  inverse.cpp
 *
 *  Contains method for InverseFilter Class and Implementation for inverse Filter
 *  (File also included Unit Test _disabled with compiler switch UNIT_TEST)
 *
 *  Created by Jinay Patel
 *  License - GPL v2
 */

#include "inverse.hpp"

InverseFilter::InverseFilter()
{
    ksize = 3;
    sigma = 0.3;
}
InverseFilter::InverseFilter(size_t ksize_, float sigma_)
{
    ksize = ksize_;
    sigma = sigma_;
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
void InverseFilter::createFilter(float **kernel)
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

void InverseFilter::GaussianFilter(const uint8_t *input, uint8_t *output, int width, int height)
{
    float *GaussianMatrix = NULL;
    createFilter(&GaussianMatrix);
    ConvolutionNxN(input, GaussianMatrix, output, width, height, 3);
    delete[] GaussianMatrix;
}
int InverseFilter::reflect(int M, int x)
{
    if (x < 0)
        return -x - 1;
    if (x >= M)
        return 2 * M - x - 1;
    return x;
}
void InverseFilter::ConvolutionNxN(const uint8_t *input, const float *kernel, uint8_t *output, int width, int height, int ksize)
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

                    dot += (float)input[cur_h * width + cur_w] * kernel[(k_h + kH) * ksize + (k_w + kW)];
                    sum += kernel[(k_h + kH) * ksize + (k_w + kW)];
                }
            }
            output[h * width + w] = (uint8_t) std::max(0, std::min(255, (int)(dot/((sum==0)?1:sum))));
        }
    }
}
void InverseFilter::createInvFilter(const float *filter, float **inv_filter, int width, int height)
{
    (*inv_filter) = new float[width * height];
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            (*inv_filter)[h * width + w] = 1.0f / filter[h * width + w];
        }
    }
}
int InverseFilter::Inverse(const uint8_t *input, uint8_t *output, int width, int height)
{
    std::cout << "ksize == 1, run Laplace with Pre-defined filter matrix 3x3." << std::endl;
    float *GaussianMatrix = NULL;
    createFilter(&GaussianMatrix);
    createInvFilter(GaussianMatrix, &inv_filter, ksize, ksize);
    ConvolutionNxN(input, inv_filter, output, width, height, 3);

    delete[] GaussianMatrix;
    delete[] inv_filter;
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
class inverseFilter : public ::testing::TestWithParam<size_t>
{
};
TEST_P(inverseFilter, ksize)
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
    InverseFilter invl(ksize, 0.4);
    invl.GaussianFilter(input.data, output.data, input.cols, input.rows);
    cv::Mat output2 = cv::Mat::zeros(input.size(), CV_8UC1);

    int result = invl.Inverse(output.data, output2.data, input.cols, input.rows);
    
    EXPECT_EQ(result, 0);

#ifdef COMPARE_RESULTS
    // finding difference between reference and target algorithms, to evaluate.
    cv::Mat diffImage;
    cv::compare(output2, input, diffImage, cv::CMP_NE);
    int diffCount = cv::countNonZero(diffImage);
    EXPECT_EQ(diffCount, 0);
    if (diffCount > 0)
    {
        cv::imshow("diffImage", diffImage);
    }
#endif
#ifndef _NDEBUG

    cv::imshow("inImage", input);
    cv::imshow("Output", output);
    cv::imshow("Inverse Output", output2);
    
    cv::waitKey(0);
#endif

}
INSTANTIATE_TEST_CASE_P(Filters, inverseFilter, ::testing::Values(3));
#endif