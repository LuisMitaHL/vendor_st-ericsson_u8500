/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _SPLITTER_PROXY_H_
#define _SPLITTER_PROXY_H_

#include <IFM_Types.h>
#include <stddef.h>
#include "wrapper_openmax_lib.h"

#include "Component.h"
#include "IFM_Types.h"

OMX_ERRORTYPE imageSplitterFactoryMethod(ENS_Component_p * ppENSComponent);
OMX_ERRORTYPE imageSplitter_Sequential_FactoryMethod(ENS_Component_p * ppENSComponent);

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(SplitterPort);
#endif
class SplitterPort: public PortVideo
//*************************************************************************************************************
{
public:
	SplitterPort(const EnsCommonPortData& commonPortData, ENS_Component& ensComp);
	virtual ~SplitterPort();
	virtual OMX_ERRORTYPE checkCompatibility(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const; // during tunneling init
	virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef);

	// New function for supporting buffer sharing
#ifdef USE_BUFFERSHARING_SEQUENTIAL
	virtual OMX_ERRORTYPE forwardInputBuffer (OMX_BUFFERHEADERTYPE *pBufferHdr);
	virtual OMX_BOOL bufferReturnedFromSharingPort(OMX_BUFFERHEADERTYPE* pBuffer);
	virtual void bufferSentToSharingPort(OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE forwardOutputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr); //Overload for managing dispatch for sequential mode
#endif

	static const char* portName(int portIndex);
	
	// friend class SplitterPort;
	virtual OMX_ERRORTYPE IncreaseAllocateLen(size_t size);

protected:
};

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(Splitter_RDB);
#endif
class Splitter_RDB: public ENS_ResourcesDB
//*************************************************************************************************************
{
public:
	Splitter_RDB();
};

typedef struct
{
	OMX_BUFFERHEADERTYPE *pBufferHdr;
	unsigned int          DispatchIndex;
} _tBufferSharingDispatchTable;

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(Splitter_component);
#endif
class Splitter_component: public OpenMax_Component
//*************************************************************************************************************
{
public:
	Splitter_component(enumSplitterMode mode);
	virtual int Construct(void);
	virtual ~Splitter_component();

	virtual int NMFConfigure(); //Call during the constrution of the processing component

	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const;
	virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const;
	virtual OMX_ERRORTYPE createResourcesDB();
	virtual void *        getConfigAddr(OMX_INDEXTYPE nConfigIndex, size_t *StructSize=NULL, int *pOffset=NULL) const;
	virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;
	
	//int SetSplitterMode(enumSplitterMode);
	enumSplitterMode GetSplitterMode() const { return(m_SplitterMode);}
	virtual int PortFormatChanged(ENS_Port & port );

	_tBufferSharingDispatchTable* FindBufferSharingDispatchEntry(OMX_BUFFERHEADERTYPE *pBufferHdr); //Return the dispatch info for a given buffer header

protected:
	// IFM_CONFIG_SETTING mSplitStruct;
	enumSplitterMode   m_SplitterMode; // can be Sequential or Standard (default)
	OMX_ERRORTYPE destroy(void);
	virtual OMX_ERRORTYPE createPort (OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_BUFFERSUPPLIERTYPE eSupplierPref, OMX_U32 nFifoBufferSize);

	_tBufferSharingDispatchTable m_DispatchTable[SPLITTER_CORE_FIFO_SIZE_MAX];
	unsigned int m_Port_OutputLastShared;
};

#endif // _SPLITTER_PROXY_H_
