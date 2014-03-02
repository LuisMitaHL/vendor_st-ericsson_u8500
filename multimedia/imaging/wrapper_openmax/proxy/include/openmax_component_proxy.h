/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _OPENMAX_COMPONENT_PROXY_H_
#define _OPENMAX_COMPONENT_PROXY_H_

#include "NmfHost_ProcessingComponent.h"
#include "openmax_processor_wrp.hpp"

class imageproc_proc_wrp;
class openmax_processor;
class OpenMax_Proxy;

typedef openmax_processor * (*fnCreateNmfComponent )();

class OpenMax_Component_ConfigCB : public openmax_processor_api_ToOMXComponentDescriptor
//*************************************************************************************************************
{
public:
	OpenMax_Component_ConfigCB(OpenMax_Proxy *pOMXProxy) : mOMXProxy(pOMXProxy) { }
private:
	virtual void setConfig(t_sint32 index, void * opaque_ptr);
	/// @brief Reference to the ENS component it belongs to
	OpenMax_Proxy *mOMXProxy;
};

class OpenMax_Component;
//ENS_Component
///!TODO OpenMax_Proxy is a very bad name
class OpenMax_Proxy : public NmfHost_ProcessingComponent
//*************************************************************************************************************
{ // generic Proxy for open max nmf interface
public:
	WRAPPER_OPENMAX_API OpenMax_Proxy(const char *name, OpenMax_Component &enscomp, fnCreateNmfComponent _fnCreateNmfComponent=NULL);
	//    /* OBSOLETE */  WRAPPER_OPENMAX_API OpenMax_Proxy(const char *Compname, OpenMax_Component &enscomp, openmax_processor* ProcComp);
	WRAPPER_OPENMAX_API virtual ~OpenMax_Proxy();

	WRAPPER_OPENMAX_API const char *GetProcessorName() const;

	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE instantiate();
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE deInstantiate();
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE start();
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE stop();
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE configure();

	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pStructure);
	//virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nIndex, OMX_PTR pStructure); //Call by processing part with ToOmxComponenent
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferredEvent);
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);

	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo);
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE useBuffer     (OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo);
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE freeBuffer    (OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo);
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer);

	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE sendCommand    (OMX_COMMANDTYPE eCmd, OMX_U32 nData);
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBufferHdr);
	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE fillThisBuffer (OMX_BUFFERHEADERTYPE* pBufferHdr);


	Iopenmax_processor_api_Param   m_IParam;
	Iopenmax_processor_api_Config  m_IConfig;

	WRAPPER_OPENMAX_API virtual OMX_ERRORTYPE setConfigFromProcessing(OMX_INDEXTYPE nIndex, OMX_PTR pStructure);

	OpenMax_Component     *GetOpenMax_Component()    {return((OpenMax_Component *)&mENSComponent);}
	openmax_processor     *GetOpenMax_Processor()    {return((openmax_processor *)m_pProcessor);}
	openmax_processor_wrp *GetOpenMax_ProcessorWrp() {return((openmax_processor_wrp *)m_pNmfProcWrp);}

	WRAPPER_OPENMAX_API virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
	WRAPPER_OPENMAX_API virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);

	WRAPPER_OPENMAX_API virtual const char *GetComponentName() const {return(m_UserName);}

	void SetProcessorPriority(unsigned int priority) { m_nmfPriority = priority; }
protected :
	openmax_processor_wrp * m_pNmfProcWrp;
	fnCreateNmfComponent    m_fnCreateNmfComponent; //Function ptr for the creation of the right component
	const char            * m_Name;
	char                  * m_UserName; //for trace functions
	openmax_processor     * m_pProcessor;
	// OpenMax_Component     * m_pComponent;
	unsigned int            m_nmfPriority;

	OpenMax_Component_ConfigCB* mConfigCB; //< this is the class that will process queued event from nmf side
	unsigned int            m_NbPortAllocated[MAX_PORTS_COUNT]; /// as ens_port doesn't give info about this memorize it

	//< toward the OpenMax_Proxy
};

#endif // _OPENMAX_COMPONENT_PROXY_H_
