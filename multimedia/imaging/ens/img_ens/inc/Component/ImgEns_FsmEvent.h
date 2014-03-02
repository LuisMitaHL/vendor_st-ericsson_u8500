/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*****************************************************************************/
#ifndef _IMGENS_FSM_EVENT_H_
#define _IMGENS_FSM_EVENT_H_

#include "ImgEns_Fsm.h"

// Events used by OMX Component FSM and OMX Port FSM
enum {
	OMX_SETSTATE_SIG  = ImgEns_Fsm::eSignal_LastEnum,
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


class ImgEns_CmdResReservationProcessedEvt: public ImgEns_FsmEvent
{
public:
	ImgEns_CmdResReservationProcessedEvt(OMX_BOOL bResourcesReservGranted) 
		: ImgEns_FsmEvent(OMX_RESRESERVATIONPROCESSED_SIG)
	{
		mResGranted = bResourcesReservGranted;
	}
	OMX_BOOL resGranted(void) const { return mResGranted; }

private:
	OMX_BOOL mResGranted;
};


class ImgEns_CmdReleaseResourceEvt: public ImgEns_FsmEvent
{
public:
	ImgEns_CmdReleaseResourceEvt(OMX_BOOL bSuspend) :
			ImgEns_FsmEvent(OMX_RELEASERESOURCE_SIG)
	{
		mSuspend = bSuspend;
	}

	OMX_BOOL resSuspend(void) const {
		return mSuspend;
	}

private:
	OMX_BOOL mSuspend;
};

class ImgEns_CmdSetStateEvt: public ImgEns_FsmEvent
{
public:
	ImgEns_CmdSetStateEvt(OMX_STATETYPE targetState) :
			ImgEns_FsmEvent(OMX_SETSTATE_SIG) {
		mTargetState = targetState;
	}

	OMX_STATETYPE targetState(void) const {
		return mTargetState;
	}

private:
	OMX_STATETYPE mTargetState;
};

class ImgEns_CmdPortEvt: public ImgEns_FsmEvent
{
public:
	ImgEns_CmdPortEvt(int signal, OMX_U32 nPortIndex) :
			ImgEns_FsmEvent(signal) {
		mPortIndex = nPortIndex;
	}

	OMX_U32 getPortIdx(void) const {
		return mPortIndex;
	}

private:
	OMX_U32 mPortIndex;
};

class ImgEns_CmdSetStateCompleteEvt: public ImgEns_FsmEvent {
public:
	ImgEns_CmdSetStateCompleteEvt(OMX_STATETYPE newState) :
			ImgEns_FsmEvent(OMX_SETSTATECOMPLETE_SIG) {
		mNewState = newState;
	}
	OMX_STATETYPE newState(void) const {
		return mNewState;
	}
private:
	OMX_STATETYPE mNewState;
};

typedef struct ImgEns_PORT_INDEX_STRUCT {
	OMX_U32         nSize;
	OMX_VERSIONTYPE nVersion;
	OMX_U32         nPortIndex;
} ImgEns_PORT_INDEX_STRUCT;

class ImgEns_CmdParamEvt: public ImgEns_FsmEvent {
public:
	ImgEns_CmdParamEvt(int signal, OMX_INDEXTYPE nIndex, OMX_PTR pStruct) :
			ImgEns_FsmEvent(signal) {
		mIndex = nIndex;
		mStructPtr = pStruct;
	}
	OMX_INDEXTYPE getIndex(void) const {
		return mIndex;
	}
	OMX_PTR getStructPtr(void) const {
		return mStructPtr;
	}
private:
	OMX_INDEXTYPE mIndex;
	OMX_PTR mStructPtr;
};

class ImgEns_CmdSetParamEvt: public ImgEns_CmdParamEvt {
public:
	ImgEns_CmdSetParamEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) :
			ImgEns_CmdParamEvt(OMX_SETPARAMETER_SIG, nIndex, pParamStruct) {
	}
};

class ImgEns_CmdGetParamEvt: public ImgEns_CmdParamEvt {
public:
	ImgEns_CmdGetParamEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) :
			ImgEns_CmdParamEvt(OMX_GETPARAMETER_SIG, nIndex, pParamStruct) {
	}
};

class ImgEns_CmdSetConfigEvt: public ImgEns_CmdParamEvt {
public:
	ImgEns_CmdSetConfigEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) :
			ImgEns_CmdParamEvt(OMX_SETCONFIG_SIG, nIndex, pParamStruct) {
	}
};

class ImgEns_CmdGetConfigEvt: public ImgEns_CmdParamEvt {
public:
	ImgEns_CmdGetConfigEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) :
			ImgEns_CmdParamEvt(OMX_GETCONFIG_SIG, nIndex, pParamStruct) {
	}
};

