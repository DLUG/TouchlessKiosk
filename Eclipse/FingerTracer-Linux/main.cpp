#define CAM_NUM 2
#define DETECT_NUM 3
#define TRUE 1
#define FALSE 1
#define DEBUG
#define DEBUG_SCREEN
#define CAM_BRIGHT 0.5

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::gpu;

Mat background[CAM_NUM];

// HaarCascade 경로명 지정
const String haarcascade[] = {
		//		"/usr/share/opencv/haarcascades/haarcascade_frontalface_alt_tree.xml",
		"/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml",
//		"/usr/local/share/OpenCV/lbpcascades/lbpcascade_frontalface.xml",
		"bottom2.xml",
		"rockdata.xml"
};

// DebugScreen 원 색 지정
#ifdef DEBUG_SCREEN
const static Scalar colors[3][8] =  {
		{
				CV_RGB(32, 0, 255),
				CV_RGB(64, 0, 255),
				CV_RGB(96, 0, 255),
				CV_RGB(128, 0, 255),
				CV_RGB(160, 0, 255),
				CV_RGB(192, 0, 255),
				CV_RGB(224, 0, 255),
				CV_RGB(255, 0, 255),
		},
		{
				CV_RGB(32, 255, 0),
				CV_RGB(64, 255, 0),
				CV_RGB(96, 255, 0),
				CV_RGB(128, 255, 0),
				CV_RGB(160, 255, 0),
				CV_RGB(192, 255, 0),
				CV_RGB(224, 255, 0),
				CV_RGB(255, 255, 0),
		},
		{
				CV_RGB(255, 0, 32),
				CV_RGB(255, 0, 64),
				CV_RGB(255, 0, 96),
				CV_RGB(255, 0, 128),
				CV_RGB(255, 0, 160),
				CV_RGB(255, 0, 192),
				CV_RGB(255, 0, 224),
				CV_RGB(255, 0, 255),
		},
};
#endif


typedef struct cam_params{
	int cam_num;
	Mat frame;
	Mat result_frame;
	Mat background;
	GpuMat gpuFrame;
	Mat beforeFrame;
}cam_params;

typedef struct detect_params{
	int x;
	int y;
	int weight;
	int height;
	CascadeClassifier_GPU cascade;

	cam_params* cam_param;
}detect_params;

typedef struct calc_object{
	int x;
	int y;
	int z;
} calc_object;

void detectAndDraw(cam_params& cam, double scale, detect_params* params, int params_cnt);

void* cam_thread_body (void* data);

void callbackButton(int val, void* data){

}


