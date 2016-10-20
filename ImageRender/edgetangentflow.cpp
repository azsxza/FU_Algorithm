#include"global.h"

#define	 DISCRETE_FILTER_SIZE	2048
#define  LOWPASS_FILTR_LENGTH	10.00000f
#define	 LINE_SQUARE_CLIP_MAX	100000.0f
#define	 VECTOR_COMPONENT_MIN   0.050000f


inline void MakeUnit (Vec2f& v)
{
	float mag = sqrt (v[0] * v[0] + v[1] * v[1]);
	if (mag != 0.0) {
		v[0] /= mag;
		v[1] /= mag;
	}
}

inline void GetPerpendicularVec (Vec2f& t)	//counter-clockwise
{
	float temp = t[0];
	t[0] = t[1];
	t[1] = -1 * temp;
}

inline int WeightSpatial (int x1, int y1, int x2, int y2, int kernel)	//Ws(X,Y)
{
	if (sqrtf ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2))<kernel)
	{
		return 1;
	}
	return 0;
}

inline float WeightMag (float X, float Y)			// Wm(X,Y)
{
	return (Y - X) + 1.0f;
}

inline int Clamp (int v, int min, int max)
{
	if (v < min)
		v = min;
	else if (v > max)
		v = max;
	return v;
};

inline float WeightDir (Vec3f centerX, Vec3f neighborY)	// Wd(X,Y)
{
	return fabs (centerX[0] * neighborY[0] + centerX[1] * neighborY[1]);
}

float Sign (Vec3f centerX, Vec3f neighborY)			// sign
{
	if (centerX[0] * neighborY[0] + centerX[1] * neighborY[1]>0)
		return 1.0f;
	else
		return -1.0f;
}

