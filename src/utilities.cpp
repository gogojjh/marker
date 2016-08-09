#include "include/utilities.h"
#include <vector>
#include <iostream>
#include <math.h>
#define HsvType int

using namespace std;
using namespace cv;

Utilities::Utilities()
{

}

// extract the single channel
Mat Utilities::colorConversion(Mat img, HsvType imgtype)
{
    GaussianBlur(img, img, Size(5,5),0,0);
    if(imgtype==HSV||imgtype==HSV_H||imgtype==HSV_S||imgtype==HSV_V)
    {
        Mat img_hsv, img_h, img_s, img_v;
        cvtColor(img, img_hsv, CV_BGR2HSV);
        vector<Mat> hsv_channels;
        split(img_hsv, hsv_channels);
        img_h = hsv_channels[0];
        img_s = hsv_channels[1];
        img_v = hsv_channels[2];
        switch (imgtype)
        {
            case 0: return img_hsv;
            case 1: return img_h;
            case 2: return img_s;
            case 3: return img_v;
            default: break;
        }
    }

    if(imgtype==BGR||imgtype==BGR_B||imgtype==BGR_G||imgtype==BGR_R)
    {
        Mat b_img, g_img, r_img;
        vector<Mat> bgr_channels;
        split(img,bgr_channels);
        b_img=bgr_channels[0];
        g_img=bgr_channels[1];
        r_img=bgr_channels[2];
        switch(imgtype)
        {
            case 4: return img;
            case 5: return b_img;
            case 6: return g_img;
            case 7: return r_img;
            default: break;
       }
       return img;
    }
}

Mat Utilities::decectMarkerContour(Mat img)
{
    Mat img_h, img_threshold, morph_img;
    Mat img_canny(img.rows, img.cols,CV_8UC3,Scalar(0,0,0));
    Mat img_show(img.rows, img.cols,CV_8UC3,Scalar(0,0,0));
    img.copyTo(img_show);
    // extract the red
    img_h=colorConversion(img, BGR_R);
    threshold(img_h,img_threshold,60,255,1);

    Mat element = getStructuringElement(MORPH_RECT,Size(3,3));
    morphologyEx(img_threshold,img_threshold,MORPH_ERODE,element);
    //imshow("1",img_threshold);

    Canny(img_threshold,img_canny,30,90);
   // imshow("2",img_canny);

    //Mat element = getStructuringElement(MORPH_RECT,Size(5,5));
    //morphologyEx(img_threshold,morph_img,MORPH_CLOSE,element);

    vector<vector<Point> > contours;
    vector<Point> approxCurve;
    vector<Vec4i> hierarchy;
    bool longEdge = 1;
    float markerArea = 0;
    bool sign_bigger = false;
    bool sign_small = false;
    //Mat imgContour(img.rows, img.cols,CV_8UC1,Scalar(0));
    Point2f pointsIn[4]={Point2f(0,0)};

    //vector<Rect> box(contours.size());
    findContours(img_canny,contours,hierarchy,RETR_TREE,CHAIN_APPROX_NONE);
    approxCurve.resize(contours.size());

    for(unsigned int i=0; i<contours.size();i++)
    {
        float contour_area=contourArea(contours[i]);
        cout << contour_area << endl;
        if(contour_area>5000)
        {
            markerArea = contour_area;
            // duo bian xing bi jin
            approxPolyDP(contours[i], approxCurve, double(contours[i].size()) * 0.05, true);
            // the input contour is convex or not
            if (isContourConvex(Mat(approxCurve)))
            {
                sign_bigger = true;
                int min_x = INT_MAX;
                int min_index = 0;
                // zuo shang jiao
                for(int j=0; j<4; j++)
                {
                    //circle(img_show,Point(approxCurve[j].x,approxCurve[j].y),8,Scalar(0,0,255));
                    //cout << approxCurve[j] << endl;
                    if(sqrt(approxCurve[j].x*approxCurve[j].x+approxCurve[j].y*approxCurve[j].y) < min_x)
                    {
                        min_x = sqrt(approxCurve[j].x*approxCurve[j].x+approxCurve[j].y*approxCurve[j].y);
                        min_index = j;
                    }
                }

                // four corners
                if(min_index == 0 )
                {
                    for(int j=0;j<4 ;j++)
                    {
                        pointsIn[j] = approxCurve[j];
                        circle(img_show,Point(pointsIn[j].x,pointsIn[j].y),8,Scalar(0,0,255));
                    }
                }
                else
                {
                    // xun zhao si ge jiao dui ying de zuo biao
                    int array[] = {1, 2, 3, 0, 1, 2, 3};
                    size_t count=sizeof(array)/sizeof(int);
                    vector<int> tmp(array, array+count);
                    vector<int>::iterator tmp_itr = tmp.begin();
                    int k=0;
                    tmp_itr = find(tmp.begin(),tmp.end(),min_index);

                    for(;min_index!=0 && tmp_itr!=tmp.end() && k!=4;tmp_itr++)
                    {
                        pointsIn[k++] = approxCurve[*tmp_itr];
                        circle(img_show,Point(pointsIn[k-1].x,pointsIn[k-1].y),8,Scalar(0,0,255));
                    }
                }

                drawContours(img_show,contours,i,Scalar(0,255,0),2,8);
                //rectangle(src_frame, boundRect[i], Scalar(0,0,255), 2,8,0);
            }
        }
        //mini rectangle
        else if(sign_bigger = true && markerArea != 0 && 0.01*markerArea < contour_area && contour_area < 0.03*markerArea)
        {
            //approxPolyDP(contours[i], approxCurve, double(contours[i].size()) * 0.05, true);
            //approxPolyDP(contours[i], approxCurve, double(contours[i].size()) * 0.05, true);
            sign_small = true;
            //            int longEdge=0;
            //            if(longEdge == 1 && abs(sqrt(approxCurve[0].x*approxCurve[0].x+approxCurve[0].y*approxCurve[0].y)-sqrt(approxCurve[1].x*approxCurve[1].x+approxCurve[1].y*approxCurve[1].y)) < abs(sqrt(approxCurve[0].x*approxCurve[0].x+approxCurve[0].y*approxCurve[0].y)-sqrt(approxCurve[3].x*approxCurve[3].x+approxCurve[3].y*approxCurve[3].y)))
            //                longEdge = 0;
            //drawContours(img_show,contours,i,Scalar(0,255,0),2,8);
        }

        if(sign_bigger == true && sign_small == true)
        {
            Mat move_pos = getPosition(img_h, pointsIn);
            //cout << "*******************" << endl;
        }
        //imshow("img_show",img_show);
    }
    //imshow("imgContour", imgContour);
    return img_show;
}


