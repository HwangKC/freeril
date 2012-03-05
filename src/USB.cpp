/*
 * USB.cpp
 *
 *  Created on: 01.03.2012
 *      Author: Swen Kuehnlein
 */

#include "USB.h"
#include "USBDevice.h"
#include "log.h"

#include "usbhost/usbhost.h"

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>

namespace freeril
{

class USB_Impl : public USB, public Loggable
{
public:

	class Destroyer
	{
	public:
		Destroyer() : target(0) {}
		~Destroyer() { if (this->target != 0) delete this->target; }

		void setTarget(USB_Impl* target) { this->target = target; }

		USB_Impl* target;
	};

	class Watcher : public Loggable
	{
	public:

		typedef std::vector<USBDevice> Devices;

		Watcher();
		virtual ~Watcher();
		void run();
		inline void requestFinish();

// TODO: Identify devices by path (thread safe)
		inline unsigned int getDeviceCount() const;
		inline USBDevice& getDevice(unsigned int no);

	private:
		static int deviceAdded(const char* dev_name, void* _that);
		static int deviceRemoved(const char* dev_name, void* _that);
		static int discoveryDone(void* _that);

		usb_host_context* uhc;
		boost::mutex finishMutex;
		bool finish;
		boost::mutex devicesMutex;
		Devices devices;
	};


	USB_Impl();
	~USB_Impl();

	static USB_Impl* _instance;
	static Destroyer _destroyer;

	Watcher watcher;
	boost::thread watcherThread;
	usb_host_context* uhc;
};

USB_Impl* USB_Impl::_instance = 0;
USB_Impl::Destroyer USB_Impl::_destroyer = USB_Impl::Destroyer();

USB_Impl::Watcher::Watcher() :
		uhc(usb_host_init()),
		finish(false)
{
}

USB_Impl::Watcher::~Watcher()
{
	usb_host_cleanup(this->uhc);
}

void USB_Impl::Watcher::run()
{
	usb_host_run(this->uhc,
				 this->deviceAdded,
				 this->deviceRemoved,
				 this->discoveryDone,
				 this);
	std::cout << "Finished" << std::endl;
}

void USB_Impl::Watcher::requestFinish()
{
	this->finishMutex.lock();
	this->finish = true;
	this->finishMutex.unlock();
}

unsigned int USB_Impl::Watcher::getDeviceCount() const
{
	return this->devices.size();
}

USBDevice& USB_Impl::Watcher::getDevice(unsigned int no)
{
	return this->devices[no];
}

int USB_Impl::Watcher::deviceAdded(const char* dev_name, void* _that)
{
	Watcher* that = reinterpret_cast<Watcher*>(_that);
	LOGI(that, "USB device added: " + std::string(dev_name));

	that->devicesMutex.lock();
	//that->devices.push_back(USBDevice(dev_name));
	that->devicesMutex.unlock();

	that->finishMutex.lock();
	bool res = that->finish;
	that->finishMutex.unlock();
	return res;
}

int USB_Impl::Watcher::deviceRemoved(const char* dev_name, void* _that)
{
	Watcher* that = reinterpret_cast<Watcher*>(_that);
	LOGI(that, "USB device removed: " + std::string(dev_name));

	that->devicesMutex.lock();
	//const Devices::iterator pos = std::find(that->devices.begin(), that->devices.end(), dev_name);
	//that->devices.erase(pos);
	that->devicesMutex.unlock();

	that->finishMutex.lock();
	bool res = that->finish;
	that->finishMutex.unlock();
	return res;
}

int USB_Impl::Watcher::discoveryDone(void* _that)
{
	Watcher* that = reinterpret_cast<Watcher*>(_that);
	LOGD(that, "USB discovery done");

	that->finishMutex.lock();
	bool res = that->finish;
	that->finishMutex.unlock();
	return res;
}

USB_Impl::USB_Impl() :
		watcher(),
		watcherThread(boost::bind(&Watcher::run, &watcher)),
		uhc(usb_host_init())
{

}

USB_Impl::~USB_Impl()
{
	this->watcher.requestFinish();
	usb_host_cleanup(this->uhc);
}

USB::USB()
{
}

USB::~USB()
{
}

USB& USB::instance()
{
	// TODO: Make thread-safe!
	if (USB_Impl::_instance == 0)
	{
		USB_Impl::_instance = new USB_Impl();
		USB_Impl::_destroyer.setTarget(USB_Impl::_instance);
	}
	return *USB_Impl::_instance;
}

}
