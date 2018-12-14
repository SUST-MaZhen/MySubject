#include "dialog.h"
#include "mythread.h"
#include "ui_dialog.h"
#include <iostream>
#include <fstream>
#include <QTimer>
#include <QTime>
#include <QDate>
#include <QIcon>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QIODevice>
#include <QMetaType>
#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsView>
#include <QtSerialPort/QSerialPort>
#include <QtConcurrent/QtConcurrent>
#include <QtSerialPort/QSerialPortInfo>
#include <opencv2/opencv.hpp>
#include <lib/json/json.h>
using namespace cv;
using namespace std;
Dialog::Dialog(QString str, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    user = str;
//    strScrollCation = QString::fromLocal8Bit("开发者：千里小马哥  Mobile：15291836200   Email：sust-mazhen@outlook.com");
//    str_timer = new QTimer(this);
//    connect(str_timer,SIGNAL(timeout()),this,SLOT(scrollCaption()));
//    str_timer->start(200);

    QString button_style="QPushButton{background-color:green;color: white; border-radius:5px;  border: 2px groove gray; border-style: outset;}"
                         "QPushButton:hover{background-color:white; color: black;}"
                         "QPushButton:pressed{background-color:rgb(85, 170, 255); border-style: inset; }";
    this->setStyleSheet(button_style);

    scene = new QGraphicsScene;
    scene_ = new QGraphicsScene;
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    setWindowIcon(QIcon("icon.ico"));
    foreach (const QSerialPortInfo &Info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "portName    :"  << Info.portName();//调试时可以看的串口信息
        qDebug() << "Description   :" << Info.description();
        qDebug() << "Manufacturer:" << Info.manufacturer();
        QSerialPort serial;
        serial.setPort(Info);
        if(serial.open(QIODevice::ReadWrite))//如果串口是可以读写方式打开的
        {
            ui->serialPort->addItem(Info.portName());//在comboBox那添加串口号
            serial.close();
        }
    }
    ui->user_online->setText(user);
    ui->closeSerial->setEnabled(false);
    ui->Send_Rec_data->setEnabled(false);
    ui->clearData->setEnabled(false);
    //this->setFixedSize( this->width (),this->height ());
    //this->setStyleSheet("background-color:gray");
    qRegisterMetaType<Mat>("Mat");
    ui->date->setStyleSheet("color:white;background-color:transparent");
    ui->HourMin->setStyleSheet("color:white;background-color:transparent");
    Date_timer = new QTimer(this);
    Date_timer->start();
    connect(Date_timer,SIGNAL(timeout()),this,SLOT(ClockTimeout()));
}

Dialog::~Dialog()
{
    this->count = 0;
    delete ui;
    delete thread;
    delete scene;
    delete scene_;
    delete my_serialPort;
}


void Dialog::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);
   QPixmap pix;
   pix.load(":/new/prefix1/image/nyxdh.jpg");//加载图片地址 可以是资源文件
   painter.drawPixmap(0, 0, width() , height() , pix);//绘制图片 横坐标、纵坐标、宽度、高度
   QWidget::paintEvent(event);
}
//文字滚动
//void Dialog::scrollCaption()
//{
//    static int nPos = 0;
//    if(nPos>strScrollCation.length())
//        nPos = 0;

//    ui->guang_gao->setText(strScrollCation.mid(nPos));
//    nPos++;
//}


void Dialog::ClockTimeout()
{
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    ui->HourMin->setText(time.toString("hh:mm:ss"));
    ui->date->setText(date.toString("yyyy-MM-dd"));
}

QImage Dialog::Mat2QImage(Mat &image)
{
    QImage img;

    if(ui->gray_check->isChecked())
        cv::cvtColor(image,image,CV_BGR2GRAY);

    //save_img = image;

    if(image.channels()==3)
    {
        //cvt Mat BGR 2 QImage RGB
        cv::cvtColor(image,image,CV_BGR2RGB);
        img =QImage((const unsigned char*)(image.data),
                    image.cols,image.rows,
                    image.cols*image.channels(),
                    QImage::Format_RGB888);
    }
    else
    {
        img =QImage((const unsigned char*)(image.data),
                    image.cols,image.rows,
                    image.cols*image.channels(),
                    QImage::Format_Grayscale8);
    }

    return img;
}


void Dialog::on_startButton_clicked()
{
    ui->imgView->repaint();
    thread = new MyThread();
    connect(thread, SIGNAL(ImageChanged(Mat, double)), this, SLOT(changeImage(Mat, double)));
    thread->start();
    //qDebug()<<"in thread "<<QThread::currentThreadId();//测试当前线程归属
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->openImage->setEnabled(false);
}

