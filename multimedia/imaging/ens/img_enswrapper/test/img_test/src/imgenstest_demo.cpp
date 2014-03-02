/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define DECLARE_AUTOVAR
#define DBGT_PREFIX "ImgEnsTest_Demo"
#include "debug_trace.h"

#include "ImgEns_Lib.h"
#include "Img_EnsWrapper.h"

#include <omxil/OMX_Index.h>

class ImgEnsDemo_Component;
class ImgEns_CommonPortData;


class Interface_ProcessorToComponent
//*************************************************************************************************************
{ // Interface for ENS component, Coming from Img_ProcessingComponent
public:
	virtual int SendOmxEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2)=0;
	virtual int EmptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)                    =0;
	virtual int FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)                    =0;
};


class ImgEnsDemo_Port: public ImgEns_Port
//*************************************************************************************************************
{
public:
	ImgEnsDemo_Port(const ImgEns_CommonPortData& commonPortData, ImgEns_Component& ensComp);
	virtual ~ImgEnsDemo_Port();

	virtual OMX_ERRORTYPE checkCompatibility(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const ;
	virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef) ;
	const OMX_PARAM_PORTDEFINITIONTYPE  &getParamPortDefinition(void) const;
/*	*/
private:
};

class ImgEnsDemo_Component : public ImgEns_Component
//*************************************************************************************************************
{
public:
	ImgEnsDemo_Component(OMX_PORTDOMAINTYPE eDomain);
	virtual ~ImgEnsDemo_Component();

	OMX_PORTDOMAINTYPE iDomain;
/*
	OMX_ERRORTYPE createPort (OMX_U32 nPortIndex, OMX_DIRTYPE eDir, OMX_BUFFERSUPPLIERTYPE eSupplierPref, OMX_PORTDOMAINTYPE eDomain, OMX_U32 nFifoBufferSize);

	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const;
	virtual OMX_ERRORTYPE setConfig   (OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
	virtual OMX_ERRORTYPE getConfig   (OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const;//
	//virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType)    const;
*/
public:
//	virtual int InitProcessor(); // Call at the end of the initialisation of asynchronous processor to allow sending uptodate information 
protected:
};

#if 0
class I_ToEnsComponent: public Interface_ProcessorToComponent
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
#endif

class ImgEnsDemo_Proxy: public Img_ProcessingComponent, public Interface_ProcessorToComponent
//*************************************************************************************************************
{
public:
	ImgEnsDemo_Proxy(const char *name, ImgEns_Component &comp /*, Interface_ProcessorToComponent &IProcessorToComponent*/);
	virtual ~ImgEnsDemo_Proxy();

	virtual OMX_ERRORTYPE construct(void)/*  = 0 */;
	virtual OMX_ERRORTYPE destroy(void)/*  = 0 */;

	virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
	virtual OMX_ERRORTYPE retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);


	virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData)/*  = 0 */;

	virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)/*  = 0 */;
	virtual OMX_ERRORTYPE fillThisBuffer (OMX_BUFFERHEADERTYPE* pBuffer)/*  = 0 */;

	virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo);
	virtual OMX_ERRORTYPE useBuffer     (OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo);
	virtual OMX_ERRORTYPE freeBuffer    (OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo);
	virtual OMX_ERRORTYPE useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer)/*  = 0 */;

	virtual OMX_ERRORTYPE getMMHWBufferInfo(OMX_U32 nPortIndex, OMX_U32 nSizeBytes, OMX_U8 *pData, void **bufferAllocInfo, void **portPrivateInfo);
	virtual OMX_ERRORTYPE freeSharedBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo);

	// interface to component
	virtual int SendOmxEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2) { mENSComponent.eventHandler(eEvent, nData1, nData2); return(S_OK); }
	virtual int EmptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)                     { mENSComponent.emptyBufferDone(pBuffer)            ; return(S_OK); }
	virtual int FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)                     { mENSComponent.fillBufferDone(pBuffer)             ; return(S_OK); }

	// New function
	virtual const char *GetComponentName() const {return(m_Name);}
	virtual const char *GetUserName() const {return(m_UserName);}

	void Debug(const char *, ...) const;
