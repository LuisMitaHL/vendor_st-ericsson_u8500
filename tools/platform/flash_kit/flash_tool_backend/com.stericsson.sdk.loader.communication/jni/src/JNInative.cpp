// JNInative.cpp : Defines the entry point for the DLL application.
//
//#define JNIEXPORT//only for devel purposes. not compile with this
//#define JNICALL//only for devel purposes. not compile with this

#include "javadefs.h"
#include "com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface.h"
#include "LCDInterface.h"
#include <cstring>
#include <cstdlib>
#include "Logger.h"
#include <iostream>
#include <iomanip>

/**
 * \param Communication_p not used
 * \param MessageLength strlen of MessageText
 * \param MessageText message text, should be null-terminated already but it is defensively
 * null-terminated again
 *
 */
void message(void *Communication_p, int MessageLength, char* MessageText) {
	JNIEnv * jniEnv = NULL;
	globalJavaVM->AttachCurrentThread((void**) &jniEnv, NULL);

	MessageText[MessageLength - 1] = '\0'; //for sure

	Logger::getInstance()->debug(LP, MessageText);
}

// this function calls the static read function in JAVA
bool deviceRead(void *Data_p, unsigned long Length,
		CommunicationCallback_t callback_fn, void *Param_p) {
	JNIEnv * jniEnv = NULL;
	globalJavaVM->AttachCurrentThread((void**) &jniEnv, NULL);

	jclass cls = jniEnv->GetObjectClass(globalObject);

	if (communicationReadCallback_fn == NULL) {
		communicationReadCallback_fn = callback_fn;
	}

	int64_t temp = (uintptr_t) Data_p;
	int64_t temp2 = (uintptr_t) Param_p;

	jniEnv->CallStaticVoidMethod(cls, globalMidRead, (int) Length, temp, temp2);

	return 0;
}

//this function calls the static write function in JAVA
int deviceWrite(void *Data_p, unsigned long Length,
		CommunicationCallback_t callback_fn, void *Param_p) {
	JNIEnv * jniEnv = NULL;
	globalJavaVM->AttachCurrentThread((void**) &jniEnv, NULL);

	jclass cls = jniEnv->GetObjectClass(globalObject);

	if (communicationWriteCallback_fn == NULL) {
		communicationWriteCallback_fn = callback_fn;
	}

	jbyteArray byteArray = jniEnv->NewByteArray((int) Length);
	if (byteArray == NULL) {
		return 0;
	}

	jniEnv->SetByteArrayRegion(byteArray, 0, (int) Length, (jbyte*) Data_p);

	int64_t temp = (uintptr_t) Data_p;
	int64_t temp2 = (uintptr_t) Param_p;

	jniEnv->CallStaticVoidMethod(cls, globalMidWrite, byteArray, (int) Length,
			temp, temp2);

	// explicit delete
	if (byteArray != NULL) {
		jniEnv->DeleteLocalRef(byteArray);
	}

	return 0;
}

//this function calls the static progress function in JAVA
void progress(void *Communication_p, int64_t TotalLength,
		int64_t TransferredLength) {
	JNIEnv * jniEnv = NULL;
	globalJavaVM->AttachCurrentThread((void**) &jniEnv, NULL);

	jclass cls = jniEnv->GetObjectClass(globalObject);

	int64_t temp = (uintptr_t) Communication_p;
	jniEnv->CallStaticVoidMethod(cls, globalMidProgress, temp, TotalLength,
			TransferredLength);

	globalJavaVM->DetachCurrentThread();
}

// this function calls the static cancel function in JAVA
int deviceCancel(void *Param_p) {
	JNIEnv * jniEnv = NULL;
	globalJavaVM->AttachCurrentThread((void**) &jniEnv, NULL);

	jclass cls = jniEnv->GetObjectClass(globalObject);

	uint64_t temp = (uintptr_t) Param_p;
	jniEnv->CallStaticVoidMethod(cls, globalMidCancel, temp);

	globalJavaVM->DetachCurrentThread();
 	return 0;
}

JNIEXPORT void JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_setLCDPath(
		JNIEnv *env, jobject obj, jstring libPath) {

#ifdef _TEST
	return;
#endif

	const char* cLibPath = env->GetStringUTFChars(libPath, 0);
	//LCDLibPath = (char*) malloc(sizeof(char)*strlen(cLibPath));
	//strcpy(LCDLibPath, cLibPath);
	LCDLibPath = cLibPath;
	env->ReleaseStringUTFChars(libPath, cLibPath);
	return;
}

JNIEXPORT void JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_setLCMPath(
		JNIEnv *env, jobject obj, jstring libPath) {

#ifdef _TEST
	return;
#endif

	const char* cLibPath = env->GetStringUTFChars(libPath, 0);
	//		LCMLibPath = (char*) malloc(sizeof(char)*strlen(cLibPath));
	//		strcpy(LCMLibPath, cLibPath);
	LCMLibPath = cLibPath;
	env->ReleaseStringUTFChars(libPath, cLibPath);
	return;
}

JNIEXPORT jstring JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_getLoaderErrorDescription
(JNIEnv *env, jobject obj , jint error) {

	//void* LCDObject = (void*) instanceMap[instance_p];
	char* result=NULL;
 	char* shortdesc = (char*)malloc(128);
	char* longdesc = (char*)malloc(256);
	
	LCD_GetLoaderErrorDescription_fn(error,(char *)shortdesc,(char *)longdesc,128,256);

      result = (char*)malloc(strlen(shortdesc) + strlen(longdesc) + 1);
      if ( result != NULL )
      {
         strcpy(result, shortdesc);
         strcat(result, longdesc);
         puts(result);
         
      }
  
   	jstring jstrBuf = (*env).NewStringUTF(result);
    free(result);
    free(shortdesc);
    free(longdesc);
	return jstrBuf;
}

// initialize the LCD
// this function will set up the LCD with function pointers to READ, WRITE, CANCEL 
// callbacks
JNIEXPORT jlong JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_initialize(
		JNIEnv *env, jobject obj) {

#ifdef _TEST
	return 1;
#endif
	globalExceptionClass = env->FindClass(EX_EXCEPTION);

	Logger::getInstance()->info(LP, "Initialize Loader Communication JNI");
	// init LCM and register callback

	if (!initialized) {
		//setup function pointers to LCM
		deviceReadFunction_p = (void*) (deviceRead);
		deviceWriteFunction_p = (void*) (deviceWrite);
		deviceCancelFunction_p = (void*) (deviceCancel);
		progressFunction_p = (void*) (progress);
		messageFunction_p = (void*) (message);
		//setup reference to JVM
		env->GetJavaVM(&globalJavaVM);

		jclass localClass = env->GetObjectClass(obj); //Class

		//void read(int length, int dataPointer, int instancePointer)
		if (!(globalMidRead = env->GetStaticMethodID(localClass,
				SMETHOD_LCI_READ, SMETHOD_LCI_READ_SIG))) {
			env->ThrowNew(globalExceptionClass,
					formatMessage(
							"Could not find the method '%s' with signture: '%s'",
							SMETHOD_LCI_READ, SMETHOD_LCI_READ_SIG));
			return -1;
		}

		//void write(byte[] data, int length, int dataPointer, int instancePointer)
		if (!(globalMidWrite = env->GetStaticMethodID(localClass,
				SMETHOD_LCI_WRITE, SMETHOD_LCI_WRITE_SIG))) {
			env->ThrowNew(globalExceptionClass,
					formatMessage(
							"Could not find the method '%s' with signture: '%s'",
							SMETHOD_LCI_WRITE, SMETHOD_LCI_WRITE_SIG));
			return -1;
		}

		//void cancel(int instance_p)
		if (!(globalMidCancel = env->GetStaticMethodID(localClass,
				SMETHOD_LCI_CANCEL, SMETHOD_LCI_CANCEL_SIG))) {
			env->ThrowNew(globalExceptionClass,
					formatMessage(
							"Could not find the method '%s' with signture: '%s'",
							SMETHOD_LCI_CANCEL, SMETHOD_LCI_CANCEL_SIG));
//			env->ThrowNew(globalExceptionClass, "Could not find the method 'cancel'");
			return -1;
		}

		//void progress(int instancePointer, long totalBytes, long transferredBytes)
		if (!(globalMidProgress = env->GetStaticMethodID(localClass,
				SMETHOD_LCI_PROGRESS, SMETHOD_LCI_PROGRESS_SIG))) {
			env->ThrowNew(globalExceptionClass,
					formatMessage(
							"Could not find the method '%s' with signture: '%s'",
							SMETHOD_LCI_PROGRESS, SMETHOD_LCI_PROGRESS_SIG));
			return -1;
		}

		//void message(long length, String message)
//		if (!(globalMidMessage = env->GetStaticMethodID(localClass, SMETHOD_LCI_MESSAGE, SMETHOD_LCI_MESSAGE_SIG))) {
//			env->ThrowNew(globalExceptionClass, formatMessage("Could not find the method '%s' with signture: '%s'",
//					SMETHOD_LCI_MESSAGE, SMETHOD_LCI_MESSAGE_SIG));
//
//		}

		globalObject = env->NewGlobalRef(obj);

		initialized = loadFunctions(env);

		if (!initialized)
			return -1;
	}

	void *LCDObject = NULL;

	sprintf(interfaceId, "%i", counter++);

	void* instance = NULL;
	//void* messageNone = NULL;

	int result = -1;
	try {
		result = LCD_Initialize_fn(&LCDObject, interfaceId,
				deviceReadFunction_p, deviceWriteFunction_p,
				deviceCancelFunction_p, &instance, messageFunction_p,
				const_cast<char*>(LCMLibPath.c_str()), progressFunction_p);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in LCDInitialize: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in LCDInitialize");
	}
	// map the instance and LCDObject
	if (result == 0) {
		instanceMap[(int64_t) (uintptr_t) instance] = LCDObject;
	} else {
		env->ThrowNew(globalExceptionClass, "Initialization failed");
		return -1;
	}
	return (int64_t) (uintptr_t) instance;

}

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_deInitialize(
		JNIEnv *env, jobject obj, jlong instance_p) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];
	int result;

	result = LCD_RemoveInstance_fn(LCDObject);
	if (result != 0) {
		env->ThrowNew(globalExceptionClass, "Failed to deinitialize");
		return result;
	}
	instanceMap.erase(instance_p);
