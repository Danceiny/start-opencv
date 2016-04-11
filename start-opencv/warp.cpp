// Usage: warp_affine <image>

#include"stdafx.h"

int warp_affine(char* filename)
{
	CvPoint2D32f srcTri[3], dstTri[3];
	CvMat* rot_mat = cvCreateMat(2, 3, CV_32FC1);
	CvMat* warp_mat = cvCreateMat(2, 3, CV_32FC1);
	IplImage *src, *dst;


	if (src = cvLoadImage(filename, 1)) {
		dst = cvCloneImage(src);
		dst->origin = src->origin;
		cvZero(dst);

		// Compute warp matrix

		srcTri[0].x = 0;// src Top left
		srcTri[0].y = 0;
		
		srcTri[1].x = src->width - 1;// src Top right
		srcTri[1].y = 0;

		srcTri[2].x = 0;	// src Bottom left offset
		srcTri[2].y = src->height - 1;

		dstTri[0].x = src->width*0.0;	// dst Top left
		dstTri[0].y = src->height*0.33;	
		
		dstTri[1].x = src->width*0.85;// dst Top right
		dstTri[1].y = src->height*0.25;
		
		dstTri[2].x = src->width*0.15;	// dst Bottom left offset
		dstTri[2].y = src->height*0.7;


		cvGetAffineTransform(srcTri, dstTri, warp_mat);
		cvWarpAffine(src, dst, warp_mat);
		
		// 检查效果（通过srcTri\dstTri硬关联的仿射变换）
		cvNamedWindow("src", 1);
		cvShowImage("src", src);

		cvNamedWindow("dst", 1);
		cvShowImage("dst", dst);

		Sleep(20000);

		cvCopy(dst, src);

		// Compute rotation matrix
		CvPoint2D32f center = cvPoint2D32f(
			src->width / 2,
			src->height / 2
			);
		double angle = -50.0;
		double scale = 0.6;
		//这个rot_mat的计算公式封装在函数中
		cv2DRotationMatrix(center, angle, scale, rot_mat);

		// Do the transformation
		cvWarpAffine(src, dst, rot_mat);

		cvNamedWindow("Affline_Transform", 1);
		cvShowImage("Affline_Transform", dst);
		cvWaitKey();
	}
	cvReleaseImage(&dst);
	cvReleaseMat(&rot_mat);
	cvReleaseMat(&warp_mat);
	return 0;
}


int warp_perspective(char* filename) {
	CvPoint2D32f srcQuad[4], dstQuad[4];
	CvMat* warp_matrix = cvCreateMat(3, 3, CV_32FC1);
	IplImage *src, *dst;
	if ((src = cvLoadImage(filename, 1))){
		dst = cvCloneImage(src);
		dst->origin = src->origin;
		cvZero(dst);

		srcQuad[0].x = 0;	// src Top left
		srcQuad[0].y = 0;
		srcQuad[1].x = src->width - 1;	// src Top right
		srcQuad[1].y = 0;
		srcQuad[2].x = 0;	// src Bottom left
		srcQuad[2].y = src->height - 1;
		srcQuad[3].x = src->width - 1;	//src Botttom right
		srcQuad[3].y = src -> height - 1;	

		dstQuad[0].x = src->width*0.05;	//dst Top left
		dstQuad[0].y = src->height*0.33;
		dstQuad[1].x = src->width*0.9;//dst Top right
		dstQuad[1].y = src->height*0.25;
		dstQuad[2].x = src->width*0.2;//dst Bottom left
		dstQuad[2].y = src->height*0.7;
		dstQuad[3].x = src->width*0.8;//dst Bot right
		dstQuad[3].y = src->height*0.9;

		cvGetPerspectiveTransform(
			srcQuad,
			dstQuad,
			warp_matrix
			);
		cvWarpPerspective(src, dst, warp_matrix);
		cvNamedWindow("Perspective_Warp", 1);
		cvShowImage("Perspective_Warp", dst);
		cvWaitKey();

		cvReleaseImage(&dst);
		cvReleaseMat(&warp_matrix);

	}
	/*cvReleaseImage(&dst);
	cvReleaseMat(&warp_matrix);*/
	//这两行代码是要放if里面呢还是外面呢？
	return 0;
}