/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   audiomodem_driver_defines.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __audiomodem_driver_defines_h__
#define __audiomodem_driver_defines_h__

typedef enum {
  AUDIOMODEM_UL,
  AUDIOMODEM_DL
} eBufferMode;

typedef enum {
  BUFFER_UNUSED,
  BUFFER_TO_BE_PROCESSED,
  BUFFER_PROCESSED
} eBufferState;


#define AUDIO_MODEM_MAX_MESSAGE_SIZE         664
#define AUDIO_MODEM_NB_MAX_MESSAGES_IN_QUEUE 32

#define AMC_TRACE(a, ...) printf(a, ...)

#ifdef AMC_TRACE_ON
#define AMC_TRACE(a, ...) printf(a, ...)
#else
//#define AMC_TRACE(a, ...)
#endif

#endif
