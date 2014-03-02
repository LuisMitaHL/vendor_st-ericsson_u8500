/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shmin.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <bindings/shmpcm/shmin.nmf>
#include <archi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dbc.h>
#include "fsm/component/include/Component.inl"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_mpc_bindings_shmpcm_shmin_src_shminTraces.h"
#endif


#define IN  0
#define OUT 1

#define MAX_NB_BUFFERS 4


#define Port_dequeueAndReturnBuffer_Sleep(a)   { preventSleep(); Port_dequeueAndReturnBuffer(a); }

////////////////////////////////////////////////////////////////////////
//					Global Variables
////////////////////////////////////////////////////////////////////////
static ShmPcmConfig_t   mShmConfig;

static void (*Shmin_copy_downmix_routine) (int *in, int *out, int size);
static void (*Shmin_copy_upmix_routine)   (int *in, int *out, int size);
static void (*Shmin_copy_routine)         (int *in, int *out, int size);
static int  size_in_shift;

static Buffer_t         mBufOut;

static void *           mFifoIn[MAX_NB_BUFFERS];
static void *           mFifoOut[1];
static Port             mPorts[2];
static Component        mShmIn;

static t_uint16         mDataConsumed;

static bool             mFsmInitialized = false;
static Buffer_p         mPendingBuffer[MAX_NB_BUFFERS] = {0,0,0,0};
// AV synchronisation
static bool             mTimeStampDetected;
static bool             mSendNewFormat;

// Synchronized ports
static bool             mInputBufferAlreadyReceived;

// Low power audio: Send an empty buffer to ARM directly at wakeup
// to start ARM processing and thereby sync ARM and DSP activities.
//
// Low power audio sync mode is active if mShmConfig.hostMpcSyncEnabled
// is true
//
static bool             mWasSleeping;  // TRUE if we left sleep state and have not acted upon this
static int              mBufsSaved;    // Number of saved buffers (kept in HostSync part)
static t_sleepNotifyCallback mSleepCallBack;
static int              mRecentlyWokeUp;

#define MAXITERCOUNT 512    // 2^n such as 2^n<=1023 (if MAXITERCOUNT=1023, compiler adds a divide which is not optimized)

////////////////////////////////////////////////////////////////////////

static void copy_downmix_in32b_out16b_swap(int *in, int *out, int size) 
{
    int i, n;
    int left_h, left_l, right_h, right_l;
    
    size /= 4;
    if(size > 0)
    {
        // left  input sample = 0xAABBCCDD
        // right input sample = 0xEEFFGGHH
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                left_h  = *in++;                                                // 0x??BBAA
                left_l  = *in++;                                                // 0x??DDCC
                right_h = *in++;                                                // 0x??FFEE
                right_l = *in++;                                                // 0x??HHGG
                left_h  = winsertu(left_h,  0x0810, left_h);                    // 0xAABBAA
                left_h  = winsertu(left_l,  0x0800, left_h);                    // 0xAABBCC
                right_h = winsertu(right_h, 0x0810, right_h);                   // 0xEEFFEE
                right_h = winsertu(right_l, 0x0800, right_h);                   // 0xEEFFGG
                *out++  = waddsat(waddsat(left_h >> 1, right_h >> 1), 0x80) >> 8;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            left_h  = *in++;                                                    // 0x??BBAA
            left_l  = *in++;                                                    // 0x??DDCC
            right_h = *in++;                                                    // 0x??FFEE
            right_l = *in++;                                                    // 0x??HHGG
            left_h  = winsertu(left_h,  0x0810, left_h);                        // 0xAABBAA
            left_h  = winsertu(left_l,  0x0800, left_h);                        // 0xAABBCC
            right_h = winsertu(right_h, 0x0810, right_h);                       // 0xEEFFEE
            right_h = winsertu(right_l, 0x0800, right_h);                       // 0xEEFFGG
            *out++  = waddsat(waddsat(left_h >> 1, right_h >> 1), 0x80) >> 8;
        }
    }
}

static void copy_downmix_in32b_out24b_swap(int *in, int *out, int size) 
{
    int       i, n;
    int       left_h, left_l, right_h, right_l;
    long long tmp;
    
    size /= 4;
    if(size > 0)
    {
        // left  input sample = 0xAABBCCDD
        // right input sample = 0xEEFFGGHH
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                left_h  = *in++;                                                // 0x??BBAA
                left_l  = *in++;                                                // 0x??DDCC
                right_h = *in++;                                                // 0x??FFEE
                right_l = *in++;                                                // 0x??HHGG
                left_h  = winsertu (left_h,  0x0810, left_h);                   // 0xAABBAA
                left_h  = winsertu (left_l,  0x0800, left_h);                   // 0xAABBCC
                right_h = winsertu (right_h, 0x0810, right_h);                  // 0xEEFFEE
                right_h = winsertu (right_l, 0x0800, right_h);                  // 0xEEFFGG
                left_l  = wextractu(left_l,  0x0808);                           // 0x0000DD
                right_l = wextractu(right_l, 0x0808);                           // 0x0000HH
                tmp     = ((wX_depmsp(left_h) + wX_depmsp(right_h)) >> 1) + (wX_deplsp(left_l + right_l) << 15);
                *out++  = wround_X(tmp);
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            left_h  = *in++;                                                    // 0x??BBAA
            left_l  = *in++;                                                    // 0x??DDCC
            right_h = *in++;                                                    // 0x??FFEE
            right_l = *in++;                                                    // 0x??HHGG
            left_h  = winsertu (left_h,  0x0810, left_h);                       // 0xAABBAA
            left_h  = winsertu (left_l,  0x0800, left_h);                       // 0xAABBCC
            right_h = winsertu (right_h, 0x0810, right_h);                      // 0xEEFFEE
            right_h = winsertu (right_l, 0x0800, right_h);                      // 0xEEFFGG
            left_l  = wextractu(left_l,  0x0808);                               // 0x0000DD
            right_l = wextractu(right_l, 0x0808);                               // 0x0000HH
            tmp     = ((wX_depmsp(left_h) + wX_depmsp(right_h)) >> 1) + (wX_deplsp(left_l + right_l) << 15);
            *out++  = wround_X(tmp);
        }
    }
}

static void copy_downmix_in32b_out16b_noswap(int *in, int *out, int size) 
{
    int i, n;
    int left_h, left_l, right_h, right_l;
    
    size /= 4;
    if(size > 0)
    {
        // left  input sample = 0xAABBCCDD
        // right input sample = 0xEEFFGGHH
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                left_l  = *in++;                                                // 0x??CCDD
                left_h  = *in++;                                                // 0x??AABB
                right_l = *in++;                                                // 0x??GGHH
                right_h = *in++;                                                // 0x??EEFF
                left_h  = wextract (left_h,  0x1000);                           // 0xssAABB
                right_h = wextract (right_h, 0x1000);                           // 0xssEEFF
                left_l  = wextractu(left_l,  0x1000);                           // 0x00CCDD
                right_l = wextractu(right_l, 0x1000);                           // 0x00GGHH
                *out++ = waddsat(waddsat(waddsat(left_h, right_h), waddsat(left_l, right_l) >> 16), 1) >> 1;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            left_l  = *in++;                                                    // 0x??CCDD
            left_h  = *in++;                                                    // 0x??AABB
            right_l = *in++;                                                    // 0x??GGHH
            right_h = *in++;                                                    // 0x??EEFF
            left_h  = wextract (left_h,  0x1000);                               // 0xssAABB
            right_h = wextract (right_h, 0x1000);                               // 0xssEEFF
            left_l  = wextractu(left_l,  0x1000);                               // 0x00CCDD
            right_l = wextractu(right_l, 0x1000);                               // 0x00GGHH
            *out++ = waddsat(waddsat(waddsat(left_h, right_h), waddsat(left_l, right_l) >> 16), 1) >> 1;
        }
    }
}

