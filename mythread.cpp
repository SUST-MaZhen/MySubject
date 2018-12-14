#include "mythread.h"
#include "cameracalibration.h"
#include <QDebug>
#include <vector>
#include <opencv2/opencv.hpp>
MyThread::MyThread(QObject *parent) :
    QThread(parent)
{
    stopped = false;
}
void MyThread::run()
{
    qDebug()<<"in thread "<<QThread::currentThreadId();//测试当前线程归属
    if(!cap.isOpened())
    {
        cap.open(0);//读取摄像头
        //cap.open("D:/masterstu/imageData/cropVideo/cropvideo.avi");//读取视频文件
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    while (!stopped)
    {
        //double fps=cap.get(CV_CAP_PROP_FPS); // 获取
        cap >> srcImage;
        if(!srcImage.data)
        {
            qDebug()<<"视频读取完毕";
            break;
            //continue;
        }
        //TODO YOUR CODE
        //实时处理农田图像，完成之后再进行信号发射
        //选择感兴趣区域
        Rect r = Rect(0, 0, 320, 240);
        PointScan pointscan(20);
        pointscan.m_col = r.width;
        pointscan.m_row = r.height;
        //预处理图像
        pointscan.g_srcImage = srcImage(r);
        Mat grayImage = pointscan.getRoiAndProcess(srcImage, r);  //获取要处理的感兴趣区域
        //得到二值图像
        Mat binaImage = pointscan.getBinImage(grayImage);
        //根据轮廓大小进行筛选
        pointscan.bwareaopen(binaImage, 150);
        //在图像底部进行查找定位点
        pointscan.findBottomPoints(binaImage);
        //添加图像中点以获取靠近中点的两个点，此处第二个参数可以任取
        Point2d p = Point2d(pointscan.m_col/2, 0);
        //对所有点进行排序，对点进行排序然后筛选离中线最近的两个点作为底端点
        if(pointscan.points.size()<2)
            continue;
        pointscan.getWantedPoints(p);
        //在原始图中进行画线
        pointscan.findCropLine(pointscan.g_srcImage);
        //获取导航线上对应实际的点
        double x = pointscan.getLastedPoint(90);
        //发送图片给界面线程用来显
        emit ImageChanged(pointscan.g_srcImage, x);
        //每隔一秒发射一次信号
        //waitKey(fps);
        //srcImage.release();
    }
    cap.release();
    stopped = true;
}

void MyThread::stop()
{
    stopped = true;   
}

