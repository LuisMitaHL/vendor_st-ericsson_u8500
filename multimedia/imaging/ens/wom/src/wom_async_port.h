/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_ASYNC_PORT_H_
#define _WOM_ASYNC_PORT_H_

class ProcessorFSM;

class WOM_API Wom_AsyncPort : public WomFsm
//*************************************************************************************************************
{
public:
	Wom_AsyncPort();

	enum enumDirection
	{
		eDir_Unspecified,
		eDir_Input,
		eDir_Output,
	};

	void init(int PortIndex, enumDirection dir, Wom_AsyncPort * sharingPort, int bufferCount, bool isDisabled, bool isTunneled, ProcessorFSM *componentOwner);
	void Default();

	OMX_STATETYPE getState(void) const;

	void returnBuffer(OMX_BUFFERHEADERTYPE * buf);
	OMX_BUFFERHEADERTYPE * dequeueBuffer();
	void                   requeueBuffer(OMX_BUFFERHEADERTYPE * buf);

	void setTunnelStatus(bool isTunneled);
	void setBufferResetReqStatus(bool flag); //For disabling reset of nfilllenght when return buffer

	bool isInvalid(void) const { return getState() == OMX_StateInvalid ; }
	bool isEnabled(void) const { return m_BufferQueue.getEnabled() ; }

	bool postProcessCheck(void) const;
	void flushBuffers(void);
	bool isFlushing(void) const;

	OMX_BUFFERHEADERTYPE * getBuffer(int n) { return (OMX_BUFFERHEADERTYPE *)m_BufferQueue.getItem(n); }

	int   bufferCount        (void) const         { return m_BufferQueue.getSize();   }
	int   queuedBufferCount  (void) const         { return m_BufferQueue.itemCount(); }
	int   dequeuedBufferCount(void) const         { return m_DequeuedCount;     }

	void  queueBuffer(OMX_BUFFERHEADERTYPE * buf) { m_BufferQueue.push_back(buf);     }
	void  dequeueAndReturnBuffer(void)            { returnBuffer(dequeueBuffer());  }
	void  returnUnprocessedBuffer(OMX_BUFFERHEADERTYPE *buf);

	// trace to be removed
	int setTraceInfo(void *  /*ptr*/, int /*id*/) { return(0); }

	friend class ProcessorFSM;
protected:

private:
	void State_Idle                     (const WomOmxEvent * evt);
	void State_Executing                (const WomOmxEvent * evt);
	void State_Pause                    (const WomOmxEvent * evt);

	void State_PauseFromIdleOrDisabled  (const WomOmxEvent * evt);
	void State_TransientToIdleOrDisabled(const WomOmxEvent * evt);
	void State_TransientToFlush         (const WomOmxEvent * evt);
	void State_Disabled                 (const WomOmxEvent * evt);

	void State_Invalid                  (const WomOmxEvent * evt);

protected:
	void returnBufferInternal(OMX_BUFFERHEADERTYPE * buf);
	void reset(void);
	void transferQueuedBuffersToSharingPort(void);
	void goToIdleOrDisable(void);
	void goToFlushBuffers(void);
	void flushComplete(bool fromTransientToFlushState);

	static t_FsmState translateState(OMX_STATETYPE state);

protected:
	unsigned int    m_Direction;
	Wom_AsyncPort  *m_pSharingPort;
	Wom_PortQueue   m_BufferQueue;

	unsigned int    m_PortIndex;
	OMX_STATETYPE   m_OmxState_Disabled;
	OMX_STATETYPE   m_OmxState_Flush;
	ProcessorFSM   *m_pComponentOwner;
	bool            m_NotifyStateTransition;
	bool            m_IsPortTunneled;
	bool            m_IsBufferResetReq;
	bool            m_EventAppliedOnEachPorts;

// for debug help
	unsigned int    m_DequeuedCount;

// Not use in this version
	bool          m_InternalBufferSupplier; //set when the processor_port is supplier (not the component_port)
	bool          m_IsHWport;
};

#endif //_WOM_ASYNC_PORT_H_
