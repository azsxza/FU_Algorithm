#include"global.h"

Mat GTA5Style (const Mat& I, int brushSize, int coarseness)
{
	
	Mat smoothI = CoherenceFilter (I, 11, 11, 0.5, 4);
	
	Mat highlightI = BalanceColor (smoothI, 49, 2, -31, TRANSFERMODE_HIGHLIGHTS, true);
	Mat shadowsI = BalanceColor (highlightI, 0, 5, 0, TRANSFERMODE_SHADOWS, true);
	Mat dst = BalanceColor (shadowsI, 23, 0, -32, TRANSFERMODE_MIDTONES, true);

	return dst;
}


void GTA5Style (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int brushSize, int coarseness)
{
	int channel = Stride / Width;
	Mat I = cv::Mat (Height, Width, CV_MAKETYPE (CV_8U, channel), Src, Stride);

	if (channel == 4)
		cvtColor (I, I, COLOR_BGRA2BGR);
	
	Mat dst = GTA5Style (I, 3, 50);

	for (int i = 0;i < dst.cols*dst.rows * 3;i++)
		Dest[i] = dst.data[i];
}