//	free(LCDLibPath);
//	free(LCMLibPath);

	return result;
}

// this will call the callback function received by the WRITE callback, since the JNI cannot
// send function pointers to JAVA
JNIEXPORT void JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_readReply(
		JNIEnv *env, jobject obj, jbyteArray data, jlong length, jlong data_p,
		jlong instance_p) {

#ifdef _TEST
	return;
#endif
	//fill the data
	jbyte * arr_p;
	arr_p = env->GetByteArrayElements(data, NULL);

	if (arr_p == NULL) {
		Logger::getInstance()->error(LP, "out of memory thrown: in readReply");
		return;
	}

	//copyArray((char*) (uintptr_t) data_p, (char*) arr_p, length);
	memcpy((char*) (uintptr_t) data_p, (char*) arr_p, length);

	communicationReadCallback_fn((void*) (uintptr_t) data_p, (long) length,
			(void*) (uintptr_t) instance_p);

	env->ReleaseByteArrayElements(data, arr_p, 0);

#ifdef _COMM_DEBUG
	debugLog(env, "readReply");
#endif

}

// this will call the callback function received by the READ callback, since the JNI cannot
// send function pointers to JAVA
JNIEXPORT void JNICALL JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_writeReply(
		JNIEnv *env, jobject obj, jlong length, jlong data_p,
		jlong instance_p) {

#ifdef _TEST
	return;
#endif

	try {
		communicationWriteCallback_fn((void*) (uintptr_t) data_p, (long) length,
				(void*) (uintptr_t) instance_p);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in writeReply: ") + errmess);
	} catch (...) {
		Logger::getInstance()->debug(LP, "Exception in writeReply");
		return; //TODO: change return value and stop execution with LCM error message
	}
#ifdef _COMM_DEBUG
	debugLog(env, "writeReply");
#endif
	// communicationCallbackFunction_p(const void *Data_p, const unsigned long Length, void *Param_p);
}

//----------------------------
// SYSTEM 0x01
//----------------------------

// LOADER STARTUP STATUS 0x01
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemLoaderStartupStatus(
		JNIEnv *env, jobject obj, jlong instance_p) {

	jclass loaderStartupTypeClass;
	jmethodID construcorId;
	jobject loaderStartupObject;
	int result = 0;

	if (!(loaderStartupTypeClass = env->FindClass(CLS_LOADER_STARTUP_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'",
						CLS_LOADER_STARTUP_TYPE));
		return NULL;
	}

	if (!(construcorId = env->GetMethodID(loaderStartupTypeClass,
			METHOD_CONSTRUCTOR, METHOD_CONST_LOADER_STARTUP_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor of class '%s' with the signature '%s'",
						CLS_LOADER_STARTUP_TYPE,
						METHOD_CONST_LOADER_STARTUP_TYPE_SIG));
		return NULL;
	}

	//get LCDObject pointer
	void* LCDObject = (void*) instanceMap[instance_p];

	int sizeVersion = VERSION_BUFF_SIZE - 1;
	char* version = new char[VERSION_BUFF_SIZE]; //alloc before knowing the size for the LCD
	int sizeProtocol = PROTOCOL_BUFF_SIZE - 1;
	char* protocol = new char[PROTOCOL_BUFF_SIZE]; //alloc before knowing the size for the LCD

	try {

#ifdef _TEST
		loaderStartupObject = env->NewObject(loaderStartupTypeClass, construcorId, 0, env->NewStringUTF("V"), 1, env->NewStringUTF("P"), 1);
		return loaderStartupObject;
#endif

		// DEBUG
		// versionSize is a strlen() value of Version string
		// protocolSize is a strlen() value of Protocol string
		result = System_LoaderStartupStatus_fn(LCDObject, version, &sizeVersion,
				protocol, &sizeProtocol);
		//if(result != 0) how can i handle this result?

	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SystemLoaderStartupStatus: ")
						+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP,
				"Exception in SystemLoaderStartupStatus");
	}

	version[sizeVersion] = '\0';

	jstring loaderVersion = env->NewStringUTF(version);

	if (loaderVersion == NULL) {
//		errorLog(env, "out of memory thrown: in loaderStartupStatus");
		return NULL;
	}

	protocol[sizeProtocol] = '\0';

	jstring loaderProtocol = env->NewStringUTF(protocol);

	if (loaderProtocol == NULL) {
//		errorLog(env, "out of memory thrown: in loaderStartupStatus");
		return NULL;
	}

	// DEBUG
	loaderStartupObject = env->NewObject(loaderStartupTypeClass, construcorId,
			result, loaderVersion, sizeVersion, loaderProtocol, sizeProtocol);

	// release resources
	delete[] version;
	delete[] protocol;

	return loaderStartupObject;
}

// CHANGE BAUDRATE 0x02
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemChangeBaudRate(
		JNIEnv *env, jobject, jlong instance_p, jint baudrate) {
#ifdef _TEST
	return 0;
#endif
	int result = 0;

	void* LCDObject = (void*) instanceMap[instance_p];

	try {
		result = System_ChangeBaudrate_fn(LCDObject, (int) baudrate);

	} catch (...) {

		char msg[] = "Exception in SystemChangeBaudrate";
		message(NULL, strlen(msg), msg);
	}
	return result;

}

// REBOOT 0x03
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemReboot(
		JNIEnv *env, jobject, jlong instance_p, jint mode) {
#ifdef _TEST
	return 0;
#endif
	int result = 0;
	void* LCDObject = (void*) instanceMap[instance_p];

	try {
		result = System_Reboot_fn(LCDObject, (int) mode);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SystemReboot: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SystemReboot");
	}

	return result;
}

// SHUTDOWN 0x04
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemShutdown(
		JNIEnv * env, jobject obj, jlong instance_p) {
#ifdef _TEST
	return 0;
#endif
	int result = 0;
	void* LCDObject = (void*) instanceMap[instance_p];

	try {
		result = System_Shutdown_fn(LCDObject);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SystemShutdown: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SystemShutdown");
	}

	return result;
}

