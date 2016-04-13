// store to disk a line segment of bgr pixels from pt1 to pt2

#include "stdafx.h"

int line_segment(char* filename,CvPoint pt1,CvPoint pt2) {
	CvCapture* capture = cvCreateFileCapture(filename);
	int max_buffer;
	IplImage* rawImage;
	int r[10000], g[10000], b[10000];
	CvLineIterator iterator;

	FILE* fptrb = fopen("blines.csv", "w");//store the data here
	FILE* fptrg = fopen("glines.csv", "w");//for each color channel
	FILE* fptrr = fopen("rlines.csv", "w");

	// main processing loop:

	for (;;) {
		if (!cvGrabFrame(capture))break;
		rawImage = cvRetrieveFrame(capture);
		max_buffer = cvInitLineIterator(rawImage, pt1, pt2, &iterator, 8, 0);

		for (int j = 0; j < max_buffer; j++) {
			fprintf(fptrb, "%d,", iterator.ptr[0]);//write blue value
			fprintf(fptrg, "%d,", iterator.ptr[1]);//green
			fprintf(fptrr, "%d,", iterator.ptr[2]);

			iterator.ptr[2] = 255;//mark this sample in red

			CV_NEXT_LINE_POINT(iterator);//step to the next pixel

		}

		// output the data in rows:
		fprintf(fptrb, "/n");
		fprintf(fptrg, "/n");
		fprintf(fptrr, "/n");

	}
	// clean up

	fclose(fptrb); fclose(fptrg); fclose(fptrr);
	cvReleaseCapture(&capture);
	return 0;
}


//也可以用下面的函数更轻松地对直线采样：
//int cvSampleLine(int) cvSampleLine(
//	const CvArr* image,
//	CvPoint pt1,
//	CvPoint pt2,
//	void* buffer,
//	int connectivity CV_DEFAULT(8)
//	);

//该函数只是cvInitLineIterator()和宏CV_NEXT_LINE_POINT(line_iterator)的简单封装。



//平均背景法代码案例

//首先，我们为需要的不同临时图像和统计属性图像创建指针，这样有助于根据图像的不同类型对以后使用的图像指针排序。

// global storage

// float,3-channel images
IplImage *IavgF, *IdiffF, *IprevF, *IhiF, *IlowF;
IplImage *Iscratch, *Iscratch2;

// float,1-channel images

IplImage *Igray1, *Igray2, *Igray3;
IplImage *Ilow1, *Ilow2, *Ilow3;
IplImage *Ihi1, *Ihi2, *Ihi3;

//byte,1-channel image
IplImage *Imaskt;

// counts number of images learned for averaging later.
float Icount;

void setHighThreshold(float scale) {
	//基于每一帧图像平均绝对差设置阈值的有效函数
	//使得对于每一帧图像的绝对差大于平均值scale倍的像素都认为是前景
	cvConvertScale(IdiffF, Iscratch, scale);
	cvAdd(Iscratch, IavgF, IhiF);
	cvSplit(IhiF, Ihi1, Ihi2, Ihi3, 0);

}
void setLowThreshold(float scale) {
	//基于每一帧图像平均绝对差设置阈值的有效函数
	//使得对于每一帧图像的绝对差小于平均值scale倍的像素都认为是背景
	cvConvertScale(IdiffF, Iscratch, scale);
	cvSub(IavgF, Iscratch, IlowF);
	cvSplit(IlowF, Ilow1, Ilow2, Ilow3, 0);
}

/*使用上面这两个函数时，用cvConvertScale()乘以预先设定的值来增加或减小与IavgF相关的范围。
操作通过函数cvSplit()为图像的每个通道设置IhiF和IlowF的范围。*/


//接下来，我们创建一个函数来给需要的所有临时图像分配内存。方便起见，传递一幅图像（来自视频）作为大小参考来分配临时图像

// I is just a sample image for allocation  
// (passed in for sizing)

void AllocateImages(IplImage* I) {
	CvSize sz = cvGetSize(I);
	IavgF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IdiffF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IprevF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IhiF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IlowF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	Ilow1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ilow2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ilow3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ihi1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ihi2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ihi3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);

	cvZero(IavgF);
	cvZero(IdiffF);
	cvZero(IprevF);
	cvZero(IlowF);
	Icount = 0.00001;//protect against divide by zero

	Iscratch = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	Iscratch2 = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	Igray1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Igray2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Igray3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Imaskt = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	cvZero(Iscratch);
	cvZero(Iscratch2);

}

