/*
 * USBHandler.cpp
 *
 *  Created on: Jun 17, 2009
 *      Author: emicroh
 */

#include "USBHandler.h"
#include "javadefs.h"
#include <cstring>
#include "Logger.h"
#include <iomanip>
#include <iostream>
#include <sstream>

void print_endpoint(struct usb_endpoint_descriptor *endpoint) {
	std::stringstream ss;
	ss << "      bDescriptorType: " << std::setbase(16)
			<< endpoint->bDescriptorType;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "      bEndpointAddress: " << std::setbase(16)
			<< endpoint->bEndpointAddress;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "      bmAttributes:     " << std::setbase(16)
			<< endpoint->bmAttributes;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "      wMaxPacketSize:   " << endpoint->wMaxPacketSize;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "      bInterval:        " << endpoint->bInterval;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "      bRefresh:         " << endpoint->bRefresh;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "      bSynchAddress:    " << endpoint->bSynchAddress;
	Logger::getInstance()->debug(LP, ss.str());
}

void print_altsetting(struct usb_interface_descriptor *interface) {
	int i;

	struct usb_interface_descriptor {
		unsigned char bLength;
		unsigned char bDescriptorType;
		unsigned char bInterfaceNumber;
		unsigned char bAlternateSetting;
		unsigned char bNumEndpoints;
		unsigned char bInterfaceClass;
		unsigned char bInterfaceSubClass;
		unsigned char bInterfaceProtocol;
		unsigned char iInterface;

		struct usb_endpoint_descriptor *endpoint;

		unsigned char *extra; /* Extra descriptors */
		int extralen;
	};

	std::stringstream ss;
	ss << "    bInterfaceNumber:   " << interface->bInterfaceNumber;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "    bAlternateSetting:  " << interface->bAlternateSetting;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "    bNumEndpoints:      " << interface->bNumEndpoints;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "    bInterfaceClass:    " << interface->bInterfaceClass;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "    bInterfaceSubClass: " << interface->bInterfaceSubClass;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "    bInterfaceProtocol: " << interface->bInterfaceProtocol;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "    iInterface:         " << interface->iInterface;
	Logger::getInstance()->debug(LP, ss.str());

	for (i = 0; i < interface->bNumEndpoints; i++) {
		print_endpoint(&interface->endpoint[i]);
	}
}

void print_interface(struct usb_interface *interface) {
	int i;

	for (i = 0; i < interface->num_altsetting; i++) {
		print_altsetting(&interface->altsetting[i]);
	}
}

void print_configuration(struct usb_config_descriptor *config) {
	int i;
	std::stringstream ss;
	ss << "  wTotalLength:         " << config->wTotalLength;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "  bNumInterfaces:       " << config->bNumInterfaces;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "  bConfigurationValue:  " << config->bConfigurationValue;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "  iConfiguration:       " << config->iConfiguration;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "  bmAttributes:         " << std::setbase(16)
			<< config->bmAttributes;
	Logger::getInstance()->debug(LP, ss.str());
	ss.str("");
	ss << "  MaxPower:             " << config->MaxPower;
	Logger::getInstance()->debug(LP, ss.str());

	for (i = 0; i < config->bNumInterfaces; i++) {
		print_interface(&config->interface[i]);
	}
}

