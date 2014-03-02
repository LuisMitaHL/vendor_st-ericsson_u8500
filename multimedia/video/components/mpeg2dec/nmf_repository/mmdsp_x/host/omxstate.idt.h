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

/* Generated omxstate.idt defined type */
#if !defined(__OMXSTATE_H_)
#define __OMXSTATE_H_


typedef enum t_xyuv_OMX_STATETYPE {
  OMX_StateInvalid,
  OMX_StateLoaded,
  OMX_StateIdle,
  OMX_StateExecuting,
  OMX_StatePause,
  OMX_StateWaitForResources,
  OMX_StateTestForResources,
  OMX_StateTransientToIdle,
  OMX_StateTransient,
  OMX_StateLoadedToIdleHOST,
  OMX_StateLoadedToIdleDSP,
  OMX_StateIdleToLoaded,
  OMX_StateIdleToExecuting,
  OMX_StateIdleToPause,
  OMX_StateExecutingToIdle,
  OMX_StateExecutingToPause,
  OMX_StatePauseToIdle,
  OMX_StatePauseToExecuting,
  OMX_StateTransientToDisable,
  OMX_StateLoadedToTestForResources,
  OMX_StateCheckRscAvailability,
  OMX_StateFlushing,
  OMX_StateIdleSuspended,
  OMX_StatePauseSuspended,
  OMX_StateMax=0X7FFF} OMX_STATETYPE;

#endif