// SUPPORTED COMMANDS 0x05
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemSupportedCommands(
		JNIEnv *env, jobject obj, jlong instance_p) {

	jclass suppportedCommandsTypeClass;
	jmethodID construcorId;
	jmethodID memberId;
	jobject supportedCommandsTypeObject;
	TSupportedCmd* cSupportedCommands = new TSupportedCmd[100];
	int cSupportedCommandsSize = 100;

	void* LCDObject = (void*) instanceMap[instance_p];

	if (!(suppportedCommandsTypeClass = env->FindClass(CLS_SUPPORTED_CMDS_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'",
						CLS_SUPPORTED_CMDS_TYPE));
	}

	if (!(construcorId = env->GetMethodID(suppportedCommandsTypeClass,
			METHOD_CONSTRUCTOR, METHOD_CONST_SUPPORTED_CMDS_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor of class '%s' with the signature '%s'.",
						CLS_SUPPORTED_CMDS_TYPE,
						METHOD_CONST_SUPPORTED_CMDS_TYPE_SIG));
	}

	//method: addSupportedCommand(int group, int command, int permitted)
	if (!(memberId = env->GetMethodID(suppportedCommandsTypeClass,
			METHOD_SCMDST_ADD_SUPPORTED_CMD,
			METHOD_SCMDST_ADD_SUPPORTED_CMD_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the method '%s' in class '%s'.",
						METHOD_SCMDST_ADD_SUPPORTED_CMD,
						CLS_SUPPORTED_CMDS_TYPE));
	}

	try {

#ifdef _TEST
		supportedCommandsTypeObject = env->NewObject(suppportedCommandsTypeClass, construcorId, 0, 0);
		return supportedCommandsTypeObject;
#endif

		int result = System_SupportedCommands_fn(LCDObject, cSupportedCommands,
				&cSupportedCommandsSize);
		supportedCommandsTypeObject = env->NewObject(
				suppportedCommandsTypeClass, construcorId, result,
				&cSupportedCommandsSize);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SystemSupportedCommands: ")
						+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP,
				"Exception in SystemSupportedCommands");
	}

	for (int i = 0; i < cSupportedCommandsSize; i++) {
		//fill the java object 
		env->CallVoidMethod(supportedCommandsTypeObject, memberId,
				cSupportedCommands[i].Command, cSupportedCommands[i].Group,
				cSupportedCommands[i].Permitted);
	}

	delete[] cSupportedCommands;

	return supportedCommandsTypeObject;
}

// EXECUTE SOFTWARE 0x06
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemExecuteSoftware(
		JNIEnv * env, jobject obj, jlong instance_p, jstring path,
		jboolean useBulk) {
#ifdef _TEST
	return 0;
#endif
	//get LCDObject pointer
	void* LCDObject = (void*) instanceMap[instance_p];
	const char* cpath = env->GetStringUTFChars(path, 0);
	int result = 0;

	try {
		// DEBUG
		result = System_ExecuteSoftware_fn(LCDObject, cpath, (int) useBulk);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SystemExecuteSoftware: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SystemExecuteSoftware");
	}

	//release
	env->ReleaseStringUTFChars(path, cpath);
	return result;
}

// System Deauthenticate  0x11
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemDeauthenticate(
		JNIEnv *env, jobject, jlong instance_p, jint permanentFlag) {
#ifdef _TEST
	return 0;
#endif
	int result = 0;

	void* LCDObject = (void*) instanceMap[instance_p];

	try {
		result = System_Deauthenticate_fn(LCDObject, (int) permanentFlag);

	} catch (...) {

		char msg[] = "Exception in SystemDeauthenticate";
		message(NULL, strlen(msg), msg);
	}
	return result;

}

// AUTHENTICATE 0x07
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemAuthenticate(
		JNIEnv *env, jobject obj, jlong instance_p, jint type) {
	jclass authenticateTypeClass;
	jmethodID construcorId;
	jobject authenticateTypeObject;

	unsigned char* response = new unsigned char[RESPONSE_BUFF_SIZE];
	int size = RESPONSE_BUFF_SIZE - 1;

	if (!(authenticateTypeClass = env->FindClass(CLS_AUTHENTICATION_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'",
						CLS_AUTHENTICATION_TYPE));
		return NULL;
	}

	void* LCDObject = (void*) instanceMap[instance_p];

#ifdef _TEST
	if(type == 0) {
		construcorId = env->GetMethodID(authenticateTypeClass, METHOD_CONSTRUCTOR, METHOD_CONST_AUTHENTICATION_TYPE_CONTROL_KEYS_SIG);
		authenticateTypeObject = env->NewObject(authenticateTypeClass, construcorId, 0);
		return authenticateTypeObject;
	} else {
		construcorId = env->GetMethodID(authenticateTypeClass, METHOD_CONSTRUCTOR, METHOD_CONST_AUTHENTICATION_TYPE_CHALLENGE_SIG);
		jbyteArray jChallengeBuffer = env->NewByteArray(1);
		authenticateTypeObject = env->NewObject(authenticateTypeClass, construcorId, 0, 1, jChallengeBuffer);
		return authenticateTypeObject;
	}
#endif
	int result = -1;
	try {
		// DEBUG
		result = System_Authenticate_fn(LCDObject, (int) type, &size, response);
		//how can i handle this result?

	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SystemAuthenticate: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SystemAuthenticate");
	}

	if ((type == 1) || (type == 4)) { // challenge
		if (!(construcorId = env->GetMethodID(authenticateTypeClass,
				METHOD_CONSTRUCTOR,
				METHOD_CONST_AUTHENTICATION_TYPE_CHALLENGE_SIG))) {
			env->ThrowNew(globalExceptionClass,
					formatMessage(
							"Could not find the constructor for class '%s' with the signature '%s'",
							CLS_AUTHENTICATION_TYPE,
							METHOD_CONST_AUTHENTICATION_TYPE_CHALLENGE_SIG));
			return NULL;
		}

		jbyteArray jChallengeBuffer = env->NewByteArray(size);

		env->SetByteArrayRegion(jChallengeBuffer, 0, size, (jbyte*) response);
		// call constructor
		authenticateTypeObject = env->NewObject(authenticateTypeClass,
				construcorId, result, size, jChallengeBuffer);
	} else {
		if (!(construcorId = env->GetMethodID(authenticateTypeClass,
				METHOD_CONSTRUCTOR,
				METHOD_CONST_AUTHENTICATION_TYPE_CONTROL_KEYS_SIG))) {
			env->ThrowNew(globalExceptionClass,
					formatMessage(
							"Could not find the constructor for class '%s' with the signature '%s'",
							CLS_AUTHENTICATION_TYPE,
							METHOD_CONST_AUTHENTICATION_TYPE_CONTROL_KEYS_SIG));
			return NULL;
		}
		// call constructor
		authenticateTypeObject = env->NewObject(authenticateTypeClass,
				construcorId, result);
	}

	delete[] response;
	return authenticateTypeObject;
}

