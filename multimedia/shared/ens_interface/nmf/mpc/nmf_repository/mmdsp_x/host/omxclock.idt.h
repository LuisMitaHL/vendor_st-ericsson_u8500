/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated omxclock.idt defined type */
#if !defined(__OMXCLOCK_H_)
#define __OMXCLOCK_H_


typedef enum t_xyuv_OMX_TIME_UPDATETYPE {
  OMX_TIME_UpdateRequestFulfillment,
  OMX_TIME_UpdateScaleChanged,
  OMX_TIME_UpdateClockStateChanged,
  OMX_TIME_UpdateMax=0x7FFF} OMX_TIME_UPDATETYPE;

typedef enum t_xyuv_OMX_TIME_REFCLOCKTYPE {
  OMX_TIME_RefClockNone,
  OMX_TIME_RefClockAudio,
  OMX_TIME_RefClockVideo,
  OMX_TIME_RefClockMax=0X7FFF} OMX_TIME_REFCLOCKTYPE;

typedef enum t_xyuv_OMX_TIME_CLOCKSTATE {
  OMX_TIME_ClockStateRunning,
  OMX_TIME_ClockStateWaitingForStartTime,
  OMX_TIME_ClockStateStopped,
  OMX_TIME_ClockStateMax=0X7FFF} OMX_TIME_CLOCKSTATE;

#endif
