/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Bindings.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _BINDINGS_H_
#define _BINDINGS_H_

#include "ENS_Nmf.h"
#include "AFM_Port.h"
#include "AFM_Controller.h"
#include "TraceObject.h"

class Bindings: public TraceObject  {
public:
  
  /// Constructor
  Bindings(AFM_Port &port, AFM_Controller &controller) :
  mPort(port), mIsPCM(0), mControllerItfIdx(0),mController(controller){}
  
  virtual OMX_ERRORTYPE instantiate(void) = 0;
  
  virtual OMX_ERRORTYPE configure(void) { return OMX_ErrorNone; }     
  
  virtual OMX_ERRORTYPE start(void) = 0;
  
  virtual OMX_ERRORTYPE stop(void) = 0;
  
  virtual OMX_ERRORTYPE deInstantiate(void) = 0;
  
  virtual OMX_ERRORTYPE connectToInput(t_cm_instance_handle hNmfOut,const char * outSuffix) = 0;
  virtual OMX_ERRORTYPE connectToInput(NMF::Composite *  hNmfOut,const char * outSuffix) = 0;
  
  virtual OMX_ERRORTYPE connectToOutput(t_cm_instance_handle hNmfOut,const char * outSuffix) = 0;
  virtual OMX_ERRORTYPE connectToOutput(NMF::Composite *  hNmfOut,const char * outSuffix) = 0;
  
  virtual OMX_ERRORTYPE disconnectFromInput(t_cm_instance_handle hNmfIn,const char * inSuffix) = 0;
  virtual OMX_ERRORTYPE disconnectFromInput(NMF::Composite *  hNmfIn,const char * inSuffix) = 0;
  
  virtual OMX_ERRORTYPE disconnectFromOutput(t_cm_instance_handle hNmfIn,const char * inSuffix) = 0;
  virtual OMX_ERRORTYPE disconnectFromOutput(NMF::Composite *  hNmfIn,const char * inSuffix) = 0;
  
  virtual OMX_ERRORTYPE instantiateBindingComponent(void) = 0;
  virtual OMX_ERRORTYPE deInstantiateBindingComponent(void) = 0;
  
  virtual t_cm_instance_handle getNmfMpcHandle(void) = 0;
  virtual NMF::Composite * getNmfHostHandle(void) = 0;
  
protected:
  AFM_Port &              mPort;	//FIXME! remove this dependency?
  bool                    mIsPCM;
  OMX_U32                 mControllerItfIdx;
  AFM_Controller &        mController;
};

/// Pointer type to an Bindings elements
typedef Bindings * Bindings_p;

#endif // _BINDINGS_H_
