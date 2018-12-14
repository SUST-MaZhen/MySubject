#include "cameracalibration.h"
#include <math.h>
#include <lib/json/json.h>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <stdlib.h>
using namespace std;
cameraCalibration::cameraCalibration()
{
    //读取相机配置文件
    readJson();

}
float cameraCalibration::getAerfa(float ins, float k_c, float k_k, float f)
{
    this->cmos_chang = k_c*sqrt(((2.54 / ins)*(2.54 / ins)) / (k_c*k_c + k_k*k_k));
    qDebug() << "length of cmos is:" << this->cmos_chang;
    return 2 * atan(cmos_chang / 2 / f)*180.0 / 3.1415;
}

float cameraCalibration::getBeta(float ins, float k_c, float k_k, float f)
{
    this->cmos_kuan = k_k*sqrt(((2.54 / ins)*(2.54 / ins)) / (k_c*k_c + k_k*k_k));
    qDebug() << "width of cmos is:" << this->cmos_kuan;
    return 2 * atan(cmos_kuan / 2 / f)*180.0 / 3.1415;
}

float cameraCalibration::getK2(float h, float beta, float fai, float f)
{
    return (h*cos(beta*3.1415 / 180.0 / 2.0) / cos((90.0 - fai - (beta / 2))*3.1415 / 180.0))/f;
}

float* cameraCalibration::getCmToPixes(float* P, float imgrow, float imgcol)
{
    P[0] = this->cmos_kuan / imgrow;
    P[1] = this->cmos_chang / imgcol;
    qDebug() << "kuan:" << P[0] <<"   "<< this->cmos_kuan;
    qDebug() << "chang:" << P[1] << "   " << this->cmos_chang;
    return P;
}

float* cameraCalibration::getPositionOfABtoXYZ(float *XYZ, float* P, float a, float b, float f)
{
    XYZ[0] = P[1]*(b-IMGCOL/2);
    XYZ[1] = P[0]*(IMGROW/2-a);
    XYZ[2] = f;
    qDebug() << "x: " << XYZ[0] << " y: " << XYZ[1] << " z: " << XYZ[2];
    return XYZ;
}

float* cameraCalibration::getPositionOfXYZtoXYZ3(float* XYZ, float *XYZ3, float k2)
{
    XYZ3[0] = k2*XYZ[0];
    XYZ3[1] = k2*XYZ[1];
    XYZ3[2] = k2*XYZ[2];
    qDebug() << "x3: " << XYZ3[0] << " y3: " << XYZ3[1] << " z3: " << XYZ3[2];
    return XYZ3;
}

float* cameraCalibration::getPositionOfXYZ3toUVW3(float* UVW3, float* XYZ3, float fai, float h)
{
    UVW3[0] = XYZ3[0];
    UVW3[2] = XYZ3[1] * sin(fai*3.1415 / 180.0) + XYZ3[2] * cos(fai*3.1415 / 180.0);
    UVW3[1] = XYZ3[2] * sin(fai*3.1415 / 180.0) - XYZ3[1] * cos(fai*3.1415 / 180.0) - h;
    qDebug() << "u3: " << UVW3[0] << " v3: " << UVW3[1] << " w3: " << UVW3[2];
    return UVW3;
}

float* cameraCalibration::getPositionOfUVW3toUVW4(float* UVW4, float* UVW3, float h)
{
    UVW4[0] = (h / (UVW3[1] + h))*UVW3[0];
    UVW4[1] = 0.0;
    UVW4[2] = (h / (UVW3[1] + h))*UVW3[2];
    qDebug() << "u4: " << UVW4[0] << " v4: " << UVW4[1] << " w4: " << UVW4[2];
    return UVW4;
}

void cameraCalibration::readJson()
{
    Json::Reader reader;
    Json::Value root;
    //读json配置文件
    ifstream is("D:\\masterstu\\配置文件\\config.json", ios::binary);
    if(!is.is_open())
    {
        qDebug() << "open file error\n";
        return;
    }

    if(reader.parse(is,root))
    {
        this->cameraHeight = root["heightOfCamera"].asDouble();
        this->cameraFocus = root["focusOfCamera"].asDouble();
        this->cameraAngle = root["angleOfCamera"].asDouble();
        this->ins = root["ins"].asDouble();
        qDebug() << "img_row:" << IMGROW << "  img_col:" << IMGCOL;
        qDebug() << "camHeight:" << this->cameraHeight << "  foucus:" << this->cameraFocus << "  eyeAngle:" << this->cameraAngle;
    }

    is.close();
}

