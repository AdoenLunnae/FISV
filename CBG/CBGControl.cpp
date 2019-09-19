#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "CmdLineParser.hpp"
#include <iostream>

using std::cout;
using namespace cv;

Mat img;



int main(int argc, char** argv){
    float contrast = 1.0, gamma = 1.0, brightness = 0.0;
    bool interactive = false;
    CmdLineParser commands(argc, argv);
    if((commands["-c"]) && (std::stof(commands("-c")) >= 0)) contrast = std::stof(commands("-c"));
    if((commands["-g"]) && (std::stof(commands("-g")) >= 0)) gamma = std::stof(commands("-g"));
    if((commands["-b"]) && (std::stof(commands("-b")) >= 0)) brightness = std::stof(commands("-b"));
    if(commands["-i"]) interactive = true;

    img = imread(argv[argc-2]);
    namedWindow("Processed");
    if(img.rows = 0){
        cout << "Error reading image " << argv[argc-2] << '\n';
        exit(1);
    }
    if(interactive){
        int *c, *g, *b;
        *c = 100*contrast;
        *g = 100*gamma;
        *b = 100*brightness;
        createTrackbar("Contrast", "Processed", c, 200, NULL, 0);
        createTrackbar("Gamma", "Processed", g, 200, NULL, 0);
        createTrackbar("Brightness", "Processed", b, 200, NULL, 0);
    }

    return 0;
}