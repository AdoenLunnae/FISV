#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;

using cv::Mat;
using cv::Point;
using cv::Rect;

Mat img;


void on_mouse(int event,int x,int y,int,void*){
	static Point firstCorner,secondCorner;
    static Rect box;
    static bool firstSelected=false, secondSelected=false;

    //Click handling
    if(event==CV_EVENT_LBUTTONDOWN){

        //First click (start the selection)
        if(!firstSelected){

            firstSelected=true;

            //Setting the first corner
            firstCorner.x=x;
            firstCorner.y=y;
            cout <<"Corner 1: "<<firstCorner<<endl;
        }

        //Second click (end the selection)
        else if(!secondSelected){
            if((abs(x-firstCorner.x) > 10) && (abs(y-firstCorner.y) > 10)){

                secondSelected=true;

                //Checking if the point is inside the image and fixing it to the border if it's outside
                if(x >= img.cols) x = img.cols-1;
                if(x < 0) x = 0;
                if(y >= img.rows) y = img.rows-1;
                if(y < 0) y = 0;

                //Setting the second corner
                secondCorner.x=x;
                secondCorner.y=y;
                cout<<"Corner 2: "<<secondCorner<<endl;
            }
            else{
                cout<<"Selection was too small"<<endl;
            }
        }
	}

    //Drawing the selection rectangle interactively
	if(firstSelected & !secondSelected){
		Point pt(x,y);
		Mat img_roi=img.clone();
		cv::rectangle(img_roi,firstCorner,pt,cv::Scalar(255,255,255), 2);
		cv::imshow("Original",img_roi);
	}

    //Creating the foreground higlighted image when both points have been selected
	if(firstSelected & secondSelected){
        //New image
        Mat edited = img.clone();

        //Getting the ROI rectangle
		box.width=abs(firstCorner.x-secondCorner.x);
		box.height=abs(firstCorner.y-secondCorner.y);

		box.x=min(firstCorner.x,secondCorner.x);
		box.y=min(firstCorner.y,secondCorner.y);

        //Editing the pixels outside the ROI
        for(int j=0;j<img.rows;j++){
	        for(int i=0;i<edited.cols;i++){
                uchar *ptr=edited.ptr<uchar>(j) + 3*i;
                if(!Point(i, j).inside(box)){
                    int avg = (/*B*/0.11*ptr[0] + /*G*/0.59*ptr[1] + /*R*/0.3*ptr[2]); //Weighted average formula for getting grayscale
                    ptr[0] = ptr[1] = ptr[2] = avg;
                    ptr += 3;
                }
            }
        }

        //Creating the new window and showing the highlighted image
		cv::namedWindow("Highlighted");
		cv::imshow("Highlighted",edited);
		firstSelected=false;
		secondSelected=false;
	}
}


int main(int argc, char** argv){
    try{
        //Checking for errors
        if(argc != 2) {cerr<<"Usage: "<<argv[0]<<" image"<<endl;return 1;}
        img=cv::imread(argv[1]);
        if( img.rows==0) {cerr<<"Error reading image"<<endl;return 1;}

        //Creating the window and displaying the image
        cv::namedWindow("Original");
        cv::imshow("Original",img);

        cv::setMouseCallback("Original",on_mouse);

        while(char(cv::waitKey(1)!=27));
    }
    catch(exception &ex){

    }
    return 0;
}
