/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#define OMX_TRACE_UID 0x30
#define OMXCOMPONENT 
#include "osi_trace.h"

#include "OMX_Core.h"
#include "ENS_Component_Fsm.h"
#include "cm/inc/cm_macros.h"
#include "OMX_Index.h"
#include "OMX_Core.h"
#include <stddef.h>
#include "splitter_common.h"

#include <IFM_Index.h>
#include "IFM_Types.h"

#include "wrapper_openmax_tools.h"
#include "openmax_component.h"
#include "splitter_process.h"


//#define TIMESTAMP_PRINT
#ifdef TIMESTAMP_PRINT
#include <sys/time.h>
#endif


#define SPLITTER_CORE_FIFO_ACTUAL_SIZE_UNDEFINED 0


#ifdef __EPOC32__
// Prevent export of symbols
NONSHARABLE_CLASS(Splitter_process);
#endif

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef    OMXCOMPONENT
#define   OMXCOMPONENT GetComponentName()

#ifdef REFERENCE_COUNT
//Static instance 
ReferenceCount Splitter_process::m_Count;
#endif


Splitter_process::Splitter_process()
//*************************************************************************************************************
{
	IN0("");
#ifdef REFERENCE_COUNT
	m_Count.Ref();
	if (m_Count==1)
		SetComponentName("Splitter_process");
	else
	{
		sprintf(m_UserName, "Splitter_process%d", (unsigned int)m_Count);
		SetComponentName(m_UserName);
	}
	MSG1("Construction of Splitter_process(%s)\n", m_UserName);
#else
	MSG0("Construction of Splitter_process\n");
	SetComponentName("Splitter_process");
#endif
	//m_SplitterMode=eSplitter_Broadcast; //Default mode
    SetSplitterMode(eSplitter_Broadcast);
	mContext = NULL; // To avoid problem in destructor in case fsmInit() hasn't been called.
	OUT0(" ");
}

Splitter_process::~Splitter_process()
//*************************************************************************************************************
{
#ifdef REFERENCE_COUNT
	m_Count.UnRef();
#endif
	if (mContext)
		delete []mContext;
	mContext = NULL;
}

int Splitter_process::UserOpen()
//*************************************************************************************************************
{ //Make the initialisation of ports
	IN0("");

	/* Ports data defaults*/
	for(int portIndex=0; portIndex<eSplitter_PortNbr; portIndex++) 
	{ /* For all ports: set buffer fifo size as undefined yet*/
		mPortInfo[portIndex].fifoActualSize  = SPLITTER_CORE_FIFO_ACTUAL_SIZE_UNDEFINED;
	}

	int inputBuffersCount = m_pPort[eSplitter_PortIndex_Input].bufferCount();

	MSG3("Port[%d](%s) queue size = %d\n", eSplitter_PortIndex_Input, portName(eSplitter_PortIndex_Input), inputBuffersCount);
	mContext = new SplitterContext[inputBuffersCount];
	mContextCount = inputBuffersCount;
	if( mContext == NULL ) 
	{
		MSG0("Failed to allocate the splitter context data structure\n");
	}
	if (m_bUseBufferSharing==true)
	{ //Buffer sharing between port 0 and 1, 2
		MSG0("****************************Splitter_process: buffer sharing setBufferResetReqStatus \n");
		GetPort(0) ->setBufferResetReqStatus(false);
	}
	else
	{
		MSG0("****************************Splitter_process: buffer sharing disabled \n");
	}
	OUT0(" ");
	return S_OK;
}



int Splitter_process::GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue)
//*************************************************************************************************************
{
	return 0;
}

void Splitter_process::sendCommand(OMX_COMMANDTYPE cmd, t_uword param)
//*************************************************************************************************************
{
	IN0("");

	if(OMX_StateIdle == (OMX_STATETYPE)param)
	{
	}
	Component::sendCommand(cmd, param);
	OUT0(" ");
}