#define FULL_SIZE_OF_KEY 17
#define SIZE_OF_KEY 16
#define SIZE_OF_BUFFER 85

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemGetControlKeys(
		JNIEnv *env, jobject obj, jlong instance_p, jobjectArray keys) {
#ifdef _TEST
	return 0;
#endif
	//SIZE_OF_BUFFER = 85;//(16 + 1) * 5; //size of key is 16bytes + 1byte zero
	unsigned char* buffer = NULL;
	buffer = (unsigned char*) malloc(sizeof(char) * SIZE_OF_BUFFER);

	if (buffer == NULL) {
		Logger::getInstance()->error(LP,
				"Out of memory. Unable allocate space for buffer of keys.");
		return -1;
	}

	unsigned char* pNLock = buffer;
	unsigned char* pNSLock = buffer + FULL_SIZE_OF_KEY;
	unsigned char* pSPLock = pNSLock + FULL_SIZE_OF_KEY;
	unsigned char* pCLock = pSPLock + FULL_SIZE_OF_KEY;
	unsigned char* pESLock = pCLock + FULL_SIZE_OF_KEY;

	void* LCDObject = (void*) instanceMap[instance_p];
	std::stringstream ss;
	ss << "Instance pointer: 0x" << std::setbase(16) << instance_p;
	Logger::getInstance()->debug(LP, ss.str());

	//Copy NLCKLock parameter.
	jstring NLCKLockKey = (jstring) env->GetObjectArrayElement(keys, 0);
	const char* cNLCKLockKey = env->GetStringUTFChars(NLCKLockKey, 0);
	int pNLockSize = strlen(cNLCKLockKey);
	ss.str("Network Lock Key: ");
	ss << cNLCKLockKey << std::setbase(16) << cNLCKLockKey[pNLockSize];
	Logger::getInstance()->debug(LP, ss.str());

	//Copy NSLCKLock parameter.
	jstring NSLCKLockKey = (jstring) env->GetObjectArrayElement(keys, 1);
	const char* cNSLCKLockKey = env->GetStringUTFChars(NSLCKLockKey, 0);
	int pNSLockSize = strlen(cNSLCKLockKey);
	ss.str("Network Subset Lock Key: ");
	ss << cNSLCKLockKey << std::setbase(16) << cNSLCKLockKey[pNSLockSize];
	Logger::getInstance()->debug(LP, ss.str());

	//Copy SPLCKLock parameter.
	jstring SPLCKLockKey = (jstring) env->GetObjectArrayElement(keys, 2);
	const char* cSPLCKLockKey = env->GetStringUTFChars(SPLCKLockKey, 0);
	int pSPLockSize = strlen(cSPLCKLockKey);
	ss.str("Service Provider Lock Key: ");
	ss << cSPLCKLockKey << std::setbase(16) << cSPLCKLockKey[pNLockSize];
	Logger::getInstance()->debug(LP, ss.str());

	//Copy CLCKLock parameter.
	jstring CLCKLockKey = (jstring) env->GetObjectArrayElement(keys, 3);
	const char* cCLCKLockKey = env->GetStringUTFChars(CLCKLockKey, 0);
	int pCLockSize = strlen(cCLCKLockKey);
	ss.str("Corporate Lock Key: ");
	ss << cCLCKLockKey << std::setbase(16) << cCLCKLockKey[pNLockSize];
	Logger::getInstance()->debug(LP, ss.str());

	//Copy ESLCKLock parameter.
	jstring ESLCKLockKey = (jstring) env->GetObjectArrayElement(keys, 4);
	const char* cESLCKLockKey = env->GetStringUTFChars(ESLCKLockKey, 0);
	int pESLockSize = strlen(cESLCKLockKey);
	ss.str("Flexible ESL Lock Key: ");
	ss << cESLCKLockKey << std::setbase(16) << cESLCKLockKey[pNLockSize];
	Logger::getInstance()->debug(LP, ss.str());

	//build buffer of keys
	memset(buffer, 0x00, SIZE_OF_BUFFER);
	memcpy(pNLock, cNLCKLockKey, pNLockSize);
	memcpy(pNSLock, cNSLCKLockKey, pNSLockSize);
	memcpy(pSPLock, cSPLCKLockKey, pSPLockSize);
	memcpy(pCLock, cCLCKLockKey, pCLockSize);
	memcpy(pESLock, cESLCKLockKey, pESLockSize);
	//  set the zero flag immediately after character data
	//	memcpy(pNLock, cNLCKLockKey, pNLockSize+1);
	//	memcpy(pNSLock, cNSLCKLockKey, pNSLockSize+1);
	//	memcpy(pSPLock, cSPLCKLockKey, pSPLockSize+1);
	//	memcpy(pCLock, cCLCKLockKey, pCLockSize+1);
	//	memcpy(pESLock, cESLCKLockKey, pESLockSize+1);
	//set a zero flag of string into the end of key area
	memset(pNLock + SIZE_OF_KEY, 0x00, 1);
	memset(pNSLock + SIZE_OF_KEY, 0x00, 1);
	memset(pSPLock + SIZE_OF_KEY, 0x00, 1);
	memset(pCLock + SIZE_OF_KEY, 0x00, 1);
	memset(pESLock + SIZE_OF_KEY, 0x00, 1);

	debugBuffer((unsigned char*) buffer, SIZE_OF_BUFFER, FULL_SIZE_OF_KEY);

	int result = 0;
	try {
		Logger::getInstance()->info(LP, "System in SystemGetControlKeys");
		result = System_GetControlKeysData_fn(LCDObject, SIZE_OF_BUFFER,
				buffer);
		ss.str("System in SystemGetControlKeys Result: ");
		ss << result;
		Logger::getInstance()->info(LP, ss.str());
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SystemGetControlKeys: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SystemGetControlKeys");
	}

	env->ReleaseStringUTFChars(NLCKLockKey, cNLCKLockKey);
	env->ReleaseStringUTFChars(NSLCKLockKey, cNSLCKLockKey);
	env->ReleaseStringUTFChars(SPLCKLockKey, cSPLCKLockKey);
	env->ReleaseStringUTFChars(CLCKLockKey, cCLCKLockKey);
	env->ReleaseStringUTFChars(ESLCKLockKey, cESLCKLockKey);
	free(buffer);
	return result;
}

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemAuthenticationChallenge(
		JNIEnv *env, jobject obj, jlong instance_p, jint length,
		jbyteArray data) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	jbyte * data_p;
	data_p = env->GetByteArrayElements(data, 0);

	if (data_p == NULL) {
//		errorLog(env, "Out of memory thrown: in SystemAuthenticationChallenge");
		return -1;
	}

	int result = 0;
	try {
		result = System_AuthenticationChallenge_fn(LCDObject,
				(unsigned char*) data_p, length);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SystemAuthenticationChallenge: ")
						+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP,
				"Exception in SystemAuthenticationChallenge");
	}

	return result;
}

JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSystemCollectData(
		JNIEnv *env, jobject obj, jlong instance_p, jint type) {

	if (instanceMap.find(instance_p) == instanceMap.end()) {
		env->ThrowNew(globalExceptionClass, "instance not found");
		return NULL;
	}

	void* lcdObject = (void*) instanceMap[instance_p];

	// according to doc, result is 64K at maximum
	const int maxLength = 64 * 1024 + 1;
	int length = maxLength - 1;
	// coverity prevent limits stack size to 32kB, single variable to 1024kB, buff must be created with new
	char * buff = new char[maxLength];

	int result = -1;
	try {
		result = System_CollectData_fn(lcdObject, (int) type, &length, buff);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SystemCollectData: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SystemCollectData");
	}

	jbyteArray collectedData = NULL;
	if (result == 0) { // success, we have reasonable data in buff
		if ((length < 0) || (length >= maxLength)) { // bad length received
			env->ThrowNew(globalExceptionClass,
					"value of length is out of range");
			delete[] buff;
			return NULL;
		}

		collectedData = env->NewByteArray(length);
		if (!collectedData) {
			delete[] buff;
			return NULL; // exception in java has already been thrown
		}

		jbyte * collectedDataBytes;
		collectedDataBytes = env->GetByteArrayElements(collectedData, 0);
		if (!collectedDataBytes) {
			delete[] buff;
			return NULL; // exception in java has already been thrown
		}

		for (int i = 0; i < length; i++)
			collectedDataBytes[i] = (jbyte) buff[i];

		env->ReleaseByteArrayElements(collectedData, collectedDataBytes, 0); // does not return, does not throw
	}
	delete[] buff;

	jclass systemCollectClass;
	jmethodID construcorId;
	jobject systemCollectObject;

	if (!(systemCollectClass = env->FindClass(CLS_SYSTEM_COLLECT_DATA_TYPE)))
		return NULL; // exception in java has already been thrown

	if (!(construcorId = env->GetMethodID(systemCollectClass,
			METHOD_CONSTRUCTOR, METHOD_CONST_SYSTEM_COLLECT_DATA_TYPE_SIG)))
		return NULL; // exception in java has already been thrown

	if (!(systemCollectObject = env->NewObject(systemCollectClass, construcorId,
			result, collectedData)))
		return NULL; // exception in java has already been thrown

	return systemCollectObject;
}

// INIT ARB TABLE 0x0?
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityInitArbTable(
		JNIEnv *env, jobject obj, jlong instance_p, jint iType, jint iLength,
		jbyteArray puarbdata) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	//fill the data
	jbyte * data_p;
	data_p = env->GetByteArrayElements(puarbdata, 0);

	if (data_p == NULL) {
//		errorLog(env, "Out of memory thrown: in SystemInitArbTable");
		return -1;
	}

	int result = -1;
	try {
		result = Security_InitARBTable_fn(LCDObject, iType, iLength,
				(unsigned char*) data_p);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in SecurityInitArbTable: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SecurityInitArbTable");
	}

	env->ReleaseByteArrayElements(puarbdata, data_p, 0);

	return result;
}

//----------------------------
// FLASH 0x02
//----------------------------

//PROCESS FILE 0x01
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashProcessFile(
		JNIEnv *env, jobject obj, jlong instance_p, jstring path, jstring type,
		jboolean useBulk) {
#ifdef _TEST
	return 0;
#endif
	//get LCDObject pointer
	void* LCDObject = (void*) instanceMap[instance_p];
	const char* cPath = env->GetStringUTFChars(path, 0);
	const char* cType = env->GetStringUTFChars(type, 0);
	int result = 0;

	try {
		//DEBUG
		result = Flash_ProcessFile_fn(LCDObject, cPath, cType, (int) useBulk,
				true);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FlashProcessFile: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FlashProcessFile");
	}

	// release
	env->ReleaseStringUTFChars(path, cPath);
	env->ReleaseStringUTFChars(type, cType);

	return result;
}

