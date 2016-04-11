#include"stdafx.h"
int write_xml()
{
	CvFileStorage* fs = cvOpenFileStorage(
		"cfg.xml",
		0,
		CV_STORAGE_WRITE
		);
	cvWriteInt(fs, "frame_count", 10);
	cvStartWriteStruct(fs, "frame_size", CV_NODE_SEQ);
	cvWriteInt(fs, 0, 320);
	cvWriteInt(fs, 0, 200);
	cvEndWriteStruct(fs);
	//void* cmatrix;
	
	//cvSave()\cvLoad()
	/*CvMat A = cvMat(5, 5, CV_32F, the_matrix_data);
	cvSave("my_matrix.xml",&A);
	...
	// to load it then in some other program use...
	CvMat* A1 = (CvMat*) cvLoad("my_matrix.xml");
	*/
	

	cvWrite(fs, "color_cvt_matrix", (void*)NULL);
	cvReleaseFileStorage(&fs);
	return 0;
}

int read_xml()
{
	CvFileStorage* fs = cvOpenFileStorage(
		"cfg.xml",
		0,
		CV_STORAGE_READ
		);
	int frame_count = cvReadIntByName(
		fs,
		0, "frame_count",
		5 /*default value*/
		);

	CvSeq* s = cvGetFileNodeByName(fs, 0, "frame_size")->data.seq;//结构体frame_size

	int frame_width = cvReadInt(
		(CvFileNode*)cvGetSeqElem(s, 0)
		);

	int frame_height = cvReadInt(
		(CvFileNode*)cvGetSeqElem(s, 1)
		);

	CvMat* color_cvt_matrix = (CvMat*)cvReadByName(
		fs,
		0,
		"color_cvt_matrix"
		);

	cvReleaseFileStorage(&fs);

	/*如果只有一个矩阵要读取，可以使用cvLoad()，但是如果矩阵是内嵌在一个较大的结构中，必须使用cvRead()。*/
	return 0;
}