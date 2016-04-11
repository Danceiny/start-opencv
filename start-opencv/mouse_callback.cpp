#include"stdafx.h"

// An example program in which the user can draw boxes on the screen.

// Define our callback which we will install for mouse events.

void my_mouse_callback(int event, int x, int y, int flags, void* param);

CvRect box;
bool drawing_box = false;

// A little subroutine to draw a box onto an image

void draw_box(IplImage* img, CvRect rect0) {
	//绘制矩形的函数；调用cvRectangle()画矩形；注意与cvRect()的区别，前者可以再图片里画矩形，而后者只是单纯的画矩形；
	cvRectangle(
		img,
		cvPoint(box.x, box.y),
		cvPoint(box.x + box.width, box.y + box.height),
		cvScalar(0xff, 0x00, 0x00)//red
		);
}

int callmouse() {
	box = cvRect(-1, -1, 0, 0);
	IplImage* image = cvCreateImage(
		cvSize(1024, 768),
		IPL_DEPTH_8U,
		3
		);
	cvZero(image);
	IplImage* temp = cvCloneImage(image);

	cvNamedWindow("Box Example");

	/* Here is the crucial moment that we actually install
	the callback. Note that we set the value 'param' to be 
	the image we are working with so that the callback will
	have the image to edit.*/

	cvSetMouseCallback(
		"Box Example",
		my_mouse_callback,
		(void*)image
		);

	/* The main program loop. Here we copy the working image to the 'temp'
	image, and if the user is drawing, then put the currently contempleted box onto that temp
	image. display the temp image, and wait 15ms for a keystroke, then repeat...
	*/

	while (1) {
		cvCopyImage(image, temp);
		if (drawing_box) draw_box(temp, box);
		cvShowImage("Box Example", temp);
		if (cvWaitKey(15) == 27)break;

	}
 
	// Be tidy
	cvReleaseImage(&image);
	cvReleaseImage(&temp);
	cvDestroyWindow("Box Example");
	
	return 0;
}

/* This is our mouse callback. If the user presses the left button,
 we start a box. When the user releases that button, then we add the box
 to the current image. When the mouse is dragged (with the button down) 
 we resize the box. */

void my_mouse_callback(int event, int x, int y, int flags, void* param) 
//鼠标回调函数；参数分别为事件，鼠标坐标，有无热键按下，附加参数，这里附加参数为一个图片指针；
{
	IplImage* image = (IplImage*)param;
	switch (event) {
	case CV_EVENT_MOUSEMOVE: {//鼠标移动，来设置矩形的宽和高，注意宽高可能为负值，这样在鼠标按下要进行处理；
		if (drawing_box) {
			box.width = x - box.x;
			box.height = y - box.y;
		}
	}
	break;
	case CV_EVENT_LBUTTONDOWN: {//左键按下，把标志设置为1，产生一个矩形，宽高为0，其实为一个点
		drawing_box = true;
		box = cvRect(x, y, 0, 0);
	}
	break;
	case CV_EVENT_LBUTTONUP: { 
		//左键放开，标志位置0，如果宽高为负值，表明鼠标画矩形时，是像屏幕左上方拖动的，这时候把矩形原点变换到他的对角点上去，宽高变为正，在图片上画出这个矩形；
		drawing_box = false;
		if (box.width < 0) {
			box.x += box.width;
			box.width *= -1;
		}
		if (box.height < 0) {
			box.y += box.height;
			box.height *= -1;
		}
		draw_box(image, box);
	}						 
	break;
	
	}
}