static void copy_downmix_in32b_out24b_noswap(int *in, int *out, int size) 
{
    int       i, n;
    int       left_h, left_l, right_h, right_l;
    long long tmp;
    
    size /= 4;
    if(size > 0)
    {
        // left  input sample = 0xAABBCCDD
        // right input sample = 0xEEFFGGHH
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                left_l  = *in++;                                                // 0x??CCDD
                left_h  = *in++;                                                // 0x??AABB
                right_l = *in++;                                                // 0x??GGHH
                right_h = *in++;                                                // 0x??EEFF
                left_h  = wextract (left_h,  0x1000);                           // 0xssAABB
                right_h = wextract (right_h, 0x1000);                           // 0xssEEFF
                left_l  = wextractu(left_l,  0x1000);                           // 0x00CCDD
                right_l = wextractu(right_l, 0x1000);                           // 0x00GGHH
                tmp     = (wX_depmsp(left_h + right_h) << 7) + (wX_deplsp(left_l + right_l) << 15);
                *out++  = wround_X(tmp);
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            left_l  = *in++;                                                    // 0x??CCDD
            left_h  = *in++;                                                    // 0x??AABB
            right_l = *in++;                                                    // 0x??GGHH
            right_h = *in++;                                                    // 0x??EEFF
            left_h  = wextract (left_h,  0x1000);                               // 0xssAABB
            right_h = wextract (right_h, 0x1000);                               // 0xssEEFF
            left_l  = wextractu(left_l,  0x1000);                               // 0x00CCDD
            right_l = wextractu(right_l, 0x1000);                               // 0x00GGHH
            tmp     = (wX_depmsp(left_h + right_h) << 7) + (wX_deplsp(left_l + right_l) << 15);
            *out++  = wround_X(tmp);
        }
    }
}

static void copy_upmix_in32b_out16b_swap(int *in, int *out, int size) 
{
    int i, n;
    int tmp_h, tmp_l;
    
    size /= 2;
    if(size > 0)
    {
        // input sample = 0xAABBCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp_h = *in++;                                                  // 0x??BBAA
                tmp_l = *in++;                                                  // 0x??DDCC
                tmp_h = winsertu(tmp_h, 0x0810, tmp_h);                         // 0xAABBAA
                tmp_h = winsertu(tmp_l, 0x0800, tmp_h);                         // 0xAABBCC
                *out  = *(out + 1) = waddsat(tmp_h, 0x80) >> 8;
                out  += 2;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp_h = *in++;                                                      // 0x??BBAA
            tmp_l = *in++;                                                      // 0x??DDCC
            tmp_h = winsertu(tmp_h, 0x0810, tmp_h);                             // 0xAABBAA
            tmp_h = winsertu(tmp_l, 0x0800, tmp_h);                             // 0xAABBCC
            *out  = *(out + 1) = waddsat(tmp_h, 0x80) >> 8;
            out  += 2;
        }
    }
}

static void copy_upmix_in32b_out24b_swap(int *in, int *out, int size) 
{
    int       i, n;
    int       tmp_h, tmp_l;
    long long tmp;
    
    size /= 2;
    if(size > 0)
    {
        // input sample = 0xAABBCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp_h = *in++;                                                  // 0x??BBAA
                tmp_l = *in++;                                                  // 0x??DDCC
                tmp_h = winsertu (tmp_h, 0x0810, tmp_h);                        // 0xAABBAA
                tmp_h = winsertu (tmp_l, 0x0800, tmp_h);                        // 0xAABBCC
                tmp_l = wextractu(tmp_l, 0x0808);                               // 0x0000DD
                tmp   = wX_depmsp(tmp_h) + (wX_deplsp(tmp_l) << 16);            // 0xAABBCCDD0000
                *out  = *(out + 1) = wround_X(tmp);
                out  += 2;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp_h = *in++;                                                      // 0x??BBAA
            tmp_l = *in++;                                                      // 0x??DDCC
            tmp_h = winsertu (tmp_h, 0x0810, tmp_h);                            // 0xAABBAA
            tmp_h = winsertu (tmp_l, 0x0800, tmp_h);                            // 0xAABBCC
            tmp_l = wextractu(tmp_l, 0x0808);                                   // 0x0000DD
            tmp   = wX_depmsp(tmp_h) + (wX_deplsp(tmp_l) << 16);                // 0xAABBCCDD0000
            *out  = *(out + 1) = wround_X(tmp);
            out  += 2;
        }
    }
}

static void copy_upmix_in32b_out16b_noswap(int *in, int *out, int size) 
{
    int i, n;
    int tmp_h, tmp_l;
    
    size /= 2;
    if(size > 0)
    {
        // input sample = 0xAABBCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp_l = *in++;                                                  // 0x??CCDD
                tmp_h = *in++;                                                  // 0x??AABB
                tmp_h = (tmp_h << 8);                                           // 0xAABB00
                tmp_l = wextractu(tmp_l, 0x0808);                               // 0x0000CC
                *out  = *(out + 1) = (waddsat(waddsat(tmp_h, tmp_l), 0x80) >> 8);
                out  += 2;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp_l = *in++;                                                      // 0x??CCDD
            tmp_h = *in++;                                                      // 0x??AABB
            tmp_h = (tmp_h << 8);                                               // 0xAABB00
            tmp_l = wextractu(tmp_l, 0x0808);                                   // 0x0000CC
            *out  = *(out + 1) = (waddsat(waddsat(tmp_h, tmp_l), 0x80) >> 8);
            out  += 2;
        }
    }
}

static void copy_upmix_in32b_out24b_noswap(int *in, int *out, int size) 
{
    int       i, n;
    int       tmp_h, tmp_l;
    long long tmp;
    
    size /= 2;
    if(size > 0)
    {
        // input sample = 0xAABBCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp_l = *in++;                                                  // 0x??CCDD
                tmp_h = *in++;                                                  // 0x??AABB
                tmp_h = (tmp_h << 8);                                           // 0xAABB00
                tmp_l = wextractu(tmp_l, 0x1000);                               // 0x00CCDD
                tmp   = wX_depmsp(tmp_h) + (wX_deplsp(tmp_l) << 16);            // 0xAABBCCDD0000
                *out  = *(out + 1) = wround_X(tmp);
                out  += 2;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp_l = *in++;                                                      // 0x??CCDD
            tmp_h = *in++;                                                      // 0x??AABB
            tmp_h = (tmp_h << 8);                                               // 0xAABB00
            tmp_l = wextractu(tmp_l, 0x1000);                                   // 0x00CCDD
            tmp   = wX_depmsp(tmp_h) + (wX_deplsp(tmp_l) << 16);                // 0xAABBCCDD0000
            *out  = *(out + 1) = wround_X(tmp);
            out  += 2;
        }
    }
}

