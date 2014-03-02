/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_FsmEvent.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _ENS_FSM_EVENT_H_
#define _ENS_FSM_EVENT_H_

#include "ENS_Redefine_Class.h"

#include "ENS_Fsm.h"
#include <ENS_HwRm.h>

class ENS_FsmEvent {
    public:
        ENS_FsmEvent(int signal) : mSignal(signal) {}
        virtual ~ENS_FsmEvent(void) {}

        int signal(void) const { return mSignal; }

    private:
        int     mSignal;
};

// Events used by OMX Component FSM and OMX Port FSM
enum {
    ENTRY_SIG,
    EXIT_SIG,
    OMX_SETSTATE_SIG,
    OMX_SETSTATECOMPLETE_SIG,
    OMX_DISABLE_PORT_SIG,
    OMX_ENABLE_PORT_SIG,
    OMX_SETPARAMETER_SIG,
    OMX_GETPARAMETER_SIG,
    OMX_SETCONFIG_SIG,
    OMX_GETCONFIG_SIG,
    OMX_GETEXTENSIONIDX_SIG,
    OMX_SET_CALLBACKS_SIG,
    OMX_COMP_TUNNEL_REQUEST_SIG,
    OMX_USE_BUFFER_SIG,
    OMX_ALLOCATE_BUFFER_SIG,
    OMX_FREE_BUFFER_SIG,
    OMX_EMPTY_THIS_BUFFER_SIG,
    OMX_FILL_THIS_BUFFER_SIG,
    OMX_EMPTY_BUFFER_DONE_SIG,
    OMX_FILL_BUFFER_DONE_SIG,
    OMX_UPDATE_PORT_SETTINGS_SIG,
    OMX_DISABLEPORTCOMPLETE_SIG,
    OMX_ENABLEPORTCOMPLETE_SIG,
    OMX_UPDATESTATE_SIG,
    OMX_RESRESERVATIONPROCESSED_SIG,
    OMX_RELEASERESOURCE_SIG,
    OMX_FLUSHPORTCOMPLETE_SIG,
    OMX_FLUSH_PORT_SIG,
    OMX_USE_SHARED_BUFFER_SIG,
    OMX_EMPTY_THIS_SHARED_BUFFER_SIG,
    OMX_FILL_THIS_SHARED_BUFFER_SIG
};

class ENS_EntryEvt: public ENS_FsmEvent {
    public:
        ENS_EntryEvt(FsmState st) : ENS_FsmEvent(ENTRY_SIG) {
            mPreviousState = st;
        }

        FsmState previousState(void) const { return mPreviousState; }

    private:
        FsmState mPreviousState;
};


class ENS_CmdResReservationProcessedEvt: public ENS_FsmEvent {
    public:
        ENS_CmdResReservationProcessedEvt(OMX_BOOL bResourcesReservGranted) : ENS_FsmEvent(OMX_RESRESERVATIONPROCESSED_SIG) {
            mResGranted = bResourcesReservGranted;
        }
        OMX_BOOL resGranted(void) const { return mResGranted; }

    private:
        OMX_BOOL mResGranted;
};


class ENS_CmdReleaseResourceEvt: public ENS_FsmEvent {
    public:
        ENS_CmdReleaseResourceEvt(OMX_BOOL bSuspend) : ENS_FsmEvent(OMX_RELEASERESOURCE_SIG) {
            mSuspend = bSuspend;
        }

        OMX_BOOL resSuspend(void) const { return mSuspend; }

    private:
        OMX_BOOL mSuspend;
}; 


class ENS_CmdSetStateEvt: public ENS_FsmEvent {
    public:
        ENS_CmdSetStateEvt(OMX_STATETYPE targetState) : ENS_FsmEvent(OMX_SETSTATE_SIG) {
            mTargetState = targetState;
        }

        OMX_STATETYPE targetState(void) const { return mTargetState; }

    private:
        OMX_STATETYPE mTargetState;
};

class ENS_CmdPortEvt: public ENS_FsmEvent {
    public:
        ENS_CmdPortEvt(int signal, OMX_U32 nPortIndex) : ENS_FsmEvent(signal) {
            mPortIndex = nPortIndex;
        }

        OMX_U32 getPortIdx(void) const { return mPortIndex; }

    private:
        OMX_U32 mPortIndex;
};

class ENS_CmdSetStateCompleteEvt: public ENS_FsmEvent {
    public:
        ENS_CmdSetStateCompleteEvt(OMX_STATETYPE newState) : ENS_FsmEvent(OMX_SETSTATECOMPLETE_SIG) {
            mNewState = newState;
        }
        OMX_STATETYPE newState(void) const { return mNewState; }
    private:
        OMX_STATETYPE mNewState;
};

typedef struct ENS_PORT_INDEX_STRUCT {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
} ENS_PORT_INDEX_STRUCT;

