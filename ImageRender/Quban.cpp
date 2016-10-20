#include"global.h"

void Quban (uchar* src, uchar* dest, int stride, int height, int width)
{
	int channel = stride / width;
	Mat I = cv::Mat (height, width, CV_MAKETYPE (CV_8U, channel), src, stride);

	Mat b;
	edgePreservingFilter (I, b);
	Mat c;
	c = 0.75*b + 0.25*I;
	for (int i = 0;i < height*width * channel;i++)
		dest[i] = c.data[i];
}

Mat Quban (Mat I)
{
	Mat b;
	edgePreservingFilter (I, b);
	Mat c;
	c = 0.75*b + 0.25*I;
	return c;
}