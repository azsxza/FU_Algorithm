#include"global.h"

Mat BalanceColor (Mat bitmap, int cyan, int magenta, int yellow,
	TRANSFERMODE mode, bool preserveLuminosity)
{

	assert (cyan >= -100 && cyan <= 100);
	assert (magenta >= -100 && magenta <= 100);
	assert (yellow >= -100 && yellow <= 100);

	//初始化色彩调整区域参数
	double  cyan_red[3];
	double  magenta_green[3];
	double  yellow_blue[3];
	for (int i = TRANSFERMODE_SHADOWS; i <= TRANSFERMODE_HIGHLIGHTS; i++)
	{
		cyan_red[i] = 0.0;
		magenta_green[i] = 0.0;
		yellow_blue[i] = 0.0;
	}
	cyan_red[mode] = cyan;
	magenta_green[mode] = magenta;
	yellow_blue[mode] = yellow;

	//变亮的转换数组
	double  highlights_add[256] = { 0 };
	double  midtones_add[256] = { 0 };
	double  shadows_add[256] = { 0 };

	//变暗的转换数组
	double  highlights_sub[256] = { 0 };
	double  midtones_sub[256] = { 0 };
	double  shadows_sub[256] = { 0 };

	//初始化转换用的数组
	for (int i = 0; i < 256; i++)
	{
		highlights_add[i] = shadows_sub[255 - i] = (1.075 - 1 / ((double)i / 16.0 + 1));
		midtones_add[i] = midtones_sub[i] = 0.667 * (1 - SQR (((double)i - 127.0) / 127.0));
		shadows_add[i] = highlights_sub[i] = 0.667 * (1 - SQR (((double)i - 127.0) / 127.0));
	}


	//创建LOOKUP TABLE
	double  *cyan_red_transfer[3];
	double  *magenta_green_transfer[3];
	double  *yellow_blue_transfer[3];
	int   red, green, blue;
	uchar r_lookup[256], g_lookup[256], b_lookup[256];


	//设置转换数组
	cyan_red_transfer[TRANSFERMODE_SHADOWS] = (cyan_red[TRANSFERMODE_SHADOWS] > 0) ? shadows_add : shadows_sub;
	cyan_red_transfer[TRANSFERMODE_MIDTONES] = (cyan_red[TRANSFERMODE_MIDTONES] > 0) ? midtones_add : midtones_sub;
	cyan_red_transfer[TRANSFERMODE_HIGHLIGHTS] = (cyan_red[TRANSFERMODE_HIGHLIGHTS] > 0) ? highlights_add : highlights_sub;

	magenta_green_transfer[TRANSFERMODE_SHADOWS] = (magenta_green[TRANSFERMODE_SHADOWS] > 0) ? shadows_add : shadows_sub;
	magenta_green_transfer[TRANSFERMODE_MIDTONES] = (magenta_green[TRANSFERMODE_MIDTONES] > 0) ? midtones_add : midtones_sub;
	magenta_green_transfer[TRANSFERMODE_HIGHLIGHTS] = (magenta_green[TRANSFERMODE_HIGHLIGHTS] > 0) ? highlights_add : highlights_sub;

	yellow_blue_transfer[TRANSFERMODE_SHADOWS] = (yellow_blue[TRANSFERMODE_SHADOWS] > 0) ? shadows_add : shadows_sub;
	yellow_blue_transfer[TRANSFERMODE_MIDTONES] = (yellow_blue[TRANSFERMODE_MIDTONES] > 0) ? midtones_add : midtones_sub;
	yellow_blue_transfer[TRANSFERMODE_HIGHLIGHTS] = (yellow_blue[TRANSFERMODE_HIGHLIGHTS] > 0) ? highlights_add : highlights_sub;

	for (int i = 0; i < 256; i++)
	{
		red = i;
		green = i;
		blue = i;

		red += (int)(cyan_red[TRANSFERMODE_SHADOWS] * cyan_red_transfer[TRANSFERMODE_SHADOWS][red]
			+ cyan_red[TRANSFERMODE_MIDTONES] * cyan_red_transfer[TRANSFERMODE_MIDTONES][red]
			+ cyan_red[TRANSFERMODE_HIGHLIGHTS] * cyan_red_transfer[TRANSFERMODE_HIGHLIGHTS][red]);
		red = CLAMP0255 (red);

		green += (int)(magenta_green[TRANSFERMODE_SHADOWS] * magenta_green_transfer[TRANSFERMODE_SHADOWS][green]
			+ magenta_green[TRANSFERMODE_MIDTONES] * magenta_green_transfer[TRANSFERMODE_MIDTONES][green]
			+ magenta_green[TRANSFERMODE_HIGHLIGHTS] * magenta_green_transfer[TRANSFERMODE_HIGHLIGHTS][green]);
		green = CLAMP0255 (green);

		blue += (int)(yellow_blue[TRANSFERMODE_SHADOWS] * yellow_blue_transfer[TRANSFERMODE_SHADOWS][blue]
			+ yellow_blue[TRANSFERMODE_MIDTONES] * yellow_blue_transfer[TRANSFERMODE_MIDTONES][blue]
			+ yellow_blue[TRANSFERMODE_HIGHLIGHTS] * yellow_blue_transfer[TRANSFERMODE_HIGHLIGHTS][blue]);
		blue = CLAMP0255 (blue);

		r_lookup[i] = (uchar)red;
		g_lookup[i] = (uchar)green;
		b_lookup[i] = (uchar)blue;

	}

	Mat dst;
	//如果不需要保证亮度不变化，直接赋值就可以了
	if (!preserveLuminosity)
	{
		dst = AdjustCurve (bitmap, r_lookup, g_lookup, b_lookup);
	}
	else
	{
		dst = PreserveLuminosityAdjustCurve (bitmap, r_lookup, g_lookup, b_lookup);
	}

	return dst;
}
