//#include"stdafx.h"
////
////������Ҫ����Delaunay���ڴ�ռ䣬����Ҫһ����Ӿ���
////Ϊ������Щ����������㶼λ��600*600��ͼ����
//
//// storage and structure for delaunay subdivision
//CvRect rect = { 0,0,600,600 };//our outer bounding box
//CvMemStorage *storage;//storage for the delaunay subdivision
//storage = cvCreateMemStorage(0);//Initialize the storage
//CvSubdiv2D* subdiv;//the subdivision itself
//subdiv = init_delaunay(storage, rect);//see this funtion below
//
////Initialization convenience function for delaunay subdivision
//CvSubdiv2D* init_delaunay(
//	CvMemStorage* storage,
//	CvRect rect) {
//	CvSubdiv2D* subdiv;
//	subdiv = cvCreateSubdiv2D(
//		CV_SEQ_KIND_SUBDIV2D,
//		sizeof(*subdiv),
//		sizeof(CvSubdiv2DPoint),
//		sizeof(CvQuadEdge2D),
//		storage);
//	cvInitSubdiv Delaunay2D(subdiv,rect);//rect sets the bounds
//	return subdiv;
//}
//
////��������Ҫ֪����β���㣨������32λ�����ͣ�
//CvPoint2D32f fp;//this is our point holder
//for (i = 0; i < as_many_points_as_you_want; i++) {
//	//however you want to set points
//	fp = your_32f_point_list[i];
//	cvSubdivDelaunay2DInsert(subdiv, fp);
//}
//
////������������õ�Delaunay�����ʷֺ󣬽������������������ú������ص�Voronoi���֡�
//cvCalcSubdiv Voronoi2D(subdiv);//fill out Voronoi data in subdiv
//cvClearSubiv Voronoi2D(subdiv);//clear the Voronoi from subdiv
//
////...����ʵ���ǲ�֪�����ˣ�������������
//
//
////ʹ��ʵ��
//
////�ú���cvSubdiv2DLocate()����Delaunay�����ʷֵı�
//void locate_point(
//	CvSubdiv2D* subdiv,
//	CvPoint2D32f fp,
//	IplImage* img,
//	CvScalar 
//	) {
//	CvSubdiv2DEdge e;
//	CvSubdiv2DEdge e0 = 0;
//	CvSubdiv2DPoint* p = 0;
//	CvSubdiv2DLocate(subdiv, fp, &e0, &p);
//	if (e0) {
//		e = e0;
//		do//always 3 edges -- this is a triangulation,after all.
//		{
//			//insert your code here
//			//do something with e...
//			e = cvSubdiv2DGetEdge(e, CV_NEXT_AROUND_LEFT);
//		} while (e != e0);
//
//	}
//}