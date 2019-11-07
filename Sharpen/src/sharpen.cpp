#include "convolve.hpp"
#include "laplacian.hpp"
#include "diffGaussians.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "CmdLineParser.hpp"
#include <iostream>
#include <string>

using namespace cv;
using namespace std;


void updateFloat(int value, void* userdata){
    float* ptr = (float*) userdata;
    *ptr = value/100.0;
}

int main(int argc, char** argv){
    if(argc < 3){
        cerr << "Usage: " << argv[0] << "[options] <input image> <output image>\n";
        exit(1);
    }
    
    CmdLineParser commands(argc, argv);
    
    int lapType = stoi(commands("-f", "1")), oldType;
    float lapGain = stof(commands("-g", "0.0")), oldGain;
    float sigma1 = stof(commands("-s", "1"));
    float sigma2 = stof(commands("-S", "2"));
    
    bool interactive = commands["-i"];
    int c;
    Mat img, output, laplacian, outputDog;
    Mat dog = createDoGFilter(sigma1, sigma2);
    namedWindow("OG");
    namedWindow("Output");
    namedWindow("DoG");

    
    img = imread(argv[argc - 2], IMREAD_GRAYSCALE);

    if(img.rows == 0){
        cerr << "Error while opening image\n";
        exit(1);
    }
    int g, s1, s2;
    g = floor(100*lapGain);
    //s1 = floor(100 * sigma1);
    //s2 = floor(100 * sigma2);
    imshow("OG", img);

    img.convertTo(img, CV_32F);

    if(interactive){
        createTrackbar("Laplacian Type", "Output", &lapType, 1);
        createTrackbar("Laplacian Gain", "Output", &g, 200, updateFloat, (void*) &lapGain);
        //createTrackbar("Sigma 1", "Output", &s1, 1000, updateFloat, (void*) &sigma1);
        //createTrackbar("Sigma 2", "Output", &s2, 1000, updateFloat, (void*) &sigma2);
    }

    laplacian = createSharpFilter(lapType, lapGain);
    
    img.convertTo(img, CV_32F);

    
    
    convolve(img, dog, outputDog);
    
    output.convertTo(output, CV_8UC1);
    
    convolve(img, laplacian, output);

    outputDog.convertTo(outputDog, CV_8UC1);
    //normalize(output, output, 0, 255, NORM_MINMAX);
    

    imshow("Output", output);
    
    while(c = waitKey(100)){
        if(c == 27){
            return 0;
        }
        if(c == 10){
            try{
                imwrite(argv[argc -1], output);
                cout << "Saved current output to " << argv[argc - 1] << "\n";
            }catch(Exception e){
                if(e.code == -2) cerr << "Unsupported extension. Can't save the image\n";
            }
        }
        if(interactive){
            if(lapType != oldType || lapGain != oldGain){
                laplacian = createSharpFilter(lapType, lapGain);
                
                convolve(img, laplacian, output);
                //normalize(output, output, 0, 255, NORM_MINMAX);
                output.convertTo(output, CV_8UC1);
                imshow("DoG", outputDog);
                imshow("Output", output);
                oldType = lapType;
                oldGain = lapGain;
            }
        }
    }

}