/*
 * Copyright (C) ST-Ericsson SA 2010
 * Author: Jean-Philippe FASSINO <jean-philippe.fassino@stericsson.com> for ST-Ericsson.
 * License terms: GNU General Public License (GPL) version 2.
 */
// jni3.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include <stdlib.h>

#include <jni.h> 
#include "windows-native.h"

/*
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    return TRUE;
}
*/

static WORD attribute = FOREGROUND_BLUE + FOREGROUND_RED + FOREGROUND_GREEN;
static void restore() {
	HANDLE h;

    h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(h, attribute);
	}
	return;
}

JNIEXPORT jint JNICALL Java_tools_common_console_windows_init(JNIEnv *env, jclass clazz) {
    atexit((void (*)(void)) restore);
  
  	return Java_tools_common_console_windows_getColor(env, clazz);
}

JNIEXPORT jint JNICALL Java_tools_common_console_windows_getColor(JNIEnv *env, jclass clazz) {
	jint colors = FOREGROUND_BLUE + FOREGROUND_RED + FOREGROUND_GREEN;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	HANDLE h;

    h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h != INVALID_HANDLE_VALUE) {
        if (GetConsoleScreenBufferInfo(h, &csbiInfo)) {
		    colors = (WORD) csbiInfo.wAttributes;
	    }
	}
	return colors;
}

JNIEXPORT void JNICALL Java_tools_common_console_windows_setColor(JNIEnv *env, jclass clazz, jint colors) {
	HANDLE h;

    h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(h, (WORD) colors);
	}
}
