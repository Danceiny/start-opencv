#pragma once
#ifndef _STDAFX_H
#define _STDAFX_H

#include <Windows.h>
#include <cv.h>
#include <cxcore.h>
#include <stdio.h>
#include <highgui.h>
#include <math.h>
#include <Windows.h>
//#include <WinUser.h>

/*include"difficult_transition.cpp"
#include"write_to_avi.cpp"
#include"read_from_cam.cpp"*/


//function 
IplImage* doCanny(IplImage*,double,double,int);
IplImage* doPyrDown(IplImage*, int);
void re_per_period(IplImage*);
int scale_twice_and_canny(IplImage*);

int read_from_cam(int, char*);
int write_to_avi(char*, char*);

int playAvi(char* filename);


int trackslide(char* filename);

int roi_add(const char* filename, int x, int y, int width, int height, int add);

int test();
int cam_bg_model(int argc, char* filename);
int imshow(int argc, char* filename);
int width_step_add(IplImage* interest_img, CvRect interest_rect);
int alphablend(char* src1, char* src2, int x, int y, int width, int height, double alpha, double beta);

int write_xml();

int callmouse();
int switch_gui();

int threshold_cmp(double threshold, int threshold_type, int adaptive_method, int block_size, double offset, char* img);

int find_circle(char* filename);

int warp_affine(char* filename);
int warp_perspective(char* filename);

int log_polar(char* filename, double m);

void speedy_convolution(
	const CvMat* A,	// Size:M1 * N1;
	const CvMat* B,	// Size:M2 * N2;
	CvMat* C	// Size: (A->rows+B->rows-1)*(A->cols+B->cols-1)
	);

int histogram(char* filename);

int template_match(char* src_img, char* tmp_img);

int write_seq(CvMemStorage* storage);

#ifdef __cplusplus
extern "C"{
#endif
void keystroke(char* rec); 
#ifdef __cplusplus
}
#endif

int gesture_main(/*nt argc, char** argv*/);

void on_trackbar_contour(int);
int do_contour(char* filename);
int do_contour_2(char* filename);
int birds_eys(int board_w, int board_h,/*CvMat* intr,CvMat* dist,*/char* intri, char* dist, char* img);


int lucas_kanade();

int nihe_2d();


#define CHANNELS 3
typedef struct ce {
	uchar learnHigh[CHANNELS];//high side threshold for learning
	uchar learnLow[CHANNELS];//low side threshold for learning
	uchar max[CHANNELS];//high side of box boundary
	uchar min[CHANNELS];//low side of box boundary
	int t_last_update;//allow us to kill stale(¾ÉµÄ) entries
	int stale;//max negative run (longest period of inactivity)
}code_element;

typedef struct code_book {
	code_element **cb;
	int numEntries;
	int t;//count every access
}codeBook;

#endif // !_STDAFX_H