/*
 * detect_obj.hpp
 *
 *  Created on: Nov 12, 2012
 *      Author: kiosk
 */

#ifndef DETECT_OBJ_HPP_
#define DETECT_OBJ_HPP_

#include "common.hpp"


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

void detect_obj_gpu(GpuMat& orig_mat, Mat& result_mat, object_gpu_t& objects);

#endif /* DETECT_OBJ_HPP_ */
