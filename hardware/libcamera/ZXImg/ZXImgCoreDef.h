////////////////////////////////////////////////////////////////////////////////
// Author:          Sicong Zhang
// Description:     Definition of constants, data structure, macro
// Features:        replacement for <stdint.h> of C99 standard
//                  Almost compatible with <stdint.h>.
//                  Unlike <stdint.h>, <ZXDataDef.h> doesn't check __STDC_LIMIT_MACROS and
//                  __STDC_CONSTANT_MACROS. Macros will be defined always even in C++.
//                  Portable
//                  Simple
// Create:          2008-08-22
// Update:          2011-08-25
// Copyright:       Zhang Sicong
//                  All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#ifndef __ZXIMGCOREDEF_H__
#define __ZXIMGCOREDEF_H__

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#define	USE_ANDROID_NEON	1
#if !_WIN32
#define	USE_ANDROID_ASM	USE_ANDROID_NEON
#else
#define	USE_ANDROID_ASM	0
#endif

/////////define data type////////////////////
#if _WIN32
    typedef signed char        int8_t;
    typedef short              int16_t;
    typedef int                int32_t;
//    typedef long long          int64_t;
    typedef unsigned char      uint8_t;
    typedef unsigned short     uint16_t;
    typedef unsigned int       uint32_t;
//    typedef unsigned long long uint64_t;
	typedef	float				float_t;
	typedef	double				double_t;
#else//android
	//typedef signed char        int8_t;
  //  typedef short              int16_t;
  //  typedef int                int32_t;
  //  typedef long long          int64_t;
  //  typedef unsigned char      uint8_t;
  //  typedef unsigned short     uint16_t;
  //  typedef unsigned int       uint32_t;
  //  typedef unsigned long long uint64_t;
	typedef	float				float_t;
	typedef	double				double_t;
#endif
    
////////////////////////////////////////////////////////////////////////////////
// Const value definition
#define ZXCORE_MAX( x, y )              (((x) > (y)) ? (x) : (y))
#define ZXCORE_MIN( x, y )              (((x) < (y)) ? (x) : (y))
#define ZXCORE_ABS( x )                 (((x) > 0) ? (x) : (-(x)))
#define ZXCORE_CLIP(x, LOW, HIGH)       (((x) < (LOW)) ? (LOW) : (((x) > (HIGH)) ? (HIGH) : (x)))
#define ZXCORE_ALIGN_4(x)               ((((x) + 3) >> 2) << 2)
#define ZXCORE_EPS_16					0.0000152587890625f

#define	GF_B_SCALE	16
#define	GF_A_SCALE	64
#define	GF_B_SHIFT	256

//threads control
typedef	struct	tagThreadParam
{
	int32_t	threads_cnt;
	int32_t	thread_id;
	int32_t	thread_flag;
	int32_t	*p_others_flag;
	uint8_t*	p_src;
	uint8_t*	p_dst;
	void		*pFilter;
}ThreadParam;

#if _WIN32
#include	<windows.h>
#include	<process.h>
#define	LOGE	printf
#define	pthread_t	uintptr_t
#define	CREATE_THREAD(th_id,fun,para)	th_id =_beginthread(fun,0,para)
#define	WAIT_THREAD(th_id0,th_id1,flag0,flag1,EXIT_FLAG)			\
		while(flag0!=EXIT_FLAG || flag1!=EXIT_FLAG)					\
			Sleep(10);												\
			Sleep(10);
#define	SLEEP	Sleep
#else
#include	<pthread.h>
#include	<unistd.h>
#include	<sys/time.h>
#include	"android/log.h"
#define	LOG_TAG	"TEST"
#define	LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define	LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define	LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define MEASURE_TIME_START()		gettimeofday(&old___, NULL)
#define MEASURE_TIME_END(str)		do { 				\
    long  time1, time2;									\
    gettimeofday(&now___, NULL);						\
    time1 = old___.tv_sec*1000000+old___.tv_usec;		\
    time2 = now___.tv_sec*1000000+now___.tv_usec;		\
    LOGD("%s (time:%d)", str, time2-time1);				\
} while(0);
#define	CREATE_THREAD(th_id,fun,para)	pthread_create(&th_id,NULL,fun,para)
#define	WAIT_THREAD(th_id0,th_id1,flag0,flag1,EXIT_FLAG)\
		if(th_id0){								\
			pthread_join(th_id0,NULL);					\
		}												\
		if(th_id1){								\
			pthread_join(th_id1,NULL);					\
		}
#define	SLEEP	sleep
#endif

#endif //__ZX_DATA_DEF_H__