static void copy_in32b_out16b_swap(int *in, int *out, int size) 
{
    int i, n;
    int tmp_h, tmp_l;
    
    size /= 2;
    if(size > 0)
    {
        // input sample = 0xAABBCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp_h  = *in++;                                                 // 0x??BBAA
                tmp_l  = *in++;                                                 // 0x??DDCC
                tmp_h  = winsertu(tmp_h, 0x0810, tmp_h);                        // 0xAABBAA
                tmp_h  = winsertu(tmp_l, 0x0800, tmp_h);                        // 0xAABBCC
                *out++ = waddsat(tmp_h, 0x80) >> 8;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp_h  = *in++;                                                     // 0x??BBAA
            tmp_l  = *in++;                                                     // 0x??DDCC
            tmp_h  = winsertu(tmp_h, 0x0810, tmp_h);                            // 0xAABBAA
            tmp_h  = winsertu(tmp_l, 0x0800, tmp_h);                            // 0xAABBCC
            *out++ = waddsat(tmp_h, 0x80) >> 8;
        }
    }
}

static void copy_in32b_out24b_swap(int *in, int *out, int size) 
{
    int       i, n;
    int       tmp_h, tmp_l;
    long long tmp;
    
    size /= 2;
    if(size > 0)
    {
        // input sample = 0xAABBCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp_h  = *in++;                                                 // 0x??BBAA
                tmp_l  = *in++;                                                 // 0x??DDCC
                tmp_h  = winsertu (tmp_h, 0x0810, tmp_h);                       // 0xAABBAA
                tmp_h  = winsertu (tmp_l, 0x0800, tmp_h);                       // 0xAABBCC
                tmp_l  = wextractu(tmp_l, 0x0808);                              // 0x0000DD
                tmp    = wX_depmsp(tmp_h) + (wX_deplsp(tmp_l) << 16);           // 0xAABBCCDD0000
                *out++ = wround_X(tmp);
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp_h  = *in++;                                                     // 0x??BBAA
            tmp_l  = *in++;                                                     // 0x??DDCC
            tmp_h  = winsertu (tmp_h, 0x0810, tmp_h);                           // 0xAABBAA
            tmp_h  = winsertu (tmp_l, 0x0800, tmp_h);                           // 0xAABBCC
            tmp_l  = wextractu(tmp_l, 0x0808);                                  // 0x0000DD
            tmp    = wX_depmsp(tmp_h) + (wX_deplsp(tmp_l) << 16);               // 0xAABBCCDD0000
            *out++ = wround_X(tmp);
        }
    }
}

static void copy_in32b_out16b_noswap(int *in, int *out, int size) 
{
    int i, n;
    int tmp_h, tmp_l;
    
    size /= 2;
    if(size > 0)
    {
        // input sample = 0xAABBCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp_l  = *in++;                                                 // 0x??CCDD
                tmp_h  = *in++;                                                 // 0x??AABB
                tmp_h  = (tmp_h << 8);                                          // 0xAABB00
                tmp_l  = wextractu(tmp_l, 0x0808);                              // 0x0000CC
                *out++ = (waddsat(waddsat(tmp_h, tmp_l), 0x80) >> 8);
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp_l  = *in++;                                                     // 0x??CCDD
            tmp_h  = *in++;                                                     // 0x??AABB
            tmp_h  = (tmp_h << 8);                                              // 0xAABB00
            tmp_l  = wextractu(tmp_l, 0x0808);                                  // 0x0000CC
            *out++ = (waddsat(waddsat(tmp_h, tmp_l), 0x80) >> 8);
        }
    }
}

static void copy_in32b_out24b_noswap(int *in, int *out, int size) 
{
    int       i, n;
    int       tmp_h, tmp_l;
    long long tmp;
    
    size /= 2;
    if(size > 0)
    {
        // input sample = 0xAABBCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp_l  = *in++;                                                 // 0x??CCDD
                tmp_h  = *in++;                                                 // 0x??AABB
                tmp_h  = (tmp_h << 8);                                          // 0xAABB00
                tmp_l  = wextractu(tmp_l, 0x1000);                              // 0x00CCDD
                tmp    = wX_depmsp(tmp_h) + (wX_deplsp(tmp_l) << 16);           // 0xAABBCCDD0000
                *out++ = wround_X(tmp);
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp_l  = *in++;                                                     // 0x??CCDD
            tmp_h  = *in++;                                                     // 0x??AABB
            tmp_h  = (tmp_h << 8);                                              // 0xAABB00
            tmp_l  = wextractu(tmp_l, 0x1000);                                  // 0x00CCDD
            tmp    = wX_depmsp(tmp_h) + (wX_deplsp(tmp_l) << 16);               // 0xAABBCCDD0000
            *out++ = wround_X(tmp);
        }
    }
}

static void copy_downmix_in16b_out16b_swap(int *in, int *out, int size) 
{
    int i, n;
    int left, right;

    size /= 2;
    if(size > 0)
    {
        // left  input sample = 0xAABB
        // right input sample = 0xCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                left   = *in++;                                                 // 0x??BBAA
                right  = *in++;                                                 // 0x??DDCC
                left   = winsertu(left,  0x0810, left);                         // 0xAABBAA
                right  = winsertu(right, 0x0810, right);                        // 0xCCDDCC
                left   = wextract(left,  0x1008);                               // 0xssAABB
                right  = wextract(right, 0x1008);                               // 0xssCCDD
                *out++ = (left + right + 1) >> 1;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            left   = *in++;                                                     // 0x??BBAA
            right  = *in++;                                                     // 0x??DDCC
            left   = winsertu(left,  0x0810, left);                             // 0xAABBAA
            right  = winsertu(right, 0x0810, right);                            // 0xCCDDCC
            left   = wextract(left,  0x1008);                                   // 0xssAABB
            right  = wextract(right, 0x1008);                                   // 0xssCCDD
            *out++ = (left + right + 1) >> 1;
        }
    }
}

static void copy_downmix_in16b_out24b_swap(int *in, int *out, int size) 
{
    int i, n;
    int left, right;

    size /= 2;
    if(size > 0)
    {
        // left  input sample = 0xAABB
        // right input sample = 0xCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                left   = *in++;                                                 // 0x??BBAA
                right  = *in++;                                                 // 0x??DDCC
                left   = winsertu(left,  0x0810, left);                         // 0xAABBAA
                right  = winsertu(right, 0x0810, right);                        // 0xCCDDCC
                left   = wextract(left,  0x1008);                               // 0xssAABB
                right  = wextract(right, 0x1008);                               // 0xssCCDD
                *out++ = (left + right) << 7;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            left   = *in++;                                                     // 0x??BBAA
            right  = *in++;                                                     // 0x??DDCC
            left   = winsertu(left,  0x0810, left);                             // 0xAABBAA
            right  = winsertu(right, 0x0810, right);                            // 0xCCDDCC
            left   = wextract(left,  0x1008);                                   // 0xssAABB
            right  = wextract(right, 0x1008);                                   // 0xssCCDD
            *out++ = (left + right) << 7;
        }
    }
}