void Dialog::on_stopButton_clicked()
{
    disconnect(thread, SIGNAL(ImageChanged(Mat, double)), this, SLOT(changeImage(Mat, double)));
    ui->imgView->repaint();
    if (thread->isRunning())
    {
        thread->stop();
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        ui->openImage->setEnabled(true);
        //thread->cap.release();//千万不要写，不然就蹦了
        thread->destroyed();
    }

}

void Dialog::changeImage(Mat img, double x)
{
   gg_srcImage = Mat2QImage(img);
   showImage(gg_srcImage);
   ui->heng_err->setText(QString::number(x, 10, 1));
}

void Dialog::showImage( QImage &image)
{
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(image));
    ui->imgView->setScene(scene);
    ui->imgView->show();
}

void Dialog::showImage2( QImage &image)
{
    scene_->clear();
    ui->cropView->repaint();
    scene_->addPixmap(QPixmap::fromImage(image));
    ui->cropView->setScene(scene_);
    ui->cropView->show();
    //ui->imgView->resize(image.width() + 5, image.height() + 5);
}


//保存图片到本地
void Dialog::on_writeImg_clicked()
{  
    if(!gg_srcImage.isNull())
        gg_srcImage.save("test.jpg");
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("No image to save"));
        msgBox.exec();
    }
}

void Dialog::on_openImage_clicked()
{
    ui->cropView->repaint();

    QString filename = QFileDialog::getOpenFileName(this,
                      tr("Open Image"),".",tr("Image File (*.jpg *.png *.bmp)"));

    image = imread(filename.toStdString());//QString转String
    if(!image.data)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Open image error"));
        msgBox.exec();
    }
    else
    {
       showImage2(Mat2QImage(image));
    }

}



void Dialog::on_stopVedio_clicked()
{
    //do something

}

void Dialog::on_cropBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                      tr("Open Image"),"D:\\masterstu\\imageData\\img",tr("Image File (*.jpg *.png *.bmp)"));

    double t = (double)getTickCount();
    if (!crop.grayShift(filename.toStdString()))
    {
        QMessageBox msgBox;
        msgBox.setText(tr("open image failed, please open the image with 240*320"));
        msgBox.exec();
        return;
    }
    qDebug()<<"max:"<<crop.RadonTrans()<<endl;

    crop.getTiao(crop.g_dstImage);

    crop.getPoint(crop.M);
    //crop.getPointByVP(crop.M);

    //输入拟合点
    std::vector<cv::Point> points;
    for (int i = 0; i < 12;i++)
    {
        points.push_back(cv::Point(crop.x[i], crop.y[i]));
    }


    Vec4f line_;
    fitLine(points, line_, CV_DIST_HUBER, 1, 0.01, 0.01);


    double cos_theta = line_[0];
    double sin_theta = line_[1];
    double x0 = line_[2], y0 = line_[3];

    double phi = atan2(sin_theta, cos_theta) + PI / 2.0;
    double rho = y0 * cos_theta - x0 * sin_theta;

    qDebug()<< "phi = " << phi / PI * 180;
    qDebug()<< "rho = " << rho;

    crop.drawLine(crop.g_srcImage, phi, rho, cv::Scalar(255,0,0));
    //crop.hough_line(crop.x, crop.y);

    t = ((double)getTickCount() - t) / getTickFrequency();

    ui->runTime->setText(QString::number(t,'f',3)+" S");

    //QImage img_res = Mat2QImage(crop.g_srcImage);
    //QString path=  "D:\\img_res\\";
    //img_res.save(path + QString::number(count,10) + ".jpg");
//    ifstream ifs;
//    ifs.open("D:\\img_reschuizhi\\this.json",ios::binary);
//    Json::Value val;
//    val["count"] = Json::Value(t);

    count++;
//    ifs.close();
    QImage img = Mat2QImage(crop.g_srcImage);
    showImage2(img);
}


void Dialog::on_openSerial_clicked()
{
    my_serialPort = new QSerialPort(this);
    my_serialPort->setPortName(ui->serialPort->currentText());
    if(my_serialPort->open(QIODevice::ReadWrite))
    {
        my_serialPort->open(QIODevice::ReadWrite);
        qDebug()<<ui->serialPort->currentText();
        my_serialPort->setBaudRate(ui->badeRate->currentText().toInt());
        my_serialPort->setDataBits(QSerialPort::Data8);//数据字节，8字节
        my_serialPort->setParity(QSerialPort::NoParity);//无校验位
        my_serialPort->setFlowControl(QSerialPort::NoFlowControl);//数据流控制，无
        my_serialPort->setStopBits(QSerialPort::OneStop);//一位停止位
        ui->closeSerial->setEnabled(true);
        ui->Send_Rec_data->setEnabled(true);
        ui->openSerial->setEnabled(false);
        ui->clearData->setEnabled(true);
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(readMyCom()));
        timer->start(100);//每100ms读一次串口
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("connect port failed"));
        msgBox.exec();
    }

}

