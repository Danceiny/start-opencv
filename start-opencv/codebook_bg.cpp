#include"stdafx.h"





// int update_codebook(uchar *p, codeBook &c, unsigned *cbBounds,int numChannels)
// Updates the codebook entry with a new data point
//
// p	pointer to a YUV pixel
// c	codebook for this pixel
// cbBounds learning bounds for codebook(Rule of thumb:10)
// numChannels	number of color channels we're learning

// NOTES:
// cvBounds must be of length equal to numChannels

// RETURN:
// codebook index

int update_codebook(uchar *p, codeBook &c, unsigned *cbBounds, int numChannels) {

	unsigned int high[3], low[3], n;
	for (n = 0; n < numChannels; n++) {
		high[n] = *(p + n) + *(cbBounds + n);
		if (high[n] > 255)high[n] = 255;
		low[n] = *(p + n) - *(cbBounds + n);
		if (low[n] < 0)low[n] = 0;
	}

	int matchChannel;
	int i;
	//See if this fits an existing codeword

	for (i = 0; i < c.numEntries; i++) {
		matchChannel = 0;
		for (n = 0; n < numChannels; n++) {
			if ((c.cb[i]->learnLow[n] <= *(p + n)) &&
				//found an entry for this channel
				(*(p + n) <= c.cb[i]->learnHigh[n]))
			{
				matchChannel++;
			}
		}


		if (matchChannel == numChannels)//if an entry was found
		{
			c.cb[i]->t_last_update = c.t;
			//adjust this codeword for the first channel
			for (n = 0; n < numChannels; n++) {
				if (c.cb[i]->max[n] < *(p + n)) {
					c.cb[i]->max[n] = *(p + n);
				}
				else if (c.cb[i]->min[n]>*(p + n)) {
					c.cb[i]->min[n] = *(p + n);
				}
			}
			break;
		}

		//overhead to track potential stale entries
		for (int s = 0; s < c.numEntries; s++) {
			//track which codebook entries are going stale:
			int negRun = c.t - c.cb[s]->t_last_update;
			if (c.cb[s]->stale < negRun)c.cb[s]->stale = negRun;
		}

		//enter a new codeword if needed
		if (i == c.numEntries)//if no existing codeword found, make one
		{
			code_element **foo = new code_element*[c.numEntries + 1];
			for (int ii = 0; ii < c.numEntries; ii++) {
				foo[ii] = c.cb[ii];
			}

			foo[c.numEntries] = new code_element;
			if (c.numEntries) delete[] c.cb;
			c.cb = foo;
			for (n = 0; n < numChannels; n++) {
				c.cb[c.numEntries]->learnHigh[n] = high[n];
				c.cb[c.numEntries]->learnLow[n] = low[n];
				c.cb[c.numEntries]->max[n] = *(p + n);
				c.cb[c.numEntries]->min[n] = *(p + n);
			}
			c.cb[c.numEntries]->t_last_update = c.t;
			c.cb[c.numEntries]->stale = 0;
			c.numEntries += 1;
		}

		//slowly adjust learning bounds
		for (n = 0; n < numChannels; n++) {
			if (c.cb[i]->learnHigh[n] < high[n])c.cb[i]->learnHigh[n] += 1;
			if (c.cb[i]->learnLow[n] > low[n])c.cb[i]->learnLow[n] -= 1;

		}
		
	}
	return i;
}


//学习有移动前景目标的背景

//int clear_stale_entries(codeBook &c)
//During learning, after you've learned for some period of time,
// periodically call this to clear out stale codebook entries

//c		codebook to clean up
// RETURN
// number of entries cleared

int clear_stale_entries(codeBook &c){
	int staleThresh = c.t >> 1;//总运行时间的一半，为经验值
	int *keep = new int[c.numEntries];
	int keepCnt = 0;
	// see which codebook entries are too stale
	for (int i = 0; i < c.numEntries; i++) {
		if (c.cb[i]->stale>staleThresh)
		{
			keep[i] = 0;//mark for destruction
		}
		else
		{
			keep[i] = 1;//mark to keep
			keepCnt += 1;
		}
	}

	// keep only the good
	c.t = 0;//full reset on stale tracking
	code_element **foo = new code_element *[keepCnt];
	int k = 0;
	for (int ii = 0; ii < c.numEntries; ii++) {
		if (keep[ii]) {
			foo[k] = c.cb[ii];
			//we have to refresh these entries for next clearStale
			foo[k]->t_last_update = 0;
			k++;
		}
	}
	//clean up
	delete[] keep;
	delete[] c.cb;
	c.cb = foo;
	int numCleared = c.numEntries - keepCnt;
	c.numEntries = keepCnt;
	return numCleared;
}


