/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_ASYNCPROCESSOR_H_
#define _WOM_ASYNCPROCESSOR_H_

class Wom_AsyncPort;
class Wom_Component;

enum
{
	eProcessEvent_ScheduleProcess =  eIMGOMX_EventLast +1,
	eProcessEvent_FillBufferDone,
	eProcessEvent_EmptyBufferDone,
};

struct AsyncMsg
{
	unsigned int Event;
	unsigned int Param1;
	unsigned int Param2;
	unsigned int Param3;
};

enum enumEvent
{
	eProxyEvent_None=0,
	eProxyEvent_EmptyThisBuffer,
	eProxyEvent_FillThisBuffer,
	eProxyEvent_SendCommand,
	eProxyEvent_SetConfig,
	eProxyEvent_SetParam,
	eProxyEvent_ScheduleProcess,
};

enum enumProcessEvent
{
	eProcessEvent_None =0,
	eProcessEvent_Init,
	eProcessEvent_BufferAvailable,
};

struct Interface_ProcessorCB
//*************************************************************************************************************
{ //Interface for callback between processor and proxy
	virtual int OmxEventFeedback(int /*_event*/, int /*param1*/, int /*param2*/)   = 0;
	virtual int RequestProcess(int why)                                            = 0;
	virtual int EmptyBufferDone(OMX_BUFFERHEADERTYPE * buf)                        = 0;
	virtual int FillBufferDone (OMX_BUFFERHEADERTYPE * buf)                        = 0;
};

struct Interface_Processor
//*************************************************************************************************************
{ //Interface for Processor
	virtual void process(void)                           =0;
	virtual void disablePortIndication(t_uint32 portIdx) =0;
	virtual void enablePortIndication (t_uint32 portIdx) =0;
	virtual void flushPortIndication  (t_uint32 portIdx) =0;
};

class WOM_API AsyncProcessor
//*************************************************************************************************************
{ // Manage Processing interface
public:
	AsyncProcessor(const char *name, void *UsrPtr=NULL);
	virtual ~AsyncProcessor() { /*DeInit();*/ };

	Interface_ProcessorCB * Get_Interface_ProcessorCB() const { return(m_pIProcessorCB);}
	void  Set_Interface_ProcessorCB(Interface_ProcessorCB *pInterface)  { m_pIProcessorCB=pInterface;}

	//Interface between Proxy and processor
	virtual int Init(unsigned int NbPort, Interface_ProcessorCB *pInterface);
	virtual int DeInit()=0;
	virtual int SetPortConfig(int /*portIndex*/, int /*hdrcount*/, int /*direction*/, size_t /*buffersize*/, int /*domain*/, bool /*enabled*/) { return(S_OK); }
	virtual int SetPortFormat(int /*portIndex*/, int /*colorFormat*/, int /*width*/, int /*height*/, int /*stride*/, int /*sliceHeight*/) { return(S_OK); }
	
	virtual int Start()                   = 0;
	virtual int Stop()                    = 0;
	virtual int SendAsyncCommand(AsyncMsg &)= 0;
	// End of Interface

	const char *GetComponentName() const { return m_UserName; }
	void        SetComponentName(const char *name);

protected:
	Interface_ProcessorCB *m_pIProcessorCB; // Interface to return parameter to Proxy
	unsigned int           m_NbPort;
	void                 * m_UserPtr;  //Pointer dedicated to user
	char                   m_UserName[WOM_MAX_NAME_SIZE];
};

class WOM_API ProcessorFSM: public WomFsm
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
public:
	ProcessorFSM();
	virtual ~ProcessorFSM();

	enum
	{ // Known signals
		eSignal_Process = WomFsm::eSignal_LastEnum,
		eSignal_SetState,
		eSignal_ReturnBuffer,
		eSignal_Flush,
		eSignal_DisablePort,
		eSignal_EnablePort,
	};

	OMX_STATETYPE getOmxState() const;
	bool isInvalid        (void) const { return getOmxState() == OMX_StateInvalid ; }
	int getPortCount      (void) const { return m_PortCount;}
	bool allPortsDisabled (void) const;
	bool allPortsEnabled  (void) const;
	bool isOnePortFlushing(void) const;

	virtual void stateChangeIndication(OMX_STATETYPE oldState, OMX_STATETYPE newState){ printf("\nProcessor change from %d to %d states", oldState, newState); };

	void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);

	void deliverBuffer(int portIndex, OMX_BUFFERHEADERTYPE *buf);
	void deliverBufferCheck(int portIndex, OMX_BUFFERHEADERTYPE* buf);
	void processEvent(void);

	// Add for trace (to be removed)
	int getId1() const {return(0); }
	void *getTraceInfoPtr() { return(NULL); }

	//Normalement protected
	int init(unsigned int portCount, Wom_AsyncPort *pPorts, Interface_ProcessorCB *pInt, Interface_Processor   * pIProcessor, bool invalid);
	Interface_ProcessorCB *GetInterface_ProcessorCB() const {return(m_pIProcessorCB);}
	Interface_Processor   *GetInterface_Processor()   const {return(m_pIProcessor);}

	void scheduleProcessEvent(void);
