#include <iostream>
#include "median.hpp"
#include "canny.hpp"
int main(int argc, char *argv[])
{
    std::cout << "Starting Test..." << std::endl;
    bool isPassed = false;
    MedianFilter m;
    m.Median();
    CannyEdgeDetector c;
    c.Canny(3);
    std::cout << "Ending Test with..." << (isPassed ? "PASS" : "FAIL") << std::endl;
    return 0;
}