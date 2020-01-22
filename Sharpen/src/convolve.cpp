#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace cv;


float getConvolutionAtPoint(const Mat &m, const Mat &filter){
    assert(m.rows == filter.rows);
    assert(m.cols == filter.cols);
    float result = 0.0;
    for(int i = 0; i < m.rows; ++i){
        const float* mPtr = m.ptr<float>(i);
        const float* fPtr = filter.ptr<float>(i);
        for(int j = 0; j < m.cols; ++j){
            result += mPtr[j] * fPtr[j];
        }
    }
    return result;
}

void convolve(const Mat &in, const Mat &filter, Mat &out, int borderType = 0){

    assert(in.type() == CV_32FC1 && filter.type() == CV_32FC1);
    assert(filter.rows == filter.cols);
    int extraSize = floor(filter.rows / 2.);
    Mat augmented(in.rows + 2*extraSize, in.cols + 2*extraSize, CV_32FC1);
    //std::cout << "Convolving: \n";

    in.copyTo(augmented(Rect(extraSize, extraSize, in.cols, in.rows)));

    out = *new Mat(in.rows, in.cols, CV_32FC1);
    
    for(int i = 0; i < out.rows; ++i){
        float* ptr = out.ptr<float>(i);
        for(int j = 0; j < out.cols; ++j){
            Mat roi = augmented(Range(i, i+filter.rows), Range(j, j+filter.cols));
            
            ptr[j] = getConvolutionAtPoint(roi, filter);
        }
    }
}
