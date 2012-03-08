/*
 * log.cpp
 *
 *  Created on: 03.03.2012
 *      Author: Swen Kuehnlein
 */

#include "log.h"

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#include <boost/assign/list_of.hpp>

#include <typeinfo>
#include <map>

// TODO: use logcat on android

#include <iostream>

namespace freeril
{

// turn off error of Eclipse's CDT parser for map_list_of
#ifdef __CDT_PARSER__
#define map_list_of
#endif

std::map<log::Priority, std::string> prioString
	= boost::assign::map_list_of
		(log::DEFAULT, "")
		(log::VERBOSE, "V")
		(log::DEBUG, "D")
		(log::INFO, "I")
		(log::WARN, "W")
		(log::ERROR, "E");

Loggable::~Loggable()
{
}

std::string Loggable::logName() const
{
#ifdef __GNUG__
	int status{-4};
	const char *const name{typeid(*this).name()};
	char* res{abi::__cxa_demangle(name, NULL, NULL, &status)};
	const char *const demangled_name{(status == 0) ? res : name};
	std::string retval{demangled_name};
	free(res);
	return retval;
#else
	return typeid(*this).name();
#endif
}

void logmsg(const std::string& from, const log::Priority priority, const std::string& msg)
{
	std::cout << prioString[priority] << "/" << from << ": " << msg << std::endl;
}

void logmsg(const Loggable* from, const log::Priority priority, const std::string& msg)
{
	logmsg(from->logName(), priority, msg);
}

}
