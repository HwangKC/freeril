/*
 * testusb.cpp
 *
 *  Created on: 03.03.2012
 *      Author: Swen Kuehnlein
 */


#include "USB.h"
#include <iostream>

int main()
{
	using namespace freeril;
	USB usb = USB::instance();

	char foo[255];
	std::cin.getline(foo, 255);
	return 0;
}