int main(){
	int detect_thr_id[CAM_NUM * DETECT_NUM];
	int cam_thr_id[CAM_NUM];
	pthread_t cam_thread[CAM_NUM];
	pthread_t detect_thread[DETECT_NUM];
	int status;
	int i, j;
	int detect_cnt = 0;

	Display *dpy = XOpenDisplay(NULL);
	Display *dpy2 = XOpenDisplay(NULL);

	char tmpChar[10];

	double scale = 2;

	int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	Window w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 200, 100, 0, blackColor, blackColor);


	cam_params cam_param[CAM_NUM];
	detect_params detect_param[CAM_NUM][DETECT_NUM];


	IplImage* iplImg[CAM_NUM];
	CvCapture* capture;
	Mat tmpBackground;
	Mat tmpBackground2;
	calc_object calc_objects[DETECT_NUM];

	cout << "Open Cams" << endl;

	for(i = 0; i < CAM_NUM; i++){
		cam_param[i].cam_num = i;
		cam_thr_id[i] = pthread_create(&cam_thread[i], NULL, cam_thread_body, (void *)&cam_param[i]);
		sleep(1);
		cam_param[i].frame.copyTo(cam_param[i].beforeFrame);
	}


	cout << "startDetect" << endl;

	for(i = 0; i < DETECT_NUM; i++){
		for(j = 0; j < CAM_NUM; j++){
			detect_param[j][i].x = 0;
			detect_param[j][i].y = 0;
			detect_param[j][i].cam_param = &cam_param[j];

			if(!detect_param[j][i].cascade.load(haarcascade[i])){
				cerr << "ERROR: Could not load classifier cascade" << endl;
				return -1;
			}
		}
	}

	/*
	cvNamedWindow("control", CV_WINDOW_NORMAL);
	char* button = "button1";
	createButton(button, callbackButton);
	 */


	int beforeX[DETECT_NUM];
	int beforeY[DETECT_NUM];
	int keycode = 0;
	int detect_switch = 0;

	XEvent event;

	int mouse_status = 0;
	int mouse_status_before = 0;

	while(1){
		for(i = 0; i < CAM_NUM; i++){
			char winName[8] = "result";
			winName[6] = 48 + i;
			winName[7] = 0;

			if(detect_switch){
				detectAndDraw(cam_param[i], scale , detect_param[i], DETECT_NUM);
			} else {
				cam_param[i].frame.copyTo(cam_param[i].result_frame);
			}
			cv::imshow(winName, (Mat)cam_param[i].result_frame);
		}

		if(detect_switch){
			if(detect_param[1][1].x != 0 && detect_param[1][1].y != 0){
				if(beforeX[1] != detect_param[1][1].x || beforeY[1] != detect_param[1][1].y){
					if(((beforeX[1] - detect_param[1][1].x < 10) && (beforeX[1] - detect_param[1][1].x > 0)) ||
							((detect_param[1][1].x - beforeX[1] < 10) && (detect_param[1][1].x - beforeX[1] > 0))){
						XWarpPointer(dpy, 0, w, 0, 0, 0, 0, (1320 - detect_param[1][1].x * 2), (detect_param[1][1].y * 3 - 120));
						XFlush(dpy);
					}
				}
			} else if(detect_param[1][2].x != 0){
				if(((detect_param[1][2].x - beforeX[1]) < 50) && ((detect_param[1][2].x - beforeX[1]) > -50)){
					cout << "CLICK!!" << endl;
					cout << "CLICK!!" << endl;
					cout << "CLICK!!" << endl;
					cout << "CLICK!!" << endl;

					memset (&event, 0x00, sizeof (event));
					//				event.xbutton.button = button;
					event.type = ButtonPress;
					event.xbutton.button = Button1;
					event.xbutton.same_screen = True;

					XQueryPointer(dpy2, RootWindow(dpy2, DefaultScreen(dpy2)),
							&event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root,
							&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);



					while(event.xbutton.subwindow)
					{
						event.xbutton.window = event.xbutton.subwindow;

						XQueryPointer(dpy2, event.xbutton.window,
								&event.xbutton.root, &event.xbutton.subwindow,&event.xbutton.x_root,
								&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
					}


					if(XSendEvent(dpy2, PointerWindow, True, 0xfff, &event) == 0)
						cout << "ERROR" << endl;

					XFlush(dpy2);
					usleep(200000);

					event.type = ButtonRelease;
					event.xbutton.state = 0x100;

					if(XSendEvent(dpy2, PointerWindow, True, 0xfff, &event) == 0)
						cout << "ERROR" << endl;

					XFlush(dpy2);
				}
			} else {
/*
				event.type = ButtonRelease;
				event.xbutton.state = 0x100;
				if (XSendEvent(dpy2, PointerWindow, True, 0xfff, &event) == 0) cerr << "Error to send the event!" << endl;
				XFlush (dpy2);
*/
			}



			for(int i = 0; i < DETECT_NUM; i++){
				if(detect_param[1][i].x != 0 && detect_param[1][i].y != 0){
					/*
					if(beforeX[i] != detect_param[1][i].x || beforeY[i] != detect_param[1][i].y){
						XWarpPointer(dpy, 0, w, 0, 0, 0, 0, (1280 - detect_param[1][i].x * 3), (detect_param[1][i].y * 2));
						XFlush(dpy);
					}
					 */



					calc_objects[i].x = detect_param[1][i].x - 320;
					calc_objects[i].y = detect_param[1][i].y;
					calc_objects[i].z = (640 - detect_param[1][i].x) + detect_param[0][i].x;

					beforeX[i] = detect_param[1][i].x;
					beforeY[i] = detect_param[1][i].y;
				}
			}


			cout << endl << endl;
			cout << "Face1 \tx: " << detect_param[0][0].x << ", \ty: " << detect_param[0][0].y << endl;
			cout << "Face2 \tx: " << detect_param[1][0].x << ", \ty: " << detect_param[1][0].y << endl;
			cout << "FrontHand1 \tx: " << detect_param[0][1].x << ", \ty: " << detect_param[0][1].y << endl;
			cout << "FrontHand2 \tx: " << detect_param[1][1].x << ", \ty: " << detect_param[1][1].y << endl;
			cout << "BottomHand1 \tx: " << detect_param[0][2].x << ", \ty: " << detect_param[0][2].y << endl;
			cout << "BottomHand2 \tx: " << detect_param[1][2].x << ", \ty: " << detect_param[1][2].y << endl;
			cout << endl;
			cout << "Face \t\tx: " << calc_objects[0].x << ", \ty: " << calc_objects[0].y << ", \tz: " << calc_objects[0].z << endl;
			cout << "FrontHand \tx: " << calc_objects[1].x << ", \ty: " << calc_objects[1].y << ", \tz: " << calc_objects[1].z << endl;
			cout << "BottomHand \tx: " << calc_objects[2].x << ", \ty: " << calc_objects[2].y << ", \tz: " << calc_objects[2].z << endl;
		}

		keycode = waitKey(1);
		if(keycode != -1)
			cout << keycode << endl;
		if(keycode == 1048681){
			cout << "Capture Background Picture" << endl;
			for(i = 0; i < CAM_NUM; i++){
				cam_param[i].frame.copyTo(cam_param[i].background);
			}
		} else if(keycode == 1048691){
			detect_switch++;
			detect_switch %= 2;
		} else if(keycode == 1048696){
			return 0;
		}

		//		sleep(1);
	}


	/*
	for(i = 0; i < CAMNUM; i++){
		pthread_join(p_thread[i], (void **) &status);
	}
	 */

	//	status = pthread_mutex_destroy(&mutex);

	return 0;
}