Mat GenETFFromGradientMap (Mat src, int kernel, int iter)	// self-call iter times
{
	assert (!src.empty ());
	if (src.channels () == 3)
	{
		Mat gray, colorboost;
		decolor (src, gray, colorboost);
		Mat dst = GenETFFromGradientMap (gray, kernel, iter);
		return dst;
	}

	Mat I;
	src.convertTo (I, CV_32F);

	int width = I.cols;
	int height = I.rows;

	Mat dst = Mat::zeros (height, width, CV_32FC3);

	/// Gradient calculation.
	Mat grad = Mat::zeros (height, width, CV_32FC1); //梯度图像
	Mat etf = Mat::zeros (height, width, CV_32FC3); //边缘正切流
	Vec2f   tempVec;

	float maxGrad = -1;

	for (int nY = 1; nY<height - 1;nY++)
	{
		for (int nX = 1;nX <width - 1;nX++)
		{
			tempVec[0] = I.at<float> (nY + 1, nX - 1) + 2 * I.at<float> (nY + 1, nX) + I.at<float> (nY + 1, nX + 1)
				- I.at<float> (nY - 1, nX - 1) - 2 * I.at<float> (nY - 1, nX) - I.at<float> (nY - 1, nX + 1);
			tempVec[1] = I.at<float> (nY - 1, nX + 1) + 2 * I.at<float> (nY, nX + 1) + I.at<float> (nY + 1, nX + 1)
				- I.at<float> (nY - 1, nX - 1) - 2 * I.at<float> (nY, nX - 1) - I.at<float> (nY + 1, nX - 1);

			// The initial ETF, denoted etf, is obtained by taking perpendicular
			// vectors (in the counter-clockwise sense) from the initial gradient map grad(x) of the input image I.
			grad.at<float> (nY, nX) = sqrt (tempVec[0] * tempVec[0] + tempVec[1] * tempVec[1]);

			// t0(x) is then normalized before use.
			if (grad.at<float> (nY, nX) > maxGrad)
				maxGrad = grad.at<float> (nY, nX);
		}
	}

	/// 处理一下边界
	// M.col(0) = M.col(1); // this will not work
	Mat tmp = grad.col (0);                  //第一列
	grad.col (1).copyTo (tmp);

	tmp = grad.col (width - 1);
	grad.col (width - 2).copyTo (tmp);	       //最后一列

	tmp = grad.row (0);
	grad.row (1).copyTo (tmp);		    	//第一行
	tmp = grad.row (height - 1);
	grad.row (height - 2).copyTo (tmp);         //最后一行


												/// 处理一下四个边角
	grad.at<float> (0, 0) = grad.at<float> (0, 1) + grad.at<float> (1, 0) / 2;				     //左上角
	grad.at<float> (0, width - 1) = grad.at<float> (0, width - 2) + grad.at<float> (1, width - 1) / 2;				 //右上角
	grad.at<float> (height - 1, 0) = grad.at<float> (height - 1, 1) + grad.at<float> (height - 2, 0) / 2;				 //左下角
	grad.at<float> (height - 1, width - 1) = grad.at<float> (height - 1, width - 2) + grad.at<float> (height - 2, width - 1) / 2;		 //左下角


																																		 /// normalize the magnitude
	for (int nY = 0; nY<height;nY++)
	{
		for (int nX = 0;nX <width;nX++)
		{
			grad.at<float> (nY, nX) = grad.at<float> (nY, nX) / maxGrad * 255;
		}
	}


	for (int nY = 1; nY<height - 1;nY++)
	{
		for (int nX = 1;nX <width - 1;nX++)
		{
			tempVec[0] = grad.at<float> (nY + 1, nX - 1) + 2 * grad.at<float> (nY + 1, nX) + grad.at<float> (nY + 1, nX + 1)
				- grad.at<float> (nY - 1, nX - 1) - 2 * grad.at<float> (nY - 1, nX) - grad.at<float> (nY - 1, nX + 1);  //	Y方向的Sobel算子
			tempVec[1] = grad.at<float> (nY - 1, nX + 1) + 2 * grad.at<float> (nY, nX + 1) + grad.at<float> (nY + 1, nX + 1)
				- grad.at<float> (nY - 1, nX - 1) - 2 * grad.at<float> (nY, nX - 1) - grad.at<float> (nY + 1, nX - 1);  //    X方向的Sobel算子

																														// The initial ETF, denoted etf, is obtained by taking perpendicular
																														// vectors (in the counter-clockwise sense) from the initial gradient map grad(x) of the input image I.
			etf.at<Vec3f> (nY, nX)[0] = -tempVec[0];
			etf.at<Vec3f> (nY, nX)[1] = tempVec[1];
			etf.at<Vec3f> (nY, nX)[2] = sqrt (tempVec[0] * tempVec[0] + tempVec[1] * tempVec[1]);

			// t0(x) is then normalized before use.
			if (etf.at<Vec3f> (nY, nX)[2] > maxGrad)
				maxGrad = etf.at<Vec3f> (nY, nX)[2];
		}
	}

	/// 处理一下边界
	// M.col(0) = M.col(1); // this will not work
	tmp = etf.col (0);                      //第一列
	etf.col (1).copyTo (tmp);

	tmp = etf.col (width - 1);
	etf.col (width - 2).copyTo (tmp);	       //最后一列

	tmp = etf.row (0);
	etf.row (1).copyTo (tmp);		    	   //第一行
	tmp = etf.row (height - 1);
	etf.row (height - 2).copyTo (tmp);         //最后一行


											   /// 处理一下四个边角
	etf.at<float> (0, 0) = etf.at<float> (0, 1) + etf.at<float> (1, 0) / 2;					     //左上角
	etf.at<float> (0, width - 1) = etf.at<float> (0, width - 2) + etf.at<float> (1, width - 1) / 2;				 //右上角
	etf.at<float> (height - 1, 0) = etf.at<float> (height - 1, 1) + etf.at<float> (height - 2, 0) / 2;				 //左下角
	etf.at<float> (height - 1, width - 1) = etf.at<float> (height - 1, width - 2) + etf.at<float> (height - 2, width - 1) / 2;		 //左下角

																																	 /// normalize
	for (int nY = 0; nY<height;nY++)
	{
		for (int nX = 0;nX <width;nX++)
		{
			float mag = sqrt (etf.at<Vec3f> (nY, nX)[0] * etf.at<Vec3f> (nY, nX)[0] + etf.at<Vec3f> (nY, nX)[1] * etf.at<Vec3f> (nY, nX)[1]);
			if (mag != 0)
			{
				etf.at<Vec3f> (nY, nX)[0] /= mag;
				etf.at<Vec3f> (nY, nX)[1] /= mag;
			}
			etf.at<Vec3f> (nY, nX)[2] = etf.at<Vec3f> (nY, nX)[2] / maxGrad;
		}
	}

	/// iter first time. NOTE: now the ori_gmap is changed to a t(x) tangent map
	Mat iterETF = etf;
	Mat iterETF2 = etf.clone ();

	int x, y;
	/// iter more times
	for (int i = 0; i<iter;i++)
	{
		/// horizontal
		for (int nY = 0; nY<height; nY++)
		{
			for (int nX = 0; nX<width; nX++)
			{
				Vec2f newVec = Vec2f (0, 0);
				Vec3f t_cur_X = iterETF.at<Vec3f> (nY, nX);	// t_cur_X is the center vector

				for (int kX = -kernel; kX <= kernel;kX++)
				{
					y = nY;
					x = Clamp (nX + kX, 0, width - 1);
					Vec3f t_cur_Y = iterETF.at<Vec3f> (y, x);	// t_cur_Y is the current neighbor vector

					float angle = t_cur_X[0] * t_cur_Y[0] + t_cur_X[1] * t_cur_Y[1];
					float sign = angle < 0.0f ? -1.0f : 1.0f;
					float wm = WeightMag (t_cur_X[2], t_cur_Y[2]);
					float wd = fabs (angle);
					newVec[0] += sign * t_cur_Y[0] * wm * wd;	// compute formula
					newVec[1] += sign * t_cur_Y[1] * wm * wd;	// compute formula
				}

				MakeUnit (newVec);
				iterETF2.at<Vec3f> (nY, nX)[0] = newVec[0];
				iterETF2.at<Vec3f> (nY, nX)[1] = newVec[1];
			}
		}

		iterETF = iterETF2.clone ();
		/// vertical
		for (int nY = 0; nY<height; nY++)
		{
			for (int nX = 0; nX<width; nX++)
			{
				Vec2f newVec = Vec2f (0, 0);
				Vec3f t_cur_X = iterETF.at<Vec3f> (nY, nX);	// t_cur_X is the center vector

				for (int kY = -kernel; kY <= kernel;kY++)
				{
					y = Clamp (nY + kY, 0, height - 1);
					x = nX;

					Vec3f t_cur_Y = iterETF.at<Vec3f> (y, x);	// t_cur_Y is the current neighbor vector

					float  angle = t_cur_X[0] * t_cur_Y[0] + t_cur_X[1] * t_cur_Y[1];
					float  sign = angle<0.0f ? -1.0f : 1.0f;
					float  wm = WeightMag (t_cur_X[2], t_cur_Y[2]);
					float  wd = fabs (angle);
					newVec[0] += sign * t_cur_Y[0] * wm * wd;	// compute formula
					newVec[1] += sign * t_cur_Y[1] * wm * wd;	// compute formula
				}

				MakeUnit (newVec);
				iterETF2.at<Vec3f> (nY, nX)[0] = newVec[0];
				iterETF2.at<Vec3f> (nY, nX)[1] = newVec[1];
			}
		}
		iterETF = iterETF2.clone ();

	}
	VisualizeByArrow (iterETF);
	return iterETF;
}


