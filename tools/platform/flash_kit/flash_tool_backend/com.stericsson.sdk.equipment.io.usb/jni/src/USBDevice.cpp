/*
 * USBDevice.cpp
 *
 *  Created on: Jul 16, 2009
 *      Author: emicroh
 */

#include "USBDevice.h"
#include "util.h"
#include "javadefs.h"
#include <string>
#include <cstring>
#include "Logger.h"

using namespace std;

USBDevice::USBDevice(const USBDeviceMapKey & _key, struct usb_device *device, const string & busDirname,
		const string & deviceFilename)
:key(_key),
 bus_dirname(busDirname),
 device_filename(deviceFilename)
{
	this->device = device;

	handle = NULL;
	configValue = UNDEF;
	interfaceNumber = UNDEF;
	bulkInEndpoint = UNDEF;
	bulkOutEndpoint = UNDEF;
	opened = false;
}

int USBDevice::bulkRead(JNIEnv *env, char *bytes, int size, int timeout) {
	try {
		int ret = -1;
		if (opened) {
			ret = usb_bulk_read(handle, bulkInEndpoint, bytes, size, timeout);
/*			if (ret > 0) {
				Logger::getInstance()->error(LP, "Bulk read: %d byte(s) have been read..", ret);
			} else if (ret == 0) {
				env->ThrowNew(env->FindClass(EX_IO_TIMEOUT_EXCEPTION), "No bytes has been read from USB device in timeout!");
			} else {
				errorLog(env, "Bulk read operation failed with error: %d!", ret);
				env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), usb_strerror());
			}*/
		} else {
			env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), "Cannot perform bulk read, port is not opened!");
		}
		return ret;
	} catch (...) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), "Bulk read operation failed!");
	}
	return UNDEF;
}

int USBDevice::bulkWrite(JNIEnv *env, char *bytes, int size, int timeout) {
	try {
		int ret = -1;
		if (opened) {
			ret = usb_bulk_write(handle, bulkOutEndpoint, bytes, size, timeout);
//			if (ret > 0) {
//				debugLog(env, "Bulk write: %d byte(s) have been written..", ret);
//			} else if (ret == 0) {
//				env->ThrowNew(env->FindClass(EX_IO_TIMEOUT_EXCEPTION), "No bytes has been written to USB device in timeout!");
//			} else {
//				errorLog(env, "Bulk write operation failed with error: %d!", ret);
//				env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), usb_strerror());
//			}
		} else {
			env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), "Cannot perform bulk write, port is not opened!");
		}
		return ret;
	} catch (...) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), "Bulk write operation failed!");
	}
	return UNDEF;
}

bool detachKernelDriver(JNIEnv *env, usb_dev_handle *handle, int interfaceNumber) {
#if defined(LIBUSB_HAS_GET_DRIVER_NP)
#if defined(LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP)
	char name[256];
	if (usb_get_driver_np(handle, interfaceNumber, (char *) name, sizeof(name))) {
		Logger::getInstance()->error(LP, std::string("Fetching driver name failed: ")+ usb_strerror());
		throw "Fetching driver name failed!";
	} else {
		Logger::getInstance()->debug(LP, std::string("Driver name: ")+ name);
		if (usb_detach_kernel_driver_np(handle, interfaceNumber)) {
			Logger::getInstance()->error(LP, std::string("Detaching kernel driver failed: ")+
					usb_strerror());
			throw "Detaching kernel driver failed!";
		}
		Logger::getInstance()->debug(LP, "Kernel driver has been detached successfully.");
	}
	return true;
#endif
#endif
	return false;
}

#ifdef LIBUSB_HAS_GET_DRIVER_NP
	void showDriverName(JNIEnv *env, usb_dev_handle *handle, int interfaceNumber) {
		const int nameLen = 256;
		char driverName[nameLen];
		usb_get_driver_np(handle, interfaceNumber, driverName, nameLen);
		Logger::getInstance()->debug(LP, std::string("Driver name bound to interface: ")+ driverName);
	}
#endif

