#include"stdafx.h"
int write_to_avi(char* input,char* output) {
	//输出设备创建之后，可以通过调用cvWriteFrame()逐帧将视频流写入文件
	//写入结束后，调用cvReleaseVideoWriter()来释放资源

	// convert a video to grayscale
	//argv[1]: input video file
	// argv[2]: name of new output file

	CvCapture* capture = 0;
	capture = cvCreateFileCapture(input);
	if (!capture) {
		return -1;
	}
	IplImage* bgr_frame = cvQueryFrame(capture);//Init the video read
	double fps = cvGetCaptureProperty(
		capture,
		CV_CAP_PROP_FPS);
	CvSize size = cvSize(
		(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH),
		(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT)
		);
	CvVideoWriter* writer = cvCreateVideoWriter(
		output,
		CV_FOURCC('M', 'J', 'P', 'G'),
		fps,
		size);

	IplImage* logpolar_frame = cvCreateImage(
		size,
		IPL_DEPTH_8U,
		3
		);
	while ((bgr_frame = cvQueryFrame(capture)) != NULL)
	{
		cvLogPolar(bgr_frame,
			logpolar_frame,
			cvPoint2D32f(bgr_frame->width/2,bgr_frame->height / 2),
			40, 
			CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
		// cvLogPolar最后一个参数flags默认值为9，上面这两个枚举常量相加刚好为9

		cvWriteFrame(writer, logpolar_frame);

	}

	cvReleaseVideoWriter(&writer);
	cvReleaseImage(&logpolar_frame);
	cvReleaseCapture(&capture);
	return 0;

}

/*打开一个视频文件，通过cvQueryFrame()函数读入视频；然后使用cvGetCaptureProperty()来获得视频流的各种重要属性。
将视频图像转换为对数极坐标格式，将转换后的图像逐帧写入视频文件，直到读入结束。

在调用函数cvCreateVideoWriter()时，第一个参数是用来指定新建视频文件的名称，第二个参数是视频压缩的编码格式。

无论采取哪种编解码格式，必须保证自己的电脑中有这种编解码器。
OpenCV用宏CV_FOURCC()来指定编码格式，其包含4个字符参数，构成编解码器的4字标记。
Motion JPEG编码格式的四字标记就是MJPG。

后面两个参数用来指定播放的帧率和视频图像的大小。
*/