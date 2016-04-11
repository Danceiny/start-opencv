// c
// Convert histograms into signatures for EMD matching
// assume we already have 2D histograms hist1 and hist2
// that are both of dimension h_bins by s_bins (though for EMD,
// histograms don't have to match in size).

#include"stdafx.h"

int hist2sig(CvHistogram* hist1,CvHistogram* hist2,int h_bins, int s_bins) {
	int numrows = h_bins*s_bins;

	// Create matrices to store the signature in 
	CvMat* sig1 = cvCreateMat(numrows, 3, CV_32FC1);//1 count + 2 coords = 3
	CvMat* sig2 = cvCreateMat(numrows, 3, CV_32FC1);//sigs are of type float

	// Fill signatures for the two histograms
	for (int h = 0; h < h_bins; h++) {
		for (int s = 0; s < s_bins; s++) {
			float bin_val = cvQueryHistValue_2D(hist1, h, s);
			cvSet2D(sig1, h*s_bins + s, 0, cvScalar(bin_val));// bin value
			cvSet2D(sig1, h*s_bins + s, 1, cvScalar(h));//Coord 1
			cvSet2D(sig1, h*s_bins + s, 2, cvScalar(s));
			bin_val = cvQueryHistValue_2D(hist2, h, s);
			cvSet2D(sig2, h*s_bins + s, 0, cvScalar(bin_val));//bin value
			cvSet2D(sig2, h*s_bins + s, 1, cvScalar(h));//Coord 1
			cvSet2D(sig2, h*s_bins + s, 2, cvScalar(s));//Coord 2

		}
	}
	return 0;
}

// 利用EMD来度量两分布之间的相似性
// Do EMD and REPORT
int emd_compare(CvMat* sig1,CvMat* sig2){
	float emd = cvCalcEMD2(sig1, sig2, CV_DIST_L2);
	printf("%f;", emd);

	return 0;
}
