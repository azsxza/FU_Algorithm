#include"global.h"

void draw_subdiv (Mat &img, Subdiv2D subdiv, Mat color)
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
			for (int i = 0;i < 3;i++)
			{
				circle (img, pt[i], 1, sc, 2);
			}
		}
	}
}

Mat Line (Mat aaa)
{
	Mat gray;
	cvtColor (aaa, gray, CV_BGR2GRAY);
	Rect rect (0, 0, aaa.cols, aaa.rows);
	Subdiv2D sub (rect);
	for (int i = 0;i < gray.cols*gray.rows;i++)
	{
		if (gray.data[i] > 10)
		{
			if (rand () % 85 == 1)
				sub.insert (Point (i%gray.cols, i / gray.cols));
		}
	}


	Mat c (aaa.size (), CV_8UC3, Scalar (0, 0, 0));
	draw_subdiv (c, sub, aaa);
	return c;
}

void Line (uchar* src, uchar*dest, int width, int height)
{
	Mat I (height, width, CV_8UC3, src);
	Mat T = Line (I);
	for (int i = 0;i < width*height * 3;i++)
		dest[i] = T.data[i];
}
