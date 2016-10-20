#include"global.h"

void SetHue (uchar* Src, uchar* Dest, int height, int width, int n)
{
	Mat Src2 (height, width, CV_8UC3, Src);
	Mat tmp;
	cvtColor (Src2, tmp, CV_BGR2HSV);
	vector<Mat> vec;
	split (tmp, vec);
	for (int i = 0;i < Src2.cols*Src2.rows;i++)
		vec[0].data[i] = (vec[0].data[i] + n) % 180;
	merge (vec, tmp);
	cvtColor (tmp, tmp, CV_HSV2BGR);
	for (int i = 0;i < Src2.cols*Src2.rows * 3;i++)
		Dest[i] = tmp.data[i];
}