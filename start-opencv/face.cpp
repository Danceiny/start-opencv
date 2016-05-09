//Detect and draw detected object boxes on image
//Presumes 2 Globals:
//Cascade���� is loaded by:
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
	cvCvtColor(img, gray, CV_BGR2GRAY);//ת�ɻҶ�ͼ��
	cvResize(gray, small_img, CV_INTER_LINEAR);//������С
	cvEqualizeHist(small_img, small_img);//ֱ��ͼ���⣺ƽ������ֵ

	//detect objects if any
	cvClearMemStorage(storage);//������ʶ�����Ŀ�������CvSeq�����з��أ�����������ڴ�
	//ʵ�ʵļ�����
	CvSeq* objects = cvHaarDetectObjects(small_img, cascade, storage, 1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(30, 30));

	//���ȡ�������ľ�������Ȼ���ò�ͬ����ɫͨ������cvRectangle()������
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
//	CvHaarClassifierCascade* cascade,	//������cascade��ͨ��cvLoad()���ص�Haar��������
//	CvMemStorage* storage,		//��������
//	double scale_factor CV_DEFAULT(1.1),	//����ÿ������ͬ��С�Ĵ���֮���ж�����Ծ������ֵ�ϴ�������죬Ҳ�Ϳ��ܶ�ʧ����
//	int min_neighbors CV_DEFAULT(3),	//�������⣬Ĭ��ֵ3����������3���ص���⣬���ǲ���Ϊ����ȷʵ����
//	int flags CV_DEFAULT(0),	//��4����ѡ��ֵ
//	CvSize min_size CV_DEFAULT(cvSize(0, 0)),	//ָʾѰ����������С����
//	CvSize max_size CV_DEFAULT(cvSize(0, 0))
//	);


//flag:
//1. CV_HAAR_DO_CANNY_PRUNING, ������������ƽ�����ޱ�Ե����
//2. CV_HAAR_SCALE_IMAGE���������������ŷ���������������ͼ�񣨴����ڴ�ͻ����ʹ�����⣬��������ܣ�
//3. CV_HAAR_FIND_BIGGEST_OBJECTS��������ֻ��������Ŀ�꣨�����������Ϊ1��0��
//4. CV_HAAR_DO_ROUGH_SEARCH��ֻ����CV_HAAR_FIND_BIGGEST_OBJECTSһ��ʹ�ã����߷��������κδ��ڣ�ֻҪ��һ����ѡ�߱����ּ�����Ѱ�ҡ