/*
 * ThermalManager JNI bindings
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define LOG_TAG "ThermalManagerJNI"

#include <pthread.h>

#include <JNIHelp.h>
#include <jni.h>
#include <cutils/log.h>

#include <thermal_manager.h>

#define CONNECTION_RETRY_TIMEOUT_SECS (2)

/*
 * Event mutexs to lock Java recieve event until new events occur
 */
static pthread_mutex_t eventMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t eventCond = PTHREAD_COND_INITIALIZER;
static STE_TM_notification_t *pendingNotification = NULL;

/*
 * Cached java references
 */
static jobject ThermalManager = NULL;
static jmethodID ThermalManager_reportEvent = NULL;
static jclass Null = NULL;
static jclass ArrayList = NULL;
static jmethodID ArrayList_add = NULL;
static jclass ThermalSensorConfiguration = NULL;
static jmethodID ThermalSensorConfiguration_init = NULL;
static jmethodID ThermalSensorConfiguration_addNotificationLevel = NULL;
static jclass ThermalNotification = NULL;
static jmethodID ThermalNotification_init = NULL;
static jmethodID ThermalNotification_addAction = NULL;
static jclass ThermalNotificationLevel = NULL;
static jmethodID ThermalNotificationLevel_init = NULL;
static jmethodID ThermalNotificationLevel_addAction = NULL;

/*
 * libthermal Interface functions
 */
static void TM_notification_cb(STE_TM_notification_t *notification);
static void TM_error_cb(STE_TM_error_t error);
static STE_TM_callbacks_t TM_callbacks = {TM_notification_cb, TM_error_cb};

/*
 * Called from libthermal to inform us of a thermal notification event.
 * This will be called from a pthread, not the main Java thread, consequently
 * as it is not possible for a non JVM thread to communicate with the Java
 * layer we must buffer this
 */
static void TM_notification_cb(STE_TM_notification_t *notification) {
	pthread_mutex_lock(&eventMutex);

	// clone the incoming notification object so we can use it in the main
	// java thread
	pendingNotification = STE_TM_clone_notification(notification);

	// flag Java receive thread that new data is ready
	pthread_cond_signal(&eventCond);
	pthread_mutex_unlock(&eventMutex);
}

/*
 * Called when libthermal detects an error. We can try and take action here
 * to mitagate the effects.
 */
static void TM_error_cb(STE_TM_error_t error) {
	switch (error) {
	case TM_UNABLE_TO_CONNECT_TO_THERMAL_MANAGER:
	case TM_INVALID_DATA_RECEIVED:
	case TM_CONNECTION_DROPPED:
		ALOGE("connection to thermal manager was lost. trying to reconnect.\n");

	case TM_NOT_INITALISED:
		STE_TM_deinit();

		sleep(CONNECTION_RETRY_TIMEOUT_SECS);
		if (STE_TM_init(&TM_callbacks) == 0) {
			ALOGD("connection to thermal manager re-established!\n");
		} else {
			ALOGE("failed to bring back connection to thermal manager....\n");
		}
		break;

	case TM_UNABLE_TO_ALLOCATE_MEMORY:
		ALOGE("libthermal failed to allocate memory\n");
		break;

	case TM_INVALID_PARAMETER:
		ALOGE("invalid data was passed to the thermal manager!\n");
		break;

	default:
		// we can't do anything about this, so silently ignore
		break;
	}
}

/*
 * Initalises the JNI part of the ThermalManager class caching
 * all the appropriate references needed
 */
