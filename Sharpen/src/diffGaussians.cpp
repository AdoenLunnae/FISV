#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "diffGaussians.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

using cv::Mat;
using cv::getGaussianKernel;

cv::Mat getKernel(int size, float sigma){
    const double pi = 3.14159265358979323846;
    const double e = 2.71828182845904523536;
    cv::Mat filter(size, size, CV_32FC1);
    for(int i = 0; i < filter.rows; ++i){
        float* ptr = filter.ptr<float>(i);
        for (int j = 0; j < filter.cols; j++){
            float x = i - floor(size/2.);
            float y = j - floor(size/2.);
            float a = 1/(sigma*sqrt(2*pi));
            float g = a*pow(e, - ((pow(x, 2)/(2*pow(sigma, 2))) + (pow(y, 2)/(2*pow(sigma, 2)))));
            ptr[j] = g;
        }
    }
    return filter;
}

cv::Mat createDoGFilter(float sigma1, float sigma2){
    assert(sigma1 < sigma2);
    int size = (ceil(3*sigma2));
    Mat filter = getKernel(size, sigma1);
    cv::subtract(filter, getKernel(size, sigma2), filter);
    return filter;
}