

////首先，为需要的不同临时图像和统计属性图像创建指针。这样有助于根据图像的不同类型对以后使用的图像指针排序。
//// global storage
////float, 3-channel images
//IplImage *IavgF, *IdiffF, *IprevF, *IhiF, *IlowF;
//IplImage *Iscratch, *Iscratch2;
////float, 1-channel images
//IplImage *Igray1, *Igray2, *Igray3;
//IplImage *Ilow1, *Ilow2, *Ilow3;
//IplImage *Ihi1, *Ihi2, *Ihi3;
////byte, 1-channel image
//IplImage *Imaskt;
////counts number of images learned for averaging later.
//float Icount;
////接下来创建一个函数给需要的所有临时图像分配内存
//// I is just a sample image for allocation  
//// (passed in for sizing)
//
//void AllocateImages(IplImage* I) {
//	CvSize sz = cvGetSize(I);
//	IavgF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	IdiffF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	IprevF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	IhiF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	IlowF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	Ilow1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ilow2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ilow3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ihi1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ihi2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ihi3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//
//	cvZero(IargF);
//	cvZero(IdiffF);
//	cvZero(IprevF);
//	cvZero(IlowF);
//	Icount = 0.00001;//protect against divide by zero
//
//	Iscratch = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	Iscratch2 = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	Igray1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Igray2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Igray3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Imaskt = cvCreateImage(sz, IPL_DEPTH_8U, 1);
//
//	cvZero(Iscratch);
//	cvZero(Iscratch2);
//
//}
