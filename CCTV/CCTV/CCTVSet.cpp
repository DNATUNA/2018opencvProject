#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<cstdio>

int main(){

	CvCapture* capture = cvCaptureFromCAM(0);

	if (!capture){
		printf("the video file was not found.");
		return 0;
	}

	int width = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int height = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);

	CvSize frameSize = cvSize(width, height);

	IplImage* grayImage = cvCreateImage(frameSize, IPL_DEPTH_8U, 1);
	IplImage* sumImage = cvCreateImage(frameSize, IPL_DEPTH_32F, 1);
	cvZero(sumImage);

	IplImage* frame = NULL;
	int nFrameCount = 0;
	for (;;)
	{
		frame = cvQueryFrame(capture);
		if (!frame) break;
		cvCvtColor(frame, grayImage, CV_BGR2GRAY);
		cvAcc(grayImage, sumImage, NULL);

		cvShowImage("grayImage", grayImage);
		char chKey = cvWaitKey(50);
		if (chKey == 27)
			break;
		nFrameCount++;
	}
	cvScale(sumImage, sumImage, 1.0 / nFrameCount);
	cvSaveImage("CctvBaseFile.jpg", sumImage);
	cvDestroyAllWindows();
	cvReleaseImage(&sumImage);
	cvReleaseImage(&grayImage);
	cvReleaseCapture(&capture);

	return 0;

}