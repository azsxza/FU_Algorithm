#include"global.h"

void Cartoon2 (uchar* src, uchar* dest, int height, int width)
{
	Mat b = imread ("ios-mangafilter-screentonefull.png");
	Mat blur;
	Mat src2 (height, width, CV_8UC3, src);
	
	cvtColor (src2, blur, CV_BGR2GRAY);
	cvtColor (b, b, CV_BGR2GRAY);

	
	resize (b, b, blur.size ());
	uchar* p = blur.ptr (0);
	uchar* pb = b.ptr (0);
	for (int i = 0;i < height*width;p++, i++, pb++)
	{
		if (*p < 70)
			*p = 35;
		else if (*p < 120)
			*p = *pb;
		else
			*p = 230;
	}
	cvtColor (blur, blur, CV_GRAY2BGR);
	for (int i = 0;i < height*width * 3;i++)
		dest[i] = blur.data[i];
}

Mat Cartoon2 (Mat src2)
{
	Mat b = imread ("ios-mangafilter-screentonefull.png");
	Mat blur;

	cvtColor (src2, blur, CV_BGR2GRAY);
	cvtColor (b, b, CV_BGR2GRAY);


	resize (b, b, blur.size ());
	uchar* p = blur.ptr (0);
	uchar* pb = b.ptr (0);
	for (int i = 0;i < src2.cols*src2.rows;p++, i++, pb++)
	{
		if (*p < 70)
			*p = 35;
		else if (*p < 120)
			*p = *pb;
		else
			*p = 255;
	}
	cvtColor (blur, blur, CV_GRAY2BGR);
	return blur;
}