#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace std;

using cv::Mat;
using cv::Point;
using cv::Rect;

Mat img;

 
void on_mouse(int event,int x,int y,int,void*)
{
	static Point cor1,cor2;
    static Rect box;
    static bool firstPoint=false, secondPoint=false;

    //Click handling
    if(event==CV_EVENT_LBUTTONDOWN){
        //First click (start the selection)
        if(!firstPoint){
            firstPoint=true;
            cor1.x=x;
            cor1.y=y;
            cout <<"Corner 1: "<<cor1<<endl;
        }
        //Second click (end the selection)
        else if(!secondPoint){
            if((abs(x-cor1.x) > 10) && (abs(y-cor1.y) > 10)){
                secondPoint=true;
                //Checking if the point is inside the image and fixing it if it's outside
                if(x >= img.cols) x = img.cols-1;
                if(x < 0) x = 0;
                if(y >= img.rows) y = img.rows-1;
                if(y < 0) y = 0;

                cor2.x=x;
                cor2.y=y;
                cout<<"Corner 2: "<<cor2<<endl;
            }
            else cout<<"Selection was too small"<<endl;
        }
	}
 
    //Drawing the selection rectangle interactively
	if(firstPoint & !secondPoint){
		Point pt(x,y);
		Mat img_roi=img.clone();
		cv::rectangle(img_roi,cor1,pt,cv::Scalar(255,255,255), 2);
		cv::imshow("Original",img_roi);
	 
	}

    //Creating the higlighted image
	if(firstPoint & secondPoint){
        //New image
        Mat edited = img.clone();

        //Getting the ROI rectangle
		box.width=abs(cor1.x-cor2.x);
		box.height=abs(cor1.y-cor2.y);
		box.x=min(cor1.x,cor2.x);
		box.y=min(cor1.y,cor2.y);

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
		cv::namedWindow("Highlighted");
		cv::imshow("Highlighted",edited);
		firstPoint=false;
		secondPoint=false;
	 
	}
 
}
 
int main(int argc, char** argv){
    try{
        if(argc != 2) {cerr<<"Usage: "<<argv[0]<<" image"<<endl;return 1;} 
        img=cv::imread(argv[1]);
        if( img.rows==0) {cerr<<"Error reading image"<<endl;return 1;}
        cv::namedWindow("Original");
        cv::imshow("Original",img);
        
        cv::setMouseCallback("Original",on_mouse); 
        
        while(char(cv::waitKey(1)!=27));
    }
    catch(exception &ex){
        
    }
    return 0;
}