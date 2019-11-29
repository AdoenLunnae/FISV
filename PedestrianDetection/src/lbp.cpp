//! \file lbp.cpp
//! Utils for LBP

#include "lbp.hpp"
#include <cassert>
#include <vector>

using namespace std;

void makeUniform(cv::Mat& mat)
{
    int uniform[256] = {
        0, 1, 2, 3, 4, 58, 5, 6, 7, 58, 58, 58, 8, 58, 9, 10, 11, 58, 58, 58, 58, 58, 58, 58, 12, 58, 58, 58, 13, 58,
        14, 15, 16, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 17, 58, 58, 58, 58, 58, 58, 58, 18,
        58, 58, 58, 19, 58, 20, 21, 22, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
        58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 23, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
        58, 58, 24, 58, 58, 58, 58, 58, 58, 58, 25, 58, 58, 58, 26, 58, 27, 28, 29, 30, 58, 31, 58, 58, 58, 32, 58,
        58, 58, 58, 58, 58, 58, 33, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 34, 58, 58, 58, 58,
        58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
        58, 35, 36, 37, 58, 38, 58, 58, 58, 39, 58, 58, 58, 58, 58, 58, 58, 40, 58, 58, 58, 58, 58, 58, 58, 58, 58,
        58, 58, 58, 58, 58, 58, 41, 42, 43, 58, 44, 58, 58, 58, 45, 58, 58, 58, 58, 58, 58, 58, 46, 47, 48, 58, 49,
        58, 58, 58, 50, 51, 52, 58, 53, 54, 55, 56, 57
    };

    for (int i = 0; i < mat.rows; ++i) {
        uchar* ptr = mat.ptr<uchar>(i);
        for (int j = 0; j < mat.cols; ++j) {
            ptr[j] = uniform[ptr[j]];
        }
    }
}

template <class T>
vector<T> getNeighbours(const cv::Mat& img, const unsigned int& x, const unsigned int& y)
{
    vector<T> neigbours = {
        img.at<T>(x - 1, y - 1),
        img.at<T>(x - 1, y),
        img.at<T>(x - 1, y + 1),
        img.at<T>(x, y + 1),
        img.at<T>(x + 1, y + 1),
        img.at<T>(x + 1, y),
        img.at<T>(x + 1, y - 1),
        img.at<T>(x, y - 1),
    };
    return neigbours;
}

void fsiv_lbp(const cv::Mat& imagem, cv::Mat& lbp)
{
    lbp = *new cv::Mat(imagem.size(), CV_8UC1);
    std::vector<uchar> neighbours;
    uchar lbp_val, pixel;
    for (int i = 0; i < imagem.rows; ++i) {
        const uchar* imgPtr = imagem.ptr<uchar>(i);
        uchar* lbpPtr = lbp.ptr<uchar>(i);

        for (int j = 0; j < imagem.cols; ++j) {
            pixel = imgPtr[j];
            neighbours = getNeighbours<uchar>(imagem, i, j);

            lbp_val = 0;

            for (int k = 0; k < 8; ++k) {
                if (neighbours[k] >= pixel) {
                    lbp_val = lbp_val | (1 << 7 - k);
                }
            }

            lbpPtr[j] = lbp_val;
        }
    }
}

void fsiv_lbp_hist(const cv::Mat& lbp, cv::Mat& lbp_hist, bool normalize, int nbins)
{
    int size = nbins;
    std::vector<cv::Mat> images = { lbp };
    std::vector<int> channels = { 0 }, sizes = { size };
    std::vector<float> range = { 0, (float)nbins };

    cv::calcHist(images, channels, cv::Mat(), lbp_hist, sizes, range);

    if (normalize) {
        lbp_hist = lbp_hist.mul(1.0 / (lbp.cols * lbp.rows));
    }
}

//OPTIONAL
void fsiv_lbp_desc(const cv::Mat& image, cv::Mat& lbp_desc, const int* ncells, bool normalize, bool asrows, int nbins)
{
    cv::Mat lbpMat, partialHist;
    cv::Size cellSize(image.rows / ncells[0], image.cols / ncells[1]);

    lbp_desc = *new cv::Mat();

    for (int j = 0; j < image.rows - cellSize.height; j += cellSize.height) {
        for (int i = 0; i < image.cols - cellSize.width; i += cellSize.width) {
            cv::Mat cell(image, cv::Rect(cv::Point(i, j), cellSize));

            fsiv_lbp(cell, lbpMat);
            if (nbins == 59)
                makeUniform(lbpMat);

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