void extSubtract(GpuMat& src1, GpuMat& src2, GpuMat& dst){
	int i, j, k;

	Mat zero = Mat::zeros(src1.size(), CV_32F);
	Mat src1Mat, src2Mat;
	src1.download(src1Mat);
	src2.download(src2Mat);

	GpuMat distance;
	distance.upload(zero);

	GpuMat temp;


	uchar *src1bgr, *src2bgr;
	src1bgr = src1Mat.data;
	src2bgr = src2Mat.data;

	int src1bright = 0;
	int src2bright = 0;
	int diff_bright = 0;

	for(i = 0; i < 5; i++){
		for(j = 0; j < 5; j++){
			for(k = 0; k < 3; k++){
				src1bright += src1bgr[(640 * 3) * (5 + i) + (5 + j) + k];
				src2bright += src2bgr[(640 * 3) * (5 + i) + (5 + j) + k];
			}
		}
	}
	src1bright /= 75;
	src2bright /= 75;
	diff_bright = src1bright - src2bright;

	subtract(src1, Scalar(diff_bright, diff_bright, diff_bright), src1);


	vector<GpuMat> src1Channels;
	vector<GpuMat> src2Channels;
	split(src1, src1Channels);
	split(src2, src2Channels);

	Mat cpuMask;
	GpuMat mask, addTemp;
	GpuMat test;

	GpuMat dstChannel[3];


/*
	for(i = 0; i < 3; i++){
		absdiff(src1Channels[i], src2Channels[i], temp);
		temp.convertTo(temp, CV_32F);

		add(distance, temp, distance);
	}
*/

	absdiff(src1Channels[0], src2Channels[0], temp);
	temp.convertTo(temp, CV_32F);

	add(distance, temp, distance);


	threshold(distance, mask, 30, 255, THRESH_BINARY);
	mask.convertTo(mask, CV_8U);


	int dilation_size = 1;
	Mat element = getStructuringElement( 0,
	                                       Size( 2 * dilation_size + 1, 2 * dilation_size + 1 ),
	                                       Point( dilation_size, dilation_size ) );

	erode(mask, test, element);
	dilate(test, mask, element);
	erode(mask, test, element);
	dilate(test, mask, element);
	erode(mask, test, element);


	for(i = 0; i < 3; i++){
		bitwise_and(src1Channels[i], test, dstChannel[i]);
	}

	merge(dstChannel, 3, dst);

	//	mask.copyTo(dst);
	dst.download(cpuMask);

	imshow("mask", cpuMask);



	if(waitKey(1) == 1048675){
		struct tm* today;
		time_t ltime;
		time(&ltime);

		String filename = "";
		stringstream ss;
		ss << ltime;
		filename = ss.str();
		filename += "_";


		imwrite(filename + "0.png", cpuMask);
	}
}


