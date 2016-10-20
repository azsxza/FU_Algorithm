#include"global.h"

typedef struct __THREAD_DATA
{
	Mat src;
	Mat p;
}THREAD_DATA;

THREAD_DATA td;
Mat aa = imread ("texture1.jpg");

Mat RotateImage (const Mat source, double angle, int border)
{
	Mat bordered_source;
	int top, bottom, left, right;
	top = bottom = left = right = border;
	copyMakeBorder (source, bordered_source, top, bottom, left, right, BORDER_CONSTANT, cv::Scalar ());
	Point2f src_center (bordered_source.cols / 2.0F, bordered_source.rows / 2.0F);
	Mat rot_mat = getRotationMatrix2D (src_center, angle, 1.0);
	Mat dst;
	warpAffine (bordered_source, dst, rot_mat, bordered_source.size ());
	return dst;
}

Mat GenStroke (const Mat I, int ks, int dirNum, double gamma)
{
	Mat smoothI;
	GaussianBlur (I, smoothI, Size (3, 3), 0);
	int i, j;

	Mat imX = Mat::zeros (I.size (), CV_8UC1);
	Mat imY = Mat::zeros (I.size (), CV_8UC1);
	Mat imEdge = Mat::zeros (I.size (), CV_8UC1);


	uchar* imXptr = imX.ptr<uchar> (0);
	uchar* imYptr = imY.ptr<uchar> (0);
	uchar* imEdgeptr = imEdge.ptr<uchar> (0);
	uchar* smoothIptr = smoothI.ptr<uchar> (0);
	for (i = 1; i < imX.rows - 1; i++)
	{
		for (j = 1; j < imX.cols - 1; j++)
		{
			imXptr[i* imX.step + j] = abs (smoothIptr[i* imX.step + j] - smoothIptr[i* imX.step + j - 1]);
			imYptr[i* imX.step + j] = abs (smoothIptr[i* imX.step + j] - smoothIptr[(i - 1)* imX.step + j]);
			imEdgeptr[i* imX.step + j] = imXptr[i* imX.step + j] + imYptr[i* imX.step + j];
		}
	}
	if (ks <= 0)
		ks = cvRound (min (I.rows, I.cols) / 100.0);

	/// convolution kernel with horizontal direction 
	Mat kerRef = Mat::zeros (Size (ks * 2 + 1, ks * 2 + 1), CV_32F);
	kerRef.row (ks) = 1;
	kerRef.row (ks - 1) = 1;
	kerRef.row (ks + 1) = 1;

	vector<Mat> response (8);

	Mat kef;
	vector<float*> responseptr (8);

	for (i = 0;i<dirNum;i++)
	{
		kef = RotateImage (kerRef, i*180.0f / dirNum,20);
		filter2D (imEdge, response[i], CV_32F, kef);
		responseptr[i] = response[i].ptr<float> (0);
	}

	int index;
	double maxvalue;
	for (i = 0; i < imEdge.rows * imEdge.cols; i++)
	{
		index = 0;
		maxvalue = -1;
		/// 最大卷积方向
		for (j = 0; j < dirNum;j++)
		{
			if (maxvalue < responseptr[j][i])
			{
				maxvalue = responseptr[j][i];
				index = j;
			}
		}
		for (j = 0; j < dirNum;j++)
		{
			if (j == index)
				responseptr[j][i] = imEdgeptr[i];
			else
				responseptr[j][i] = 0;
		}
	}

	Mat sp = Mat::zeros (I.size (), CV_32F);
	for (i = 0;i<dirNum;i++)
	{
		sp += response[i];
	}

	normalize (sp, sp, 0.0, 1.0, NORM_MINMAX);
	sp = 1.0 - sp;

	pow (sp, gamma, sp);

	return sp;
}


