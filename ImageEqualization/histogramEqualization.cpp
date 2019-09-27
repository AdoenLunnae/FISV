#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;

int sum(float array[256]){
    int result = 0;
    for(int i = 0; i < 256; ++i){
        result += (int)array[i];
    }
    return result;
}

void getHistogram(Mat image, float histogram[256]){
    for(int i = 0; i < 256; ++i) histogram[i] = 0.0;
    uchar *ptr = image.ptr<uchar>(0);
    for(int i = 0; i < (image.rows * image.cols); ++i){
        histogram[*(ptr+i)] += 1;
    }
}

void normalizeHistogram(float histogram[256], int imageSize){
    for(int i = 0; i < 256; ++i){
        histogram[i] = histogram[i] * 255.0/imageSize;
    }
}

void cumulativeHistogram(float histogram[256]){
    for(int i = 1; i < 256; ++i){
        histogram[i] += histogram[i-1];
    }
}

void applyEqualization(float histogram[256], Mat image, Mat &edited){
    uchar *oPtr = image.ptr<uchar>(0);
    uchar *ePtr = edited.ptr<uchar>(0);

    for(int i = 0; i < (image.rows * image.cols); ++i){
        *(ePtr+i) = (uchar) histogram[*(oPtr + i)];
    }
}

int main(int argc, char** argv){
    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input image> <output image>" << '\n';
        return 1;
    }

    Mat original = imread(argv[1], IMREAD_GRAYSCALE);
    float hist[256];

    if(original.rows == 0){
        std::cerr << "Error reading input image" << '\n';
        return 1;
    }

    Mat edited(original.rows, original.cols, CV_8UC1);

    namedWindow("Original");
    imshow("Original", original);

    getHistogram(original, hist);
    normalizeHistogram(hist, original.rows * original.cols);


    cumulativeHistogram(hist);
    applyEqualization(hist, original, edited);

    namedWindow("Equalized");
    imshow("Equalized", edited);

    imwrite(argv[2], edited);
    while (waitKey(0) != 27);

}
