/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shmout.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _shm_shmout_hpp_
#define _shm_shmout_hpp_

#include "Component.h"
#include "common_interface.h"

class hst_bindings_shm_shmout : public Component, public hst_bindings_shm_shmoutTemplate
{
public:

  //Component virtual functions
  virtual void process() ;
    virtual void reset() ;
	virtual void disablePortIndication(t_uint32 portIdx) ;
    virtual void enablePortIndication(t_uint32 portIdx) ;
    virtual void flushPortIndication(t_uint32 portIdx) ;
  
  virtual void fsmInit(fsmInit_t initFsm);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
  virtual void processEvent(void)  			       { Component::processEvent() ; }
  virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) { Component::deliverBuffer(INPUT_PORT, buffer); } 
  virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)  { Component::deliverBuffer(OUTPUT_PORT, buffer); }
  
  virtual void setParameter(ShmConfig_t config);

private:
  typedef enum {INPUT_PORT, OUTPUT_PORT} portname;
#define MAX_NB_BUFFERS 4

  virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size);
  static void swap_bytes(t_uint8 *buffer, t_uint32 size, t_swap_mode swap_mode);

  ShmConfig_t  mShmConfig;
  bool         mBufferSent;
  Port         mPorts[2];
};

#endif // _shm_shmout_hpp_