// LIST DEVICES 0x02
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashListDevices(
		JNIEnv *env, jobject obj, jlong instance_p) {

	jclass listDeviceTypeClass;
	jmethodID construcorId;
	jmethodID memberId;
	jobject listDeviceTypeObject;
	TDevices* devices = new TDevices[100]; // ???
	int devicesSize = 100;

	void* LCDObject = (void*) instanceMap[instance_p];

	if (!(listDeviceTypeClass = env->FindClass(CLS_LIST_DEVICES_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'",
						CLS_LIST_DEVICES_TYPE));
		return NULL;
	}

	//constructor: ListDevicesType(int status, int deviceCount)
	if (!(construcorId = env->GetMethodID(listDeviceTypeClass,
			METHOD_CONSTRUCTOR, METHOD_CONST_DEVICES_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor of class '%s' with the signature '%s'.",
						CLS_LIST_DEVICES_TYPE, METHOD_CONST_DEVICES_TYPE_SIG));
		return NULL;
	}

	if (!(memberId = env->GetMethodID(listDeviceTypeClass,
			METHOD_LDEVT_ADD_DEVICE, METHOD_LDEVT_ADD_DEVICE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the method '%s'.",
						METHOD_LDEVT_ADD_DEVICE));
		return NULL;
	}

	int result = 0;

	try {
#ifdef _TEST
		listDeviceTypeObject = env->NewObject(listDeviceTypeClass, construcorId, 0, 0);
		return listDeviceTypeObject;
#endif		
		// (void *Object, TDevices* Devices, int* DeviceSize);
		result = Flash_ListDevices_fn(LCDObject, devices, &devicesSize);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FlashListDevices: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FlashListDevices");
	}

	if (result != 0) {
		listDeviceTypeObject = env->NewObject(listDeviceTypeClass, construcorId,
				result, 0);
		return listDeviceTypeObject;
	}
	// call constructor
	listDeviceTypeObject = env->NewObject(listDeviceTypeClass, construcorId,
			result, devicesSize);

	for (int i = 0; i < devicesSize; i++) {
		devices[i].Path[devices[i].PathSize] = '\0';
		devices[i].Type[devices[i].TypeSize] = '\0';

		jstring path = env->NewStringUTF(devices[i].Path);
		jstring type = env->NewStringUTF(devices[i].Type);

		// call member method
		env->CallVoidMethod(listDeviceTypeObject, memberId, path,
				devices[i].PathSize, type, devices[i].TypeSize,
				devices[i].BlockSize, devices[i].Start, devices[i].Length);
	}

	delete[] devices;

	return listDeviceTypeObject;
}

// DUMP AREA 0x03
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashDumpArea(
		JNIEnv *env, jobject obj, jlong instance_p, jstring sourcePath,
		jlong start, jlong length, jstring destPath, jint extraDumpParameters,
		jboolean useBulk) {

#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];
	int result = 0;
	const char* cSrcPath = env->GetStringUTFChars(sourcePath, 0);
	const char* cDestPath = env->GetStringUTFChars(destPath, 0);

	try {
		result = Flash_DumpArea_fn(LCDObject, cSrcPath, start, length,
				cDestPath, extraDumpParameters, (int) useBulk);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FlashDumpArea: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FlashDumpArea");
	}

	env->ReleaseStringUTFChars(sourcePath, cSrcPath);
	env->ReleaseStringUTFChars(destPath, cDestPath);

	return result;
}

// ERASE AREA 0x04
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashEraseArea(
		JNIEnv *env, jobject obj, jlong instance_p, jstring sourcePath,
		jlong start, jlong length) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];
	const char* cSrcPath = env->GetStringUTFChars(sourcePath, 0);
	int result = 0;

	// (void *Object,char* Path, int Start, int Length)
	try {
		result = Flash_EraseArea_fn(LCDObject, cSrcPath, start, length);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FlashEraseArea: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FlashEraseArea");
	}

	env->ReleaseStringUTFChars(sourcePath, cSrcPath);
	return result;
}

// SET ENHANCED AREA 0x05
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashSetEnhancedArea(
		JNIEnv *env, jobject obj, jlong instance_p, jstring sourcePath,
		jlong start, jlong length) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];
	const char* cSrcPath = env->GetStringUTFChars(sourcePath, 0);
	int result = 0;

	try {
		result = Flash_SetEnhancedArea_fn(LCDObject, cSrcPath, start, length);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FlashSetEnhancedArea: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FlashSetEnhancedArea");
	}

	env->ReleaseStringUTFChars(sourcePath, cSrcPath);
	return result;
}

JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFlashSelectLoaderOptions(
		JNIEnv *env, jobject obj, jlong instance_p, jlong uiProperty, jlong uiValue) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];
	int result = -1;
	try {
        	result = Flash_SelectLoaderOptions_fn(LCDObject, uiProperty, uiValue);
	} catch (const char* errmess) {
	        Logger::getInstance()->error(LP, std::string( "Exception in FlashSelectLoaderOptions: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FlashSelectLoaderOptions");
	}
	return result;
}


//----------------------------
// FILE SYSTEM 0x03
//----------------------------

// VOLUME PROPERTIES 0x02
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSVolumeProperties(
		JNIEnv *env, jobject obj, jlong instance_p, jstring devicePath) {

	jclass volumePropertiesClass;
	jmethodID construcorId;
	jobject volumeProperitesObject;
	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cDevicePath = env->GetStringUTFChars(devicePath, 0);
	char* fsType = new char[100];
	int fsTypeSize = 100;

	uint64_t* size = new uint64_t;
	uint64_t* free = new uint64_t;

	if (!(volumePropertiesClass = env->FindClass(CLS_VOLUME_PROPS_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'",
						CLS_VOLUME_PROPS_TYPE));
		return NULL;
	}

	//constructor: VolumePropertiesType(int status, String fsType, int size, int free)
	if (!(construcorId = env->GetMethodID(volumePropertiesClass,
			METHOD_CONSTRUCTOR, METHOD_CONST_VOLUME_PROPS_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor of class '%s' with the signature '%s'",
						CLS_VOLUME_PROPS_TYPE,
						METHOD_CONST_VOLUME_PROPS_TYPE_SIG));
		return NULL;
	}

	int result = 0;
	try {

#ifdef _TEST
		volumeProperitesObject = env->NewObject(volumePropertiesClass, construcorId, result, env->NewStringUTF("T"), 1, 1, 1);
		return volumeProperitesObject;
#endif

		// fsTypeSize is strlen() value of fsType string
		result = FileSystem_VolumeProperties_fn(LCDObject, cDevicePath, fsType,
				&fsTypeSize, size, free);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSVolumeProperties: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FSVolumeProperties");
	}

//	if(result != 0) {
//		volumeProperitesObject = env->NewObject(volumePropertiesClass, construcorId, result, jfsType, *fsTypeSize, *size, *free);
//		return volumeProperitesObject;
//	}

	fsType[fsTypeSize] = '\0';

	jstring jfsType = env->NewStringUTF(fsType);

	volumeProperitesObject = env->NewObject(volumePropertiesClass, construcorId,
			result, jfsType, fsTypeSize, *size, *free);

	delete[] fsType;
	delete size;
	delete free;
	env->ReleaseStringUTFChars(devicePath, cDevicePath);

	return volumeProperitesObject;
}

// FORMAT VOLUME 0x03
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSFormatVolume(
		JNIEnv *env, jobject obj, jlong instance_p, jstring devicePath) {
#ifdef _TEST
	return 0;
#endif

	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cDevicePath = env->GetStringUTFChars(devicePath, 0);
	int result = 0;
	try {
		result = FileSystem_FormatVolume_fn(LCDObject, cDevicePath);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSFormatVolume: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FSFormatVolume");
	}

	env->ReleaseStringUTFChars(devicePath, cDevicePath);

	return result;
}

