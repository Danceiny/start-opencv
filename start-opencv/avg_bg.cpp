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


//Ҳ����������ĺ��������ɵض�ֱ�߲�����
//int cvSampleLine(int) cvSampleLine(
//	const CvArr* image,
//	CvPoint pt1,
//	CvPoint pt2,
//	void* buffer,
//	int connectivity CV_DEFAULT(8)
//	);

//�ú���ֻ��cvInitLineIterator()�ͺ�CV_NEXT_LINE_POINT(line_iterator)�ļ򵥷�װ��



//ƽ�����������밸��

//���ȣ�����Ϊ��Ҫ�Ĳ�ͬ��ʱͼ���ͳ������ͼ�񴴽�ָ�룬���������ڸ���ͼ��Ĳ�ͬ���Ͷ��Ժ�ʹ�õ�ͼ��ָ������

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
	//����ÿһ֡ͼ��ƽ�����Բ�������ֵ����Ч����
	//ʹ�ö���ÿһ֡ͼ��ľ��Բ����ƽ��ֵscale�������ض���Ϊ��ǰ��
	cvConvertScale(IdiffF, Iscratch, scale);
	cvAdd(Iscratch, IavgF, IhiF);
	cvSplit(IhiF, Ihi1, Ihi2, Ihi3, 0);

}
void setLowThreshold(float scale) {
	//����ÿһ֡ͼ��ƽ�����Բ�������ֵ����Ч����
	//ʹ�ö���ÿһ֡ͼ��ľ��Բ�С��ƽ��ֵscale�������ض���Ϊ�Ǳ���
	cvConvertScale(IdiffF, Iscratch, scale);
	cvSub(IavgF, Iscratch, IlowF);
	cvSplit(IlowF, Ilow1, Ilow2, Ilow3, 0);
}

/*ʹ����������������ʱ����cvConvertScale()����Ԥ���趨��ֵ�����ӻ��С��IavgF��صķ�Χ��
����ͨ������cvSplit()Ϊͼ���ÿ��ͨ������IhiF��IlowF�ķ�Χ��*/


//�����������Ǵ���һ������������Ҫ��������ʱͼ������ڴ档�������������һ��ͼ��������Ƶ����Ϊ��С�ο���������ʱͼ��

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

//�ڽ������Ĵ���Ƭ���У�ѧϰ�ۻ�����ͼ���ÿһ֡ͼ���ֵ�ľ���ֵ����ͨ����Ҫ30-1000֡����ʱÿ�뼸֡������ʱ��Ҫ�����м�ֵ��֡ͼ�񡣺���������Ҫ3ͨ��8��ȵĲ�ɫͼ��

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

/*������cvCvtScale()��ԭʼ��ÿͨ��8λ��3��ɫͨ���Ĳ�ɫͼ��ת��λһ�������͵�3ͨ��ͼ��
֮�����ԭʼ�ĸ���ͼ��ΪIavgF��
��������cvAbsDiff()����ÿ֡ͼ��֮��ľ��Բ�ͼ�񣬲��������Ϊͼ��IdiffF��
ÿ�λ���ͼ��֮������ͼ�������Icount��ֵ���ü�������һ��ȫ���������ڽ���������ƽ��ֵ��*/

/*�������㹻���֡ͼ��֮�󣬾ͽ���ת����Ϊһ��������ͳ��ģʽ�������˵������ÿһ�����صľ�ֵ�ͷ���۲⣨ƽ�����Բ�֣���*/

void createModelFromStats() {
	cvConvertScale(IavgF, IavgF, (double)(1.0 / Icount));
	cvConvertScale(IdiffF, IdiffF, (double)(1.0 / Icount));
	// Make sure diff is always something
	cvAddS(IdiffF, cvScalar(1.0, 1.0, 1.0), IdiffF);
	setHighThreshold(7.0);
	setLowThreshold(6.0); 
}

/*cvConvertScale()ͨ����������ͼ���ۻ�����Ŀ����ƽ��ԭʼͼ��;��Բ��ͼ��
ȷ��ƽ�����ͼ�����СֵΪ1��
������ǰ���ͱ�����ֵ�Լ�����ǰ����ֵ�ͱ�����ֵ��ȶ����ֵ��˻����ʱ������Ҫ����������ء�*/



//ͼ��ָ�ͨ����������ĺ������
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

/*��������ͨ�����ú���cvCvtScale()������ͼ��I�����ڷָת��Ϊ�����͵�ͼ��
�ٵ���cvSplit()��3ͨ��ͼ��ֽ�Ϊ��ͨ����
���ͨ������cvInRange()�����Щ��ͨ��ͼ���Ƿ���ƽ���������صĸߵ���ֵ֮�䣬�ú�����8λ��ȵĻҶ�ͼ��Imaskt���ڱ�����Χ�ڵ�������Ϊ���ֵ255��������Ϊ0.
�������cvSubRS()����ת��ΪImaskͼ��*/

//��ɱ�����ģ����Ҫ�ͷ��ڴ�
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


/*������һ�ֱ��������ͷָ�ǰ��Ŀ��ļ򵥷�����ֻ�����ڱ��������в������˶��Ĳ��֣���Ҫ����߱��ֲ��䡣*/