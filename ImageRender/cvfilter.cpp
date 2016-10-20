#include"global.h"

void edgePreservingFilter (uchar*src, uchar*dest, int height, int width)
{
	Mat I (height, width, CV_8UC3, src);
	Mat T;
	edgePreservingFilter (I, T);
	for (int i = 0;i < width*height * 3;i++)
		dest[i] = T.data[i];
}

void detailEnhance (uchar* src, uchar* dest, int height, int width)
{
	Mat I (height, width, CV_8UC3, src);
	Mat T;
	detailEnhance (I, T);
	for (int i = 0;i < width*height * 3;i++)
		dest[i] = T.data[i];
}

void stylization (uchar* src, uchar* dest, int height, int width)
{
	Mat I (height, width, CV_8UC3, src);
	Mat T;
	stylization (I, T);
	for (int i = 0;i < width*height * 3;i++)
		dest[i] = T.data[i];
}