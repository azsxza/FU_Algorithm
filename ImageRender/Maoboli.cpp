#include"global.h"

void Maoboli (uchar* src, uchar* dest, int stride, int height, int width)
{
	int channel = stride / width;
	Mat I = cv::Mat (height, width, CV_MAKETYPE (CV_8U, channel), src, stride);

	Mat temp;
	I.convertTo (temp, CV_32F);

	Mat result_tem = temp.clone ();
	GaussianBlur (temp, result_tem, Size (5, 5), 0);

	Mat result = temp.clone ();

	int size = 3;
	float k1, k2;
	float m, n;
	int h, w;


	srand ((unsigned)time (NULL));

	for (int i = size; i < temp.rows - size; i++) {
		//float *re = result.ptr<float>(i);
		//float *re_t = result_tem.ptr<float>(i);
		for (int j = size; j < temp.cols - size; j++) {

			k1 = (float)((rand () % 100)) / 100.0 - 0.5;
			k2 = (float)((rand () % 100)) / 100.0 - 0.5;

			m = (k1*(size * 2 - 1));
			n = (k2*(size * 2 - 1));

			h = (int)(i + m) % temp.rows;
			w = (int)(j + n) % temp.cols;

			result.at<Vec3f> (i, j) = result_tem.at<Vec3f> (h, w);

		}
	}

	result.convertTo (result, CV_8U);
	for (int i = 0;i < height*width * channel;i++)
		dest[i] = result.data[i];

}

Mat Maoboli (Mat I)
{
	Mat temp;
	I.convertTo (temp, CV_32F);

	Mat result_tem = temp.clone ();
	GaussianBlur (temp, result_tem, Size (5, 5), 0);

	Mat result = temp.clone ();

	int size = 3;
	float k1, k2;
	float m, n;
	int h, w;


	srand ((unsigned)time (NULL));

	for (int i = size; i < temp.rows - size; i++) {
		//float *re = result.ptr<float>(i);
		//float *re_t = result_tem.ptr<float>(i);
		for (int j = size; j < temp.cols - size; j++) {

			k1 = (float)((rand () % 100)) / 100.0 - 0.5;
			k2 = (float)((rand () % 100)) / 100.0 - 0.5;

			m = (k1*(size * 2 - 1));
			n = (k2*(size * 2 - 1));

			h = (int)(i + m) % temp.rows;
			w = (int)(j + n) % temp.cols;

			result.at<Vec3f> (i, j) = result_tem.at<Vec3f> (h, w);

		}
	}

	result.convertTo (result, CV_8U);
	return result;
}