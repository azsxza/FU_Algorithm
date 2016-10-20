#include"global.h"
#include "LandmarkCoreIncludes.h"
#include "GazeEstimation.h"

LandmarkDetector::FaceModelParameters det_parameters;
LandmarkDetector::CLNF clnf_model;

Mat_<float> depth_image;
Mat_<uchar> grayscale_image;
vector<Point2d> pts;
void inti ()
{
	vector<string> arguments;
	arguments.push_back ("");
	det_parameters = LandmarkDetector::FaceModelParameters (arguments);
	clnf_model = LandmarkDetector::CLNF ("../model//main_clnf_general.txt");
	det_parameters.track_gaze = false;
}

void GetKeyPoint (Mat img, int* key)
{
	cvtColor (img, grayscale_image, CV_BGR2GRAY);
	LandmarkDetector::DetectLandmarksInVideo (grayscale_image, depth_image, clnf_model, det_parameters);
	if (clnf_model.detection_certainty < 0.2)
	{
		pts = LandmarkDetector::CalculateLandmarks (clnf_model);
		key[0] = ((pts[2] + pts[3] + pts[31]) / 3).x;
		key[1] = ((pts[2] + pts[3] + pts[31]) / 3).y;
		key[2] = ((pts[14] + pts[13] + pts[35]) / 3).x;
		key[3] = ((pts[14] + pts[13] + pts[35]) / 3).y;
		key[4] = pts[30].x;
		key[5] = pts[30].y;
		key[6] = ((pts[37] + pts[38] + pts[40] + pts[41]) / 4).x;
		key[7] = ((pts[37] + pts[38] + pts[40] + pts[41]) / 4).y;
		key[8] = ((pts[43] + pts[44] + pts[46] + pts[47]) / 4).x;
		key[9] = ((pts[43] + pts[44] + pts[46] + pts[47]) / 4).y;
		key[10] = pts[18].x;
		key[11] = pts[18].y;
		key[12] = pts[25].x;
		key[13] = pts[25].y;
	}
}

void GetKeyPoint (uchar* imgData, int* key, int height, int width)
{
	Mat img (height, width, CV_8UC3, imgData);
	
	GetKeyPoint (img, key);
}