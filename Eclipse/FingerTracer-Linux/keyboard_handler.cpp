/*
 * keyboard_handler.cpp
 *
 *  Created on: Nov 21, 2012
 *      Author: mabin359
 */





#include "keyboard_handler.hpp"

void keyboard_handler::click(KeySym keySymbol){
	Display *display=NULL;
	unsigned int keycode;

	display = XOpenDisplay(NULL);
	keycode = XKeysymToKeycode(display, keySymbol);

	cout << "\npressed\n";
	XTestFakeKeyEvent(display, keycode, True, CurrentTime);
	XFlush(display);
	cout << "\nreleased\n";
	XTestFakeKeyEvent(display, keycode, False, CurrentTime);
	XFlush(display);


	cout << "Pressed" << endl;
	cout << "Pressed" << endl;
	cout << "Pressed" << endl;
	cout << "Pressed" << endl;

}