Mat GenETFFromInputImage (Mat src, int kernel, int iter)	// self-call iter times
{
	assert (!src.empty ());
	if (src.channels () == 3)
	{
		Mat gray, colorboost;
		cvtColor (src, gray, COLOR_BGR2GRAY);
		Mat dst = GenETFFromInputImage (gray, kernel, iter);
		return dst;
	}

	Mat I;
	src.convertTo (I, CV_32F);

	int width = I.cols;
	int height = I.rows;

	/// Gradient calculation.
	Mat etf = Mat::zeros (height, width, CV_32FC3); //边缘正切流
	float   tempVec[2];

	float maxGrad = -1;

	for (int nY = 1; nY<height - 1;nY++)
	{
		for (int nX = 1;nX <width - 1;nX++)
		{
			tempVec[0] = I.at<float> (nY + 1, nX - 1) + 2 * I.at<float> (nY + 1, nX) + I.at<float> (nY + 1, nX + 1)
				- I.at<float> (nY - 1, nX - 1) - 2 * I.at<float> (nY - 1, nX) - I.at<float> (nY - 1, nX + 1); //	Y方向的Sobel算子
			tempVec[1] = I.at<float> (nY - 1, nX + 1) + 2 * I.at<float> (nY, nX + 1) + I.at<float> (nY + 1, nX + 1)
				- I.at<float> (nY - 1, nX - 1) - 2 * I.at<float> (nY, nX - 1) - I.at<float> (nY + 1, nX - 1); //	X方向的Sobel算子

																											  // The initial ETF, denoted etf, is obtained by taking perpendicular
																											  // vectors (in the counter-clockwise sense) from the initial gradient map grad(x) of the input image I.
			etf.at<Vec3f> (nY, nX)[0] = -tempVec[0];
			etf.at<Vec3f> (nY, nX)[1] = tempVec[1];
			etf.at<Vec3f> (nY, nX)[2] = sqrt (tempVec[0] * tempVec[0] + tempVec[1] * tempVec[1]);

			// t0(x) is then normalized before use.
			if (etf.at<Vec3f> (nY, nX)[2] > maxGrad)
				maxGrad = etf.at<Vec3f> (nY, nX)[2];
		}
	}


	/// 处理一下边界
	// M.col(0) = M.col(1); // this will not work
	Mat tmp = etf.col (0);                  //第一列
	etf.col (1).copyTo (tmp);

	tmp = etf.col (width - 1);
	etf.col (width - 2).copyTo (tmp);	       //最后一列

	tmp = etf.row (0);
	etf.row (1).copyTo (tmp);		    	   //第一行
	tmp = etf.row (height - 1);
	etf.row (height - 2).copyTo (tmp);        //最后一行


											  /// 处理一下四个边角
	etf.at<Vec3f> (0, 0) = etf.at<Vec3f> (0, 1) + etf.at<Vec3f> (1, 0) / 2;					     //左上角
	etf.at<Vec3f> (0, width - 1) = etf.at<Vec3f> (0, width - 2) + etf.at<Vec3f> (1, width - 1) / 2;				 //右上角
	etf.at<Vec3f> (height - 1, 0) = etf.at<Vec3f> (height - 1, 1) + etf.at<Vec3f> (height - 2, 0) / 2;				 //左下角
	etf.at<Vec3f> (height - 1, width - 1) = etf.at<Vec3f> (height - 1, width - 2) + etf.at<Vec3f> (height - 2, width - 1) / 2;		 //左下角

																																	 /// normalize
	for (int nY = 0; nY<height;nY++)
	{
		for (int nX = 0;nX <width;nX++)
		{
			float mag = etf.at<Vec3f> (nY, nX)[2];
			if (mag != 0)
			{
				etf.at<Vec3f> (nY, nX)[0] /= mag;
				etf.at<Vec3f> (nY, nX)[1] /= mag;
			}
			etf.at<Vec3f> (nY, nX)[2] = etf.at<Vec3f> (nY, nX)[2] / maxGrad;
		}
	}

	/// iter first time. NOTE: now the ori_gmap is changed to a t(x) tangent map
	Mat iterETF = etf;
	Mat iterETF2 = etf.clone ();

	int x, y;
	/// iter more times
	for (int i = 0; i<iter;i++)
	{
		/// horizontal
		for (int nY = 0; nY<height; nY++)
		{
			for (int nX = 0; nX<width; nX++)
			{
				Vec2f newVec = Vec2f (0, 0);
				Vec3f tX = iterETF.at<Vec3f> (nY, nX);	// t_cur_X is the center vector

				for (int kX = -kernel; kX <= kernel; kX++)
				{
					y = nY; x = Clamp (nX + kX, 0, width - 1);
					Vec3f tY = iterETF.at<Vec3f> (y, x);	// t_cur_Y is the current neighbor vector

					float sign = Sign (tX, tY);
					float wm = WeightMag (tX[2], tY[2]);
					float wd = WeightDir (tX, tY);
					//float ws    =    WeightSpatial(nX,nY,x,y,kernel);
					newVec[0] += sign * tY[0] * wm * wd;	// compute formula
					newVec[1] += sign * tY[1] * wm * wd;	// compute formula
				}

				MakeUnit (newVec);
				iterETF2.at<Vec3f> (nY, nX)[0] = newVec[0];
				iterETF2.at<Vec3f> (nY, nX)[1] = newVec[1];
			}
		}

		iterETF = iterETF2.clone ();
		/// vertical
		for (int nY = 0; nY<height; nY++)
		{
			for (int nX = 0; nX<width; nX++)
			{
				Vec2f newVec = Vec2f (0, 0);
				Vec3f tX = iterETF.at<Vec3f> (nY, nX);	// t_cur_X is the center vector

				for (int kY = -kernel; kY <= kernel;kY++)
				{
					y = Clamp (nY + kY, 0, height - 1);x = nX;

					Vec3f tY = iterETF.at<Vec3f> (y, x);	// t_cur_Y is the current neighbor vector

					float sign = Sign (tX, tY);
					float wm = WeightMag (tX[2], tY[2]);
					float wd = WeightDir (tX, tY);
					//float ws    =   WeightSpatial(nX,nY,x,y,kernel);
					newVec[0] += sign * tY[0] * wm * wd;	// compute formula
					newVec[1] += sign * tY[1] * wm * wd;	// compute formula
				}

				MakeUnit (newVec);
				iterETF2.at<Vec3f> (nY, nX)[0] = newVec[0];
				iterETF2.at<Vec3f> (nY, nX)[1] = newVec[1];
			}
		}
		iterETF = iterETF2.clone ();
	}
	return iterETF;
}