class ImgEns_CmdUpdateSettingsEvt: public ImgEns_CmdParamEvt {
public:
	ImgEns_CmdUpdateSettingsEvt(OMX_INDEXTYPE nIndex, OMX_PTR pParamStruct) :
			ImgEns_CmdParamEvt(OMX_UPDATE_PORT_SETTINGS_SIG, nIndex,
					pParamStruct) {
	}
};

class ImgEns_CmdGetExtIdxEvt: public ImgEns_FsmEvent 
{
public:
	ImgEns_CmdGetExtIdxEvt(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) :
			ImgEns_FsmEvent(OMX_GETEXTENSIONIDX_SIG) {
		mParamName = cParameterName;
		mIndexTypePtr = pIndexType;
	}
	OMX_STRING getParamName(void) const { return mParamName; }
	OMX_INDEXTYPE* getIndexTypePtr(void) const {return mIndexTypePtr; }

private:
	OMX_STRING mParamName;
	OMX_INDEXTYPE* mIndexTypePtr;
};

class ImgEns_CmdSetCbEvt: public ImgEns_FsmEvent
{
public:
	ImgEns_CmdSetCbEvt(const OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData) :
		ImgEns_FsmEvent(OMX_SET_CALLBACKS_SIG) 
	{
		mCbStructPtr = pCallbacks;
		mAppDataPtr = pAppData;
	}
	const OMX_CALLBACKTYPE* getCbStructPtr(void) const { return mCbStructPtr; }
	OMX_PTR getAppDataPtr(void) const { return mAppDataPtr; }
private:
	const OMX_CALLBACKTYPE* mCbStructPtr;
	OMX_PTR mAppDataPtr;
};

class ImgEns_CmdBufferEvt: public ImgEns_FsmEvent 
{
public:
	ImgEns_CmdBufferEvt(int signal, OMX_BUFFERHEADERTYPE** ppBufferHdr,
			OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes,
			OMX_U8* pBuffer) :
			ImgEns_FsmEvent(signal) {
		mPtrToBufferHdrPtr = ppBufferHdr;
		mPortIndex = nPortIndex;
		mAppPrivate = pAppPrivate;
		mSizeBytes = nSizeBytes;
		mBufferPtr = pBuffer;
	}

	OMX_BUFFERHEADERTYPE** getOMXBufferHdrPtr(void) const {
		return mPtrToBufferHdrPtr;
	}
	OMX_U32 getPortIdx(void) const {
		return mPortIndex;
	}
	OMX_PTR getAppPrivate(void) const {
		return mAppPrivate;
	}
	OMX_U32 getSizeBytes(void) const {
		return mSizeBytes;
	}
	OMX_U8* getBufferPtr(void) const {
		return mBufferPtr;
	}

private:
	OMX_BUFFERHEADERTYPE** mPtrToBufferHdrPtr;
	OMX_U32 mPortIndex;
	OMX_PTR mAppPrivate;
	OMX_U32 mSizeBytes;
	OMX_U8* mBufferPtr;
};

class ImgEns_CmdUseBufferEvt: public ImgEns_CmdBufferEvt {
public:
	ImgEns_CmdUseBufferEvt(OMX_BUFFERHEADERTYPE** ppBufferHdr,
			OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes,
			OMX_U8* pBuffer) :
			ImgEns_CmdBufferEvt(OMX_USE_BUFFER_SIG, ppBufferHdr, nPortIndex,
					pAppPrivate, nSizeBytes, pBuffer) {
	}
};

class ImgEns_CmdUseSharedBufferEvt: public ImgEns_CmdBufferEvt {
public:
	void *mSharedChunk;
	void* getSharedChunk() const {
		return mSharedChunk;
	}

	ImgEns_CmdUseSharedBufferEvt(OMX_BUFFERHEADERTYPE** ppBufferHdr,
			OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes,
			OMX_U8* pBuffer, void *pSharedChunk) :
			ImgEns_CmdBufferEvt(OMX_USE_SHARED_BUFFER_SIG, ppBufferHdr,
					nPortIndex, pAppPrivate, nSizeBytes, pBuffer) {
		mSharedChunk = pSharedChunk;
	}
};

class ImgEns_CmdAllocBufferEvt: public ImgEns_CmdBufferEvt {
public:
	ImgEns_CmdAllocBufferEvt(OMX_BUFFERHEADERTYPE** ppBufferHdr,
			OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes) :
			ImgEns_CmdBufferEvt(OMX_ALLOCATE_BUFFER_SIG, ppBufferHdr,
					nPortIndex, pAppPrivate, nSizeBytes, 0) {
	}
};

