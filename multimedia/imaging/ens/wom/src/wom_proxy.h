/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_PROXY_H_
#define _WOM_PROXY_H_

class AsyncProcessor;

typedef Wom_Processor * (*fnCreateAsyncProcessor)(const char *name, void *usrptr);

class WOM_API Async_Proxy
	: public Sync_Proxy
	, public c_ProcessingThread
	, public Interface_ProcessorCB
//*************************************************************************************************************
{
public:
	Async_Proxy(const char *name, ImgEns_Component &comp, Interface_ProcessorToComponent &IProcessorToComponent, fnCreateAsyncProcessor _fnCreateAsyncProcessor);
	virtual ~Async_Proxy();

	Async_Proxy & operator =(const Async_Proxy &) { WOM_ASSERT(0); return(*this); }

	//interface with ImgEns_Component
	virtual OMX_ERRORTYPE construct(void);
	virtual OMX_ERRORTYPE destroy(void);

	virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE fillThisBuffer (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData);
	
public:
	//Asynchronous cmd from proxy to processor
	int SetState(unsigned int state);
	int ScheduleProcess(int why);
	int Marshall_AsyncCommand(int _event, int param1=0, int param2=0, int param3=0);
	virtual int Execute_CBFromProcessor(AsyncMsg &Evt); // overidable
protected:
	static void staticExecute_CBFromProcessor(void *pContext); // After notification from processor via  c_ProcessingThread event

//c_ProcessingThread overide
public:
	virtual void ExecuteIncomingMsg(void* ap_Msg);
	virtual void NotifyOutgoingMsg();

	//Async_Proxy
public:
	// Implementation of Interface_ProcessorCB Interface
	virtual int OmxEventFeedback(int _event, int param1, int param2);
	virtual int RequestProcess(int why);
	virtual int EmptyBufferDone(OMX_BUFFERHEADERTYPE *buff);
	virtual int FillBufferDone(OMX_BUFFERHEADERTYPE *buff);

protected:
	fnCreateAsyncProcessor  m_fnCreateAsyncProcessor; //Function ptr for the creation of the right component
	Wom_Processor         * m_pProcessor;
	unsigned int            m_AsyncProcessPriority;

	c_ProcessingObserver  * m_pObserver; // Needed by c_ProcessingThread

};

typedef Async_Proxy Wom_Proxy;

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************


class WOM_API I_ToEnsComponent: public Interface_ProcessorToComponent
//*************************************************************************************************************
{ //Standard call to ImgEns_Component
public:
	I_ToEnsComponent(ImgEns_Component &Cpt): m_EnsComponent(Cpt) {}
	virtual int SendOmxEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2) { m_EnsComponent.eventHandler(eEvent, nData1, nData2); return(S_OK); }
	virtual int EmptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)                     { m_EnsComponent.emptyBufferDone(pBuffer)            ; return(S_OK); }
	virtual int FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)                     { m_EnsComponent.fillBufferDone(pBuffer)             ; return(S_OK); }
protected:
	ImgEns_Component &m_EnsComponent;
};

#endif //_WOM_PROXY_H_
