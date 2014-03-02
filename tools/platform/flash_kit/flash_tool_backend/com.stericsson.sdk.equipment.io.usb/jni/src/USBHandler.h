/*
 * USBHandler.h
 *
 *  Created on: Jun 17, 2009
 *      Author: emicroh
 */

#ifndef USBHANDLER_H_
#define USBHANDLER_H_

#include <jni.h>
#include <map>
#include <set>

#include "usb.h"
#include "util.h"
#include "USBDevice.h"

#define USB_DEBUG_LEVEL 1

class RejectedDeviceKey {
public:
	RejectedDeviceKey(jint vid, jint pid);
	jint vid;
	jint pid;
};

class RejectedDeviceKeyCmp {
public:
	bool operator()( RejectedDeviceKey k1, RejectedDeviceKey k2 ) const {
		if (k1.vid != k2.vid) {
			return (k1.vid < k2.vid);
		} else
			return (k1.pid < k2.pid);
	}
};

typedef std::map<USBDeviceMapKey, USBDevice *, USBDeviceMapKeyCmp> device_map;
typedef std::set<USBDeviceMapKey, USBDeviceMapKeyCmp> device_map_key_set;
typedef std::set<RejectedDeviceKey, RejectedDeviceKeyCmp> rejected_devices_set;

class USBHandler {
public:
	USBHandler(jint jniLogLevel);
	~USBHandler();
	USBDevice* getDevice(unsigned char devnum, const char *bus_dirname);
	USBDevice* getDevice(const USBDeviceMapKey & key);
	void removeDevice(const USBDeviceMapKey & key);
	bool isCached(const USBDeviceMapKey & key);
	void checkBusses(JNIEnv *env, jobject src);
private:
	device_map devices;
	rejected_devices_set rejected_devices;
	jclass getUSBHandlerCls(JNIEnv *env);
	jmethodID getRegisterUSBMethod(JNIEnv *env);
	jmethodID getAcceptUSBDeviceMethod(JNIEnv *env);
	void getDeviceConfigs(JNIEnv *env, jobject *deviceDescriptor, struct usb_device *dev, const char *bus_dirname);
	void checkLibUSBDevices(JNIEnv *env, jobject src);
	void setIntField(JNIEnv *env, jobject srcObject, const char *fieldName, jint value);
	void setStringField(JNIEnv *env, jobject srcObject, const char *fieldName, const char *value);
};

#endif /* USBHANDLER_H_ */
