#ifndef MYTHREAD_H
#define MYTHREAD_H
#include "pointscan.h"
#include <QThread>
#include <QMutex>
#include <opencv2/opencv.hpp>
using namespace cv;
class MyThread : public QThread
{
    Q_OBJECT
public:
    void stop();
    explicit MyThread(QObject *parent = 0);
    VideoCapture cap;
    Mat srcImage;
protected:
    void run();
private:
    volatile bool stopped;

signals:
    void ImageChanged(Mat img, double x);
public slots:

};

#endif // MYTHREAD_H