protected:
	const char            * m_Name;
	const char            * m_UserName; //for trace functions

	ImgEnsDemo_Port       *m_pPort;
protected:
	Interface_ProcessorToComponent &mI_ProcessorToComponent;
	//specific new framework
};

//I_ToEnsComponent   GlobalToEnsComponent;


IMGENS_TEST_CPT_API int Factory_ImgEns_Test_Demo(OMX_COMPONENTTYPE &Handle)
//*************************************************************************************************************
{ // New factory method that only expose HANDLE, create "OMX.ST.ImgEns_Demo0"
	GET_AND_SET_TRACE_LEVEL(ImgEns_Component);
	printf("create handle for 'OMX.ST.ImgEns_Demo0': \n");

	OMX_ERRORTYPE errorReturn = OMX_ErrorNone;

	ImgEnsDemo_Component * pComponent = new ImgEnsDemo_Component(OMX_PortDomainVideo);
	if (pComponent == 0)
	{
		return OMX_ErrorInsufficientResources;
	}
	// Now create dedicated wrapper
	pComponent->setName("ImgEns_Demo0");
	
	Img_EnsWrapper *pWrapper= new Img_EnsWrapper(pComponent, &Handle);
	pComponent->SetWrapper(pWrapper); // By this way it will not be created by bellagio loader

	ImgEnsDemo_Proxy * pProxy = new ImgEnsDemo_Proxy("ImgEns_Demo0", *pComponent/*, GlobalToEnsComponent*/);
	if (pProxy == 0)
	{
		return OMX_ErrorInsufficientResources;
	}
	pComponent->setProcessingComponent((Img_ProcessingComponent*)pProxy);
	return errorReturn;
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

ImgEnsDemo_Component::ImgEnsDemo_Component(OMX_PORTDOMAINTYPE eDomain)
: ImgEns_Component("ImgEnsDemo")
//*************************************************************************************************************
{
	iDomain=eDomain;
	const OMX_U32 NbPort=1;
	construct(NbPort);
	ImgEns_CommonPortData commonPortData(0, OMX_DirInput, 1, 0, iDomain, OMX_BufferSupplyUnspecified);
	ImgEns_Port* pPort = new ImgEnsDemo_Port(commonPortData, *this);
	addPort(pPort);
}

ImgEnsDemo_Component::~ImgEnsDemo_Component()
//*************************************************************************************************************
{
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
static const int Default_Width                        = 176;
static const int Default_Height                       = 144;
static const OMX_COLOR_FORMATTYPE Default_ColorFormat = OMX_COLOR_FormatCbYCrY;

ImgEnsDemo_Port::ImgEnsDemo_Port(const ImgEns_CommonPortData& commonPortData, ImgEns_Component& ensComp) 
: ImgEns_Port(commonPortData, ensComp)
//*************************************************************************************************************
{
	switch(commonPortData.mPortDomain)
	{
	case OMX_PortDomainAudio:
		mParamPortDefinition.nBufferSize = 0;
		break;
	case OMX_PortDomainVideo:
		mParamPortDefinition.format.video.nFrameWidth        = Default_Width;
		mParamPortDefinition.format.video.nFrameHeight       = Default_Height;
		mParamPortDefinition.format.video.nStride            = 0;
		mParamPortDefinition.format.video.nSliceHeight       = 0;
		mParamPortDefinition.format.video.eColorFormat       = Default_ColorFormat;
		break;
	case OMX_PortDomainImage:
		mParamPortDefinition.format.image.nFrameWidth        = Default_Width;
		mParamPortDefinition.format.image.nFrameHeight       = Default_Height;
		mParamPortDefinition.format.image.nStride            = 0;
		mParamPortDefinition.format.image.nSliceHeight       = 0;
		mParamPortDefinition.format.image.eColorFormat       = Default_ColorFormat;
		mParamPortDefinition.format.image.eCompressionFormat = OMX_IMAGE_CodingUnused;
		break;
	case OMX_PortDomainOther:
		mParamPortDefinition.nBufferSize = 0;
		break;
	default:
		OTB_ASSERT(0);
		break;
	}

}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************



ImgEnsDemo_Port::~ImgEnsDemo_Port()
//*************************************************************************************************************
{
}

OMX_ERRORTYPE ImgEnsDemo_Port::checkCompatibility(OMX_HANDLETYPE /*hTunneledComponent*/, OMX_U32 /*nTunneledPort*/) const
//*************************************************************************************************************
{
	return OMX_ErrorNone;
}

const OMX_PARAM_PORTDEFINITIONTYPE & ImgEnsDemo_Port::getParamPortDefinition(void) const
//*************************************************************************************************************
{
	return mParamPortDefinition;
}

OMX_ERRORTYPE ImgEnsDemo_Port::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &format)
//*************************************************************************************************************
{
	this->mParamPortDefinition = format;
	return OMX_ErrorNone;
}




//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************


ImgEnsDemo_Proxy::ImgEnsDemo_Proxy(const char *name, ImgEns_Component &enscomp/*, IToEnsComponent &enscomp*/)
: Img_ProcessingComponent(enscomp)
//, IToEnsComponent(enscomp)
, mI_ProcessorToComponent(*this)
//*************************************************************************************************************
{
	m_Name     = name;
	m_UserName = name;
	m_pPort    = (ImgEnsDemo_Port*) enscomp.getPort(0);
}

ImgEnsDemo_Proxy::~ImgEnsDemo_Proxy()
//*************************************************************************************************************
{
}


OMX_ERRORTYPE ImgEnsDemo_Proxy::construct()
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("construct\n");
	//Simulate command from 'dsp'
	mI_ProcessorToComponent.SendOmxEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle); //eventHandler <- When return from asynchrone cmd
	return(status);
}