// LIST DIRECTORY 0x04
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSListDirectory(
		JNIEnv *env, jobject obj, jlong instance_p, jstring fileSystemPath) {

	jclass listDirectoryTypeClass;
	jmethodID construcorId;
	jmethodID memberId;
	jobject listDirectoryObject;
	TEntries* entries = new TEntries[100]; // ???
	int entriesSize = 100;

	const char* cFileSystemPath = env->GetStringUTFChars(fileSystemPath, 0);

	void* LCDObject = (void*) instanceMap[instance_p];

	if (!(listDirectoryTypeClass = env->FindClass(CLS_LIST_DIR_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'",
						CLS_LIST_DIR_TYPE));
		return NULL;
	}

	//constructor: ListDirectoryType(int status, int entryCount)
	if (!(construcorId = env->GetMethodID(listDirectoryTypeClass,
			METHOD_CONSTRUCTOR, METHOD_CONST_LIST_DIR_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor for class '%s' with the signature '%s'.",
						CLS_LIST_DIR_TYPE, METHOD_CONST_LIST_DIR_TYPE_SIG));
		return NULL;
	}

	//method: addDirectory(String name, int size, int mode, int time)
	memberId = env->GetMethodID(listDirectoryTypeClass,
			METHOD_LDIRT_ADD_DIRECTORY, METHOD_LDIRT_ADD_DIRECTORY_SIG);

	if (memberId == NULL) {
		env->ThrowNew(globalExceptionClass,
				"Could not find the method 'addDirectory'");
		return NULL;
	}

	int result = 0;
	try {

#ifdef _TEST
		listDirectoryObject = env->NewObject(listDirectoryTypeClass, construcorId, 0, 0);
		return listDirectoryObject;
#endif

		// (void *Object, const char* Path, TEntries* Entries, int* DeviceSize);
		result = FileSystem_ListDirectory_fn(LCDObject, cFileSystemPath,
				entries, &entriesSize);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSListDirectory: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FSListDirectory");
	}

	listDirectoryObject = env->NewObject(listDirectoryTypeClass, construcorId,
			result, entriesSize);

	for (int i = 0; i < entriesSize; i++) {
		entries[i].Name[entries[i].NameSize] = '\0';
		jstring name = env->NewStringUTF(entries[i].Name);
		// call member method
		env->CallVoidMethod(listDirectoryObject, memberId, name,
				entries[i].NameSize, entries[i].Size, entries[i].Mode,
				entries[i].Time);
	}

	// relese resources
	env->ReleaseStringUTFChars(fileSystemPath, cFileSystemPath);

	delete[] entries;

	return listDirectoryObject;
}

// MOVE FILE 0x05
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSMoveFile(
		JNIEnv *env, jobject obj, jlong instance_p, jstring sourcePath,
		jstring destPath) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cSrcPath = env->GetStringUTFChars(sourcePath, 0);
	const char* cDestPath = env->GetStringUTFChars(destPath, 0);

	int result = 0;
	try {
		result = FileSystem_MoveFile_fn(LCDObject, cSrcPath, cDestPath);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSMoveFile: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FSMoveFile");
	}

	env->ReleaseStringUTFChars(sourcePath, cSrcPath);
	env->ReleaseStringUTFChars(destPath, cDestPath);

	return result;
}

// DELETE FILE 0x06
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSDeleteFile(
		JNIEnv *env, jobject obj, jlong instance_p, jstring fileSystemFile) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cFileSystemFile = env->GetStringUTFChars(fileSystemFile, 0);

	int result = 0;
	try {
		result = FileSystem_DeleteFile_fn(LCDObject, cFileSystemFile);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSDeleteFile: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FSDeleteFile");
	}

	env->ReleaseStringUTFChars(fileSystemFile, cFileSystemFile);

	return result;
}

// COPY FILE 0x07
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSCopyFile(
		JNIEnv *env, jobject obj, jlong instance_p, jstring sourcePath,
		jstring destPath, jboolean srcUseBulk, jboolean destUseBulk) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cSrcPath = env->GetStringUTFChars(sourcePath, 0);
	const char* cDestPath = env->GetStringUTFChars(destPath, 0);

	int result = 0;
	try {
		// (void *Object, const char* SourcePath, int sourceUseBulk, const char* DestinationPath, int destinationUseBulk);
		result = FileSystem_CopyFile_fn(LCDObject, cSrcPath, (int) srcUseBulk,
				cDestPath, (int) destUseBulk);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSCopyFile: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FSCopyFile");
	}

	env->ReleaseStringUTFChars(sourcePath, cSrcPath);
	env->ReleaseStringUTFChars(destPath, cDestPath);

	return result;
}

// CREATE DIRECTORY 0x08
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSCreateDirectory(
		JNIEnv *env, jobject obj, jlong instance_p, jstring targetPath) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cTargetPath = env->GetStringUTFChars(targetPath, 0);

	int result = 0;
	try {
		result = FileSystem_CreateDirectory_fn(LCDObject, cTargetPath);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSCreateDirecotry: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FSCreateDirecotry");
	}

	env->ReleaseStringUTFChars(targetPath, cTargetPath);

	return result;
}

// PROPERTIES 0x09
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSProperties(
		JNIEnv *env, jobject obj, jlong instance_p, jstring fileSystemPath) {

	jclass FSPropertiesTypeClass;
	jmethodID construcorId;
	jobject FSPropertiesObject;
	int* mode = new int;
	int* access = new int;
	uint64_t* size = new uint64_t;
	int* mTime = new int;
	int* aTime = new int;
	int* cTime = new int;

	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cFileSystemPath = env->GetStringUTFChars(fileSystemPath, 0);

	FSPropertiesTypeClass = env->FindClass(CLS_FS_PROPS_TYPE);

	if (!(construcorId = env->GetMethodID(FSPropertiesTypeClass,
			METHOD_CONSTRUCTOR, METHOD_CONST_FS_PROPS_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor of class '%s' with the signature '%s'.",
						CLS_FS_PROPS_TYPE, METHOD_CONST_FS_PROPS_TYPE_SIG));
		return NULL;
	}

	int result = 0;
	try {
#ifdef _TEST
		FSPropertiesObject = env->NewObject(FSPropertiesTypeClass, construcorId, 0, 0, 0, 0, 0, 0 );
		return FSPropertiesObject;
#endif
		// (void *Object, const char* TargetPath, int* Mode, unsigned __int64* Size, int* MTime, int* ATime, int* CTime);
		result = FileSystem_Properties_fn(LCDObject, cFileSystemPath, mode,
				size, mTime, aTime, cTime);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSproperties: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FSproperties");
	}

	FSPropertiesObject = env->NewObject(FSPropertiesTypeClass, construcorId,
			result, *mode, *size, *mTime, *aTime, *cTime);

	delete mode;
	delete access;
	delete size;
	delete mTime;
	delete aTime;
	delete cTime;

	env->ReleaseStringUTFChars(fileSystemPath, cFileSystemPath);

	return FSPropertiesObject;
}

// CHANGE ACCESS 0x10
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSChangeAccess(
		JNIEnv *env, jobject obj, jlong instance_p, jstring targetPath,
		jint access) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cTargetPath = env->GetStringUTFChars(targetPath, 0);
	int result = 0;
	try {
		result = FileSystem_ChangeAccess_fn(LCDObject, cTargetPath, access);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSChangeAccess: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in FSChangeAccess");
	}

	env->ReleaseStringUTFChars(targetPath, cTargetPath);

	return result;
}

// READ LOAD MODULE MANIFESTS
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nFSReadLoadModuleManifests(
		JNIEnv *env, jobject obj, jlong instance_p, jstring targetPath,
		jstring sourcePath) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cTargetPath = env->GetStringUTFChars(targetPath, 0);
	const char* cSourcePath = env->GetStringUTFChars(sourcePath, 0);

	int result = 0;
	try {
		result = FileSystem_ReadLoadModulesManifests_fn(LCDObject, cTargetPath,
				cSourcePath);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in FSReadLoadModuleManifests: ")
						+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP,
				"Exception in FSReadLoadModuleManifests");
	}

	env->ReleaseStringUTFChars(targetPath, cTargetPath);
	env->ReleaseStringUTFChars(sourcePath, cSourcePath);

	return result;
}

//----------------------------
// OTP 0x04
//----------------------------

