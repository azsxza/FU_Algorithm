#include"global.h"

//RGB通道曲线调整
Mat AdjustCurve (Mat Src, uchar (&r_lookup)[256], uchar (&g_lookup)[256], uchar (&b_lookup)[256])
{
	int width = Src.cols;
	int height = Src.rows;

	Mat Dest = Mat::zeros (Src.size (), Src.type ());
	for (int nY = 0; nY < height; nY++)
	{
		for (int nX = 0; nX < width; nX++)
		{
			Dest.at<Vec3b> (nY, nX)[2] = r_lookup[Src.at<Vec3b> (nY, nX)[2]];
			Dest.at<Vec3b> (nY, nX)[1] = r_lookup[Src.at<Vec3b> (nY, nX)[1]];
			Dest.at<Vec3b> (nY, nX)[0] = r_lookup[Src.at<Vec3b> (nY, nX)[0]];
		}
	}

	return Dest;
}


Mat PreserveLuminosityAdjustCurve (Mat I, uchar (&r_lookup)[256], uchar (&g_lookup)[256], uchar (&b_lookup)[256])
{
	int width = I.cols;
	int height = I.rows;
	int rlookup, glookup, blookup;
	int r, g, b;

	Mat dst = Mat::zeros (I.size (), I.type ());

	for (int nY = 0; nY < height; nY++)
	{
		for (int nX = 0; nX < width; nX++)
		{
			r = (int)I.at<Vec3b> (nY, nX)[2];
			g = (int)I.at<Vec3b> (nY, nX)[1];
			b = (int)I.at<Vec3b> (nY, nX)[0];

			rlookup = (int)r_lookup[r];
			glookup = (int)g_lookup[g];
			blookup = (int)b_lookup[b];

			Rgb2Hsl_Int (rlookup, glookup, blookup);
			blookup = Rgb2Hsl_L (r, g, b);
			Hsl2Rgb_Int (rlookup, glookup, blookup);


			dst.at<Vec3b> (nY, nX)[2] = (uchar)rlookup;
			dst.at<Vec3b> (nY, nX)[1] = (uchar)glookup;
			dst.at<Vec3b> (nY, nX)[0] = (uchar)blookup;
		}
	}
	return dst;
}
