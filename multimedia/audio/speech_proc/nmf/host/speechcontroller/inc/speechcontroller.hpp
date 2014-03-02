/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     speechcontroller.hpp
 * \brief    controller for Speech Proc Composite
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef __speechcontrollerhpp
#define __speechcontrollerhpp

#include "audiocontroller.hpp"

typedef enum {
  DOWNLINK_INDEX   = 0,
  UPLINK_INDEX     = 1,
  TIME_ALIGN_INDEX = 2,
  DSP_PORT_INDEX   = 3,
} ComponentIndex;

#ifdef USE_HYBRID_COMPONENT
#define NB_SLAVE 4  // Hybrid component has DSP_PORT component
#else
#define NB_SLAVE 3 
#endif

typedef enum {
  DOWNLINK_OMX_PORT_INDEX       = 1,
  UPLINK_OMX_PORT_INDEX         = 3,
  TIME_ALIGN_REF_OMX_PORT_INDEX = 2,
  TIME_ALIGN_OMX_PORT_INDEX     = 0,
  DSP_PORT_OMX_PORT_INDEX       = 4,
  CTRL_PORT_INDEX               = 5,
} ComponentOmxPortIndex;


typedef enum {
  DOWNLINK_LOCAL_OUT_PORT_INDEX  = 0,
  DOWNLINK_LOCAL_IN_PORT_INDEX   = 1,
  
  UPLINK_LOCAL_OUT_PORT_INDEX  = 0,
  UPLINK_LOCAL_IN_PORT_INDEX   = 1,

  DSP_PORT_LOCAL_OUT_PORT_INDEX  = 0,
  DSP_PORT_LOCAL_IN_PORT_INDEX   = 1,
  
  TIME_ALIGN_LOCAL_IN_PORT_INDEX   = 0,
  TIME_ALIGN_LOCAL_REF_PORT_INDEX  = 1,
  TIME_ALIGN_LOCAL_OUT_PORT_INDEX  = 2,
} ComponentLocalPortIndex;

#define ALL_PORTS_TUNNELED 0xFFFF

class speech_proc_nmf_host_speechcontroller : public hst_misc_audiocontroller, public speech_proc_nmf_host_speechcontrollerTemplate
{
public:
  virtual void fsmInit(fsmInit_t initFsm);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { hst_misc_audiocontroller::sendCommand(cmd, param); }
  virtual void forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword portIdx) ;

  virtual void  downlink_eventHandler      (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
  virtual void  uplink_eventHandler        (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
  virtual void  timealignment_eventHandler (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
  virtual void  dspport_eventHandler       (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
                                        
private:
  t_uint16 omx2localPortMask(t_uint16 mask, ComponentIndex compo);
};

#endif