static void copy_downmix_in16b_out16b_noswap(int *in, int *out, int size) 
{
    int i, n;
    int left, right;

    size /= 2;
    if(size > 0)
    {
        // left  input sample = 0xAABB
        // right input sample = 0xCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                left   = wsgn16(*in++);                                         // 0xssAABB
                right  = wsgn16(*in++);                                         // 0xssCCDD
                *out++ = (left + right + 1) >> 1;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            left   = wsgn16(*in++);                                             // 0xssAABB
            right  = wsgn16(*in++);                                             // 0xssCCDD
            *out++ = (left + right + 1) >> 1;
        }
    }
}

static void copy_downmix_in16b_out24b_noswap(int *in, int *out, int size) 
{
    int i, n;
    int left, right;

    size /= 2;
    if(size > 0)
    {
        // left  input sample = 0xAABB
        // right input sample = 0xCCDD
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                left   = wsgn16(*in++);                                         // 0xssAABB
                right  = wsgn16(*in++);                                         // 0xssCCDD
                *out++ = (left + right) << 7;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            left   = wsgn16(*in++);                                             // 0xssAABB
            right  = wsgn16(*in++);                                             // 0xssCCDD
            *out++ = (left + right) << 7;
        }
    }
}

static void copy_upmix_in16b_out16b_swap(int *in, int *out, int size) 
{
    int i, n;
    int tmp;
    
    if(size > 0)
    {
        // input sample = 0xAABB
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp  = *in++;                                                   // 0x??BBAA
                tmp  = winsertu(tmp, 0x0810, tmp);                              // 0xAABBAA
                tmp  = wextract(tmp, 0x1008);                                   // 0xssAABB
                *out = *(out + 1) = tmp;
                out += 2;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp  = *in++;                                                       // 0x??BBAA
            tmp  = winsertu(tmp, 0x0810, tmp);                                  // 0xAABBAA
            tmp  = wextract(tmp, 0x1008);                                       // 0xssAABB
            *out = *(out + 1) = tmp;
            out += 2;
        }
    }
}

static void copy_upmix_in16b_out24b_swap(int *in, int *out, int size) 
{
    int i, n;
    int tmp;
    
    if(size > 0)
    {
        // input sample = 0xAABB
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp  = *in++;                                                   // 0x??BBAA
                tmp  = winsertu(tmp, 0x0810, tmp);                              // 0xAABBAA
                tmp  = (tmp & 0xFFFF00);                                        // 0xAABB00
                *out = *(out + 1) = tmp;
                out += 2;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp  = *in++;                                                       // 0x??BBAA
            tmp  = winsertu(tmp, 0x0810, tmp);                                  // 0xAABBAA
            tmp  = (tmp & 0xFFFF00);                                            // 0xAABB00
            *out = *(out + 1) = tmp;
            out += 2;
        }
    }
}

static void copy_upmix_in16b_out16b_noswap(int *in, int *out, int size) 
{
    int i, n;
    
    if(size > 0)
    {
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                *out = *(out + 1) = wsgn16(*in++);
                out += 2;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            *out = *(out + 1) = wsgn16(*in++);
            out += 2;
        }
    }
}

static void copy_upmix_in16b_out24b_noswap(int *in, int *out, int size) 
{
    int i, n;
    
    if(size > 0)
    {
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                *out = *(out + 1) = (*in++ << 8);
                out += 2;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            *out = *(out + 1) = (*in++ << 8);
            out += 2;
        }
    }
}

static void copy_in16b_out16b_swap(int *in, int *out, int size) 
{
    int i, n;
    int tmp;
    
    if(size > 0)
    {
        // input sample = 0xAABB
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp    = *in++;                                                 // 0x??BBAA
                tmp    = winsertu(tmp, 0x0810, tmp);                            // 0xAABBAA
                tmp    = wextract(tmp, 0x1008);                                 // 0xssAABB
                *out++ = tmp;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp    = *in++;                                                     // 0x??BBAA
            tmp    = winsertu(tmp, 0x0810, tmp);                                // 0xAABBAA
            tmp    = wextract(tmp, 0x1008);                                     // 0xssAABB
            *out++ = tmp;
        }
    }
}

static void copy_in16b_out24b_swap(int *in, int *out, int size) 
{
    int i, n;
    int tmp;
    
    if(size > 0)
    {
        // input sample = 0xAABB
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                tmp    = *in++;                                                 // 0x??BBAA
                tmp    = winsertu(tmp, 0x0810, tmp);                            // 0xAABBAA
                tmp    = (tmp & 0xFFFF00);                                      // 0xAABB00
                *out++ = tmp;
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            tmp    = *in++;                                                     // 0x??BBAA
            tmp    = winsertu(tmp, 0x0810, tmp);                                // 0xAABBAA
            tmp    = (tmp & 0xFFFF00);                                          // 0xAABB00
            *out++ = tmp;
        }
    }
}

static void copy_in16b_out16b_noswap(int *in, int *out, int size) 
{
    int i, n;
    
    if(size > 0)
    {
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                *out++ = wsgn16(*in++);
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            *out++ = wsgn16(*in++);
        }
    }
}

