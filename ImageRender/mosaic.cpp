#include"global.h"

void Mosaic (uchar* src, uchar* dest, int stride, int height, int width, int size)
{
	int channel = stride / width;
	Mat I = cv::Mat (height, width, CV_MAKETYPE (CV_8U, channel), src, stride);

	Mat a = I.clone ();
	Size aaz = a.size ();
	resize (a, a, Size (a.size ().width / 10, a.size ().height / 10));
	resize (a, a, aaz, 0, 0, 3);
	for (int i = 0;i < height*width * channel;i++)
		dest[i] = a.data[i];
}

Mat Mosaic (Mat I, int size)
{
	Mat a = I.clone ();
	Size aaz = a.size ();
	resize (a, a, Size (a.size ().width / 10, a.size ().height / 10));
	resize (a, a, aaz, 0, 0, 3);
	return a;
}