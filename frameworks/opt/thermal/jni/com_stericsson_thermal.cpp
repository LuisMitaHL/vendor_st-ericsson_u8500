/*
 * Thermal JNI bindings
 *
 * Copyright (C) ST-Ericsson SA 2011g. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define LOG_TAG "ThermalJNI"
#include <cutils/log.h>

#include "register.h"
#include "jni.h"

/**
 * Called by JVM to register native methods
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;
	jint result = -1;

	// sanity checks...
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		ALOGE("ERROR: GetEnv failed\n");
		goto bail;
	}

	if (env == NULL) {
		ALOGE("can't get JNI Enviroment\n");
		goto bail;
	}

	// register our methods...
	if (register_com_stericsson_thermal_ThermalManager(env) != 0) {
		ALOGE("ERROR: PlatformLibrary native registration failed\n");
		goto bail;
	}

	result = JNI_VERSION_1_4; // XXX should be 1.5?

bail:
	return result;
}

