/*
 * keyboard_handler.hpp
 *
 *  Created on: Nov 21, 2012
 *      Author: mabin359
 */

#ifndef KEYBOARD_HANDLER_HPP_
#define KEYBOARD_HANDLER_HPP_


#include "common.hpp"
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <time.h>


class keyboard_handler{
public:
	void click(KeySym keySymbol);
};

#endif /* KEYBOARD_HANDLER_HPP_ */
