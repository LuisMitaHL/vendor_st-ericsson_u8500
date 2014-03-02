/*
 * util.h
 *
 *  Created on: Jun 15, 2009
 *      Author: emicroh
 */

#ifndef UTIL_H_
#define UTIL_H_
#include <wchar.h>
#include <jni.h>
#include "javadefs.h"

#if _MSC_VER
#define snprintf _snprintf
#endif
//#ifndef UNICODE
//#define UNICODE
//#endif

//debug printing..
#define PRINTF(text) printf(text)

#define ILLEGAL_INPARAM_ARRAY_LENGTH -1

inline jstring newString(JNIEnv* env, const char * body) {
	return env->NewStringUTF(body);
}

inline void releaseStringBody(JNIEnv* env, jstring str, wchar_t const* body) {
	env->ReleaseStringChars(str, (jchar *) body);
}

inline void releaseStringBody(JNIEnv* env, jstring str, char const* body) {
	env->ReleaseStringUTFChars(str, body);
}

inline int getStringLength(JNIEnv* env, jstring str) {
	return env->GetStringLength(str);
}

//UNICODE dependent functions
#ifdef UNICODE

inline jstring newString(JNIEnv* env, const wchar_t * body) {
	return env->NewString((jchar *) body, wcslen(body));
}

inline wchar_t const* getStringBody(JNIEnv* env, jstring str) {
return (wchar_t *) env->GetStringChars(str, 0);
}

#else

inline char const* getStringBody(JNIEnv* env, jstring str) {
return env->GetStringUTFChars(str, 0);
}

#endif

/* Default buffer length used for variety of string operations. */
#define DEFAULT_BUF_LENGTH 1024

char * vgetCharString(const char *fString, ...);
/* Helper function to retrieve result string from given format string and variable argument list using default buffer length */
char * vgetCharString(const char *fString, va_list args);
/* Helper function to retrieve result string from given format string and variable argument list using custom buffer length */
char * vgetCharString(unsigned int bufLength, const char *fString, va_list args);

#endif /*UTIL_H_*/
