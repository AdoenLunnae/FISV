//! \file test_lbp.cpp
//! \author FSIV-UCO

#include <cassert>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "lbp.hpp"

using namespace std;

const cv::String keys = "{help h usage ? |      | print this message.   }"
                        "{@image         |<none>| path to input image. }"
                        "{image2         |      | path to second image, for comparison. }"
                        "{show           |      | show the images. }"
                        "{u              |      | calculate uniform histogram. }";

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Test LBP implementation.");
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    setvbuf(stdout, NULL, _IONBF, 0);

    int nbins = 256;
    bool show = parser.has("show");
    /// Load the image
    cv::Mat image = cv::imread(parser.get<cv::String>("@image"), cv::IMREAD_GRAYSCALE);
    bool normalize = true;

    /// Compute LBP matrix
    cv::Mat lbpmat;
    fsiv_lbp(image, lbpmat);

    /// Display LBP image
    if (show)
        fsiv_lbp_disp(lbpmat, "LBP image");

    /// Save LBP image to disk
    cv::imwrite("lbp_mat.jpeg", lbpmat);

    if (parser.has("u")) {
        makeUniform(lbpmat);
        nbins = 59;
    }
    /// Compute the LBP histogram
    cv::Mat lbp_h1;
    fsiv_lbp_hist(lbpmat, lbp_h1, normalize, nbins);

    float sum = 0.0;
    ofstream f("lbp_hist.txt", ios::out);
    for (int i = 0; i < lbp_h1.rows; i++) {
        f << lbp_h1.at<float>(i, 0) << '\n';
        sum += lbp_h1.at<float>(i, 0);
    }
    f.close();

    float expected = normalize ? 1.0 : image.rows * image.cols;
    assert(abs(expected - sum) < 0.0001);

    cv::Mat lbp_desc;
    lbp_desc.convertTo(lbp_desc, CV_32FC1);
    int cells[] = { 16, 16 };
    fsiv_lbp_desc(image, lbp_desc, cells, normalize, false);

    f.open("lbp_desc.txt", ios::out);
    for (int i = 0; i < lbp_desc.rows; i++) {
        for (int j = 0; j < lbp_desc.cols; j++)
            f << lbp_desc.at<float>(i, j) << ' ';
        f << '\n';
    }
    f.close();

    /// Compute the Chi^2 distance between the input image and its mirror
    if (parser.has("image2")) {
        cout << fixed;
        cv::Mat lbp_h2, lbp2, image2 = cv::imread(parser.get<cv::String>("image2"), cv::IMREAD_GRAYSCALE);
        fsiv_lbp(image2, lbp2);
        if (parser.has("u"))
            makeUniform(lbp2);
        fsiv_lbp_hist(lbp2, lbp_h2, normalize, nbins);
        float dist = fsiv_chisquared_dist(lbp_h1, lbp_h2);

        // Show distance
        cout << dist << scientific;
    }

    if (show)
        std::cout << "End! " << std::endl;

    return 0;
}