static void copy_in16b_out24b_noswap(int *in, int *out, int size) 
{
    int i, n;
    
    if(size > 0)
    {
        #pragma loop maxitercount(1023)
        for(n = size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
        {
            for(i = 0; i < MAXITERCOUNT; i++) 
            {
                *out++ = (*in++ << 8);
            }
        }
        #pragma loop minitercount(1)
        #pragma loop maxitercount(MAXITERCOUNT)
        for(i = 0; i < n; i++) 
        {
            *out++ = (*in++ << 8);
        }
    }
}





// HostSync_dequeueInputBuffer replaces calls to Port_dequeueAndReturnBuffer_Sleep
static void HostSync_dequeueInputBuffer(void)
{
    TRACE_t * this = (TRACE_t *)&mShmIn;

    if (!mShmConfig.hostMpcSyncEnabled) {
        Port_dequeueAndReturnBuffer_Sleep(&mPorts[IN]);
        return;
    }

    // If we don't have a saved buffer, keep this buffer
    // as a saved buffer.
    // If we have a saved buffer, return that one, and out the new
    // one as a saved buffer. Call prevent sleep.
    // If we have no data at all, return all buffers. Otherwise
    // we can't get back all buffers, which complicates use of
    // shared buffers for LPA mode (ultimately to be shared all
    // the way between audio flinger and the DSP).
    /* if (Port_queuedBufferCount(&mPorts[IN]) == 1) {
        // Only one buffer left, so return it.
        preventSleep();
        Port_dequeueAndReturnBuffer(&mPorts[IN]);
        mBufsSaved=0;
        OstTraceFiltInst0(TRACE_DEBUG, "AFM_MPC: shmpcmin HostSync_dequeueInputBuffer - done with last buffer, returning it");
    } else */
    if (mRecentlyWokeUp && mBufsSaved == 0) {
        // We don't have enough saved buffers, so keep this.
        mBufsSaved++;
        OstTraceFiltInst2(TRACE_DEBUG, "AFM_MPC: shmpcmin HostSync_dequeueInputBuffer - keeping buffer. mBufsSaved=%d, mRecentlyWokeUp=%d", mBufsSaved, mRecentlyWokeUp);
    } else {
        // If we already have a saved buffer, we return the current one and
        // the next one conceptually becomes the saved buffer
        preventSleep();
        Port_dequeueAndReturnBuffer(&mPorts[IN]);
        OstTraceFiltInst1(TRACE_DEBUG, "AFM_MPC: shmpcmin HostSync_dequeueInputBuffer - input buffer returned. mBufsSaved=%d", mBufsSaved);
    }

    while (mBufsSaved > 0 && mBufsSaved == Port_queuedBufferCount(&mPorts[IN])) {
        OstTraceFiltInst0(TRACE_DEBUG, "AFM_MPC: shmpcmin HostSync_dequeueInputBuffer - only saved buffers. Returning them");
        preventSleep();
        mBufsSaved--;
        Port_dequeueAndReturnBuffer(&mPorts[IN]);
    }

    if (mRecentlyWokeUp) {
        mRecentlyWokeUp--;
    }
}

// Return the number of full input buffers - i.e., do not count the
// empty ones used for LPA host sync.
static int HostSync_queuedInputBufferCount()
{
    // !mShmConfig.hostMpcSyncEnabled --> mBufsSaved == 0
    ASSERT(mBufsSaved <= Port_queuedBufferCount(&mPorts[IN]));
    return Port_queuedBufferCount(&mPorts[IN]) - mBufsSaved;
}

// Return a full input buffer
static Buffer_p HostSync_getInputBuffer(int index)
{
    // !mShmConfig.hostMpcSyncEnabled --> mBufsSaved == 0
    ASSERT(mBufsSaved <= Port_queuedBufferCount(&mPorts[IN]));
    return Port_getBuffer(&mPorts[IN], mBufsSaved + index);
}

static void HostSync_activateHost()
{
    if (mBufsSaved > 0) {
        TRACE_t * this = (TRACE_t *)&mShmIn;

        preventSleep();
        Port_dequeueAndReturnBuffer(&mPorts[IN]);
        mBufsSaved--;
    }
}


static void ReturnOutputBuffer(Component *this, Buffer_p bufOut)
{
    OstTraceFiltInst3 (TRACE_DEBUG, "AFM_MPC: shmpcmin: ReturnOutputBuffer 0x%x (nFilledLen=%d) (nb queued buf %d)", (unsigned int)bufOut, (unsigned int)bufOut->filledLen, Port_queuedBufferCount(&mPorts[OUT]));
    // telephony latency control
    OstTraceFiltInst3 (TRACE_DEBUG, "AFM_MPC: shmpcmin: latency control (mpc_shmpcmin, output) (timestamp = 0x%x 0x%x 0x%x us)", bufOut->nTimeStamph, (unsigned int)(bufOut->nTimeStampl >> 24), (unsigned int)(bufOut->nTimeStampl & 0xffffffu));
    Port_dequeueAndReturnBuffer(&this->ports[OUT]);
}

void fillBufWithZeros(Buffer_p buf)
{
    int i;
    for (i = buf->filledLen; i < buf->allocLen; i++) { 
        buf->data[i] = 0;
    }
    buf->filledLen = buf->allocLen;
}

void ShmPcmIn_Process(Component *this)
{
    Buffer_p    bufIn, bufOut;
    int         out_offset, available_out_size, remaining_in_size, size_in_eq_out,i;

    OstTraceFiltInst3(TRACE_DEBUG, "AFM_MPC: shmpcmin Process ENTER. hostMpcSyncEnabled=%d mBufsSaved=%d actualBufs=%d", mShmConfig.hostMpcSyncEnabled, mBufsSaved, Port_queuedBufferCount(&this->ports[IN]));

    if (mShmConfig.hostMpcSyncEnabled && mWasSleeping) {
        OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process, was sleeping in LPA mode");
        HostSync_activateHost();
        mWasSleeping = false;
    }

    OstTraceFiltInst2(TRACE_DEBUG, "AFM_MPC: shmpcmin Process isSynchronized=%d, mInputBufferAlreadyReceived=%d", mShmConfig.isSynchronized, mInputBufferAlreadyReceived);

    if ( (mShmConfig.isSynchronized == false)
            &&  mInputBufferAlreadyReceived
	    &&  HostSync_queuedInputBufferCount() == 0
            &&  Port_queuedBufferCount(&this->ports[OUT]) != 0)
    {
        bufOut = Port_getBuffer(&this->ports[OUT], 0);

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_MPC: Shmpcmin::Underflow, output buffer filled with zeros (%d bytes added)", (int)bufOut->filledLen);

        for (i =0; i < bufOut->allocLen; i++) { 
            bufOut->data[i] = 0;
        }

        bufOut->flags = 0;
        bufOut->filledLen = bufOut->allocLen;
        Port_dequeueAndReturnBuffer(&this->ports[OUT]);
        return;
    }

    if (HostSync_queuedInputBufferCount() == 0
            || Port_queuedBufferCount(&this->ports[OUT]) == 0) {
        return;
    }

    mInputBufferAlreadyReceived = true;

    bufIn  = HostSync_getInputBuffer(0);
    bufOut = Port_getBuffer(&this->ports[OUT], 0);

    out_offset         =  (int) bufOut->filledLen;
    available_out_size = ((int) bufOut->allocLen) - out_offset;
    remaining_in_size  = ((int) bufIn->filledLen) - mDataConsumed;

    if (bufIn->flags & BUFFERFLAG_STARTTIME)
    {
        mTimeStampDetected = true;
    }

    // propagate timestamp if needed
    if (mTimeStampDetected)
    {
        if (out_offset) 
        {
            TSpropagation(bufOut, bufIn, mDataConsumed, out_offset, mShmConfig.sampleFreq, mShmConfig.channelsOut);
        }
        else 
        {
            TSpropagation(bufOut, bufIn, mDataConsumed, out_offset, mShmConfig.sampleFreq, mShmConfig.channelsIn);
        }
    }

    size_in_eq_out = (remaining_in_size >> size_in_shift); /* Support of 32b input data, need to calculate output size */
    if (size_in_eq_out == available_out_size)
    {
        (*Shmin_copy_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, remaining_in_size);
        bufOut->filledLen += size_in_eq_out;

        bufOut->flags     |= bufIn->flags;

        if (bufOut->flags & BUFFERFLAG_EOS) {
            OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process eos sent");
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->flags);
        }

        ReturnOutputBuffer(this, bufOut);

        OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process release input buffer");
        HostSync_dequeueInputBuffer();
        mDataConsumed = 0;
    }
    else if (size_in_eq_out < available_out_size) {
        (*Shmin_copy_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, remaining_in_size);
        bufOut->filledLen += size_in_eq_out;

        bufOut->flags     |= bufIn->flags;

        OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process release input buffer");
        HostSync_dequeueInputBuffer();
        mDataConsumed = 0;

        if (bufOut->flags & BUFFERFLAG_EOS) {
            OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process eos sent");
            fillBufWithZeros(bufOut);
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->flags);
            ReturnOutputBuffer(this, bufOut);
        }
        else if (bufOut->flags & BUFFERFLAG_MASK_FOR_DRAIN){
            OstTraceFiltInst1 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process EOS tag (%x) for DRAIN sent",(int)bufOut->flags & BUFFERFLAG_MASK_FOR_DRAIN);
            fillBufWithZeros(bufOut);
            ReturnOutputBuffer(this, bufOut);
        }
        else if (HostSync_queuedInputBufferCount() != 0) {
            Component_scheduleProcessEvent(this);
        }
        else if (mShmConfig.isSynchronized == false) {
            OstTraceFiltInst2(TRACE_DEBUG,"AFM_MPC: Shmpcmin::process Underflow, output buffer filled with zeros (%d/%d)", (int)(bufOut->filledLen), (int)(bufOut->allocLen));
            fillBufWithZeros(bufOut);
            bufOut->flags = 0;             
            ReturnOutputBuffer(this,bufOut);          
        }        
    }
    else if (size_in_eq_out > available_out_size) {
        (*Shmin_copy_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, available_out_size << size_in_shift);
        mDataConsumed += (available_out_size << size_in_shift);

        if (bufIn->flags & BUFFERFLAG_STARTTIME)
        {
            bufOut->flags     |= (bufIn->flags & BUFFERFLAG_STARTTIME);
            bufIn->flags      ^= BUFFERFLAG_STARTTIME;
        }

        bufOut->filledLen += available_out_size;
        ReturnOutputBuffer(this, bufOut);
    }
}

