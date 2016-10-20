#include"global.h"

Mat fenbi (Mat I)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat a;
	Canny (I, a, 100, 300);
	findContours (a, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	Mat b = Mat (a.size (), CV_8UC1, Scalar (255));

	for (int i = 0; i< contours.size (); i++)
	{
		drawContours (b, contours, i, Scalar (0), 2, 8, hierarchy, 0);
	}

	Mat noise (b.size (), b.type ());
	randn (noise, 300, 200);
	GaussianBlur (noise, noise, cv::Size (3, 3), 0.5);

	Mat letter = noise - b;

	Mat colorLetter;
	cvtColor (letter, colorLetter, cv::COLOR_GRAY2BGR);

	Mat backColor (b.size (), CV_8UC3, cv::Scalar (0, 40, 0));

	return backColor + colorLetter;
}

void Fenbi (uchar* src, uchar* dest, int height, int width)
{
	Mat I (height, width, CV_8UC3, src);
	Mat T = fenbi (I);
	for (int i = 0;i < width*height * 3;i++)
		dest[i] = T.data[i];
}