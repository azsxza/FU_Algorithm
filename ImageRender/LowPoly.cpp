#include"global.h"

void draw_subdiv (Mat &img, Subdiv2D subdiv, Mat color, int index)
{
	bool draw;
	vector<Vec6f> triangleList;
	subdiv.getTriangleList (triangleList);
	cout << triangleList.size () << endl;
	vector<Point> pt (3);
	Scalar sc;
	for (size_t i = 0; i < triangleList.size (); ++i)
	{
		Vec6f t = triangleList[i];
		pt[0] = Point (cvRound (t[0]), cvRound (t[1]));
		pt[1] = Point (cvRound (t[2]), cvRound (t[3]));
		pt[2] = Point (cvRound (t[4]), cvRound (t[5]));

		draw = true;
		for (int i = 0;i<3;i++)
		{
			if (pt[i].x>img.cols || pt[i].y>img.rows || pt[i].x<0 || pt[i].y<0)
				draw = false;
		}
		if (draw)
		{
			Point pc ((pt[0].x + pt[1].x + pt[2].x) / 3, (pt[0].y + pt[1].y + pt[2].y) / 3);
			sc = Scalar (color.at<Vec3b> (pc)[0], color.at<Vec3b> (pc)[1], color.at<Vec3b> (pc)[2]);
			line (img, pt[0], pt[1], sc, 1);
			line (img, pt[1], pt[2], sc, 1);
			line (img, pt[2], pt[0], sc, 1);
			if (index == 1)
				fillConvexPoly (img, pt, sc);
		}
	}

	if (index == 0)
	{
		GaussianBlur (img, img, Size (3, 3), 0);
		for (size_t i = 0; i < triangleList.size (); ++i)
		{
			Vec6f t = triangleList[i];
			pt[0] = Point (cvRound (t[0]), cvRound (t[1]));
			pt[1] = Point (cvRound (t[2]), cvRound (t[3]));
			pt[2] = Point (cvRound (t[4]), cvRound (t[5]));

			draw = true;
			for (int i = 0;i < 3;i++)
			{
				if (pt[i].x > img.cols || pt[i].y>img.rows || pt[i].x < 0 || pt[i].y < 0)
					draw = false;
			}
			if (draw)
			{
				Point pc ((pt[0].x + pt[1].x + pt[2].x) / 3, (pt[0].y + pt[1].y + pt[2].y) / 3);
				sc = Scalar (color.at<Vec3b> (pc)[0], color.at<Vec3b> (pc)[1], color.at<Vec3b> (pc)[2]);
				floodFill (img, pc, sc);
			}
		}
	}
}

Mat Lowpoly (Mat aaa, int index)
{
	Mat Ta;
	int Tw = aaa.size ().width*1.2, Th = aaa.size ().height*1.2;
	resize (aaa, Ta, Size (Tw, Th));

	for (int nY = (Th - aaa.rows) / 2;nY < aaa.rows + (Th - aaa.rows) / 2;nY++)
	{
		for (int nX = (Tw - aaa.cols) / 2;nX < aaa.cols + (Tw - aaa.cols) / 2;nX++)
		{
			Ta.at<Vec3b> (nY, nX)[0] = aaa.at<Vec3b> (nY - (Th - aaa.rows) / 2, nX - (Tw - aaa.cols) / 2)[0];
			Ta.at<Vec3b> (nY, nX)[1] = aaa.at<Vec3b> (nY - (Th - aaa.rows) / 2, nX - (Tw - aaa.cols) / 2)[1];
			Ta.at<Vec3b> (nY, nX)[2] = aaa.at<Vec3b> (nY - (Th - aaa.rows) / 2, nX - (Tw - aaa.cols) / 2)[2];
		}
	}

	GaussianBlur (Ta, Ta, Size (3, 3), 0);
	Mat a;
	cvtColor (Ta, a, CV_RGB2GRAY);
	Mat ax, ay;
	Mat axx, ayy;
	Sobel (a, ax, CV_16S, 1, 0);
	Sobel (a, ay, CV_16S, 0, 1);
	convertScaleAbs (ax, axx);
	convertScaleAbs (ay, ayy);
	a = axx + ayy;

	Rect rect (0, 0, a.cols, a.rows);
	Subdiv2D sub (rect);

	uchar* p = a.ptr (0);
	int t;
	for (int i = 0;i < a.cols*a.rows;i++)
	{
		Point tmp (i%a.cols, i / a.cols);
		t = rand () % 900;
		if (p[i] > 190 && (t >= 2 && t <= 25))
		{
			sub.insert (tmp);
		}
		if (t == 1)
		{
			sub.insert (tmp);
		}
	}

	Mat c (a.size (), CV_8UC3, Scalar (0, 0, 0));
	draw_subdiv (c, sub, Ta, index);

	Mat dest = aaa.clone ();
	for (int nY = (Th - aaa.rows) / 2;nY < aaa.rows + (Th - aaa.rows) / 2;nY++)
	{
		for (int nX = (Tw - aaa.cols) / 2;nX < aaa.cols + (Tw - aaa.cols) / 2;nX++)
		{
			dest.at<Vec3b> (nY - (Th - aaa.rows) / 2, nX - (Tw - aaa.cols) / 2)[0] = c.at<Vec3b> (nY, nX)[0];
			dest.at<Vec3b> (nY - (Th - aaa.rows) / 2, nX - (Tw - aaa.cols) / 2)[1] = c.at<Vec3b> (nY, nX)[1];
			dest.at<Vec3b> (nY - (Th - aaa.rows) / 2, nX - (Tw - aaa.cols) / 2)[2] = c.at<Vec3b> (nY, nX)[2];
		}
	}
	return dest;
}

void Lowpoly (uchar* src, uchar*dest, int index, int width, int height)
{
	Mat I (height, width, CV_8UC3, src);
	Mat T = Lowpoly (I, index);
	for (int i = 0;i < width*height * 3;i++)
		dest[i] = T.data[i];
}
