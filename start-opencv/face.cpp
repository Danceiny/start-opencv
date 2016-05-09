//Detect and draw detected object boxes on image
//Presumes 2 Globals:
//Cascade级联 is loaded by:
//cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name,0,0,0);
//and that storage is allocated:
//CvMemStorage* storage = cvCreateMemStorage(0);
#include "stdafx.h"
void detect_and_draw(
	IplImage* img,
	double scale ) {


	cvNamedWindow("face",1);
	cvShowImage("face", img);

	CvMemStorage* storage=cvCreateMemStorage(0);
	char* cascade_name = "D:\\Program Files\\opencv\\sources\\data\haarcascades\\haarcascade_frontalface_alt2.xml";
	CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);

	static CvScalar colors[] = {
		{{0,0,255}},{{0,128,255}},{ { 0,255,255 } },{ { 0,255,0 } },
		{ { 255,128, } },{ { 255,255,0 } },{ { 255,0,0 } },{ { 255,0,255 } }
	};//just some pretty colors to draw with

	//Image preparation:
	IplImage* gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage* small_img = cvCreateImage(cvSize(cvRound(img->width / scale), cvRound(img->height / scale)), 8, 1);
	cvCvtColor(img, gray, CV_BGR2GRAY);//转成灰度图像
	cvResize(gray, small_img, CV_INTER_LINEAR);//调整大小
	cvEqualizeHist(small_img, small_img);//直方图均衡：平衡亮度值

	//detect objects if any
	cvClearMemStorage(storage);//分类器识别出的目标矩形在CvSeq序列中返回，则需先清除内存
	//实际的检测代码
	CvSeq* objects = cvHaarDetectObjects(small_img, cascade, storage, 1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(30, 30));

	//逐个取出人脸的矩形区域，然后用不同的颜色通过函数cvRectangle()画出来
	//loop through found objects and draw boxes around them
	for (int i = 0; i < (objects ? objects->total : 0); i++) {
		CvRect* r = (CvRect*)cvGetSeqElem(objects, i);
		cvRectangle(img, cvPoint(r->x, r->y), cvPoint(r->x + r->width, r->y + r->height), colors[i % 8]);
	}
	cvReleaseImage(&gray);
	cvReleaseImage(&small_img);
	cvDestroyAllWindows();
}


//CvSeq* cvHaarDetectObjects(
//	const CvArr* image,
//	CvHaarClassifierCascade* cascade,	//分类器cascade是通过cvLoad()加载的Haar特征级联
//	CvMemStorage* storage,		//工作缓存
//	double scale_factor CV_DEFAULT(1.1),	//决定每两个不同大小的窗口之间有多大的跳跃，参数值较大则计算变快，也就可能丢失物体
//	int min_neighbors CV_DEFAULT(3),	//控制误检测，默认值3表明至少有3次重叠检测，我们才认为人脸确实存在
//	int flags CV_DEFAULT(0),	//有4个待选数值
//	CvSize min_size CV_DEFAULT(cvSize(0, 0)),	//指示寻找人脸的最小区域
//	CvSize max_size CV_DEFAULT(cvSize(0, 0))
//	);


//flag:
//1. CV_HAAR_DO_CANNY_PRUNING, 分类器将跳过平滑（无边缘区域）
//2. CV_HAAR_SCALE_IMAGE，分类器将不缩放分类器，而是缩放图像（处理内存和缓存的使用问题，以提高性能）
//3. CV_HAAR_FIND_BIGGEST_OBJECTS，分类器只返回最大的目标（返回物体个数为1或0）
//4. CV_HAAR_DO_ROUGH_SEARCH，只可与CV_HAAR_FIND_BIGGEST_OBJECTS一起使用，告诉分类器在任何窗口，只要第一个候选者被发现即结束寻找。