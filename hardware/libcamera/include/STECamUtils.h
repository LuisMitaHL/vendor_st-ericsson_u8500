/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMUTILS_H
#define STECAMUTILS_H

//Internal Includes
#include "STECamTrace.h"

#if defined(CAM_PRINT_API_PARAM) || defined(CAM_DEBUG)
#undef CAM_PRINT_API_PARAM
#define CAM_PRINT_API_PARAM
#endif //CAM_PRINT_API_PARAM || CAM_DEBUG

#ifdef CAM_PRINT_API_PARAM

//System Includes
#include <utils/Timers.h>
#include <sys/resource.h>
#include <ctype.h>
#include <semaphore.h>

#include <IFM_Types.h>
#include <IFM_Index.h>
#include <OMX_Symbian_ComponentExt_Ste.h>

#define SEM_WAIT_TIMEOUT 2000000 //us

#endif //CAM_PRINT_API_PARAM

/** Counts the number of items in a static array */
#define ARRAYCOUNT(a)  static_cast<int>(sizeof(a)/sizeof((a)[0]))
/** Counts the number of meaningful items in a zero-terminated array */
#define ZARRAYCOUNT(a) (ARRAYCOUNT(a)-1)

template <typename Type>
class Align
    {
    public:
        /* Align Up */
        static inline void up(Type& type, const uint32_t alignment);

        /* Align down */
        static inline void down(Type& type, const uint32_t alignment);

        /* Check aligment */
        static inline void check(const Type& type, const uint32_t alignment);
    };

template <typename Type>
class AlignPow2
    {
    public:
        /* Align Up */
        static inline void up(Type& type, const uint32_t alignment);

        /* Align down */
        static inline void down(Type& type, const uint32_t alignment);

    private:
        /* Check requested alignment power of 2*/
        static inline void check(const uint32_t alignment);
    };

//utility function to compare floats
inline bool Comparef(float, float);

//extract HEX value
inline uint32_t extractHexValue(const char * const aStr);

//append timeout value
inline void appendTimeoutValue(uint32_t aTimeoutMilliSeconds, struct timespec& aTimeSpec);

// get omx pixel format from key string
inline OMX_COLOR_FORMATTYPE getOmxPixFmtFromKeyStr( char* pixFmt );

// get key string from omx pixel format
inline void getKeyStrFromOmxPixFmt( char* pixFmt, OMX_COLOR_FORMATTYPE omxPixFmt );

// sem wait with timeout
#define camera_sem_wait(sem,sem_timeout){                                 \
    int retval = camera_sem_wait_l(sem,sem_timeout);                      \
    DBGT_ASSERT(retval == 0,                                              \
    "!!! sem_timedwait failed for %s !!!: %s for sem @ %p at %s, %s:%d",  \
    #sem, strerror(errno), sem, __FUNCTION__, __FILE__, __LINE__);        \
}

#include "STECamUtils.inl"

#endif // STECAMUTILS_H

