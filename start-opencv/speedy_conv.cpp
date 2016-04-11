// 用cvDFT()加快卷积的计算
// Use DFT to accelerate the convolution of array A by kernel B.
// Place the result in array V.

#include"stdafx.h"

void speedy_convolution(
	const CvMat* A,	// Size:M1 * N1;
	const CvMat* B,	// Size:M2 * N2;
	CvMat* C	// Size: (A->rows+B->rows-1)*(A->cols+B->cols-1)
	) {

	int dft_M = cvGetOptimalDFTSize(A->rows + B->rows - 1);
	int dft_N = cvGetOptimalDFTSize(A->cols + B->cols - 1);

	CvMat* dft_A = cvCreateMat(dft_M, dft_N, A->type);
	CvMat* dft_B = cvCreateMat(dft_M, dft_N, B->type);
	CvMat tmp;

	// copy A to dft_A and pad dft_A with zeros
	cvGetSubRect(dft_A, &tmp, cvRect(0, 0, A->cols, A->rows));
	cvCopy(A, &tmp);//&tmp是一个地址，是指向dft_A内部的子矩形的，所以复制给tmp就是复制给dft_A

	cvGetSubRect(dft_A, &tmp, cvRect(A->cols, 0, dft_A->cols, A->rows));
	//这个从横坐标A->cols开始截起，宽度为大矩形的宽dft_A->cols，按理说这个子矩形有部分是超出大矩形了的，
	//那么就是cvGetSubRect()自动截去了超出部分？
	//那么dft_A->cols改成dft_A->cols-A->cols也是可行的？
	cvZero(&tmp);

	// no need to pad bottom part of dft_A with zeros because of
	// use nonzero_rows parameter in cvDFT() call below

	cvDFT(dft_A, dft_A, CV_DXT_FORWARD, A->rows);//A的傅里叶变换

	// repeat the same with the second array
	cvGetSubRect(dft_B, &tmp, cvRect(0, 0, B->cols, B->rows));
	cvCopy(B, &tmp);
	cvGetSubRect(dft_B, &tmp, cvRect(B->cols, 0, dft_B->cols - B->cols, B->rows));
	//这里的dft_B->cols-B->cols验证了上面的猜想？还是说上面那个源代码出错？

	cvZero(&tmp);

	cvDFT(dft_B, dft_B, CV_DXT_FORWARD, B->rows);//B的傅里叶变换
	
	
	// or CV_DXT_MUL_CONJ to get correlation rather than convolution
	cvMulSpectrums(dft_A, dft_B, dft_A, 0);//A、B的傅里叶变换相乘

	// calculate only the top part
	cvDFT(dft_A, dft_A, CV_DXT_INV_SCALE, C->rows);//A、B的频域积逆变换到时域
	
	//原文为cvGetSubRect(dft_A, &tmp, cvRect(0, 0, conv->cols, C->rows));
	cvGetSubRect(dft_A, &tmp, cvRect(0, 0, C->cols, C->rows));
	
	cvCopy(&tmp, C);
	cvReleaseMat(&dft_A);
	cvReleaseMat(&dft_B);
}

