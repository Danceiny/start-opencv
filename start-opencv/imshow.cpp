#include "stdafx.h"

int imshow(int argc, char* filename)
{
	IplImage* img = cvLoadImage(filename);
	cvNamedWindow("Example", CV_WINDOW_AUTOSIZE);
	cvShowImage("Example", img);
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvDestroyWindow("Example");
	return 0;
}
