/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Port.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _PORT_H_
#define _PORT_H_

#include "ENS_Redefine_Class.h"

#include "FSM.h"  
#include "PortQueue.h"
#include "OmxEvent.h"

#include <inc/typedef.h>
#include "armnmf_emptythisbuffer.hpp"
#include "armnmf_fillthisbuffer.hpp"

#include "OMX_Core.h"

typedef enum {
  InputPort,
  OutputPort
} Direction;

class Component ;

class Port : public FSM {
public:
  ENS_API_IMPORT void init(Direction dir, bool bufferSupplier, bool isHWport, Port * sharingPort, OMX_BUFFERHEADERTYPE** buffers, int bufferCount, NMF::InterfaceReference *nmfitf, int portIdx, bool isDisabled, bool isTunneled, Component *componentOwner);

  ENS_API_IMPORT OMX_STATETYPE getState(void);
  ENS_API_IMPORT void returnBuffer(OMX_BUFFERHEADERTYPE * buf) ;
	
  ENS_API_IMPORT void setTunnelStatus(bool isTunneled);	
  ENS_API_IMPORT void setBufferResetReqStatus(bool flag);

  bool 	isInvalid(void) { return getState() == OMX_StateInvalid ; }
  bool  isEnabled(void) { return bufferQueue.getEnabled() ; }
	
  bool  postProcessCheck(void);
  void 	flushBuffers(void);
  bool  isFlushing(void);
	
	
  OMX_BUFFERHEADERTYPE * getBuffer(int n) { return (OMX_BUFFERHEADERTYPE *)bufferQueue.getItem(n); }

  int   bufferCount(void)         { return bufferQueue.getSize(); }
  int   queuedBufferCount(void)   { return bufferQueue.itemCount(); }
  int   dequeuedBufferCount(void) { return dequeuedbuffercount; }    
  void  queueBuffer(OMX_BUFFERHEADERTYPE * buf) { bufferQueue.push_back(buf); }  
  void  dequeueAndReturnBuffer(void)  { returnBuffer(dequeueBuffer()); }
  ENS_API_IMPORT void  returnUnprocessedBuffer(OMX_BUFFERHEADERTYPE *buf);

  OMX_BUFFERHEADERTYPE * dequeueBuffer() {
	this->dequeuedbuffercount++;
	return (OMX_BUFFERHEADERTYPE *)bufferQueue.pop_front();	
  }
		
  void   requeueBuffer(OMX_BUFFERHEADERTYPE * buf) { 
	this->dequeuedbuffercount--;
	bufferQueue.push_front(buf); 
  }

private:
  void idleState(const OmxEvent * evt);
  void executingState(const OmxEvent * evt);
  void pauseState(const OmxEvent * evt);
		
  void pauseFromIdleOrDisabledState(const OmxEvent * evt);
  void transientToIdleOrDisabledState(const OmxEvent * evt);
  void transientToFlushState(const OmxEvent * evt);
  void disabledState(const OmxEvent * evt);
		
  void invalidState(const OmxEvent * evt);

  void returnBufferInternal(OMX_BUFFERHEADERTYPE_p buf) ;
  void returnBufferInternalNmf(OMX_BUFFERHEADERTYPE_p buf) ;
  void reset(void) ;		
  void transferQueuedBuffersToSharingPort(void) ;
  void goToIdleOrDisable(void) ;
  void goToFlushBuffers(void) ;
  void flushComplete(bool fromTransientToFlushState);
		
  static FSM_State translateState(OMX_STATETYPE state) ;

private:
  unsigned char isBufferSupplier ;
  unsigned char direction ;
  unsigned char dequeuedbuffercount ;
  unsigned char isHWport ;
  Port *		sharingPort;            
  PortQueue     bufferQueue;

  union {
	Iarmnmf_emptythisbuffer * armnmf_emptythisbuffer;
	Iarmnmf_fillthisbuffer  * armnmf_fillthisbuffer ;
  } nmfItf ;
	
  unsigned int  portIdx;
  bool          notifyStateTransition;
  OMX_STATETYPE disabled_State;
  OMX_STATETYPE flush_State;
  Component *   componentOwner;
  bool			isPortTunneled;
  bool			isBufferResetReq;
  bool          eventAppliedOnEachPorts;
} ;								/* Port */

#endif // _PORT_H_
