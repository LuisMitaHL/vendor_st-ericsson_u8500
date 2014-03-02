/*
 * JNInative.h
 *
 *  Created on: 13.1.2010
 *      Author: rohacmic
 */

#ifndef JNINATIVE_H_
#define JNINATIVE_H_

#include <jni.h>

#define PRINTF printf

// global that holds the infromation if LCM has been initialized before
bool initialized = false;

//global method id's for the READ, WRITE and CANCEL functions in java
static jmethodID globalMidRead = NULL;
static jmethodID globalMidWrite = NULL;
static jmethodID globalMidCancel = NULL;

static jmethodID globalMidProgress = NULL;

static JavaVM * globalJavaVM = NULL;
static jobject globalObject = NULL;

static jclass globalExceptionClass = NULL;

static int counter = 0;
// this is not used by PA
char* interfaceId = new char[1];

#endif /* JNINATIVE_H_ */
