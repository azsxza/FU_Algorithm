#include "global.h"

Mat CoherenceFilter (const Mat& img, int sigma, int str_sigma, float blend, int iter)
{
	Mat I = img.clone ();
	int height = I.rows;
	int width = I.cols;

	for (int i = 0;i <iter; i++)
	{
		Mat gray;
		cvtColor (I, gray, COLOR_BGR2GRAY);
		Mat eigen;
		cornerEigenValsAndVecs (gray, eigen, str_sigma, 3);

		std::vector<Mat> vec;
		split (eigen, vec);

		Mat x, y;
		x = vec[2];
		y = vec[3];

		Mat gxx, gxy, gyy;
		Sobel (gray, gxx, CV_32F, 2, 0, sigma);
		Sobel (gray, gxy, CV_32F, 1, 1, sigma);
		Sobel (gray, gyy, CV_32F, 0, 2, sigma);

		Mat ero;
		Mat dil;
		erode (I, ero, Mat ());
		dilate (I, dil, Mat ());

		Mat img1 = ero;
		for (int nY = 0;nY<height;nY++)
		{
			for (int nX = 0;nX<width;nX++)
			{
				if (x.at<float> (nY, nX)* x.at<float> (nY, nX)* gxx.at<float> (nY, nX)
					+ 2 * x.at<float> (nY, nX)* y.at<float> (nY, nX)* gxy.at<float> (nY, nX)
					+ y.at<float> (nY, nX)* y.at<float> (nY, nX)* gyy.at<float> (nY, nX)<0)
				{
					img1.at<Vec3b> (nY, nX) = dil.at<Vec3b> (nY, nX);
				}
			}
		}
		I = I*(1.0 - blend) + img1*blend;
	}
	return I;
}

void CoherenceFilter (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int sigma, int str_sigma, float blend, int iter)
{
	int channel = Stride / Width;
	Mat I = cv::Mat (Height, Width, CV_MAKETYPE (CV_8U, channel), Src, Stride);

	if (channel == 4)
		cvtColor (I, I, COLOR_BGRA2BGR);

	Mat dst = CoherenceFilter (I, sigma, str_sigma, blend, iter);

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
