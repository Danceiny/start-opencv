#include"stdafx.h"
// We make this value global so everyone can see it.

int g_switch_value = 0;

// This will be the callback that we give to the trackbar.

void switch_callback(int position) {
	if (position == 0) {
		//switch_off_function();
		cam_bg_model(1, "no file");
	}
	else {
		//switch_on_function();
		imshow(1, "pic.jpg");
	}
}

int switch_gui() {
	// Name the main window

	cvNamedWindow("Demo Window", 1);

	// Create the trackbar. We give it a name, and 
	// tell it the name of the parent window.

	cvCreateTrackbar(
		"Switch",
		"Demo Window",
		&g_switch_value,
		1,
		switch_callback
		);

	// This will just cause OpenCV to idle until someone
	// hits the "Escape" key.

	while (1) {
		if (cvWaitKey(15) == 27)break;
	}
	return 0;
}