static void native_class_init(JNIEnv *env, jclass clazz) {
	jclass localRef;

	// get Java NULL pointer
	Null = (jclass) env->NewGlobalRef(NULL);

	// resolve java.util.ArrayList & required methods
	localRef = env->FindClass("java/util/ArrayList");
	ArrayList = (jclass) env->NewGlobalRef(localRef);
	ArrayList_add = env->GetMethodID(ArrayList, "add", "(Ljava/lang/Object;)Z");

	// resolve com.stericsson.thermal.ThermalSensorConfiguration
	localRef = env->FindClass("com/stericsson/thermal/ThermalSensorConfiguration");
	ThermalSensorConfiguration = (jclass) env->NewGlobalRef(localRef);
	ThermalSensorConfiguration_init = env->GetMethodID(ThermalSensorConfiguration,
	                                  "<init>", "(Ljava/lang/String;I)V");
	ThermalSensorConfiguration_addNotificationLevel = env->GetMethodID(ThermalSensorConfiguration,
	        "addNotificationLevel", "(Lcom/stericsson/thermal/ThermalNotificationLevel;)V");

	// resolve com.stericsson.thermal.ThermalNotification
	localRef = env->FindClass("com/stericsson/thermal/ThermalNotification");
	ThermalNotification = (jclass) env->NewGlobalRef(localRef);
	ThermalNotification_init = env->GetMethodID(ThermalNotification, "<init>",
	                           "(Ljava/lang/String;ZIII)V");
	ThermalNotification_addAction = env->GetMethodID(ThermalNotification,
	                                "addAction",
	                                "(Ljava/lang/String;)V");

	// resolve com.stericsson.thermal.ThermalNotificationLevel
	localRef = env->FindClass("com/stericsson/thermal/ThermalNotificationLevel");
	ThermalNotificationLevel = (jclass) env->NewGlobalRef(localRef);
	ThermalNotificationLevel_init = env->GetMethodID(ThermalNotificationLevel, "<init>",
	                                "(III)V");
	ThermalNotificationLevel_addAction = env->GetMethodID(ThermalNotificationLevel,
	                                     "addAction",
	                                     "(Ljava/lang/String;)V");

	// resolve com.stericsson.thermal.ThermalManger methods
	ThermalManager_reportEvent = env->GetMethodID(clazz, "reportEvent",
	                             "(Lcom/stericsson/thermal/ThermalNotification;)V");
}

/*
 * Called during the startup of the main java object.
 *
 * HACK ALERT!
 *
 * In theroy the reference passed into obj should be of the type
 * ThermalManager, however it appears to be coming in as a
 * java.lang.Class, which is not castable to a ThermalManager.
 *
 * So to get around this, we pass in a reference to this during
 * the object's construction so that the JNI has access.
 */
static void native_init(JNIEnv *env, jobject obj, jobject me) {
	ThermalManager = env->NewGlobalRef(me);

	// bring up connection to thermal manager
	if (STE_TM_init(&TM_callbacks) != 0) {
		ALOGE("thermal manager connection failed to come up.\n");
	}
}

/*
 * Called to release all the cached references
 */
static void native_release(JNIEnv *env, jobject obj) {
	STE_TM_deinit();

	if (Null)
		env->DeleteGlobalRef(Null);

	if (ArrayList)
		env->DeleteGlobalRef(ArrayList);

	if (ThermalSensorConfiguration)
		env->DeleteGlobalRef(ThermalSensorConfiguration);

	if (ThermalNotification)
		env->DeleteGlobalRef(ThermalNotification);

	if (ThermalNotificationLevel)
		env->DeleteGlobalRef(ThermalNotificationLevel);

	if (ThermalManager)
		env->DeleteGlobalRef(ThermalManager);

	if (pendingNotification != NULL) {
		free(pendingNotification);
	}
}

/**
 * Called by Java receive loop which will block until new data is ready.
 * This is needed because the JVM will not allow communication with
 * a non JVM thread (ie the libthermal thread is not able to talk
 * with java)
 */
static void native_wait_for_event(JNIEnv *env, jobject obj) {
	STE_TM_notification_t *notification;
	jobject notificationObject;
	int i;

	// sit and wait for a notification event
	pthread_mutex_lock(&eventMutex);
	pthread_cond_wait(&eventCond, &eventMutex);

	notification = pendingNotification;
	pendingNotification = NULL;

	pthread_mutex_unlock(&eventMutex);

	if (notification == NULL)
		return;

	notificationObject = env->NewObject(ThermalNotification,
	                                    ThermalNotification_init,
	                                    env->NewStringUTF(notification->sensor),
	                                    ((notification->shutdown_in == 0) ? JNI_FALSE : JNI_TRUE),
	                                    notification->shutdown_in,
	                                    notification->current_level,
	                                    notification->previous_level);

	for (i = 0; i < notification->number_of_actions_taken; i++) {
		env->CallVoidMethod(notificationObject,
		                    ThermalNotification_addAction,
		                    env->NewStringUTF(notification->actions_taken[i]));
	}

	env->CallVoidMethod(ThermalManager, ThermalManager_reportEvent,
	                    notificationObject);

	env->DeleteLocalRef(notificationObject);

	STE_TM_free_cloned_notification(&notification);
}

