#include <iostream>
#include <stdint.h>

class MedianFilter
{
    private:
        size_t kernel[2];
    
    public:
        MedianFilter();
        MedianFilter(size_t ksize);
        int Median(const uint8_t *input, uint8_t *output, int width, int height);
};