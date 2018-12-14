#include "findcontour.h"
#include <QDebug>
FindContour::FindContour()
{

}

int FindContour::grayShift(string path)
{
    Mat srcImage;
    srcImage = imread(path);
    if (!srcImage.data || (srcImage.cols!=320)|| (srcImage.rows!=240))
        return 0;
    g_srcImage = srcImage;
    qDebug()<< "cols= " <<srcImage.cols <<endl;
    g_dstImage = Mat::zeros(srcImage.size(), srcImage.type());
    cvtColor(g_dstImage, g_dstImage, CV_RGB2GRAY);
    for (int i=0;i<srcImage.rows;i++)
        for (int j = 0; j < srcImage.cols; j++)
        {

            if ((srcImage.at<Vec3b>(i, j)[1] < srcImage.at<Vec3b>(i, j)[0]) || (srcImage.at<Vec3b>(i, j)[1] < srcImage.at<Vec3b>(i, j)[2]))
                g_dstImage.at<uchar>(i, j) = 0;
            else
                //获取灰度图像某一点的像素值img.at<uchar>(nrows,ncols)
                g_dstImage.at<uchar>(i, j) = 2 * srcImage.at<Vec3b>(i, j)[1] - srcImage.at<Vec3b>(i, j)[0] - srcImage.at<Vec3b>(i, j)[2];
        }

    medianBlur(g_dstImage, g_dstImage, 7);

    Mat element2 = getStructuringElement(MORPH_RECT, Size(7, 7));
    erode(g_dstImage, g_dstImage, element2);

    Mat element1 = getStructuringElement(MORPH_RECT, Size(5,5));
    dilate(g_dstImage, g_dstImage, element1);

    return 1;
}

void FindContour::getPoint(int n)
{
    int MM = n;
    int index_middle[12];  //导航定位点
    Mat tiao[12];
    memset(index_middle, MM, sizeof(index_middle));
    for (int i = 0; i<12; i++)
    {
        tiao[i] = g_tiaoImage(Rect(1, 10 * (2 * i + 1) + 1, g_tiaoImage.cols - 2, 8));
        findContours(tiao[i], contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );

        int cnt = 0;

        int sum_ = 0, sum_x = 0, sum_y = 0, sum_width = 0, sum_height = 0;
        Rect rect;
        for (size_t j = 0; j < contours.size(); j++)
        {
            Rect r = boundingRect(contours[j]);
            if ((r.width > 2) && (r.height>2))
            {
                int position = r.x + (int)(0.5 * (r.width));
                if (fabs(position - MM) < 20)
                {
                    cnt++;
                    sum_x += r.x;
                    sum_y += r.y;
                    sum_width += r.width;
                    sum_height += r.height;
                    sum_ += position;
                    index_middle[i] = sum_ / cnt;
                    rect.x = sum_x / cnt;
                    rect.y = sum_y / cnt;
                    rect.width = sum_width / cnt;
                    rect.height = sum_height / cnt;
                }
            }
        }
        if (0 == cnt)
        {
            if (i > 0)
            {
                index_middle[i] = index_middle[i - 1];
            }
            else if (i > 1)
            {
                //index_middle[i] = (index_middle[i - 1] + index_middle[i - 2]) / 2;
                index_middle[i] = ((((20 * i + 15) - (20 * (i - 1) + 15))*(index_middle[i - 2] - index_middle[i - 1])) / ((20 * (i - 2) + 15) - (20 * (i - 1) + 15))) + index_middle[i - 1];
            }
            else
                index_middle[i] = MM;
            rectangle(tiao[i], Rect(index_middle[i]-7,rect.y,15,8), Scalar(255));
        }
        else
        {
            rectangle(tiao[i], rect, Scalar(255));
        }

    }
    /*描出点*/
    for (int i = 0; i<12; i++)
    {
        x[i] = index_middle[i];
        y[i] = (int)(0.5*(20 * i + 10 + 20 * (i + 1)));

        g_tiaoImage.at<uchar>((int)(0.5*(20 * i + 10 + 20 * i + 19)), index_middle[i]) = 255;  // 在途中标出点
        g_tiaoImage.at<uchar>((int)(0.5*(20 * i + 10 + 20 * i + 19) - 1), index_middle[i]) = 255;
        g_tiaoImage.at<uchar>((int)(0.5*(20 * i + 10 + 20 * i + 19) + 1), index_middle[i]) = 255;
        g_tiaoImage.at<uchar>((int)(0.5*(20 * i + 10 + 20 * i + 19)), index_middle[i] + 1) = 255;
        g_tiaoImage.at<uchar>((int)(0.5*(20 * i + 10 + 20 * i + 19)), index_middle[i] - 1) = 255;
    }
}
/*计算垂直投影值*/
int FindContour::RadonTrans()
{

    int s[320];
    memset(s, 0, sizeof(s));

    for (int j=0;j<g_dstImage.cols;j++)
        for (int i=0;i<g_dstImage.rows;i++)
            s[j] += g_dstImage.at<uchar>(i,j);

    //找最大值
    int res=0,maxnum=0;
    for (int i=0;i<320;i++)
        if (s[i]>maxnum)
        {
            maxnum = s[i];//当前最大值成为最大值
            M = i;   //最大值所在的下标
        }
    res = maxnum;
    qDebug()<<"Max cloume:"<<M;
    return res;
}

