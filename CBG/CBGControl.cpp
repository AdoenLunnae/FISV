#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "CmdLineParser.hpp"
#include <iostream>

using std::cout;
using std::stof;
using namespace cv;




int main(int argc, char** argv){
    float contrast = 1.0, gamma = 1.0, brightness = 0.0;
    bool interactive = false;

    Mat img, normalized, edited;
    CmdLineParser commands(argc, argv);
    if(argc < 3){
        cout << "Usage: " << argv[0] <<" [options] <source> <output>\n";
        exit(1);
    }
    if((commands["-c"]) && (stof(commands("-c")) >= 0) && (stof(commands("-c")) <= 2)) contrast = stof(commands("-c"));
    if((commands["-g"]) && (stof(commands("-g")) >= 0) && (stof(commands("-c")) <= 2)) gamma = stof(commands("-g"));
    if((commands["-b"]) && (stof(commands("-b")) >= -1) && (stof(commands("-c")) <= 1)) brightness = stof(commands("-b"));
    if(commands["-i"]) interactive = true;

    img = imread(argv[argc-2]);
    namedWindow("Processed");
    if(img.rows == 0){
        cout << "Error reading image " << argv[argc-2] << '\n';
        exit(1);
    }

    imshow("Processed", img);

    if(interactive){
        int *c = new int, *g = new int, *b = new int;
        *c = (int)(100*contrast);
        *g = 100*gamma;
        *b = 100*brightness;
        createTrackbar("Contrast", "Processed", c, 200, NULL, 0);
        createTrackbar("Gamma", "Processed", g, 200, NULL, 0);
        createTrackbar("Brightness", "Processed", b, 200, NULL, 0);
    }
    while(waitKey(0) != 27);
    return 0;
}