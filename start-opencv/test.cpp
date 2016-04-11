#include"stdafx.h"

using namespace cv;

int test()
{
	//读入一张图片
	Mat img = imread("pic.jpg");
	//创建一个窗口
	cvNamedWindow("弹簧振子");
	//在窗口中显示游戏原画
	imshow("弹簧振子", img);
	//等待5000ms后窗口自动关闭
	waitKey(5000);
	return 0;
}