/*
 * Retrives all the sensors currently known by the ThermalManager
 * and adds it to the passed in ArrayList
 */
static void native_getsensors(JNIEnv *env, jobject obj, jobject list) {
	char **sensors;
	int count, i;

	count = STE_TM_get_sensors(&sensors);
	for (i = 0; i < count; i++) {
		jstring sensor;

		sensor = env->NewStringUTF(sensors[i]);
		(void) env->CallBooleanMethod(list, ArrayList_add, sensor);
		env->DeleteLocalRef(sensor);
	}

	STE_TM_free_sensors(count, &sensors);
}

/*
 * Retrives all the actions currently known by the the ThermalManager
 * and adds it to the passed in ArrayList
 */
static void native_getactions(JNIEnv *env, jobject obj, jobject list) {
	char **actions;
	int count, i;

	count = STE_TM_get_actions(&actions);
	for (i = 0; i < count; i++) {
		jstring action;

		action = env->NewStringUTF(actions[i]);
		(void) env->CallBooleanMethod(list, ArrayList_add, action);
		env->DeleteLocalRef(action);
	}

	STE_TM_free_actions(count, &actions);
}

/*
 * Retrives the sensor configuration for a given sensor
 */
static jobject native_getsensorconfig(JNIEnv *env, jobject obj, jstring sensor) {
	STE_TM_sensor_config_t *sensorconfig;
	const char *sensorid;
	jobject config;
	int i;
	int err;

	// fetch the sensor config
	sensorid = env->GetStringUTFChars(sensor, NULL);
	err = STE_TM_get_sensor_config(sensorid, &sensorconfig);
	env->ReleaseStringUTFChars(sensor, sensorid);

	if (err != 0)
		return Null;

	// create the ThermalSensorConfiguration object
	config = env->NewObject(ThermalSensorConfiguration,
	                        ThermalSensorConfiguration_init,
	                        sensor,
	                        sensorconfig->current_notification_level);

	// for each notication level, add a ThermalNoticationLevel object
	// to the created ThermalSensorConfiguration object
	for (i = 0; i < sensorconfig->no_notification_levels; i++) {
		STE_TM_notification_level_t *level = sensorconfig->notification_levels[i];
		jobject obj;
		int j;

		obj = env->NewObject(ThermalNotificationLevel, ThermalNotificationLevel_init,
		                     level->notification_level,
		                     level->min_alarm_level,
		                     level->max_alarm_level);

		for (j = 0; j < level->number_of_actions; j++) {
			jstring action;

			action = env->NewStringUTF(level->actions[j]);
			env->CallVoidMethod(obj, ThermalNotificationLevel_addAction, action);
			env->DeleteLocalRef(action);
		}

		env->CallVoidMethod(config,
		                    ThermalSensorConfiguration_addNotificationLevel, obj);
		env->DeleteLocalRef(obj);
	}

	return config;
}

/**
 * Native to Java mapping table
 */
static JNINativeMethod native_methods[] = {
	{"native_class_init", "()V", (void *) native_class_init},
	{"native_init", "(Lcom/stericsson/thermal/ThermalManager;)V", (void *) native_init},
	{"native_release", "()V", (void *) native_release},
	{"native_wait_for_event", "()V", (void *) native_wait_for_event},
	{"native_getsensors", "(Ljava/util/ArrayList;)V", (void *) native_getsensors},
	{"native_getactions", "(Ljava/util/ArrayList;)V", (void *) native_getactions},
	{"native_getsensorconfig", "(Ljava/lang/String;)Lcom/stericsson/thermal/ThermalSensorConfiguration;", (void *) native_getsensorconfig},
};

/**
 * Called at startup to register methods
 */
int register_com_stericsson_thermal_ThermalManager(JNIEnv *env) {
	return jniRegisterNativeMethods(env,
	                                "com/stericsson/thermal/ThermalManager",
	                                native_methods, NELEM(native_methods));
}

