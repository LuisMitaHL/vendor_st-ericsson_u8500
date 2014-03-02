/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Component.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "ENS_Redefine_Class.h"
#include "FSM.h"
#include "Port.h"

#include "OmxEvent.h"
#include "postevent.hpp"

#include "eventhandler.hpp"
#include "omxcommand.idt.h"
#include "omxstate.idt.h"
#include "fsmInit.idt.h"

#include "nmf_mutex.h" 


class Component : public FSM {

public:
    ENS_API_IMPORT Component() ;
    ENS_API_IMPORT virtual ~Component() ;
    OMX_STATETYPE getState() ;
    bool  isInvalid(void) { return getState() == OMX_StateInvalid ; }
    int getPortCount(void) { return portCount;}
    bool allPortsDisabled(void);
    bool allPortsEnabled(void);
    bool isOnePortFlushing(void);
    
    // NOTE: "reset()" function will be removed in a future version. 
    // Is replaced by "StateChangeIndication()" when newState is OMX_StateIdle.
    virtual void reset(void){};
    virtual void process(void) = 0 ;
    virtual void disablePortIndication(t_uint32 portIdx) = 0 ;
    virtual void enablePortIndication(t_uint32 portIdx)  = 0 ;
    virtual void flushPortIndication(t_uint32 portIdx)   = 0 ;
    virtual void stateChangeIndication(OMX_STATETYPE oldState, OMX_STATETYPE newState){};
    
    ENS_API_IMPORT void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) ;

    void deliverBuffer(int portIndex, OMX_BUFFERHEADERTYPE_p buf) {
        ports[portIndex].queueBuffer((OMX_BUFFERHEADERTYPE *)buf);
        scheduleProcessEvent();
    } 
    
    void deliverBufferCheck(int portIndex, OMX_BUFFERHEADERTYPE_p buf) {
        if( ports[portIndex].isEnabled())
        {
            deliverBuffer(portIndex, buf);
        }
        else
        {
            ports[portIndex].returnUnprocessedBuffer(buf);
        }
    }
    
    Ieventhandler * getEventHandler(void) { return eventHandler ; }
    
protected:
    ENS_API_IMPORT void scheduleProcessEvent(void) ;
    ENS_API_IMPORT void processEvent(void) ;
    ENS_API_IMPORT void init(int portCount, Port *ports, Ieventhandler *nmfitf, Ipostevent * me, bool invalid);
    ENS_API_IMPORT void returnBufferAsync(OMX_U32 portIdx,OMX_BUFFERHEADERTYPE * buf) ; // To return a buffer when the component is not processing a FSM event. 
    

private:
    void idleState(const OmxEvent *);
    void executingState(const OmxEvent *);
    void pauseState(const OmxEvent *);
    void transientToIdleState(const OmxEvent *);
    void waitForPortFlushedOrDisabled(const OmxEvent *);	
    void invalidState(const OmxEvent *);
    
    bool postProcessCheck(void) ;
    void updateState(FSM_State currentState) ;
    void setState(const OmxEvent *evt, FSM_State currentState) ;
    FSM_State getStateFromPorts(void) ;

private:
    int             portCount;
    Port*           ports; // Array of Ports
    bool            pendingEvent;

    Ieventhandler*  eventHandler ;
    Ipostevent*     nmf_me ;
    
    OMX_STATETYPE   transientState;
    
    hMutex	    lock ;
} ; /* Component */

#endif // _COMPONENT_H_