void mySubtract(Mat& src1, Mat& src2, Mat& dst){
	int width = src1.cols;
	int height = src1.rows;

	int i, j, k;
	int ptr = 0;

	uchar *src1bgr, *src2bgr, *dstbgr;
	uchar diff_pix;
	int gray_p1, gray_p2;
	int diff_bright;

	Point pt;
	ptr = 0;

	src1bgr = src1.data;
	src2bgr = src2.data;
	dstbgr = dst.data;

	gray_p1 = (src1bgr[((640*3) * 40 + 40)] * 0.114) + (src1bgr[((640*3) * 40 + 40) + 1] * 0.287) + (src1bgr[((640*3) * 40 + 40) + 2] * 0.299);
	gray_p1 += (src1bgr[((640*3) * 41 + 40)] * 0.114) + (src1bgr[((640*3) * 41 + 40) + 1] * 0.287) + (src1bgr[((640*3) * 41 + 40) + 2] * 0.299);
	gray_p1 += (src1bgr[((640*3) * 40 + 41)] * 0.114) + (src1bgr[((640*3) * 42 + 40) + 1] * 0.287) + (src1bgr[((640*3) * 42 + 40) + 2] * 0.299);
	gray_p1 += (src1bgr[((640*3) * 41 + 41)] * 0.114) + (src1bgr[((640*3) * 42 + 40) + 1] * 0.287) + (src1bgr[((640*3) * 42 + 40) + 2] * 0.299);
	gray_p2 = (src2bgr[((640*3) * 40 + 40)] * 0.114) + (src2bgr[((640*3) * 40 + 40) + 1] * 0.287) + (src2bgr[((640*3) * 40 + 40) + 2] * 0.299);
	gray_p2 += (src2bgr[((640*3) * 41 + 40)] * 0.114) + (src2bgr[((640*3) * 41 + 40) + 1] * 0.287) + (src2bgr[((640*3) * 41 + 40) + 2] * 0.299);
	gray_p2 += (src2bgr[((640*3) * 40 + 41)] * 0.114) + (src2bgr[((640*3) * 42 + 40) + 1] * 0.287) + (src2bgr[((640*3) * 42 + 40) + 2] * 0.299);
	gray_p2 += (src2bgr[((640*3) * 41 + 41)] * 0.114) + (src2bgr[((640*3) * 42 + 40) + 1] * 0.287) + (src2bgr[((640*3) * 42 + 40) + 2] * 0.299);

	gray_p1 /= 4;
	gray_p2 /= 4;

	diff_bright = gray_p1 - gray_p2;

	for(i = 0; i < height * width * 3; i += 3){
		//			ptr = i * width + j;
		//			Point point(i, j);


		gray_p1 = (src1bgr[i + 0] * 0.114) + (src1bgr[i + 1] * 0.287) + (src1bgr[i + 2] * 0.299) - diff_bright;
		gray_p2 = (src2bgr[i + 0] * 0.114) + (src2bgr[i + 1] * 0.287) + (src2bgr[i + 2] * 0.299);

		if(gray_p2 - gray_p1 < 0){
			diff_pix = 0;
		} else {
			diff_pix = gray_p2 - gray_p1;
		}

		if(diff_pix < 10){
			dstbgr[i] = 0;
			dstbgr[i + 1] = 0;
			dstbgr[i + 2] = 0;
		}
	}
}


void* cam_thread_body (void* data){
	cam_params* param = (cam_params*)data;

	char winName[8] = "result";
	winName[6] = 48 + param->cam_num;
	winName[7] = 0;

	cout << "Capture " << winName << endl;

	CvCapture* capture = 0;
	Mat frame;

	//capture camera image
	capture = cvCaptureFromCAM(param->cam_num);

	cvSetCaptureProperty(capture, CV_CAP_PROP_BRIGHTNESS, CAM_BRIGHT);

	//create window
	cvNamedWindow(winName, 1);  //arg2 = 1 for autosize

	if(capture){
		cout << "In capture ..." << endl;
		while(TRUE)	{
			IplImage* iplImg = cvQueryFrame(capture);
			frame = iplImg;

			if(frame.empty())
				break;

			//			pthread_mutex_lock(&mutex[param->cam_num]);
			if(iplImg->origin == IPL_ORIGIN_TL){
				frame.copyTo(param->frame);
			}else{
				flip(frame, param->frame, 0);
			}

			//			pthread_mutex_unlock(&mutex[param->cam_num]);
		}
	}
	cvReleaseCapture( &capture );

	return (void*) 0;
}



