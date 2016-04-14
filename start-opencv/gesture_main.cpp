/*		TO DO
Reset the imgLines matrix is left unchanged after 2 seconds - DONE
Re-initialize the matrix - DONE
Record Gesture by zero threshold - DONE
Record Gesture information in string - DONE
Compare Gesture string with pre-defined function string. - DONE
Translate the function string into actual string. - DONE
Optimizing Cap tracing.
*/

#include "stdafx.h"
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <sstream>
#include <stdlib.h>
#include <Windows.h>
#include <WinUser.h>

using namespace cv;
using namespace std;
using namespace std;

//void keystroke(char*);

int gesture_main(/*nt argc, char** argv*/)
{
	int flag = 0;
	//VideoCapture cap(1); //Capture the video from webcam
	VideoCapture cap(0); //Capture the video from webcam
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}
	//Filename
	int ct = 0;
	stringstream ss;
	string name = "Track";
	string type = ".png";
	ss << name << (ct + 0) << type;
	string filename = ss.str();

	namedWindow("Control", CV_WINDOW_NORMAL); //create a window called "Control"

											  //Color co-ordinates for orange
	int iLowH = 0;
	int iHighH = 212;

	int iLowS = 125;
	int iHighS = 237;

	int iLowV = 186;
	int iHighV = 255;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 255); //Hue (0 - 255)
	createTrackbar("HighH", "Control", &iHighH, 255);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	//Position variables
	int posX;
	int posY;
	int iLastX = -1;
	int iLastY = -1;
	int X;
	int Y;
	int i = 0;
	int j = 0;
	int gestnum = 0;
	char *recognize = (char*)calloc(5, sizeof(int));

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);

	//Record keeping variable
	int record = 0;

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat imgHSV;

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		Mat imgThresholded;

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

																									  //morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 10000)
		{
			//calculate the position of the ball
			posX = (int)dM10 / dArea;
			posY = (int)dM01 / dArea;

			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
				//Draw a red line from the previous point to the current point
				line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
			}

			record++;
			X = posX - iLastX;
			Y = posY - iLastY;
			if (i == 0)
			{
				i++;
			}
			else	//Assign ASCII code for direction letters - u d l r
			{
				if (X > 20 && Y < 20 && Y > -20)
				{
					printf("Left\n");
					gestnum = 108;
				}
				if (X < -20 && Y < 20 && Y > -20)
				{
					printf("Right\n");
					gestnum = 114;
				}

				if (Y > 20 && X < 20 && X > -20)
				{
					printf("Down\n");
					gestnum = 100;
				}
				if (Y < -20 && X < 20 && X > -20)
				{
					printf("Up\n");
					gestnum = 117;
				}
			}

			iLastX = posX;
			iLastY = posY;
		}

		if (dArea == 0.00 && record != 0 || j == 3)
		{
			printf("Gesture recorded!\n");
			recognize[0] = 'g';		//Inherently, first element will be NULL if we don't do this
			printf("%s\n", recognize);
			keystroke(recognize);		//Keystroke function. See keystroke.cpp
			imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);	//Resetting the threshold matrix
			record = 0;
			iLastX = -1;
			iLastY = -1;
			i = 0;
			j = 0;
			gestnum = 0;
			free(recognize);
			recognize = (char*)calloc(5, sizeof(int));
			Sleep(500); //idles the program for 2 seconds. Enough time to zero out the threshold
		}

		if (recognize[j] != gestnum)		//Recording direction to the gesture array
		{
			j++;
			recognize[j] = gestnum;
		}

		imshow("Thresholded Image", imgThresholded); //show the thresholded image

		imgOriginal = imgOriginal + imgLines;
		imshow("Original", imgOriginal); //show the original image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;
}



