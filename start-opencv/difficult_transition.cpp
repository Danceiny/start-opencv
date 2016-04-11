#include"stdafx.h"
IplImage* doPyrDown(
	IplImage* in,
	int filter// = IPL_GAUSSIAN_5x5
	) 
{
	//best to make sure input image is divisible by two.
	assert(in->width % 2 == 0 && in->height % 2 == 0);
	IplImage* out = cvCreateImage(
		cvSize(in->width / 2, in->height / 2),
		in->depth,
		in->nChannels
		);
	cvPyrDown(in, out);
	return out;

}

// Canny边缘检测将输出写入一个单通道（灰度级）图像
IplImage* doCanny(
	IplImage* in,
	double lowThresh,//阈值
	double hightThresh,
	int aperture//光圈
	) 
{
	if (in->nChannels != 1)
		return 0;// Canny only handles gray scale images

	CvSize cvsize = cvGetSize(in);
	IplImage* out = cvCreateImage(
			cvsize,
			IPL_DEPTH_8U,
			1);
	cvCanny(in, out, lowThresh, hightThresh, aperture);
	return out;
}

int scale_twice_and_canny(IplImage* in)
{

// 缩放图像两次，然后在缩放后的图像中寻找边缘
	IplImage* img1;
img1= doPyrDown(in, IPL_GAUSSIAN_5x5);
IplImage* img2;
img2 = doPyrDown(img1, IPL_GAUSSIAN_5x5);
IplImage* img3;
img3 = doCanny(img2, 10, 100, 3);
// do whatever with img3

cvReleaseImage(&img1);
cvReleaseImage(&img2);
cvReleaseImage(&img3);

return 0;
}



// 每个独立阶段释放内存
void re_per_period(IplImage* in)
{

	IplImage* out;
	out = doPyrDown(in, IPL_GAUSSIAN_5x5);
	out = doPyrDown(out, IPL_GAUSSIAN_5x5);
	out = doCanny(out, 10, 100, 3);
	//do whatever with out

	cvReleaseImage(&out);
}