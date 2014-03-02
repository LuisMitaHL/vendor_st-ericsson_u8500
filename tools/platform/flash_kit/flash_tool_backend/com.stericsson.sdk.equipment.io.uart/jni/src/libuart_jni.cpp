/*
 * libuart_jni.cpp
 *
 *  Created on: Dec 10, 2009
 *      Author: emicroh
 */

#include "util.h"
#include "COMPort.h"
#include "Logger.h"
#include "com_stericsson_sdk_equipment_io_uart_internal_SerialPortHandler.h"
#include "com_stericsson_sdk_equipment_io_uart_internal_SerialPort.h"
#include <string>

using namespace std;

list<COMPort *> comPorts;
COMPort * getCOMPort(JNIEnv * env, jobject jserialPortSrc) {
	jmethodID mgetId = env->GetMethodID(env->FindClass(CLASS_SERIAL_PORT),
			METHOD_SP_GET_IDENTIFIER, METHOD_SP_GET_IDENTIFIER_SIG);
	string comName = string(
			getStringBody(env,
					(jstring) env->CallObjectMethod(jserialPortSrc, mgetId)));
	list<COMPort *>::const_iterator it;

	for (it = comPorts.begin(); it != comPorts.end(); it++) {
		if ((*it)->getName() == comName) {
			return *it;
		}
	}
	return NULL;
}

jobject getObjectField(JNIEnv * env, jobject jconfiguration, jclass jconfClass,
		LPCTSTR fieldName, LPCTSTR fieldSignature) {
	return env->GetObjectField(jconfiguration,
			env->GetFieldID(jconfClass, fieldName, fieldSignature));
}

jint getEnumFieldValue(JNIEnv * env, jobject fieldObj, jclass fieldObjClass,
		LPCTSTR methodName, LPCTSTR methodSignature) {
	jmethodID methodID = env->GetMethodID(fieldObjClass, methodName,
			methodSignature);
	jint ret = env->CallIntMethod(fieldObj, methodID);
	return ret;
}