void ShmPcmIn_Process_DownMix(Component *this)
{
    Buffer_p    bufIn, bufOut;
    int         i, out_offset, available_out_size, remaining_in_size, size_in_eq_out;

    OstTraceFiltInst2(TRACE_DEBUG, "AFM_MPC: shmpcmin Process DownMix isSynchronized=%d, mInputBufferAlreadyReceived=%d", mShmConfig.isSynchronized, mInputBufferAlreadyReceived);

    if ( (mShmConfig.isSynchronized == false)
            &&  mInputBufferAlreadyReceived
	    &&  HostSync_queuedInputBufferCount() == 0
            &&  Port_queuedBufferCount(&this->ports[OUT]) != 0)
    {
        bufOut = Port_getBuffer(&this->ports[OUT], 0);

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_MPC: Shmpcmin:: Process DownMix Underflow, output buffer filled with zeros (%d bytes added)", (int)bufOut->filledLen);

        for (i =0; i < bufOut->allocLen; i++) { 
            bufOut->data[i] = 0;
        }

        bufOut->flags = 0;
        bufOut->filledLen = bufOut->allocLen;
        Port_dequeueAndReturnBuffer(&this->ports[OUT]);
        return;
    }

    if (HostSync_queuedInputBufferCount() == 0
            || Port_queuedBufferCount(&this->ports[OUT]) == 0) {
        return;
    }

    mInputBufferAlreadyReceived = true;

    bufIn  = HostSync_getInputBuffer(0);
    bufOut = Port_getBuffer(&this->ports[OUT], 0);

    out_offset         =  (int) bufOut->filledLen;
    available_out_size = ((int) bufOut->allocLen) - out_offset;
    remaining_in_size  = ((int) bufIn->filledLen) - mDataConsumed;

    if (bufIn->flags & BUFFERFLAG_STARTTIME)
    {
        mTimeStampDetected = true;
    }

    // propagate timestamp if needed
    if (mTimeStampDetected)
    {
        if (out_offset)
        {
            TSpropagation(bufOut, bufIn, mDataConsumed, out_offset, mShmConfig.sampleFreq, mShmConfig.channelsOut);
        }
        else
        {
            TSpropagation(bufOut, bufIn, mDataConsumed, out_offset, mShmConfig.sampleFreq, mShmConfig.channelsIn);
        }
    }


    size_in_eq_out = (remaining_in_size >> size_in_shift); /* Support of 32b input data, need to calculate output size */
    if (size_in_eq_out == available_out_size * 2)
    {
        (*Shmin_copy_downmix_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, remaining_in_size);
        bufOut->filledLen += (size_in_eq_out >> 1);

        bufOut->flags     |= bufIn->flags;

        HostSync_dequeueInputBuffer();
        mDataConsumed = 0;

        if (bufOut->flags & BUFFERFLAG_EOS) {
            OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process DownMix eos sent");
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->flags);
        }
        ReturnOutputBuffer(this, bufOut);
    }
    else if (size_in_eq_out < available_out_size * 2) {
        (*Shmin_copy_downmix_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, remaining_in_size);
        bufOut->filledLen += (size_in_eq_out >> 1);

        bufOut->flags     |= bufIn->flags;

        HostSync_dequeueInputBuffer();
        mDataConsumed = 0;

        if (bufOut->flags & BUFFERFLAG_EOS) {
            OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process DownMix eos sent");
            fillBufWithZeros(bufOut);
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->flags);
            ReturnOutputBuffer(this, bufOut);
        }
        else if (bufOut->flags & BUFFERFLAG_MASK_FOR_DRAIN) {
            OstTraceFiltInst1 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process DownMix EOS tag (%x) for DRAIN sent", (int)bufOut->flags & BUFFERFLAG_MASK_FOR_DRAIN);
            fillBufWithZeros(bufOut);
            ReturnOutputBuffer(this, bufOut);
        }
        else if (HostSync_queuedInputBufferCount() != 0) {
            Component_scheduleProcessEvent(this);
        }
        else if (mShmConfig.isSynchronized == false) {
            OstTraceFiltInst2(TRACE_DEBUG,"AFM_MPC: Shmpcmin::process DownMix Underflow, output buffer filled with zeros (%d/%d)", (int)(bufOut->filledLen), (int)(bufOut->allocLen));
            fillBufWithZeros(bufOut);
            bufOut->flags = 0;
            ReturnOutputBuffer(this,bufOut);
        }
    }
    else if (size_in_eq_out > available_out_size * 2) {
        (*Shmin_copy_downmix_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, available_out_size << (size_in_shift + 1));
        mDataConsumed += available_out_size << (size_in_shift + 1);

        if (bufIn->flags & BUFFERFLAG_STARTTIME)
        {
            bufOut->flags     |= (bufIn->flags & BUFFERFLAG_STARTTIME);
            bufIn->flags      ^= BUFFERFLAG_STARTTIME;
        }

        bufOut->filledLen += available_out_size;
        ReturnOutputBuffer(this, bufOut);
    }
}

