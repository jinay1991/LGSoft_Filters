#include "canny.hpp"

void CannyEdgeDetector::Canny(int m)
{
    std::cout << "canny(" << m << ")" << std::endl;
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
    cv::Mat input = cv::imread("/Users/jinay/workspace/git-repo/imfilter/data/apple.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat output = cv::Mat::zeros(input.size(), CV_8UC1);

    size_t ksize = GetParam();
    CannyEdgeDetector c;
    c.Canny(ksize);
    
    EXPECT_EQ(0,0);
}
INSTANTIATE_TEST_CASE_P(Features, cannyDetector, ::testing::Values(8,3));
#endif