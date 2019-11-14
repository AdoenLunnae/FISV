#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;

using cv::Mat;
using cv::Point;
using cv::Rect;

Mat img;

void on_mouse(int event, int x, int y, int flag, void*)
{
    static Mat mask(img.rows, img.cols, CV_8UC1), tmp;
    static vector<vector<Point>> polygons; //We need a vector of vectors to send to fillPoly
    static vector<Point> pts;
    static bool started;
    Mat edited = img.clone();

    //Left click handling to add points
    if (event == cv::EVENT_LBUTTONDOWN) {
        if (!started) {
            started = true;
            mask.setTo(0);
            pts.clear();
            polygons.clear();
        }
        pts.push_back(Point(x, y));
    }

    //Right click handling to delete points
    if (event == cv::EVENT_RBUTTONDOWN) {
        if (started && (pts.size() >= 1)) {
            pts.pop_back();
        }
    }

    //Double left click handling to finish
    if (event == cv::EVENT_LBUTTONDBLCLK) {
        if (started && (pts.size() >= 3)) {
            polygons.push_back(pts);
            cv::fillPoly(mask, polygons, cv::Scalar(255, 255, 255));
            started = false;
            for (int y = 0; y < edited.rows; ++y) {
                for (int x = 0; x < edited.cols; ++x) {
                    if (!mask.at<uchar>(Point(x, y))) {
                        uchar* ptr = edited.ptr<uchar>(y) + 3 * x;
                        int avg = (/*B*/ 0.11 * ptr[0] + /*G*/ 0.59 * ptr[1] + /*R*/ 0.3 * ptr[2]); //Weighted average formula for getting grayscale
                        ptr[0] = ptr[1] = ptr[2] = avg;
                    }
                }
            }
            cv::namedWindow("Edited");
            cv::imshow("Edited", edited);
        }
    }
    tmp = img.clone();
    if (started)
        pts.push_back(Point(x, y));
    cv::polylines(tmp, pts, !started, cv::Scalar(255, 255, 255), 2);
    cv::imshow("Original", tmp);
    if (started)
        pts.pop_back();
}

int main(int argc, char** argv)
{
    try {
        if (argc != 2) {
            cerr << "Usage: " << argv[0] << " image" << endl;
            return 1;
        }

        img = cv::imread(argv[1]);

        if (img.rows == 0) {
            cerr << "Error reading image" << endl;
            return 1;
        }

        cv::namedWindow("Original");
        cv::imshow("Original", img);

        cv::setMouseCallback("Original", on_mouse);

        while (char(cv::waitKey(1) != 27))
            ;
    } catch (exception& ex) {
    }
    return 0;
}