void ShmPcmIn_Process_Upmix(Component *this)
{
    Buffer_p    bufIn, bufOut;
    int         i, out_offset, available_out_size, remaining_in_size, size_in_eq_out;

    OstTraceFiltInst2(TRACE_DEBUG, "AFM_MPC: shmpcmin Process Upmix isSynchronized=%d, mInputBufferAlreadyReceived=%d", mShmConfig.isSynchronized, mInputBufferAlreadyReceived);

    if ( (mShmConfig.isSynchronized == false)
            &&  mInputBufferAlreadyReceived
	    &&  HostSync_queuedInputBufferCount() == 0
            &&  Port_queuedBufferCount(&this->ports[OUT]) != 0)
    {
        bufOut = Port_getBuffer(&this->ports[OUT], 0);

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_MPC: Shmpcmin:: Process Upmix Underflow, output buffer filled with zeros (%d bytes added)", (int)bufOut->filledLen);

        for (i =0; i < bufOut->allocLen; i++) { 
            bufOut->data[i] = 0;
        }

        bufOut->flags = 0;
        bufOut->filledLen = bufOut->allocLen;
        Port_dequeueAndReturnBuffer(&this->ports[OUT]);
        return;
    }

    if (HostSync_queuedInputBufferCount() == 0
            || Port_queuedBufferCount(&this->ports[OUT]) == 0) {
        return;
    }

    mInputBufferAlreadyReceived = true;

    bufIn  = HostSync_getInputBuffer(0);
    bufOut = Port_getBuffer(&this->ports[OUT], 0);

    out_offset         =  (int) bufOut->filledLen;
    available_out_size = ((int) bufOut->allocLen) - out_offset;
    remaining_in_size  = ((int) bufIn->filledLen) - mDataConsumed;

    if (bufIn->flags & BUFFERFLAG_STARTTIME)
    {
        mTimeStampDetected = true;
    }

    // propagate timestamp if needed
    if (mTimeStampDetected)
    {
        if (out_offset) 
        {
            TSpropagation(bufOut, bufIn, mDataConsumed, out_offset, mShmConfig.sampleFreq, mShmConfig.channelsOut);
        }
        else 
        {
            TSpropagation(bufOut, bufIn, mDataConsumed, out_offset, mShmConfig.sampleFreq, mShmConfig.channelsIn);
        }
    }

    size_in_eq_out = (remaining_in_size >> size_in_shift); /* Support of 32b input data, need to calculate output size */
    if (size_in_eq_out * 2 == available_out_size)
    {
        (*Shmin_copy_upmix_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, remaining_in_size);
        bufOut->filledLen += (size_in_eq_out << 1);

        bufOut->flags     |= bufIn->flags;

        HostSync_dequeueInputBuffer();
        mDataConsumed = 0;

        if (bufOut->flags & BUFFERFLAG_EOS) {
            OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process UpMix eos sent");
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->flags);
        }

        ReturnOutputBuffer(this, bufOut);
    }
    else if (size_in_eq_out * 2 < available_out_size) {
        (*Shmin_copy_upmix_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, remaining_in_size);
        bufOut->filledLen += (size_in_eq_out << 1);

        bufOut->flags     |= bufIn->flags;

        HostSync_dequeueInputBuffer();
        mDataConsumed = 0;

        if (bufOut->flags & BUFFERFLAG_EOS) {
            OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process UpMix eos sent");
            fillBufWithZeros(bufOut);
            proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->flags);
            ReturnOutputBuffer(this, bufOut);
        }
        else if (bufOut->flags & BUFFERFLAG_MASK_FOR_DRAIN) {
            OstTraceFiltInst1 (TRACE_DEBUG, "AFM_MPC: shmpcmin Process UpMix EOS tag (%x) for DRAIN sent", (int)bufOut->flags & BUFFERFLAG_MASK_FOR_DRAIN);
            fillBufWithZeros(bufOut);
            ReturnOutputBuffer(this, bufOut);
        }
        else if (HostSync_queuedInputBufferCount() != 0) {
            Component_scheduleProcessEvent(this);
        }
        else  if (mShmConfig.isSynchronized == false) {
            OstTraceFiltInst2(TRACE_DEBUG,"AFM_MPC: Shmpcmin::process UpMix Underflow, output buffer filled with zeros (%d/%d)", (int)(bufOut->filledLen), (int)(bufOut->allocLen));
            fillBufWithZeros(bufOut);
            bufOut->flags = 0;
            ReturnOutputBuffer(this,bufOut);
        }
    }
    else if (size_in_eq_out * 2 > available_out_size) {
        (*Shmin_copy_upmix_routine) (bufIn->data + mDataConsumed, bufOut->data + out_offset, available_out_size >> (1 - size_in_shift));
        mDataConsumed += available_out_size >> (1 - size_in_shift);

        bufOut->filledLen += available_out_size;

        if (bufIn->flags & BUFFERFLAG_STARTTIME)
        {
            bufOut->flags     |= (bufIn->flags & BUFFERFLAG_STARTTIME);
            bufIn->flags      ^= BUFFERFLAG_STARTTIME;
        }
        ReturnOutputBuffer(this, bufOut);
    }
}


void ShmPcmIn_Reset(Component *this)
{
    mBufOut.filledLen       = 0;
    mBufOut.address         = NULL;
    mBufOut.byteInLastWord  = 0;
    mBufOut.flags           = 0;
    mDataConsumed           = 0;
    mTimeStampDetected      = false;
    mSendNewFormat          = true;
    mInputBufferAlreadyReceived = false;
}

static void
ShmPcmIn_disablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

static void
ShmPcmIn_enablePortIndication(t_uint32 portIdx) {
    ASSERT(0);
}

static void
ShmPcmIn_flushPortIndication(t_uint32 portIdx) {
    (*mShmIn.reset)(&mShmIn);
}

////////////////////////////////////////////////////////////////////////
//					Provided Interfaces
////////////////////////////////////////////////////////////////////////

