#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <vector>

using namespace cv;

std::vector<Point3f> createAxis(const float& length)
{
    std::vector<Point3f> axis;

    axis.emplace_back(0, 0, 0);
    axis.emplace_back(0, 0, -length);
    axis.emplace_back(0, length, 0);
    axis.emplace_back(length, 0, 0);

    return axis;
}

void drawAxis(Mat& dst, const std::vector<Point3f>& realAxis, const std::vector<Point2f>& projectedAxis, int& squareSize)
{
    for (auto i = 0; i < realAxis.size(); ++i) {
        circle(dst, projectedAxis[i], 4, Scalar(-realAxis[i].z * 255 / squareSize, realAxis[i].y * 255 / squareSize, realAxis[i].x * 255 / squareSize), -1);
        if (i != 0) {
            line(dst, projectedAxis[0], projectedAxis[i], Scalar(-realAxis[i].z * 255 / squareSize, realAxis[i].y * 255 / squareSize, realAxis[i].x * 255 / squareSize), 3);
        }
    }
}

int main(int argc, char** argv)
{
    //DEFINING THE PARSER
    const cv::String keys = "{help h usage ? |          | print this message   }"
                            "{rows           |6         | number of rows of the pattern.}"
                            "{cols           |8         | number of columns of the pattern.}"
                            "{intrinsics     |camera.yml| path to the file wit the intrinsincs of the camera}"
                            "{@input         |<none>    | path to a video file or camera index}"
                            "{i              |          | path to an image or video that will be displayed on top}";
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Augmented Reality Practice");

    if (parser.has("help")) {
        parser.printMessage();
        return (0);
    }

    //DECLARING VARIABLES AND GETTING DATA FROM THE PARSER

    bool overlap = parser.has("i"), overlapVideo;

    VideoCapture video, video2;
    Mat image, image2, copy, rvec, tvec;
    Mat distortionCoefficients(5, 1, CV_64F), cameraMatrix(3, 3, CV_64F);

    std::vector<Point3f> realCorners, axis;
    std::vector<Point2f> corners, points, projectedAxis;

    FileStorage fs;

    int width = parser.get<int>("cols"), height = parser.get<int>("rows");
    cv::String intrinsicsFile = parser.get<cv::String>("intrinsics"), inputFile = parser.get<String>("@input"), overlapFile = parser.get<String>("i");
    if (!parser.check()) {
        parser.printMessage();
        parser.printErrors();
        return (1);
    }
    if (overlap) {
        image2 = imread(overlapFile);
        if (overlapVideo = (image2.rows == 0)) {
            video2.open(overlapFile);
            if (!video2.isOpened()) {
                std::cerr << "Error opening overlap file\n";
                return 1;
            }
        }
    }

    fs.open(intrinsicsFile, cv::FileStorage::READ);

    int squareSize = fs["square_size"];
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distortionCoefficients;

    fs.release();

    Size patternSize(width, height);
    Mat perspectiveTransform;

    Point2f boardCorners[4];
    Point2f imageCorners[4];

    if (overlap) {
        if (overlapVideo) {
            imageCorners[0] = { 0, 0 };
            imageCorners[1] = { video2.get(CAP_PROP_FRAME_WIDTH), 0 };
            imageCorners[2] = { 0, video2.get(CAP_PROP_FRAME_HEIGHT) };
            imageCorners[3] = { video2.get(CAP_PROP_FRAME_WIDTH), video2.get(CAP_PROP_FRAME_HEIGHT) };
        } else {
            imageCorners[0] = { 0, 0 };
            imageCorners[1] = { image2.rows, 0 };
            imageCorners[2] = { 0, image2.cols };
            imageCorners[3] = { image2.rows, image2.cols };
        }
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            realCorners.emplace_back(squareSize * j, squareSize * i, 0);
        }
    }

    axis = createAxis(squareSize);

    namedWindow("image");

    if (inputFile.empty() || (isdigit(inputFile[0]) && inputFile.size() == 1)) {
        int camera = inputFile.empty() ? 0 : inputFile[0] - '0';
        if (!video.open(camera)) {
            std::cout << "Capture from camera #" << camera << " didn't work\n";
            return 1;
        }
    } else {
        image = imread(inputFile);
        if (image.empty()) {
            if (!video.open(inputFile)) {
                std::cout << "Could not read " << inputFile << '\n';
                return 1;
            }
        }
    }

    while (video.grab() && waitKey(50) != 27) {
        video.retrieve(image);
        copy = image.clone();
        bool found = findChessboardCorners(image, patternSize, corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);
        //drawChessboardCorners(copy, patternSize, corners, found);
        if (found) {
            Mat gray;
            cvtColor(image, gray, cv::COLOR_BGR2GRAY);
            cornerSubPix(gray, Mat(corners), Size(5, 5), Size(-1, -1), TermCriteria(3, 40, 0.001));

            if (overlap) {
                boardCorners[0] = corners[0],
                boardCorners[1] = corners[width - 1],
                boardCorners[2] = corners[corners.size() - width];
                boardCorners[3] = corners[corners.size() - 1];

                perspectiveTransform = getPerspectiveTransform(imageCorners, boardCorners);
                if (overlapVideo) {
                    if (video2.grab())
                        video2.retrieve(image2);
                    else
                        overlapVideo = false;
                }
                cv::warpPerspective(image2, copy, perspectiveTransform, copy.size(), 1, BORDER_TRANSPARENT);
            }

            solvePnP(realCorners, corners, cameraMatrix, distortionCoefficients, rvec, tvec);

            cv::projectPoints(axis, rvec, tvec, cameraMatrix, distortionCoefficients, projectedAxis);

            drawAxis(copy, axis, projectedAxis, squareSize);

            /// Show what you got
            cv::imshow("image", copy);
        } else
            cv::imshow("image", image);
    }
}
