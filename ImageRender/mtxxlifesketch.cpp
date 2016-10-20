#include"global.h"

Mat MtxxLifeSketch (Mat I, float opacity)
{
	Mat gray;
	cvtColor (I, gray, COLOR_BGR2GRAY);
	cvtColor (gray, gray, COLOR_GRAY2BGR);

	Mat texture = imread ("mtxx_LifeSketch_wenli2.jpg", 1);
	resize (texture, texture, I.size ());

	/// 不透明度计算
	I.convertTo (I, CV_32FC3, 1 / 255.0);
	gray.convertTo (gray, CV_32FC3, 1 / 255.0);
	texture.convertTo (texture, CV_32FC3, 1 / 255.0);

	I = (I*(100.0f - opacity) + gray*opacity) / 100.0f;
	texture = texture * opacity / 100.0f;

	/// 屏幕方式混合，f(a,b)= a + b - a*b
	Mat lifeSketch;
	Mat temp;
	multiply (I, texture, temp);
	lifeSketch = I + texture - temp;

	lifeSketch.convertTo (lifeSketch, CV_8UC3, 255.0);
	return lifeSketch;
}

void MtxxLifeSketch (uchar* Src, uchar* Dest, int Width, int Height, int Stride, float opacity)
{
	int channel = Stride / Width;
	Mat I = cv::Mat (Height, Width, CV_MAKETYPE (CV_8U, channel), Src, Stride);

	if (channel == 4)
		cvtColor (I, I, COLOR_BGRA2BGR);

	Mat dst = MtxxLifeSketch (I, opacity);

	for (int nY = 0;nY <dst.rows;nY++)
	{
		for (int nX = 0;nX<dst.cols;nX++)
		{
			Dest[nY*Stride + channel*nX] = dst.data[nY*dst.step + 3 * nX];
			Dest[nY*Stride + channel*nX + 1] = dst.data[nY*dst.step + 3 * nX + 1];
			Dest[nY*Stride + channel*nX + 2] = dst.data[nY*dst.step + 3 * nX + 2];
		}
	}
}