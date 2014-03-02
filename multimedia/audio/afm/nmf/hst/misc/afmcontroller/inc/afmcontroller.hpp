/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   afmcontroller.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __afmcontroller_hpp__
#define __afmcontroller_hpp__

#include "audiocontroller.hpp"

class hst_misc_afmcontroller : public hst_misc_audiocontroller, public hst_misc_afmcontrollerTemplate 
{
    public:
        hst_misc_afmcontroller(): 
            mPortDirections(0), 
            mPortTypes(0), 
            mPortsTunneled(0), 
            mEventAppliedOnEachPort(false), 
            mBindingsCmdReceived(0),
            mIsMainWaittingForDisablePortCmd(false),
            mEnablePortCmdOngoing(false), 
			mSentCommandsToBindings(0) {};

        ~hst_misc_afmcontroller() {};

        virtual void fsmInit(fsmInit_t init);
        virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
        virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { hst_misc_audiocontroller::sendCommand(cmd, param) ; }
        virtual void eventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 collectionIndex);
        virtual void main_eventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2) ;

        virtual void forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) ;
        virtual void setParameter(t_uint16 portDirections, t_uint16 portTypes);

    private:
        bool isOutputPort(int idx);
        bool isInputPort(int idx);
        bool dispatchSendCmdIdleOnBindings(OMX_STATETYPE state, t_uword portIdx);
        bool allBindingsStateCmdReceived(OMX_STATETYPE state, t_uword portIdx);
        bool isThereOneBindingInstantiated(t_uword portIdx);
        void enableBindingComponent(OMX_STATETYPE state, t_uword portIdx);
        void dispatchEnablePortCmdOnBindings(OMX_STATETYPE state, t_uword portIdx);
        void printMainEvents(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2);
        void printBindingEvents(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 collectionIndex);

        int  mPortDirections;
        int  mPortTypes;
        int  mPortsTunneled;
        bool mEventAppliedOnEachPort;
        int  mBindingsCmdReceived;
        bool mIsMainWaittingForDisablePortCmd;    
        bool mEnablePortCmdOngoing;
		int mSentCommandsToBindings;

#define MAX_PORTS (9)
#define ALL_PORTS_TUNNELED (0xFFFF)
#define MAIN_COMPONENT_INDEX MAX_PORTS
};

#endif
