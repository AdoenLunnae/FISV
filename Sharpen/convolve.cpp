#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cassert>

using namespace cv;

void convolve(const Mat &in, const Mat &filter, Mat &out, int borderType =0){

    assert(in.type() == CV_32FC1 && filter.type() == CV_32FC1);
}