#include"global.h"

Mat OilPaint (const Mat& I, int brushSize, int coarseness)
{
	CV_Assert (!I.empty ());

	if (brushSize < 1) brushSize = 1;
	if (brushSize > 8) brushSize = 8;

	if (coarseness < 1) coarseness = 1;
	if (coarseness > 255) coarseness = 255;

	int width = I.cols;
	int height = I.rows;

	int lenArray = coarseness + 1;
	int* CountIntensity = new int[lenArray];
	uint* RedAverage = new uint[lenArray];
	uint* GreenAverage = new uint[lenArray];
	uint* BlueAverage = new uint[lenArray];

	/// 图像灰度化
	Mat gray;
	cvtColor (I, gray, COLOR_BGR2GRAY);


	/// 目标图像
	Mat dst = Mat::zeros (I.size (), I.type ());

	for (int nY = 0;nY <height; nY++)
	{
		// 油画渲染范围上下边界
		int top = nY - brushSize;
		int bottom = nY + brushSize + 1;

		if (top<0) top = 0;
		if (bottom >= height) bottom = height - 1;

		for (int nX = 0;nX<width;nX++)
		{
			// 油画渲染范围左右边界
			int left = nX - brushSize;
			int right = nX + brushSize + 1;

			if (left<0) left = 0;
			if (right >= width) right = width - 1;

			//初始化数组
			for (int i = 0;i <lenArray;i++)
			{
				CountIntensity[i] = 0;
				RedAverage[i] = 0;
				GreenAverage[i] = 0;
				BlueAverage[i] = 0;
			}


			// 下面这个内循环类似于外面的大循环
			// 也是油画特效处理的关键部分
			for (int j = top;j<bottom;j++)
			{
				for (int i = left;i<right;i++)
				{
					uchar intensity = static_cast<uchar>(coarseness*gray.at<uchar> (j, i) / 255.0);
					CountIntensity[intensity]++;

					RedAverage[intensity] += I.at<Vec3b> (j, i)[2];
					GreenAverage[intensity] += I.at<Vec3b> (j, i)[1];
					BlueAverage[intensity] += I.at<Vec3b> (j, i)[0];
				}
			}

			// 求最大值，并记录下数组索引
			uchar chosenIntensity = 0;
			int maxInstance = CountIntensity[0];
			for (int i = 1;i<lenArray;i++)
			{
				if (CountIntensity[i]>maxInstance)
				{
					chosenIntensity = (uchar)i;
					maxInstance = CountIntensity[i];
				}
			}

			dst.at<Vec3b> (nY, nX)[2] = saturate_cast<uchar>(RedAverage[chosenIntensity] / static_cast<float>(maxInstance));
			dst.at<Vec3b> (nY, nX)[1] = saturate_cast<uchar>(GreenAverage[chosenIntensity] / static_cast<float>(maxInstance));
			dst.at<Vec3b> (nY, nX)[0] = saturate_cast<uchar>(BlueAverage[chosenIntensity] / static_cast<float>(maxInstance));
		}

	}

	delete[] CountIntensity;
	delete[] RedAverage;
	delete[] GreenAverage;
	delete[] BlueAverage;

#ifdef _DEBUG
	imshow ("dst", dst);
	waitKey ();
#endif

	return dst;
}


void OilPaint (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int brushSize, int coarseness)
{
	int channel = Stride / Width;
	Mat I = cv::Mat (Height, Width, CV_MAKETYPE (CV_8U, channel), Src, Stride);

	if (channel == 4)
		cvtColor (I, I, COLOR_BGRA2BGR);

	Mat OilI = OilPaint (I, brushSize, coarseness);

	for (int nY = 0;nY <OilI.rows;nY++)
	{
		for (int nX = 0;nX<OilI.cols;nX++)
		{
			Dest[nY*Stride + channel*nX] = OilI.data[nY*OilI.step + 3 * nX];
			Dest[nY*Stride + channel*nX + 1] = OilI.data[nY*OilI.step + 3 * nX + 1];
			Dest[nY*Stride + channel*nX + 2] = OilI.data[nY*OilI.step + 3 * nX + 2];
		}
	}
}