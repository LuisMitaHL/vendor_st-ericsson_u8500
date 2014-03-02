/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#define OMXCOMPONENT "Splitter_factory"
#define OMX_TRACE_UID 0x30
#include "osi_trace.h"

#include "wrapper_openmax_lib.h"
#include "splitter_process.h"
#include "splitter_proxy.h"
#include "splitter_common.h"
#include "OMX_Index.h"
#include "ENS_Component_Fsm.h"


#include <IFM_Index.h>
#include <IFM_Types.h>
#include <string.h>

#include "extradata.h" // used for both extradata & extensions

#undef OMXCOMPONENT
#define OMXCOMPONENT "SPLITTER_PROXY"

#define SPLITTER_INPUT_FIFO_SIZE   1
#define SPLITTER_OUTPUT_FIFO_SIZE  1

openmax_processor* Create_splitter_NMF()
{ // wrp Factory for splitter
	return(new Splitter_process);
}

OMX_ERRORTYPE imageSplitterFactory(ENS_Component_p * ppENSComponent, enumSplitterMode aMode=eSplitter_Broadcast)
//*************************************************************************************************************
{
	IN0("");
	int           omxReturnCode = OMX_ErrorNone;
	Splitter_component*     pSplitterInst = NULL;

	// Allocate the Splitter_component
	pSplitterInst = new Splitter_component(aMode);
	if (pSplitterInst == NULL)
	{
		MSG0("Failed to allocate Splitter_component instance\n");
		OUTR(" ", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}

	// Construct the Splitter_component
	omxReturnCode = pSplitterInst->Construct();
	if (omxReturnCode != eNoError)
	{
		MSG1("Splitter_component contruct failed, omxReturnCode=%d\n", omxReturnCode);
		OUTR(" ", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}

	OpenMax_Proxy * pProxy = new OpenMax_Proxy(pSplitterInst->GetComponentName(), *pSplitterInst, Create_splitter_NMF);
	if (pProxy == NULL)
	{
		delete pSplitterInst;
		OUTR(" ", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}
	pSplitterInst->setProcessingComponent(pProxy);
	*ppENSComponent = pSplitterInst;

	OUTR(" ", OMX_ErrorNone);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE imageSplitterFactoryMethod(ENS_Component_p * ppENSComponent)
//*************************************************************************************************************
{ //Standard, Not sequential one (default)
	return(imageSplitterFactory(ppENSComponent, eSplitter_Broadcast));
}

OMX_ERRORTYPE imageSplitter_Sequential_FactoryMethod(ENS_Component_p * ppENSComponent)
//*************************************************************************************************************
{
	return(imageSplitterFactory(ppENSComponent, eSplitter_Sequential));
}


//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

#undef  OMXCOMPONENT
#define OMXCOMPONENT GetComponentName()

#ifdef __EPOC32__
// Prevent export of symbols
NONSHARABLE_CLASS(Splitter_component);
#endif


Splitter_component::Splitter_component(enumSplitterMode mode)
	: OpenMax_Component("Splitter_component")
	, m_SplitterMode(mode)
//*************************************************************************************************************
{
	IN0("");
	if (mode == eSplitter_Sequential)
	{
		m_ComponentName="Splitter_Sequential";
#ifdef USE_BUFFERSHARING_SEQUENTIAL
		m_bUseBufferSharing=true;
		MSG0("Splitter mode: Sequential with buffer sharing\n");
		m_Port_OutputLastShared=2;
#else
		m_bUseBufferSharing=false;
		MSG0("Splitter mode: Sequential\n");
#endif
	}
	else if (mode == eSplitter_Broadcast)
	{
		m_ComponentName="Splitter_Broadcast";
#ifdef USE_BUFFERSHARING_BROADCAST
		m_bUseBufferSharing=true;
		MSG0("Splitter mode: Broadcast with buffer sharing\n");
#else
		m_bUseBufferSharing=false;
		MSG0("Splitter mode: Broadcast\n");
#endif
	}
	else
	{ //Sequential mode
		m_bUseBufferSharing=false;
		WOM_ASSERT(0);
	}
	_tBufferSharingDispatchTable* pEntry=&m_DispatchTable[0];

	// Init by default dispatch table
	for (unsigned int i=0; i< SPLITTER_CORE_FIFO_SIZE_MAX; ++i)
	{
		pEntry->pBufferHdr=NULL;
		pEntry->DispatchIndex=0;
		++pEntry;
	}
	OUT0(" ");
}

Splitter_component::~Splitter_component()
//*************************************************************************************************************
{
	IN0("");
	destroy();
	OUT0(" ");
}


OMX_ERRORTYPE Splitter_component::getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const
//*************************************************************************************************************
{
#if 0
	if (0 == strcmp(cParameterName, "OMX.SYMBIAN.INDEX.CONFIG.SHAREDCHUNKMETADATA"))
	{
		*pIndexType = (OMX_INDEXTYPE)OMX_SymbianIndexConfigSharedChunkMetadata;
		MSG1("Splitter_component::getExtensionIndex=%d\n", *pIndexType);
		return OMX_ErrorNone;
	}
#endif
	return (CExtension::getExtensionIndex(cParameterName, pIndexType));
}

int Splitter_component::Construct(void)
//*************************************************************************************************************
{
	IN0("");
	OMX_ERRORTYPE omxReturnCode = OMX_ErrorNone;

	// Contrust the Splitter_component/ENS component
	MSG1("Instantiating ENS component with %d ports\n", eSplitter_PortNbr);
	omxReturnCode = ENS_Component::construct(eSplitter_PortNbr);
	if( omxReturnCode != OMX_ErrorNone )
	{
		OUTR(" ", omxReturnCode);
		return omxReturnCode;
	}

	if ((m_bUseBufferSharing==true) /*&& (GetSplitterMode()==eSplitter_Broadcast)*/)
	{	/* code for buffer sharing */
		MSG0("\n*****************************\n Initialise Ports and buffer sharing\n");

		// Create the input port
		MSG2("Creating Port[%d](%s)\n", eSplitter_PortIndex_Input, SplitterPort::SplitterPort::portName(eSplitter_PortIndex_Input));
		omxReturnCode = createPort( eSplitter_PortIndex_Input, OMX_DirInput, OMX_BufferSupplyOutput, SPLITTER_INPUT_FIFO_SIZE);
		if( omxReturnCode != OMX_ErrorNone )
		{
			OUTR(" ", omxReturnCode);
			goto failed;
		}

		// Create Output Ports
		for( int portIndex=eSplitter_PortIndex_Output1; portIndex <=eSplitter_PortIndex_Output_Last; portIndex++)
		{
			MSG2("Creating Port[%d](%s)\n", portIndex, SplitterPort::portName(portIndex));
			omxReturnCode = createPort( portIndex, OMX_DirOutput, OMX_BufferSupplyOutput, SPLITTER_OUTPUT_FIFO_SIZE);
			if( omxReturnCode != OMX_ErrorNone )
			{
				OUTR(" ", omxReturnCode);
				goto failed;
			}
		}

		OMX_ERRORTYPE error=OMX_ErrorNone;
		for( int portIndex=eSplitter_PortIndex_Output1; portIndex <=eSplitter_Port_OutputLastShared; portIndex++)
		{
			MSG1("Connecting buffer sharing Port[0] -> Port[%d]\n", portIndex);
			error = connectBufferSharingPorts(0, portIndex);
			if(error!=OMX_ErrorNone) 
			{
				ReportError(eError_CannotCreateOutputPort, "Splitter Cannot make buffer sharing between ports 1 and %d", portIndex);
				omxReturnCode=error;
			}
		}
		MSG0("\n Ports connected\n*****************************\n");
	}
	else
	{ // NO buffer sharing
		// Create the input port
		MSG2("Creating Port[%d](%s)\n", eSplitter_PortIndex_Input, SplitterPort::SplitterPort::portName(eSplitter_PortIndex_Input));
		omxReturnCode = createPort( eSplitter_PortIndex_Input, OMX_DirInput, OMX_BufferSupplyUnspecified, SPLITTER_INPUT_FIFO_SIZE);
		if( omxReturnCode != OMX_ErrorNone )
		{
			OUTR(" ", omxReturnCode);
			goto failed;
		}

		// Create Output Ports
		for( int portIndex=eSplitter_PortIndex_Output1; portIndex <=eSplitter_PortIndex_Output_Last; portIndex++)
		{
			MSG2("Creating Port[%d](%s)\n", portIndex, SplitterPort::portName(portIndex));
			omxReturnCode = createPort( portIndex, OMX_DirOutput, OMX_BufferSupplyOutput, SPLITTER_OUTPUT_FIFO_SIZE);
			if( omxReturnCode != OMX_ErrorNone )
			{
				OUTR(" ", omxReturnCode);
				goto failed;
			}
		}

	}

	OUTR(" ", omxReturnCode);
	return omxReturnCode;

	// Cleanup in case of failure
failed:
	OUTR(" ", omxReturnCode);
	return omxReturnCode;
}

OMX_ERRORTYPE Splitter_component::destroy(void)
//*************************************************************************************************************
{
	IN0("");
	// Note: Splitter_component ports are automatically deleted by the ENS
	OUTR(" ", OMX_ErrorNone);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Splitter_component::createPort(
	OMX_U32                nPortIndex,
	OMX_DIRTYPE            eDir,
	OMX_BUFFERSUPPLIERTYPE eSupplierPref,
	OMX_U32                nFifoBufferSize)
//*************************************************************************************************************
{
	IN0("");
	if (nPortIndex > getPortCount())
	{
		OUTR(" ", OMX_ErrorBadPortIndex);
		return OMX_ErrorBadPortIndex;
	}

	// Init port data
	EnsCommonPortData commonPortData( nPortIndex, eDir, nFifoBufferSize, 0, OMX_PortDomainVideo, eSupplierPref);

	// Allocate a new port
	MSG3("Instantiating Port[%d]: direction=%s, buff supplier pref=%s\n",
		(int)nPortIndex, eDir==OMX_DirInput?"input":"output",
		eSupplierPref==OMX_BufferSupplyInput?"input":
		eSupplierPref==OMX_BufferSupplyOutput?"output":
		eSupplierPref==OMX_BufferSupplyUnspecified?"unspecified":"wrong value");
	ENS_Port* newPort = new SplitterPort( commonPortData, *this);
	if( newPort == NULL )
	{
		OUTR(" ", OMX_ErrorInsufficientResources);
		return OMX_ErrorInsufficientResources;
	}

	// Add port to ENS
	MSG2("Adding Port[%d](%s) to ENS\n", (int)nPortIndex, SplitterPort::portName(nPortIndex));
	addPort(newPort);

	OUTR(" ", OMX_ErrorNone);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Splitter_component::getParameter( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameters) const
//*************************************************************************************************************
{
	IN0("");
	OMX_ERRORTYPE err = OMX_ErrorNone;

	if (pComponentParameters == NULL)
	{
		MSG0("Error: pComponentParameters == NULL\n");
		OUTR(" ", OMX_ErrorBadParameter);
		return OMX_ErrorBadParameter;
	}

	switch (nParamIndex)
	{
	default:
		err = ENS_Component::getParameter(nParamIndex, pComponentParameters);
		break;
	}
	OUTR(" ", err);
	return err;
}

OMX_ERRORTYPE Splitter_component::setParameter( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameters)
//*************************************************************************************************************
{
	IN0("");
	OMX_ERRORTYPE err = OMX_ErrorNone;

	if (pComponentParameters == NULL)
	{
		MSG0("Error: pComponentParameters == NULL\n");
		OUTR(" ", OMX_ErrorBadParameter);
		return OMX_ErrorBadParameter;
	}

	switch (nParamIndex)
	{
	default:
		err = ENS_Component::setParameter(nParamIndex, pComponentParameters);
		break;
	}
	OUTR(" ", err);
	return err;
}

OMX_ERRORTYPE Splitter_component::getConfig( OMX_INDEXTYPE nConfigIndex, OMX_PTR pConfigStructure) const
//*************************************************************************************************************
{
	IN0("");
	OMX_ERRORTYPE err = OMX_ErrorNone;

	if (pConfigStructure == NULL)
	{
		MSG0("Error: pComponentParameters == NULL\n");
		OUTR(" ", OMX_ErrorBadParameter);
		return OMX_ErrorBadParameter;
	}

	switch (nConfigIndex)
	{
#if 0
	case OMX_SymbianIndexConfigSharedChunkMetadata:
		{
			OMX_SYMBIAN_CONFIG_SHARED_CHUNK_METADATA *pSharedChunkConfig=(OMX_SYMBIAN_CONFIG_SHARED_CHUNK_METADATA *)pConfigStructure;
			ENS_Port * pPort=getPort(nPortIndex);
			if (pPort==NULL)
				return(OMX_ErrorBadPortIndex);
			//MMHwBuffer *pChunk=pPort->getSharedChunk();
			pSharedChunkConfig->nHandleId      = 0/*pChunk->BufferInfo();*/ /** This is a handle id of the shared chunk */
			pSharedChunkConfig->nOwnerThreadId = 0; /** This is thread that contains the handle */
		}
		break;
#endif
	default:
		err = ENS_Component::getConfig(nConfigIndex, pConfigStructure);
		break;
	}
	OUTR(" ", err);
	return err;
}

OMX_ERRORTYPE Splitter_component::setConfig( OMX_INDEXTYPE nConfigIndex, OMX_PTR pConfigStruct)
//*************************************************************************************************************
{
	IN0("");
	OMX_ERRORTYPE err = OMX_ErrorNone;
	//int val;
	if (pConfigStruct == NULL)
	{
		MSG0("Error: pConfigStruct == NULL\n");
		OUTR(" ", OMX_ErrorBadParameter);
		return OMX_ErrorBadParameter;
	}

	switch (nConfigIndex)
	{
	default:
		err = ENS_Component::setConfig(nConfigIndex, pConfigStruct);
		break;
	}

	OUTR(" ", err);
	return err;
}

OMX_PTR Splitter_component::getConfigAddr(OMX_INDEXTYPE nIndex, size_t *pStructSize, int *pOffset) const
//*************************************************************************************************************
{
	IN0("");
	size_t size=0;
	OMX_PTR pStructure=NULL;
	switch ((OMX_U32)nIndex) 
	{
	case 0:
		break;
	default : // call base class
		return( OpenMax_Component::getConfigAddr(nIndex, pStructSize, pOffset));
	}

	if (pStructSize)
	{ //return the size of the struct
		*pStructSize=size;
	}
	return pStructure;
}


/**
@brief Give a chance to add proper initialisation when the NMF component is start
Call OpenMax_Proxy::applyConfig for all parameters that have beeen configured before idle state when the processor haven't been started
*/
int Splitter_component::NMFConfigure()
//*************************************************************************************************************
{	// Configuration before real start
	IN0("");
	int status=S_OK;
	//ProcessingComponent &Comp=getProcessingComponent();

	IFM_CONFIG_SETTING ConfigStruct;
	ConfigStruct.nNewValue=GetSplitterMode();
	OUTR(" ", (status));
	return(status);
}

#if 0
int Splitter_component::SetSplitterMode(enumSplitterMode mode)
//*************************************************************************************************************
{
	IN0("");
	int err=S_OK;
	if (mode == eSplitter_Sequential)
	{
		m_SplitterMode= eSplitter_Sequential;
		MSG0("Splitter_component mode Sequential\n");
	}
	else if (mode == eSplitter_Broadcast)
	{
		m_SplitterMode= eSplitter_Broadcast;
		MSG0("Splitter_component mode Standard\n");
	}
	else
	{
		ReportError(-1, "Wrong splitter mode value %d (mode Sequential=0 or mode Standard=1 accepted)", mode);
		err=-1;
	}

	OUTR(" ", err);
	return(err);
}
#endif

OMX_ERRORTYPE Splitter_component::createResourcesDB()
//*************************************************************************************************************
{
	IN0("");
	mRMP = new Splitter_RDB;
	if (mRMP == NULL)
	{
		OUTR(" ", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}
	OUTR(" ", OMX_ErrorNone);
	return OMX_ErrorNone;
}

/// call when a set param has been done on a port
int Splitter_component::PortFormatChanged(ENS_Port & port )
//*************************************************************************************************************
{ //By default done nothing
	IN0("");
	int res=S_OK;
	SplitterPort &Port=(SplitterPort &)port;
	// size_t BufferSize=Port.getParamPortDefinition().nBufferSize;
	size_t MaxSize=Port.getParamPortDefinition().nBufferSize;
	//if (Port.getPortIndex()==0)
	{ //Check with all output port
		//BufferSize=MaxSize;
		for (unsigned int i=0; i<eSplitter_PortNbr ;++i)
		{
			SplitterPort  *pPortOut=(SplitterPort  *)getPort(i);
			if (MaxSize > pPortOut->getParamPortDefinition().nBufferSize)
			{
				MSG3("Increase port[%d] size from %u to %u\n", i, (unsigned int)pPortOut->getParamPortDefinition().nBufferSize, (unsigned int)MaxSize);
				pPortOut->getParamPortDefinition().nBufferSize=MaxSize;
			}
			else if (MaxSize < pPortOut->getParamPortDefinition().nBufferSize)
			{
				MaxSize = pPortOut->getParamPortDefinition().nBufferSize;
				i=0; //Restart at beggining
				Port.getParamPortDefinition().nBufferSize=MaxSize; // change input port size
				MSG3("Increase input port[%d] size from %u to %u\n", i, (unsigned int)Port.getParamPortDefinition().nBufferSize, (unsigned int)MaxSize);
			}
		}
	}
	//else
	//{ //Check output port
	//	SplitterPort  *pPortIn=(SplitterPort  *)getPort(0);
	//	if (BufferSize < pPortIn->getParamPortDefinition().nBufferSize)
	//	{
	//		MSG2("Increase Output port size from %u to %u",(unsigned int) BufferSize, (unsigned int)pPortIn->getParamPortDefinition().nBufferSize);
	//		Port.getParamPortDefinition().nBufferSize= pPortIn->getParamPortDefinition().nBufferSize;
	//	}
	//}

	OUTR(" ", res);
	return(res);
}

_tBufferSharingDispatchTable* Splitter_component::FindBufferSharingDispatchEntry(OMX_BUFFERHEADERTYPE *pBufferHdr)
//*************************************************************************************************************
{ //Return the dispatch info for a given buffer header
	IN0("");
	_tBufferSharingDispatchTable* pOut=NULL;
	_tBufferSharingDispatchTable* pEntry=&m_DispatchTable[0];
	for (unsigned int i=0; i< SPLITTER_CORE_FIFO_SIZE_MAX; ++i)
	{
		if (pBufferHdr ==pEntry->pBufferHdr)
		{
			pOut=pEntry;
			break;
		}
		++pEntry;
	}
	OUTR(" ", pOut);
	return(pOut);
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef  OMXCOMPONENT
#define OMXCOMPONENT "SplitterPort"


#ifdef __EPOC32__
// Prevent export of symbols
NONSHARABLE_CLASS(SplitterPort);
#endif
SplitterPort::SplitterPort( const EnsCommonPortData& commonPortData, ENS_Component& ensComp)
: PortVideo(commonPortData, ensComp)
//*************************************************************************************************************
{
	IN0("");
	OUT0(" ");
}

SplitterPort::~SplitterPort()
//*************************************************************************************************************
{
	IN0("");
	MSG1("Port[%ld] destructing\n", getPortIndex());
	OUT0(" ");
}


static const char* portNames[] =
{ 
	"input",
	"output1",
	"output2",
	"output3",
	"output4"
};

const char* SplitterPort::portName(int portIndex)
//*************************************************************************************************************
{
	if( 0<=portIndex && portIndex<eSplitter_PortNbr ) {
		return portNames[portIndex];
	}
	else
	{
		return "invalid port index";
	}
}

OMX_ERRORTYPE SplitterPort::checkCompatibility( OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const
//*************************************************************************************************************
{ // Called by tunelled component
	IN0("");
	OMX_ERRORTYPE error=OMX_ErrorNone;

	// Get tunneled port size
	OMX_PARAM_PORTDEFINITIONTYPE   PortConfig;
	getOmxIlSpecVersion(&PortConfig.nVersion);
	PortConfig.nSize      = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	PortConfig.nPortIndex = nTunneledPort;
	error=OMX_GetParameter(hTunneledComponent, OMX_IndexParamPortDefinition, &PortConfig);
	if (error!=OMX_ErrorNone)
	{
		OUTR(" ", error);
		return(error);
	}
	if (PortConfig.nBufferSize > mParamPortDefinition.nBufferSize)
	{ // Tunneled port has bigger size, must propagate it
		MSG3("Splitter::checkCompatibility port[%d] Must increase port size from %u to %u because of tunnelling\n", (unsigned int)mParamPortDefinition.nPortIndex, (unsigned int)mParamPortDefinition.nBufferSize, (unsigned int)PortConfig.nBufferSize);
		SplitterPort *pPort=(SplitterPort *)m_Omx_Component.getPort(mParamPortDefinition.nPortIndex);
		if (pPort !=NULL)
		{// checkCompatibility is const must workaround ....
			error=pPort->IncreaseAllocateLen(PortConfig.nBufferSize);
		}
	}

	OUTR(" ", error);
	return error;
}

OMX_ERRORTYPE SplitterPort::IncreaseAllocateLen(size_t new_size)
//*************************************************************************************************************
{ // for forcing the increasing of port alloc size
	IN0("");
	OMX_ERRORTYPE error=OMX_ErrorNone;
	if (new_size <= mParamPortDefinition.nBufferSize)
	{
		OUTR(" ", OMX_ErrorNone);
		return(OMX_ErrorNone);
	}
	mParamPortDefinition.nBufferSize=new_size; //update the size
	if( m_Omx_Component.PortFormatChanged(*this)!=S_OK)
		error=OMX_ErrorUnsupportedSetting;

	OUTR(" ", error);
	return(error);
}


OMX_ERRORTYPE SplitterPort::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
//*************************************************************************************************************
{ // Call by the omx_SetParameter OMX_IndexParamPortDefinition
	IN0("");
	// TODO: switch according to eDomain
	OMX_VIDEO_PORTDEFINITIONTYPE &video=mParamPortDefinition.format.video;
	video.nFrameWidth           = portdef.format.video.nFrameWidth;
	video.nFrameHeight          = portdef.format.video.nFrameHeight;
	video.pNativeRender         = portdef.format.video.pNativeRender; // not used
	video.nStride               = portdef.format.video.nStride;
	//video.nSliceHeight          = portdef.format.video.nSliceHeight; //RO field
	video.bFlagErrorConcealment = portdef.format.video.bFlagErrorConcealment;
	video.eCompressionFormat    = portdef.format.video.eCompressionFormat;
	video.eColorFormat          = portdef.format.video.eColorFormat;
	video.pNativeWindow         = portdef.format.video.pNativeWindow;
	WOM_ASSERT(video.nStride >= (OMX_S32) video.nFrameWidth);

	size_t bufferSize;
	OMX_ERRORTYPE error=OMX_ErrorNone;
	int res =ComputeImageSize(video.eColorFormat, video.nStride, video.nFrameHeight, bufferSize);
	if (res != S_OK)
	{
		ReportError( -1, "Wrong format for computing image size", video.eColorFormat);
		error=OMX_ErrorUnsupportedSetting;
		bufferSize=0;
	}
	if (mParamPortDefinition.eDir==OMX_DirOutput)
	{ //For the moment add extradat more space ....
		bufferSize += EXTRADATA_STILLPACKSIZE;
	}
	if (bufferSize == 0)
	{
		MSG0("Error: computed buffer size == 0\n");
	}
	if (mParamPortDefinition.nBufferSize < bufferSize)
		mParamPortDefinition.nBufferSize = bufferSize; //Update nBufferSize only for bigger size

	MSG3("Port[%d](%s) Buffer size = %d\n", (int)getPortIndex(), SplitterPort::portName(getPortIndex()), (int)bufferSize);
	//Inform the component that port settings change
	if( m_Omx_Component.PortFormatChanged(*this)!=S_OK)
		error=OMX_ErrorUnsupportedSetting;
	OUTR(" ", error);
	return error;
}

#ifdef USE_BUFFERSHARING_SEQUENTIAL
void SplitterPort::bufferSentToSharingPort(OMX_BUFFERHEADERTYPE* pBufferHdr)
//*************************************************************************************************************
{
	IN0("");
	MSG2("Splitter_port[%d] bufferSentToSharingPort for bufferHdr %p\n", (int)getPortIndex(), pBufferHdr);
	ENS_Port::bufferSentToSharingPort(pBufferHdr);
	OUT0(" ");
}

//The code is guarded because requires new version of the ens

OMX_BOOL SplitterPort::bufferReturnedFromSharingPort(OMX_BUFFERHEADERTYPE* pBufferHdr)
//*************************************************************************************************************
{ //Buffer has been threaded and returned by the sharing port 
	// In sequential mode we deliver the buffer to next sharing port
	IN0("");
	OMX_BOOL res=OMX_TRUE;
	//OMX_ERRORTYPE error=OMX_ErrorNone;
	if ((isBufferSharingEnabled()==OMX_TRUE) && (getDirection()==OMX_DirInput) && (((Splitter_component &)m_Omx_Component).GetSplitterMode()==eSplitter_Sequential) )
	{ // In sequential mode need to dispatch on next output port
		OMX_ERRORTYPE error=OMX_ErrorNone;
		ENS_Port* pPort;
		ENS_Port_Fsm * portFsm;
		ENS_CmdEmptyThisSharedBufferEvt emptyThisSharedBufferEvt(pBufferHdr);
		const ENS_INTER_CONNECTED_PORT_INFO &List= GetBufSharingPortList();

		MSG2("Splitter_port[%d] bufferReturnedFromSharingPort for buffer %p\n", (int)getPortIndex(), pBufferHdr);

		_tBufferSharingDispatchTable* pTable=((Splitter_component &)m_Omx_Component).FindBufferSharingDispatchEntry(pBufferHdr); //Find an empty table
		if (pTable==NULL)
		{
			WOM_ASSERT(pTable);
			res=OMX_FALSE;
			OUTR(" ", res);
			return res;
		}
		++pTable->DispatchIndex; // Goto next output port
		
		if (pTable->DispatchIndex >= List.numPorts)
		{// All port have been served just call default
			MSG2("Splitter_port[%d] All output ports served for bufferHdr %p, return output\n", (int)getPortIndex(), pBufferHdr);
			pTable->DispatchIndex= 0;
			pTable->pBufferHdr   = NULL; //Reset info about dispatch 
			res=ENS_Port::bufferReturnedFromSharingPort(pBufferHdr); //Call base class
			OUTR(" ", res);
			return(res);
		}

		// modify buffer for having visible effect on image
		// memset(pBufferHdr->pBuffer, 0, 10000);

		// end to be removed
		pPort = *(List.mBufSharingPorts+pTable->DispatchIndex);
		portFsm = pPort->getPortFsm();
		MSG3("Splitter_port[%d] Call emptyThisSharedBufferEvt bufferHdr %p to next output Port[%d]\n", (int)getPortIndex(), pBufferHdr, (int)pPort->getPortIndex() );
		MSG3("splitter_port[%d] sequential bufferHdr %p to Output[%d]\n", (int)getPortIndex(), pBufferHdr, (int)pPort->getPortIndex() );

		// Dispatch to next sharing port
		error = portFsm->dispatch(&emptyThisSharedBufferEvt);
		if(error!=OMX_ErrorNone)
		{
			WOM_ASSERT(error!=OMX_ErrorNone);
			OUTR(" ", OMX_FALSE);
			return OMX_FALSE;
		}
		res=ENS_Port::bufferReturnedFromSharingPort(pBufferHdr); //Call base class
		OUTR(" ", res);
		return res;
	}

	//Do default action
	MSG2("Splitter[%d] Call ENS_Port::bufferReturnedFromSharingPort  bufferHdr %p\n", (int)getPortIndex(), pBufferHdr);
	res=ENS_Port::bufferReturnedFromSharingPort(pBufferHdr);
	OUTR(" ", res);
	return(res);
}

OMX_ERRORTYPE SplitterPort::forwardInputBuffer(OMX_BUFFERHEADERTYPE *pBufferHdr)
//*************************************************************************************************************
{ //When an input buffer is dispatch to its output (buffer sharing)
	IN0("");
	OMX_ERRORTYPE res=OMX_ErrorNone;
	if ((isBufferSharingEnabled()==OMX_TRUE) && (((Splitter_component &)m_Omx_Component).GetSplitterMode()==eSplitter_Sequential) )
	{ // In sequential mode only dispatch to first output port
		OMX_ERRORTYPE error=OMX_ErrorNone;
		ENS_Port* pPort;
		ENS_Port_Fsm * portFsm;
		ENS_CmdEmptyThisSharedBufferEvt emptyThisSharedBufferEvt(pBufferHdr);
		const ENS_INTER_CONNECTED_PORT_INFO &List= GetBufSharingPortList();

		pPort = *(List.mBufSharingPorts+0); //Take the first port in the list
		portFsm = pPort->getPortFsm();
		MSG3("splitter_port[%d] forwardInputBuffer bufferHdr %p to Output[%d]\n", (int)getPortIndex(), pBufferHdr, (int)pPort->getPortIndex() );
		MSG3("splitter_port[%d] sequential bufferHdr %p to Output[%d]\n", (int)getPortIndex(), pBufferHdr, (int)pPort->getPortIndex() );
		_tBufferSharingDispatchTable* pTable=((Splitter_component &)m_Omx_Component).FindBufferSharingDispatchEntry(NULL); //Find an empty table
		if (pTable==NULL)
		{
			WOM_ASSERT(pTable);
			OUTR(" ", OMX_ErrorUnsupportedSetting);
			return OMX_ErrorUnsupportedSetting;
		}
		// Save info for current dispatch
		pTable->pBufferHdr   =pBufferHdr;
		pTable->DispatchIndex=0;

		// Dispatch to first sharing port
		error = portFsm->dispatch(&emptyThisSharedBufferEvt);
		if(error!=OMX_ErrorNone)
		{
			WOM_ASSERT(error!=OMX_ErrorNone);
			OUTR(" ", error);
			return error;
		}
		OUTR(" ", OMX_ErrorNone);
		return OMX_ErrorNone;
	}
	else
	{
		MSG2("Splitter_port[%d] call default forwardInputBuffer for bufferHdr %p\n", (int)getPortIndex(), pBufferHdr);
		res=ENS_Port::forwardInputBuffer(pBufferHdr);
	}
	OUTR(" ", res);
	return(res);
}

OMX_ERRORTYPE SplitterPort::forwardOutputBuffer (OMX_BUFFERHEADERTYPE *pBufferHdr)
//*************************************************************************************************************
{ //When an input buffer is dispatch to its output (buffer sharing)
	IN0("");
	MSG2("Splitter_port[%d] forwardOutputBuffer for buffer Hdr %p\n", (int)getPortIndex(), pBufferHdr);
	OMX_ERRORTYPE res=ENS_Port::forwardOutputBuffer (pBufferHdr);
	OUTR(" ", res);
	return(res);
}
#endif


//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************


Splitter_RDB::Splitter_RDB(): ENS_ResourcesDB(1)
//*************************************************************************************************************
{
	IN0("");
	setDefaultNMFDomainType(RM_NMFD_PROCSIA);
	OUTR(" ", 0);
}
