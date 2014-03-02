/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   audiocontroller.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _audiocontroller_hpp_
#define _audiocontroller_hpp_

#include "FSM.h"
#include "CtrlEvent.h"
#include "AFM_macros.h"

#include "eventhandler.hpp"
#include "sendcommand.hpp"          

typedef struct {
    Isendcommand        * itf ;             // Not NULL, but may be not binded
    bool                isBufferSupplier;
    OMX_STATETYPE  	    state ;
    OMX_COMMANDTYPE     currentCommand ;    // Defined if remainingCommands != 0
    t_uint8 			remainingCommands ;
    t_uint8             remainingReceivedCommands ;
} slaveInfo_t;

typedef struct {
    OMX_COMMANDTYPE        command ;
    t_uword                id ;
} standbyInfo_t ;

class hst_misc_audiocontroller : public FSM {

    public:
        AFM_API_IMPORT virtual ~hst_misc_audiocontroller() = 0 ;	

        // Interface from proxy - Catch all command
        AFM_API_IMPORT void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) ;
        // Interface from slaves - Catch OMX_EventCmdComplete and OMX_EventError(->Invalid State)
        AFM_API_IMPORT void slavesEventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 id) ;

    protected:
        AFM_API_IMPORT void init(t_uint8 slaveCount, Ieventhandler *nmfitf) ;

        /*
         * add slave to ctrl
         */
        AFM_API_IMPORT void setSlave(t_uint8 slaveIndex, Isendcommand * slave, bool isBufferSupplier) ;

        /*
         * Registers a command for a given slave
         */
        AFM_API_IMPORT void waitCommandFromSlave(t_uint8 slaveIndex, OMX_COMMANDTYPE command, t_uint8 count) ;

        /*
         * Return controller OMX state
         */
        OMX_STATETYPE getState(void) { return mOmxState; };

        /* 
         * Delegate forward of OMX_CommandPortDisable, OMX_CommandPortEnable and OMX_CommandFlush
         * Called by the controller FSM each time a Command must be forwared to some sub-components
         */	
        virtual  void forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) = 0 ;

        /* 
         * Delegate forward of OMX_EventCmdComplete for OMX_CommandPortDisable, OMX_CommandPortEnable and OMX_CommandFlush
         * Called by the controller FSM each time a Command is completed (all waitCommandFromSlave are completed), and an event must be
         * forwarded to the proxy.
         */
        virtual  void forwardPortEvent(OMX_EVENTTYPE ev, OMX_COMMANDTYPE cmd, t_uword id) {
            mEventHandler->eventHandler(ev, cmd, id);
        };

    private:	

        void omxState(const CtrlEvent *);
        void invalidState(const CtrlEvent *);        
        void transientState(const CtrlEvent *);
        void waitForBufferUserSetStateReceived(const CtrlEvent *evt);
        void waitForBufferSupplierSetStateReceived(const CtrlEvent *evt); 
        void waitForCommandCompleteState(const CtrlEvent *evt);

        //ctrl dispatch OMX_CommandStateSet on all slaves
        void forwardStateChange(const OMX_STATETYPE targetState) ;
        void forwardStateChangeToBufferUser(const OMX_STATETYPE targetState);
        void forwardStateChangeToBufferSupplier(const OMX_STATETYPE targetState);
        bool allSlavesInTargetState(const OMX_STATETYPE targetState);
        bool allSlaveCmdExecuted(void);
        bool allSlaveCmdReceived(void);        
        bool allBufferUserSlavesReceivedSetStateCmd(void);
        bool allBufferSupplierSlavesReceivedSetStateCmd(void);
        bool allSlavesReceivedSetStateCmd(void);

    private:
        t_uint8       	    mSlaveCount;
        t_uint8       	    mSlaveCountBinded;

        slaveInfo_t         *mSlavesArray;  // Array of slaveInfo_t
        Ieventhandler       *mEventHandler; // Proxy

        standbyInfo_t		mStandbyInfo;

        OMX_STATETYPE       mOmxState;        
        bool                mNotifyStateTransition;
};

#endif /* _audiocontroller_hpp_ */