int print_device(struct usb_device *dev, int level) {
	usb_dev_handle *udev;
	char description[256];
	char string[256];
	int ret, i;

	udev = usb_open(dev);
	if (udev) {
		if (dev->descriptor.iManufacturer) {
			ret = usb_get_string_simple(udev, dev->descriptor.iManufacturer,
					string, sizeof(string));
			if (ret > 0)
				snprintf(description, sizeof(description), "%s - ", string);
			else
				snprintf(description, sizeof(description), "%04X - ",
						dev->descriptor.idVendor);
		} else
			snprintf(description, sizeof(description), "%04X - ",
					dev->descriptor.idVendor);

		if (dev->descriptor.iProduct) {
			ret = usb_get_string_simple(udev, dev->descriptor.iProduct, string,
					sizeof(string));
			if (ret > 0)
				snprintf(description + strlen(description),
						sizeof(description) - strlen(description), "%s",
						string);
			else
				snprintf(description + strlen(description),
						sizeof(description) - strlen(description), "%04X",
						dev->descriptor.idProduct);
		} else
			snprintf(description + strlen(description),
					sizeof(description) - strlen(description), "%04X",
					dev->descriptor.idProduct);

	} else
		snprintf(description, sizeof(description), "%04X - %04X",
				dev->descriptor.idVendor, dev->descriptor.idProduct);

	std::stringstream ss;
	ss << std::string(level * 2, ' ') << "Dev #" << dev->devnum << description;
	Logger::getInstance()->debug(LP, ss.str());

	if (udev) {
		if (dev->descriptor.iSerialNumber) {
			ret = usb_get_string_simple(udev, dev->descriptor.iSerialNumber,
					string, sizeof(string));
			if (ret > 0) {
				ss.str("");
				ss << std::string(level * 2, ' ') << "  - Serial Number: "
						<< string;
				Logger::getInstance()->debug(LP, ss.str());
			}
		}
	}

	if (udev)
		usb_close(udev);

	if (!dev->config) {
		Logger::getInstance()->debug(LP, "  Couldn't retrieve descriptors");
		return 0;
	}

	for (i = 0; i < dev->descriptor.bNumConfigurations; i++)
		print_configuration(&dev->config[i]);

	for (i = 0; i < dev->num_children; i++)
		print_device(dev->children[i], level + 1);

	return 0;
}

USBHandler::USBHandler(jint libusbLogLevel) {
	usb_set_debug(0);
	usb_init();
}

void USBHandler::checkBusses(JNIEnv *env, jobject src) {
	Logger::getInstance()->debug(LP, "checkBusses");
	if ((usb_find_busses() > 0) | (usb_find_devices() > 0)) {
		Logger::getInstance()->debug(LP,
				"USB bus state changed, scan devices..");
		checkLibUSBDevices(env, src);
	}
}