// READ BITS 0x01
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nOTPReadBits(
		JNIEnv *env, jobject obj, jlong instance_p, jint id, jint start,
		jint length) {

	jclass otpReadBitsClass;
	jmethodID construcorId;
	jobject otpReadBitsObject;
	int dataBufferSize = (length + 7) / 8;
	int statusBufferSize = dataBufferSize;
	unsigned char* dataBuffer = new unsigned char[dataBufferSize];
	unsigned char* statusBuffer = new unsigned char[statusBufferSize];

	void* LCDObject = (void*) instanceMap[instance_p];

	Logger::getInstance()->debug(LP, "----------------------------------");
	Logger::getInstance()->debug(LP, "OTP - Read Bits");
	Logger::getInstance()->debug(LP, "----------------------------------");

	if (!(otpReadBitsClass = env->FindClass(CLS_OTP_READ_BITS_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'",
						CLS_OTP_READ_BITS_TYPE));
		return NULL;
	}

	if (!(construcorId = env->GetMethodID(otpReadBitsClass, METHOD_CONSTRUCTOR,
			METHOD_CONST_OTP_READ_BITS_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor of class '%s' with the signature '%s'.",
						CLS_OTP_READ_BITS_TYPE,
						METHOD_CONST_OTP_READ_BITS_TYPE_SIG));
		return NULL;
	}

	int result = 0;
	try {
#ifdef _TEST
		otpReadBitsObject = env->NewObject(otpReadBitsClass, construcorId, 0, env->NewByteArray(1), env->NewByteArray(1));
		return otpReadBitsObject;
#endif
		std::stringstream ss;
		ss << "before:" << std::endl;
		ss << "BufferData: 0x" << std::setbase(16) << dataBuffer << std::endl;
		ss << "BufferStatus: 0x" << std::setbase(16) << statusBuffer
				<< std::endl;
		ss << "dataBufferSize: " << std::setbase(10) << dataBufferSize
				<< std::endl;
		ss << "statusBufferSize: " << statusBufferSize << std::endl;
		ss << "Id: " << id << std::endl;
		ss << "Start: " << start << std::endl;
		ss << "Length: " << length << std::endl;
		Logger::getInstance()->debug(LP, ss.str());
		result = OTP_ReadBits_fn(LCDObject, id, start, length, dataBuffer,
				&dataBufferSize, statusBuffer, &statusBufferSize);
		ss.str("after:");
		ss << std::endl;
		ss << "BufferData: 0x" << std::setbase(16) << dataBuffer << std::endl;
		ss << "BufferStatus: 0x" << std::setbase(16) << statusBuffer
				<< std::endl;
		ss << "dataBufferSize: " << std::setbase(10) << dataBufferSize
				<< std::endl;
		ss << "statusBufferSize: " << statusBufferSize << std::endl;
		ss << "Id: " << id << std::endl;
		ss << "Start: " << start << std::endl;
		ss << "Length: " << length << std::endl;
		Logger::getInstance()->debug(LP, ss.str());
		debugBuffer((unsigned char*) dataBuffer, dataBufferSize, 8,
				"dataBuffer");

	} catch (const char* errmess) {
		Logger::getInstance()->error(LP,
				std::string("Exception in readOTPBits: ") + errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in readOTPBits");
	}

	jbyteArray jDataBuffer = env->NewByteArray(dataBufferSize);
	jbyteArray jStatusBuffer = env->NewByteArray(statusBufferSize);

	if (jDataBuffer == NULL || jStatusBuffer == NULL) {
		return NULL;
	}

	env->SetByteArrayRegion(jDataBuffer, 0, dataBufferSize,
			(jbyte*) dataBuffer);
	env->SetByteArrayRegion(jStatusBuffer, 0, statusBufferSize,
			(jbyte*) statusBuffer);

	otpReadBitsObject = env->NewObject(otpReadBitsClass, construcorId, result,
			jDataBuffer, jStatusBuffer);

	delete[] dataBuffer;
	delete[] statusBuffer;

	return otpReadBitsObject;
}

// SET BITS 0x02
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nOTPSetBits(
		JNIEnv *env, jobject obj, jlong instance_p, jint id, jint start,
		jint length, jbyteArray bits) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	Logger::getInstance()->debug(LP, "----------------------------------");
	Logger::getInstance()->debug(LP, "OTP - Set Bits");
	Logger::getInstance()->debug(LP, "----------------------------------");

	//fill the data
	jbyte * bits_p;
	bits_p = env->GetByteArrayElements(bits, 0);
	if (bits_p == NULL) {
//		errorLog(env, "Out of memory thrown: in OTPWriteBits");
		return -1;
	}

	int result = 0;
	try {
		std::stringstream ss;
		ss << "Buffer: 0x" << std::setbase(16) << bits_p << std::endl;
		ss << "Id: " << std::setbase(10) << id << std::endl;
		ss << "Start: " << start << std::endl;
		ss << "Length: " << length << std::endl;
		ss << "----------------------------------";
		Logger::getInstance()->debug(LP, ss.str());
		debugBuffer((unsigned char*) bits_p, length / 8, 8, "bits_p");

		result = OTP_SetBits_fn(LCDObject, id, start, length,
				(unsigned char*) bits_p);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string("Exception in OTPSetBits: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in OTPSetBits");
	}

	env->ReleaseByteArrayElements(bits, bits_p, 0);

	return result;
}

// WRITE AND LOCK BITS 0x03
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nOTPWriteAndLockBits(
		JNIEnv *env, jobject obj, jlong instance_p, jint id) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	int result = 0;
	try {
		result = OTP_WriteAndLock_fn(LCDObject, id);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in OTPWriteAndLockBits: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in OTPWriteAndLockBits");
	}

	return result;
}

//----------------------------
// PARAMETER STORAGE 0x05
//----------------------------

// READ GLOBAL DATA UNIT 0x01
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSReadGlobalDataUnit(
		JNIEnv *env, jobject obj, jlong instance_p, jstring devicePath,
		jint unitId) {

	jclass readGlobalDataUnitClass;
	jmethodID construcorId;
	jobject readGlobalDataUnitObject;

	void* LCDObject = (void*) instanceMap[instance_p];
	unsigned char* dataBuffer = new unsigned char[5000]; // ???
	int bufferSize = 5000;
	const char *utfDevicePath = env->GetStringUTFChars(devicePath, 0);

	if (!(readGlobalDataUnitClass = env->FindClass(
			CLS_PSREAD_GLOBAL_DATA_UNIT_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'.",
						CLS_PSREAD_GLOBAL_DATA_UNIT_TYPE));
		env->ReleaseStringUTFChars(devicePath, utfDevicePath);
		delete[] dataBuffer;
		return NULL;
	}

	if (!(construcorId = env->GetMethodID(readGlobalDataUnitClass,
			METHOD_CONSTRUCTOR, METHOD_CONST_PSREAD_GLOBAL_DATA_UNIT_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor of class '%s' with the signature '%s'.",
						CLS_PSREAD_GLOBAL_DATA_UNIT_TYPE,
						METHOD_CONST_PSREAD_GLOBAL_DATA_UNIT_TYPE_SIG));
		env->ReleaseStringUTFChars(devicePath, utfDevicePath);
		delete[] dataBuffer;
		return NULL;
	}

	int result = 0;
	try {

#ifdef _TEST
		readGlobalDataUnitObject = env->NewObject(readGlobalDataUnitClass, construcorId, 0, env->NewByteArray(1));
		return readGlobalDataUnitObject;
#endif
		// (void *Object, int GdfsId, int Unit, unsigned char* DataBuffer, int* Size);
		result = ParameterStorage_ReadGlobalDataUnit_fn(LCDObject,
				utfDevicePath, unitId, dataBuffer, &bufferSize);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in ReadGlobalDataUnit: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in ReadGlobalDataUnit");
	}

	jbyteArray jDataBuffer = env->NewByteArray(bufferSize);
	if (jDataBuffer == NULL) {
		env->ReleaseStringUTFChars(devicePath, utfDevicePath);
		delete[] dataBuffer;
		return NULL;
	}

	env->SetByteArrayRegion(jDataBuffer, 0, bufferSize, (jbyte*) dataBuffer);

	readGlobalDataUnitObject = env->NewObject(readGlobalDataUnitClass,
			construcorId, result, jDataBuffer);
	if (readGlobalDataUnitObject == NULL) {
		env->ReleaseStringUTFChars(devicePath, utfDevicePath);
		delete[] dataBuffer;
		return NULL;
	}

	env->ReleaseStringUTFChars(devicePath, utfDevicePath);
	delete[] dataBuffer;

	return readGlobalDataUnitObject;
}

// WRITE GLOBAL DATA UNIT 0x02
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSWriteGlobalDataUnit(
		JNIEnv *env, jobject obj, jlong instance_p, jstring devicePath,
		jint unitId, jbyteArray data, jint length) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];
	const char *utfDevicePath = env->GetStringUTFChars(devicePath, 0);

	//fill the data
	jbyte * data_p;
	data_p = env->GetByteArrayElements(data, 0);
	if (data_p == NULL) {
//		errorLog(env, "Out of memory thrown: in WriteGLobalDataUnit");
		return -1;
	}

	int result = 0;
	try {
		result = ParameterStorage_WriteGlobalDataUnit_fn(LCDObject,
				utfDevicePath, unitId, (unsigned char*) data_p, length);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in WriteGlobalDataUnit: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in WriteGlobalDataUnit");
	}

	env->ReleaseByteArrayElements(data, (jbyte*) data_p, 0);
	env->ReleaseStringUTFChars(devicePath, utfDevicePath);

	return result;
}

