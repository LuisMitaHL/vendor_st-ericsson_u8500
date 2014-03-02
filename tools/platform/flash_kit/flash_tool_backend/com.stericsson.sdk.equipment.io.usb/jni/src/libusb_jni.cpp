/*
 * jusb_handler.cpp
 *
 *  Created on: Jun 15, 2009
 *      Author: emicroh
 */

#include "com_stericsson_sdk_equipment_io_usb_internal_USBNativeHandler.h"
#include "com_stericsson_sdk_equipment_io_usb_internal_USBNativeDevice.h"

#include "util.h"
#include "USBHandler.h"
#include "javadefs.h"
#include "Logger.h"
USBHandler *usbHandler;

JNIEXPORT void JNICALL Java_com_stericsson_sdk_equipment_io_usb_internal_USBNativeHandler_initJNILayer(
		JNIEnv *env, jobject src, jint libusbLogLevel, jint jniLogLevel) {
	Logger::getInstance()->setLogLevel(jniLogLevel);
	Logger::getInstance()->info(LP, "init");
	try { //it's not allowed to use logging here
		usbHandler = new USBHandler(libusbLogLevel);
	} catch (const char *errorString) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), errorString);
	} catch (...) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION),
				"Unexpected native call failure in method initDll!");
	}
	Logger::getInstance()->info(LP, "init Done");
}

JNIEXPORT void JNICALL Java_com_stericsson_sdk_equipment_io_usb_internal_USBNativeHandler_checkBusses(
		JNIEnv *env, jobject src) {
	try {
		usbHandler->checkBusses(env, src);
	} catch (const char *errorString) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), errorString);
	} catch (...) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION),
				"Unexpected native call failure in method checkBusses!");
	}
}

USBDevice * getUSBDevice(JNIEnv *env, jobject src) {
	jmethodID methodID = env->GetMethodID(env->GetObjectClass(src),
			METHOD_ND_GET_KEY, METHOD_ND_GET_KEY_SIG);
	jobject jdevKey = env->CallObjectMethod(src, methodID);

	methodID = env->GetMethodID(env->GetObjectClass(jdevKey),
			METHOD_DK_GET_BUS_DIRNAME, METHOD_DK_GET_BUS_DIRNAME_SIG);
	const char *bus_dirname = getStringBody(env,
			(jstring) env->CallObjectMethod(jdevKey, methodID));

	methodID = env->GetMethodID(env->GetObjectClass(jdevKey),
			METHOD_DK_GET_DEV_FILENAME, METHOD_DK_GET_DEV_FILENAME_SIG);
	const char *device_filename = getStringBody(env,
			(jstring) env->CallObjectMethod(jdevKey, methodID));

	methodID = env->GetMethodID(env->GetObjectClass(jdevKey),
			METHOD_DK_GET_DEVNUM, METHOD_DK_GET_DEVNUM_SIG);
	const unsigned char devnum = (unsigned char) env->CallIntMethod(jdevKey,
			methodID);

	methodID = env->GetMethodID(env->GetObjectClass(jdevKey),
			METHOD_DK_GET_VENDOR_ID, METHOD_DK_GET_VENDOR_ID_SIG);
	const jint idVendor = env->CallIntMethod(jdevKey, methodID);

	methodID = env->GetMethodID(env->GetObjectClass(jdevKey),
			METHOD_DK_GET_PRODUCT_ID, METHOD_DK_GET_PRODUCT_ID_SIG);
	const jint idProduct = env->CallIntMethod(jdevKey, methodID);

	USBDeviceMapKey cdevKey(devnum, bus_dirname, device_filename, idVendor,
			idProduct);

	USBDevice *res = usbHandler->getDevice(cdevKey);

	return res;
}

JNIEXPORT void JNICALL Java_com_stericsson_sdk_equipment_io_usb_internal_USBNativeDevice_open(
		JNIEnv *env, jobject src) {
	try {
		USBDevice *device = getUSBDevice(env, src);

		if (device) {
			device->open();
		} else {
			Logger::getInstance()->error(LP, "Cannot find device!");
		}
	} catch (const char *errorString) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), errorString);
	} catch (...) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION),
				"Unexpected native call failure in method open!");
	}
}

JNIEXPORT void JNICALL Java_com_stericsson_sdk_equipment_io_usb_internal_USBNativeDevice_close(
		JNIEnv *env, jobject src) {
	try {
		USBDevice *device = getUSBDevice(env, src);

		if (device) {
			device->close(env);
			Logger::getInstance()->debug(LP, "Removing device from internal cache..");
			usbHandler->removeDevice(device->getKey());
			if (usbHandler->getDevice(device->getKey()) != NULL) {
				Logger::getInstance()->error(LP, "Removing from internal cache failed!");
			}
		} else {
			Logger::getInstance()->error(LP, "Cannot find device!");
		}
	} catch (const char *errorString) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), errorString);
	} catch (...) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION),
				"Unexpected native call failure in method close!");
	}
}

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_equipment_io_usb_internal_USBNativeDevice_read(
		JNIEnv *env, jobject src, jbyteArray data, jint offset, jint length,
		jint timeout) {
	try {
		USBDevice *device = getUSBDevice(env, src);

		if (device) {
			jsize dataLength = env->GetArrayLength(data);
			if ((offset < 0) || ((offset + length) > dataLength)) {
				env->ThrowNew(env->FindClass(EX_ARRAY_INDEX_OUT_OF_BOUND),
						"Given offset and length exceeds read buffer space!");
			} else {
				jbyte *buf = (jbyte *) malloc(length * sizeof(jbyte));
				env->GetByteArrayRegion(data, offset, length, buf);

				int ret = device->bulkRead(env, (char *) buf, length, timeout);
				if (ret > 0) {
					env->SetByteArrayRegion(data, offset, ret, buf);
				}
				free(buf);
				return ret;
			}
		} else {
			Logger::getInstance()->error(LP, "Cannot find device!");
		}
	} catch (const char *errorString) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), errorString);
	} catch (...) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION),
				"Unexpected native call failure in method read!");
	}
	return UNDEF;
}

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_equipment_io_usb_internal_USBNativeDevice_write(
		JNIEnv *env, jobject src, jbyteArray data, jint offset, jint length,
		jint timeout) {
	try {
		USBDevice *device = getUSBDevice(env, src);

		if (device) {
			jsize dataLength = env->GetArrayLength(data);
			if ((offset < 0) || ((offset + length) > dataLength)) {
				std::stringstream ss;
				ss<<"offset:"<<offset<<"\tlength: "<<length<<"\tdataLength: "<<dataLength;
				Logger::getInstance()->error(LP, ss.str());
				env->ThrowNew(env->FindClass(EX_ARRAY_INDEX_OUT_OF_BOUND),
						"Given offset and length exceeds write buffer space!");
			} else {
				jbyte *buf = (jbyte *) malloc(length * sizeof(jbyte));
				env->GetByteArrayRegion(data, offset, length, buf);

				int ret = device->bulkWrite(env, (char *) buf, length, timeout);
				free(buf);
				return ret;
			}
		} else {
			Logger::getInstance()->error(LP, "Cannot find device!");
		}
	} catch (const char *errorString) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION), errorString);
	} catch (...) {
		env->ThrowNew(env->FindClass(EX_NATIVE_EXCEPTION),
				"Unexpected native call failure in method write!");
	}

	return UNDEF;
}