bool USBDevice::open() {
	Logger::getInstance()->debug(LP, "Attempt to open port..");
	if (opened) {
		Logger::getInstance()->error(LP, "Port is already opened!");
		return false;
	}

	handle = usb_open(device);
	if (!handle) {
		Logger::getInstance()->error(LP, string("Opening device failed: ") + usb_strerror());
		throw "Opening device failed!";
	} else {
		Logger::getInstance()->debug(LP, "Device has been opened successfuly..");
	}

	configValue = device->config->bConfigurationValue;

#ifdef WIN32
	if (usb_set_configuration(handle, configValue)) {
		Logger::getInstance()->error(LP, "Setting configuration failed!");
		throw "Setting configuration failed!";
	} else {
		Logger::getInstance()->debug(LP, "Configuration has been set successfuly..");
	}
#endif

	if ((this->device->config->bNumInterfaces > 0) && (this->device->config->interface->num_altsetting > 0)) {
		interfaceNumber = device->config->interface->altsetting->bInterfaceNumber;

		if (usb_claim_interface(handle, interfaceNumber)) {
			opened = false;
			Logger::getInstance()->error(LP, string("Claiming interface failed: ")+ usb_strerror());
			throw "Access denied to USB device . This error is probably caused by insufficient access rights";
		} else {
			Logger::getInstance()->debug(LP, "Interface has been claimed successfuly..");
			opened = true;
		}
	}

	if ((bulkInEndpoint == UNDEF) || (bulkOutEndpoint == UNDEF)) {
		Logger::getInstance()->debug(LP, "Looking for endpoints..");
		//find default endpoints
		for (int i = 0; i < device->config->interface->altsetting->bNumEndpoints; i++) {
			if ((bulkInEndpoint != UNDEF) && (bulkOutEndpoint != UNDEF)) break;
			if (device->config->interface->altsetting->endpoint[i].bEndpointAddress & 0x80) {
				if ((bulkInEndpoint == UNDEF)
					&& ((device->config->interface->altsetting->endpoint[i].bmAttributes & 0x3f) == 0x2))
				{
					bulkInEndpoint = device->config->interface->altsetting->endpoint[i].bEndpointAddress;
					stringstream ss;
					ss<<"Bulk IN endpoint found and ready to use on address "<< bulkInEndpoint;
					Logger::getInstance()->debug(LP, ss.str());
				}
			} else {
				if ((bulkOutEndpoint == UNDEF)
					&& ((device->config->interface->altsetting->endpoint[i].bmAttributes & 0x3f) == 0x2))
				{
					bulkOutEndpoint = device->config->interface->altsetting->endpoint[i].bEndpointAddress;
					stringstream ss;
					ss<<"Bulk OUT endpoint found and ready to use on address %02xh"<< bulkOutEndpoint;
					Logger::getInstance()->debug(LP, ss.str());
				}
			}
		}

		if ((bulkInEndpoint == UNDEF) || (bulkOutEndpoint == UNDEF)) {
			Logger::getInstance()->error(LP, "Bulk endpoints cannot be found!");
			throw "Bulk endpoints cannot be found!";
		}
	}

	return opened;
}

bool USBDevice::isOpen() {
	return opened;
}

bool USBDevice::close(JNIEnv *env) {
	if (!opened) {
		return true;
	}
	char desc[256] = "";

	opened = false;
	int ret = 0;

#ifdef WIN32
	if ((ret = usb_release_interface(handle, interfaceNumber))) {
		snprintf(desc, sizeof(desc), "Releasing interface failed with message: %s\n", usb_strerror());
	} else {
		Logger::getInstance()->debug(LP, "Interface has been released successfuly..");
	}
#endif

	if ((ret = usb_close(handle))) {
		snprintf(desc + strlen(desc), sizeof(desc) - strlen(desc), "Closing device handle failed with message %s!", usb_strerror());
		handle = NULL;
	} else {
		Logger::getInstance()->debug(LP, "Usb device has been closed successfuly..");
	}


	if (strlen(desc) > 0) {
		Logger::getInstance()->debug(LP, desc);
	}

	jclass clazz = env->FindClass(CLASS_USB_NATIVE_HANDLER);
	jmethodID methodID = env->GetStaticMethodID(clazz, METHOD_NH_GET_INSTANCE, METHOD_NH_GET_INSTANCE_SIG);
	jobject nativeHandler = env->CallStaticObjectMethod(clazz, methodID);

	jclass jdeviceKeyClass = env->FindClass(CLASS_USB_DEVICE_KEY);
	jmethodID jdeviceKeyConstructor = env->GetMethodID(jdeviceKeyClass, METHOD_CONSTRUCTOR, METHOD_CONST_USB_DEVICE_KEY_SIG);
	jobject deviceKey = env->NewObject(jdeviceKeyClass, jdeviceKeyConstructor,
			key.devnum, newString(env, key.bus_dirname.c_str()), newString(env, key.device_filename.c_str()), key.vid, key.pid);

	methodID = env->GetMethodID(clazz, METHOD_NH_USB_DEREGISTERED, METHOD_NH_USB_DEREGISTERED_SIG);
	env->CallVoidMethod(nativeHandler, methodID, deviceKey);

	return strlen(desc) == 0;
}

struct usb_device* USBDevice::getDevice() {
	return device;
}

USBDevice::~USBDevice() {
	if (isOpen()) {
		usb_release_interface(handle, interfaceNumber);
		usb_close(handle);
	}
}

USBDeviceMapKey::USBDeviceMapKey(unsigned char devNum, const char *busDirname, const char *devFilename, jint vid, jint pid) {
	this->bus_dirname = busDirname;
	this->device_filename = devFilename;
	this->devnum = devNum;
	this->vid = vid;
	this->pid = pid;
}
