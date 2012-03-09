/*
 * USBWatcher.cpp
 *
 *  Created on: 08.03.2012
 *      Author: Swen Kühnlein
 */

#include "USB.h"
#include "log.h"
#include "Ttos.h"

#include <usbhost/usbhost.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include <memory>
#include <map>
#include <string>

namespace freeril
{

class USB_ : public USB, public Loggable
{
	friend class USB;
public:
	USB_();
	virtual ~USB_();

	virtual std::string logName() const;

	void startThread();
	void run();

private:
	typedef boost::tuple<VendorID, ProductID> ProductKey;
	typedef boost::tuple<DeviceClass, DeviceSubclass, DeviceProtocol>
		DeviceKey;
	typedef boost::tuple<InterfaceClass, InterfaceSubclass, InterfaceProtocol>
		InterfaceKey;
	// TODO(swen): Make values const if ptr_map supports it
	typedef boost::ptr_map<ProductKey, Driver::Factory> ProductRegistry;
	typedef boost::ptr_map<DeviceKey, Driver::Factory> DeviceClassRegistry;
	typedef boost::ptr_map<InterfaceKey, Driver::Factory>
		InterfaceClassRegistry;

	void findDriver(const std::string& devName);
	void tryDriver(
			const std::string& devName,
			const Driver::Factory& factory);

	static int deviceAdded(const char* devName, void* _that);
	static int deviceRemoved(const char* devName, void* _that);
	static int discoveryDone(void* _that);

	static boost::mutex initLock;
	static boost::scoped_ptr<USB_> instance;

	static boost::mutex registryLock;
	static ProductRegistry productRegistry;
	static DeviceClassRegistry deviceClassRegistry;
	static InterfaceClassRegistry interfaceClassRegistry;

	usb_host_context* usbContext;

