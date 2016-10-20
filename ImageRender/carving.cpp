#include"global.h"

void Carving (uchar* src, uchar*dest, int height, int width,int stride)
{
	int channel = stride / width;
	Mat img1 = cv::Mat (height, width, CV_MAKETYPE (CV_8U, 3));
	Mat src2 = cv::Mat (height, width, CV_MAKETYPE (CV_8U, channel), src, stride);
	if(channel==4)
		cvtColor (src2, src2, CV_BGRA2BGR);
	for (int y = 1; y<height - 1; y++)
	{
		uchar *p0 = src2.ptr<uchar> (y);
		uchar *p1 = src2.ptr<uchar> (y + 1);
		uchar *q1 = img1.ptr<uchar> (y);
		for (int x = 1; x<src2.cols - 1; x++)
		{
			for (int i = 0; i<3; i++)
			{
				int tmp1 = p0[3 * (x - 1) + i] - p1[3 * (x + 1) + i] + 128;
				if (tmp1<0)
					q1[3 * x + i] = 0;
				else if (tmp1>255)
					q1[3 * x + i] = 255;
				else
					q1[3 * x + i] = tmp1;
			}
		}
	}
	if (channel == 4)
		cvtColor (img1, img1, CV_BGR2BGRA);
	for (int i = 0;i < height*width * channel;i++)
		dest[i] = img1.data[i];
}

Mat Carving (Mat src2)
{
	Mat img1 = src2.clone ();
	for (int y = 1; y<src2.rows - 1; y++)
	{
		uchar *p0 = src2.ptr<uchar> (y);
		uchar *p1 = src2.ptr<uchar> (y + 1);
		uchar *q1 = img1.ptr<uchar> (y);
		for (int x = 1; x<src2.cols - 1; x++)
		{
			for (int i = 0; i<3; i++)
			{
				int tmp1 = p0[3 * (x - 1) + i] - p1[3 * (x + 1) + i] + 128;
				if (tmp1<0)
					q1[3 * x + i] = 0;
				else if (tmp1>255)
					q1[3 * x + i] = 255;
				else
					q1[3 * x + i] = tmp1;
			}
		}
	}
	return img1;
}