// Visualize a vector field by using LIC (Linear Integral Convolution).
void VisualizeByLIC (Mat vf)
{
	assert (vf.channels () >= 2);

	int width = vf.cols;
	int height = vf.rows;

	vector<Mat> vector_field;
	split (vf, vector_field);
	Mat white_noise (height, width, CV_8UC1);
	Mat show_field (height, width, CV_8UC1);
	for (int r = 0; r < height; ++r)
	{
		for (int c = 0; c < width; ++c)
		{
			int n = rand ();
			n = ((n & 0xff) + ((n & 0xff00) >> 8)) & 0xff;
			white_noise.at<uchar> (r, c) = static_cast<uchar>(n);
		}
	}

	float p_LUT0[DISCRETE_FILTER_SIZE], p_LUT1[DISCRETE_FILTER_SIZE];
	for (int i = 0; i < DISCRETE_FILTER_SIZE; ++i)
	{
		p_LUT0[i] = p_LUT1[i] = i;
	}

	// Do LIC.
	float   krnlen = LOWPASS_FILTR_LENGTH;
	int		advDir;						///ADVection DIRection (0: positive;  1: negative)
	int		advcts;						///number of ADVeCTion stepS per direction (a step counter)
	int		ADVCTS = int (krnlen * 3);	///MAXIMUM number of advection steps per direction to break dead loops

	float	vctr_x;						///x-component  of the VeCToR at the forefront point
	float	vctr_y;						///y-component  of the VeCToR at the forefront point
	float	clp0_x;						///x-coordinate of CLiP point 0 (current)
	float	clp0_y;						///y-coordinate of CLiP point 0	(current)
	float	clp1_x;						///x-coordinate of CLiP point 1 (next   )
	float	clp1_y;						///y-coordinate of CLiP point 1 (next   )
	float	samp_x;						///x-coordinate of the SAMPle in the current pixel
	float	samp_y;						///y-coordinate of the SAMPle in the current pixel
	float	tmpLen;						///TeMPorary LENgth of a trial clipped-segment
	float	segLen;						///SEGment   LENgth
	float	curLen;						///CURrent   LENgth of the streamline
	float	prvLen;						///PReVious  LENgth of the streamline
	float	W_ACUM;						///ACcuMulated Weight from the seed to the current streamline forefront
	float	texVal;						///TEXture VALue
	float	smpWgt;						///WeiGhT of the current SaMPle
	float	t_acum[2];					///two ACcUMulated composite Textures for the two directions, perspectively
	float	w_acum[2];					///two ACcUMulated Weighting values   for the two directions, perspectively
	float*	wgtLUT = NULL;				///WeiGhT Look Up Table pointing to the target filter LUT
	float	len2ID = (DISCRETE_FILTER_SIZE - 1) / krnlen;	///map a curve LENgth TO an ID in the LUT

	for (int r = 0; r < height; ++r)
	{
		for (int c = 0; c < width; ++c)
		{
			///init the composite texture accumulators and the weight accumulators///
			t_acum[0] = t_acum[1] = w_acum[0] = w_acum[1] = 0.0f;
			///for either advection direction///
			for (advDir = 0; advDir < 2; advDir++)
			{
				advcts = 0;
				curLen = 0.0f;
				clp0_x = c + 0.5f;
				clp0_y = r + 0.5f;
				///access the target filter LUT///
				wgtLUT = (advDir == 0) ? p_LUT0 : p_LUT1;
				///until the streamline is advected long enough or a tightly  spiralling center / focus is encountered///
				while (curLen < krnlen && advcts < ADVCTS)
				{
					///access the vector at the sample///
					vctr_x = vector_field[0].at<float> (r, c);
					vctr_y = vector_field[1].at<float> (r, c);
					///in case of a critical point///
					if (vctr_x == 0.0f && vctr_y == 0.0f)
					{
						t_acum[advDir] = (advcts == 0) ? 0.0f : t_acum[advDir];		   ///this line is indeed unnecessary
						w_acum[advDir] = (advcts == 0) ? 1.0f : w_acum[advDir];
						break;
					}
					///negate the vector for the backward-advection case///
					vctr_x = (advDir == 0) ? vctr_x : -vctr_x;
					vctr_y = (advDir == 0) ? vctr_y : -vctr_y;
					// clip the segment against the pixel boundaries
					// --- find the shorter from the two clipped segments.
					// replace  all  if-statements  whenever  possible  as
					// they  might  affect the computational speed.
					segLen = LINE_SQUARE_CLIP_MAX;
					segLen = (vctr_x < -VECTOR_COMPONENT_MIN) ?
						(int (clp0_x) - clp0_x) / vctr_x : segLen;
					segLen = (vctr_x > VECTOR_COMPONENT_MIN) ?
						(int (int (clp0_x) + 1.5f) - clp0_x) / vctr_x : segLen;
					segLen = (vctr_y < -VECTOR_COMPONENT_MIN) ?
						(((tmpLen = (int (clp0_y) - clp0_y) / vctr_y) < segLen) ? tmpLen : segLen) : segLen;
					segLen = (vctr_y > VECTOR_COMPONENT_MIN) ?
						(((tmpLen = (int (int (clp0_y) + 1.5f) - clp0_y) / vctr_y)  <  segLen) ? tmpLen : segLen) : segLen;
					///update the curve-length measurers///
					prvLen = curLen;
					curLen += segLen;
					segLen += 0.0004f;
					///check if the filter has reached either end///
					segLen = (curLen > krnlen) ? ((curLen = krnlen) - prvLen) : segLen;
					///obtain the next clip point///
					clp1_x = clp0_x + vctr_x * segLen;
					clp1_y = clp0_y + vctr_y * segLen;
					///obtain the middle point of the segment as the texture-contributing sample///
					samp_x = (clp0_x + clp1_x) * 0.5f;
					samp_y = (clp0_y + clp1_y) * 0.5f;
					///obtain the texture value of the sample///
					texVal = static_cast<float>(white_noise.at<uchar> (samp_y, samp_x));
					///update the accumulated weight and the accumulated composite texture (texture x weight)///
					W_ACUM = wgtLUT[int (curLen * len2ID)];
					smpWgt = W_ACUM - w_acum[advDir];
					w_acum[advDir] = W_ACUM;
					t_acum[advDir] += texVal * smpWgt;
					///update the step counter and the "current" clip point///
					advcts++;
					clp0_x = clp1_x;
					clp0_y = clp1_y;
					///check if the streamline has gone beyond the flow field///
					if (clp0_x < 0.0f || clp0_x >= width || clp0_y < 0.0f || clp0_y >= height)
						break;
				}
			}
			///normalize the accumulated composite texture///
			texVal = (t_acum[0] + t_acum[1]) / (w_acum[0] + w_acum[1]);
			///clamp the texture value against the displayable intensity range [0, 255]
			texVal = (texVal < 0.0f) ? 0.0f : texVal;
			texVal = (texVal > 255.0f) ? 255.0f : texVal;
			show_field.at<uchar> (r, c) = static_cast<uchar>(texVal);
		}
	}

	imshow ("Visualized Field", show_field);
	waitKey ();
}


