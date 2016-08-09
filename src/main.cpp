#include <opencv2/opencv.hpp>
#include "include/utilities.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

int main()
{
    VideoCapture capture(0);
    Mat src_frame;
    //src_frame = imread("/home/gogojjh/QT/marker/data/1.jpg");
    //resize(src_frame, src_frame, Size(src_frame.cols/3, src_frame.rows/3));

    if (!capture.isOpened())
    {
        cout << "Can't open video"<<endl;
        return 0;
    }

    while (1)
    {
        double time0=static_cast<double>(getTickCount());
        capture >> src_frame;
        //Mat img_show(src_frame.size());
        if(!src_frame.data) break;
        Utilities test;
        //img_h=test.colorConversion(src_frame,3);
        Mat marker=test.decectMarkerContour(src_frame);
        time0 = ((double)getTickCount()-time0)/getTickFrequency();
        //cout << 1000*time0 << endl;
        imshow("test", marker);
        imshow("show", src_frame);
        waitKey(50);
        //if(waitKey(1) > 0)  waitKey(0);
        //adaptiveThreshold(hsv_img_h,threshold_img,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,3,1);

    }
    return 0;
}
