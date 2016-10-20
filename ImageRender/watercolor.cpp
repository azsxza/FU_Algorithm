#include"global.h"

const int Arraysize = 256;

Mat watercolourTone (Mat & img, int brush_w, int brush_h)
{
	Mat gray;
	cvtColor (img, gray, CV_BGR2GRAY);
	Mat result = Mat::zeros (img.size (), img.type ());
	int hight = img.rows;
	int width = img.cols;

	for (int i = 0; i < hight; i = i + 3)
	{
		int Top = i - brush_h;
		int Bottom = i + brush_h + 1;
		if (Bottom > hight - 1)
			Bottom = hight - 1;
		if (Top < 0)
			Top = 0;
		uchar *pp = result.ptr<uchar> (i);
		for (int j = 0; j < width; j = j + 3)
		{
			int left = j - brush_w;
			int right = j + brush_w + 1;
			if (left < 0)
				left = 0;
			if (right > width - 1)
				right = width - 1;

			int cum[Arraysize] = { 0 };
			uint B[Arraysize] = { 0 };
			uint G[Arraysize] = { 0 };
			uint R[Arraysize] = { 0 };

			for (int i = Top; i < Bottom; i += 10)
			{
				uchar *p = img.ptr<uchar> (i);
				for (int j = left; j < right; j += 10)
				{
					uchar intensity = static_cast<uchar>(255 * gray.at<uchar> (i, j) / 255.0);
					cum[intensity]++;
					B[intensity] += p[j * 3 + 0];
					G[intensity] += p[j * 3 + 1];
					R[intensity] += p[j * 3 + 2];
				}
			}

			uchar remmber;
			int maxvall = cum[0];
			for (int i = 1; i < Arraysize; i++)
			{
				if (maxvall < cum[i])
				{
					maxvall = cum[i];
					remmber = (uchar)i;
				}
			}

			pp[j * 3 + 0] = B[remmber] / maxvall;
			pp[j * 3 + 1] = G[remmber] / maxvall;
			pp[j * 3 + 2] = R[remmber] / maxvall;
		}
	}
	
	return result;
}
Mat WaterColor (Mat img, int brushSize_w, int brushSize_h)
{
	GaussianBlur (img, img, Size (7, 7), 0, 0);
	Mat result;

	Mat i = watercolourTone (img, brushSize_w, brushSize_h);

	Mat elemet = getStructuringElement (MORPH_ELLIPSE, Size (12, 22));
	dilate (i, result, elemet);
	erode (result, result, elemet);
	return result;
}

void WaterColor (uchar* Src, uchar* Dest, int Width, int Height, int brushSize_w, int brushSize_h,int stride)
{
	int channel = stride / Width;
	Mat img = cv::Mat (Height, Width, CV_MAKETYPE (CV_8U, channel), Src, stride);
	/*GaussianBlur (img, img, Size (7, 7), 0, 0);
	Mat result;

	Mat i = watercolourTone (img, brushSize_w, brushSize_h);

	Mat elemet = getStructuringElement (MORPH_ELLIPSE, Size (12, 22));
	dilate (i, result, elemet);
	erode (result, result, elemet);*/
	Mat result = WaterColor (img, brushSize_w, brushSize_h);

	for (int i = 0;i < result.cols*result.rows * 3;i++)
		Dest[i] = result.data[i];
}