void USBHandler::checkLibUSBDevices(JNIEnv *env, jobject src) {
	struct usb_bus *bus;

	//key set for cached devices
	device_map_key_set devicesKeySet;

	device_map_key_set::iterator cdi;
	device_map::const_iterator dmi;

	//store keys of all cached devices at the moment to identify at the end of enumeration
	//disconnected devices
	for (dmi = devices.begin(); dmi != devices.end(); dmi++) {
		devicesKeySet.insert(dmi->first);
	}

	Logger::getInstance()->debug(LP, "Check Devices");
	Logger::getInstance()->debug(LP, "----------------------------");

	//handle bus enumeration
	for (bus = usb_get_busses(); bus; bus = bus->next) {
		Logger::getInstance()->debug(LP,
				std::string("Bus ") + bus->dirname + " enumerated");
		struct usb_device *dev;
		for (dev = bus->devices; dev; dev = dev->next) {
			//debugLog(env, "Device enumerated");
			std::stringstream ss;
			ss << bus->dirname << "." << dev->filename << std::setbase(16)
					<< dev->descriptor.idVendor << std::setbase(16)
					<< dev->descriptor.idProduct;
			Logger::getInstance()->debug(LP, ss.str());

			//Unique key for each device
			USBDeviceMapKey tmpKey(dev->devnum, bus->dirname, dev->filename,
					dev->descriptor.idVendor, dev->descriptor.idProduct);

			RejectedDeviceKey rejectedDev(dev->descriptor.idVendor,
					dev->descriptor.idProduct);

			//Attempt to find device in cached devices
			device_map_key_set::iterator it = devicesKeySet.find(tmpKey);
			rejected_devices_set::iterator itrd = rejected_devices.find(
					rejectedDev);

			//if device is already cached it means that matches VID, PID, bus_dirname, etc. it's treated like
			//no change on device and no need for special handling -> enumeration will continue immediately
			if (it != devicesKeySet.end()) {
				devicesKeySet.erase(it);
				continue;
			} else if (itrd != rejected_devices.end()) { //device was once rejected, it has no sense to ask for it again
				continue;
			}

			//device is not cached so we need to find out if VID & PID is supported
			jboolean accept = env->CallBooleanMethod(src,
					getAcceptUSBDeviceMethod(env),
					(jint) dev->descriptor.idVendor,
					(jint) dev->descriptor.idProduct);

			if (accept) {
				jobject deviceDescriptor;
				jclass nativeDevClass = env->FindClass(CLASS_USB_NATIVE_DEVICE);
				jmethodID ddConstID = env->GetMethodID(nativeDevClass,
						METHOD_CONSTRUCTOR, METHOD_CONST_USB_NATIVE_DEVICE_SIG);
				jobject nativeUsbDevice;

				Logger::getInstance()->debug(LP, "Found new uncached device..");
				devices[tmpKey] = new USBDevice(tmpKey, dev, bus->dirname,
						dev->filename);

				getDeviceConfigs(env, &deviceDescriptor, dev, bus->dirname);
				nativeUsbDevice = env->NewObject(nativeDevClass, ddConstID,
						deviceDescriptor);

				env->CallVoidMethod(src, getRegisterUSBMethod(env),
						nativeUsbDevice);
			} else {
				rejected_devices.insert(rejectedDev);

//				debugLog(env, "Adding [%d, %d] into rejected devices..", rejectedDev->vid, rejectedDev->pid);
//				debugLog(env, "Dumping rejected devices..");
//				rejected_devices_set::iterator rit;
//				for (rit = rejected_devices.begin(); rit != rejected_devices.end(); rit++) {
//					debugLog(env, "--rejected device vid = %d, pid = %d", rit->vid, rit->pid);
//				}

			}
		}
	}

	Logger::getInstance()->debug(LP, "----------------------------");

	//handle disconnected devices
	for (cdi = devicesKeySet.begin(); cdi != devicesKeySet.end(); cdi++) {
		dmi = devices.find(*cdi);
		if (dmi != devices.end()) {
			Logger::getInstance()->debug(LP, "Closing disconnected device..");
			dmi->second->close(env);
			devices.erase(dmi->first);
		}
	}

}

void USBHandler::setIntField(JNIEnv *env, jobject srcObject,
		const char *fieldName, jint value) {
	jfieldID fieldId = env->GetFieldID(env->GetObjectClass(srcObject),
			fieldName, "I");
	env->SetIntField(srcObject, fieldId, value);
}

