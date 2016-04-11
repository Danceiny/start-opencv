//Template matching.
//Usage: matchTemplate image template
	
#include"stdafx.h"

int template_match(char* src_img,char* tmp_img) {
	IplImage *src, *templ, *ftmp[6];//ftmp will hold results
	int i;
	// Read in the source image to be searched.
	if ((src = cvLoadImage(src_img, 1)) == 0) {
		printf("Error on reading src image %s\n", src_img);
		return -1;
	}
	// Read in the template to be used for matching.
	if ((templ = cvLoadImage(tmp_img, 1)) == 0) {
		printf("Error on reading template %s\n", tmp_img);
		return -1;
	}

	// Allocate output images
	int iwidth = src->width - templ->width + 1;
	int iheight = src->height - templ->height + 1;
	for (i = 0; i < 6; i++) {
		ftmp[i] = cvCreateImage(cvSize(iwidth, iheight), 32, 1);
	}

	// Do the matching of the template with the image
	for (i = 0; i < 6; i++) {
		cvMatchTemplate(src, templ, ftmp[i], i);
		cvNormalize(ftmp[i], ftmp[i], 1, 0, CV_MINMAX);
	}
	// Display
	cvNamedWindow("Template", 0);
	cvShowImage("Template", templ);

	cvNamedWindow("Image", 0);
	cvShowImage("Image", src);
	
	cvNamedWindow("SQDIFF", 0);
	cvShowImage("SQDIFF", ftmp[0]);
	
	cvNamedWindow("SQDIFF_NORMED", 0);
	cvShowImage("SQDIFF_NORMED", ftmp[1]);
	
	
	cvNamedWindow("CCORR", 0);
	cvShowImage("CCORR", ftmp[2]);

	cvNamedWindow("CCORR_NORMED", 0);
	cvShowImage("CCORR_NORMED", ftmp[3]);

	cvNamedWindow("CCOEFF", 0);
	cvShowImage("CCOEFF", ftmp[4]);

	cvNamedWindow("CCOEFF_NORMED", 0);
	cvShowImage("CCOEFF_NORMED", ftmp[5]);

	// Let user view results
	cvWaitKey(0);

	return 0;
}