void Splitter_process::process()
//*************************************************************************************************************
{
	IN0("");
	int portIndex = 0;
	int slotIndex = -1;
	OMX_BUFFERHEADERTYPE* pInputBufferHeader  = NULL;
	OMX_BUFFERHEADERTYPE* pOutputBufferHeader = NULL;

	if (m_bUseBufferSharing==true)
	{
		process_buffersharing();
		OUT0(" ");
		return;
	}


	/* For debug: dump ports status*/
	for( portIndex=0; portIndex<eSplitter_PortNbr ; portIndex++) 
	{
		if (m_pPort[portIndex].isEnabled())
        {
			MSG3("Entry process Port[%d](%s) %d buffer(s) waiting\n", portIndex, portName(portIndex), m_pPort[portIndex].queuedBufferCount());
	    }
	}


	/* Check if an input buffer is waiting*/
	if(m_pPort[eSplitter_PortIndex_Input].isEnabled() && m_pPort[eSplitter_PortIndex_Input].queuedBufferCount())
	{
		// Seach a new context slot and Unqueue buffer from input
		for(slotIndex=0; slotIndex < mContextCount; slotIndex++)
		{
			if(mContext[slotIndex].isFree())
			{	/* Found a free Context entry*/
				pInputBufferHeader = m_pPort[eSplitter_PortIndex_Input].dequeueBuffer();
				mContext[slotIndex].setInputBufferHearder(pInputBufferHeader); 
				MSG4("Port[%d](%s) dequeued buffer header 0x%p. Dispatch to %d context\n", eSplitter_PortIndex_Input, portName(eSplitter_PortIndex_Input), pInputBufferHeader, mContextCount);
				MSG2("ContextSlot[%d] is free => use with buffer 0x%p\n", slotIndex, pInputBufferHeader);
				break;
			}
		}
		if (slotIndex >= mContextCount)
		{
			ReportError(-10, "***********************************No free context found\n");
		}
	}

	int NbDelivered=0; // count the number of delivered buffer

	for(slotIndex=0; slotIndex < mContextCount; slotIndex++)
	{ /* Parse all context and try to serve output ports*/
		if(mContext[slotIndex].isUsed() == false)
			continue; //Go next
		bool waitSequentialBuffer = false;
		if (m_SplitterMode == eSplitter_Sequential)
		{ // For sequential all sent buffer must be returned before sending others
			for( portIndex=eSplitter_PortIndex_Output1; portIndex<=eSplitter_PortIndex_Output_Last /*LR_CHANGE031210 && m_pPort[portIndex].isEnabled()*/; portIndex++)
			{
				if((m_pPort[portIndex].isEnabled()) && (mContext[slotIndex].Buffer_Count(portIndex) !=0 ))
				{
					waitSequentialBuffer = true;
					break;
				}
			}
		}

		if(!waitSequentialBuffer)
		{
			for( portIndex=eSplitter_PortIndex_Output1; portIndex<=eSplitter_PortIndex_Output_Last; portIndex++)
			{
				/* Port is enabled => try to serve it*/
				if( (m_pPort[portIndex].isEnabled()) && (!mContext[slotIndex].isOutputServed(portIndex)) )
				{
					MSG2("Port[%d](%s) need to be served\n", portIndex, portName(portIndex));
					/* Try to serve port*/
					if(m_pPort[portIndex].queuedBufferCount())
					{
						// port is populated => unqueue one buffer
						pOutputBufferHeader = m_pPort[portIndex].dequeueBuffer();
						pInputBufferHeader = mContext[slotIndex].getInputBufferHearder();
						MSG3("Port[%d](%s) dequeued buffer header 0x%p\n", portIndex, portName(portIndex), pOutputBufferHeader);
						MSG3("Buffer Fill len= %u alloc size input=%u, output=%u \n", (unsigned int)pInputBufferHeader->nFilledLen, (unsigned int)pInputBufferHeader->nAllocLen, (unsigned int)pOutputBufferHeader->nAllocLen);
						WOM_ASSERT(pInputBufferHeader->nAllocLen <= pOutputBufferHeader->nAllocLen);
						size_t size=MIN(pInputBufferHeader->nAllocLen, pOutputBufferHeader->nAllocLen);
						
						/* Copy payload from input to output*/
						memcpy( pOutputBufferHeader->pBuffer, pInputBufferHeader->pBuffer, size);

						WOM_ASSERT(pInputBufferHeader->nFilledLen <= size); //Check nFilledLen size
						size=MIN(pInputBufferHeader->nFilledLen, size);
						pOutputBufferHeader->nFilledLen = size;
						pOutputBufferHeader->nOffset    = pInputBufferHeader->nOffset;
						pOutputBufferHeader->nFlags     = pInputBufferHeader->nFlags;

						if((pInputBufferHeader->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK )!=0)
						{// If End of Stream reached, then generate an event
							MSG2("< send EOS to Port[%d](%s)\n", portIndex, portName(portIndex));
							proxy.eventHandler(OMX_EventBufferFlag, portIndex, (pOutputBufferHeader->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK));
						}
						// Return the buffer to the output queue (this triggers 'emptyThisBuffer')
						m_pPort[portIndex].returnBuffer(pOutputBufferHeader);
						MSG3("Port[%d](%s) Send  buffer header 0x%p\n", portIndex, portName(portIndex), pOutputBufferHeader);
						mContext[slotIndex].BufferSent(portIndex);

						// Set this output port as 'served'
						mContext[slotIndex].setOutputAsServed(portIndex);

						++NbDelivered;

						//to break out of the loop
						if(m_SplitterMode == eSplitter_Sequential)
							portIndex = eSplitter_PortIndex_Output_Last + 1;
					}
				}
			}
		}

		// Check whether all ports of this slot have been served
		bool allEnabledPortsServed = true;
		for( portIndex=eSplitter_PortIndex_Output1; portIndex<=eSplitter_PortIndex_Output_Last; portIndex++)
		{
			if( (m_pPort[portIndex].isEnabled()) && (!mContext[slotIndex].isOutputServed(portIndex)) )
			{
				allEnabledPortsServed = false; //Reset
				break;
			}
		}

		if( allEnabledPortsServed )
		{	/* All output ports of this slot have been served => return the input buffer*/
			pInputBufferHeader = mContext[slotIndex].getInputBufferHearder();
			/* Return the buffer to the input queue (this triggers 'fillThisBuffer')*/
			if( (pInputBufferHeader->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK)!=0)
			{// If End of Stream reached, then generate an event
				MSG0("< InputPort EOS detected \n");
				// no eos on input proxy.eventHandler(OMX_EventBufferFlag, eSplitter_PortIndex_Input, (pInputBufferHeader->nFlags & WOM_BUFFERFLAG_CALLBACK_MASK));
			}

			m_pPort[eSplitter_PortIndex_Input].returnBuffer(pInputBufferHeader);
			MSG3("Port[%d](%s) returned buffer header 0x%p\n", eSplitter_PortIndex_Input, portName(eSplitter_PortIndex_Input), pInputBufferHeader);

			/* Free the context entry*/
			mContext[slotIndex].freeEntry();
			MSG2("ContextSlot[%d] freed (was for buffer 0x%p)\n", slotIndex, pInputBufferHeader);
		}
	}

	// For debug: dump ports status
	for( portIndex=0; portIndex< eSplitter_PortNbr; portIndex++) 
	{
		if (m_pPort[portIndex].isEnabled())
        {
			MSG3("Exit process Port[%d](%s) %d buffer(s) waiting\n", portIndex, portName(portIndex), m_pPort[portIndex].queuedBufferCount());
	    }
	}

	// Check whether we need to be resheduled
	bool isInputBufferWaiting = (bool)(m_pPort[eSplitter_PortIndex_Input].isEnabled() && m_pPort[eSplitter_PortIndex_Input].queuedBufferCount()!=0);

	bool isOutputBufferWaiting = false;
	for( portIndex=eSplitter_PortIndex_Output1; portIndex<=eSplitter_PortIndex_Output_Last; portIndex++) 
	{
		if (m_pPort[portIndex].isEnabled())
		{
			if((m_pPort[portIndex].queuedBufferCount()!=0) || (mContext[slotIndex].Buffer_Count(portIndex))) 
			{
				isOutputBufferWaiting = true;
				break;
			}
		}
	}

	if(isInputBufferWaiting && isOutputBufferWaiting) 
	{ //Don't understand why it's an AND and not a OR
		MSG0("Input AND Output have buffers waiting => reschedule\n");
		scheduleProcessEvent();
	}
	OUT0(" ");
}

