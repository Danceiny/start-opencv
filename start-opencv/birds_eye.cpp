// //call:
// birds-eye board_w board_h instrinics distortion image_file
// adjust view height using keys 'u' up,'d' down. ESC to quit.
#include "stdafx.h"
int birds_eys(int board_w,int board_h,/*CvMat* intr,CvMat* dist,*/char* intri,char* dist,char* img){
	//input parameters:
	/*int board_w = atoi(argv[1]);
	int board_h = atoi(argv[2]);*/
	int board_n = board_w * board_h;
	CvSize board_sz = cvSize(board_w, board_h);


	/*CvMat* intrinsic = (CvMat*)cvLoad(argv[3]);
	CvMat* distortion = (CvMat*)cvLoad(argv[4]);*/
	CvMat* intrinsic = (CvMat*)cvLoad(intri);
	CvMat* distortion = (CvMat*)cvLoad(dist);

	
	IplImage* image = 0;
	IplImage* gray_image = 0;
	if((image = cvLoadImage(img))==0){
		printf("Error: Couldn't load %s\n",img);
		return -1;
	}
	gray_image = cvCreateImage(cvGetSize(image),8,1);
	cvCvtColor(image,gray_image,CV_BGR2GRAY);
	
	//undistort our image
	IplImage* mapx = cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
	IplImage* mapy = cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
	//this initializes rectification matrices
	cvInitUndistortMap(intrinsic,distortion,mapx,mapy);
	IplImage* t = cvCloneImage(image);
	
	//rectify our image
	cvRemap(t,image,mapx,mapy);
	
	//get the chessboard on the plane
	cvNamedWindow("Chessboard");
	CvPoint2D32f* corners = new CvPoint2D32f[board_n];
	
	int corner_count = 0;
	int found = cvFindChessboardCorners(image,board_sz,corners,&corner_count,CV_CALIB_CB_ADAPTIVE_THRESH|CV_CALIB_CB_FILTER_QUADS);
	if(!found){
		printf("Couldn't aquire chessboard on %s, only found %d of %d corners\n",img,corner_count,board_n);
		return -1;
	}
	//get subpixel accuracy on those corners:
	cvFindCornerSubPix(gray_image,corners,corner_count,cvSize(11,11),cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_EPS|CV_TERMCRIT_ITER,30,0.1));
	
	//get the image and object points:
	//we will choose chessboard object points as (r,c):
	//(0,0),(board_w-1,0),(0,board_h-1),(board_w-1,board_h-1).
	CvPoint2D32f objPts[4],imgPts[4];
	objPts[0].x = 0;objPts[0].y =0;
	objPts[1].x = board_w-1;objPts[1].y = 0;
	objPts[2].x = 0;objPts[2].y = board_h-1;
	objPts[3].x = board_w-1;objPts[3].y = board_h-1;
	imgPts[0] = corners[0];
	imgPts[1] = corners[board_w-1];
	imgPts[2] = corners[(board_h-1)*board_w];
	imgPts[3] = corners[(board_h-1)*board_w+board_w-1];
	
	//draw the points in order: B,G,R,Yellow
	cvCircle(image,cvPointFrom32f(imgPts[0]),9,CV_RGB(0,0,255),3);
	cvCircle(image,cvPointFrom32f(imgPts[1]),9,CV_RGB(0,255,0),3);
	cvCircle(image,cvPointFrom32f(imgPts[2]),9,CV_RGB(255,0,0),3);
	cvCircle(image,cvPointFrom32f(imgPts[3]),9,CV_RGB(0,255,255),3);

	//draw the found chessboard
	cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);
	cvShowImage("Chessboard", image);
	//find the homograyphy
	CvMat* H = cvCreateMat(3, 3, CV_32F);
	cvGetPerspectiveTransform(objPts, imgPts, H);
	//let the user adjust the Z height of the view
	float Z = 25;
	int key = 0;
	IplImage* birds_image = cvCloneImage(image);
	cvNamedWindow("Birds_Eye");
	//loop to allow user to play with height:
	//escape key stops
	while (key != 27) {
		//set the height
		CV_MAT_ELEM(*H, float, 2, 2) = Z;
		//compute the frontal parallel or bird's-eye view:
		//using homography to remap the view
		cvWarpPerspective(image, birds_image, H, CV_INTER_LINEAR | CV_WARP_INVERSE_MAP | CV_WARP_FILL_OUTLIERS);
		cvShowImage("Birds_Eye", birds_image);
		key = cvWaitKey();
		if (key == 'u')Z += 0.5;
		if (key == 'd')Z -= 0.5;

	}
	cvSave("H.xml", H);//we can reuse H for the same camera mounting
	return 0;

}