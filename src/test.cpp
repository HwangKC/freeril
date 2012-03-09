/*
 * test.cpp
 *
 *  Created on: 08.03.2012
 *      Author: Swen Kühnlein
 */

#include "USB.h"

#include <iostream>

int main()
{
	using namespace freeril;

	USB& usb __attribute__((unused)) = USB::instance();

	char foo[255];
	std::cin.getline(foo, 255);
	return 0;
}


