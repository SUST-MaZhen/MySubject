#include "pointscan.h"
/************************************************************************/
/*  基于直线包含最大点数的作物行检测方法     2018.02.01   马振          */
/************************************************************************/


PointScan::PointScan(int len)
{
    this->m_length = len;
}

PointScan::~PointScan() {}

int PointScan::readImage(string path)
{
    Mat img = imread(path);
    if (!img.data)
    {
        return -1;
    }
    //this->m_row = img.rows;
    //this->m_col = img.cols;
    this->g_srcImage = img;
    return 0;
}
Mat PointScan::preProcess(Mat img)
{
    Mat img_copy = img;
    Mat dstImage = Mat::zeros(img_copy.size(), img_copy.type());
    cvtColor(dstImage, dstImage, CV_RGB2GRAY);
    for (int i = 0; i < this->m_row; i++)
    {
        for (int j = 0; j < this->m_col; j++)
        {
            if ((img_copy.at<Vec3b>(i, j)[1] < img_copy.at<Vec3b>(i, j)[0]) || (img_copy.at<Vec3b>(i, j)[1] < img_copy.at<Vec3b>(i, j)[2]))
                dstImage.at<uchar>(i, j) = 0;
            else
                //获取灰度图像某一点的像素值img.at<uchar>(nrows,ncols)
                dstImage.at<uchar>(i, j) = 2 * img_copy.at<Vec3b>(i, j)[1] - img_copy.at<Vec3b>(i, j)[0] - img_copy.at<Vec3b>(i, j)[2];
        }
    }
    return dstImage;
}

Mat PointScan::getBinImage(Mat img)
{
    Mat binImg = img;
    medianBlur(binImg, binImg, 5);
    /*Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(binImg, binImg, MORPH_CLOSE, element);*/
    return binImg;
}

void PointScan::bwareaopen(Mat& image, double size)
{
    image.convertTo(image, CV_8UC1);
    Mat labels, stats, centroids;
    int nLabels = connectedComponentsWithStats(image.clone(), labels, stats, centroids);
    int N = nLabels - 1;

    Mat temparea = stats.col(4);
    Mat idxx;
    findNonZero(temparea > size, idxx);
    Mat zerores = Mat::zeros(image.rows, image.cols, CV_8UC1);
    for (int j = 1; j < idxx.rows; j++)
    {
        bitwise_or(zerores, labels == idxx.at<int>(j, 1), zerores);
    }

    image = zerores;

    this->g_grayImage = image;
}

void PointScan::imfill(Mat srcimage, Mat &dstimage)
{
    Size m_Size = srcimage.size();
    Mat temimage = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcimage.type());//延展图像
                                                                                    //imshow("temimage", temimage);
    srcimage.copyTo(temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
    floodFill(temimage, Point(0, 0), Scalar(255));
    //imshow("temimage", temimage);
    //waitKey(0);
    Mat cutImg;//裁剪延展的图像
    temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
    dstimage = srcimage | (~cutImg);
}

void PointScan::findBottomPoints(Mat binIm)
{
    this->g_binImage = binIm;
    int x1, y1, x2, y2;
    x1 = 1;
    y1 = ((this->m_row / this->m_length) - 1)*this->m_length;
    x2 = this->m_col - 2;
    y2 = this->m_row - (((this->m_row / this->m_length) - 1)*this->m_length) - 1;
    Mat tiao = binIm(Rect(x1, y1, x2, y2));
    //imshow("tiao", tiao);
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 3));
    cv::dilate(tiao, tiao, element);
    //imshow("tiao1", tiao);
    findContours(tiao, this->contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (size_t j = 0; j < contours.size(); j++)
    {
        Rect r = boundingRect(contours[j]);
        if (r.area() >(this->m_length * 10))
        {
            int p_col = r.x + floor((0.5 * (r.width)));
            int p_row = this->m_row - 1;
            Point2d p = Point2d(p_col, p_row);
            this->points.push_back(p);
        }
    }
    /*for (auto iter = this->points.cbegin(); iter != this->points.cend(); iter++)
    {
    cout << (*iter) << endl;
    }*/
}