Mat NaturalHistogramMatching (Mat I, int Ua = 100, int Ub = 255, int Mud = 90, int DeltaB = 9, int DeltaD = 11, int Omega1 = 76, int Omega2 = 22, int Omega3 = 2)
{
	/// a little blur
	GaussianBlur (I, I, Size (7, 7), 0, 0);
	int i;
	double histF[256];
	double histCF[256];

	double K = 0; //normalization factor
	float value;
	for (i = 0; i < 256; i++)
	{
		if (i<Ua || i>Ub)
			value = 0.0f;
		else
			value = 1.0f / (Ub - Ua);

		histF[i] = (Omega2*value + 1.0f / DeltaB*exp (-(255.0 - i) / DeltaB)*Omega1 + 1.0f / sqrt (2 * CV_PI*DeltaD)*exp (-(i - Mud)*(i - Mud) / (2.0f*DeltaD*DeltaD))*Omega3)* 0.01f;
		K = K + histF[i];
	}

	/// normalization and cumsum histogram of function
	double cumsum = 0;
	for (i = 0; i < 256; i++)
	{
		histF[i] = histF[i] / K;
		cumsum = cumsum + histF[i];
		histCF[i] = cumsum;
	}

	double histI[256] = { 0 };
	double histCI[256] = { 0 };
	int matchingFun[256] = {};

	uchar* Iptr = I.ptr<uchar> (0);
	/// histogram of origin image
	for (i = 0; i < I.rows*I.cols; i++)
	{
		histI[Iptr[i]]++;
	}
	float imsz = static_cast<float>(I.rows*I.cols);

	/// normalization and cumsum histogram of origin image
	cumsum = 0;
	for (i = 0; i < 256; i++)
	{
		histI[i] = histI[i] / imsz;
		cumsum = cumsum + histI[i];
		histCI[i] = cumsum;
	}

	Mat Iadjusted = Mat::zeros (I.size (), CV_32F);
	float* Iadjustedptr = Iadjusted.ptr<float> (0);
	double minvalue;
	int index;
	for (i = 0;i <= 255;i++)
	{
		minvalue = abs (histCI[i] - histCF[0]);
		index = 0;
		for (int j = 1;j <= 255;j++)
		{
			if (minvalue> abs (histCI[i] - histCF[j]))
			{
				minvalue = abs (histCI[i] - histCF[j]);
				index = j;
			}
		}
		matchingFun[i] = index;
	}

	/// do the histogram matching
	for (i = 0; i<I.rows*I.cols;i++)
	{
		Iadjustedptr[i] = matchingFun[Iptr[i]] / 255.0f;
	}

	return Iadjusted;
}

Mat BlendImage (Mat I1, Mat I2)
{
	if (I1.type () != CV_32F)
		I1.convertTo (I1, CV_32F, 1.0 / 255.0);

	if (I2.type () != CV_32F)
		I2.convertTo (I2, CV_32F, 1.0 / 255.0);

	Mat dst = Mat::zeros (I1.size (), CV_32F);
	float* I1ptr = I1.ptr<float> (0);
	float* I2ptr = I2.ptr<float> (0);
	float* dstptr = dst.ptr<float> (0);

	for (int i = 0; i<I1.rows*I1.cols;i++)
	{
		dstptr[i] = I1ptr[i] * I2ptr[i];
	}
	dst.convertTo (dst, CV_8U, 255.0);
	return dst;
}


Mat HorizontalStitch (Mat I, int width)
{
	Mat Istitched = I;
	int i;
	int winsz = cvRound (I.cols / 4);
	while (Istitched.cols<width)
	{
		Mat left = I (Range::all (), Range (I.cols - winsz, I.cols));
		Mat right = I (Range::all (), Range (0, winsz));

		Mat aleft = Mat::zeros (left.rows, winsz, CV_32FC1);
		Mat aright = Mat::zeros (left.rows, winsz, CV_32FC1);

		for (i = 0;i<winsz;i++)
		{
			aleft.col (i) = left.col (i)*(1 - (i + 1) / winsz);
			aright.col (i) = right.col (i)*((i + 1) / winsz);
		}

		Mat lr = aleft + aright;

		Mat temp = Mat::zeros (Istitched.rows, 2 * Istitched.cols - winsz, CV_32F);
		Rect roil (0, 0, Istitched.cols - winsz, Istitched.rows);
		Rect roim (Istitched.cols - winsz, 0, winsz, Istitched.rows);
		Rect roir (Istitched.cols, 0, Istitched.cols - winsz, Istitched.rows);

		Mat  tail = Istitched (Range::all (), Range (winsz, Istitched.cols));

		Istitched (roil).copyTo (temp (roil));
		lr.copyTo (temp (roim));
		tail.copyTo (temp (roir));

		Istitched = temp;
	}

	Istitched = Istitched (Range::all (), Range (0, width));
	return Istitched;
}

