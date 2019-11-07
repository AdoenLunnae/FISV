#ifndef _DIFF_GAUSSIANS_HPP_
#define _DIFF_GAUSSIANS_HPP_
    #include "opencv2/core/core.hpp"
    
    cv::Mat getKernel(int size, float sigma);
    cv::Mat createDoGFilter(float sigma1, float sigma2); 

#endif