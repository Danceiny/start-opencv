//#include"stdafx.h"
////
////首先需要储存Delaunay的内存空间，还需要一个外接矩形
////为设置这些参数，假设点都位于600*600的图像中
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
////接下来需要知道如何插入点（必须是32位浮点型）
//CvPoint2D32f fp;//this is our point holder
//for (i = 0; i < as_many_points_as_you_want; i++) {
//	//however you want to set points
//	fp = your_32f_point_list[i];
//	cvSubdivDelaunay2DInsert(subdiv, fp);
//}
//
////可以输入点来得到Delaunay三角剖分后，接下来用两个函数设置和清除相关的Voronoi划分。
//cvCalcSubdiv Voronoi2D(subdiv);//fill out Voronoi data in subdiv
//cvClearSubiv Voronoi2D(subdiv);//clear the Voronoi from subdiv
//
////...书中实在是不知所云了，翻译质量堪忧
//
//
////使用实例
//
////用函数cvSubdiv2DLocate()遍历Delaunay三角剖分的边
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