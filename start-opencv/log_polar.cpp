// log_polar.cpp:Defines the entry point for the console application.

#include"stdafx.h"

int log_polar(char* filename,double m) {
	IplImage* src;
	
	if ((src = cvLoadImage(filename))) {
		IplImage* dst = cvCreateImage(cvGetSize(src), 8, 3);
		IplImage* src2 = cvCreateImage(cvGetSize(src), 8, 3);
		cvLogPolar(
			src,
			dst,
			cvPoint2D32f(src->width / 4, src->height / 2),
			m,
			CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS
			);
		cvLogPolar(
			dst,
			src2,
			cvPoint2D32f(src->width / 4, src->height / 2),
			m,
			CV_INTER_LINEAR| CV_WARP_INVERSE_MAP
			);
		cvNamedWindow("log-polar", 1);
		cvShowImage("log-polar", dst);
		cvNamedWindow("inverse log-polar", 1);
		cvShowImage("inverse log-polar", src2);
		cvWaitKey();
		cvReleaseImage(&dst);
		cvReleaseImage(&src2);
		cvReleaseImage(&src);

	}

	return 0;
}