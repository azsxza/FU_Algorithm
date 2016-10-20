#include"global.h"

Mat shan (Mat a, double ddd)
{
	int dd = a.cols*ddd;
	vector<Mat> ss;
	split (a, ss);
	vector<Mat> b;
	b.push_back (ss[0]);
	b.push_back (Mat (a.size (), CV_8UC1, Scalar (0)));
	b.push_back (Mat (a.size (), CV_8UC1, Scalar (0)));

	vector<Mat> g;
	g.push_back (Mat (a.size (), CV_8UC1, Scalar (0)));
	g.push_back (ss[1]);
	g.push_back (Mat (a.size (), CV_8UC1, Scalar (0)));

	vector<Mat> r;
	r.push_back (Mat (a.size (), CV_8UC1, Scalar (0)));
	r.push_back (Mat (a.size (), CV_8UC1, Scalar (0)));
	r.push_back (ss[2]);
	Mat B, G, R;
	merge (b, B);
	merge (g, G);
	merge (r, R);
	for (int i = 0;i < a.rows;i++)
	{
		for (int j = 0;j < a.cols;j++)
		{
			if (j < a.cols - dd)
			{
				G.at<Vec3b> (i, j) += B.at<Vec3b> (i, j + dd);
			}
			if (j > dd)
			{
				G.at<Vec3b> (i, j) += R.at<Vec3b> (i, j - dd);
			}
		}
	}
	return G;
}

void shan (uchar* src, uchar* dest, double ddd, int height, int width)
{

	Mat I (height, width, CV_8UC3, src);

	Mat T = shan (I, ddd);
	for (int i = 0;i < width*height * 3;i++)
		dest[i] = T.data[i];
}