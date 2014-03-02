/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/

#ifndef _audiocodec_debug_h_
#define _audiocodec_debug_h_

// Uncomment this define to get debug-prints from this component.
//#define AUDIOCODEC_DEBUG

#define LOG_TAG "audiocodec"

#ifdef ANDROID
	#include <cutils/log.h>
	#define PRINT_TEXT_E ALOGE
	#define PRINT_TEXT_I ALOGI
#else
	#include <stdio.h>
	#define PRINT_TEXT_E printf
	#define PRINT_TEXT_I printf
#endif

#define LOG_LENGTH_MAX 512

const char* get_fcn_name(const char* fcn_name);

#define LOG_E(a, ...) do { PRINT_TEXT_E("%s: " a, get_fcn_name(__PRETTY_FUNCTION__), ##__VA_ARGS__); } while (0)
#ifdef AUDIOCODEC_DEBUG
	#define LOG_I(a, ...) do { PRINT_TEXT_I("%s: " a, get_fcn_name(__PRETTY_FUNCTION__), ##__VA_ARGS__); } while (0)
#else
	#define LOG_I(...)
#endif

#endif // _audiocodec_debug_h_
