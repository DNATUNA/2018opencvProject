#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<cstdio>
#include<Windows.h>
#include<iostream>

int main(){
	CvCapture* capture = cvCaptureFromCAM(0);
	if (!capture){
		printf("the video file was not found.");
		return 0;
	}

	int width = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int height = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	CvSize size = cvSize(width, height);

	IplImage* bkgImag = cvLoadImage("CctvBaseFile.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (bkgImag == NULL) return  -1;

	IplImage* grayImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* diffImage = cvCreateImage(size, IPL_DEPTH_8U, 1);
	
	int nFrame = 0;
	int nThreshold = 60;
	int stack = 0;
	IplImage* frame = NULL;

	for (;;)
	{
		frame = cvQueryFrame(capture);
		if (!frame) break;
		nFrame++;
		cvCvtColor(frame, grayImage, CV_BGR2GRAY);
		cvAbsDiff(grayImage, bkgImag, diffImage);
		cvThreshold(diffImage, diffImage, nThreshold, 255, CV_THRESH_BINARY);

		double value = cvGetReal2D(diffImage, 255, 0);

		for (int i = 0; i<height; i += 10)
		{
			for (int j = 0; j<width; j += 10)
			{
				double value = cvGetReal2D(diffImage, i, j);
				printf("Data: %lf\n", value);
				if (value>100) stack++;
			}
		}
		if (stack>100)
			MessageBox(0, "침입자 발견", "경고", MB_ICONWARNING);

		stack = 0;
		cvShowImage("grayImage", grayImage);
		cvShowImage("diffImage", diffImage);
		char chKey = cvWaitKey(10);
		if (chKey == 27)
			break;
	}

	cvDestroyAllWindows();
	cvReleaseImage(&grayImage);
	cvReleaseImage(&diffImage);
	cvReleaseCapture(&capture);

	return 0;

}