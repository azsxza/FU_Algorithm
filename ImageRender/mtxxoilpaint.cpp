#include"global.h"

Mat MtxxOilPaint (Mat I, float opacity)
{
	Mat texture = imread ("mtxx_yhwl.jpg", 1);
	resize (texture, texture, I.size ());

	/// 转换格式
	I.convertTo (I, CV_32FC3, 1 / 255.0);
	texture.convertTo (texture, CV_32FC3, 1 / 255.0);

	/// 叠加方式混合，f(a,b)= a + b - a*b
	Mat oilPaint;
	oilPaint = I.clone ();

	for (int nY = 0;nY<texture.rows;nY++)
	{
		for (int nX = 0;nX<texture.cols;nX++)
		{
			oilPaint.at<Vec3f> (nY, nX)[0] = oilPaint.at<Vec3f> (nY, nX)[0] < 0.5 ? 2 * oilPaint.at<Vec3f> (nY, nX)[0] * texture.at<Vec3f> (nY, nX)[0] : 1 - 2 * (1 - oilPaint.at<Vec3f> (nY, nX)[0])*(1 - texture.at<Vec3f> (nY, nX)[0]);
			oilPaint.at<Vec3f> (nY, nX)[1] = oilPaint.at<Vec3f> (nY, nX)[1] < 0.5 ? 2 * oilPaint.at<Vec3f> (nY, nX)[1] * texture.at<Vec3f> (nY, nX)[1] : 1 - 2 * (1 - oilPaint.at<Vec3f> (nY, nX)[1])*(1 - texture.at<Vec3f> (nY, nX)[1]);
			oilPaint.at<Vec3f> (nY, nX)[2] = oilPaint.at<Vec3f> (nY, nX)[2] < 0.5 ? 2 * oilPaint.at<Vec3f> (nY, nX)[2] * texture.at<Vec3f> (nY, nX)[2] : 1 - 2 * (1 - oilPaint.at<Vec3f> (nY, nX)[2])*(1 - texture.at<Vec3f> (nY, nX)[2]);
		}
	}

	//不透明度计算
	oilPaint = I*(100 - opacity) / 100.0f + oilPaint*opacity / 100.0f;

	oilPaint.convertTo (oilPaint, CV_8UC3, 255.0);
	return oilPaint;
}

void MtxxOilPaint (uchar* Src, uchar* Dest, int Width, int Height, int Stride, float opacity)
{
	int channel = Stride / Width;
	Mat I = cv::Mat (Height, Width, CV_MAKETYPE (CV_8U, channel), Src, Stride);

	if (channel == 4)
		cvtColor (I, I, COLOR_BGRA2BGR);

	Mat dst = MtxxOilPaint (I, opacity);

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