class ImgEns_CmdFreeBufferEvt: public ImgEns_FsmEvent {
public:
	ImgEns_CmdFreeBufferEvt(OMX_U32 nPortIndex,
			OMX_BUFFERHEADERTYPE* pBufferHdr) :
			ImgEns_FsmEvent(OMX_FREE_BUFFER_SIG) {
		mPortIdx = nPortIndex;
		mBufferHdrPtr = pBufferHdr;
	}

	OMX_U32 getPortIdx() const {
		return mPortIdx;
	}
	OMX_BUFFERHEADERTYPE* getOMXBufferHdrPtr(void) const {
		return mBufferHdrPtr;
	}

private:
	OMX_U32 mPortIdx;
	OMX_BUFFERHEADERTYPE* mBufferHdrPtr;
};

class ImgEns_CmdTunnelRequestEvt: public ImgEns_FsmEvent {
public:
	ImgEns_CmdTunnelRequestEvt(OMX_HANDLETYPE hTunneledComp,
			OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup) :
			ImgEns_FsmEvent(OMX_COMP_TUNNEL_REQUEST_SIG) {
		mTunneledComp = hTunneledComp;
		mTunneledPortIdx = nTunneledPort;
		mTunnelStructPtr = pTunnelSetup;
	}

	OMX_HANDLETYPE getTunneledCompHdl(void) const {
		return mTunneledComp;
	}
	OMX_U32 getTunneledPortIdx(void) const {
		return mTunneledPortIdx;
	}
	OMX_TUNNELSETUPTYPE* getTunnelStructPtr(void) const {
		return mTunnelStructPtr;
	}

private:
	OMX_HANDLETYPE mTunneledComp;
	OMX_U32 mTunneledPortIdx;
	OMX_TUNNELSETUPTYPE* mTunnelStructPtr;
};

class ImgEns_CmdBufferHdrEvt: public ImgEns_FsmEvent {
public:
	ImgEns_CmdBufferHdrEvt(int signal, OMX_BUFFERHEADERTYPE* pBufferHdr) :
			ImgEns_FsmEvent(signal) {
		mBufferHdrPtr = pBufferHdr;
	}
	OMX_BUFFERHEADERTYPE* getOMXBufferHdrPtr(void) const {
		return mBufferHdrPtr;
	}
private:
	OMX_BUFFERHEADERTYPE* mBufferHdrPtr;
};

class ImgEns_CmdEmptyThisBufferEvt: public ImgEns_CmdBufferHdrEvt {
public:
	ImgEns_CmdEmptyThisBufferEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) :
			ImgEns_CmdBufferHdrEvt(OMX_EMPTY_THIS_BUFFER_SIG, pBufferHdr) {
	}
};

class ImgEns_CmdFillThisBufferEvt: public ImgEns_CmdBufferHdrEvt {
public:
	ImgEns_CmdFillThisBufferEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) :
			ImgEns_CmdBufferHdrEvt(OMX_FILL_THIS_BUFFER_SIG, pBufferHdr)
	{
	}
};

class ImgEns_CmdEmptyThisSharedBufferEvt: public ImgEns_CmdBufferHdrEvt
{
public:
	ImgEns_CmdEmptyThisSharedBufferEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) :
			ImgEns_CmdBufferHdrEvt(OMX_EMPTY_THIS_SHARED_BUFFER_SIG, pBufferHdr) {
	}
};

class ImgEns_CmdFillThisSharedBufferEvt: public ImgEns_CmdBufferHdrEvt
{
public:
	ImgEns_CmdFillThisSharedBufferEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) :
			ImgEns_CmdBufferHdrEvt(OMX_FILL_THIS_SHARED_BUFFER_SIG, pBufferHdr) {
	}
};

class ImgEns_CmdEmptyBufferDoneEvt: public ImgEns_CmdBufferHdrEvt {
public:
	ImgEns_CmdEmptyBufferDoneEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) :
			ImgEns_CmdBufferHdrEvt(OMX_EMPTY_BUFFER_DONE_SIG, pBufferHdr) {
	}
};

class ImgEns_CmdFillBufferDoneEvt: public ImgEns_CmdBufferHdrEvt {
public:
	ImgEns_CmdFillBufferDoneEvt(OMX_BUFFERHEADERTYPE* pBufferHdr) :
			ImgEns_CmdBufferHdrEvt(OMX_FILL_BUFFER_DONE_SIG, pBufferHdr) {
	}
};
#endif // _IMGENS_FSM_EVENT_H_