Mat Utilities::getPosition(Mat &img, Point2f* pointsIn)
{
    Mat img_out(400,400,CV_8UC1,Scalar(0,0,0));
    Point2f pointsRes[4];
    pointsRes[0] = Point2f(0, 0);
    pointsRes[1] = Point2f(400 - 1, 0);
    pointsRes[2] = Point2f(400 - 1, 400 - 1);
    pointsRes[3] = Point2f(0, 400 - 1);

    // Size of the Marker, a standard marker
    vector<Point3f> objectPoints;
    objectPoints.push_back(Point3f(0,0,0));
    objectPoints.push_back(Point3f(410,0,0));
    objectPoints.push_back(Point3f(410,410,0));
    objectPoints.push_back(Point3f(0,410,0));

    vector<Point2f> imagePoints;
    for(int i=0;i<4;i++)
    {
        imagePoints.push_back(Point2f(pointsIn[i].x,pointsIn[i].y));
    }

    float tmp1[3][3] = {{1760.80462, 0, 929.95610}, {0, 1760.28449, 535.82561}, {0, 0, 1}};
    Mat cameMatrix(3,3,CV_32FC1, tmp1);

    float tmp2[5] = {-0.41624, 0.23446, -0.00058, 0.00086, 0};
    Mat distCoeffs(1,5,CV_32F,tmp2);

    Mat rotat_vec(3,3,CV_32F), trans_vec(3,3,CV_32F);

    //how to  use this information?????
    solvePnP(objectPoints, imagePoints, cameMatrix, distCoeffs, rotat_vec, trans_vec);

    // get the transformation matrix
    Mat transformMatrix = getPerspectiveTransform(pointsIn, pointsRes);
    //cout << transformMatrix << endl;

    // Applies a perspective transformation to an image.
    warpPerspective(img, img_out, transformMatrix, Size(400,400), cv::INTER_NEAREST);
    cout<<rotat_vec.col(0).row(0)<<endl;
    cout<<rotat_vec.at<double>(0,0)<<endl;
    //  cout<<"rotat_vec:"<<rotat_vec<<endl;
    //  cout<<"trans_vec:"<<trans_vec<<endl;
    cout<<"*****************************"<<endl;
    //namedWindow("img_out");
    //imshow("img_out",img_out);
    return rotat_vec;
}

/*
Focal Length:          fc = [ 1760.80462   1760.28449 ] +/- [ 5.65463   5.71223 ]
Principal point:       cc = [ 926.95610   535.82561 ] +/- [ 6.68057   5.33905 ]
Skew:             alpha_c = [ 0.00000 ] +/- [ 0.00000  ]   => angle of pixel axes = 90.00000 +/- 0.00000 degrees
Distortion:            kc = [ -0.41624   0.23446   -0.00058   0.00086  0.00000 ] +/- [ 0.00756   0.03623   0.00048   0.00093  0.00000 ]
Pixel error:          err = [ 0.24291   0.15701 ]

*/
