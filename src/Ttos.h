/*
 * Ttos.h
 *
 *  Created on: 08.03.2012
 *      Author: Swen Kühnlein
 */

#ifndef TTOS_H_
#define TTOS_H_

#include <string>
#include <sstream>

namespace freeril
{

	template<typename T>
	inline ::std::string Ttos(const T value,
			::std::ios_base& (&base)(::std::ios_base&) = std::dec)
	{
		::std::ostringstream oss{};
		oss << base << value;
		return oss.str();
	}

	template<>
	inline ::std::string Ttos<unsigned char>(const unsigned char value,
			::std::ios_base& (&base)(::std::ios_base&))
	{
		::std::ostringstream oss{};
		typedef unsigned int uint;
		oss << base << uint(value);
		return oss.str();
	}

	template<>
	inline ::std::string Ttos<signed char>(const signed char value,
			::std::ios_base& (&base)(::std::ios_base&))
	{
		::std::ostringstream oss{};
		oss << base << int(value);
		return oss.str();
	}

}


#endif /* TTOS_H_ */
