#include"global.h"

void Worlde (uchar* src, uchar* dest, int height, int width,int stride)
{
	int c = stride / width;
	float B, G, R, newB, newG, newR;
	for (int i = 0; i < height*width; i++)
	{
	B = src[c * i];
	G = src[c * i + 1];
	R = src[c * i + 2];
	newB = 0.272*R + 0.534*G + 0.131*B;
	newG = 0.349*R + 0.686*G + 0.168*B;
	newR = 0.393*R + 0.769*G + 0.189*B;
	if (newG > 255)newG = 255;
	if (newR > 255)newR = 255;
	dest[c * i] = (uchar)newB;
	dest[c * i + 1] = (uchar)newG;
	dest[c * i + 2] = (uchar)newR;
	}
}

Mat Worlde (Mat src)
{
	Mat dst = src.clone ();
	float B, G, R, newB, newG, newR;
	for (int i = 0; i < src.cols*src.rows; i++)
	{
		B = src.data[3 * i];
		G = src.data[3 * i + 1];
		R = src.data[3 * i + 2];
		newB = 0.272*R + 0.534*G + 0.131*B;
		newG = 0.349*R + 0.686*G + 0.168*B;
		newR = 0.393*R + 0.769*G + 0.189*B;
		if (newG > 255)newG = 255;
		if (newR > 255)newR = 255;
		dst.data[3 * i] = (uchar)newB;
		dst.data[3 * i + 1] = (uchar)newG;
		dst.data[3 * i + 2] = (uchar)newR;
	}
	return dst;
}

Mat Nuanse (Mat src)
{
	Mat dst = src.clone ();
	float B, G, R, newB, newG, newR;
	for (int i = 0; i < src.cols*src.rows; i++)
	{
		B = src.data[3 * i];
		G = src.data[3 * i + 1];
		R = src.data[3 * i + 2];
		newB = -0.9689*R + 2.2415*G - 0.6775*B;
		newG = 0.5782*R - 0.6287*G + 0.9845*B;
		newR = 0.2494*R + 2.2957*G - 1.3445*B;
		if (newG > 255)newG = 255;
		if (newR > 255)newR = 255;
		if (newB > 255)newB = 255;
		if (newR < 0)newR = 0;
		if (newG < 0)newG = 0;
		if (newB < 0)newB = 0;
		dst.data[3 * i] = (uchar)newB;
		dst.data[3 * i + 1] = (uchar)newG;
		dst.data[3 * i + 2] = (uchar)newR;
	}
	return dst;
}

Mat Sansehua (Mat src2)
{
	Mat blur;
	cvtColor (src2, blur, CV_BGR2GRAY);
	uchar* p = blur.ptr (0);
	for (int i = 0;i < src2.cols*src2.rows;p++, i++)
	{
		if (*p < 70)
			src2.at<Vec3b> (i / src2.cols, i%src2.cols) = Vec3b (110, 94, 58);
		else if (*p < 120)
			src2.at<Vec3b> (i / src2.cols, i%src2.cols) = Vec3b (219, 200, 165);
		else
			src2.at<Vec3b> (i / src2.cols, i%src2.cols) = Vec3b (244, 226, 219);
	}
	return src2;
}