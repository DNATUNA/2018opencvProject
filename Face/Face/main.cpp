#include<opencv2\objdetect\objdetect.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>

#include<iostream>
#include<cstdio>

#define WIN_NAME "Face ver.KGH"
#define FACE_CLASSIFIER_PATH "haarcascade_frontalface_default.xml"
#define FACE_SEARCH_SCALE 1.1
#define MERGE_DETECTED_GROUP_CNTS 3
#define FACE_FRAME_WIDTH 50
#define FACE_FRAME_HEIGHT 50
#define FACE_FRAME_THICKNESS 1


using namespace std;
using namespace cv;

void drawMosaicRectangle(cv::Mat frame, cv::Rect face) {
	int cnts = 0;
	int mb = 9;
	int wPoint = 0;
	int hPoint = 0;
	int xStartPoint = 0;
	int yStartPoint = 0;
	double R = 0;
	double G = 0;
	double B = 0;

	for (int i = 0; i < face.height / mb; i++) {
		for (int j = 0; j < face.width / mb; j++) {
			cnts = 0;
			B = 0;
			G = 0;
			R = 0;
			xStartPoint = face.x + (j * mb);
			yStartPoint = face.y + (i * mb);

			// �̹����� �ȼ� ���� r, g, b ���� ���� ���� ����
			for (int mbY = yStartPoint; mbY < yStartPoint + mb; mbY++) {
				for (int mbX = xStartPoint; mbX < xStartPoint + mb; mbX++) {
					wPoint = mbX;
					hPoint = mbY;

					if (mbX >= frame.cols) {
						wPoint = frame.cols - 1;
					}
					if (mbY >= frame.rows){
						hPoint = frame.rows - 1;
					}

					cv::Vec3b color = frame.at<cv::Vec3b>(hPoint, wPoint);
					B += color.val[0];
					G += color.val[1];
					R += color.val[2];
					cnts++;
				}
			}

			// r, g, b ���� ��� ����
			B /= cnts;
			G /= cnts;
			R /= cnts;

			// ������ũ ���� ����
			cv::Scalar color;
			color.val[0] = B;
			color.val[1] = G;
			color.val[2] = R;

			// �����ӿ� ������ũ �̹��� ����
			cv::rectangle(
				frame,
				cvPoint(xStartPoint, yStartPoint),
				cvPoint(xStartPoint + mb, yStartPoint + mb),
				color,
				CV_FILLED,
				8,
				0
				);
		}
	}
}

/**
*   ���� �޼ҵ�
*/
int main(int argc, char *argv[]) {
	// �� ķ ����
	cv::VideoCapture capture(0);

	// �� ķ�� �������� ���� ��� ���� ��� �� ����
	if (!capture.isOpened()) {
		std::cerr << "�� ķ ����̽��� ã�� �� �����ϴ�." << std::endl;
		return 0;
	}

	// ������ ����
	cv::namedWindow(WIN_NAME, 1);

	// ���ν� ���ø� ����
	cv::CascadeClassifier face_classifier;

	if (!face_classifier.load("haarcascade_frontalface_alt.xml")) {
		printf("Unable to load classifier  XML");
		return 0;
	}

	// ������ũ ȿ�� ���� ����
	bool isOnMosaicEffect = false;

	std::cout << "'esc' Ű�� �̿��Ͽ� ���α׷� ���� " << std::endl;

	while (true) {
		bool isFrameValid = true;
		cv::Mat frameOriginalMat;
		cv::Mat frame;

		try {
			// �� ķ �������� ���� ũ�� ����
			capture >> frameOriginalMat;

			// ���� ũ���� 1/2�� ��� (�������� ũ�Ⱑ Ŭ ��� ����ð��� ����)
			cv::resize(frameOriginalMat, frame, cv::Size(frameOriginalMat.cols, frameOriginalMat.rows), 0, 0, CV_INTER_NN);
		}
		catch (cv::Exception& e) {
			// ���� ���
			std::cerr << "������ ��ҿ� �����߱⿡, �ش� �������� �����մϴ�." << e.err << std::endl;
			isFrameValid = false;
		}

		// ������ ũ�� ��ҿ� ������ ��� ���ν�
		if (isFrameValid) {
			try {
				// �������� �׷��� ������ �� ���������� ó��
				cv::Mat grayframe;
				cv::cvtColor(frame, grayframe, CV_BGR2GRAY);
				cv::equalizeHist(grayframe, grayframe);
				std::vector<cv::Rect> faces;

				// ���ν� ���ø��� �̿��Ͽ� ���ν�
				face_classifier.detectMultiScale(
					grayframe, faces,
					FACE_SEARCH_SCALE,
					MERGE_DETECTED_GROUP_CNTS,
					CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE,
					cv::Size(FACE_FRAME_WIDTH, FACE_FRAME_HEIGHT)
					);

				for (int i = 0; i < faces.size(); i++) {
					if (isOnMosaicEffect) {
						// ������ũ �簢�� ���
						drawMosaicRectangle(frame, faces[i]);
					}

					else {
						// ���ν� �簢�� Ʋ ���
						cv::Point lb(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
						cv::Point tr(faces[i].x, faces[i].y);
						cv::rectangle(frame, lb, tr, cv::Scalar(0, 0, 255), FACE_FRAME_THICKNESS, 4, 0);
					}
				}

				// �����쿡 ��� ���
				cv::imshow(WIN_NAME, frame);
			}
			catch (cv::Exception& e) {
				std::cerr << "���ν� ó���� �����߱⿡, �ش� �������� �����մϴ�." << e.err << std::endl;
			}
		}

		int keyCode = cv::waitKey(30);

		// esc Ű�� ������ ������ ĸ�� ����
		if (keyCode == 27) {
			break;
		}

		// m Ű�� ������ ������ũ ȿ�� on, off ����
		else if (keyCode == 109) {
			if (isOnMosaicEffect) {
				isOnMosaicEffect = false;
			}

			else {
				isOnMosaicEffect = true;
			}
		}
	}

	return 0;
}