class ENS_CmdParamEvt: public ENS_FsmEvent {
    public:
        ENS_CmdParamEvt(int signal, OMX_INDEXTYPE nIndex, OMX_PTR pStruct) 
                : ENS_FsmEvent(signal) {
            mIndex = nIndex;
            mStructPtr = pStruct;
        }
        OMX_INDEXTYPE getIndex(void) const { return mIndex; }
        OMX_PTR getStructPtr(void) const { return mStructPtr; }
    private:
        OMX_INDEXTYPE           mIndex;
        OMX_PTR                 mStructPtr;
};

class ENS_CmdSetParamEvt: public ENS_CmdParamEvt {
    public:
        ENS_CmdSetParamEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) 
                : ENS_CmdParamEvt(OMX_SETPARAMETER_SIG, nIndex, pParamStruct) { }
};

class ENS_CmdGetParamEvt: public ENS_CmdParamEvt {
    public:
        ENS_CmdGetParamEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) 
                : ENS_CmdParamEvt(OMX_GETPARAMETER_SIG, nIndex, pParamStruct) { }
};

class ENS_CmdSetConfigEvt: public ENS_CmdParamEvt {
    public:
        ENS_CmdSetConfigEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) 
                : ENS_CmdParamEvt(OMX_SETCONFIG_SIG, nIndex, pParamStruct) { }
};

class ENS_CmdGetConfigEvt: public ENS_CmdParamEvt {
    public:
        ENS_CmdGetConfigEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) 
                : ENS_CmdParamEvt(OMX_GETCONFIG_SIG, nIndex, pParamStruct) { }
};

class ENS_CmdUpdateSettingsEvt: public ENS_CmdParamEvt {
    public:
        ENS_CmdUpdateSettingsEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) 
                : ENS_CmdParamEvt(OMX_UPDATE_PORT_SETTINGS_SIG, nIndex, pParamStruct) { }
};

class ENS_CmdGetExtIdxEvt: public ENS_FsmEvent {
    public:
        ENS_CmdGetExtIdxEvt(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) 
                : ENS_FsmEvent(OMX_GETEXTENSIONIDX_SIG) {
            mParamName = cParameterName;
            mIndexTypePtr = pIndexType;
        }

        OMX_STRING getParamName(void) const { return mParamName; }
        OMX_INDEXTYPE* getIndexTypePtr(void) const { return mIndexTypePtr; }

    private:
        OMX_STRING mParamName;
        OMX_INDEXTYPE* mIndexTypePtr;
};

class ENS_CmdSetCbEvt: public ENS_FsmEvent {
    public:
        ENS_CmdSetCbEvt(OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData) 
                : ENS_FsmEvent(OMX_SET_CALLBACKS_SIG) {
            mCbStructPtr = pCallbacks;
            mAppDataPtr = pAppData;
        }

        OMX_CALLBACKTYPE* getCbStructPtr(void) const { return mCbStructPtr; }
        OMX_PTR getAppDataPtr(void) const { return mAppDataPtr; }
        
    private:
        OMX_CALLBACKTYPE*   mCbStructPtr;
        OMX_PTR             mAppDataPtr;
};

class ENS_CmdBufferEvt: public ENS_FsmEvent {
    public:
        ENS_CmdBufferEvt(int signal, OMX_BUFFERHEADERTYPE** ppBufferHdr, 
                OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, 
                OMX_U8* pBuffer) : ENS_FsmEvent(signal) {
            mPtrToBufferHdrPtr = ppBufferHdr;
            mPortIndex = nPortIndex;
            mAppPrivate = pAppPrivate;
            mSizeBytes = nSizeBytes;
            mBufferPtr = pBuffer;
        }

        OMX_BUFFERHEADERTYPE** getOMXBufferHdrPtr(void) const { return mPtrToBufferHdrPtr; }
        OMX_U32 getPortIdx(void) const { return mPortIndex; }
        OMX_PTR getAppPrivate(void) const { return mAppPrivate; }
        OMX_U32 getSizeBytes(void) const { return mSizeBytes; }
        OMX_U8* getBufferPtr(void) const { return mBufferPtr; }

    private:
        OMX_BUFFERHEADERTYPE**  mPtrToBufferHdrPtr;
        OMX_U32                 mPortIndex;
        OMX_PTR                 mAppPrivate;
        OMX_U32                 mSizeBytes;
        OMX_U8*                 mBufferPtr;
};

class ENS_CmdUseBufferEvt: public ENS_CmdBufferEvt {
    public:
        ENS_CmdUseBufferEvt(OMX_BUFFERHEADERTYPE** ppBufferHdr, 
                OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, 
                OMX_U8* pBuffer) : ENS_CmdBufferEvt(OMX_USE_BUFFER_SIG, ppBufferHdr,
                nPortIndex, pAppPrivate, nSizeBytes, pBuffer) {}
};


class ENS_CmdUseSharedBufferEvt: public ENS_CmdBufferEvt {
    public:
        void *mSharedChunk;
        void* getSharedChunk() const { return mSharedChunk; }
        
