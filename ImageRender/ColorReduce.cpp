#include"global.h"

Mat ColorReduce (Mat I, int div)
{
	Mat src = I.clone ();
	uchar*ppp = src.ptr (0);
	uchar* p = I.ptr (0);
	for (int i = 0;i < I.cols*I.rows * 3;i++)
		ppp[i] = p[i] - (p[i] % div) + div / 2;
	return src;
}

void ColorReduce (uchar* src, uchar* dest, int div, int height, int width,int stride)
{
	int channel = stride / width;
	Mat I = cv::Mat (height, width, CV_MAKETYPE (CV_8U, channel), src, stride);
	Mat T = ColorReduce (I, div);
	for (int i = 0;i < width*height * 3;i++)
		dest[i] = T.data[i];
}