/*返回条形分割图*/
void FindContour::getTiao(Mat img)
{
    g_tiaoImage = img;
    for (int i=0;i<12;i++)
        for (int j=20*i;j<20*i+white_len;j++)
            for (int k=0;k<320;k++)
                g_tiaoImage.at<uchar>(j,k) = 255;    //将这几行像素值置为255，即白色，以达到条形分割的目的
}


void FindContour::getLeastSquareDist()
{
    double dis=0;
    for (int i = 0; i < 12; i++)
        dis += ((a * x[i] - y[i] + b)*(a * x[i] - y[i] + b)) / (a*a + 1);
    qDebug()<<"the list square sum is:"<<dis;
}

void FindContour::drawLine(cv::Mat &image, double theta, double rho, cv::Scalar color)
{
    if (theta < PI / 4. || theta > 3.*PI / 4.)// ~vertical line
    {
        cv::Point pt1(rho / cos(theta), 0);
        cv::Point pt2((rho - image.rows * sin(theta)) / cos(theta), image.rows);
        cv::line(image, pt1, pt2, cv::Scalar(255), 2);
    }
    else
    {
        cv::Point pt1(0, rho / sin(theta));
        cv::Point pt2(image.cols, (rho - image.cols * cos(theta)) / sin(theta));
        cv::line(image, pt1, pt2, color, 2);
    }
}

void FindContour::leastSquare(int *x, int *y)
{
    double t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    for (int i = 0; i < 12; ++i)
    {
        t1 += x[i] * x[i];
        t2 += x[i];
        t3 += x[i] * y[i];
        t4 += y[i];
    }
    a = (t3 * 12 - t2*t4) / (t1 * 12 - t2*t2);
    b = (t1*t4 - t2*t3) / (t1 * 12 - t2*t2);
    qDebug()<< "xielv:" << a<<"jieju"<< b;
    /*画出直线在图中*/
    line(g_srcImage, Point((-b) / a, 0), Point(((240 - b) / a), 240), Scalar(0, 0, 0), 2);
}
//hough变换拟合直线
void FindContour::hough_line(int *x,int *y)
{
    Mat hou = Mat::zeros(g_srcImage.size(), CV_8UC1);
    for (int i = 0; i < 12; i++)
    {
        hou.at<uchar>(y[i], x[i]) = 255;
    }
    vector<Vec2f> lines;
    HoughLines(hou, lines, 2, CV_PI / 180, 6,0, 0);
    for (size_t i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));
        //此句代码的OpenCV2版为:
        line(g_srcImage, pt1, pt2, Scalar(0, 0, 0), 2, CV_AA);
    }
}

void FindContour::getPointByVP(int n)
{
    int MM = n;
    int index_middle[12],index_up=0,index_low=0;  //上升点，下降点以及中点的索引
    long sum[12],sum_j[12][320];
    long m[12];
    memset(sum, 0, sizeof(sum));
    memset(sum_j, 0, sizeof(sum_j));
    memset(m, 0, sizeof(m));
    memset(index_middle, 0, sizeof(index_middle));
    for (int i=0;i<12;i++)
    {
        for (int k=0;k<320;k++)
        {
            for (int j=(20*i+10);j<20*(i+1);j++)
            {
                sum_j[i][k] += g_tiaoImage.at<uchar>(j,k); //每个水平条第j列的垂直投影值
            }
            sum[i] += sum_j[i][k];  //每个水平条的所有像素值之和
        }
        m[i] = sum[i]/320;   //每个水平条每列的平均值
    }
    /*计算上升点和下降点*/

    //计算定位点步骤一
    for (int line_num=0;line_num<12;line_num++)
    {
        for (int i=1;i<320;i++)
        {
            if (sum_j[line_num][MM]>m[line_num])
            {
                for (int j=MM;j>0;j--)
                {
                    if (sum_j[line_num][j-1]<m[line_num]<=sum_j[line_num][j])  //上升点
                    {
                        index_up = j;
                        break;
                    }
                }
                for (int k=MM;k<320;k++)
                {
                    if (sum_j[line_num][k-1]>m[line_num]>=sum_j[line_num][k])  //下降点
                    {
                        index_low = k;
                        break;
                    }
                }
                /************************************************************************/
                /*                                                                      */
                /************************************************************************/
                index_middle[line_num] = 0.5*(index_low + index_up);
                break;
            }
            else
            {
                index_middle[line_num] = MM;
                break;
            }
        }
    }

    /*描出点*/
    for (int i=0;i<12;i++)
    {
        for (int j=20*i+10;j<20*(i+1);j++)
        {
            x[i] = index_middle[i];
            y[i] = (int)(0.5*(20*i+10+20*(i+1)));
            g_tiaoImage.at<uchar>((int)(0.5*(20*i+10+20*(i+1))),index_middle[i]) = 255;  // 在途中标出点
            g_tiaoImage.at<uchar>((int)(0.5*(20*i+10+20*(i+1))-1),index_middle[i]) = 255;
            g_tiaoImage.at<uchar>((int)(0.5*(20*i+10+20*(i+1))+1),index_middle[i]) = 255;
            g_tiaoImage.at<uchar>((int)(0.5*(20*i+10+20*(i+1))),index_middle[i]+1) = 255;
            g_tiaoImage.at<uchar>((int)(0.5*(20*i+10+20*(i+1))),index_middle[i]-1) = 255;
        }
    }
}