	boost::thread thread;
};

boost::mutex USB_::initLock{};
boost::scoped_ptr<USB_> USB_::instance{};

boost::mutex USB_::registryLock{};
USB_::ProductRegistry USB_::productRegistry{};
USB_::DeviceClassRegistry USB_::deviceClassRegistry{};
USB_::InterfaceClassRegistry USB_::interfaceClassRegistry{};

USB_::USB_() :
		USB(),
		Loggable(),
		usbContext(usb_host_init()),
		thread()
{
}

USB_::~USB_()
{
	usb_host_cleanup(this->usbContext);
}

std::string USB_::logName() const
{
	return "USB";
}

void USB_::startThread()
{
	this->thread = boost::thread(boost::bind(&USB_::run, this));
}

void USB_::run()
{
	usb_host_run(this->usbContext,
			this->deviceAdded,
			this->deviceRemoved,
			this->discoveryDone,
			this);
}

void USB_::findDriver(const std::string& devName)
{
	usb_device* dev = usb_device_open(devName.c_str());
	const VendorID vendorID = usb_device_get_vendor_id(dev);
	const ProductID productID = usb_device_get_product_id(dev);

	CLOGD("Searching driver for VID " +
			Ttos(vendorID, std::hex) + " PID " +
			Ttos(productID, std::hex));
	const ProductKey key(vendorID, productID);
	this->registryLock.lock();
// remove Eclipse CDT Error for end
#ifdef __CDT_PARSER__
#define end() end(0)
#endif // __CDT_PARSER__
	if (this->productRegistry.find(key) != this->productRegistry.end())
#undef end
	{
		const Driver::Factory& factory = this->productRegistry.at(key);
		this->registryLock.unlock();
		this->tryDriver(devName, factory);
	}
	else
	{
		this->registryLock.unlock();
	}

	usb_descriptor_iter* descriptorIter = new usb_descriptor_iter();
	usb_descriptor_iter_init(dev, descriptorIter);
	usb_descriptor_header* descriptorHeader
		= usb_descriptor_iter_next(descriptorIter);
	while (descriptorHeader != 0)
	{
		if (descriptorHeader->bDescriptorType == USB_DT_DEVICE)
		{
			usb_device_descriptor* devDescriptor
				= reinterpret_cast<usb_device_descriptor*>(descriptorHeader);
			const DeviceKey key(
				devDescriptor->bDeviceClass,
				devDescriptor->bDeviceSubClass,
				devDescriptor->bDeviceProtocol);
			CLOGD("Searching driver for device class " +
					Ttos(key.get<0>()) +
					"/" + Ttos(key.get<1>()) +
					"/" + Ttos(key.get<2>()));
			this->registryLock.lock();
// remove Eclipse CDT Error for end
#ifdef __CDT_PARSER__
#define end() end(0)
#endif // __CDT_PARSER__
			if (this->deviceClassRegistry.find(key)
					!= this->deviceClassRegistry.end())
#undef end
			{
				const Driver::Factory& factory
					= this->deviceClassRegistry.at(key);
				this->registryLock.unlock();
				this->tryDriver(devName, factory);
			}
			else
			{
				this->registryLock.unlock();
			}
		}
		if (descriptorHeader->bDescriptorType == USB_DT_INTERFACE)
		{
			usb_interface_descriptor* intDescriptor
				= reinterpret_cast<usb_interface_descriptor*>(
						descriptorHeader);
			const InterfaceKey key(
				intDescriptor->bInterfaceClass,
				intDescriptor->bInterfaceSubClass,
				intDescriptor->bInterfaceProtocol);
			CLOGD("Searching driver for interface class " +
					Ttos(key.get<0>()) +
					"/" + Ttos(key.get<1>()) +
					"/" + Ttos(key.get<2>()));
			this->registryLock.lock();
// remove Eclipse CDT Error for end
#ifdef __CDT_PARSER__
#define end() end(0)
#endif // __CDT_PARSER__
			if (this->interfaceClassRegistry.find(key)
					!= this->interfaceClassRegistry.end())
#undef end
			{
				const Driver::Factory& factory
					= this->interfaceClassRegistry.at(key);
				this->registryLock.unlock();
				this->tryDriver(devName, factory);
			}
			else
			{
				this->registryLock.unlock();
			}
		}
		descriptorHeader = usb_descriptor_iter_next(descriptorIter);
	}
	delete descriptorIter;
	usb_device_close(dev);
}

void USB_::tryDriver(
		const std::string& devName,
		const Driver::Factory& factory)
{
	CLOGD("Trying driver " + factory.name() + " for " + devName);
}

int USB_::deviceAdded(const char* devName, void* _that)
{
	USB_* that = reinterpret_cast<USB_*>(_that);
	LOGI(that, "USB device added: " + std::string(devName));
	that->findDriver(devName);
	return false;
}

int USB_::deviceRemoved(const char* devName, void* _that)
{
	USB_* that = reinterpret_cast<USB_*>(_that);
	LOGI(that, "USB device removed: " + std::string(devName));
	return false;
}

int USB_::discoveryDone(void* _that)
{
	USB_* that = reinterpret_cast<USB_*>(_that);
	LOGI(that, "USB discovery done");
	return false;
}


USB::USB() :
		NotCopyable(),
		NotAssignable()
{
}

USB::~USB()
{
}

USB& USB::instance()
{
	USB_::initLock.lock();
	if (USB_::instance == NULL)
	{
		USB_::instance.reset(new USB_());
		USB_::instance->startThread();
	}
	USB_::initLock.unlock();
	return *USB_::instance;
}

USB::Registered USB::registerProduct(
		const USB::Driver::Factory* factory,
		const USB::VendorID vendor,
		const USB::ProductID product)
{
	USB_::ProductKey key(vendor, product);
	USB_::registryLock.lock();
	// TODO(swen): remove const_cast if ptr_map supports const values
	USB_::productRegistry.insert(
			key,
			const_cast<USB::Driver::Factory*>(factory));
	USB_::registryLock.unlock();
	LOGI("USB", "registered driver " + factory->name() +
			" for VID " + Ttos(vendor, std::hex) +
			" PID " + Ttos(product, std::hex));
	return USB::Registered();
}

USB::Registered USB::registerDeviceClass(
		const USB::Driver::Factory* factory,
		const USB::DeviceClass deviceClass,
		const USB::DeviceSubclass deviceSubclass,
		const USB::DeviceProtocol deviceProtocol)
{
	USB_::DeviceKey key(deviceClass, deviceSubclass, deviceProtocol);
	USB_::registryLock.lock();
	// TODO(swen): remove const_cast if ptr_map supports const values
	USB_::deviceClassRegistry.insert(
			key,
			const_cast<USB::Driver::Factory*>(factory));
	USB_::registryLock.unlock();
	LOGI("USB", "registered driver " + factory->name() +
			" for device class " + Ttos(deviceClass) +
			"/" + Ttos(deviceSubclass) +
			"/" + Ttos(deviceProtocol));
	return USB::Registered();
}

USB::Registered USB::registerInterfaceClass(
		const USB::Driver::Factory* factory,
		const USB::InterfaceClass interfaceClass,
		const USB::InterfaceSubclass interfaceSubclass,
		const USB::InterfaceProtocol interfaceProtocol)
{
	USB_::InterfaceKey key{interfaceClass, interfaceSubclass, interfaceProtocol};
	USB_::registryLock.lock();
	// TODO(swen): remove const_cast if ptr_map supports const values
	USB_::interfaceClassRegistry.insert(
			key,
			const_cast<USB::Driver::Factory*>(factory));
	USB_::registryLock.unlock();
	LOGI("USB", "registered driver " + factory->name() +
			" for interface class " + Ttos(interfaceClass) +
			"/" + Ttos(interfaceSubclass) +
			"/" + Ttos(interfaceProtocol));
	return USB::Registered();
}

} // namespace freeril

