//use kalman filter to model particle in circular trajectory.

#include "stdafx.h"

//#include"cvx_defs.h"

#define phi2xy(mat) cvPoint(cvRound(img->width/2+img->width/3*cos(mat->data.fl[0])),cvRound(img->height/2-img->width/3*sin(mat->data.fl[0])))

int kalman() {
	//Initialize, create kalman filter object, window, random number generator etc.
	cvNamedWindow("Kalman", 1);
	//接下来创建一个随机数发生器，一幅用于绘制的图像和Kalman滤波器结构。
	//需要指定Kalman滤波器状态变量的维数和测量变量的维数。
	CvRandState rng;
	cvRandInit(&rng, 0, 1, -1, CV_RAND_UNI);

	IplImage* img = cvCreateImage(cvSize(500, 500), 8, 3);
	CvKalman* kalman = cvCreateKalman(2, 1, 0);

	//为状态x_k、过程噪声w_k、测量值z_k、传递矩阵F创建矩阵并初始化为一些分布在0附近的合理随机数


	// state is (phi,delta_phi) - angle and angular velocity
	// initialize with random guess.
	CvMat* x_k = cvCreateMat(2, 1, CV_32FC1);
	cvRandSetRange(&rng, 0, 0.1, 0);
	rng.disttype = CV_RAND_NORMAL;
	cvRand(&rng, x_k);

	// process noise
	CvMat* w_k = cvCreateMat(2, 1, CV_32FC1);

	// measurements, only one parameter for angle
	CvMat* z_k = cvCreateMat(1, 1, CV_32FC1);
	cvZero(z_k);

	// Transition matrix 'F' describes relationship between model parameters at step k and at step k+1 (this is the "dynamics" in our model)

	const float F[] = { 1,1,0,1 };
	memcpy(kalman->transition_matrix->data.fl, F, sizeof(F));


	// Initialize other Kalman filter parameters
	cvSetIdentity(kalman->measurement_matrix, cvRealScalar(1));
	cvSetIdentity(kalman->process_noise_cov, cvRealScalar(1e-5));
	cvSetIdentity(kalman->measurement_matrix, cvRealScalar(1e-1));
	cvSetIdentity(kalman->error_cov_post, cvRealScalar(1));
	
	wprintf((wchar_t*)"红色：预测；\t绿色：观测；\t蓝色：实际；");

	// choose random initial state

	while(1){
		// predict point position
		const CvMat* y_k = cvKalmanPredict(kalman, 0);


		// generate measurement (z_k)
		cvRandSetRange(&rng, 0, sqrt(kalman->measurement_noise_cov->data.fl[0]), 0);
		cvRand(&rng, z_k);
		cvMatMulAdd(kalman->measurement_matrix, x_k, z_k, z_k);

		//画出由先前合成的观测值、由kalman滤波器预测的位置和基本状态所代表的三个点
		//plot points (eg convert to planar coordinates and draw)
		cvZero(img);
		cvCircle(img, phi2xy(z_k), 4, cvScalar(255, 0, 0));

		// observed state
		cvCircle(img, phi2xy(y_k), 4, cvScalar(0, 255, 0), 2);

		// *predicted* state
		cvCircle(img, phi2xy(x_k), 4, cvScalar(0, 0, 255));//real state
		cvShowImage("Kalman", img);

		// 到此开始下一次迭代
		// 首先调用Kalman滤波器并赋给最新测量值，然后产生过程噪声，然后对x_k乘以时间传递矩阵F完成一次迭代并加上过程噪声

		// adjust Kalman filter state
		cvKalmanCorrect(kalman, z_k);

		// apply the transition matrix 'F' (e.g., step time forward)
		// and also apply the "process" noise w_k.
		cvRandSetRange(&rng, 0, sqrt(kalman->process_noise_cov->data.fl[0]), 0);
		cvRand(&rng, w_k);
		cvMatMulAdd(kalman->transition_matrix, x_k, w_k, x_k);

		//exit if user hits 'Esc'
		if (cvWaitKey(100) == 27)break;
}
return 0;
}