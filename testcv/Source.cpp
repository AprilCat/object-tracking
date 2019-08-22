#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/video.hpp>
#include<iostream>
#include<string>
#include<tracking_utility.hpp>
#include<kcftracker.hpp>
#include<cstdio>
#include<chrono>
using namespace std;
using namespace cv;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> timer;
typedef std::chrono::duration<float> duration;
int main()
{
	KCFTracker* tracker = nullptr;
	TrackingUtility tu;
	string windowName = "usb";
	namedWindow(windowName);
	cv::setMouseCallback(windowName, TrackingUtility::mouseCallback, (void*)&tu);
	Rect roi(0, 0, 0, 0);
	char message1[100];
	char message2[100];
	Mat img = imread("D:\\v2.jpg");
	Mat meow;
	VideoCapture vc(0);
	vc >> meow;
	while (!meow.empty()) {
		char c = waitKey(10);
		if (c == 27) {
			break;
		}
		tu.getKey(c);
		int dx = 0;
		int dy = 0;
		int yawRate = 0;
		int pitchRate = 0;
		timer trackerStartTime, trackerFinishTime;
		duration trackerTimeDiff;
		switch (tu.getState())
		{
		case TrackingUtility::STATE_IDLE:
			roi = tu.getROI();
			cout << "please select ROI and press g" << endl;
			break;
		case TrackingUtility::STATE_INIT:
			cout << "g pressed, initialize the tracker" << endl;
			roi = tu.getROI();
			tracker = new KCFTracker(true, true, false, false);
			tracker->init(roi, meow);
			tu.startTracker();
			break;
		case TrackingUtility::STATE_ONGOING:
			trackerStartTime = std::chrono::high_resolution_clock::now();
			roi = tracker->update(meow);
			trackerFinishTime = std::chrono::high_resolution_clock::now();
			trackerTimeDiff = trackerFinishTime - trackerStartTime;
			cout << "Tracking: bounding box update time = " << trackerTimeDiff.count() * 1000.0 << "ms" << endl;
			dx = (int)(roi.x + roi.width / 2 - meow.rows / 2);
			dy = (int)(roi.y + roi.height / 2 - meow.cols / 2);
			yawRate = dx;
			pitchRate = -dy;
			if (abs(dx) < 10) {
				yawRate = 0;
			}
			if (abs(dy) < 10) {
				pitchRate = 0;
			}
			break;
		case TrackingUtility::STATE_STOP:
			cout << "s pressed, stop tracker" << endl;
			delete tracker;
			tracker = NULL;
			tu.stopTracker();
			roi = tu.getROI();
			break;
		default:
			break;
		}
		dx = roi.x + roi.width / 2 - meow.rows / 2;
		dy = roi.y + roi.height / 2 - meow.cols / 2;
		cv::circle(meow, Point(meow.rows / 2, meow.cols / 2), 5, cv::Scalar(255, 0, 0), 2, 8);
		if (roi.width != 0) {
			cv::circle(meow, Point(roi.x + roi.width / 2, roi.y + roi.height / 2), 3, cv::Scalar(0, 0, 255), 1, 8);
			cv::line(meow, Point(meow.rows / 2, meow.cols / 2),
				Point(roi.x + roi.width / 2, roi.y + roi.height / 2),
				cv::Scalar(0, 255, 255));
		}

		cv::rectangle(meow, roi, cv::Scalar(0, 255, 0), 1, 8, 0);
		//cvtColor(meow, meow, COLOR_RGB2BGR);
		sprintf_s(message1, "dx=%04d, dy=%04d", dx, dy);
		putText(meow, message1, Point2f(20, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));
		putText(meow, message2, Point2f(20, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0));
		cv::imshow(windowName, meow);
		imshow("usb", meow);
		vc >> meow;
	}
	delete tracker;
	namedWindow("v2.jpg");
	imshow("v2.jpg", img);
	waitKey();
	return 0;
}

