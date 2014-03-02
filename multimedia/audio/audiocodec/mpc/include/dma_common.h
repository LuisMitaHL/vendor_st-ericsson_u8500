/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dma_common.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _DMA_COMMON_H_
#define  _DMA_COMMON_H_
#include <archi.h>
#include <stdio.h>
#include <stdbool.h>
#include <dbc.h>
#include "audiocodec.h"
#include "usefull.h"
#include "inc/archi-wrapper.h"


#define PORT_IS_NOT_TUNNELED      0
#define PORT_IS_TUNNELED          1
#define PORT_IS_ENABLED           0
#define PORT_IS_DISABLED          1
#define PORT_IS_NOT_HW            0
#define PORT_IS_HW                1
#define PORT_IS_BUFFER_USER       0
#define PORT_IS_BUFFER_SUPPLIER   1
#define SHARING_PORT_PTR_NOT_USED NULL


//--------------------------------
// NUMBER OF CHANNELS / SAMPLES 
//--------------------------------
#define NB_MSP_CHANNELS           8
#define DMA_PORT_BUFFER_COUNT     1

#define BT_GRAIN_IN_MS            5

struct portStatus_t {
    bool isRunning;        //!< tranfer are running ? 
	bool weHaveTheBuffer;  //!< do we currently have the buffer 
    bool data_droppped;
    bool muted;
    bool mute_cb;
    Buffer_p buffer;       //!< pointer on buffer
    t_uint48 time_stamp;
    t_uint48 time_diff_cb;
    t_uint16 expandedFragments;
    t_uint16 vol;
    t_sint16 ramp_inc;
    t_uint16 ramp_steps;
};
typedef struct portStatus_t portStatus_t;

// TODO: Remove the use of this type!
struct doubleBuffer_t{
    int *               buffer[2];     // Dsp addr of ping/pong buffer
    unsigned long       phy_addr[2];   // Physical addr of ping/pong buffer (in ARM addressable memory space)
    t_uint16            readIndex;     // values goes from 0 to 1
    t_uint16            writeIndex;    // values goes from 0 to 1
    volatile t_uint16   filledBuffers; // values goes from 0 to 2
};
typedef struct doubleBuffer_t doubleBuffer_t;

#define JITTERBUFFER_LENGTH_MAX  20

struct JitterBuffer_t{
    int *               buffer[JITTERBUFFER_LENGTH_MAX];
    unsigned long       phy_addr[JITTERBUFFER_LENGTH_MAX];
    t_uint16            length;
    t_uint16            read_index;
    t_uint16            write_index;
    volatile t_uint16   filledslots;
};
typedef struct JitterBuffer_t JitterBuffer_t;

//--------------------
// Usefull function
//--------------------
#define MASK_EOT_IT    AUDIO_MASK_IT(mDmaEndOfTranferIT)
#define UNMASK_EOT_IT  AUDIO_UNMASK_IT(mDmaEndOfTranferIT)

void scheduleProcessEvent(void);

#endif   // _DMA_COMMON_H_