void detectAndDraw(cam_params& cam, double scale, detect_params* params, int params_cnt){

	//	Mat& img = param->cam_param->frame;

	GpuMat img, bgImg, blurImg, beforeImg;
	GpuMat faceBuf[CAM_NUM * DETECT_NUM];
	Mat faces_downloaded[CAM_NUM * DETECT_NUM];
	Mat& result_img = cam.result_frame;
	GpuMat gray, gray2, smallImg, hsv, hsv2, filtered, subImg;

	CascadeClassifier_GPU* cascade[DETECT_NUM];

	int i, j;

	int maxHeight, maxX, maxY;

	double t = 0;
	Rect* faces_p[DETECT_NUM];
	vector<Rect> faces[DETECT_NUM];

	int detections_num[DETECT_NUM];

	Mat subFrame;

	/*
	cam.frame.copyTo(subFrame);
	mySubtract(cam.frame, cam.background, subFrame);
	extSubtract(cam.frame, cam.background, subFrame);
//			frame.copyTo(subFrame);

	subFrame.copyTo(cam.frame);
	subFrame.copyTo(cam.result_frame);
	 */

	img.upload(cam.frame);
//	bgImg.upload(cam.background);
	beforeImg.upload(cam.beforeFrame);

	cout << img.type() << endl;

	blurImg = GpuMat(img.size(), img.type());

	//equalizeHist(img , img);

//	GaussianBlur(bgImg, bgImg, Size(1, 1), 0, 0);
//	GaussianBlur(img, img, Size(1, 1), 0, 0);

	img.download(result_img);

//	extSubtract(img, bgImg, subImg);
	extSubtract(img, beforeImg, subImg);


	cvtColor(subImg, gray, CV_BGR2GRAY);


	//	cvtColor(filtered, gray, CV_BGR2GRAY);
	Size sz(cvRound(gray.cols / scale), cvRound(gray.rows / scale));
	resize( gray, smallImg, sz);
	//	equalizeHist( smallImg, smallImg);


	//	param->cam_param->frame.copyTo(img);
	for(i = 0; i < DETECT_NUM; i++){
		cascade[i] = &params[i].cascade;

		cascade[i]->visualizeInPlace = true;
		cascade[i]->findLargestObject = false;

		detections_num[i] = cascade[i]->detectMultiScale( smallImg, faceBuf[i], 1.1, 1, Size(50, 50));


		faceBuf[i].colRange(0, detections_num[i]).download(faces_downloaded[i]);

		faces_p[i] = faces_downloaded[i].ptr<Rect>();

		for(j = 0; j < detections_num[i]; j++){
			faces[i].push_back(faces_p[i][j]);
		}

		j = 0;

		int limitHeight = 60;
		maxHeight = 0;
		maxX = maxY = 0;
		for( vector<Rect>::const_iterator r = faces[i].begin(); r != faces[i].end(); r++, j++ ){
			Mat smallImgROI;
			vector<Rect> nestedObjects;
			Point center;
			Scalar color = colors[i][j % 8];
			int radius;

			if(r->height > limitHeight){
				center.x = cvRound((r->x + r->width*0.5)*scale);
				center.y = cvRound((r->y + r->height*0.5)*scale);
				radius = cvRound((r->width + r->height)*0.25*scale);
				circle( (Mat&)result_img, center, radius, color, 3, 8, 0 );

				if(r->height > maxHeight){
					maxHeight = r->height;
					maxX = center.x;
					maxY = center.y;
				}
			}

			//			params[i].x = center.x;
			//			params[i].y = center.y;

			//Commenting eye detection part out to make it faster...

		}
		params[i].x = maxX;
		params[i].y = maxY;
	}
	cam.frame.copyTo(cam.beforeFrame);
	//	cv::imshow(resultName, img );
}
