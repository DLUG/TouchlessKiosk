/*
 * main.hpp
 *
 *  Created on: Nov 12, 2012
 *      Author: kiosk
 */

#ifndef MAIN_HPP_
#define MAIN_HPP_

#include "common.hpp"
//#include <pthread.h>

#include "detect_obj.hpp"
#include "remove_bg.hpp"
#include "mouse_handler.hpp"
#include "keyboard_handler.hpp"

typedef struct real_object{
	int x;
	int y;
	int z;
}real_object_t;

#endif /* MAIN_HPP_ */
