#include"global.h"

// Prepare 1-d gaussian template.
static void GetGaussianWeights (float* weights, int neighbor, float sigma)
{
	if ((NULL == weights) || (neighbor < 0))
		return;

	float term1 = static_cast<float>(1.0f / (sqrt (2.0f * CV_PI) * sigma));
	float term2 = static_cast<float>(-1.0f / (2 * pow (sigma, 2)));

	weights[neighbor] = term1;
	float sum = weights[neighbor];

	for (int i = 1; i <= neighbor; ++i)
	{
		weights[neighbor + i] = static_cast<float>(exp (pow ((double)i, 2) * term2) * term1);
		weights[neighbor - i] = static_cast<float>(weights[neighbor + i]);
		sum += weights[neighbor + i] + weights[neighbor - i];
	}

	// Normalization
	for (int j = 0; j < neighbor * 2 + 1; ++j)
	{
		weights[j] /= sum;
	}
}

// Prepare 1-d difference of gaussian template.
static void GetDiffGaussianWeights (float* weights, int neighbor, float sigma_c, float sigma_s, float rho)
{
	if ((NULL == weights) || (neighbor < 0))
		return;

	float* gaussian_e = new float[neighbor * 2 + 1];
	float* gaussian_r = new float[neighbor * 2 + 1];

	GetGaussianWeights (gaussian_e, neighbor, sigma_c);
	GetGaussianWeights (gaussian_r, neighbor, sigma_s);

	float sum = 0;
	for (int i = 0; i < neighbor * 2 + 1; ++i)
	{
		weights[i] = gaussian_e[i] - rho * gaussian_r[i];
		sum += weights[i];
	}

	// Normalization
	for (int j = 0; j < neighbor * 2 + 1; ++j)
	{
		weights[j] /= sum;
	}

	delete[] gaussian_e;
	delete[] gaussian_r;
}


Mat GetDogEdge (Mat src, float sigma_c, float tau)
{
	float sigma_s = 1.6f * sigma_c;

	int height = src.rows;
	int width = src.cols;

	Mat dogEdge = cv::Mat::zeros (height, width, CV_8UC1);
	int gaussianSize = static_cast<int>(ceilf (2.0f * sigma_s) * 2 + 1);

	Mat blurE, blurR, gray;
	cv::bilateralFilter (src, gray, 5, 150, 150);
	gray.convertTo (gray, CV_32FC1);

	GaussianBlur (gray, blurE, Size2i (gaussianSize, gaussianSize), sigma_c);
	GaussianBlur (gray, blurR, Size2i (gaussianSize, gaussianSize), sigma_s);

	float diff;
	for (int nY = 0; nY < height; ++nY)
	{
		for (int nX = 0; nX < width; ++nX)
		{
			diff = blurE.at<float> (nY, nX) - tau * blurR.at<float> (nY, nX);
			if (diff > 0)
			{
				dogEdge.at<uchar> (nY, nX) = 255;
			}
			else
			{
				dogEdge.at<uchar> (nY, nX) = 0;
			}
		}
	}
	return dogEdge;
}

