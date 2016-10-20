#include"global.h"

bool stop = false;

void StartCapture (string Name)
{
	CaptureSender sender (CS_SHARED_PATH, CS_EVENT_WRITE, CS_EVENT_READ);
	DWORD avgTimePF = 1000 / 15;  //15FPS
	Mat image;
	VideoCapture capture;
	capture.open (0);
	cout << "Capture is opened" << endl;
	for (int i = 0;;i++)
	{
		capture >> image;
		if (image.empty ())
		{
			waitKey (avgTimePF);
			continue;
		}
		//image = ColorPencilDraw (image, 5, 4, 1, aa);
		imshow ("Sample", image);
		//image = RotateImage (image, 180, 0);
		cvtColor (image, image, CV_BGR2RGB);
		//sender.Send (i*avgTimePF, image.cols, image.rows, image.data);
		if (waitKey (avgTimePF) >= 0)
		{
			break;
		}
	}
}