void Splitter_process::process_buffersharing()
//*************************************************************************************************************
{ //Process function when buffer sharing
	IN0("");
	int portIndex = 0;
	OMX_BUFFERHEADERTYPE* pInputBufferHeader  = NULL;

	/* Check if an input buffer is waiting*/
	if(m_pPort[eSplitter_PortIndex_Input].isEnabled() && m_pPort[eSplitter_PortIndex_Input].queuedBufferCount())
	{
		pInputBufferHeader = m_pPort[eSplitter_PortIndex_Input].dequeueBuffer();
		if(pInputBufferHeader->nFlags)
		{ //Input buffer is marked with flags
			for( portIndex=eSplitter_PortIndex_Output1; portIndex<=eSplitter_PortIndex_Output_Last; portIndex++) 
			{
				if (m_pPort[portIndex].isEnabled())
				{ // Send message to enabled port
					proxy.eventHandler(OMX_EventBufferFlag, portIndex, pInputBufferHeader->nFlags);
				}
			}
		}
		MSG1("Splitter_process release input buffer %p\n", pInputBufferHeader);

		GetPort(eSplitter_PortIndex_Input)     ->returnBuffer(pInputBufferHeader);
	}

	OUT0(" ");
}

void Splitter_process::setTunnelStatus(t_sint16 portIndex, t_uint16 isTunneled)
//*************************************************************************************************************
{
	IN0("");
	ARMNMF_DBC_ASSERT( portIndex < eSplitter_PortNbr );
	if (portIndex >= eSplitter_PortNbr )
		return;
	if(portIndex==-1)
	{ //-1 means all
		for(int index = 0 ; index < eSplitter_PortNbr; index++) 
		{
			m_pPort[index].setTunnelStatus((bool)((isTunneled>>index)&0x1));
		}
	}
	else
	{
		m_pPort[portIndex].setTunnelStatus((bool)((isTunneled>>portIndex)&0x1));
	}
	OUT0(" ");
}


