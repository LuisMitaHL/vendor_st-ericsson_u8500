/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MpcBindings.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MPCBINDINGS_H_
#define _MPCBINDINGS_H_

#include "Bindings.h"

class MpcBindings : public Bindings {
public:
       
  /// Constructor
  MpcBindings(AFM_Port &port, AFM_Controller &controller, OMX_U32 priority);

  virtual OMX_ERRORTYPE instantiate(void);
  
  virtual OMX_ERRORTYPE start(void);
  
  virtual OMX_ERRORTYPE stop(void);
  
  virtual OMX_ERRORTYPE deInstantiate(void);

  virtual OMX_ERRORTYPE connectToInput(t_cm_instance_handle hNmfOut,const char * outSuffix);
  virtual OMX_ERRORTYPE connectToInput(NMF::Composite *  hNmfOut,const char * outSuffix) {
    DBC_ASSERT(0);
    return OMX_ErrorNotImplemented;}
  
  virtual OMX_ERRORTYPE connectToOutput(t_cm_instance_handle hNmfOut,const char * outSuffix);
  virtual OMX_ERRORTYPE connectToOutput(NMF::Composite *  hNmfOut,const char * outSuffix) {
    DBC_ASSERT(0);
    return OMX_ErrorNotImplemented;}
  
  virtual OMX_ERRORTYPE disconnectFromInput(t_cm_instance_handle hNmfIn,const char * inSuffix);
  virtual OMX_ERRORTYPE disconnectFromInput(NMF::Composite *  hNmfOut,const char * outSuffix) {
    DBC_ASSERT(0);
    return OMX_ErrorNotImplemented;}
  
  virtual OMX_ERRORTYPE disconnectFromOutput(t_cm_instance_handle hNmfIn,const char * inSuffix);
  virtual OMX_ERRORTYPE disconnectFromOutput(NMF::Composite *  hNmfOut,const char * outSuffix) {
    DBC_ASSERT(0);
    return OMX_ErrorNotImplemented;}
  
  
  virtual t_cm_instance_handle getNmfMpcHandle(void){return mNmfHandle;}
  virtual NMF::Composite * getNmfHostHandle(void){DBC_ASSERT(0); return (NMF::Composite *)0;}
  virtual OMX_U32 getPortPriorityLevel();
  
protected:
  OMX_U32 getNMFDomainHandle(void) {
    OMX_U32 portIndex = mPort.getPortIndex();
    return mPort.getENSComponent().getNMFDomainHandle(portIndex);
  }

  t_cm_instance_handle    mNmfHandle;
  t_cm_instance_handle    mOstTrace;
  OMX_U32                 mPriority;
	
private:
  t_cm_instance_handle    mNmfGenericFsmLib;
  t_cm_instance_handle    mNmfComponentFsmLib;
};

/// Pointer type to an Bindings elements
typedef MpcBindings * MpcBindings_p;

#endif // _MPCBINDINGS_H_