        ENS_CmdUseSharedBufferEvt(OMX_BUFFERHEADERTYPE** ppBufferHdr, 
                OMX_U32 nPortIndex,OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, 
                OMX_U8* pBuffer,void *pSharedChunk) : ENS_CmdBufferEvt(OMX_USE_SHARED_BUFFER_SIG, ppBufferHdr,
                nPortIndex,pAppPrivate, nSizeBytes, pBuffer) {
                    mSharedChunk = pSharedChunk;
                }
};


class ENS_CmdAllocBufferEvt: public ENS_CmdBufferEvt {
    public:
        ENS_CmdAllocBufferEvt(OMX_BUFFERHEADERTYPE** ppBufferHdr, 
                OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes) 
            : ENS_CmdBufferEvt(OMX_ALLOCATE_BUFFER_SIG, ppBufferHdr, nPortIndex, 
                    pAppPrivate, nSizeBytes, 0) {}
};

class ENS_CmdFreeBufferEvt: public ENS_FsmEvent {
    public:
        ENS_CmdFreeBufferEvt(OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE* pBufferHdr) 
                : ENS_FsmEvent(OMX_FREE_BUFFER_SIG) {
            mPortIdx = nPortIndex;
            mBufferHdrPtr = pBufferHdr;
        }

        OMX_U32 getPortIdx() const { return mPortIdx; }
        OMX_BUFFERHEADERTYPE* getOMXBufferHdrPtr(void) const { return mBufferHdrPtr; }

    private:
        OMX_U32                 mPortIdx;
        OMX_BUFFERHEADERTYPE*   mBufferHdrPtr;
};

class ENS_CmdTunnelRequestEvt: public ENS_FsmEvent {
    public:
        ENS_CmdTunnelRequestEvt(OMX_HANDLETYPE hTunneledComp,
                OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup) 
                : ENS_FsmEvent(OMX_COMP_TUNNEL_REQUEST_SIG) {
            mTunneledComp = hTunneledComp;
            mTunneledPortIdx = nTunneledPort;
            mTunnelStructPtr = pTunnelSetup;
        }
        
        OMX_HANDLETYPE getTunneledCompHdl(void) const { return mTunneledComp; }
        OMX_U32 getTunneledPortIdx(void) const { return mTunneledPortIdx; }
        OMX_TUNNELSETUPTYPE* getTunnelStructPtr(void) const { return mTunnelStructPtr; }
        
    private:
        OMX_HANDLETYPE          mTunneledComp;
        OMX_U32                 mTunneledPortIdx;
        OMX_TUNNELSETUPTYPE*    mTunnelStructPtr;
};

class ENS_CmdBufferHdrEvt: public ENS_FsmEvent {
    public:
        ENS_CmdBufferHdrEvt(int signal, OMX_BUFFERHEADERTYPE* pBufferHdr) 
                : ENS_FsmEvent(signal) {
            mBufferHdrPtr = pBufferHdr;
        }
        OMX_BUFFERHEADERTYPE* getOMXBufferHdrPtr(void) const { return mBufferHdrPtr; }
    private:
        OMX_BUFFERHEADERTYPE*   mBufferHdrPtr;
};

class ENS_CmdEmptyThisBufferEvt: public ENS_CmdBufferHdrEvt {
    public:
        ENS_CmdEmptyThisBufferEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) 
                : ENS_CmdBufferHdrEvt(OMX_EMPTY_THIS_BUFFER_SIG, pBufferHdr) {}
};

class ENS_CmdFillThisBufferEvt: public ENS_CmdBufferHdrEvt {
    public:
        ENS_CmdFillThisBufferEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) 
                : ENS_CmdBufferHdrEvt(OMX_FILL_THIS_BUFFER_SIG, pBufferHdr) {}
};


class ENS_CmdEmptyThisSharedBufferEvt: public ENS_CmdBufferHdrEvt {
    public:
        ENS_CmdEmptyThisSharedBufferEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) 
                : ENS_CmdBufferHdrEvt(OMX_EMPTY_THIS_SHARED_BUFFER_SIG, pBufferHdr) {}
};


class ENS_CmdFillThisSharedBufferEvt: public ENS_CmdBufferHdrEvt {
    public:
        ENS_CmdFillThisSharedBufferEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) 
                : ENS_CmdBufferHdrEvt(OMX_FILL_THIS_SHARED_BUFFER_SIG, pBufferHdr) {}
};


class ENS_CmdEmptyBufferDoneEvt: public ENS_CmdBufferHdrEvt {
    public:
        ENS_CmdEmptyBufferDoneEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) 
                : ENS_CmdBufferHdrEvt(OMX_EMPTY_BUFFER_DONE_SIG, pBufferHdr) {}
};

class ENS_CmdFillBufferDoneEvt: public ENS_CmdBufferHdrEvt {
    public:
        ENS_CmdFillBufferDoneEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) 
                : ENS_CmdBufferHdrEvt(OMX_FILL_BUFFER_DONE_SIG, pBufferHdr) {}
};
#endif // _ENS_FSM_EVENT_H