Mat VerticalStitch (Mat I, int hei)
{
	Mat Istitched = I;
	int i;
	int winsz = cvRound (I.rows / 4);
	while (Istitched.rows<hei)
	{
		Mat up = I (Range (I.rows - winsz, I.rows), Range::all ());
		Mat down = I (Range (0, winsz), Range::all ());

		Mat aup = Mat::zeros (winsz, up.cols, CV_32FC1);
		Mat adown = Mat::zeros (winsz, down.cols, CV_32FC1);

		for (i = 0;i<winsz;i++)
		{
			aup.row (i) = up.row (i)*(1 - (i + 1) / winsz);
			adown.row (i) = up.row (i)*((i + 1) / winsz);
		}

		Mat ud = aup + adown;
		Mat temp = Mat::zeros (2 * Istitched.rows - winsz, Istitched.cols, CV_32F);
		Rect roiu (0, 0, Istitched.cols, Istitched.rows - winsz);
		Rect roim (0, Istitched.rows - winsz, Istitched.cols, winsz);
		Rect roid (0, Istitched.rows, Istitched.cols, Istitched.rows - winsz);

		Mat  tail = Istitched (Range (winsz, Istitched.rows), Range::all ());

		Istitched (roiu).copyTo (temp (roiu));
		ud.copyTo (temp (roim));
		tail.copyTo (temp (roid));

		Istitched = temp;
	}

	Istitched = Istitched (Range (0, hei), Range::all ());
	return Istitched;
}

Mat fft2 (const Mat& I, Size size)
{
	Mat If = Mat::zeros (I.size (), I.type ());

	Size dftSize;

	dftSize = size;

	// allocate temporary buffers and initialize them with 0's
	Mat tempI (dftSize, I.type (), Scalar::all (0));

	//copy I to the top-left corners of temp
	Mat roiI (tempI, Rect (0, 0, I.cols, I.rows));
	I.copyTo (roiI);

	// now transform the padded A & B in-place;
	// use "nonzeroRows" hint for faster processing
	if (I.channels () == 1)
	{
		dft (tempI, If, DFT_COMPLEX_OUTPUT);
	}
	else
	{
		vector<Mat> channels;
		split (tempI, channels);
		for (int n = 0;n<I.channels ();n++)
		{
			dft (channels[n], channels[n], DFT_COMPLEX_OUTPUT);
		}

		merge (channels, If);
	}

	return If (Range (0, size.height), Range (0, size.width));
}

Mat TextureRendering (const Mat I, Mat P)
{
	Mat Cut = P (Range (101, P.rows - 100), Range (101, P.cols - 100));
	resize (Cut, Cut, Size (), 0.8f*min (P.rows, P.cols) / 1024.0f, 0.8f*min (P.rows, P.cols) / 1024.0f);
	Cut.convertTo (Cut, CV_32F, 1.0 / 255.0);

	Mat texture = VerticalStitch (HorizontalStitch (Cut, I.cols), I.rows);

	Mat tone = I.clone ();
	/// 惯例，先转换为double型
	if (tone.type () != CV_32F)
		tone.convertTo (tone, CV_32F, 1.0 / 255.0);
	tone = 1.0 - tone;

	/// 转换到对数域
	Mat logtone, logtexture;
	log (tone + 1.0, logtone);
	log (texture + 1.0, logtexture);

	Mat B = logtone / logtexture;

	Mat planes[] = { Mat_<float> (B), Mat::zeros (B.size (), CV_32F) };
	Mat Bf;
	merge (planes, 2, Bf);

	/// 傅里叶变换
	dft (Bf, Bf);

	float l[3][3] = { { 0, -1, 0 },{ -1, 4, -1 },{ 0, -1, 0 } };
	Mat L (3, 3, CV_32F, l);
	Mat Lf;

	Lf = fft2 (L, Size (I.cols, I.rows));

	Mat Lf2;
	pow (Lf, 2, Lf2);

	Mat betaf = Bf / (1 + 0.05f*Lf2);

	Mat beta;
	idft (betaf, beta, DFT_SCALE | DFT_REAL_OUTPUT);

	Mat dst = Mat::zeros (tone.size (), CV_32F);
	for (int nY = 0; nY < dst.rows; nY++)
	{
		for (int nX = 0; nX <dst.cols;nX++)
		{
			dst.at<float> (nY, nX) = pow (texture.at<float> (nY, nX), beta.at<float> (nY, nX));
		}
	}

	return dst;
}