protected:
	void returnBufferAsync(OMX_U32 portIdx, OMX_BUFFERHEADERTYPE * buf) ; // To return a buffer when the component is not processing a WomFsm event.

protected:
	void State_Idle                   (const WomOmxEvent *);
	void State_Executing              (const WomOmxEvent *);
	void State_Pause                  (const WomOmxEvent *);
	void State_TransientToIdle        (const WomOmxEvent *);
	void State_Invalid                (const WomOmxEvent *);
	void waitForPortFlushedOrDisabled (const WomOmxEvent *);

	bool postProcessCheck(void) const;
	void updateState(t_FsmState currentState);
	void setState(const WomOmxEvent *evt, t_FsmState currentState);
	t_FsmState getStateFromPorts(void);

protected:
	unsigned int            m_PortCount;
	Wom_AsyncPort          *m_pPorts; // Array of Ports
	bool                    m_bHavePendingEvent;
	OMX_STATETYPE           m_TransientState;
	ste_Mutex               m_Mutex;
	// Interfaces 
	Interface_Processor   * m_pIProcessor;
	Interface_ProcessorCB * m_pIProcessorCB; //For speaking outside the FSM via callback
};

typedef struct portInformation
//*******************************************************************************
{ // Defined for workaround limitation of classes Component that doesn't support derivation of Port
	int                  width;
	int                  height;
	OMX_COLOR_FORMATTYPE colorFormat;
	int                  stride;
	int                  sliceHeight;
	int                  omxPortIndex;
	size_t               bufferCount;
	OMX_DIRTYPE          direction; //Port direction Input or Output
	int                  bufferSupplier;
	size_t               bufferSize;
	int                  domain;
	bool                 enable; // If port enabled
	//Methods
	void InitDefault(); //Make default initialiation
} portInformation;


#define WOM_MAX_NAME_SIZE 32

class WOM_API Wom_Processor: public AsyncProcessor, public Interface_Processor
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
public:
	Wom_Processor(const char *name, void *UsrPtr=NULL);
	virtual ~Wom_Processor();
	ProcessorFSM * GetFsm() const { return(m_pProcessorFsm); }
	Wom_AsyncPort *GetPort(unsigned int index) const { if ( (index >= m_NbPort) || (m_pPort==NULL) ) { WOM_ASSERT(0); return(NULL); } else return (m_pPort + index); }

	//override
	virtual int Init(Wom_Component &Component, unsigned int NbPort, Interface_ProcessorCB *pInterface);
	virtual int DeInit();
	//virtual int SetPortConfig(int portIndex, int fifoSize, int direction, size_t buffersize, int domain, int colorFormat, int width, int height, int stride, int sliceHeight);
	virtual int SetPortConfig(int portIndex, int buffercount, int direction, size_t buffersize, int domain, bool bEnabled);
	virtual int SetPortFormat(int portIndex, int colorFormat, int width, int height, int stride, int sliceHeight);
	virtual int Start();
	virtual int Stop();
	virtual int SendAsyncCommand(AsyncMsg &);
	virtual int SetConfig(size_t index, void* opaque_ptr);

	//Interface_Processor part
	virtual void process(void)                               { WOM_ASSERT(0); } /*  =0 */
	virtual void disablePortIndication(t_uint32 /*portIdx*/) { WOM_ASSERT(0); } /*  =0 */
	virtual void enablePortIndication (t_uint32 /*portIdx*/) { WOM_ASSERT(0); } /*  =0 */
	virtual void flushPortIndication  (t_uint32 /*portIdx*/) { WOM_ASSERT(0); } /*  =0 */

	// New functions
	portInformation* GetPortInfo(size_t index) const {  if ( (index >=m_NbPort) || (m_pPortInfo==NULL) ) { WOM_ASSERT(0); return(NULL);}  return(m_pPortInfo+ index); }

	Wom_Component *GetComponent() const { return(m_pWom_Component); }

	enum enumInternalState
	{
		eInternalState_Idle,
		eInternalState_Init,
		eInternalState_Start,
		eInternalState_Stop,
		eInternalState_DeInit,
	};
	enumInternalState GetInternalState() { return(m_InternalState); }

protected:
	ProcessorFSM          *m_pProcessorFsm; // the processor fsm (couldn't be null
	Wom_AsyncPort         *m_pPort;         // Array of ports
	portInformation       *m_pPortInfo;
	bool                   m_bUseBufferSharing;
	enumInternalState      m_InternalState;
	// C style function for easy interface with C libraries
	int (*m_fn_UserProcessBuffer) (const _tConvertionConfig &Config);
	int (*m_fn_UserClose)         (void **pHdl);
	int (*m_fn_UserOpen )         (const _tConvertionConfig &Config, void **pHdl);
	int (*m_fn_UserSetConfig)     (const _tConvertionConfig &Config, int index, void *ptr);
	int (*m_fn_UserSetTuning)     (const _tConvertionConfig &Config, int index, void *ptr);

	int InitBufferInfo(struct sBufferInfo &Info, const int Index, bool bDequeue);
	int InitProcessingInfo(_tConvertionConfig &Config, bool bDequeue, const int InputIndex, const int OutputIndex, const int MetadataIndex);

	Wom_Component *m_pWom_Component;
};

#endif //_WOM_ASYNCPROCESSOR_H_