//	# An Analysis of Hand Gesture Recognition
//	Arpit Goyal Manikanta Reddy D Sourav Anand R Sundararajan
//
//## ABSTRACT
//	In this era of where when we take a small step, technology takes a giant leap.Countless new innovation spawns each day which looks like something out of science fiction fantasy.Continuing on that trend, our current interface of communicating with computers have become quite old.Like the advents in technology, we need new technologies to interact with computers, technologies which can become more user friendly.Hence, we decided to make an interface through which we can easily communicate with the computer, a Gesture.
//	������������һС������������һ�󲽵�ʱ������������ʹ������������ƻ�С˵�еĴ��档�����������ֽ��������Ľ�����ʽ��Ȼ��ȥ������������٣�������Ҫ�µĸ����û��Ѻõļ����������������ˣ����Ǿ���ͨ��������һ�򵥷�ʽ����Ϊ�����ӿڡ�
//
//	This analysis utilises opensource libraries of OpenCV available for C++, to implement Gaussian Mixture Models and Color based models for Background subtraction and feature extraction.
//	�÷���ʹ���˶�C++���õ�OpenCV�Ŀ�Դ�⣬��ʵ�ָ�˹���ģ�ͺͻ�����ɫ�ı������ģ�ͺ�������ȡ��
//
//
//## WHAT IS GESTURE?
//	Loosely speaking, Gesture is a form of non - verbal communication which uses various parts of the body like arms and face to communicate.Gesture as a language is found in nature all around us, as animals, who lacks many distinct sounds to communicate effectively, rely mainly on gesture to do so.It was also our primitive language before we started weaving sounds together.
//	��˵����������һ��ʹ�����岻ͬ��λ�������ֱۺ���������н����ķ����ｻ����ʽ��������Ϊ��Ȼ�洦�ɼ������ԣ���ȱ��ֱ����Ч�����������Ķ����ǵ���Ҫ��ͨ��ʽ��Ҳ�����ǿ�ʼһͬ��֯��������֮ǰ����ѡ���ԡ�
//
//	To interact with the computer in a more natural way, we will go down to our basics - communicate with gesture.Give the computer a signal with hand, and the command will be executed.
//	Ϊ����һ�ָ�Ϊ��Ȼ�ķ�ʽ���������������ǻع���ġ��������ǵ����ơ��������һ�������źţ����ִ�С�
//
//## DEVELOPMENT
//### SOFTWARE USED
//	1.	Microsoft Visual Studio x64 2013 (Our IDE)
//	2.	OpenCV 2.4.9 (Libraries and function) The program was written in C++ language.
//
//
//### THE THOUGHT PROCESS
//	Prior and during development, the basic idea of the project was framed, the essential steps to make our idea a reality.We had certain checkpoints.Our task was
//	1.	To capture the image as a video feed from the camera feed.
//	2.	To track the gesture from the camera feed.
//	3.	To recognize the gesture made.
//	4.	To check the gesture made with the database.
//	5.	Execute the function on the match.
//
//	�ڿ���֮ǰ�������˸���Ŀ�Ļ���˼�롣����ʵ�����ǵ��뷨�ı�Ҫ���衣������ȷ���ļ��㡣���ǵ������ǣ�
//	1. �����ͨ����Ƶ������ͼ��
//	2. ���������Ƶ��׷�����ơ�
//	3. ʶ�����ơ�
//	4. �����ݿ�Աȼ�����ơ�
//	5. ִ��ƥ�亯����
//
//## THEORY
//	We focused mainly two main methods of gesture recognition, Gaussian model and color models.
//	������Ҫ��ע��������ʶ�𷽷�����˹ģ�ͺ���ɫģ�͡�
//
//### Gaussian Mixture Model
//	Every image is made up of pixels and pixels are store of numbers that decode information about the image.These images can be modeled as a set of features.Let us delve a bit more into this.An image can have various components, say a straight line, a sloped line etc.These components can be approximated as a sum / mixture of Gaussians.This is a statistical model and when we say a mixture of Gaussians we imply that at every pixel the numbers it contains are approximately due to a linear combination of Gaussians.So it must be evident that if something is in background and it remains stationary for a long time then its mixture must be fairly constant over time.This is the whole crux of our method.Now we can apply a temporal filter which can subtract these background mixtures from every frame of the input feed and extract the feature as an alpha frame.The background should also be re - estimated at regular intervals so as to maintain a good approximation of the background pixels.
//
//	ÿ��ͼ������������ɵģ����ر��洢Ϊ������ͼ����Ϣ�����ּ�����Щͼ���ͨ����ģ��Ϊ�����������˵��һ��ͼ���������ಿ�֣�����ֱ�ߡ�б�ߵȡ���Щ���ֿ��ɸ�˹��� / ����Ͻ��ơ�����һ�����ڼ����ͳ��ģ�ͣ����Ǽ�����ÿ�����ص��ϰ����������ǿ���ͨ����˹�ֲ���������Ͻ��Ƶġ��Զ��׼���������������屣�ֳ�ʱ�侲ֹ����ô���˹�ֲ���ϱ�Ȼ��ʱ�䲻��ġ������㷨�Ĺؼ����������ǿ���Ӧ��һ����ʱ�˲���������ÿһ֡��������ȥ������ϣ�����ȡ����Ϊalpha֡������ҲӦ�����ع��ƣ��Ա��ֱ������ص����ý��ơ�
//
//#### IMPLEMENTATION
//	OpenCV provides us with elegant mixture model based functions to estimate matrices of Gaussian��s of the background, which can then be subtracted.For good tracking of features tweaks must be made to number of n - mixtures used in the mixture model and at n = 6 we found an optimal balance between computational complexity and tracking precision any higher the computation is complex enough to slow do the program and an smaller gives us a very rough estimation which is prone to misinterpretation.
//
//	OpenCV�ṩ�˺ܺõĻ��ں����Ļ��ģ�͡��ú������Ʊ����ĸ�˹�ֲ�����Ϊ�����õ�����ƥ�䣬������ڻ��ģ����ʹ�õ�n��������ü������n = 6ʱ���Ƿ��ֽ������ӶȺ�׷�پ��ȵ�һ�����ƽ�⡣nȡ����ֵʱ�������㹻���������ڳ����ٶȹ�������nȡ��Сֵʱ���ƾ��Ȳ�����
//
//### Color based tracking
//	The color based model relies on a much simpler mode of feature extraction which is computationally lighter.Every color image lies in the Red - Green - Blue(R, G, B) color space.So every pixel has a coordinate information and then a color information encoded as(r, g, b).So if we are able to recognize the color of the tracking object fairly closely, we can track it to a very good approximation, by estimating its centroid.
//
//	������ɫ��ģ��������һ���򵥵ö��������ȡģʽ�����������١�ÿ����ɫͼ��λ��RGB��ɫ�ռ䣬���ÿ��������һ��������Ϣ������ɫ��Ϣ������Ϊ(r, g, b)����ˣ���������ܹ��൱����ʶ����׷���������ɫ������ͨ���������Ĵﵽһ���ǳ��õ�׷�ٽ��ƶȡ�
//
//	It is much simpler to work in Hue - Saturation - Value(HSV) in order to extract bright colors.HSV is an other color space, into which RGB can be easily converted.A HSV space is better to use because unlike RGB, HSV seperates luma, or the imageintensity, from chroma or the color information.
//
//	Ϊ��ȡ���ȵ㣬��HSV�¸�Ϊ�򵥡�HSV����һ����ɫ�ռ䣬RGB���Լ򵥵ر任�õ���HSV�ռ������ʹ�ã���ΪHSV��ͼ��ɫ�Ȼ���ɫ��Ϣ�з��������ȣ���ͼ��ǿ�ȣ������RGB��ͬ��
//
//	This is very much useful as we want to seperate color components from intensity for various reasons, such as robustness to lighting changes, removing shadows, etc.However HSV is one of many color spaces that seperate chroma from luma, like LAB, YCbCr, etc..HSV is being used because the code for converting from RGB to HSV is easily available and can be easily implemented.
//
//	���ڲ�ͬ��ԭ��������ձ仯��³���ԣ��Ƴ���Ӱ�ȣ�������Ҫ��ͼ��ǿ���з�����ɫ�ɷ֣����HSV�Ƿǳ����õġ�������HSVֻ�������ɫ�ռ��е�һ�֣�����LAB��YCBCr��һ������ɫ�Ⱥ����ȡ���Ϊ��RGBת��ΪHSV�Ĵ���ǳ��򵥿�ʵ�֣�����HSV����ʹ�á�
//
//	Now by thresholding the HSV values by a low and high bars we can extract the desired color.
//
//	����ͨ���趨HSV����ֵ�����ǿ�����ȡ��Ҫ����ɫ��
//
//#### IMPLEMENTATION
//	In our project, we used colour markers as trackers.That means our software will track the colour movement and record it.To recognize the colour, we converted our webcam feed��s RGB colour space into easy - to - manipulate HSV colour space by using cvtColor() function.Then by using inRange(), we assigned the range of desired colour between two adjustable H, S and V co - ordinates.This results in the formation of threshold matrix which can be interpretted as the alpha image of all points which have HSV value in the range of our two defined co - ordinates.Now as we have an alpha feed, we can start estimating the median in every frame and track it.This is indeed a very good approximation of the Gesture made by the hand.
//
//	�����ǵ���Ŀ�У�����ʹ����ɫ�����Ϊ׷����������ζ�����ǵ��������׷�ٲ���¼��ɫ���ƶ���Ϊʶ����ɫ��ͨ��cvtColor()�������ǽ�����ͷ����RGB��ɫ�ռ�ת��Ϊ�ײ�����HSV��ɫ�ռ䡣Ȼ��ͨ��ʹ��inRange()������Ŀ����ɫ�ڿɵ���H��S�Լ�V������趨��Χ����ʹ�ÿ����Ϊalphaͼ�����ֵ�����ʽ���ˣ�alphaͼ������е��HSVֵ������������������귶Χ�ڡ�������������alpha��������Կ�ʼ����ÿһ֡����ֵ��׷�١���ʵ���������Ƶ�һ���ܺõĽ��ơ�
//
//## RECOGNISING THE GESTURE
//	Now that we are done tracking and recording a gesture, we must get along with recognising the gesture.The current program can recognise only a few gestures.The gestures we are using are fundamentally quite easy to recognize, for all gestures in this projects are just permutations of 4 directional reading taken up to 3 times.
//
//	��Ȼ׷�ٲ���¼�����Ѿ���ɣ���������ʶ���ˡ���ǰ�ĳ���ֻ����ʶ�������ơ�����ʹ�õ����ƻ��������൱����ʶ��ģ���Ϊ��Щ���Ƶ����н�������ʱ3�ε�4�������ȡ��
//
//	That gives us more than enough, i.e. 64 gestures for this project.
//	���㹻�ˣ���64�������ء�
//
//
//	To recognize the direction, we based it on the difference in the co - ordinates of initial point and the current point in the threshold matrix.The recognized direction is registered only when the previous direction registered is not same as the current direction awaiting registry.This way, we can also record the change in direction and string out a gesture.
//
//	Ϊʶ�������ǻ�����ֵ�����еĳ�ʼ��͵�ǰ��������֡����������еı�ע�᷽��ͬʱ����ʶ��ķ���Żᱻע�ᡣ���������ǻ����Լ�¼����ĸı䣬�����������С�
//
//	To terminate the gesture mid - way, we just have to zero out the threshold matrix, i.e.remove the colour marker out of the webcam range.
//
//	��ֹ����ʱ��ֻ��Ҫ����ֵ�������㣬������ͷ��Χ���Ƴ���ɫ��ǡ�
//
//## FINDING A MATCH
//	The directions registered are labelled as ��l��, V, ��u�� and ��d��(quiet naturally they mean left, right, up, down).They are threaded together in a form of an array of characters for example{ urd }.They are then compared to pre - defined strings with strcmp() function to find a match.On finding the match, the if condition is initialized and a command is executed.
//	ע��ķ��򱻱��Ϊ'l', 'r', 'u'��'d'����ʾ�������£�����һ����ĸ����ʽ������{ urd })������Ȼ����Щ������Ԥ������ַ����Ƚϣ���Ѱ��ƥ�䡣�ҵ�ƥ��󣬳�ʼ��if������ִ��һ�����
//
//## EXECUTING A COMMAND
//	We also decide to for a recognized gesture to trigger an event.Upon recognition a gesture can perform a certain operation by utilizing keystrokes api provided by various Operating Systems.
//