Mat CalcStructureTensor (Mat src)
{
	int height = src.rows;
	int width = src.cols;
	Mat st = Mat::zeros (height, width, CV_32FC3);

	if (1 == src.channels ())
	{
		// Gradient calculation.
		cv::Mat gx, gy;
		cv::Sobel (src, gx, CV_32F, 1, 0);
		cv::Sobel (src, gy, CV_32F, 0, 1);
		float dx, dy;
		for (int nY = 0; nY < height; ++nY)
		{
			for (int nX = 0; nX < width; ++nX)
			{
				dx = gx.at<float> (nY, nX);
				dy = gy.at<float> (nY, nX);
				st.at<cv::Vec3f> (nY, nX)[0] = dx * dx;  // E
				st.at<cv::Vec3f> (nY, nX)[1] = dy * dy;  // G
				st.at<cv::Vec3f> (nY, nX)[2] = dx * dy;  // F
			}
		}
		return st;
	}
	else if ((3 == src.channels ()) || (4 == src.channels ()))
	{
		// BGR color space Gradient calculation.
		vector<cv::Mat> bgrChnnels;
		cv::split (src, bgrChnnels);
		cv::Mat gx[3], gy[3];
		for (int k = 0; k < 3; ++k)
		{
			cv::Sobel (bgrChnnels[k], gx[k], CV_32F, 1, 0);
			cv::Sobel (bgrChnnels[k], gy[k], CV_32F, 0, 1);
		}
		cv::Vec3f fx, fy;
		for (int nY = 0; nY < height; ++nY)
		{
			for (int nX = 0; nX < width; ++nX)
			{
				fx = Vec3f (gx[0].at<float> (nY, nX), gx[1].at<float> (nY, nX), gx[2].at<float> (nY, nX));
				fy = Vec3f (gy[0].at<float> (nY, nX), gy[1].at<float> (nY, nX), gy[2].at<float> (nY, nX));
				st.at<Vec3f> (nY, nX)[0] = fx.dot (fx);  // E
				st.at<Vec3f> (nY, nX)[1] = fy.dot (fy);  // G
				st.at<Vec3f> (nY, nX)[2] = fx.dot (fy);  // F
			}
		}
	}

	return st;
}

