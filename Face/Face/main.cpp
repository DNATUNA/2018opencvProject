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

			// 이미지의 픽셀 값의 r, g, b 값의 각각 합을 구함
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

			// r, g, b 값의 평균 산출
			B /= cnts;
			G /= cnts;
			R /= cnts;

			// 모자이크 색상 생성
			cv::Scalar color;
			color.val[0] = B;
			color.val[1] = G;
			color.val[2] = R;

			// 프레임에 모자이크 이미지 삽입
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
*   메인 메소드
*/
int main(int argc, char *argv[]) {
	// 웹 캠 생성
	cv::VideoCapture capture(0);

	// 웹 캠을 실행하지 못한 경우 에러 출력 및 종료
	if (!capture.isOpened()) {
		std::cerr << "웹 캠 디바이스를 찾을 수 없습니다." << std::endl;
		return 0;
	}

	// 윈도우 생성
	cv::namedWindow(WIN_NAME, 1);

	// 얼굴인식 템플릿 설정
	cv::CascadeClassifier face_classifier;

	if (!face_classifier.load("haarcascade_frontalface_alt.xml")) {
		printf("Unable to load classifier  XML");
		return 0;
	}

	// 모자이크 효과 설정 변수
	bool isOnMosaicEffect = false;

	std::cout << "'esc' 키를 이용하여 프로그램 종료 " << std::endl;

	while (true) {
		bool isFrameValid = true;
		cv::Mat frameOriginalMat;
		cv::Mat frame;

		try {
			// 웹 캠 프레임의 원본 크기 저장
			capture >> frameOriginalMat;

			// 원본 크기의 1/2로 축소 (프레임의 크기가 클 경우 연산시간이 증가)
			cv::resize(frameOriginalMat, frame, cv::Size(frameOriginalMat.cols, frameOriginalMat.rows), 0, 0, CV_INTER_NN);
		}
		catch (cv::Exception& e) {
			// 에러 출력
			std::cerr << "프레임 축소에 실패했기에, 해당 프레임을 무시합니다." << e.err << std::endl;
			isFrameValid = false;
		}

		// 프레임 크기 축소에 성공한 경우 얼굴인식
		if (isFrameValid) {
			try {
				// 프레임을 그레이 스케일 및 이퀄라이즈 처리
				cv::Mat grayframe;
				cv::cvtColor(frame, grayframe, CV_BGR2GRAY);
				cv::equalizeHist(grayframe, grayframe);
				std::vector<cv::Rect> faces;

				// 얼굴인식 템플릿을 이용하여 얼굴인식
				face_classifier.detectMultiScale(
					grayframe, faces,
					FACE_SEARCH_SCALE,
					MERGE_DETECTED_GROUP_CNTS,
					CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE,
					cv::Size(FACE_FRAME_WIDTH, FACE_FRAME_HEIGHT)
					);

				for (int i = 0; i < faces.size(); i++) {
					if (isOnMosaicEffect) {
						// 모자이크 사각형 출력
						drawMosaicRectangle(frame, faces[i]);
					}

					else {
						// 얼굴인식 사각형 틀 출력
						cv::Point lb(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
						cv::Point tr(faces[i].x, faces[i].y);
						cv::rectangle(frame, lb, tr, cv::Scalar(0, 0, 255), FACE_FRAME_THICKNESS, 4, 0);
					}
				}

				// 윈도우에 결과 출력
				cv::imshow(WIN_NAME, frame);
			}
			catch (cv::Exception& e) {
				std::cerr << "얼굴인식 처리에 실패했기에, 해당 프레임을 무시합니다." << e.err << std::endl;
			}
		}

		int keyCode = cv::waitKey(30);

		// esc 키가 눌리면 프레임 캡쳐 종료
		if (keyCode == 27) {
			break;
		}

		// m 키가 눌리면 모자이크 효과 on, off 변경
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