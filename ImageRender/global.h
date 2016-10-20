#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#include<math.h>
#include<Windows.h>
#include<thread>
#include<ycapture.h>
#include<CaptureSender.h>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

enum TRANSFERMODE
{
	TRANSFERMODE_SHADOWS,		//阴影
	TRANSFERMODE_MIDTONES,	//中间调
	TRANSFERMODE_HIGHLIGHTS,	//高光
};

enum LineStyle
{
	DOG, FDOG
};

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define ROUND(x) ((int) ((x) + 0.5))

#ifndef CLAMP
#define CLAMP(x,l,u) ((x)<(l)?(l):((x)>(u)?(u):(x)))
#endif

#define CLAMP0255(x) ((x)<(0)?(0):((x)>(255)?(255):(x)))
#define SQR(x) ((x)*(x))


#define CLAMP0255(x) ((x)<(0)?(0):((x)>(255)?(255):(x)))


Mat CoherenceFilter (const Mat& img, int sigma, int str_sigma, float blend, int iter);
Mat BalanceColor (Mat bitmap, int cyan, int magenta, int yellow, TRANSFERMODE mode, bool preserveLuminosity);
Mat AdjustCurve (Mat Src, uchar (&r_lookup)[256], uchar (&g_lookup)[256], uchar (&b_lookup)[256]);
Mat PreserveLuminosityAdjustCurve (Mat I, uchar (&r_lookup)[256], uchar (&g_lookup)[256], uchar (&b_lookup)[256]);
static int hslValueInt (double n1, double n2, double hue);
void Rgb2Hsl_Int (int& red, int& green, int& blue);
int Rgb2Hsl_L (uchar red, uchar green, uchar blue);
void Hsl2Rgb_Int (int& hue, int& saturation, int& lightness);
int Clamp (int v, int min, int max);
Mat GenETFFromGradientMap (cv::Mat src, int kernel = 5, int iter = 3);
Mat GenETFFromInputImage (cv::Mat src, int kernel = 5, int iter = 3);
Mat GetEdgeTangentFlow (cv::Mat src);
Mat VisualizeByArrow (cv::Mat src);
void MakeUnit (cv::Vec2f& v);
void VisualizeByLIC (cv::Mat src);
Mat BlendImage (Mat I1, Mat I2);
Mat RotateImage (const Mat source, double angle, int border = 20);

//视频实时处理可用上的函数
Mat PencilDraw (Mat I, int ks, int dirNum, double gamma, Mat P);
Mat ColorPencilDraw (Mat I2, int ks, int dirNum, double gamma, Mat P2);
Mat MtxxCQ (const Mat& img, float opacity);
Mat MtxxLifeSketch (Mat I, float opacity);
Mat Worlde (Mat src);
Mat Carving (Mat src2);
Mat Cartoon2 (Mat src2);
Mat Quban (Mat I);
Mat Maoboli (Mat I);
Mat Mosaic (Mat I, int size);




//铅笔画 1
extern "C" __declspec(dllexport) void PencilDraw (uchar* I, int ks, int dirNum, double gamma, uchar* P, int rows, int cols, int rows2, int cols2, uchar* output);

//彩色铅笔画 2
extern "C" __declspec(dllexport) void ColorPencilDraw (uchar* I, int ks, int dirNum, double gamma, uchar* P, int rows, int cols, int rows2, int cols2,uchar* output);

//GTA5风格 3 
extern "C" __declspec(dllexport) void GTA5Style (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int brushSize, int coarseness);

//卡通 4
extern "C" __declspec(dllexport) void  Cartoon (uchar* Src, uchar* Dest, int Width, int Height, int Stride, float sigma_s, float sigma_r);

//抽象画 5
extern "C" __declspec(dllexport) void CoherenceFilter (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int sigma, int str_sigma, float blend, int iter);

//线条画 6
extern "C" __declspec(dllexport) void  LineDrawing (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int kernel, int iter, float sigma_c, float sigma_m, LineStyle style);

//磨皮 7
extern "C" __declspec(dllexport) void Dermabrasion (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int ks, int opacity, int white);

//油画 8
extern "C" __declspec(dllexport) void OilPaint (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int brushSize, int coarseness);

//美图秀秀油画 9
extern "C" __declspec(dllexport) void MtxxOilPaint (uchar* Src, uchar* Dest, int Width, int Height, int Stride, float opacity);

//美图秀秀彩色铅笔画 10
extern "C" __declspec(dllexport) void MtxxCQ (uchar* Src, uchar* Dest, int Width, int Height, int Stride, float opacity);

//美图秀秀写生铅笔画 11
extern "C" __declspec(dllexport) void MtxxLifeSketch (uchar* Src, uchar* Dest, int Width, int Height, int Stride, float opacity);

//水彩画 12 
extern "C" __declspec(dllexport) void WaterColor (uchar* Src, uchar* Dest, int Width, int Height, int brushSize_w, int brushSize_h, int stride);

//设置色调 13
extern "C" __declspec(dllexport) void SetHue (uchar* Src, uchar* Dest, int height, int width, int n, int stride);

//设置饱和度 14
extern "C" __declspec(dllexport) void SetSaturation (uchar* Src, uchar* Dest, int height, int width, int n, int stride);

// 复古 15
extern "C" __declspec(dllexport) void Worlde (uchar* src, uchar* dest, int height, int width, int stride);

//雕刻 16
extern "C" __declspec(dllexport) void Carving (uchar* src, uchar*dest, int height, int width, int stride);

//卡通2 17   暂时不能用
extern "C" __declspec(dllexport) void Cartoon2 (uchar* src, uchar* dest, int height, int width, int stride);

//祛斑 18
extern "C" __declspec(dllexport) void Quban (uchar* src, uchar* dest, int stride, int height, int width);

//毛玻璃 19
extern "C" __declspec(dllexport) void Maoboli (uchar* src, uchar* dest, int stride, int height, int width);

//马赛克 20
extern "C" __declspec(dllexport) void Mosaic (uchar* src, uchar* dest, int stride, int height, int width,int size);

//ascii马赛克 21
extern "C" __declspec(dllexport) void ascii (uchar* src, uchar*dest, int width, int height, int stride);

//edgePreservingFilter  22
extern "C" __declspec(dllexport) void edgePreservingFilter (uchar*src, uchar*dest, int height, int width, int stride);

//detailEnhance 23
extern "C" __declspec(dllexport) void detailEnhance (uchar* src, uchar* dest, int height, int width, int stride);

//stylization 24
extern "C" __declspec(dllexport) void stylization (uchar* src, uchar* dest, int height, int width, int stride);

//粉笔画 25
extern "C" __declspec(dllexport) void Fenbi (uchar* src, uchar* dest, int height, int width, int stride);

//LowPoly风格 26
extern "C" __declspec(dllexport) void Lowpoly (uchar* src, uchar*dest, int index, int width, int height, int stride);

//线条2 27
extern "C" __declspec(dllexport) void Line (uchar* src, uchar*dest, int width, int height, int stride);

//减少颜色 28
extern "C" _declspec(dllexport) void ColorReduce (uchar* src, uchar* dest, int div, int height, int width, int stride);

//................ 29
extern "C" _declspec(dllexport) void shan (uchar* src, uchar* dest, double ddd, int height, int width, int stride);











//face
extern "C" __declspec(dllexport) void inti ();
extern "C" __declspec(dllexport) void GetKeyPoint (uchar* imgData, int* key, int height, int width);