Mat GetEdgeTangentFlow (Mat src)
{
	int height = src.rows;
	int width = src.cols;

	/// Step 1: Cclculate the structure tensor.
	Mat st = CalcStructureTensor (src); 				// CV_32FC3 (E, G, F)

														/// Step 2: Gaussian blur the struct tensor. sst_sigma = 2.0
	float sigma_sst = 2;
	int gaussianSize = ceil (sigma_sst * 2) * 2 + 1;
	GaussianBlur (st, st, Size2i (gaussianSize, gaussianSize), sigma_sst);

	/// Step 3: Extract etf: CV_32FC3 (v2.x, v2.y, sqrt(lambda2).
	Mat etf = Mat::zeros (height, width, CV_32FC3);
	float E, G, F, lambda1, v2x, v2y, v2;
	for (int nY = 0; nY < height; ++nY)
	{
		for (int nX = 0; nX < width; ++nX)
		{
			E = st.at<cv::Vec3f> (nY, nX)[0];
			G = st.at<cv::Vec3f> (nY, nX)[1];
			F = st.at<cv::Vec3f> (nY, nX)[2];
			lambda1 = 0.5 * (E + G + sqrtf ((G - E) * (G - E) + 4 * F * F));
			v2x = E - lambda1;
			v2y = F;
			v2 = sqrtf (v2x * v2x + v2y * v2y);
			etf.at<Vec3f> (nY, nX)[0] = (0 == v2) ? 0 : (v2x / v2);
			etf.at<Vec3f> (nY, nX)[1] = (0 == v2) ? 0 : (v2y / v2);
			assert (E + G - lambda1 >= 0);
			etf.at<Vec3f> (nY, nX)[2] = sqrtf (E + G - lambda1);
		}
	}
	// What to show it?
	VisualizeByLIC (etf);
	VisualizeByArrow (etf);
	return etf;
}

