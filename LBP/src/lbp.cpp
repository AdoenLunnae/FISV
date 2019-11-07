//! \file lbp.cpp
//! Utils for LBP

#include "lbp.hpp"
#include <vector>

using namespace std;

void fsiv_lbp(const cv::Mat& imagem, cv::Mat& lbp)
{
    lbp = *new cv::Mat(imagem.size(), CV_8UC1);
    std::vector<uchar> neighbours;
    for (int i = 0; i < imagem.rows; ++i) {
        const uchar* imgPtr = imagem.ptr<uchar>(i);
        uchar* lbpPtr = lbp.ptr<uchar>(i);
        for (int j = 0; j < imagem.cols; ++j) {
            uchar pixel = imgPtr[j];
            neighbours = {
                imagem.at<uchar>(i - 1, j - 1),
                imagem.at<uchar>(i - 1, j),
                imagem.at<uchar>(i - 1, j + 1),
                imagem.at<uchar>(i, j + 1),
                imagem.at<uchar>(i + 1, j + 1),
                imagem.at<uchar>(i + 1, j),
                imagem.at<uchar>(i + 1, j - 1),
                imagem.at<uchar>(i, j - 1),
            };
            lbpPtr[j] = 0;
            for (int k = 0; k < 8; ++k) {
                if (neighbours[k] >= pixel) {
                    lbpPtr[j] += pow(2, 7 - k);
                }
            }
        }
    }
}

void fsiv_lbp_hist(const cv::Mat& lbp, cv::Mat& lbp_hist, bool normalize)
{
    cv::calcHist(&lbp, 1, { 0 }, cv::Mat(), lbp_hist, 1, { lbp.cols * lbp.rows }, { { 0, 255 } });
    //cv::calcHist({ lbp }, { 0 }, cv::Mat::zeros(lbp.rows, lbp.cols, CV_8U), lbp_hist, { 256 }, { 0, lbp.rows * lbp.cols });
}

void fsiv_lbp_desc(const cv::Mat& image, cv::Mat& lbp_desc, const int* ncells, bool normalize, bool asrows)
{
    ;
}

void fsiv_lbp_disp(const cv::Mat& lbpmat, const std::string& winname)
{
    cv::namedWindow(winname);
    cv::imshow(winname, lbpmat);
    while (cv::waitKey(0) != 27)
        ;
}

float fsiv_chisquared_dist(const cv::Mat& h1, const cv::Mat& h2)
{
    return -1;
}
