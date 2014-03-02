/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMUTILS_INL
#define STECAMUTILS_INL

#include <math.h>
#include "STECamTrace.h"

template <typename Type>
/* static */ inline void Align<Type>::up(Type& type, const uint32_t alignment)
    {

    //sanity check
    DBGT_ASSERT(0 != alignment, "Alignment zero");

    //get remainder
    Type rem = type%alignment;

    //Check alignment needed
    if(0 != rem)
        type += (alignment - rem);

    //check aligment done
    check(type, alignment);
    }

template <typename Type>
/* static */ inline void Align<Type>::down(Type& type, const uint32_t alignment)
    {
    //sanity check
    DBGT_ASSERT(0 != alignment, "Alignment zero");

    //get remainder
    Type rem = type%alignment;

    //Check alignment needed
    if(0 != rem)
        type -= rem;

    //check aligment done
    check(type, alignment);
    }

template <typename Type>
/* static */ inline void Align<Type>::check(const Type& type, const uint32_t alignment)
{
    if(0 != type%alignment) {
		DBGT_CRITICAL("Not aligned alignment - %d", alignment);
	}
}

template <typename Type>
/* static */ inline void AlignPow2<Type>::up(Type& type, const uint32_t alignment)
{
    //check power of 2
    check(alignment);

    type = (type + alignment - 1) & ~(alignment - 1);

    //check aligment done
    Align<Type>::check(type, alignment);
}

template <typename Type>
/* static */ inline void AlignPow2<Type>::down(Type& type, const uint32_t alignment)
{
    //check power of 2
    check(alignment);

    type = type & ~(alignment - 1);

    //check aligment done
    Align<Type>::check(type, alignment);
}

template <typename Type>
/* static */ inline void AlignPow2<Type>::check(const uint32_t alignment)
{
    DBGT_ASSERT( 0 == (alignment & (alignment-1)), "alignment is not power of 2");
}

inline bool Comparef(float temp1, float temp2)
{
  float prec = 0.0001;
  if (((temp1 - prec) < temp2) && ((temp1 + prec) > temp2))
    return true;
  else
    return false;
}

inline uint32_t extractHexValue(const char * const aStr)
{
    DBGT_PROLOG("String: %s", aStr);

    DBGT_ASSERT(NULL != aStr, "Str is NULL");

    //convert to value
    uint32_t value = 0;
    int32_t len = strlen(aStr);
    uint32_t index = 0;

    //format 0xYYYYYYYY
    while(len-- > 0){
        char temp;
        value *= 16;
        if(isdigit(aStr[index]))
            value += (aStr[index] - '0');
        else{
            temp = tolower(aStr[index]);
            if('x' != temp){
				if(temp <= 'a' || temp >= 'g') {
					DBGT_CRITICAL("Dup failed with error: %s", strerror(errno));
				}
                temp = (temp - 'a' + 10);
                value += temp;
            }
        }

        //increment to next character
        index++;
    };

	DBGT_EPILOG("value - %d", value);
	return value;
}

inline void appendTimeoutValue(uint32_t aTimeoutMilliSeconds, struct timespec& aTimeSpec)
{
    DBGT_PROLOG("TimeoutMilliseconds: %u Currenttime Sec: %ld NSec: %ld",
        aTimeoutMilliSeconds,
        aTimeSpec.tv_sec,
        aTimeSpec.tv_nsec);

    const int msToSecFactor = 1000;

    //find out number of seconds
    int numSeconds = aTimeoutMilliSeconds/msToSecFactor;
    DBGT_PTRACE("Num seconds: %d", numSeconds);

    //append seconds
    aTimeSpec.tv_sec += numSeconds;

    //rem ms
    int numMS = aTimeoutMilliSeconds % msToSecFactor;
    //convert to seconds
    numMS *= 1000*1000;
    DBGT_PTRACE("Num ms: %d", numMS);

    //check if ns causing overflow
    if(aTimeSpec.tv_nsec + numMS > 1*1000*1000*1000)
    {
        //inc seconds
        aTimeSpec.tv_sec++;
        //rem ms
        aTimeSpec.tv_nsec = aTimeSpec.tv_nsec + numMS - 1*1000*1000*1000;
    }
    else
    {
    //appen MS
    aTimeSpec.tv_nsec += numMS;
    }

    DBGT_EPILOG("New Sec: %ld NSec: %ld", aTimeSpec.tv_sec, aTimeSpec.tv_nsec);
}

inline OMX_COLOR_FORMATTYPE getOmxPixFmtFromKeyStr( char* pixFmt )
{
    if( strcmp("yuv420sp",pixFmt)==0 ){
        return (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420SemiPlanar;
    }

    if( strcmp("yuv420p",pixFmt)==0 ){
        return (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420Planar;
    }

    if( strcmp("yuv420mb",pixFmt)==0 ){
        return (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
    }

    if( strcmp("yuv420swmb",pixFmt)==0 ){
        return (OMX_COLOR_FORMATTYPE)OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar;
    }

    return (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
}

inline void getKeyStrFromOmxPixFmt( char* pixFmt, OMX_COLOR_FORMATTYPE omxPixFmt )
{
    switch((uint32_t)omxPixFmt){
        case OMX_COLOR_FormatYUV420SemiPlanar:
            strcpy(pixFmt,"yuv420sp");
        break;
        case OMX_COLOR_FormatYUV420Planar:
            strcpy(pixFmt,"yuv420p");
        break;
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
        case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
            strcpy(pixFmt,"yuv420mb");
        break;
        default:
            // default pixel format
            strcpy(pixFmt,"yuv420mb");
        break;
    }
}

inline int camera_sem_wait_l(sem_t* sem, int sem_timeout /* in us */)
{
    DBGT_PROLOG("IN camera_sem_wait: sem @ %p", sem);

    int val, retval;
    struct timespec ts;
    struct timeval tv;

    // get current time
    gettimeofday(&tv, NULL);

    ts.tv_sec = tv.tv_sec;

    if ((tv.tv_usec + sem_timeout) >= (long)pow(10,6)) //Convert us into s
    {
        ldiv_t divresult;
        divresult = ldiv((tv.tv_usec + sem_timeout), (long)pow(10,6));
        ts.tv_sec += divresult.quot;
        ts.tv_nsec = divresult.rem * 1000;
    }else{
        ts.tv_nsec =((long)tv.tv_usec + (long)sem_timeout) * 1000;
    }

    retval = sem_timedwait(sem, &ts);

    DBGT_EPILOG("OUT camera_sem_wait: sem @ %p", sem);
    return retval;
}

#endif // STECAMUTILS_INL
