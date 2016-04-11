#include"stdafx.h"

int g_slider_position = 0;
CvCapture* g_capture = NULL;

void onTrackbarSlide(int pos){
	cvSetCaptureProperty(
		g_capture,
		CV_CAP_PROP_POS_FRAMES,
		pos
		);
}

int trackslide(char* filename){
	//if (NULL == argv[1])return -1;	// it always gets no  parameter, and no chance for me to put in a parameter, so i just shut it down with return value -1.
	cvNamedWindow("Example3", CV_WINDOW_AUTOSIZE);
	g_capture = cvCreateFileCapture(filename);

	// If it doesn't run at cmd, g_capture will be a null pointer, so let's give a check.
	// but it seemly doesn't work!!!why~
	// I assume that argv[1], which is NULL actually, passed into cvCreateFileCapture() as a parameter, at the same time, the program trys to read the address 0x000000.
	// So, it aborted.
	if (!g_capture){
		printf("loading video failed!\n");
		return -1;
	}

	int frames = (int)cvGetCaptureProperty(
		g_capture,
		CV_CAP_PROP_FRAME_COUNT
		);
	if (frames != 0){
		cvCreateTrackbar(
			"Position",
			"Example3",
			&g_slider_position,
			frames,
			onTrackbarSlide
			);
	}
	IplImage* frame;
	//While loop (as in Example2) capture & show video
	while (1){
		frame = cvQueryFrame(g_capture);
		if (!frame)break;
		cvShowImage("Example3", frame);
		char c = cvWaitKey(33);
		if (c == 27)break;
	}

	//Release memory and destroy window
	cvReleaseCapture(&g_capture);
	cvDestroyAllWindows();
	return 0;

}