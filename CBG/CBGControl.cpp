#include "CmdLineParser.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>
#include <iostream>

using std::cout;
using std::stof;
using namespace cv;

void updateContrast(int pos, void* userdata)
{
    float* ptr = (float*)userdata;
    *ptr = pos / 100.0;
};

void updateGamma(int pos, void* userdata)
{
    float* ptr = (float*)userdata;
    *ptr = pos / 100.0;
};

void updateBrightness(int pos, void* userdata)
{
    float* ptr = (float*)userdata;
    *ptr = (pos / 100.0) - 1;
};

void applyVec(const Mat& img, Mat& img2, const float c, const float g, const float b, const bool luma)
{
    int nChannels = img.channels();
    img2 = img.clone();
    Mat channels[nChannels];
    float* ptr[nChannels];
    if ((nChannels == 1) || (!luma)) {
        split(img2, channels);
        for (int i = 0; i < nChannels; ++i) {
            ptr[i] = channels[i].ptr<float>(0);
        }
        for (int i = 0; i < nChannels; ++i) {
            cv::pow(channels[i], g, channels[i]);
            multiply(channels[i], Scalar(c), channels[i]);
            add(channels[i], Scalar(b), channels[i]);
        }
        merge(channels, nChannels, img2);
    } else {
        cvtColor(img2, img2, 41);
        split(img2, channels);
        cv::pow(channels[2], g, channels[2]);
        multiply(channels[2], Scalar(c), channels[2]);
        add(channels[2], Scalar(b), channels[2]);
        merge(channels, nChannels, img2);
        cvtColor(img2, img2, 55);
    }
}

void apply(const Mat& img, Mat& img2, const float c, const float g, const float b, const bool luma)
{
    int nChannels = img.channels();
    img2 = img.clone();
    Mat channels[nChannels];
    float* ptr[nChannels];
    if ((nChannels == 1) || (!luma)) {
        split(img2, channels);
        for (int i = 0; i < nChannels; ++i) {
            ptr[i] = channels[i].ptr<float>(0);
        }
        for (int offset = 0; offset < img2.rows * img2.cols; ++offset) {
            for (int i = 0; i < nChannels; ++i) {
                *(ptr[i] + offset) = c * pow(*(ptr[i] + offset), g) + b;
            }
        }
        merge(channels, nChannels, img2);
    } else {
        cvtColor(img2, img2, 41);
        split(img2, channels);
        for (int i = 0; i < nChannels; ++i) {
            ptr[i] = channels[i].ptr<float>(0);
        }
        for (int offset = 0; offset < img2.rows * img2.cols; ++offset) {
            *(ptr[2] + offset) = c * pow(*(ptr[2] + offset), g) + b;
        }
        merge(channels, nChannels, img2);
        cvtColor(img2, img2, 55);
    }
}

int main(int argc, char** argv)
{
    float contrast = 1.0, gamma = 1.0, brightness = 0.0;

    bool interactive = false, luma = false, vectorial = false;

    Mat img, normalized, normEdited, edited;
    CmdLineParser commands(argc, argv);
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " [options] <source> <output>\n";
        exit(1);
    }
    if ((commands["-c"]) && (stof(commands("-c")) >= 0) && (stof(commands("-c")) <= 2))
        contrast = stof(commands("-c"));
    if ((commands["-g"]) && (stof(commands("-g")) >= 0) && (stof(commands("-c")) <= 2))
        gamma = stof(commands("-g"));
    if ((commands["-b"]) && (stof(commands("-b")) >= -1) && (stof(commands("-c")) <= 1))
        brightness = stof(commands("-b"));
    if (commands["-i"])
        interactive = true;
    if (commands["-luma"])
        luma = true;
    if (commands["-vec"])
        vectorial = true;

    float oldContrast = contrast, oldGamma = gamma, oldBrightness = brightness;
    img = imread(argv[argc - 2]);
    namedWindow("Processed");
    if (img.rows == 0) {
        cout << "Error reading image " << argv[argc - 2] << '\n';
        exit(1);
    }
    imshow("Processed", img);
    if (interactive) {
        int *c = new int, *g = new int, *b = new int;
        *c = (int)(100 * contrast);
        *g = (int)(100 * gamma);
        *b = (int)(100 * (brightness + 1));
        createTrackbar("Contrast", "Processed", c, 200, updateContrast, (void*)&contrast);
        createTrackbar("Gamma", "Processed", g, 200, updateGamma, (void*)&gamma);
        createTrackbar("Brightness", "Processed", b, 200, updateBrightness, (void*)&brightness);
    }

    img.convertTo(normalized, CV_32F, 1.0 / 255.0);
    vectorial ? applyVec(normalized, normEdited, contrast, gamma, brightness, luma) : apply(normalized, normEdited, contrast, gamma, brightness, luma);
    normEdited.convertTo(edited, img.type(), 255);
    imshow("Processed", edited);
    if (interactive) {
        while (waitKey(50) != 27) {
            if ((oldContrast != contrast) || (oldGamma != gamma) || (oldBrightness != brightness)) {
                vectorial ? applyVec(normalized, normEdited, contrast, gamma, brightness, luma)
                          : apply(normalized, normEdited, contrast, gamma, brightness, luma);
                normEdited.convertTo(edited, img.type(), 255);
                oldContrast = contrast, oldGamma = gamma, oldBrightness = brightness;
            }
            imshow("Processed", edited);
        }
    }

    imwrite(argv[argc - 1], edited);
    return 0;
}