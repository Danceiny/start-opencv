#include"stdafx.h"
int write_seq(CvMemStorage* storage) {
	CvSeqWriter writer;
	cvStartWriteSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage, &writer);
	for (int i = 0; i < 100; i++) {
		CvPoint pt;
		pt.x = rand() % 320;
		pt.y = rand() % 240;
		CV_WRITE_SEQ_ELEM(pt, writer);
	}
	CvSeq* seq = cvEndWriteSeq(&writer);
	return 0;
}

//int 