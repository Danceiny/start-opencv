# include "cv.h"
# include "highgui.h"

void example2_4(IplImage* image)
{
	// Create some windows to show the input and output images in.

	cvNamedWindow("Example4-in");
	cvNamedWindow("Example4-out");

	// Create a window to show our input image

	cvShowImage("Example4-in", image);

	// Create an image to hold the smoothed output
	IplImage* out = cvCreateImage(
		cvGetSize(image),
		IPL_DEPTH_8U,
		3
		);

	// Do the smoothing

	//cvSmooth(image, out, CV_GAUSSIAN, 3, 3);
	cvSmooth(image, out, CV_GAUSSIAN,5,5);

	// Show the smoothed image in the output window

	cvShowImage("Example4-out", out);

	// Be tidy

	cvReleaseImage(&out);

	// Wait for the user to hit a key, then clean up the windows

	cvWaitKey(0);
	
	cvDestroyWindow("Example4-in");
	cvDestroyWindow("Example4-out");
}

int main(int argc,char** argv)
{
	CvCapture* g_capture = NULL;
	//IplImage* frame;
	//While loop (as in Example2) capture & show video
	
		//frame = cvQueryFrame(g_capture);
		//if (!frame)break;
		
	//	cvShowImage("Example3", frame);
	
	//	char c = cvWaitKey(33);
		//if (c == 27)break;
	
	IplImage* image;

	image = cvLoadImage(argv[1]);
	cvNamedWindow("Example", CV_WINDOW_AUTOSIZE);
	cvShowImage("Example", image);
	char c = cvWaitKey(100);
	example2_4(image);
	

	
}