//! \file lbp.cpp
//! Utils for LBP

#include "lbp.hpp"
#include <cassert>
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
                    lbpPtr[j] = lbpPtr[j] | (1 << 7 - k);
                }
            }
        }
    }
}

void fsiv_lbp_hist(const cv::Mat& lbp, cv::Mat& lbp_hist, bool normalize)
{
    int size = 256;
    std::vector<cv::Mat> images = { lbp };
    std::vector<int> channels = { 0 }, sizes = { size };
    std::vector<float> range = { 0, 256 };
    cv::calcHist(images, channels, cv::Mat(), lbp_hist, sizes, range);
    if (normalize) {
        //cv::normalize(lbp_hist, lbp_hist, 0.0, 1, cv::NORM_L1);
        lbp_hist = lbp_hist.mul(1.0 / (lbp.cols * lbp.rows));
    }
}

//OPTIONAL
void fsiv_lbp_desc(const cv::Mat& image, cv::Mat& lbp_desc, const int* ncells, bool normalize, bool asrows)
{
    cv::Mat lbpMat, partialHist;
    cv::Size cellSize(image.rows / ncells[0], image.cols / ncells[1]);

    lbp_desc = *new cv::Mat();
    for (int j = 0; j < image.rows - cellSize.height; j += cellSize.height) {
        for (int i = 0; i < image.cols - cellSize.width; i += cellSize.width) {
            cv::Mat cell(image, cv::Rect(cv::Point(i, j), cellSize));
            fsiv_lbp(cell, lbpMat);
            fsiv_lbp_hist(lbpMat, partialHist, normalize);

            float sum = 0.0;
            for (int i = 0; i < partialHist.rows; i++) {
                sum += partialHist.at<float>(i, 0);
            }

            float expected = normalize ? 1.0 : cellSize.height * cellSize.width;
            assert(abs(expected - sum) < 0.001);

            if (lbp_desc.rows == 0)
                lbp_desc = partialHist.clone();
            else
                cv::vconcat(lbp_desc, partialHist, lbp_desc);
        }
    }
    if (asrows)
        lbp_desc = lbp_desc.t();
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
    assert(h1.rows == h2.rows);
    assert(h1.cols == 1);
    assert(h2.cols == 1);

    float err = 0.0;
    float xi, yi;

    for (int i = 0; i < h1.rows; i++) {
        xi = *h1.ptr<float>(i);
        yi = *h2.ptr<float>(i);
        if (abs(xi + yi) > 0.00001) {
            err += (pow(xi - yi, 2) / (xi + yi));
        }
    }
    err *= 0.5;
    return err;
}