Mat VisualizeByArrow (Mat src)
{
	Mat showField = src.clone ();
	showField = Scalar (1, 1, 1);
	int height = showField.rows;
	int width = showField.cols;

	float angle;
	float dx;
	float dy;
	float mag;
	Point2d p, q;

	for (int nY = 0; nY < height; nY += 10)
	{
		for (int nX = 0; nX < width; nX += 10)
		{
			dx = src.at<Vec3f> (nY, nX)[0];
			dy = src.at<Vec3f> (nY, nX)[1];
			mag = src.at<Vec3f> (nY, nX)[2];
			if (mag > 0)
			{
				if (fabs (dx) < 0.0000001)
					angle = CV_PI / 2.0f;
				else
					angle = atan2f (dy, dx);

				p = Point2d (nX, nY);
				q = Point2d (nX - (int)(mag * cos (angle)), nY - (int)(mag * sin (angle)));
				line (showField, p, q, Scalar (0, 0, 255));

				p.x = (int)(q.x + 0.1 * mag * cos (angle + CV_PI / 5));
				p.y = (int)(q.y + 0.1 * mag * sin (angle + CV_PI / 5));
				line (showField, p, q, Scalar (0, 0, 255));

				p.x = (int)(q.x + 0.1 * mag * cos (angle - CV_PI / 5));
				p.y = (int)(q.y + 0.1 * mag * sin (angle - CV_PI / 5));
				line (showField, p, q, Scalar (0, 0, 255));
			}
		}
	}
	imshow ("Visualized Field", showField);
	waitKey ();
	return showField;
}

