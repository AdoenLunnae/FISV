#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "laplacian.hpp"

using namespace cv;


Mat createSharpFilter(int type, float gain){
    Mat filter(3, 3, CV_32FC1);
    filter = Scalar(0);
    for(int x = 0; x < 3; ++x){
        for(int y = 0; y < 3; ++y){
            if((x == 0 && y == 0) || (x == 0 && y == 2) || (x == 2 && y ==0) || (x == 2 && y== 2)){
                if(type){
                    filter.at<float>(x, y) = -1;
                }
            }
            else if(x == y == 1){
                filter.at<float>(x, y) = gain + (type?8:4);
            }
            else{
                filter.at<float>(x, y) = -1;
            }
        }
    }
    return filter;
}
 