// READ GLOBAL DATA SET 0x03
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSReadGlobalDataSet(
		JNIEnv *env, jobject obj, jlong instance_p, jstring devicePath,
		jstring sourcePath, jboolean useBulk) {
#ifdef _TEST
	return 1;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	const char* cSrcPath = env->GetStringUTFChars(sourcePath, 0);
	const char *utfDevicePath = env->GetStringUTFChars(devicePath, 0);

	int result = 0;
	try {
		result = ParameterStorage_ReadGlobalDataSet_fn(LCDObject, utfDevicePath,
				cSrcPath, (int) useBulk);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in ReadGlobalDataSet: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in ReadGlobalDataSet");
	}

	env->ReleaseStringUTFChars(sourcePath, cSrcPath);
	env->ReleaseStringUTFChars(devicePath, utfDevicePath);

	return result;
}

// WRITE GLOBAL DATA SET 0x04
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSWriteGlobalDataSet(
		JNIEnv *env, jobject obj, jlong instance_p, jstring devicePath,
		jlong dataLength, jstring sourcePath, jboolean useBulk) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];
	const char* cSrcPath = env->GetStringUTFChars(sourcePath, 0);
	const char *utfDevicePath = env->GetStringUTFChars(devicePath, 0);

	int result = 0;
	try {
		result = ParameterStorage_WriteGlobalDataSet_fn(LCDObject,
				utfDevicePath, cSrcPath, (int) useBulk);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in WriteGlobalDataSet: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in WriteGlobalDataSet");
	}

	env->ReleaseStringUTFChars(sourcePath, cSrcPath);
	env->ReleaseStringUTFChars(devicePath, utfDevicePath);

	return result;
}

// ERASE GLOBAL DATA SET 0x05
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nPSEraseGlobalDataSet(
		JNIEnv *env, jobject obj, jlong instance_p, jstring devicePath) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];
	const char *utfDevicePath = env->GetStringUTFChars(devicePath, 0);

	int result = 0;
	try {
		result = ParameterStorage_EraseGlobalDataSet_fn(LCDObject,
				utfDevicePath);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in EraseGlobalDataSet: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in EraseGlobalDataSet");
	}

	env->ReleaseStringUTFChars(devicePath, utfDevicePath);

	return result;
}

//----------------------------
// SECURITY 0x06
//----------------------------

// SET DOMAIN 0x01
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecuritySetDomain(
		JNIEnv *env, jobject obj, jlong instance_p, jint domain) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	int result = 0;
	try {
		result = Security_SetDomain_fn(LCDObject, domain);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in SecuritySetDomain: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SecuritySetDomain");
	}

	return result;
}

// GET DOMAIN 0x02
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityGetDomain(
		JNIEnv *env, jobject obj, jlong instance_p) {

	jclass domainClass;
	jmethodID construcorId;
	jobject domainObject;
	int* writtenDomain = new int;

	void* LCDObject = (void*) instanceMap[instance_p];

	if (!(domainClass = env->FindClass(CLS_DOMAIN_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'",
						CLS_DOMAIN_TYPE));
		return NULL;
	}

	//constructor: SecurityPropertiesType(int status, byte[] buffer)
	if (!(construcorId = env->GetMethodID(domainClass, METHOD_CONSTRUCTOR,
			METHOD_CONST_DOMAIN_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor of class '%s' with the signature '%s'",
						CLS_DOMAIN_TYPE, METHOD_CONST_DOMAIN_TYPE_SIG));
		return NULL;
	}

	int result = 0;
	try {

#ifdef _TEST
		domainObject = env->NewObject(domainClass, construcorId, 0, 1);
		return domainObject;
#endif

		result = Security_GetDomain_fn(LCDObject, writtenDomain);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in SecurityGetDomain: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SecurityGetDomain");
	}

	domainObject = env->NewObject(domainClass, construcorId, result,
			*writtenDomain);

	delete writtenDomain;

	return domainObject;
}

// GET PROPERTIES 0x03  TODO: implement when the command is defined
JNIEXPORT jobject JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityGetProperties(
		JNIEnv *env, jobject obj, jlong instance_p, jint propertyId) {

	jclass propertiesClass;
	jmethodID construcorId;
	jobject propertiesObject;
	unsigned char* dataBuffer = new unsigned char[2000]; //???
	int dataBufferSize = 2000;

	void* LCDObject = (void*) instanceMap[instance_p];

	if (!(propertiesClass = env->FindClass(CLS_SECURITY_PROPS_TYPE))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage("Could not find the class: '%s'",
						CLS_SECURITY_PROPS_TYPE));
		return NULL;
	}

	//constructor: SecurityPropertiesType(int status, byte[] buffer)
	if (!(construcorId = env->GetMethodID(propertiesClass, METHOD_CONSTRUCTOR,
			METHOD_CONST_SECURITY_PROPS_TYPE_SIG))) {
		env->ThrowNew(globalExceptionClass,
				formatMessage(
						"Could not find the constructor of class '%s' with the signature '%s'.",
						CLS_SECURITY_PROPS_TYPE,
						METHOD_CONST_SECURITY_PROPS_TYPE_SIG));
		return NULL;
	}

	int result = 0;
	try {
#ifdef _TEST
		propertiesObject = env->NewObject(propertiesClass, construcorId, 0, env->NewByteArray(1));
		return propertiesObject;
#endif

		result = Security_GetProperties_fn(LCDObject, propertyId,
				&dataBufferSize, dataBuffer);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in SecurityGetProperties: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SecurityGetProperties");
	}

	jbyteArray jDataBuffer = env->NewByteArray(dataBufferSize);
	if (jDataBuffer == NULL) {
		delete[] dataBuffer;
		return NULL;
	}

	env->SetByteArrayRegion(jDataBuffer, 0, dataBufferSize,
			(jbyte*) dataBuffer);

	propertiesObject = env->NewObject(propertiesClass, construcorId, result,
			jDataBuffer);

	delete[] dataBuffer;

	return propertiesObject;
}

// SET PROPERTIES 0x04 TODO: implement when the command is defined
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecuritySetProperties(
		JNIEnv *env, jobject obj, jlong instance_p, jint propertyId,
		jint dataLength, jbyteArray data) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	//fill the data
	jbyte * data_p;
	data_p = env->GetByteArrayElements(data, 0);

	if (data_p == NULL) {
//		errorLog(env, "Out of memory thrown: in SetProperties");
		return -1;
	}

	int result = -1;
	try {
		result = Security_SetProperties_fn(LCDObject, propertyId, dataLength,
				(unsigned char*) data_p);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in SecuritySetProperties: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SecuritySetProperties");
	}

	env->ReleaseByteArrayElements(data, data_p, 0);

	return result;
}

// BIND PROPERTIES 0x05
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityBindProperties(
		JNIEnv *env, jobject obj, jlong instance_p) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	int result = -1;
	try {
		result = Security_BindProperties_fn(LCDObject);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in SecurityBindProperties: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SecurityBindProperties");
	}

	return result;
}

// STORE SECURITY OBJECT 0x06
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nOTPStoreSecurityObject(
		JNIEnv *env, jobject obj, jlong instance_p, jstring sourcePath,
		jint destination, jboolean useBulk) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];
	const char* cSrcPath = env->GetStringUTFChars(sourcePath, 0);

	int result = -1;
	try {
		result = OTP_StoreSecureObject_fn(LCDObject, cSrcPath, destination,
				(int) useBulk);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in StoreSecureObject: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in StoreSecureObject");
	}

	env->ReleaseStringUTFChars(sourcePath, cSrcPath);

	return result;
}

// WRITE RPMB KEY 0x07
JNIEXPORT jint JNICALL Java_com_stericsson_sdk_loader_communication_internal_LoaderCommunicationInterface_nSecurityWriteRpmbKey(
		JNIEnv *env, jobject obj, jlong instance_p, jint deviceId, jboolean commercial) {
#ifdef _TEST
	return 0;
#endif
	void* LCDObject = (void*) instanceMap[instance_p];

	int result = -1;
	try {
		result = Security_WriteRpmbKey_fn(LCDObject, deviceId, (int) commercial);
	} catch (const char* errmess) {
		Logger::getInstance()->error(LP, std::string( "Exception in SecurityWriteRpmbKey: ")+ errmess);
	} catch (...) {
		Logger::getInstance()->error(LP, "Exception in SecurityWriteRpmbKey");
	}

	return result;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