//背景差分：寻找前景目标
//uchar background_diff(uchar *p,codeBook &c,int minMod,int maxMod)
//
//Given a pixel and a codebook, determine if the pixel is covered by the codebook
//
//p	pixel pointer(YUV interleaved)
//c Codebook reference
//numChannels	number of channels we are testing
//maxMod	Add this (possibly negative) number onto max level when determining if new pixel is foreground
//minMod	Subract this (possibly negative) number from min level when determining if new pixel is foreground
//NOTES:
//minMode and maxMod must have length numChannels,
//e.g. 3channels=>minMod[3],maxMod[3]. There is one min and one max threshold per channel.
//
//RETURN:
// 0=>background,255=>foreground

uchar background_diff(
	uchar* p,
	codeBook& c,
	int numChannels,
	int* minMod,
	int* maxMod
	) {
	int matchChannel;
	int i;
	//see if this fits an existing codeword
	for (i = 0; i < c.numEntries; i++) {
		matchChannel = 0;
		for (int n = 0; n < numChannels; n++) {
			if ((c.cb[i]->min[n] - minMod[n] <= *(p + n)) &&
				(*(p + n) <= c.cb[i]->max[n] + maxMod[n])) {
				matchChannel++;//found an entry for this channel
			}
			else break;
		}
		if (matchChannel == numChannels) {
			break;//found an entry that matched all channels
		}

	}
	if (i >= c.numEntries)return 255;
	return 0;
}
//
//使用codebook背景分割技术，通常步骤如下：
//1. 使用函数update_codebook()在几秒或几分内训练一个基本的背景模型；
//2. 调用函数clear_stale_entries()清除stale索引；
//3. 调整阈值minMod和maxMod对已知前景达到最好的分割；
//4. 保持一个更高级别的场景模型；
//5. 通过函数background_diff()使用训练好的模型将前景从背景中分割出来；
//6. 定期更新学习的背景像素；
//7. 在一个频率较慢的情况下，用函数clear_stale_entries()定期清理stale的codebook索引。


//通常codebook的效果很好，训练和运行速度较快，但是不能很好处理不同模式的光。

// # 用于前景清除的连通部分
//讨论使用连通成分分析来清理原始分割图像的方法。
//这种分析的方式包含噪声输入掩模图像；然后利用形态学“开”操作将小的噪声置0，紧接着用“闭”操作重建由于“开”操作丢失的边缘部分。
//然后可以找到“足够大”存在的部分轮廓，有选择地统计这些片段，再恢复最大的轮廓。
//
//头文件定义如下：
//void find_connected_components(IplImage *mask, int poly1_hull0, float perimScale, int *num, CvRect *bbs, CvPoint *centers) 
//This cleans up the foreground segmentation mask derived from calls to backgroundDiff
//
//mask		Is a grayscale (8-bit depth) "raw" mask image that will be clean up
//
//Optional parameters:
//poly1_hull0		If set, approximate connected COMPONENT by default Polygon, or else convex hull(0)
//perimScale		Len = Image(width + height) / perimScale.If contour len < this, delete that contour(default:4)
//num				Maximum number of rectangles and / or centers to return; on return will contain number filled (default:NULL)
//bbs				Pointer to bounding box rectangle vector of length num.(default setting: NULL)
//centers			Pointer to contour centers vector of length num(default:NULL)
//
//void find_connected_components(
//	IplImage *mask,
//	int poly1_hull0 = 1,
//	float perimScale = 4,
//	int *num = NULL,
//	CvRect *bbs = NULL,
//	CvPoint *centers = NULL
//	);

//for connected componence:
//Approx.threshold - the bigger it is, the simpler is the boundary 

#define CVCONTOUR_APPROX_LEVEL 2

//how many iterations of erosion and/or dilation there should be 

#define CVCLOSE_ITR 1