OMX_ERRORTYPE ImgEnsDemo_Proxy::destroy()
//*************************************************************************************************************
{ //Replace deinstantiate
	// to be simulated
	OMX_ERRORTYPE res = OMX_ErrorNone;
	return (res);
}

#if 0
OMX_ERRORTYPE ImgEnsDemo_Proxy::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE res = OMX_ErrorNone;
	return (res);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE res = OMX_ErrorNone;
	return (res);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo)
//*************************************************************************************************************
{
	OMX_ERRORTYPE res = OMX_ErrorNone;
	return (res);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::construct(void)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("construct\n");
	//Simulate command from 'dsp'
	mI_ProcessorToComponent.SendOmxEvent(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle); //eventHandler <- When return from asynchrone cmd
	return(status);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::destroy(void)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("destroy\n");
	return(status);
}
#endif

OMX_ERRORTYPE ImgEnsDemo_Proxy::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorUnsupportedIndex;
	if (nConfigIndex || pComponentConfigStructure){}
	OTB_ASSERT(0);
	Debug("applyConfig\n");
	return(status);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorUnsupportedIndex;
	if (nConfigIndex || pComponentConfigStructure){}
	OTB_ASSERT(0);
	Debug("retrieveConfig\n");
	return(status);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("sendCommand eCmd=%d, nData=%d\n", eCmd, nData);
	if (eCmd==OMX_CommandStateSet)
	{	// Il faut appeler la version dediée asynchrone eventHandler et non eventHandlerCB
		mI_ProcessorToComponent.SendOmxEvent((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, OMX_CommandStateSet, nData);
		mI_ProcessorToComponent.SendOmxEvent(OMX_EventCmdComplete, OMX_CommandStateSet, nData);
	}
	else
	{	// Il faut appeler la version dediée asynchrone
		mI_ProcessorToComponent.SendOmxEvent((OMX_EVENTTYPE)eIMGOMX_EventCmdReceived, eCmd, nData); //First have to acknowledge the cmd
		mI_ProcessorToComponent.SendOmxEvent(OMX_EventCmdComplete, eCmd, nData); // Then said that it is complete
	}
	return(status);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("emptyThisBuffer InputPortIndex=%d\n", pBuffer->nInputPortIndex);
	pBuffer->nFilledLen=0;
	mI_ProcessorToComponent.EmptyBufferDone(pBuffer);
	return(status);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("fillThisBuffer nOutputPortIndex=%d\n", pBuffer->nOutputPortIndex);
	pBuffer->nFilledLen=17;
	mI_ProcessorToComponent.FillBufferDone(pBuffer);
	return(status);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo)
