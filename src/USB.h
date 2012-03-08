/*
 * USBWatcher.h
 *
 *  Created on: 08.03.2012
 *      Author: Swen Kühnlein
 */

#ifndef USB_H_
#define USB_H_

#include <cstdint>
#include <string>

#define FREERIL_JOIN(symbol1, symbol2) _FREERIL_DO_JOIN(symbol1, symbol2)
#define _FREERIL_DO_JOIN(symbol1, symbol2) _FREERIL_DO_JOIN2(symbol1, symbol2)
#define _FREERIL_DO_JOIN2(symbol1, symbol2) symbol1##symbol2
// TODO: This will fail, if there are 2 files with registrations in identical line numbers
#define FREERIL_UNIQUE_NAME(prefix) FREERIL_JOIN(prefix, __LINE__)

#define FREERIL_USB_REGISTER_PRODUCT(driver, vid, pid) \
	static ::freeril::USB::Registered FREERIL_UNIQUE_NAME(__usb_registered_product)  __attribute__((__unused__)) \
	 = ::freeril::USB::registerProduct(new driver::Factory(), vid, pid)
#define FREERIL_USB_REGISTER_DEVICECLASS(driver, devClass, subClass) \
	static ::freeril::USB::Registered FREERIL_UNIQUE_NAME(__usb_registered_deviceClass) __attribute__((__unused__)) \
	 = ::freeril::USB::registerDeviceClass(new driver::Factory(), devClass, subClass)
#define FREERIL_USB_REGISTER_INTERFACECLASS(driver, intClass, subClass) \
	static ::freeril::USB::Registered FREERIL_UNIQUE_NAME(__usb_registered_interfaceClass) __attribute__((__unused__)) \
	 = ::freeril::USB::registerInterfaceClass(new driver::Factory(), intClass, subClass)

namespace freeril
{
	class USB
	{
	public:
		enum Registered {};
		typedef uint16_t VendorID;
		typedef uint16_t ProductID;
		typedef uint8_t DeviceClass;
		typedef uint8_t DeviceSubclass;
		typedef uint8_t InterfaceClass;
		typedef uint8_t InterfaceSubclass;
		typedef std::string DeviceReference;

		class Driver
		{
		public:
			class Factory
			{
			public:
				virtual ~Factory() {}
				virtual bool isSupported(const DeviceReference& device) const = 0;
				virtual std::string name() const = 0;
				virtual Driver* create(const DeviceReference& device) const = 0;
			protected:
				Factory() {}
			};

			template<class DRIVER>
			class DefaultFactory : public Factory
			{
			public:
				typedef DRIVER Driver;

				virtual std::string name() const
				{
					return Driver::name();
				}

				virtual Driver* create(const DeviceReference& device) const
				{
					return new Driver(device);
				}
			};

			virtual ~Driver() {}
		protected:
			Driver() {}
		};

		virtual ~USB();

		static USB& initInstance();
		static Registered registerProduct(
				const Driver::Factory* factory,
				const VendorID vendor,
				const ProductID product);
		static Registered registerDeviceClass(
				const Driver::Factory* factory,
				const DeviceClass deviceClass,
				const DeviceSubclass deviceSubclass);
		static Registered registerInterfaceClass(
				const Driver::Factory* factory,
				const InterfaceClass interfaceClass,
				const InterfaceSubclass interfaceSubclass);

	protected:
		USB();
	private:
		USB(const USB& usb);
	};

}

#endif /* USB_H_ */