void METH(setParameter)(ShmPcmConfig_t config ,void *buffer,t_uint16 output_blocksize) {
    int idx;

    PRECONDITION(config.nb_buffer < MAX_NB_BUFFERS+1);
    PRECONDITION(config.channelsIn <= 2);
    PRECONDITION(config.channelsOut <= 2);

    mShmConfig = config;
    
    ASSERT((mShmConfig.bitsPerSampleIn  == 16) || (mShmConfig.bitsPerSampleIn  == 32));
    ASSERT((mShmConfig.bitsPerSampleOut == 16) || (mShmConfig.bitsPerSampleOut == 24));
    if(mShmConfig.bitsPerSampleIn == 16)
    {
        size_in_shift = 0;  // 0 for 16 bits per sample, 1 for 32 bits per sample
        if(mShmConfig.bitsPerSampleOut == 16) 
        {
            if(mShmConfig.swap_bytes != NO_SWAP) 
            {
                Shmin_copy_downmix_routine = copy_downmix_in16b_out16b_swap;
                Shmin_copy_upmix_routine   = copy_upmix_in16b_out16b_swap;
                Shmin_copy_routine         = copy_in16b_out16b_swap;
            }
            else
            {
                Shmin_copy_downmix_routine = copy_downmix_in16b_out16b_noswap;
                Shmin_copy_upmix_routine   = copy_upmix_in16b_out16b_noswap;
                Shmin_copy_routine         = copy_in16b_out16b_noswap;
            }
        }
        else
        {
            if(mShmConfig.swap_bytes != NO_SWAP) 
            {
                Shmin_copy_downmix_routine = copy_downmix_in16b_out24b_swap;
                Shmin_copy_upmix_routine   = copy_upmix_in16b_out24b_swap;
                Shmin_copy_routine         = copy_in16b_out24b_swap;
            }
            else
            {
                Shmin_copy_downmix_routine = copy_downmix_in16b_out24b_noswap;
                Shmin_copy_upmix_routine   = copy_upmix_in16b_out24b_noswap;
                Shmin_copy_routine         = copy_in16b_out24b_noswap;
            }
        }
    }
    else
    {
        size_in_shift = 1;  // 0 for 16 bits per sample, 1 for 32 bits per sample
        if(mShmConfig.bitsPerSampleOut == 16) 
        {
            if(mShmConfig.swap_bytes != NO_SWAP) 
            {
                Shmin_copy_downmix_routine = copy_downmix_in32b_out16b_swap;
                Shmin_copy_upmix_routine   = copy_upmix_in32b_out16b_swap;
                Shmin_copy_routine         = copy_in32b_out16b_swap;
            }
            else
            {
                Shmin_copy_downmix_routine = copy_downmix_in32b_out16b_noswap;
                Shmin_copy_upmix_routine   = copy_upmix_in32b_out16b_noswap;
                Shmin_copy_routine         = copy_in32b_out16b_noswap;
            }
        }
        else
        {
            if(mShmConfig.swap_bytes != NO_SWAP) 
            {
                Shmin_copy_downmix_routine = copy_downmix_in32b_out24b_swap;
                Shmin_copy_upmix_routine   = copy_upmix_in32b_out24b_swap;
                Shmin_copy_routine         = copy_in32b_out24b_swap;
            }
            else
            {
                Shmin_copy_downmix_routine = copy_downmix_in32b_out24b_noswap;
                Shmin_copy_upmix_routine   = copy_upmix_in32b_out24b_noswap;
                Shmin_copy_routine         = copy_in32b_out24b_noswap;
            }
        }
    }

    mBufOut.data     = buffer;
    mBufOut.allocLen = output_blocksize;

    for(idx=0;idx<config.nb_buffer;idx++){
        preventSleep();
    }

}

void METH(fsmInit) (fsmInit_t initFsm) {
    int i;
    // trace init (mandatory before port init)
    FSM_traceInit(&mShmIn, initFsm.traceInfoAddr, initFsm.id1);

    mFifoOut[0] = &mBufOut;

    Port_init(&mPorts[IN], InputPort, false, false, 0, mFifoIn,  mShmConfig.nb_buffer,
            &inputport, IN, (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mShmIn);
    Port_init(&mPorts[OUT],OutputPort,true , false, 0, mFifoOut, 1,
            &outputport, OUT, (initFsm.portsDisabled & (1 << (OUT))), (initFsm.portsTunneled & (1 << (OUT))), &mShmIn);

    if (mShmConfig.channelsIn == mShmConfig.channelsOut) {
        mShmIn.process = ShmPcmIn_Process;
    }
    else if (mShmConfig.channelsIn == 1 && mShmConfig.channelsOut == 2) {
        mShmIn.process = ShmPcmIn_Process_Upmix;
    }
    else if (mShmConfig.channelsIn == 2 && mShmConfig.channelsOut == 1) {
        mShmIn.process = ShmPcmIn_Process_DownMix;
    }
    else { ASSERT(0); }

    mShmIn.reset = ShmPcmIn_Reset;
    mShmIn.disablePortIndication    = ShmPcmIn_disablePortIndication;
    mShmIn.enablePortIndication     = ShmPcmIn_enablePortIndication;
    mShmIn.flushPortIndication      = ShmPcmIn_flushPortIndication;

    Component_init(&mShmIn, 2, &mPorts, &proxy);
    
    mFsmInitialized = true;
    for(i=0;i<MAX_NB_BUFFERS;i++)
    {
      if(mPendingBuffer[i] != (Buffer_p)0)
      {
        Port_queueBuffer(&mPorts[IN], mPendingBuffer[i]);
        mPendingBuffer[i] = (Buffer_p)0;
      }
    }
}

void METH(start)(void) {
    mSleepCallBack.interfaceCallback = sleepNotify;
    registerForSleep(&mSleepCallBack);
}

void METH(stop)(void) {
    unregisterForSleep(&mSleepCallBack);
}

void METH(enter)(void) {
    // Method called just before DSP goes to sleep
    mWasSleeping = true;
    mRecentlyWokeUp = 1;
}


void METH(leave)(void) {
    // Method called just after DSP leaves sleep
    Component_scheduleProcessEvent(&mShmIn);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(processEvent)() {
    Component_processEvent(&mShmIn);
}

// From Coder/decoder
void METH(fillThisBuffer)(Buffer_p buffer) {
    TRACE_t * this = (TRACE_t *)&mShmIn;
    OstTraceFiltInst2 (TRACE_DEBUG, "AFM_MPC: shmpcmin: fillThisBuffer 0x%x (nb queued buf %d)", (unsigned int)buffer, Port_queuedBufferCount(&mPorts[OUT]));
    buffer->flags = 0; //Reset buffer flag (needed for EOS handing)
    Component_deliverBuffer(&mShmIn, OUT, buffer);
}

// From ARM (shared_bufin) port
void METH(emptyThisBuffer)(Buffer_p buffer) {
    TRACE_t * this = (TRACE_t *)&mShmIn;
    OstTraceFiltInst3 (TRACE_DEBUG, "AFM_MPC: shmpcmin: emptyThisBuffer 0x%x (nFilledLen=%d) (nb queued buf %d)", (unsigned int)buffer,  (unsigned int)buffer->filledLen, Port_queuedBufferCount(&mPorts[IN]) );

    // telephony latency control
    OstTraceFiltInst4(TRACE_DEBUG, "AFM_MPC: shmpcmin: latency control (mpc_shmpcmin, input) (timestamp = 0x%x 0x%x 0x%x us) (flags %d)", buffer->nTimeStamph, (unsigned int)(buffer->nTimeStampl >> 24), (unsigned int)(buffer->nTimeStampl & 0xffffffu), (unsigned int)buffer->flags);

    if (buffer->flags & BUFFERFLAG_EOS) {
        OstTraceFiltInst0 (TRACE_DEBUG, "AFM_MPC: shmpcmin emptyThisBuffer eos found");
    }
    allowSleep();

    if(mFsmInitialized)
    {
      Component_deliverBuffer(&mShmIn, IN, buffer);
    }
    else
    {
      int i;
      for (i=0;i<MAX_NB_BUFFERS;i++)
      {
        if(mPendingBuffer[i] == (Buffer_p)0)
        {
          mPendingBuffer[i] = buffer;
          break;
        }
      }
    }
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    if (mSendNewFormat
            && cmd == OMX_CommandStateSet && ((OMX_STATETYPE)param) == OMX_StateExecuting){
        outputsettings.newFormat(
                mShmConfig.sampleFreq,
                mShmConfig.channelsOut,
                mShmConfig.bitsPerSampleOut);
        mSendNewFormat = false;
    }
    Component_sendCommand(&mShmIn, cmd, param);
}