DWORD WINAPI Render (LPVOID lpParameter)
{
	THREAD_DATA* pThreadData = (THREAD_DATA*)lpParameter;
	Mat tone = NaturalHistogramMatching (pThreadData->src);
	Mat render = TextureRendering (tone, pThreadData->p);
	pThreadData->src = render;
	return 0L;
}

void PencilDraw (uchar* I, int ks, int dirNum, double gamma, uchar* P, int rows, int cols, int rows2, int cols2, uchar* output)
{
	Mat I2 (rows, cols, CV_8UC3, I);
	Mat P2 (rows2, cols2, CV_8UC3, P);
	cvtColor (I2, I2, CV_BGRA2GRAY);
	cvtColor (P2, P2, CV_BGRA2GRAY);

	resize (P2, P2, I2.size ());
	td.p = P2;
	td.src = I2;
	HANDLE hThread1 = CreateThread (NULL, 0, Render, &td, 0, NULL);
	Mat stroke = GenStroke (I2, ks, dirNum, gamma);
	CloseHandle (hThread1);

	Mat mul = BlendImage (stroke, td.src);
	cvtColor (mul, mul, CV_GRAY2BGR);
	for (int i = 0;i < mul.cols*mul.rows*3;i++)
		output[i] = mul.data[i];
}

Mat PencilDraw (Mat I, int ks, int dirNum, double gamma, Mat P)
{
	resize (P, P, I.size ());
	td.p = P;
	td.src = I;
	HANDLE hThread1 = CreateThread (NULL, 0, Render, &td, 0, NULL);
	Mat stroke = GenStroke (I, ks, dirNum, gamma);
	CloseHandle (hThread1);

	Mat mul = BlendImage (stroke, td.src);
	
	return mul;
}

void ColorPencilDraw (uchar* I, int ks, int dirNum, double gamma, uchar* P, int rows, int cols, int rows2, int cols2,uchar* output)
{
	Mat Iruv;
	vector<Mat> vec;
	Mat I2 (rows, cols, CV_8UC3, I);
	Mat P2 (rows2, cols2, CV_8UC3, P);

	cvtColor (I2, Iruv, COLOR_BGR2YUV);
	cvtColor (P2, P2, CV_BGR2GRAY);

	split (Iruv, vec);

	vec[0] = PencilDraw (vec[0], ks, dirNum, gamma, P2);
	merge (vec, Iruv);

	Mat dst;

	cvtColor (Iruv, dst, COLOR_YUV2BGR);

	for (int i = 0;i < dst.cols*dst.rows * 3;i++)
		output[i] = dst.data[i];
}

Mat ColorPencilDraw (Mat I2, int ks, int dirNum, double gamma, Mat P2)
{
	Mat Iruv;
	vector<Mat> vec;

	cvtColor (I2, Iruv, COLOR_BGR2YUV);
	cvtColor (P2, P2, CV_BGR2GRAY);

	split (Iruv, vec);

	vec[0] = PencilDraw (vec[0], ks, dirNum, gamma, P2);
	merge (vec, Iruv);

	Mat dst;

	cvtColor (Iruv, dst, COLOR_YUV2BGR);

	return dst;
}