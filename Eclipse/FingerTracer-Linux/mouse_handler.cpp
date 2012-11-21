/*
 * mouse_handler.cpp
 *
 *  Created on: Nov 15, 2012
 *      Author: kiosk
 */


#include "mouse_handler.hpp"


mouse_handler::mouse_handler(){
	dpy = XOpenDisplay(0);
	root = DefaultRootWindow(dpy);

	int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 200, 100, 0, blackColor, blackColor);
	lock = 0;
}

void mouse_handler::set_point(int tmpX, int tmpY){
	if(lock == 0){
		x = tmpX;
		y = tmpY;
		XWarpPointer(dpy, None, w, 0, 0, 0, 0, x, y);
		XFlush(dpy);
	}
}

void mouse_handler::click(){
	lock = 1;
	int button = 1;

	XEvent event;
	Display *dpy2 = XOpenDisplay(NULL);
	memset(&event, 0x00, sizeof(event));

	event.type = ButtonPress;
	event.xbutton.button = button;
	event.xbutton.same_screen = True;

	XQueryPointer(dpy2,
			RootWindow(dpy2, DefaultScreen(dpy2)),
			&event.xbutton.root,
			&event.xbutton.window,
			&event.xbutton.x_root,
			&event.xbutton.y_root,
			&event.xbutton.x,
			&event.xbutton.y,
			&event.xbutton.state);

	event.xbutton.subwindow = event.xbutton.window;

	while(event.xbutton.subwindow){
		event.xbutton.window = event.xbutton.subwindow;
		XQueryPointer(dpy2, event.xbutton.window,
				&event.xbutton.root,
				&event.xbutton.subwindow,
				&event.xbutton.x_root,
				&event.xbutton.y_root,
				&event.xbutton.x,
				&event.xbutton.y,
				&event.xbutton.state);
	}


	if(XSendEvent(dpy2, PointerWindow, True, 0xfff, &event) == 0)
		cout << "Error nell'invio dell'evento !!!\n";

/*
	if(XSendEvent(dpy2, RootWindow(dpy2, DefaultScreen(dpy2)), True, 0xfff, &event) == 0)
		cout << "Error nell'invio dell'evento !!!\n";
*/

	XFlush(dpy2);

	cout << "Click" << endl;
	cout << "Click" << endl;
	cout << "Click" << endl;
	cout << "Click" << endl;

	usleep(100000);

	event.type = ButtonRelease;
	event.xbutton.state = 0x100;


	if(XSendEvent(dpy2, PointerWindow, True, 0xfff, &event) == 0)
		cout << "Error nell'invio dell'evento !!!\n";
/*
	if(XSendEvent(dpy2, RootWindow(dpy2, DefaultScreen(dpy2)), True, 0xfff, &event) == 0)
		cout << "Error nell'invio dell'evento !!!\n";
*/

	XFlush(dpy2);
	usleep(100000);
	lock = 0;

	XCloseDisplay(dpy2);
}

mouse_handler::~mouse_handler(){
	XCloseDisplay(dpy);
}
