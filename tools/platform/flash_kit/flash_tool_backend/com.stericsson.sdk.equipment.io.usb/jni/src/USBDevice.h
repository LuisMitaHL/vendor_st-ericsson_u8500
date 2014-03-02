/*
 * USBDevice.h
 *
 *  Created on: Jul 16, 2009
 *      Author: emicroh
 */

#ifndef USBDEVICE_H_
#define USBDEVICE_H_

#include <string>
#include <jni.h>
#include "usb.h"

class USBDeviceMapKey {
public:
	USBDeviceMapKey(unsigned char devNum, const char *busDirname, const char *devFilename, jint vid, jint pid);
	unsigned char devnum;
	std::string bus_dirname;
	std::string device_filename;
	jint vid;
	jint pid;
};

struct USBDeviceMapKeyCmp {
  bool operator()( USBDeviceMapKey k1, USBDeviceMapKey k2 ) const {
	  if (k1.vid != k2.vid){
		  return k1.vid < k2.vid;
	  } else if (k1.pid != k2.pid) {
		  return k1.pid < k2.pid;
	  } else if (k1.devnum != k2.devnum) {
		  return k1.devnum < k2.devnum;
	  } else {
		  return k1.bus_dirname.compare(k2.bus_dirname)<0;
	  }
  }
};


class USBDevice {
public:
	USBDevice(const USBDeviceMapKey & key, struct usb_device *device, const std::string & bus_dirname, const std::string & device_filename);
	int bulkRead(JNIEnv *env, char *bytes, int size, int timeout);
	int bulkWrite(JNIEnv *env, char *bytes, int size, int timeout);
	bool open();
	bool isOpen();
	bool close(JNIEnv *env);
	struct usb_device* getDevice();

	~USBDevice();

	USBDeviceMapKey getKey() const
    {
        return key;
    }

    int getBulkInEndpoint() const
    {
        return bulkInEndpoint;
    }

    int getBulkOutEndpoint() const
    {
        return bulkOutEndpoint;
    }

    std::string getBus_dirname() const
    {
        return bus_dirname;
    }

    int getConfigValue() const
    {
        return configValue;
    }

    std::string getDevice_filename() const
    {
        return device_filename;
    }

    struct usb_dev_handle *getHandle() const
    {
        return handle;
    }

    int getInterfaceNumber() const
    {
        return interfaceNumber;
    }

    bool getOpened() const
    {
        return opened;
    }


private:
    const USBDeviceMapKey key;
	struct usb_device *device;
	struct usb_dev_handle *handle;

	const std::string bus_dirname;
	const std::string device_filename;

	int configValue;
	int interfaceNumber;
	int bulkInEndpoint;
	int bulkOutEndpoint;

	bool opened;
};

#endif /* USBDEVICE_H_ */
