#ifndef _CONVOLVE_HPP_
#define _CONVOLVE_HPP_
    #include "opencv2/core/core.hpp"

    void convolve(const cv::Mat &in, const cv::Mat &filter, cv::Mat &out, int borderType = 0);

#endif