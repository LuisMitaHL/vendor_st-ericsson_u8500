/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shmout.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <hst/bindings/shm/shmout.nmf>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <armnmf_dbc.h>

#include "bindings_optim.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_hst_bindings_shm_shmout_src_shmoutTraces.h"
#endif

// TODO FIXME: Manage buffers than are not 32 bit multiple long
#define SUPPORTED_BUFFER_SIZE_MULTIPLE_IN_BYTES 4
void hst_bindings_shm_shmout::swap_bytes(t_uint8 *buffer, t_uint32 size, t_swap_mode swap_mode)
{
    swap_bytes_optim((void *) buffer, size, (int) swap_mode);
}

void hst_bindings_shm_shmout::process()
{
  if (mPorts[INPUT_PORT].queuedBufferCount())
    {
        // Process Input buffer
      OMX_BUFFERHEADERTYPE * bufIn =  mPorts[INPUT_PORT].dequeueBuffer();
      mBufferSent = false;
      
      ARMNMF_DBC_ASSERT_MSG((bufIn->nAllocLen % SUPPORTED_BUFFER_SIZE_MULTIPLE_IN_BYTES) == 0, "Buffer size not multiple of 32 bits\n");

      if (mShmConfig.swap_bytes != NO_SWAP) {
	swap_bytes(bufIn->pBuffer, 
		   (bufIn->nFilledLen + SUPPORTED_BUFFER_SIZE_MULTIPLE_IN_BYTES - 1) & ~(SUPPORTED_BUFFER_SIZE_MULTIPLE_IN_BYTES-1),
		   mShmConfig.swap_bytes );
      }
      
      if(bufIn->nFlags & OMX_BUFFERFLAG_EOS){
	proxy.eventHandler(OMX_EventBufferFlag, 1, bufIn->nFlags);
      }
   	  OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmout::process release output buffer");
      mPorts[OUTPUT_PORT].returnBuffer(bufIn);
    }
  
  
  if (mPorts[OUTPUT_PORT].queuedBufferCount() && mBufferSent == false)
    {
      // Process Output buffer
      OMX_BUFFERHEADERTYPE * bufOut =  mPorts[OUTPUT_PORT].dequeueBuffer();
      
      mBufferSent = true;
	  OstTraceFiltInst0(TRACE_DEBUG, "AFM_HOST: Shmout::process release input buffer");
      mPorts[INPUT_PORT].returnBuffer(bufOut);
    }
}

void hst_bindings_shm_shmout::reset()
{
  mBufferSent = false; 
}
void hst_bindings_shm_shmout::disablePortIndication(t_uint32 portIdx) {} ;
void hst_bindings_shm_shmout::enablePortIndication(t_uint32 portIdx) {} ;
void hst_bindings_shm_shmout::flushPortIndication(t_uint32 portIdx) { reset(); } ;

////////////////////////////////////////////////////////////////////////
//					Provided Interfaces
////////////////////////////////////////////////////////////////////////
void METH(fsmInit)(fsmInit_t initFsm)
{
  // trace init (mandatory before port init)
  if (initFsm.traceInfoAddr){
  	  setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
  }

	mPorts[INPUT_PORT].init(InputPort,  true, false,  &mPorts[OUTPUT_PORT], 0, 1, &inputport,   INPUT_PORT, 
         (initFsm.portsDisabled & (1 << (INPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (INPUT_PORT)))!=0, this);
  mPorts[OUTPUT_PORT].init(OutputPort,false ,false, &mPorts[INPUT_PORT],  0, mShmConfig.nb_buffer, &outputport, OUTPUT_PORT, 
         (initFsm.portsDisabled & (1 << (OUTPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (OUTPUT_PORT)))!=0, this);

  init(2, mPorts, &proxy, &me, false);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ARMNMF_DBC_ASSERT(0);
    mPorts[portIdx].setTunnelStatus(isTunneled);
}

void METH(setParameter)(ShmConfig_t config) {
  ARMNMF_DBC_PRECONDITION(config.nb_buffer < MAX_NB_BUFFERS+1);
  
  mShmConfig = config;
}

void METH(newFormat)(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size)
{
  outputsettings.newFormat(sample_freq, chans_nb, sample_size);
}
