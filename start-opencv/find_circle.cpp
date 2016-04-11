#include"stdafx.h"
// 使用cvHoughCircles返回在灰度图中找到的圆序列

int find_circle(char* filename)
{
	IplImage* image = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
	//IplImage* image = cvLoadImage(filename, 1);
	//IplImage* dst = cvLoadImage("pic.jpg", 0);
	//cvConvertImage(image, dst);
	
	CvMemStorage* storage = cvCreateMemStorage(0);
	cvSmooth(image, image, CV_GAUSSIAN, 5, 5);
	CvSeq* results = cvHoughCircles(
		image,
		storage,
		CV_HOUGH_GRADIENT,
		2,
		image->width / 10
		);

	for (int i = 0; i < results->total; i++) {
		float* p = (float*)cvGetSeqElem(results, i);
		CvPoint pt = cvPoint(cvRound(p[0]), cvRound(p[1]));
		cvCircle(
			image,
			pt,
			cvRound(p[2]),
			CV_RGB(0xff, 0xff, 0)
			);
	}
	cvNamedWindow("cvHoughCircles", 1);
	cvShowImage("cvHoughCircles", image);
	cvWaitKey(0);
	

	cvReleaseImage(&image);
	return 0;
}