void Splitter_process::reset()
//*************************************************************************************************************
{
	IN0("");
	OUT0(" ");
}


void Splitter_process::processEvent(void)
//*************************************************************************************************************
{
	IN0("");
	Component::processEvent();
	OUT0(" ");
}

void Splitter_process::setParam(t_uint32 portIndex, t_uint32 fifoSize, t_uint32 direction, t_uint32 buffSupplierType)
//*************************************************************************************************************
{
	IN0("");
	ARMNMF_DBC_ASSERT( portIndex < eSplitter_PortNbr );
	if (portIndex < eSplitter_PortNbr)
	{
		MSG3("[Port=%u](%s), fifoSize=%u\n", (unsigned int)portIndex, portName(portIndex), (unsigned int)fifoSize);
		MSG2("direction=%d, buffSupplierType=%d\n", (int)direction,  (int)buffSupplierType);

		// check whether negociated fifo size is not too big
		if( fifoSize > SPLITTER_CORE_FIFO_SIZE_MAX)
		{
			MSG3("WARNING: Port[%d] Negociated fifo size (=%d) is greater that internal hardcoded fifo size (=%d)\n", (int)portIndex, (int)fifoSize, SPLITTER_CORE_FIFO_SIZE_MAX);
			ReportError(-1, "Port[%d] Negociated fifo size (=%d) is greater that internal hardcoded fifo size (=%d)\n", (int)portIndex, (int)fifoSize, SPLITTER_CORE_FIFO_SIZE_MAX);
			fifoSize= SPLITTER_CORE_FIFO_SIZE_MAX-1;
		}

		// Assign fifo size (decrease the value if needed)
		mPortInfo[portIndex].fifoActualSize = MIN(fifoSize, SPLITTER_CORE_FIFO_SIZE_MAX);
		MSG3("Port[%d](%s) fifo size set to %d\n",  (int)portIndex, portName(portIndex),  (int)mPortInfo[portIndex].fifoActualSize);
	}
	else
	{
		ReportError(-1, "Invalid port index =%d. Must be < %d)\n", (int)portIndex, SPLITTER_CORE_FIFO_SIZE_MAX);
	}
	OUT0(" ");
}

void Splitter_process::setConfig(t_sint32 index, void* opaque_ptr)
//*************************************************************************************************************
{
	IN0("");
	OUT0(" ");
}


void Splitter_process::fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBufferHeader, t_uint8 nmfOutputPortIdx)
//*************************************************************************************************************
{
#ifdef TIMESTAMP_PRINT
	struct timeval  first;
	struct timezone tzp;
	gettimeofday (&first, &tzp);
	MSG2("Fill this buffer at time=%d.%d\n", first.tv_sec, first.tv_usec);
#endif

	IN0("");
	mContext[0].BufferReceived(nmfOutputPortIdx);
	Component::deliverBuffer( nmfOutputPortIdx, pBufferHeader);
	OUT0(" ");
}

static const char * portNames[] = { "input", "output1", "output2", "output3", "output4"};

