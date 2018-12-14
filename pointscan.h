#pragma once
#include "cameracalibration.h"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
class PointScan
{
public:
    PointScan(int);
    ~PointScan();
    int readImage(string);
    Mat preProcess(Mat);
    Mat getBinImage(Mat);
    void bwareaopen(Mat&, double);
    void imfill(Mat, Mat&);
    void findBottomPoints(Mat);
    void findCropLine(Mat&);
    Mat getRoiAndProcess(Mat& img, Rect r);
    void getWantedPoints(Point2d p);
    int getLastedPoint(int y);

public:
    Mat g_srcImage;
    Mat g_binImage;
    Mat g_grayImage;
    Mat g_tiao;
    int m_row;
    int m_col;
    int m_length;
    vector<Point2d> points;
    vector<vector<cv::Point>> contours;
    int indexOfCenter;  //插入的图像中点在点数组中的
    Point2d up[2], down[2]; //定义直线上顶点
};