//*************************************************************************************************************
{
	Debug("Allocate buffer PortIndex %lu, nBufferIndex=%lu, nSizeBytes %lu\n", nPortIndex, nBufferIndex, nSizeBytes);
	OMX_ERRORTYPE status=OMX_ErrorNone;
#if (defined(WORKSTATION) || defined(_MSC_VER))
	//Make allocation on the heap
	*ppData = (OMX_U8 *) new char[nSizeBytes];
	if (*ppData  ==NULL)
	{
		OTB_ASSERT(false);
		return OMX_ErrorInsufficientResources;
	}
	*((OMX_U32 **) bufferAllocInfo) = (OMX_U32 *) *ppData ;
	*portPrivateInfo = (void *)NULL;
#else
	OTB_ASSERT(0); //Not implemented today
#endif
	Debug("allocateBuffer PortIndex %lu, nBufferIndex=%lu, nSizeBytes %lu\n", nPortIndex, nBufferIndex, nSizeBytes);
	return(status);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	int index;
	if (dir == OMX_DirInput)
		index= pBuffer->nInputPortIndex;
	else
		index= pBuffer->nOutputPortIndex;

	Debug("useBufferHeader dir=%s, index=%d \n", dir==OMX_DirInput?"Input":"Output", index);
	return(status);
}


OMX_ERRORTYPE ImgEnsDemo_Proxy::useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	if (pBufferHdr || portPrivateInfo){}
	Debug("useBuffer nPortIndex=%d nBufferIndex=%d\n", nPortIndex, nBufferIndex);
	return(status);
}


OMX_ERRORTYPE ImgEnsDemo_Proxy::freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	if (portPrivateInfo) {}
	Debug( "freeBuffer nPortIndex %lu- nBufferIndex %lu bBufferAllocated=%s\n", nPortIndex, nBufferIndex, bBufferAllocated==OMX_TRUE?"true":"false");
#if (defined(WORKSTATION) || defined(_MSC_VER))
	Debug("freeBuffer nPortIndex=%lu nBufferIndex=%lu bBufferAllocated=%s\n", nPortIndex, nBufferIndex, bBufferAllocated==OMX_TRUE?"true":"false");
	if (bBufferAllocated == OMX_TRUE)
	{
		if (bBufferAllocated)
		{
			delete [] (char *)bufferAllocInfo;
		}
	}
#else
	OTB_ASSERT(0);
	Debug("**** freeBuffer nPortIndex=%lu nBufferIndex=%lu bBufferAllocated=%s is not yet implemented for target\n", nPortIndex, nBufferIndex, bBufferAllocated==OMX_TRUE?"true":"false");
#endif
	return(status);
}


OMX_ERRORTYPE ImgEnsDemo_Proxy::getMMHWBufferInfo(OMX_U32 /*nPortIndex*/, OMX_U32 /*nSizeBytes*/, OMX_U8 * /*pData*/, void ** /*bufferAllocInfo*/, void ** /*portPrivateInfo*/)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("getMMHWBufferInfo is not yet implemented\n");
	return(status);
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::freeSharedBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void * /*bufferAllocInfo*/, void * /*portPrivateInfo*/)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("freeSharedBuffer nPortIndex=%d nBufferIndex=%d bBufferAllocated=%d is not yet implemented\n", nPortIndex, nBufferIndex, bBufferAllocated);
	return(status);
}


void ImgEnsDemo_Proxy::Debug(const char *format, ...) const
//*************************************************************************************************************
{
	va_list list;
	va_start(list, format);
	const char *name=m_Name;
	if ( (name==NULL) || (*name=='\0') )
		name="ImgEnsDemo_Proxy";

#ifndef __SYMBIAN32__
	fprintf (stdout, "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %s: ", name);
	vfprintf(stdout, format, list);
#else
	static char DebugString[1024];
	vsprintf(DebugString, format, list);
	RDebug::Printf("\nnew_splitter_Proxy:%s %s", name, DebugString);
#endif
	va_end(list);
}


#if 0

