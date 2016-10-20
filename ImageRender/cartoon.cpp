#include"global.h"

void  Cartoon (uchar* Src, uchar* Dest, int Width, int Height, int Stride, float sigma_s, float sigma_r)
{
	int channel = Stride / Width;
	Mat I = cv::Mat (Height, Width, CV_MAKETYPE (CV_8U, channel), Src, Stride);

	if (channel == 4)
		cvtColor (I, I, COLOR_BGRA2BGR);

	Mat dst;
	stylization (I, dst, sigma_s, sigma_r);

	for (int i = 0;i < dst.cols*dst.rows * 3;i++)
		Dest[i] = dst.data[i];
}