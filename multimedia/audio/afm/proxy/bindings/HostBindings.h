/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostBindings.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _HOSTBINDINGS_H_
#define _HOSTBINDINGS_H_

#include "AFMNmfHost_ProcessingComp.h" // just because of memset. should be removed
#include "Bindings.h"

class HostBindings : public Bindings {
public:
  
  /// Constructor
  HostBindings(AFM_Port &port, AFM_Controller &controller, OMX_HANDLETYPE omxhandle);
  
  virtual OMX_ERRORTYPE instantiate(void);
  
  virtual OMX_ERRORTYPE start(void);
  
  virtual OMX_ERRORTYPE stop(void);
  
  virtual OMX_ERRORTYPE deInstantiate(void);

  virtual OMX_ERRORTYPE connectToInput(NMF::Composite * hNmfOut,const char * outSuffix);
  virtual OMX_ERRORTYPE connectToInput(t_cm_instance_handle hNmfOut,const char * outSuffix) {
    DBC_ASSERT(0);
    return OMX_ErrorNotImplemented;}
  
  virtual OMX_ERRORTYPE connectToOutput(NMF::Composite * hNmfOut,const char * outSuffix);
  virtual OMX_ERRORTYPE connectToOutput(t_cm_instance_handle hNmfOut,const char * outSuffix) {
    DBC_ASSERT(0);
    return OMX_ErrorNotImplemented;}
  
  virtual OMX_ERRORTYPE disconnectFromInput(NMF::Composite * hNmfIn,const char * inSuffix);
  virtual OMX_ERRORTYPE disconnectFromInput(t_cm_instance_handle hNmfIn,const char * inSuffix) {
    DBC_ASSERT(0);
    return OMX_ErrorNotImplemented;}
  
  virtual OMX_ERRORTYPE disconnectFromOutput(NMF::Composite * hNmfIn,const char * inSuffix);
  virtual OMX_ERRORTYPE disconnectFromOutput(t_cm_instance_handle hNmfIn,const char * inSuffix) {
    DBC_ASSERT(0);
    return OMX_ErrorNotImplemented;}
  
  virtual t_cm_instance_handle getNmfMpcHandle(void){DBC_ASSERT(0); return (t_cm_instance_handle)0;}
  virtual NMF::Composite * getNmfHostHandle(void){return mNmfHandle;}
  virtual OMX_U32 getPortPriorityLevel();
  
#define SAMPLE_SIZE sizeof(short);
  
protected:
  NMF::Composite *        mNmfHandle;
  OMX_HANDLETYPE          mOMXHandle;
  
};


namespace AFM {
  AFM_API_IMPORT OMX_ERRORTYPE hostConnectPorts(
				 NMF::Composite * hNmfOut, 
				 const char * outSuffix,
				 NMF::Composite * hNmfIn,
				 const char * inSuffix,
				 bool pcm);

  AFM_API_IMPORT OMX_ERRORTYPE hostDisconnectPorts(
				    NMF::Composite * hNmfOut, 
				    const char * outSuffix,
				    NMF::Composite * hNmfIn,
				    const char * inSuffix,
				    bool pcm);
}




/// Pointer type to an Bindings elements
typedef HostBindings * HostBindings_p;

#endif // _HOSTBINDINGS_H_
