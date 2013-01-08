/*
 * detect_obj.cpp
 *
 *  Created on: Nov 12, 2012
 *      Author: kiosk
 */


#include "detect_obj.hpp"

void detect_obj_gpu(GpuMat& orig_gpumat, Mat& result_mat, object_gpu_t& objects){

	//	Mat& img = param->cam_param->frame;

	GpuMat objects_buf;
//	Mat faces_downloaded[CAM_NUM * DETECT_NUM];
	Mat objects_downloaded;
	GpuMat gray_gpumat, small_gray_gpumat, hsv, hsv2, filtered, subImg;

//	CascadeClassifier_GPU* cascade[DETECT_NUM];

	int j;

	int maxX, maxY;

	double t = 0;
	Rect* objects_p;
	vector<Rect> objects_vector;

	int detected_objects_num;

	Mat subFrame;


//	blurImg = GpuMat(img.size(), img.type());

	//equalizeHist(img , img);

//	GaussianBlur(bgImg, bgImg, Size(1, 1), 0, 0);
//	GaussianBlur(img, img, Size(1, 1), 0, 0);

	cvtColor(orig_gpumat, gray_gpumat, CV_BGR2GRAY);


	//	cvtColor(filtered, gray, CV_BGR2GRAY);
	Size sz(cvRound(gray_gpumat.cols / DETECT_SCALE), cvRound(gray_gpumat.rows / DETECT_SCALE));
	resize(gray_gpumat, small_gray_gpumat, sz);
	//	equalizeHist( smallImg, smallImg);


	objects.cascade.visualizeInPlace = true;
	objects.cascade.findLargestObject = false;

	detected_objects_num = objects.cascade.detectMultiScale( small_gray_gpumat, objects_buf, 1.1, 1);

	objects_buf.colRange(0, detected_objects_num).download(objects_downloaded);

	objects_p = objects_downloaded.ptr<Rect>();

	for(j = 0; j < detected_objects_num; j++){
		objects_vector.push_back(objects_p[j]);
	}

	j = 0;

	int limitHeight = 60;
//	maxY = 0;
	maxX = 0;
	maxY = 480;
	for( vector<Rect>::const_iterator r = objects_vector.begin(); r != objects_vector.end(); r++, j++ ){
		Mat smallImgROI;
		vector<Rect> nestedObjects;
		Point center;
		Scalar color = colors[1][j % 8];
		int radius;

		if(r->height > limitHeight){
			center.x = cvRound((r->x + r->width*0.5) * DETECT_SCALE);
			center.y = cvRound((r->y + r->height*0.5) * DETECT_SCALE);
			radius = cvRound((r->width + r->height)*0.25 * DETECT_SCALE);
			circle( (Mat&)result_mat, center, radius, color, 3, 8, 0 );

			if(r->y < maxY){
//				maxHeight = r->height;
				maxX = center.x;
				maxY = center.y;
			}
		}

		//			params.x = center.x;
		//			params.y = center.y;

		//Commenting eye detection part out to make it faster...

	}

	objects.x = maxX;
	if(maxY == 480)
		objects.y = 0;
	else
		objects.y = maxY;

	if(maxX)
		objects.onoff = 1;
	else
		objects.onoff = 0;
}

void detect_obj_gpu(GpuMat& orig_gpumat, Mat& result_mat, object_gpu_t objects[], int object_num){

	//	Mat& img = param->cam_param->frame;

	GpuMat objects_buf[object_num];
//	Mat faces_downloaded[CAM_NUM * DETECT_NUM];
	Mat objects_downloaded[object_num];
	GpuMat gray_gpumat, small_gray_gpumat, hsv, hsv2, filtered, subImg;

//	CascadeClassifier_GPU* cascade[DETECT_NUM];

	int i, j;

	int maxHeight, maxX, maxY;

	double t = 0;
	Rect* objects_p[object_num];
	vector<Rect> objects_vector[object_num];

	int detected_objects_num[object_num];

	Mat subFrame;


//	blurImg = GpuMat(img.size(), img.type());

	//equalizeHist(img , img);

//	GaussianBlur(bgImg, bgImg, Size(1, 1), 0, 0);
//	GaussianBlur(img, img, Size(1, 1), 0, 0);

	cvtColor(orig_gpumat, gray_gpumat, CV_BGR2GRAY);


	//	cvtColor(filtered, gray, CV_BGR2GRAY);
	Size sz(cvRound(gray_gpumat.cols / DETECT_SCALE), cvRound(gray_gpumat.rows / DETECT_SCALE));
	resize(gray_gpumat, small_gray_gpumat, sz);
	//	equalizeHist( smallImg, smallImg);


	//	param->cam_param->frame.copyTo(img);
	for(i = 0; i < object_num; i++){
		objects[i].cascade.visualizeInPlace = true;
		objects[i].cascade.findLargestObject = false;

		detected_objects_num[i] = objects[i].cascade.detectMultiScale( small_gray_gpumat, objects_buf[i], 1.1, 1, Size(20, 20));

		objects_buf[i].colRange(0, detected_objects_num[i]).download(objects_downloaded[i]);

		objects_p[i] = objects_downloaded[i].ptr<Rect>();

		for(j = 0; j < detected_objects_num[i]; j++){
			objects_vector[i].push_back(objects_p[i][j]);
		}

		j = 0;

		int limitHeight = 60;
		maxHeight = 0;
		maxX = maxY = 0;
		for( vector<Rect>::const_iterator r = objects_vector[i].begin(); r != objects_vector[i].end(); r++, j++ ){
			Mat smallImgROI;
			vector<Rect> nestedObjects;
			Point center;
			Scalar color = colors[i][j % 8];
			int radius;

			if(r->height > limitHeight){
				center.x = cvRound((r->x + r->width*0.5) * DETECT_SCALE);
				center.y = cvRound((r->y + r->height*0.5) * DETECT_SCALE);
				radius = cvRound((r->width + r->height)*0.25 * DETECT_SCALE);
				circle( (Mat&)result_mat, center, radius, color, 3, 8, 0 );

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
		objects[i].x = maxX;
		objects[i].y = maxY;

	}
	//	cv::imshow(resultName, img );
}