void Dialog::on_Send_Rec_data_clicked()
{
    QString str = ui->lineEdit_write->text();
    int len = str.length();
    if(!(ui->lineEdit_write->text().isEmpty()) && str[len-1]=='#')
    {
        ui->textEdit_read->append("Qt: "+ str);
        my_serialPort->write((str.toUtf8()));

        ui->lineEdit_write->setText("");
    }
    else
    {
        //delete my_serialPort;
        QMessageBox msgBox;
        msgBox.setText(tr("content is empty or end with '#'"));
        msgBox.exec();
        return;
    }

}

void Dialog::readMyCom()
{
    requestData = my_serialPort->readAll();

    if(!requestData.isNull())
    {
        ui->textEdit_read->append("stm32: " + requestData);
    }
    requestData.clear();
}

void Dialog::on_clearData_clicked()
{
    if(ui->textEdit_read->document()->isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("content is empty"));
        msgBox.exec();
    }
    else
    {
        ui->textEdit_read->clear();
    }
}

void Dialog::on_closeSerial_clicked()
{

    my_serialPort->close();
    delete my_serialPort;
    ui->openSerial->setEnabled(true);
    ui->closeSerial->setEnabled(false);
    ui->Send_Rec_data->setEnabled(false);
    disconnect(timer, SIGNAL(timeout()), this, SLOT(readMyCom()));
    QMessageBox msgBox;
    msgBox.setText(tr("serial is closed"));
    msgBox.exec();
}

void Dialog::on_calculate_position_clicked()
{
    cameraCalibration cam;
    float beta = cam.getBeta(cam.ins, 4, 3, cam.cameraFocus);
    float erfa = cam.getAerfa(cam.ins, 4, 3, cam.cameraFocus);

    qDebug() << "beta:" << beta << "  erfa:" << erfa;
    float k2 = cam.getK2(cam.cameraHeight, beta, cam.cameraAngle, cam.cameraFocus);
    qDebug() << "K2:" << k2 << "    d:"<< k2*cam.cameraFocus;
    cam.getCmToPixes(cam.P, IMGROW, IMGCOL);
    cam.getPositionOfABtoXYZ(cam.XYZ, cam.P, ui->pixes_row->text().toInt(), ui->pixes_col->text().toInt(), cam.cameraFocus);
    cam.getPositionOfXYZtoXYZ3(cam.XYZ, cam.XYZ3, k2);
    cam.getPositionOfXYZ3toUVW3(cam.UVW3, cam.XYZ3, cam.cameraAngle, cam.cameraHeight);
    cam.getPositionOfUVW3toUVW4(cam.UVW4, cam.UVW3, cam.cameraHeight);

    ui->u4_word->setText(QString::number(cam.UVW4[0], 'f', 1));
    ui->w4_word->setText(QString::number(cam.UVW4[2], 'f', 1));
}

void Dialog::on_point_scan_clicked()
{
    char text[15];
    QString filename = QFileDialog::getOpenFileName(this,
                      tr("Open Image"),"D:\\masterstu\\imageData\\img",tr("Image File (*.jpg *.png *.bmp)"));
    Rect r = Rect(0, 0, 320, 240);
    PointScan pointscan1(30);
    pointscan1.m_col = r.width;
    pointscan1.m_row = r.height;
    if(pointscan1.readImage(filename.toStdString())==-1)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("read image failed!"));
        msgBox.exec();
        return;
    }
    Mat hehe = pointscan1.g_srcImage;
    pointscan1.g_srcImage = pointscan1.g_srcImage(r);
    //获取要处理的感兴趣区域
    Mat grayImage = pointscan1.getRoiAndProcess(hehe, r);
    Mat binImage = pointscan1.getBinImage(grayImage);
    threshold(binImage, binImage, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);//6月7号添加的,这句很重要
    pointscan1.bwareaopen(binImage, 150);
    //pointscan->imfill(binImage, binImage);
    double t = (double)getTickCount();
    //定位基准点
    pointscan1.findBottomPoints(binImage);
    Point2d p = Point2d(pointscan1.m_col/2, 0);
    //对所有点进行排序，对点进行排序然后筛选离中线最近的两个点作为底端点
    pointscan1.getWantedPoints(p);
    //在原始图中进行画线
    pointscan1.findCropLine(pointscan1.g_srcImage);
    //获取导航线上对应实际的点
    double x = pointscan1.getLastedPoint(90);
    t = ((double)getTickCount() - t) / getTickFrequency();
    sprintf(text, "Time: %.3f s", t);
    ui->runTime->setText(QString::number(t,'f',3)+" S");
    ui->heng_err->setText(QString::number(x, 10, 1));
    QImage img = Mat2QImage(pointscan1.g_srcImage);
    showImage2(img);
}


