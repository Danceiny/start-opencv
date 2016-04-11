#include"stdafx.h"
#include"math.h"
// Compare thresholding with adaptive thresholding
// CALL:
//./adaptThreshold		Threshold 1binary 1adaptivemean\
//						blocksize offset filename


IplImage *Igray = 0, *It = 0, *Iat;
int threshold_cmp(double threshold,int threshold_type,int adaptive_method,int block_size,double offset,char* img)
{

	// Command line (but modified by me)
	// int threshold_type = atoi(argv[2])?\
	//		CV_THRESH_BINARY:CV_THRESH_BINARY_INV;	
	// int adaptive_method = atoi(argv[3]) ? \
	//		CV_ADAPTIVE_THRESH_MEAN_C : CV_ADAPTIVE_THRESH_GAUSSIAN_C;

	// Read in gray image
	if (!(Igray = cvLoadImage(img, CV_LOAD_IMAGE_GRAYSCALE)))return -1;

	// Create the grayscale output images
	It = cvCreateImage(cvSize(Igray->width, Igray->height), IPL_DEPTH_8U, 1);
	Iat = cvCreateImage(cvSize(Igray->width, Igray->height), IPL_DEPTH_8U, 1);

	// Threshold
	cvThreshold(Igray, It, threshold, 255, threshold_type);
	cvAdaptiveThreshold(Igray, Iat, 255, adaptive_method, threshold_type, block_size, offset);

	// Put up 2 windows
	cvNamedWindow("Raw", 1);
	cvNamedWindow("Threshold", 1);
	cvNamedWindow("Adaptive Threshold", 1);

	// Show the results
	cvShowImage("Raw", Igray);
	cvShowImage("Threshold", It);
	cvShowImage("Adaptive Threshold", Iat);

	cvWaitKey(0);

	// Clean up
	cvReleaseImage(&Igray);
	cvReleaseImage(&It);
	cvReleaseImage(&Iat);
	cvDestroyAllWindows();
	return 0;
}