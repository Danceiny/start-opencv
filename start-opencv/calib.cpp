//读入棋盘的宽度和高度，读入收集到的不同场景图像，然后标定摄像机
//calib.cpp
// Calling convention:
//calib board_w board_h number_of_views
//
//His 'p' to pause/unpause, ESC to quit.
//


#include "stdafx.h"

#include<stdio.h>
#include <stdlib.h>

const int board_dt = 20;//wait 20 frames per chessboard view
//int n_boards = 0;//will be set by input list
//
//int board_w;
//int board_h;

int calib(/*int argc, char* argv[]*/int board_w,int board_h,int n_boards) {
	//if (argc != 4) {
	//	printf("ERROR: Wrong number of input parameters\n");
	//	return -1;
	//}
	//board_w = atoi(argv[0]);
	//board_h = atoi(argv[1]);
	//n_boards = atoi(argv[2]);

	int board_n = board_w*board_h;
	CvSize board_sz = cvSize(board_w, board_h);
	CvCapture* capture = cvCreateCameraCapture(0);
	assert(capture);

	cvNamedWindow("Calibration");

	//Allocate storage
	CvMat* image_points = cvCreateMat(n_boards*board_n, 2, CV_32FC1);
	CvMat* object_points = cvCreateMat(n_boards*board_n, 3, CV_32FC1);
	CvMat* point_counts = cvCreateMat(n_boards, 1, CV_32SC1);
	CvMat* intrinsic_matrix = cvCreateMat(3, 3, CV_32FC1);
	CvMat* distortion_coeffs = cvCreateMat(5, 1, CV_32FC1);

	CvPoint2D32f* corners = new CvPoint2D32f[board_n];
	int corner_count;
	int successes = 0;
	int step, frame = 0;
	IplImage* image = cvQueryFrame(capture);
	IplImage* gray_image = cvCreateImage(cvGetSize(image), 8, 1);//subpixel

	//capture corner views loop until we've got n_boards successful captures (all corners on the board are found.

	while (successes < n_boards) {
		//skip every board_dt frames to allow user to move chessboard
		if (frame++%board_dt == 0) {
			//find chessboard corners:
			int found = cvFindChessboardCorners(image, board_sz, corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

			//Get subpixel accuracy on those corners
			cvCvtColor(image, gray_image, CV_BGR2GRAY);
			cvFindCornerSubPix(gray_image, corners, corner_count, cvSize(11, 11), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

			//draw it
			cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);
			cvShowImage("Calibration", image);

			//if we got a good board, add it to our data
			if (corner_count == board_n) {
				step = successes*board_n;
				for (int i = step, j = 0; j < board_n; i++, j++) {
					CV_MAT_ELEM(*image_points, float, i, 0) = corners[j].x;
					CV_MAT_ELEM(*image_points, float, i, 1) = corners[j].y;
					CV_MAT_ELEM(*object_points, float, i, 0) = j / board_w;
					CV_MAT_ELEM(*object_points, float, i, 1) = j % board_w;
					CV_MAT_ELEM(*object_points, float, i, 2) =  0.0f;
				}
				CV_MAT_ELEM(*point_counts, int, successes, 0) = board_n;
				successes++;
			}
		
		}//end skip board_dt between chessboard capture
		
		//handle pause/unpause and ESC
		int c = cvWaitKey(15);
		if (c == 'p') {
			c = 0;
			while (c != 'p'&&c != 27) {
				c = cvWaitKey(250);
			}
		}
		if (c == 27) 
			return 0;
			image = cvQueryFrame(capture);//get next image
		}//end collection while loop.


	//allocate matrices according to how many chessboards found
	CvMat* object_points2 = cvCreateMat(successes*board_n, 3, CV_32FC1);
	CvMat* image_points2 = cvCreateMat(successes*board_n, 2, CV_32FC1);
	CvMat* point_counts2 = cvCreateMat(successes, 1, CV_32SC1);

	//transfer the points into the correct size matrices
	//below, we write out the details in the next two loops.
	// we could instead have written:
	//image_points->rows = object_points->rows = successes*board_n;point_counts->rows = successes;
	for (int i = 0; i < successes*board_n; i++) {
		CV_MAT_ELEM(*image_points2, float, i, 0) =	\
			CV_MAT_ELEM(*image_points, float, i, 0);
		CV_MAT_ELEM(*image_points2,float,i,1)=\
			CV_MAT_ELEM(*image_points, float, i, 1);
		CV_MAT_ELEM(*object_points2, float, i, 0)=\
			CV_MAT_ELEM(*object_points, float, i, 0);
		CV_MAT_ELEM(*object_points2, float, i, 1) = \
			CV_MAT_ELEM(*object_points, float, i, 1);
		CV_MAT_ELEM(*object_points2, float, i, 2)=\
			CV_MAT_ELEM(*object_points, float, i, 2);
	}

	for (int i = 0; i < successes; i++) {
		//these are all the same number
		CV_MAT_ELEM(*point_counts2, int, i, 0) = CV_MAT_ELEM(*point_counts, int, i, 0);
	}
	cvReleaseMat(&object_points);
	cvReleaseMat(&image_points);
	cvReleaseMat(&point_counts);

	//at this point we have all of the chessboard corners we need.
	//initialize the intrinsic matrix such that the two focal lengths have ratio of 1.0
	CV_MAT_ELEM(*intrinsic_matrix, float, 0, 0) = 1.0f;
	CV_MAT_ELEM(*intrinsic_matrix, float, 1, 1) = 1.0f;

	//calibrate the camera
	cvCalibrateCamera2(
		object_points2, image_points2,
		point_counts2, cvGetSize(image),
		intrinsic_matrix, distortion_coeffs,
		NULL, NULL, 0//CV_CALIB_FIX_ASPECT_RATIO
		);
	//save the intrinsics and distortions
	cvSave("Intrinsics.xml", intrinsic_matrix);
	cvSave("Distortion.xml", distortion_coeffs);

	//example of loading these matrices back in:
	CvMat* intrinsic = (CvMat*)cvLoad("Intrinsics.xml");
	CvMat* distortion = (CvMat*)cvLoad("Distortion.xml");
		
		
	//build the undistort map that we will use for all subsequent frames.

	IplImage* mapx = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 1);
	IplImage* mapy = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 1);
	cvInitUndistortMap(intrinsic, distortion, mapx, mapy);
	//just run the camera to the screen, now showing the raw and the undistoried images.
	cvNamedWindow("Undistort");
	while (image) {
		IplImage* t = cvCloneImage(image);
		cvShowImage("Calibration", image);//show raw image
		cvRemap(t, image, mapx, mapy);//undistort image
		cvReleaseImage(&t);
		cvShowImage("Undistort", image);//show undistort image

		//handle pause/unpause and ESC
		int c = cvWaitKey(15);
		if (c == 'p') {
			c = 0;
			while (c != 'p'&&c != 27) {
				c = cvWaitKey(250);
			}
		}
		if (c == 27) break;
				
			image = cvQueryFrame(capture);//get next image
	}
	return 0;
}
