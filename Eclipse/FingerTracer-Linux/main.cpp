#include "main.hpp"

int main(){
	int i, j, k;
	bool detectSwitch = false;

	Mat orig_img[CAM_NUM];
	Mat rotate_img[CAM_NUM];
	Mat bgremoved_img[CAM_NUM];
	Mat before_img[CAM_NUM];
	Mat bg_img[CAM_NUM];
	Mat dst_img[CAM_NUM];
	Mat blur_img[CAM_NUM];

	GpuMat orig_img_gpu[CAM_NUM];
	GpuMat rotate_img_gpu[CAM_NUM];
	GpuMat before_img_gpu[CAM_NUM];
	GpuMat bg_img_gpu[CAM_NUM];
	GpuMat bgremoved_img_gpu[CAM_NUM];

	VideoCapture cam[CAM_NUM];

	object_gpu_t captured_objects[CAM_NUM];
	real_object_t real_objects[2];

	int judgement[30];
	int judgement_score = 0;
	int click_status[2] = {0, 0};

	for(i = 0; i < 30; i++){
		judgement[i] = 0;
	}

	for(i = 0; i < 2; i++){
		real_objects[i].x = 0;
		real_objects[i].y = 0;
		real_objects[i].z = 0;
	}
	real_objects[0].z = 1000;

	mouse_handler mouse_handler_object;

	for(i = 0; i < CAM_NUM; i++){
		if(!cam[i].open(i)){
			cerr << "Cannot Open Cam" << i << endl;
			return -1;
		} else {
			cam[i].set(CV_CAP_PROP_BRIGHTNESS, CAM_BRIGHT[i]);
			cam[i].set(CV_CAP_PROP_GAIN, 0);
			cam[i].set(CV_CAP_PROP_EXPOSURE, 1);
			if(i == 2){
				cam[i].set(CV_CAP_PROP_FRAME_WIDTH, 640);
				cam[i].set(CV_CAP_PROP_FRAME_HEIGHT, 480);
			} else {
				cam[i].set(CV_CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
				cam[i].set(CV_CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);
			}


			char resultWinName[8] = "webcam";
			resultWinName[6] = 48 + i;
			resultWinName[7] = 0;

			char maskWinName[6] = "mask";
			maskWinName[4] = 48 + i;
			maskWinName[5] = 0;

			cvNamedWindow(resultWinName, 1);
			cvNamedWindow(maskWinName, 1);
		}
		sleep(1);
	}

	char resultWinName[8] = "webcam";
	resultWinName[6] = 48 + i;
	resultWinName[7] = 0;

	char maskWinName[6] = "mask";
	maskWinName[4] = 48 + i;
	maskWinName[5] = 0;

	int keyCode;

	for(i = 0; i < CAM_NUM; i++){
		captured_objects[i].width = 0;
		captured_objects[i].height = 0;
		captured_objects[i].x = 0;
		captured_objects[i].y = 0;
		captured_objects[i].cascade.load(HAAR_DATA[i]);
	}

	while(TRUE){
		for(i = 0; i < CAM_NUM; i++){
			resultWinName[6] = 48 + i;
			maskWinName[4] = 48 + i;

			cam[i].read(orig_img[i]);

			orig_img_gpu[i].upload(orig_img[i]);

			Size sz(CAM_HEIGHT, CAM_WIDTH);

//			GaussianBlur(orig_img_gpu[i], orig_img_gpu[i], Size(1, 1), 0, 0);
//			bg_img_gpu[i].upload(bg_img[i]);
//			GaussianBlur(bg_img_gpu[i], bg_img_gpu[i], Size(1, 1), 0, 0);
//			bg_img_gpu[i].download(bg_img[i]);


			if(i == 0){
				gpu::rotate(orig_img_gpu[i], rotate_img_gpu[i], sz, -90, CAM_HEIGHT, 0);
			}else if(i == 1){
				gpu::rotate(orig_img_gpu[i], rotate_img_gpu[i], sz, 90, 0, CAM_WIDTH);
			}else {
				orig_img_gpu[i].copyTo(rotate_img_gpu[i]);
			}


			rotate_img_gpu[i].download(rotate_img[i]);
			rotate_img[i].copyTo(dst_img[i]);

			if(detectSwitch){
//				if(i != 2){
				if(false){
					remove_bg_gpu(rotate_img[i], bg_img[i], bgremoved_img[i]);
					imshow(maskWinName, bgremoved_img[i]);
				}else
					rotate_img[i].copyTo(bgremoved_img[i]);

				bgremoved_img_gpu[i].upload(bgremoved_img[i]);
				detect_obj_gpu(bgremoved_img_gpu[i], dst_img[i], captured_objects[i]);
			}

			imshow(resultWinName, dst_img[i]);

// Key Event

			keyCode = waitKey(1);
			if(keyCode != -1)
				cout << keyCode << endl;
			if(keyCode == 1048675){		// 'C'
				struct tm* today;
				time_t ltime;
				time(&ltime);

				String filename = "";
				stringstream ss;
				ss << ltime;
				filename = ss.str();
				filename += "_";

				imwrite(filename + "0.png", rotate_img[0]);
				imwrite(filename + "1.png", rotate_img[1]);
			} else if(keyCode == 1048691){ // 'S'
				if(detectSwitch == false)
					detectSwitch = true;
				else
					detectSwitch = false;
			} else if(keyCode == 1048681){  // 'I'
				for(j = 0; j < CAM_NUM; j++){
					rotate_img[j].copyTo(bg_img[j]);
				}
			} else if(keyCode == 1048696){	//'X'
				return 0;
			} else if(keyCode == 1048685){	//'M'
				mouse_handler_object.click();
			}

//			orig_img[i].copyTo(before_img[i]);
		}

		if(detectSwitch){
			if(captured_objects[2].onoff){
				real_objects[0].x = captured_objects[2].x;
				real_objects[0].y = captured_objects[2].y;
			}

			if(captured_objects[0].onoff && captured_objects[1].onoff){
				real_objects[1].x = (captured_objects[1].x - 40) * SCREEN_WIDTH / 150;
				real_objects[1].z = captured_objects[1].y;
				real_objects[1].y = ((320 - captured_objects[1].x) + captured_objects[0].x - 280) * SCREEN_HEIGHT / 80;
				mouse_handler_object.set_point(SCREEN_WIDTH - real_objects[1].x, SCREEN_HEIGHT - real_objects[1].y);
			}

#ifdef CLICK_ENABLE
			judgement_score = 0;
			for(i = 0; i < 14; i++){
				judgement[i] = judgement[i + 1];
				judgement_score += judgement[i];
			}
			judgement[14] = captured_objects[0].onoff || captured_objects[1].onoff;

			if(judgement_score > 10){
				if(click_status[0] != 1 && click_status[1] != 1){
					click_status[1] = 1;
					mouse_handler_object.click();
				} else {
					click_status[1] = 1;
				}
				click_status[0] = click_status[1];
			} else {
				click_status[1] = 0;
				click_status[0] = click_status[1];
			}
#endif

// Print Console
			for(i = 0; i < CAM_NUM; i++){
				cout << "Captured Object" << i
						<< " - x: " << captured_objects[i].x
						<< ", y: " << captured_objects[i].y << endl;
			}
			cout << endl;

			for(i = 0; i < 2; i++){
				cout << "Real Object" << i
						<< " - x: " << real_objects[i].x
						<< ", y: " << real_objects[i].y
						<< ", z: " << real_objects[i].z << endl;
			}
			cout << endl;
		}


	}

	for(i = 0; i < CAM_NUM; i++){
		cam[i].release();
	}

	return 0;
}
