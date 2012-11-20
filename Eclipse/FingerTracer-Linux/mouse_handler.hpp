/*
 * mouse_handler.hpp
 *
 *  Created on: Nov 15, 2012
 *      Author: kiosk
 */

#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "common.hpp"


#ifndef MOUSE_HANDLER_HPP_
#define MOUSE_HANDLER_HPP_

class mouse_handler{
private:
	int x;
	int y;
	int lock;
	Display *dpy;
	Display *dpy2;

	Window w;
	XEvent event;
	Window root;

public:
	mouse_handler();
	void set_point(int x, int y);
	void click();
	~mouse_handler();
};

#endif /* MOUSE_HANDLER_HPP_ */
