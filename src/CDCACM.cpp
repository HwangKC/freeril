/*
 * CDCACM.cpp
 *
 *  Created on: 01.03.2012
 *      Author: Swen Kuehnlein
 */

#include "USB.h"
#include "log.h"

#include <string>

namespace freeril
{

	class CDCACM : public USB::Driver, public Loggable
	{
	public:
		CDCACM(const std::string& deviceName);
		virtual ~CDCACM();

		class Factory : public USB::Driver::DefaultFactory<CDCACM>
		{
			typedef USB::Driver::DefaultFactory<CDCACM> Super;
		public:
			Factory();
			virtual ~Factory();

			virtual bool isSupported(const USB::DeviceReference& device) const;

		};

		static std::string name();
	};

}

using namespace freeril;

CDCACM::Factory::Factory() :
		Super{}
{
}

CDCACM::Factory::~Factory()
{
	// TODO
}

bool CDCACM::Factory::isSupported(const USB::DeviceReference&) const
{
	// TODO
	return true;
}

CDCACM::CDCACM(const std::string&)
{
}

CDCACM::~CDCACM()
{
	// TODO
}

std::string CDCACM::name()
{
	return "CDCACM";
}

// turn off Eclipse CDT's warning about unused variable
#ifdef __CDT_PARSER__
#define FREERIL_USB_REGISTER_INTERFACECLASS(a,b,c)
#endif
FREERIL_USB_REGISTER_INTERFACECLASS(CDCACM, 2, 2);
