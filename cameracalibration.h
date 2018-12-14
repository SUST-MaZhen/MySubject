#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H
#define IMGROW 240
#define IMGCOL 320
class cameraCalibration
{
public:
    cameraCalibration();
    float getAerfa(float ins, float k_c, float k_k, float f);
    float getBeta(float ins, float k_c, float k_k, float f);
    float* getCmToPixes(float* P, float imgrow, float imgcol);
    float getK2(float h, float beta, float fai, float f);
    float* getPositionOfABtoXYZ(float *XYZ, float* P, float a, float b, float f);
    float* getPositionOfXYZtoXYZ3(float* XYZ, float *XYZ3, float k2);
    float* getPositionOfXYZ3toUVW3(float* UVW3, float* XYZ3, float fai, float h);
    float* getPositionOfUVW3toUVW4(float* UVW4, float* UVW3, float h);

    void readJson();
public:
    float cameraHeight;
    float cameraFocus;
    float cameraAngle;
    float camerEyeAngle;
    float cmos_chang;
    float cmos_kuan;
    float ins;
    float P[2];
    float XYZ[3];
    float XYZ3[3];  //像素坐标在S3平面中的坐标
    float UVW3[3];  //S3平面在UVW中的坐标
    float UVW4[3];  //世界坐标UVW4
};

#endif // CAMERACALIBRATION_H