void find_connected_components(
IplImage *mask,
int poly1_hull0,
float perimScale,
int *num,
CvRect *bbs,
CvPoint *centers){
	static CvMemStorage* mem_storage = NULL;
	static CvSeq* contours = NULL;

	// clean up ram mask
	cvMorphologyEx(mask, mask, 0, 0, CV_MOP_OPEN, CVCLOSE_ITR);
	cvMorphologyEx(mask, mask, 0, 0, CV_MOP_CLOSE, CVCLOSE_ITR);

	//现在噪声已经从掩模图像上清除了，找到了所有的轮廓
	// find contours around only bigger regions
	if(mem_storage == NULL) {
		mem_storage = cvCreateMemStorage(0);
	}
	else {
		cvClearMemStorage(mem_storage);
	}
	CvContourScanner scanner = cvStartFindContours(mask, mem_storage, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	//下一步丢弃过小的轮廓，用多边形或凸包拟合剩下的轮廓，复杂度由CVCONTOUR_APPROX_LEVEL设置。
	CvSeq* c;
	int numCont = 0;
	while ((c = cvFindNextContour(scanner)) != NULL) {
		double len = cvContourPerimeter(c);
		//calculate perimeter len threshold:
		double q = (mask->height + mask->width) / perimScale;
		//Get rid of blob if its perimeter is too small:
		if (len < q) {
			cvSubstituteContour(scanner, NULL);
		}
		else {
			// Smooth its edges if its large enough
			CvSeq* c_new;
			if (poly1_hull0) {
				//Polygonal approximation
				c_new = cvApproxPoly(c, sizeof(CvContour), mem_storage, CV_POLY_APPROX_DP, CVCONTOUR_APPROX_LEVEL, 0);
			}
			else {
				//convex hull of the segmentation
				c_new = cvConvexHull2(c, mem_storage, CV_CLOCKWISE, 1);
				
				}

			cvSubstituteContour(scanner, c_new);
			numCont++;
			}
		}
	contours = cvEndFindContours(&scanner);


//在前面的代码中，CV_POLY_APPROX_DP使得Douglas - Peucher拟合算法被调用，且CV_CLOCKWISE是默认的凸形轮廓方向。
//在将轮廓绘制到掩模图像之前，定义一些简单的绘制颜色：
// Just some convenience variables 
	const CvScalar CV_WHITE = CV_RGB(0xff, 0xff, 0xff);
	const CvScalar CV_BLACK = CV_RGB(0x00, 0x00, 0x00);

//下面的代码中，首先把掩模外的部分剔除，然后在掩模图像上绘出完整的轮廓。还要检查用户是否想手机轮廓的统计信息（外接矩形和中心）。
// paint the found regions back into the image
cvZero(mask);
IplImage* maskTemp;
//calc center of mass and/or bounding rectangles
if (NULL != num){
	//User wants to collect statistics
	int N = *num, numFilled = 0, i = 0;
	CvMoments moments;
	double M00, M01, M10;
	maskTemp = cvCloneImage(mask);
	for (i = 0, c = contours; c != NULL; c = c->h_next, i++) {
		if (i < N) {
			//only process up to *num of them
			cvDrawContours(maskTemp, c, CV_WHITE, CV_WHITE, -1, CV_FILLED, 8);
			//find the center of each contour
			if (centers != NULL) {
				cvMoments(maskTemp, &moments, 1);
				M00 = cvGetSpatialMoment(&moments, 0, 0);
				M10 = cvGetSpatialMoment(&moments, 1, 0);
				M01 = cvGetSpatialMoment(&moments, 0, 1);
				centers[i].x = (int)(M10 / M00);
				centers[i].y = (int)(M01 / M00);

			}

			// bounding rectangles around blobs
			if (bbs != NULL) {
				bbs[i] = cvBoundingRect(c);
			}
			cvZero(maskTemp);
			numFilled++;
		}
		//draw filled contours into mask
		cvDrawContours(mask, c, CV_WHITE, CV_WHITE, -1, CV_FILLED, 8);
	}//end looping over contours
	*num = numFilled;
	cvReleaseImage(&maskTemp);
}
//else just draw processed contours into the mask
else {
	// the user doesn't want statistics, just draw the contours
	for (c = contours; c != NULL; c = c->h_next) {
		cvDrawContours(mask, c, CV_WHITE, CV_BLACK, -1, CV_FILLED, 8);
	}
}

//至此已经得到一个很有用的程序，可从原始噪声掩模图像创建出完整的掩模图像。
}