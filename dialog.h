#ifndef DIALOG_H
#define DIALOG_H
#include <QImage>
#include <QDialog>
#include "mythread.h"
#include "login.h"
#include "cameracalibration.h"
#include "findcontour.h"
#include "pointscan.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QString str, QWidget *parent = 0);
    ~Dialog();

protected:
    virtual void paintEvent(QPaintEvent *event);
private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void changeImage(Mat image, double x);
    QImage Mat2QImage(Mat &image);
    void showImage(QImage &image);
    void showImage2(QImage &image);
    void on_openImage_clicked();
    void on_writeImg_clicked();
    void on_stopVedio_clicked();
    void on_cropBtn_clicked();
    void on_openSerial_clicked();
    void on_Send_Rec_data_clicked();
    void readMyCom();
    void on_clearData_clicked();
    void on_closeSerial_clicked();
    void ClockTimeout();
    //void scrollCaption();
    void on_calculate_position_clicked();

    void on_point_scan_clicked();

private:
    Ui::Dialog *ui;
    MyThread *thread = 0;
    QGraphicsScene *scene = NULL;
    QGraphicsScene *scene_ = NULL;
    QImage gg_srcImage;
    Mat image,save_img;
    FindContour crop;
    //PointScan *pointscan;
    QSerialPort *my_serialPort = 0;//(实例化一个指向串口的指针，可以用于访问串口)
    QByteArray requestData;//（用于存储从串口那读取的数据）
    QTimer *timer,*Date_timer;//（用于计时）
    //QTimer *str_timer;
    //QSTring strScrollCation;
    int count = 1;
    QString user;
    cameraCalibration cam;
};

#endif // DIALOG_H