inline const char* Splitter_process::portName(int portIndex)
//*************************************************************************************************************
{
	if(portIndex >= 0 && portIndex < eSplitter_PortNbr) 
	{
		return portNames[portIndex];
	}
	else 
	{
		static char invalidIndexMsg[64] = "";
		snprintf( invalidIndexMsg, sizeof(invalidIndexMsg), "invalid port index %d", portIndex);
		return invalidIndexMsg;
	}
}

int Splitter_process::SetSplitterMode(enumSplitterMode mode)
//*************************************************************************************************************
{
	IN0("");

	int err=S_OK;
	if (mode == eSplitter_Sequential)
	{
#ifdef USE_BUFFERSHARING_SEQUENTIAL
		m_bUseBufferSharing=true;
		MSG0("Splitter mode Sequential with buffer sharing\n");
#else
		m_bUseBufferSharing=false;
		MSG0("Splitter mode Sequential\n");
#endif
		m_SplitterMode= eSplitter_Sequential;
	}
	else if (mode == eSplitter_Broadcast)
	{
#ifdef USE_BUFFERSHARING_BROADCAST
		m_bUseBufferSharing=true;
		MSG0("Splitter mode Broadcast with buffer sharing\n");
#else
		m_bUseBufferSharing=false;
		MSG0("Splitter mode Broadcast\n");
#endif
		m_SplitterMode= eSplitter_Broadcast;
	}
	else
	{
		ReportError(-1, "Wrong splitter mode value %d (0 or 1 accepted)", mode);
		err=-1;
	}

	OUTR(" ", err);
	return(err);
}





//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef  OMXCOMPONENT
#define OMXCOMPONENT "SplitterContext"

// Class for managing the buffer dispatching context

#ifdef __EPOC32__
// Prevent export of symbols
NONSHARABLE_CLASS(SplitterContext);
#endif

SplitterContext::SplitterContext()
//*************************************************************************************************************
{
	IN0("");
	MSG0("Splitter Context Init\n");
	init();
	OUT0(" ");
}

SplitterContext::~SplitterContext()
//*************************************************************************************************************
{
	IN0("");
	OUT0(" ");
}

void SplitterContext::init()
//*************************************************************************************************************
{
	pInputBufferHeader = NULL;
	for(int index=0; index<eSplitter_PortNbr; index++) 
	{
		isPortServed[index] = false;
		BufferSentCount[index] = 0;
	}
}

inline bool SplitterContext::isFree() const
//*************************************************************************************************************
{
	if( pInputBufferHeader == NULL )
		return true;
	else
		return false;
}

inline bool SplitterContext::isUsed() const
//*************************************************************************************************************
{
	return !isFree();
}

inline void SplitterContext::freeEntry()
//*************************************************************************************************************
{
	init();
}

inline void SplitterContext::setInputBufferHearder(OMX_BUFFERHEADERTYPE* const pIBH)
//*************************************************************************************************************
{
	pInputBufferHeader = pIBH;
}

inline OMX_BUFFERHEADERTYPE* SplitterContext::getInputBufferHearder()
//*************************************************************************************************************
{
	return pInputBufferHeader;
}

inline void SplitterContext::setOutputAsServed(const int index)
//*************************************************************************************************************
{
	if(index>=eSplitter_PortIndex_Output1 && index<=eSplitter_PortIndex_Output_Last)
	{
		isPortServed[index] = true;
	}
}

inline bool SplitterContext::isOutputServed(const int index) const
//*************************************************************************************************************
{
	if(index>=eSplitter_PortIndex_Output1 && index<=eSplitter_PortIndex_Output_Last)
	{
		return isPortServed[index];
	}
	return false;
}

inline int SplitterContext::Buffer_Count(const int index) const
//*************************************************************************************************************
{
	if(index>=eSplitter_PortIndex_Output1 && index<=eSplitter_PortIndex_Output_Last)
	{
		return BufferSentCount[index];
	}
	return 0;
}

inline void SplitterContext::BufferSent(const int index)
//*************************************************************************************************************
{
	if(index>=eSplitter_PortIndex_Output1 && index<=eSplitter_PortIndex_Output_Last)
	{
		BufferSentCount[index] += 1;
	}
}

inline void SplitterContext::BufferReceived(const int index)
//*************************************************************************************************************
{
	if(index>=eSplitter_PortIndex_Output1 && index<=eSplitter_PortIndex_Output_Last)
	{
		/* To take care of the first set of calls*/
		if(BufferSentCount[index] !=0)
			BufferSentCount[index] -= 1;
	}
}





