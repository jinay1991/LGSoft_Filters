#include <iostream>

class CannyEdgeDetector
{
    public:
        CannyEdgeDetector()
        {
            std::cout << "CannyEdgeDetector()" << std::endl;
        }

        void Canny(int m);
};