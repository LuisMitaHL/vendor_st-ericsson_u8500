/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _SPLITTER_PROCESS_H_
#define _SPLITTER_PROCESS_H_

#include "OMX_Types.h"
#include <OMX_Component.h>
#include "Component.h" // because we NMF-extend fsm.component.component
#include "IFM_Types.h"

#include "splitter_common.h"

#include <stddef.h>
#include "wrapper_openmax_lib.h"

#include <OMX_IndexExt.h>
#include <OMX_CoreExt.h>


#define SPLITTER_CORE_FIFO_SIZE_MAX 5

typedef struct
{
	OMX_BUFFERHEADERTYPE bufferHeadersFifo[SPLITTER_CORE_FIFO_SIZE_MAX];
	int                  fifoActualSize;
} SplitterPortInfo_t;

class SplitterContext
//*************************************************************************************************************
{
private:
	OMX_BUFFERHEADERTYPE* pInputBufferHeader;
	bool isPortServed[eSplitter_PortNbr];
	int BufferSentCount[eSplitter_PortNbr];

public:
	SplitterContext();
	~SplitterContext();
	void init();
	inline bool isFree() const;
	inline bool isUsed() const;
	inline void freeEntry();
	inline int Buffer_Count(const int index) const;
	inline void BufferSent(const int index);
	inline void BufferReceived(const int index);
	inline void setInputBufferHearder(OMX_BUFFERHEADERTYPE* const pIBH);
	inline OMX_BUFFERHEADERTYPE* getInputBufferHearder();
	inline void setOutputAsServed(int const index);
	inline bool isOutputServed(const int index) const;
};

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(Splitter_process);
#endif

#define  REFERENCE_COUNT

class Splitter_process: public openmax_processor
//*************************************************************************************************************
{ //public splitter_coreTemplate,
public:
	Splitter_process();
	virtual ~Splitter_process();

	//Constructor interface from component.type
	virtual int GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue);
	// virtual void functions from Component.h
	virtual void process();
	virtual void processEvent(void);
	virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 portIndex);
	virtual void setConfig(t_sint32 index, void* opaque_ptr);
	virtual void setParam(t_uint32 portIndex, t_uint32 fifoSize, t_uint32 direction, t_uint32 buffSupplierType);
	virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
	virtual void reset();
	virtual void setTunnelStatus(t_sint16 portIndex, t_uint16 isTunneled);

	void process_buffersharing();

	int SetSplitterMode(enumSplitterMode);
protected:
	virtual int UserOpen();


protected:
	enumSplitterMode      m_SplitterMode; // can be Sequential or Standard (default)
	SplitterPortInfo_t    mPortInfo[eSplitter_PortNbr];
	SplitterContext*      mContext;
	int                   mContextCount;
	inline const char*    portName(int portIndex);
#ifdef REFERENCE_COUNT
	char                  m_UserName[32];
	static ReferenceCount m_Count;
#endif
};

#endif //_SPLITTER_PROCESS_H_
