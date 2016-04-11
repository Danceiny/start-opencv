// 直方图的计算与显示
#include"stdafx.h"

int histogram(char* filename) {
	IplImage* src;
	if ((src = cvLoadImage(filename))) {
		// Compute the HSV image and decompose it into separate planes.
		IplImage* hsv = cvCreateImage(cvGetSize(src), 8, 3);
		cvCvtColor(src, hsv, CV_BGR2HSV);

		IplImage* h_plane = cvCreateImage(cvGetSize(src), 8, 1);
		IplImage* s_plane = cvCreateImage(cvGetSize(src), 8, 1);
		IplImage* v_plane = cvCreateImage(cvGetSize(src), 8, 1);

		//IplImage* planes[] = { h_plane,s_plane };
		//cvCvtPixToPlane(hsv, h_plane, s_plane, v_plane, 0);

		//我认为上面两行应该要换个次序吧，把像素拷到单通道中，
		//这样组合成双通道planes,才是有像素的，才可以作为IplImage参数传给cvCalcHist。

		cvCvtPixToPlane(hsv, h_plane, s_plane, v_plane, 0);
		IplImage* planes[] = { h_plane,s_plane };

		// Build the histogram and compute its contents.
		int h_bins = 30;
		int s_bins = 32;

		CvHistogram* hist;

			int hist_size[] = {h_bins,s_bins};
			float h_ranges[] = { 0,180 };//hue is [0,180]
			float s_ranges[] = { 0,255 };
			float* ranges[] = { h_ranges,s_ranges };
			hist = cvCreateHist(
				2,
				hist_size,
				CV_HIST_ARRAY,
				ranges,
				1
				);
	

		cvCalcHist(planes, hist, 0, 0);//compute histogram
		
		// 原书冒出来一个i
		//cvNormalizeHist(hist[i], 1.0);// Normalize it
		
		cvNormalizeHist(hist, 1.0);// Normalize it
		// Create an image to use to visualize our histogram.
		int scale = 10;
		IplImage* hist_img = cvCreateImage(
			cvSize(h_bins*scale, s_bins*scale),
			8,
			3
			);
		cvZero(hist_img);

		// populate our visualization with little gray squares.
		float max_value = 0;
		cvGetMinMaxHistValue(hist, 0, &max_value, 0, 0);


		for (int h = 0; h < h_bins; h++) {
			for (int s = 0; s < s_bins; s++) {
				float bin_val = cvQueryHistValue_2D(hist, h, s);
				int intensity = cvRound(bin_val * 255 / max_value);
				cvRectangle(
					hist_img,
					cvPoint(h*scale, s*scale),
					cvPoint(scale*(h + 1)-1, scale*(s + 1)-1),
					CV_RGB(intensity, intensity, intensity),
						CV_FILLED
						);
			}
		}
		cvNamedWindow("Source", 1);
		cvShowImage("Source", src);
		cvNamedWindow("H-S Histogram",1);
		cvShowImage("H-S Histogram", hist_img);
		cvWaitKey();

	}
	return 0;
}

// 说明

// 选择对可视化显示中的颜色进行归一化处理，而不是归一化直方图本身。这样才有理由调用cvGetMinMaxHistValue
// 尽管对于某些应用而言归一化直方图本身会更好一些。
// 

//妈蛋，最后效果好差，完全看不出是直方图，一大坨黑的，中间有一小坨白的……告诉我究竟哪里有问题啊~~~~