uart_config * getConfiguration(JNIEnv * env, jobject jconfiguration) {
	uart_config * config = new uart_config;
	if (jconfiguration == NULL) {
		throw "NULL configuration object cannot be set!";
	}

	jclass brClass = env->FindClass(CLASS_SPC_BAUD_RATE);
	jclass dbClass = env->FindClass(CLASS_SPC_DATA_BITS);
	jclass sbClass = env->FindClass(CLASS_SPC_STOP_BITS);
	jclass parClass = env->FindClass(CLASS_SPC_PARITY);
	jclass onoffClass = env->FindClass(CLASS_SPC_ON_OFF);
	jclass dtrCircClass = env->FindClass(CLASS_SPC_DTR_CIRCUIT);
	jclass rtsCircClass = env->FindClass(CLASS_SPC_RTS_CIRCUIT);

	jclass jconfClass = env->GetObjectClass(jconfiguration);

	jobject enumFieldObj;
	jint value;

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_BAUD_RATE, FN_SPC_BAUD_RATE_SIG);
	value = getEnumFieldValue(env, enumFieldObj, brClass, METHOD_SPC_BAUD_RATE,
			METHOD_SPC_BAUD_RATE_SIG);

	switch (value) {

	case BR_110:
		config->baudRate = BR_110;
		break;
	case BR_300:
		config->baudRate = BR_300;
		break;
	case BR_600:
		config->baudRate = BR_600;
		break;
	case BR_1200:
		config->baudRate = BR_1200;
		break;
	case BR_2400:
		config->baudRate = BR_2400;
		break;
	case BR_4800:
		config->baudRate = BR_4800;
		break;
	case BR_9600:
		config->baudRate = BR_9600;
		break;
	case BR_14400:
		config->baudRate = BR_14400;
		break;
	case BR_19200:
		config->baudRate = BR_19200;
		break;
	case BR_38400:
		config->baudRate = BR_38400;
		break;
	case BR_56000:
		config->baudRate = BR_56000;
		break;
	case BR_57600:
		config->baudRate = BR_57600;
		break;
	case BR_115200:
		config->baudRate = BR_115200;
		break;
	case BR_128000:
		config->baudRate = BR_128000;
		break;
	case BR_230400:
		config->baudRate = BR_230400;
		break;
	case BR_256000:
		config->baudRate = BR_256000;
		break;
	case BR_460800:
		config->baudRate = BR_460800;
		break;
	case BR_921600:
		config->baudRate = BR_921600;
		break;
	case BR_1152000:
		config->baudRate = BR_1152000;
		break;
	default:
		throw getMessage("Attempt to set unsupported value for baud rate '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_DATA_BITS, FN_SPC_DATA_BITS_SIG);
	value = getEnumFieldValue(env, enumFieldObj, dbClass, METHOD_SPC_DATA_BITS,
			METHOD_SPC_DATA_BITS_SIG);
	switch (value) {
	case DATA_BITS_5:
		config->dataBits = DATA_BITS_5;
		break;
	case DATA_BITS_6:
		config->dataBits = DATA_BITS_6;
		break;
	case DATA_BITS_7:
		config->dataBits = DATA_BITS_7;
		break;
	case DATA_BITS_8:
		config->dataBits = DATA_BITS_8;
		break;
	default:
		throw getMessage("Attempt to set unsupported value for data bits '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_STOP_BITS, FN_SPC_STOP_BITS_SIG);
	value = getEnumFieldValue(env, enumFieldObj, sbClass,
			METHOD_SPC_ENUM_ORDINAL, METHOD_SPC_ENUM_ORDINAL_SIG);
	switch (value) {
	case STOP_BITS_1:
		config->stopBits = STOP_BITS_1;
		break;
	case STOP_BITS_1_5:
		config->stopBits = STOP_BITS_1_5;
		break;
	case STOP_BITS_2:
		config->stopBits = STOP_BITS_2;
		break;
	default:
		throw getMessage("Attempt to set unsupported value for stop bits '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_PARITY, FN_SPC_PARITY_SIG);
	value = getEnumFieldValue(env, enumFieldObj, parClass,
			METHOD_SPC_ENUM_ORDINAL, METHOD_SPC_ENUM_ORDINAL_SIG);
	switch (value) {
	case PAR_NONE:
		config->parity = PAR_NONE;
		break;
	case PAR_EVEN:
		config->parity = PAR_EVEN;
		break;
	case PAR_ODD:
		config->parity = PAR_ODD;
		break;
	case PAR_MARK:
		config->parity = PAR_MARK;
		break;
	case PAR_SPACE:
		config->parity = PAR_SPACE;
		break;
	default:
		throw getMessage("Attempt to set unsupported value for parity '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_TIMEOUT, FN_SPC_TIMEOUT_SIG);
	value = getEnumFieldValue(env, enumFieldObj, onoffClass,
			METHOD_SPC_ENUM_ORDINAL, METHOD_SPC_ENUM_ORDINAL_SIG);
	switch (value) {
	case DEFAULT:
		config->timeout = DEFAULT;
		break;
	case ON:
		config->timeout = ON;
		break;
	case OFF:
		config->timeout = OFF;
		break;
	default:
		throw getMessage("Attempt to set unsupported value for timeout '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_XONOFF, FN_SPC_XONOFF_SIG);
	value = getEnumFieldValue(env, enumFieldObj, onoffClass,
			METHOD_SPC_ENUM_ORDINAL, METHOD_SPC_ENUM_ORDINAL_SIG);
	switch (value) {
	case DEFAULT:
		config->xOnOff = DEFAULT;
		break;
	case ON:
		config->xOnOff = ON;
		break;
	case OFF:
		config->xOnOff = OFF;
		break;
	default:
		throw getMessage("Attempt to set unsupported value for  XOnOff '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_DSR_HANDSHAKE, FN_SPC_DSR_HANDSHAKE_SIG);
	value = getEnumFieldValue(env, enumFieldObj, onoffClass,
			METHOD_SPC_ENUM_ORDINAL, METHOD_SPC_ENUM_ORDINAL_SIG);
	switch (value) {
	case DEFAULT:
		config->dsr_handshake = DEFAULT;
		break;
	case ON:
		config->dsr_handshake = ON;
		break;
	case OFF:
		config->dsr_handshake = OFF;
		break;
	default:
		throw getMessage(
				"Attempt to set unsupported value for DSR handshake '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_CTS_HANDSHAKE, FN_SPC_CTS_HANDSHAKE_SIG);
	value = getEnumFieldValue(env, enumFieldObj, onoffClass,
			METHOD_SPC_ENUM_ORDINAL, METHOD_SPC_ENUM_ORDINAL_SIG);
	switch (value) {
	case DEFAULT:
		config->cts_handshake = DEFAULT;
		break;
	case ON:
		config->cts_handshake = ON;
		break;
	case OFF:
		config->cts_handshake = OFF;
		break;
	default:
		throw getMessage(
				"Attempt to set unsupported value for CTS handshake '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_DTR_CIRCUIT, FN_SPC_DTR_CIRCUIT_SIG);
	value = getEnumFieldValue(env, enumFieldObj, dtrCircClass,
			METHOD_SPC_ENUM_ORDINAL, METHOD_SPC_ENUM_ORDINAL_SIG);
	switch (value) {
	case DTR_DEFAULT:
		config->dtr = DTR_DEFAULT;
		break;
	case DTR_ON:
		config->dtr = DTR_ON;
		break;
	case DTR_OFF:
		config->dtr = DTR_OFF;
		break;
	case DTR_HS:
		config->dtr = DTR_HS;
		break;
	default:
		throw getMessage(
				"Attempt to set unsupported value for DTR circuit '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass,
			FN_SPC_RTS_CIRCUIT, FN_SPC_RTS_CIRCUIT_SIG);
	value = getEnumFieldValue(env, enumFieldObj, rtsCircClass,
			METHOD_SPC_ENUM_ORDINAL, METHOD_SPC_ENUM_ORDINAL_SIG);
	switch (value) {
	case RTS_DEFAULT:
		config->rts = RTS_DEFAULT;
		break;
	case RTS_ON:
		config->rts = RTS_ON;
		break;
	case RTS_OFF:
		config->rts = RTS_OFF;
		break;
	case RTS_HS:
		config->rts = RTS_OFF;
		break;
	case RTS_TG:
		config->rts = RTS_TG;
		break;
	default:
		throw getMessage(
				"Attempt to set unsupported value for RTS circuit '%d'!",
				value);
	}

	enumFieldObj = getObjectField(env, jconfiguration, jconfClass, FN_SPC_DSR,
			FN_SPC_DSR_SIG);
	value = getEnumFieldValue(env, enumFieldObj, onoffClass,
			METHOD_SPC_ENUM_ORDINAL, METHOD_SPC_ENUM_ORDINAL_SIG);
	switch (value) {
	case DEFAULT:
		config->dsr = DEFAULT;
		break;
	case ON:
		config->dsr = ON;
		break;
	case OFF:
		config->dsr = OFF;
		break;
	default:
		throw getMessage("Attempt to set unsupported value for DSR '%d'!",
				value);
	}

	return config;
}

JNIEXPORT void JNICALL Java_com_stericsson_sdk_equipment_io_uart_internal_SerialPortHandler_initJNILayer(
		JNIEnv *env, jobject src, jint logLevel) {
	Logger::getInstance()->info(LP, "JNI Layer is initiated..");
	return;
}

JNIEXPORT jobjectArray JNICALL Java_com_stericsson_sdk_equipment_io_uart_internal_SerialPortHandler_getAvailablePorts(
		JNIEnv *env, jobject src) {
	LPCTSTR errorMsg = NULL;
	try {
		comPorts.clear();

		list<string> test;
		list<string> portNames;
		scanPorts(portNames);
		list<string>::const_iterator it;

		jclass spClass = env->FindClass(CLASS_SERIAL_PORT);
		jmethodID spConstructor = env->GetMethodID(spClass, METHOD_CONSTRUCTOR,
				METHOD_SP_CONSTRUCTOR_SIG);
		jobjectArray availablePorts = env->NewObjectArray(portNames.size(),
				spClass, NULL);
		int index = 0;
		for (it = portNames.begin(); it != portNames.end(); it++) {
			comPorts.push_back(new COMPort(*it));
			string prefix = "added device: ";
			Logger::getInstance()->debug(LP, prefix + *it);
			jobject serialPort = env->NewObject(spClass, spConstructor,
					newString(env, it->c_str()));
			env->SetObjectArrayElement(availablePorts, index++, serialPort);
		}
		return availablePorts;
	} catch (LPCTSTR errMsg) {
		errorMsg = errMsg;
		goto ErrorExit;
	} catch (...) {
		errorMsg =
				"Caught unexpected exeption in function 'getAvailablePorts'!";
		goto ErrorExit;
	}

	ErrorExit: env->ThrowNew(env->FindClass(EX_UART_EXCEPTION), errorMsg);
	return NULL; //avoid warning
}

JNIEXPORT void JNICALL Java_com_stericsson_sdk_equipment_io_uart_internal_SerialPort_openPort(
		JNIEnv *env, jobject src) {
	LPCTSTR errorMsg = NULL;
	try {
		getCOMPort(env, src)->open();
		return;
	} catch (LPCTSTR errMsg) {
		errorMsg = errMsg;
		goto ErrorExit;
	} catch (...) {
		errorMsg = "Caught unexpected exeption in function 'openPort'!";
		goto ErrorExit;
	}

	ErrorExit: env->ThrowNew(env->FindClass(EX_UART_EXCEPTION), errorMsg);
}

JNIEXPORT void JNICALL Java_com_stericsson_sdk_equipment_io_uart_internal_SerialPort_setPort(
		JNIEnv *env, jobject src, jobject configuration) {
	LPCTSTR errorMsg = NULL;
	try {
		uart_config * conf = getConfiguration(env, configuration);
		getCOMPort(env, src)->set(*conf);
		delete conf;
		return;
	} catch (LPCTSTR errMsg) {
		errorMsg = errMsg;
		goto ErrorExit;
	} catch (...) {
		errorMsg = "Caught unexpected exeption in function 'setPort'!";
		goto ErrorExit;
	}

	ErrorExit: env->ThrowNew(env->FindClass(EX_UART_EXCEPTION), errorMsg);
}

JNIEXPORT void JNICALL Java_com_stericsson_sdk_equipment_io_uart_internal_SerialPort_closePort(
		JNIEnv *env, jobject src) {
	LPCTSTR errorMsg = NULL;
	try {
		getCOMPort(env, src)->close();
		return;
	} catch (LPCTSTR errMsg) {
		errorMsg = errMsg;
		goto ErrorExit;
	} catch (...) {
		errorMsg = "Caught unexpected exeption in function 'closePort'!";
		goto ErrorExit;
	}

	ErrorExit: env->ThrowNew(env->FindClass(EX_UART_EXCEPTION), errorMsg);
}

JNIEXPORT jboolean JNICALL Java_com_stericsson_sdk_equipment_io_uart_internal_SerialPort_portOpened(
		JNIEnv *env, jobject src) {
	LPCTSTR errorMsg = NULL;
	try {
		return getCOMPort(env, src)->isOpened() ? JNI_TRUE : JNI_FALSE;
	} catch (LPCTSTR errMsg) {
		errorMsg = errMsg;
		goto ErrorExit;
	} catch (...) {
		errorMsg = "Caught unexpected exeption in function 'portOpened'!";
		goto ErrorExit;
	}

	ErrorExit: env->ThrowNew(env->FindClass(EX_UART_EXCEPTION), errorMsg);
	return false; //avoid warning
}

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_equipment_io_uart_internal_SerialPort_portRead(
		JNIEnv *env, jobject src, jbyteArray buf, jint offset, jint length) {
	LPCTSTR errorMsg = NULL;
	char * nbuf = NULL;
	try {
		if (sizeof(TCHAR) != sizeof(jbyte)) {
			throw "Unexpected cast problem between TCHAR and jbyte, sizes are different!";
		}
		if (env->GetArrayLength(buf) < (offset + length)) {
			env->ThrowNew(env->FindClass(EX_ARRAY_INDEX_OUT_OF_BOUND),
					"Offset and length exceeds limitation of reading buffer!");
		}
		COMPort * port = getCOMPort(env, src);
		nbuf = new char[length];
		env->GetByteArrayRegion(buf, offset, length, (jbyte *) nbuf);
		jint ret = port->read(nbuf, 0, length);
		env->SetByteArrayRegion(buf, offset, ret, (jbyte *) nbuf);
		delete[] nbuf;
		return ret;
	} catch (LPCTSTR errMsg) {
		errorMsg = errMsg;
		goto ErrorExit;
	} catch (...) {
		errorMsg = "Caught unexpected exeption in function 'portRead'!";
		goto ErrorExit;
	}

	ErrorExit: if (nbuf != NULL) {
		delete nbuf;
	}
	env->ThrowNew(env->FindClass(EX_UART_EXCEPTION), errorMsg);
	return 0; //avoid warning
}

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_equipment_io_uart_internal_SerialPort_portWrite(
		JNIEnv *env, jobject src, jbyteArray buf, jint offset, jint length) {
	LPCTSTR errorMsg = NULL;
	char * nbuf = NULL;
	try {
		if (sizeof(TCHAR) != sizeof(jbyte)) {
			throw "Unexpected cast problem between TCHAR and jbyte, sizes are different!";
		}
		if (env->GetArrayLength(buf) < (offset + length)) {
			env->ThrowNew(env->FindClass(EX_ARRAY_INDEX_OUT_OF_BOUND),
					"Offset and length exceeds limitation of writing buffer!");
		}
		COMPort * port = getCOMPort(env, src);
		nbuf = new char[length];
		env->GetByteArrayRegion(buf, offset, length, (jbyte *) nbuf);
		jint ret = port->write(nbuf, 0, length);
		//there was no change in the array, that's why next line is not needed
//		env->SetByteArrayRegion(buf, offset, ret, (jbyte *) nbuf);
		delete[] nbuf;
		return ret;
	} catch (LPCTSTR errMsg) {
		errorMsg = errMsg;
		goto ErrorExit;
	} catch (...) {
		errorMsg = "Caught unexpected exeption in function 'portWrite'!";
		goto ErrorExit;
	}

	ErrorExit: if (nbuf != NULL) {
		delete nbuf;
	}
	env->ThrowNew(env->FindClass(EX_UART_EXCEPTION), errorMsg);
	return 0; //avoid warning
}

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_equipment_io_uart_internal_SerialPort_portFlush(
		JNIEnv *env, jobject src) {
	string errorMsg;
	try {
		COMPort * port = getCOMPort(env, src);
		jint ret = port->flush();
		return ret;
	} catch (LPCTSTR errMsg) {
		errorMsg = errMsg;
		goto ErrorExit;
	} catch (...) {
		errorMsg = "Caught unexpected exception in function 'portFlush'!";
		goto ErrorExit;
	}

	ErrorExit: env->ThrowNew(env->FindClass(EX_UART_EXCEPTION),
			errorMsg.c_str());
	return 0; //avoid warning
}

