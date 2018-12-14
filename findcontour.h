#ifndef FindContour_H
#define FindContour_H
#define PI 3.1415
#include <ctime>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <random>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
class FindContour
{
public:
    vector<vector<cv::Point>> contours;
    Mat g_srcImage;
    Mat g_dstImage;  //全局变量
    Mat g_tiaoImage;
    uchar white_len = 10; //白条的宽度
    int x[12];      //定位点横坐标
    int y[12];      //定位点纵坐标
    double a,b;     //定义斜率和截距
    int M;
    FindContour();
    int  grayShift(string);
    void hough_line(int *x,int *y);
    void getTiao(Mat);
    void leastSquare(int *x,int *y);
    void getLeastSquareDist();
    void drawLine(cv::Mat &image, double theta, double rho, cv::Scalar color);
    int  RadonTrans();//Radon变换求最大的列
    void getPoint(int);
    void getPointByVP(int);
private:

};

#endif // FindContour_H
