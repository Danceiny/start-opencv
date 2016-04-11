#include"stdafx.h"

//用ImageROI来增加某范围的像素
int roi_add(const char* filename, int x, int y, int width, int height, int add)
{
	IplImage* src;
	if(src = cvLoadImage(filename, 1))
	{
		IplImage* origin;
		origin = cvLoadImage(filename);
		cvSetImageROI(src, cvRect(x, y, width, height));
		cvAddS(src, cvScalar(add), src);
		cvResetImageROI(src);
		cvNamedWindow("ROI_Add", 1);
		cvNamedWindow("Origin", 1);
		cvShowImage("ROI_Add", src);
		cvShowImage("Origin", origin);
		cvWaitKey();
	}
	return 0;
}

/*ROI要方便些，但是widthStep自有妙用。
有些时候在处理过程中，想在操作过程中设置和保持一幅图像的多个子区域出于活动状态，
但是ROI只能串行处理并且必须不断地设置和重置。*/
int width_step_add(IplImage* interest_img, CvRect interest_rect)
{
	// Assuming IplImage* interest_img;
	
	// and CvRect interest_rect;
	
	// Use widthStep to get a rigion of interest
	// <Ailternate method>

	IplImage* sub_img = cvCreateImageHeader(
		cvSize(
			interest_rect.width,
			interest_rect.height
			),
		interest_img->depth,
		interest_img->nChannels
		);

	sub_img->origin = interest_img->origin;

	sub_img->widthStep = interest_img->widthStep;

	sub_img->imageData = interest_img->imageData +
		interest_rect.y * interest_img->widthStep +
		interest_rect.x * interest_img->nChannels;

	cvAddS(sub_img, cvScalar(1), sub_img);
	cvReleaseImageHeader(&sub_img);

	/*掩码和模板，
	在代码示例中，cvAddS()函数允许第四个参数默认值为空：
	const CvArr* mask=NULL。
	这是一个8位单通道数组，允许把操作限制到任意形状的非0像素的掩码区。
	如果ROI随着掩码或者模板变化，进程将会被限制在ROI和掩码的交集区域。
	*/

	return 0;

}

/*alphablend
	src2中alpha融合ROI从(0,0)开始，src1中ROI从(x,y)开始
	*/
int alphablend(char* src1, char* src2, int x, int y, int width, int height, double alpha, double beta)
{
	IplImage *img1, *img2;
	if (((img1 = cvLoadImage(src1)))&&(img2 = cvLoadImage(src2)))
	{
		cvSetImageROI(img1, cvRect(x,y,width, height));

		cvSetImageROI(img2, cvRect(0,0, width, height));

		cvAddWeighted(img1, alpha, img2, beta, 0.0, img2);

		cvResetImageROI(img1);


		cvNamedWindow("Alpha_blend", 1);
		cvShowImage("Alpha_blend", img1);
		cvWaitKey();
		
	}
	return 0;
	
}