/*
 * util.h
 *
 *  Created on: Jun 15, 2009
 *      Author: emicroh
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <jni.h>
#include <iostream>
#include <sstream>
#include <wchar.h>
#include <list>
#include "javadefs.h"

#ifdef __WIN__
	#define WINDOWS_OS
#endif

#if _MSC_VER
  #define WINDOWS_OS
  #define snprintf _snprintf
#endif
//#ifndef UNICODE
//#define UNICODE
//#endif

#ifndef WINDOWS_OS
	#include <stdio.h>
	#include <memory.h>
	#include <stdarg.h>

	//define types for compatibility with Windows tchar.h
	typedef int HANDLE;
	typedef const char * LPCTSTR;
	typedef char TCHAR;
	typedef char * PTCHAR;
	typedef unsigned int DWORD;

	//function defines for compatibility with Windows
	#define _tmain main
	#define _vstprintf vsprintf
	#define _tprintf printf
	#define _stprintf sprintf
	#define _tscanf scanf
	#define FillMemory(d,l,f) memset((d), (f), (l))

	//define for invalid handle
	#define INVALID_HANDLE_VALUE -1

#else
	#include <windows.h>
	#include <tchar.h>
#endif

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
#define LOG_BUF_LENGTH 1024
PTCHAR getMessage(LPCTSTR fstring, ...);

/**
 * Dumps contents of a buffer to a file in hex.
 */
//void dump(const unsigned char * buff, int len, bool out);

#endif /*UTIL_H_*/