void USBHandler::setStringField(JNIEnv *env, jobject srcObject,
		const char *fieldName, const char *value) {
	jfieldID fieldID = env->GetFieldID(env->GetObjectClass(srcObject),
			fieldName, "Ljava/lang/String;");
	env->SetObjectField(srcObject, fieldID, newString(env, value));
}
jclass USBHandler::getUSBHandlerCls(JNIEnv *env) {
	jclass cls = env->FindClass(CLASS_USB_NATIVE_HANDLER);
	if (cls == NULL) {
		Logger::getInstance()->error(LP,
				"USBNativeHandler cannot be found, callbacks initialization failed!");
	}
	return cls;
}
jmethodID USBHandler::getRegisterUSBMethod(JNIEnv *env) {
	return env->GetMethodID(getUSBHandlerCls(env), METHOD_NH_REGISTER_USB,
			METHOD_NH_REGISTER_USB_SIG);
}
jmethodID USBHandler::getAcceptUSBDeviceMethod(JNIEnv *env) {
	return env->GetMethodID(getUSBHandlerCls(env), METHOD_NH_ACCEPT_USB_DEVICE,
			METHOD_NH_ACCEPT_USB_DEVICE_SIG);
}
void USBHandler::getDeviceConfigs(JNIEnv *env, jobject *deviceDescriptor,
		struct usb_device *dev, const char *bus_dirname) {
	jobject usbDesc;
	jobjectArray configs;
	//new object for descriptor field
	jclass usbdClazz = env->FindClass(CLASS_USB_DESCRIPTOR);
	jmethodID usbdConstructorID = env->GetMethodID(usbdClazz,
			METHOD_CONSTRUCTOR, "()V");
	usbDesc = env->NewObject(usbdClazz, usbdConstructorID);
	//new object for config descriptor field
	configs = env->NewObjectArray(dev->descriptor.bNumConfigurations,
			env->FindClass(CLASS_USB_CONF_DESCRIPTOR), NULL);

	jclass ddClazz = env->FindClass(CLASS_DEVICE_DESCRIPTOR);
	jmethodID ddConstID = env->GetMethodID(ddClazz, METHOD_CONSTRUCTOR,
			METHOD_CONST_DEFAULT_SIG);
	*deviceDescriptor = env->NewObject(ddClazz, ddConstID);

	//set device number
	env->SetIntField(*deviceDescriptor,
			env->GetFieldID(ddClazz, FN_DEVNUM, "I"), dev->devnum);

	//set bus dirname
	env->SetObjectField(*deviceDescriptor,
			env->GetFieldID(ddClazz, FN_BUS_DIR_NAME, "Ljava/lang/String;"),
			newString(env, bus_dirname));

	//set bus dirname
	env->SetObjectField(*deviceDescriptor,
			env->GetFieldID(ddClazz, FN_DEVICE_FILE_NAME, "Ljava/lang/String;"),
			newString(env, dev->filename));

	//set usb descriptor
	jfieldID usbdFieldID = env->GetFieldID(ddClazz, FN_USB_DESCRIPTOR,
			F_USB_DESCRIPTOR_SIG);
	env->SetObjectField(*deviceDescriptor, usbdFieldID, usbDesc);

	//set config descriptors
	jfieldID confField = env->GetFieldID(ddClazz, FN_USB_CONF_DESCRIPTOR_ARRAY,
			F_USB_CONF_DESCRIPTOR_ARRAY_SIG);
	env->SetObjectField(*deviceDescriptor, confField, configs);

	char desc[256];
	usb_dev_handle *udev = usb_open(dev);

	if (udev) {
		//set descriptor values
		setIntField(env, usbDesc, FN_USBD_BLENGTH, dev->descriptor.bLength);
		setIntField(env, usbDesc, FN_USBD_BDESC_TYPE,
				dev->descriptor.bDescriptorType);
		setIntField(env, usbDesc, FN_USBD_BCDUSB, dev->descriptor.bcdUSB);
		setIntField(env, usbDesc, FN_USBD_BDEV_CLASS,
				dev->descriptor.bDeviceClass);
		setIntField(env, usbDesc, FN_USBD_BDEV_SUBCLASS,
				dev->descriptor.bDeviceSubClass);
		setIntField(env, usbDesc, FN_USBD_BDEV_PROT,
				dev->descriptor.bDeviceProtocol);
		setIntField(env, usbDesc, FN_USBD_BMAX_PKT_SIZE0,
				dev->descriptor.bMaxPacketSize0);
		setIntField(env, usbDesc, FN_USBD_ID_VENDOR, dev->descriptor.idVendor);
		setIntField(env, usbDesc, FN_USBD_ID_PRODUCT,
				dev->descriptor.idProduct);
		if (dev->descriptor.iManufacturer
				&& (usb_get_string_simple(udev, dev->descriptor.iManufacturer,
						desc, sizeof(desc)) > 0)) {
			setStringField(env, usbDesc, FN_USBD_MANUFACTURER, desc);
		}
		if (dev->descriptor.iProduct
				&& (usb_get_string_simple(udev, dev->descriptor.iProduct, desc,
						sizeof(desc)) > 0)) {
			setStringField(env, usbDesc, FN_USBD_PRODUCT, desc);
		}
		if (dev->descriptor.iSerialNumber
				&& (usb_get_string_simple(udev, dev->descriptor.iSerialNumber,
						desc, sizeof(desc)) > 0)) {
			setStringField(env, usbDesc, FN_USBD_SERIAL, desc);
		}
		setIntField(env, usbDesc, FN_USBD_BCDDEV, dev->descriptor.bcdDevice);

		//create and set instances of inner class to store USB configuration descriptor info
		for (int i = 0; i < dev->descriptor.bNumConfigurations; i++) {
			//create config desc instance
			jclass cDescClazz = env->FindClass(CLASS_USB_CONF_DESCRIPTOR);
			jmethodID cDescConstID = env->GetMethodID(cDescClazz,
					METHOD_CONSTRUCTOR, "()V");
			jobject cdescVal = env->NewObject(cDescClazz, cDescConstID);
			//put config desc into array
			env->SetObjectArrayElement(configs, i, cdescVal);

			//set fields for config desc
			setIntField(env, cdescVal, FN_CONFD_BLENGTH, dev->config->bLength);
			setIntField(env, cdescVal, FN_CONFD_BDESC_TYPE,
					dev->config->bDescriptorType);
			setIntField(env, cdescVal, FN_CONFD_WTOT_LENGTH,
					dev->config->wTotalLength);
			setIntField(env, cdescVal, FN_CONFD_BNUM_INT,
					dev->config->bNumInterfaces);
			setIntField(env, cdescVal, FN_CONFD_BCONF_VAL,
					dev->config->bConfigurationValue);
			if (dev->config->iConfiguration
					&& (usb_get_string_simple(udev, dev->config->iConfiguration,
							desc, sizeof(desc)) > 0)) {
				setStringField(env, cdescVal, FN_CONFD_DESC, desc);
			} else {
				setStringField(env, cdescVal, FN_CONFD_DESC, "");
			}
			setIntField(env, cdescVal, FN_CONFD_BM_ATTRS,
					dev->config->bmAttributes);
			setIntField(env, cdescVal, FN_CONFD_MAX_POWER,
					dev->config->MaxPower);

			int intCount = 0;
			int ni, alt;
			for (ni = 0; ni < dev->config->bNumInterfaces; ni++) {
				intCount += dev->config->interface[ni].num_altsetting;
			}
			jobjectArray interfaces = env->NewObjectArray(intCount,
					env->FindClass(CLASS_USB_INT_DESCRIPTOR), NULL);
			jfieldID intField = env->GetFieldID(cDescClazz,
					FN_USB_IDESCRIPTOR_ARRAY, F_USB_IDESCRIPTOR_ARRAY_SIG);
			env->SetObjectField(cdescVal, intField, interfaces);

			for (ni = 0; ni < dev->config->bNumInterfaces; ni++) {
				for (alt = 0; alt < dev->config->bNumInterfaces; alt++) {
					jclass iDescClazz = env->FindClass(
							CLASS_USB_INT_DESCRIPTOR);
					jmethodID iDescConstID = env->GetMethodID(iDescClazz,
							METHOD_CONSTRUCTOR, METHOD_CONST_DEFAULT_SIG);
					jobject iVal = env->NewObject(iDescClazz, iDescConstID);
					env->SetObjectArrayElement(interfaces, ni + alt, iVal);

					usb_interface_descriptor idesc =
							dev->config->interface[ni].altsetting[alt];

					setIntField(env, iVal, FN_USB_IDESC_BLENGTH, idesc.bLength);
					setIntField(env, iVal, FN_USB_IDESC_BDESC_TYPE,
							idesc.bDescriptorType);
					setIntField(env, iVal, FN_USB_IDESC_BINT_NUM,
							idesc.bInterfaceNumber);
					setIntField(env, iVal, FN_USB_IDESC_BALT_SETS,
							idesc.bAlternateSetting);
					setIntField(env, iVal, FN_USB_IDESC_BNUM_ENDPOINTS,
							idesc.bNumEndpoints);
					setIntField(env, iVal, FN_USB_IDESC_BINT_CLASS,
							idesc.bInterfaceClass);
					setIntField(env, iVal, FN_USB_IDESC_BINT_SUBCLASS,
							idesc.bInterfaceSubClass);
					setIntField(env, iVal, FN_USB_IDESC_BINT_PROT,
							idesc.bInterfaceProtocol);
					if (idesc.iInterface
							&& (usb_get_string_simple(udev, idesc.iInterface,
									desc, sizeof(desc)) > 0)) {
						setStringField(env, iVal, FN_USB_IDESC_DESC, desc);
					} else {
						setStringField(env, iVal, FN_USB_IDESC_DESC, "");
					}

					jobjectArray endpoints = env->NewObjectArray(
							idesc.bNumEndpoints,
							env->FindClass(CLASS_USB_EP_DESCRIPTOR), NULL);
					jfieldID epsField = env->GetFieldID(iDescClazz,
							FN_USB_EPDESCRIPTOR_ARRAY,
							F_USB_EPDESCRIPTOR_ARRAY_SIG);
					env->SetObjectField(iVal, epsField, endpoints);

					for (int ep = 0; ep < idesc.bNumEndpoints; ep++) {
						jclass epDescClazz = env->FindClass(
								CLASS_USB_EP_DESCRIPTOR);
						jmethodID epDescConstID = env->GetMethodID(epDescClazz,
								METHOD_CONSTRUCTOR, METHOD_CONST_DEFAULT_SIG);
						jobject eVal = env->NewObject(epDescClazz,
								epDescConstID);
						env->SetObjectArrayElement(endpoints, ep, eVal);

						setIntField(env, eVal, FN_USB_EPDESC_BLENGTH,
								idesc.endpoint[ep].bLength);
						setIntField(env, eVal, FN_USB_EPDESC_BDESC_TYPE,
								idesc.endpoint[ep].bDescriptorType);
						setIntField(env, eVal, FN_USB_EPDESC_BEP_ADDR,
								idesc.endpoint[ep].bEndpointAddress);
						setIntField(env, eVal, FN_USB_EPDESC_BM_ATTRS,
								idesc.endpoint[ep].bmAttributes);
						setIntField(env, eVal, FN_USB_EPDESC_WMAX_PKT_SIZE,
								idesc.endpoint[ep].wMaxPacketSize);
						setIntField(env, eVal, FN_USB_EPDESC_BINT,
								idesc.endpoint[ep].bInterval);
						setIntField(env, eVal, FN_USB_EPDESC_BREFRESH,
								idesc.endpoint[ep].bRefresh);
						setIntField(env, eVal, FN_USB_EPDESC_BSYNCH_ADDR,
								idesc.endpoint[ep].bSynchAddress);
					}
				}
			}
		}
	}

	if (udev) {
		usb_close(udev);
	}
}

void USBHandler::removeDevice(const USBDeviceMapKey & key) {
	device_map::iterator idev = devices.find(key);
	if (idev != devices.end()) {
		devices.erase(idev);
	}
}

USBDevice* USBHandler::getDevice(const USBDeviceMapKey & key) {
	device_map::iterator idev = devices.find(key);
	if (idev == devices.end()) {
		return NULL;
	} else {
		return idev->second;
	}
}

bool USBHandler::isCached(const USBDeviceMapKey & key) {
	device_map::iterator idev = devices.find(key);
	return !(idev == devices.end());
}

USBHandler::~USBHandler() {
	device_map::iterator dmi;
	for (dmi = devices.begin(); dmi != devices.end(); dmi++) {
		dmi->second->close(NULL);
	}
	devices.clear();
}

RejectedDeviceKey::RejectedDeviceKey(jint vid, jint pid) {
	this->vid = vid;
	this->pid = pid;
}