//在接下来的代码片段中，学习累积背景图像和每一帧图像差值的绝对值。这通常需要30-1000帧，有时每秒几帧或者有时需要所有有价值的帧图像。函数调用需要3通道8深度的彩色图像。

//learn the background statistics for one more frame
// I is a color sample of the background, 3-channel, 8u

void accumulateBackground(IplImage* I) {
	static int first = 1;//nb.Not thread safe
	cvCvtColor(I, Iscratch, 1.0);//convert to float
	if (!first) {
		cvAcc(Iscratch, IavgF);
		cvAbsDiff(Iscratch, IavgF,Iscratch2);
		cvAcc(Iscratch2, IdiffF);
		Icount += 1.0;
	}
	first = 0;
	cvCopy(Iscratch, IprevF);
}

/*首先用cvCvtScale()将原始的每通道8位、3颜色通道的彩色图像转换位一个浮点型的3通道图像。
之后积累原始的浮点图像为IavgF。
接下来用cvAbsDiff()计算每帧图像之间的绝对差图像，并将其积累为图像IdiffF。
每次积累图像之后，增加图像计数器Icount的值，该计数器是一个全部变量用于接下来计算平均值。*/

/*积累了足够多的帧图像之后，就将其转化成为一个背景的统计模式。这就是说，计算每一个像素的均值和方差观测（平均绝对差分）。*/

void createModelFromStats() {
	cvConvertScale(IavgF, IavgF, (double)(1.0 / Icount));
	cvConvertScale(IdiffF, IdiffF, (double)(1.0 / Icount));
	// Make sure diff is always something
	cvAddS(IdiffF, cvScalar(1.0, 1.0, 1.0), IdiffF);
	setHighThreshold(7.0);
	setLowThreshold(6.0); 
}

/*cvConvertScale()通过除以输入图像累积的数目计算平均原始图像和绝对差分图像。
确保平均差分图像的最小值为1。
当计算前景和背景阈值以及避免前景阈值和背景阈值相等而出现的退化情况时，我们要缩放这个因素。*/



//图像分割通过调用下面的函数完成
// create a binary: 0,255 mask where 255 means foreground pixels
// I: Input image, 3-channel, 8u
// Imask: mask image to be created, 1-channel 8u

void backgroundDiff(
	IplImage *I,
	IplImage *Imask
	) {
	cvCvtScale(I, Iscratch, 1.0);//to float;
		cvSplit(Iscratch, Igray1, Igray2, Igray3, 0);

	// channel 1

	cvInRange(Igray1, Ilow1, Ihi1, Imask);

	// channel 2
	cvInRange(Igray2, Ilow2, Ihi2, Imaskt);
	cvOr(Imask, Imaskt, Imask);

	// channel 3
	cvInRange(Igray3, Ilow3, Ihi3, Imaskt);
	cvOr(Imask, Imaskt, Imask);

	// finally, invert the results
	cvSubRS(Imask, cvScalarAll(255), Imask);
}

/*函数首先通过调用函数cvCvtScale()将输入图像I（用于分割）转换为浮点型的图像。
再调用cvSplit()将3通道图像分解为单通道。
最后通过函数cvInRange()检查这些单通道图像是否在平均背景像素的高低阈值之间，该函数将8位深度的灰度图像Imaskt中在背景范围内的像素设为最大值255，否则设为0.
最后利用cvSubRS()将其转化为Imask图像。*/

//完成背景建模后，需要释放内存
void DeallocateImages() {
	cvReleaseImage(&IavgF);
	cvReleaseImage(&IdiffF);
	cvReleaseImage(&IprevF);
	cvReleaseImage(&IhiF);
	cvReleaseImage(&IlowF);
	cvReleaseImage(&Ilow1);
	cvReleaseImage(&Ilow2);
	cvReleaseImage(&Ilow3);
	cvReleaseImage(&Ihi1); 
	cvReleaseImage(&Ihi2); 
	cvReleaseImage(&Ihi3); 
	cvReleaseImage(&Iscratch); 
	cvReleaseImage(&Iscratch2);
	cvReleaseImage(&Igray1);
	cvReleaseImage(&Igray2);
	cvReleaseImage(&Igray3);
	cvReleaseImage(&Imaskt);
}


/*以上是一种背景场景和分割前景目标的简单方法。只能用于背景场景中不包含运动的部分，且要求光线保持不变。*/