Mat GetFDogEdge (Mat src, Mat etf, float sigma_c, float sigma_m)
{
	int height = src.rows;
	int width = src.cols;

	Mat fdog = Mat::zeros (height, width, CV_32FC1);
	Mat f0 = Mat::ones (height, width, CV_32FC1);
	Mat f1 = Mat::ones (height, width, CV_32FC1);

	Mat gray, bsrc, bgray;

	cvtColor (src, gray, COLOR_BGR2GRAY);

	float	sintheta;
	float	costheta;

	float	sigma_s = 1.6f * sigma_c;
	float	rho = 0.99f;
	int		neighbor1 = static_cast<int>(ceilf (2.0f * sigma_s)); //这里都取高斯sigma值得2倍
	float*	diff_gaussian_weights = new float[neighbor1 * 2 + 1];
	float*	sample_pixels1;
	float	sum_dog;
	GetDiffGaussianWeights (diff_gaussian_weights, neighbor1, sigma_c, sigma_s, rho);

	float*  sample_pixels2;
	int		neighbor2 = static_cast<int>(ceilf (2.0f * sigma_m));  //这里都取高斯sigma值得2倍
	float*	gaussian_weights = new float[neighbor2 * 2 + 1];
	float   sum_gauss;
	GetGaussianWeights (gaussian_weights, neighbor2, sigma_m);

	// Step 1: do DoG along the gradient direction.
	for (int nY = 0; nY < height; ++nY)
	{
		for (int nX = 0; nX < width; ++nX)
		{
			// Get pixel gradient direction.
			sintheta = -1 * etf.at<Vec3f> (nY, nX)[0];
			costheta = etf.at<Vec3f> (nY, nX)[1];

			sample_pixels1 = new float[neighbor1 * 2 + 1];
			sample_pixels1[neighbor1] = static_cast<float>(gray.at<uchar> (nY, nX));

			for (int k = 1; k <= neighbor1; ++k)
			{
				int y_offset = cvRound (sintheta * k);
				int x_offset = cvRound (costheta * k);
				sample_pixels1[neighbor1 + k] = static_cast<float>(gray.at<uchar> (Clamp (nY + y_offset, 0, height - 1), Clamp (nX + x_offset, 0, width - 1)));
				sample_pixels1[neighbor1 - k] = static_cast<float>(gray.at<uchar> (Clamp (nY - y_offset, 0, height - 1), Clamp (nX - x_offset, 0, width - 1)));
			}

			// Calculate edge response.
			sum_dog = 0;
			for (int k = 0; k < 2 * neighbor1 + 1; ++k)
			{
				sum_dog += sample_pixels1[k] * diff_gaussian_weights[k];
			}
			f0.at<float> (nY, nX) = sum_dog;
			delete[] sample_pixels1;
		}
	}


	// Step 2: do Gaussian blur along tangent direction.
	for (int nY = 0; nY < height; ++nY)
	{
		for (int nX = 0; nX < width; ++nX)
		{
			// Get pixel tangent direction.
			sintheta = etf.at<Vec3f> (nY, nX)[1];
			costheta = etf.at<Vec3f> (nY, nX)[0];

			sample_pixels2 = new float[neighbor2 * 2 + 1];
			sample_pixels2[neighbor2] = f0.at<float> (nY, nX);

			for (int k = 1; k <= neighbor2; ++k)
			{
				int y_offset = cvRound (sintheta * k);
				int x_offset = cvRound (costheta * k);
				sample_pixels2[neighbor2 + k] = f0.at<float> (Clamp (nY + y_offset, 0, height - 1), Clamp (nX + x_offset, 0, width - 1));
				sample_pixels2[neighbor2 - k] = f0.at<float> (Clamp (nY - y_offset, 0, height - 1), Clamp (nX - x_offset, 0, width - 1));
			}

			// Calculate edge response.
			sum_gauss = 0;
			for (int k = 0; k < 2 * neighbor2 + 1; ++k)
			{
				sum_gauss += sample_pixels2[k] * gaussian_weights[k];
			}

			f1.at<float> (nY, nX) = sum_gauss;

			/// we convert it to a black-and-white image by binary thresholding,
			if (f1.at<float> (nY, nX) < 0)
				fdog.at<float> (nY, nX) = 0;
			else
				fdog.at<float> (nY, nX) = 1;

			delete[] sample_pixels2;
		}
	}

	delete[] diff_gaussian_weights;
	delete[] gaussian_weights;
	fdog.convertTo (fdog, CV_8U, 255);
	return fdog;
}

Mat LineDrawing (const Mat& src, int kernel, int iter, float sigma_c, float sigma_m, LineStyle style)
{
	Mat etf = GenETFFromInputImage (src, kernel, iter);
	Mat fdog = GetFDogEdge (src, etf, sigma_c, sigma_m);
	return fdog;
}

void  LineDrawing (uchar* Src, uchar* Dest, int Width, int Height, int Stride, int kernel, int iter, float sigma_c, float sigma_m, LineStyle style)
{
	int channel = Stride / Width;
	Mat I = cv::Mat (Height, Width, CV_MAKETYPE (CV_8U, channel), Src, Stride);

	if (channel == 4)
		cvtColor (I, I, COLOR_BGRA2BGR);

	Mat dst = LineDrawing (I, kernel, iter, sigma_c, sigma_m, style);


	for (int nY = 0;nY <dst.rows;nY++)
	{
		for (int nX = 0;nX<dst.cols;nX++)
		{
			Dest[nY*Stride + channel*nX] = dst.data[nY*dst.step + nX];
			Dest[nY*Stride + channel*nX + 1] = dst.data[nY*dst.step + nX];
			Dest[nY*Stride + channel*nX + 2] = dst.data[nY*dst.step + nX];
		}
	}
}