void PointScan::findCropLine(Mat &img)
{
    threshold(this->g_grayImage, this->g_binImage, 0.0, 1, CV_THRESH_BINARY);
    int sum[320];
    for (int i = 0; i < 2; i++)
    {

        memset(sum, 0, sizeof(sum));
        Point2d p_bottom = this->down[i];
        int index = 0, max = sum[0];
        if (p_bottom.x < 0.5*this->m_col)
        {
            for (int i = 1; i < 0.6*this->m_col; i = i + 1)
            {
                for (int j = 1; j < this->m_row - 1; j = j + 1)
                {
                    int x = floor(((p_bottom.x - i) / ((this->m_row - 1) - 1))*(j - 1)) + i;
                    if (((int)(this->g_binImage.at<uchar>(j, x)) == 1) && ((int)(this->g_binImage.at<uchar>(j, x - 1)) == 1)
                        && ((int)(this->g_binImage.at<uchar>(j, x + 1)) == 1))
                        sum[i]++;
                }
                //cout << sum[i] << endl;
                if (sum[i] > max)
                {
                    max = sum[i];
                    index = i;
                }
                if (max == this->m_row)
                    break;
            }

            //cout << max << endl;
            cv::line(img, p_bottom, Point2d(index, 1), Scalar(0, 0, 255), 2);
            this->up[0] = Point2d(index, 1);
        }
        else
        {
            for (int i = 0.3*this->m_col; i < p_bottom.x; i = i + 1)
            {
                for (int j = 1; j < this->m_row - 1; j = j + 1)
                {
                    int x = (((p_bottom.x - i) / ((this->m_row - 1) - 1))*(j - 1)) + i;
                    if (((int)(this->g_binImage.at<uchar>(j, x)) == 1) && ((int)(this->g_binImage.at<uchar>(j, x - 1)) == 1)
                        && ((int)(this->g_binImage.at<uchar>(j, x + 1)) == 1))
                        sum[i]++;
                }
                //cout << sum[i] << endl;
                if (sum[i] > max)
                {
                    max = sum[i];
                    index = i;
                }
                if (max == this->m_row)
                    break;
            }
            //cout << max << endl;
            cv::line(img, p_bottom, Point2d(index, 1), Scalar(0, 0, 255), 2);
            this->up[1] = Point2d(index,1);
        }
    }
    cv::line(img, (this->down[0]+this->down[1])/2, (this->up[0] + this->up[1]) / 2, Scalar(0, 255, 255), 1);
}


Mat PointScan::getRoiAndProcess(Mat& img, Rect r)
{
    Mat imgcopy = img(r);
    //2G-B-R灰度化处理
    imgcopy = preProcess(imgcopy);
    return imgcopy;
}

void PointScan::getWantedPoints(Point2d p)
{
    int num = 0;
    //将图像中点插入其中
    this->points.push_back(p);
    Point2d *p_ = new Point2d[this->points.size()];
    for (auto iter = this->points.cbegin(); iter != this->points.cend(); iter++)
    {
        //将容器中的点重新放在点数组里，为了排序
        p_[num].x = (*iter).x;
        p_[num].y = (*iter).y;
        num++;
        cout << (*iter) << endl;
    }
    //对所有点进行排序，选择离中心线最近的两个点
    for (int i = 0; i < num-1; i++)
    {
        for (int j = 0; j < num - i - 1; j++)
        {
            if (p_[j].x > p_[j + 1].x)
            {
                int temp_x = p_[j].x;
                int temp_y = p_[j].y;
                p_[j].x = p_[j + 1].x;
                p_[j].y = p_[j + 1].y;
                p_[j + 1].x = temp_x;
                p_[j + 1].y = temp_y;
            }
        }
    }
    for (int i = 0; i < num; i++)
    {
        //确定插入的点的index
        if (int(p_[i].x) == this->m_col/2)
        {
            this->indexOfCenter = i;
        }
    }
    cout << "size" << this->points.size() << endl;
    //清除之前的容器，下次要重新赋点
    this->points.clear();
    cout << "size" << this->points.size() << endl;
    this->down[0] = Point2d(p_[this->indexOfCenter - 1].x, p_[this->indexOfCenter - 1].y);
    this->down[1] = Point2d(p_[this->indexOfCenter + 1].x, p_[this->indexOfCenter + 1].y);
    cout << "size" << this->points.size() << endl;
    delete[] p_;
}

//通过相机标定来确定导航线上的具体的某一个点的世界坐标
int PointScan::getLastedPoint(int z)
{
    double y2 = (this->up[0].y + this->up[1].y) / 2;
    double y1 = (this->down[0].y + this->down[1].y) / 2;
    double x2 = (this->up[0].x + this->up[1].x) / 2;
    double x1 = (this->down[0].x + this->down[1].x) / 2;
    double k = (y2- y1) / (x2 - x1);
    double b = y2 - x2 * k;
    //double b1 = y1 - x1 * k;
    double x = (z-b)/k;
    cout << "b:" << b << "  x:"<<x<< endl;
    return x;
}



