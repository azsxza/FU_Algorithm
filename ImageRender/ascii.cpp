#include"global.h"

Mat ascii (Mat aaa)
{
	Mat a = aaa.clone ();
	Size az = a.size ();
	resize (a, a, a.size () / 10);
	resize (a, a, az, 0, 0, 3);
	Mat b (a.size (), CV_8UC3, Scalar (255, 255, 255));
	for (int j = 0;j < a.rows / 10;j++)
		for (int i = 0;i < a.cols / 10;i++)
		{
			int t = rand () % 75 + 48;
			string c = "";
			c += (char)t;
			putText (b, c, Point (i * 10, j * 10 + 7), CV_FONT_HERSHEY_DUPLEX, 0.4, a.at<Vec3b> (j * 10, i * 10), 2);
		}
	return b;
}

void ascii (uchar* src, uchar*dest, int width, int height)
{
	Mat I (height, width, CV_8UC3, src);
	Mat T = ascii (I);
	for (int i = 0;i < width*height * 3;i++)
		dest[i] = T.data[i];
	
}
