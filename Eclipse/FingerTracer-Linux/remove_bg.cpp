/*
 * extract_bg.cpp
 *
 *  Created on: Nov 12, 2012
 *      Author: kiosk
 */

#include "remove_bg.hpp"



void remove_bg_gpu(Mat& src1Mat, Mat& src2Mat, Mat& dstMat){
	int i, j, k;

	GpuMat src1, src2, dst;
	src1Mat.copyTo(dstMat);
	src1.upload(src1Mat);
	src2.upload(src2Mat);

	Mat zero = Mat::zeros(src1Mat.size(), CV_32F);
//	Mat src1Mat, src2Mat;
//	src1.download(src1Mat);
//	src2.download(src2Mat);

	GpuMat distance;
	distance.upload(zero);

	GpuMat temp;


	uchar *src1bgr, *src2bgr;
	src1bgr = src1Mat.data;
	src2bgr = src2Mat.data;
/*

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

*/

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

	absdiff(src2Channels[0], src1Channels[2], temp);
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
/*
	erode(mask, test, element);
	dilate(test, mask, element);
	erode(mask, test, element);
*/

	for(i = 0; i < 3; i++){
		bitwise_and(src1Channels[i], mask, dstChannel[i]);
	}

	merge(dstChannel, 3, dst);

//	Scalar sumVal = sum(dst);
//	double* sumValSeperate = sumVal.val;

	//	mask.copyTo(dst);
//	double realsum = 0;
//	realsum += sumValSeperate[0] + sumValSeperate[1] + sumValSeperate[2] + sumValSeperate[3];
	dst.download(cpuMask);


//	if(realsum != 0){
		cpuMask.copyTo(dstMat);
//	}
}