OMX_ERRORTYPE ImgEnsDemo_Proxy::doSpecificEventHandler_cb( OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferredEvent)
//*************************************************************************************************************
{
	switch (event) 
	{
	case OMX_EventCmdComplete:
	default:
		break;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd) 
//*************************************************************************************************************
{
	return OMX_ErrorNone;
}

OMX_PTR ImgEnsDemo_Proxy::retrieveStructureFromProxy(OMX_INDEXTYPE nParamIndex) 
//*************************************************************************************************************
{
	OMX_PTR pParameterStructure = 0;
	switch (nParamIndex) 
	{
	case OMX_IndexParamComponentSuspended:
	default:
		break;
	}
	return pParameterStructure;
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) 
//*************************************************************************************************************
{
	OMX_PTR pParameterStructure = retrieveStructureFromProxy(nParamIndex);
	if (pParameterStructure) 
	{
		Marshall_AsyncCommand(eProxyEvent_SetConfig, (int) nParamIndex, (int) pParameterStructure);
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) 
//*************************************************************************************************************
{
	OMX_PTR pParameterStructure = retrieveStructureFromProxy(nParamIndex);
	if (pParameterStructure) 
	{
		Marshall_AsyncCommand(eProxyEvent_SetConfig, (int) nParamIndex, (int) pParameterStructure);
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::configure() 
{
	OMX_PARAM_PORTDEFINITIONTYPE portDef = m_pPort->getParamPortDefinition();
	OMX_DIRTYPE eDir = OMX_DirMax;
	eDir = m_pPort->getDirection();
	switch (portDef.eDomain) 
	{
	case OMX_PortDomainAudio:
		OTB_ASSERT(0);
		break;
	case OMX_PortDomainVideo:
		m_pProcessor->SetPortConfig(0, m_pPort->getBufferCountActual(), eDir, portDef.nBufferSize, portDef.eDomain, portDef.bEnabled?true: false); 
		m_pProcessor->SetPortFormat(0, portDef.format.video.eColorFormat, portDef.format.video.nFrameWidth, portDef.format.video.nFrameHeight, portDef.format.video.nStride, portDef.format.video.nSliceHeight);
		break;
	case OMX_PortDomainImage:
		m_pProcessor->SetPortConfig(0, m_pPort->getBufferCountActual(), eDir, portDef.nBufferSize, portDef.eDomain, portDef.bEnabled?true: false);
		m_pProcessor->SetPortFormat(0, portDef.format.image.eColorFormat, portDef.format.image.nFrameWidth, portDef.format.image.nFrameHeight, portDef.format.image.nStride, portDef.format.image.nSliceHeight);
		break;
	case OMX_PortDomainOther:
		m_pProcessor->SetPortConfig(0, m_pPort->getBufferCountActual(), eDir, portDef.nBufferSize, portDef.eDomain, portDef.bEnabled?true: false);
		m_pProcessor->SetPortFormat(0, portDef.format.other.eFormat, 0, 0, 0, 0);
		break;
	default:
		m_pProcessor->SetPortConfig(0, m_pPort->getBufferCountActual(), eDir, portDef.nBufferSize, portDef.eDomain, portDef.bEnabled?true: false);
		m_pProcessor->SetPortFormat(0, 0, 0, 0, 0, 0);
		break;
	}
	return OMX_ErrorNone;
}


OMX_ERRORTYPE ImgEnsDemo_Proxy::useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo) 
{
	OMX_ERRORTYPE err = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE portDef = m_pPort->getParamPortDefinition();
	OMX_STATETYPE currentState = OMX_StateInvalid;
	ImgEns_Component* myImgEnsTest = (ImgEns_Component*) &mENSComponent;
	OMX_DIRTYPE eDir = OMX_DirMax;
	eDir = m_pPort->getDirection();
	switch (eDir) 
	{
	case OMX_DirInput:
		break;
	case OMX_DirOutput:
		DBGT_PTRACE("ImgEnsTest warning getDirection problem : OMX_DirOutput\n");
		break;
	default:
		DBGT_PTRACE("ImgEnsTest warning getDirection problem : %d\n", eDir);
		break;
	}

	myImgEnsTest->GetState(&currentState);
	if (currentState == OMX_StateLoaded) 
	{
	} 
	else 
	{
		OTB_ASSERT(m_pProcessor);
		m_pProcessor->SetPortConfig(nPortIndex, m_pPort->getBufferCountActual(), eDir, portDef.nBufferSize, portDef.eDomain, portDef.bEnabled?true: false);
		m_pProcessor->SetPortFormat(nPortIndex, portDef.format.video.eColorFormat, portDef.format.video.nFrameWidth, portDef.format.video.nFrameHeight, portDef.format.video.nStride, portDef.format.video.nSliceHeight);
	}
	err = Async_Proxy::useBuffer(nPortIndex, nBufferIndex, pBufferHdr, 	portPrivateInfo);
	DBGT_EPILOG(); // End of function (level2)
	return err;
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer) 
{
	DBGT_PROLOG(); // Start of function (level 2)
	OMX_ERRORTYPE err = OMX_ErrorNone;
	DBGT_PTRACE(
			"buff ImgEnsDemo_Proxy::useBufferHeader calls useBufferHeader, pBuffer = 0x%x, this = 0x%x\n",
			(int) pBuffer, (int) this);

	OMX_U32 nPortIndex = 0;
	OMX_PARAM_PORTDEFINITIONTYPE portDef = m_pPort->getParamPortDefinition();
	OMX_STATETYPE currentState = OMX_StateInvalid;
	ImgEns_Component* myImgEnsTest = (ImgEns_Component*) &mENSComponent;
	OMX_DIRTYPE eDir = OMX_DirMax;
	eDir = m_pPort->getDirection();
	switch (eDir) 
	{
	case OMX_DirInput:
		break;
	case OMX_DirOutput:
		DBGT_PTRACE("ImgEnsTest warning getDirection problem : OMX_DirOutput\n");
		break;
	default:
		DBGT_PTRACE("ImgEnsTest warning getDirection problem : %d\n", eDir);
		break;
	}
	myImgEnsTest->GetState(&currentState);
	//ImgEnsTest__print_state(currentState);
	if (currentState == OMX_StateLoaded) 
	{
		//DBGT_PTRACE("\t ImgEnsDemo_Proxy::useBufferHeader not good state to foward to processing component\n");
	} 
	else 
	{
		m_pProcessor->SetPortConfig(nPortIndex, m_pPort->getBufferCountActual(), eDir/*, m_pPort->getBufferSupplier()*/, portDef.nBufferSize, portDef.eDomain, portDef.bEnabled? true: false);
		m_pProcessor->SetPortFormat(nPortIndex,
				portDef.format.video.eColorFormat,
				portDef.format.video.nFrameWidth,
				portDef.format.video.nFrameHeight, portDef.format.video.nStride,
				portDef.format.video.nSliceHeight);
	}

	// OTB_ASSERT(0); // to do err = useBufferHeaderHeap(dir, pBuffer);
	err = Async_Proxy::useBufferHeader(dir, pBuffer);
	DBGT_EPILOG(); // End of function (level2)
	return err;
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo) 
//*************************************************************************************************************
{
	OMX_ERRORTYPE err = OMX_ErrorNone;
	err = Async_Proxy::allocateBuffer(nPortIndex, nBufferIndex, nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo);
	return err;
}

OMX_ERRORTYPE ImgEnsDemo_Proxy::freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo) 
//*************************************************************************************************************
{
	OMX_ERRORTYPE err = OMX_ErrorNone;
	err = Async_Proxy::freeBuffer(nPortIndex, nBufferIndex, bBufferAllocated, bufferAllocInfo, portPrivateInfo);
	return err;
}
int ImgEnsDemo_Proxy::Execute_CBFromProcessor(AsyncMsg& Msg)
//*************************************************************************************************************
{
	int res = Async_Proxy::Execute_CBFromProcessor(Msg); // Call base class
	return (res);
}

void ImgEnsDemo_Proxy::ExecuteIncomingMsg(void *pMsg)
//*************************************************************************************************************
{
	Async_Proxy::ExecuteIncomingMsg(pMsg); // Call base class
}
#endif
