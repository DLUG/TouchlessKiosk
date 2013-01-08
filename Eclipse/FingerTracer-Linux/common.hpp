/*
 * Common.hpp
 *
 *  Created on: Nov 12, 2012
 *      Author: kiosk
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 1024

#define CAM_NUM 3
#define CAM_WIDTH 480
#define CAM_HEIGHT 320
#define OBJECT_NUM 2
#define TRUE 1
#define FALSE 0
#define DEBUG
#define DEBUG_SCREEN
#define CLICK_ENABLE
//#define CAM_BRIGHT 0.4

#define DETECT_SCALE 1

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>

#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::gpu;

const double CAM_BRIGHT[] = {
		0.4,
		0.4,
		0.5
};

// HaarCascade 경로명 지정
const String HAAR_DATA[] = {
//		"haar_data/point_left_24_1.xml",
//		"haar_data/point_right_24_1.xml"
//		"haar_data/point_48_1.xml",
//		"haar_data/point_24.xml",
//		"haar_data/right_result_2.xml",
//		"haar_data/right_result_2.xml",
//		"haar_data/capston3_convert_result.xml",
//		"haar_data/capston3_convert_result.xml",
		"haar_data/kintex_convert_result_2.xml",
		"haar_data/kintex_convert_result_2.xml",
		"/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml"
};

typedef struct object_t{
	int x;
	int y;
	int width;
	int height;
	CascadeClassifier cascade;
}object_t;

typedef struct object_gpu_t{
	int x;
	int y;
	int width;
	int height;
	int onoff;
	CascadeClassifier_GPU cascade;
}object_gpu_t;

#endif /* COMMON_HPP_ */
