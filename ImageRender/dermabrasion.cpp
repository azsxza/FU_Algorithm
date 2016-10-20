#include"global.h"

Mat Dermabrasion (Mat I, int ks, int opacity, int white)
{
	assert (!I.empty () && I.channels () == 3);

	I.convertTo (I, CV_32F);

	Mat smooth_layer, gauss_layer;
	float value = 128.0f;

	edgePreservingFilter (I, smooth_layer, 1, 30);
	smooth_layer.convertTo (smooth_layer, CV_32F);

	smooth_layer = smooth_layer - I + value;

	GaussianBlur (smooth_layer, gauss_layer, Size (ks, ks), 0);


	Mat result = (I*(100 - opacity) + (I + 2 * gauss_layer - 255)*opacity) / 100.0f;

	result.convertTo (result, CV_8U);

	result = BalanceColor (result, white, white, white, TRANSFERMODE_MIDTONES, false);
#ifdef _DEBUG
	//Mat beforeafter = Mat::zeros(result.rows, result.cols*2, CV_8UC3);
	//Rect roil (0, 0, beforeafter.cols/2, beforeafter.rows);
	//Rect roir (beforeafter.cols/2, 0, beforeafter.cols/2, beforeafter.rows);
	//source.copyTo(beforeafter(roil));
	//result.copyTo(beforeafter(roir));
	//imshow("Dermabrasion",beforeafter);  
	//waitKey();
#endif

	return result;
}

void Dermabrasion (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int ks, int opacity, int white)
{
	int channel = Stride / Width;
	Mat I = cv::Mat (Height, Width, CV_MAKETYPE (CV_8U, channel), Src, Stride);

	if (channel == 4)
		cvtColor (I, I, COLOR_BGRA2BGR);

	Mat dst = Dermabrasion (I, ks, opacity, white);

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