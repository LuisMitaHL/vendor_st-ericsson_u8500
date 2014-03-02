/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <OMX_Core.h>
#include <OMX_Index.h>
#include <OMX_Core.h>
#include <ENS_Component_Fsm.h>
#include <cm/inc/cm_macros.h>

#undef    OMXCOMPONENT
#define   OMXCOMPONENT "portInformation"
#undef  OMX_TRACE_UID
#define OMX_TRACE_UID 0x8
#include "osi_trace.h"

#include "../../../../proxy/include/wrapper_openmax_tools.h"
#include "openmax_processor.nmf"
#include "../inc/openmax_processor.hpp"
#include "../../../../proxy/include/openmax_component.h"

void portInformation::Default()
//*******************************************************************************
{ //Make default initialisation
	BufferHeaderList   = NULL;
	fifoSize         = 0;
	width            = 0;
	height           = 0;
	colorFormat      = (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatUnused;
	omxPortIndex     = 0;
	nbAllocatedHeader= 0;
	direction        = OMX_DirInput;//Port direction Input or Output
	bufferSupplier   = 0;
}

int portInformation::AllocateBufferHeader()
//*******************************************************************************
{ ///allocate the list of buffer header
	OMX_BUFFERHEADERTYPE *pBufHeader;
	const size_t nbr=fifoSize;
	DeleteBufferHeader();
	if (nbr!= 0)
	{
		BufferHeaderList=new OMX_BUFFERHEADERTYPE*[nbr]; //Allocate the array of ptr
		if (BufferHeaderList==NULL)
			return(eError_CannotAllocateHeaderList);
		pBufHeader    =new OMX_BUFFERHEADERTYPE[nbr];
		if (pBufHeader==NULL)
		{
			delete [] BufferHeaderList; //Must clean before return
			BufferHeaderList=NULL;
			return(eError_CannotAllocateHeader);
		}
		//initialise the list
		size_t index=0;
		for (index=0; index < nbr; ++index)
		{
			BufferHeaderList[index]=pBufHeader;
			++pBufHeader;
		}
	}
	nbAllocatedHeader=nbr;
	return(eNoError);
}

void portInformation::DeleteBufferHeader()
//*******************************************************************************
{
	IN0("DeleteBufferHeader");
	if ((nbAllocatedHeader !=0) && (BufferHeaderList!=NULL))
	{
		if (BufferHeaderList[0])
		{
			delete [] BufferHeaderList[0];     //Delete the array of ptr
		}
		else
		{
			WARNING(BufferHeaderList[0]);
		}
		delete [] BufferHeaderList;     //Delete the array of ptr
	}
	BufferHeaderList= NULL;
	nbAllocatedHeader  = 0;
	OUT0("\n");
}

//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
#undef    OMXCOMPONENT
#ifdef __SYMBIAN32__
	#define   OMXCOMPONENT "openmax_processor"
#else
	#define   OMXCOMPONENT GetComponentName()
#endif

EXPORT_C openmax_processor::openmax_processor(const char *name, OpenMax_Component* OMXowner)
	: m_pPort(NULL)
	, m_pPortInfo(NULL)
	, m_bUseBufferSharing(false) //no buffer sharing by default
	, mProcessorTemplate( *this )
//*******************************************************************************
{
	IN0("\n");
	SetComponentName(name);
	//Set user pointer to null
	m_UserPtr      = NULL;
	m_LastError    = 0;
	m_NbPorts      = 0;

	m_fn_UserClose          = NULL;
	m_fn_UserOpen           = NULL;
	m_fn_UserSetConfig      = NULL;
	m_fn_UserProcessBuffer  = NULL;
	m_fn_UserSetTuning      = NULL;
	OUT0("\n");
}

openmax_processor::~openmax_processor()
//*******************************************************************************
{
	Deinit();
}

int openmax_processor::Init(size_t nbPort)
//*******************************************************************************
{ // Make the binding of interfaces
	int result= eNoError;
	Deinit(); //First clean if necessary
	m_NbPorts=nbPort;
	m_pPortInfo = new portInformation[nbPort];
	if (m_pPortInfo ==NULL)
	{
		result= eError_CannotAllocate_PortInfo;
	}
	else
	{ // Init all port info to null;
		portInformation *pInfo;
		for(size_t i=0; i < nbPort; i++)
		{ //Default ports initialisation
			pInfo=&m_pPortInfo[i];
			pInfo->Default();
		}
	}
	//Allocate port array
	m_pPort = new Port[nbPort];
	if (m_pPort ==NULL)
	{
		result= eError_CannotAllocate_Ports;
	}
	else
	{ // Init all port info to null;
	}
	return(result);
}

void openmax_processor::setTargetComponent(t_uint32 NbPort, void * pEnsComponent/*component_addr*/)
//*******************************************************************************
{ //This function must be called before the start of the component
	int res=Init(NbPort);
	SetOpenMax_Component((OpenMax_Component*)pEnsComponent); //Set the ens component
	WARNING(res==0);
}

int openmax_processor::Deinit(int  /*flags*/)
//*******************************************************************************
{
	int result= eNoError;
	if (m_pPortInfo)
	{
		for (size_t i=0; i <GetNbPorts(); ++i)
		{ //clean all headers
			GetPortInfo(i)->DeleteBufferHeader();
		}
		delete []m_pPortInfo;
		m_pPortInfo=NULL;
	}
	if (m_pPort)
	{
		delete []m_pPort;
		m_pPort=NULL;
	}
	return(result);
}

int openmax_processor::GetConvertionConfig (_tConvertionConfig &Config, bool bDequeue)
//*************************************************************************************************************
{ //This method is called for preparing ConvertionConfig struct
	IN0(" this function need to be overloaded");
	WARNING(0); // Need to be overloaded
	Config.pProcessor=this;
	Config.DataPtr=NULL; //Use for internal processing
	Config.UserPtr=NULL;
	OUT0("\n");
	return(-1);
}

Port *openmax_processor::GetPort(size_t index) const
//*******************************************************************************
{ // return the number of ports for a given family
	if (( index < GetNbPorts()) && m_pPort!=NULL)
		return( & m_pPort[index] );
	else
		return(NULL);
}

portInformation* openmax_processor::GetPortInfo(size_t index) const
//*******************************************************************************
{/* Return the right port info address*/ ;
	if ( index < GetNbPorts())
		return( & m_pPortInfo[index] );
	else
		return(NULL);
}

int openmax_processor::InitBufferInfo(struct sBufferInfo &Info, const int Index, bool bDequeue)
//*******************************************************************************
{ //Init BufferInfo struct and dequeueBuffer
	IN0("\n");
	int res=S_OK;
	portInformation *pPortInfo=NULL;
	portInformation EnsPortInfo;
	ENS_Port *pPort=NULL;

	// Set input struct
	if (Index >=0)
	{
		OpenMax_Component *pEnsCpt=GetOpenMax_Component();
		if (pEnsCpt!=NULL)
		{
			pPort=pEnsCpt->getPort(Index);
			if (pPort!=NULL)
			{
				OMX_VIDEO_PORTDEFINITIONTYPE *pDef=pPort->getVideoPortDefinition();
				if (pDef!=NULL)
				{
					EnsPortInfo.colorFormat= pDef->eColorFormat;
					EnsPortInfo.width      = pDef->nFrameWidth;
					EnsPortInfo.height     = pDef->nFrameHeight;
					EnsPortInfo.stride     = pDef->nStride;
					pPortInfo=&EnsPortInfo;
				}
			}
		}
		if (pPortInfo==NULL)
		{
			pPortInfo= GetPortInfo(Index);
		}
		if (pPortInfo==NULL)
		{
			res=-1;
			InitBufferInfo(Info, -1, false);
			OUTR("", res);
			return(res);
		}
		if (bDequeue)
			Info.pBufferHdr = GetPort(Index)->dequeueBuffer();
		else
			Info.pBufferHdr = NULL;
		Info.Format   = pPortInfo->colorFormat;
		Info.Width    = pPortInfo->width;
		Info.Height   = pPortInfo->height;
		Info.Stride   = pPortInfo->stride;
		Info.pPort    = GetPort(Index);
		Info.Error    = 0;
		if (pPort)
			Info.pImageInfo= &((PortOther *)pPort)->GetImageInfo();
		else
			Info.pImageInfo= NULL;
	}
	else
	{ //Set default value
		pPortInfo= NULL;
		Info.pBufferHdr = NULL;
		Info.Format     = OMX_COLOR_FormatUnused;
		Info.Width      = 0;
		Info.Height     = 0;
		Info.Stride     = 0;
		Info.pPort      = NULL;
		Info.Error      = 0;
		Info.pImageInfo = NULL;
	}
	OUTR("", res);
	return(res);
}

int openmax_processor::InitProcessingInfo(_tConvertionConfig &Config, bool bDequeue, const int InputIndex, const int OutputIndex, const int MetadataIndex)
//*******************************************************************************
{ //Init info struct before call Process function
	IN0("\n");
	//portInformation *pInfo;
	Config.pProcessor=this;

	// memcpy( &Config.ImageConfig, &proc_config_FILTER, sizeof(proc_config_FILTER));
	Config.UserPtr      = m_UserPtr;   // Give the pointer dedicater to user
	Config.DataPtr      = NULL;        //Use for internal processing

	// Set input struct
	InitBufferInfo(Config.Input, InputIndex, bDequeue);

	// set data struct for output buffer
	InitBufferInfo(Config.Output, OutputIndex, bDequeue);

	// set data struct for output buffer
	InitBufferInfo(Config.Metadata, MetadataIndex, bDequeue);

	OUT0("\n");
	return(0);
}

void openmax_processor::start(void)
//*************************************************************************************************************
{ //This method is called during Idle to Execute transition
	IN0("\n");
	// proc_config_FILTER.nAlgoType = 0; // no algo selected
	// Don't call m_fn_UserOpen here because port's parameters aren't ready
	OUT0("\n");
}

void openmax_processor::fsmInit(fsmInit_t initFsm)
//*************************************************************************************************************
{ //Make the initialisation of ports
	IN0("\n");
	if (initFsm.traceInfoAddr)
	{
		setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
	}
	portInformation *pPortInfo;
	Port *pPort;
	NMF::InterfaceReference *pInterfaceRef;
	const size_t NbPorts=GetNbPorts();
	for (size_t portIndex=0; portIndex < NbPorts ; ++portIndex)
	{
		pPort=GetPort(portIndex);
		pPortInfo=GetPortInfo(portIndex);
		pPortInfo->AllocateBufferHeader();

		if (pPortInfo->direction== OMX_DirInput)
		{ // for input
			pInterfaceRef=&mProcessorTemplate.emptybufferdone[portIndex]; // NMF 'return' interface
		}
		else
		{ //for output
			pInterfaceRef=&mProcessorTemplate.fillbufferdone[portIndex]; // NMF 'return' interface
		}

		pPort->init(pPortInfo->direction==OMX_DirInput?InputPort: OutputPort, // Direction
			false /*(bool)pPortInfo->bufferSupplier*/, // Buffer supplier
			true,                        // isHWPort ca fait quoi?
			NULL,                       // sharingPort
			pPortInfo->BufferHeaderList,
			pPortInfo->fifoSize,        // bufferCount (FIFO size )
			pInterfaceRef,              // NMF 'return' interface
			portIndex,                  // port index
			(initFsm.portsDisabled & (1 << portIndex) )?true:false,
			(initFsm.portsTunneled & (1 << portIndex) )?true:false,
			this // owner
		);
		MSG4("%s->InitPort(%d) Direction=%d Fifo=%d \n", GetComponentName(), portIndex, pPortInfo->direction, pPortInfo->fifoSize);
		for (int i=0; i< pPortInfo->fifoSize; ++i)
		{
			MSG2("BufferHeader[i]=%p \n", i, pPortInfo->BufferHeaderList[i]);
		}
		MSG0("\n");
	}
	init(NbPorts, m_pPort, &proxy /* Event Handler */, &me /* Postevent */, false);

	//Now call user init
	UserOpen();
	OUT0("\n");
}

int WRAPPER_OPENMAX_API openmax_processor::UserOpen()
//*******************************************************************************
{ // This function is called at the end of the processor initialisation. Called user defined function if exist 
	int res=S_OK;
	if (m_fn_UserOpen!=NULL)
	{
		_tConvertionConfig Config;
		GetConvertionConfig(Config, false);
		res=m_fn_UserOpen(Config, &m_UserPtr); //Call user configuration
		if (res !=S_OK)
		{
		}
	}
	return(res);
}

int /* Virtual */ WRAPPER_OPENMAX_API openmax_processor::UserClose()
//*******************************************************************************
{ // Called user defined function if exist 
	int res=OMX_ErrorNone;
	if (m_fn_UserClose !=NULL)
		res=m_fn_UserClose(&m_UserPtr);
	if (res==OMX_ErrorNone)
	{ /*Need to report properly errors*/
		 ;
	}
	return(res);
}

void WRAPPER_OPENMAX_API openmax_processor::setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled)
//*******************************************************************************
{
	IN0("\n");
	Port *pPort=GetPort(portIdx);
	if (pPort)
	{
		pPort->setTunnelStatus(isTunneled==0?false: true);
	}
	else
	{ //report error


	}
	OUT0("\n");
}

void openmax_processor::reset(void)
//*******************************************************************************
{// Called when port disable or deinstantiate
	IN0("\n");
	OUT0("\n");
}

void openmax_processor::process()
//*******************************************************************************
{// Need to add parameter for i and o
	IN0("\n");
	{
		WARNING(0); // this function need to be overided
	}
	OUT0("\n");
}

void openmax_processor::emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 portNb)
//*******************************************************************************
{ //Just wrap to the base component
	IN0("\n");
	Port *pPort=GetPort(portNb);
	WARNING(pPort);
	MSG3("%s->emptyThisBuffer[%lu] Header=%p \n", GetComponentName(), buffer->nInputPortIndex, (void *)buffer);
	WARNING(buffer->nInputPortIndex==portNb);
	Component::deliverBuffer(portNb, buffer);
	OUT0("\n");
}

void openmax_processor::fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 portNb)
//*******************************************************************************
{ //Just wrap to the base component
	IN0("\n");
	Port *pPort=GetPort(portNb);
	WARNING(pPort);
	MSG3("%s->fillThisBuffer[%lu] Header=%p \n", GetComponentName(), buffer->nOutputPortIndex, buffer);
	if (buffer->nOutputPortIndex!=portNb)
	{ // Should make more stuff
		WARNING(buffer->nOutputPortIndex==portNb);
	}
	Component::deliverBuffer(portNb, buffer);
	OUT0("\n");
}

void openmax_processor::setConfig(t_sint32 index, void* opaque_ptr)
//*******************************************************************************
{
	IN0("\n");
#if 0
	switch (index)
	{
	case IFM_IndexConfigCommonFilter :
		memcpy(&proc_config_FILTER, opaque_ptr, sizeof(IFM_CONFIG_FILTER));
		break;
	default:
		break;
	}
#endif
	_tConvertionConfig Config;
	GetConvertionConfig (Config, false);
	if (m_fn_UserSetConfig!=NULL)
		m_fn_UserSetConfig(Config, index, opaque_ptr);

	OUT0("\n");
}

void openmax_processor::setParam(t_sint32 portIndex, t_sint32 fifoSize, t_sint32 direction, t_sint32 buffSupplierType, t_sint32 correspondingPortIndex, t_sint32 width, t_sint32 height, t_sint32 colorFormat, t_sint32 stride)
//*************************************************************************************************************
{ //DEfault initialisation of the ports
	IN0("\n");
	MSG4("portIndex %ld, fifoSize %ld, direction %ld, buffSupplierType%ld\n", portIndex, fifoSize, direction, buffSupplierType);
	MSG4("correspondingPortIndex %ld, width %ld, height %ld, colorFormat %ld\n", correspondingPortIndex, width, height, colorFormat);
	MSG1("stride %ld\n", stride);
	if(portIndex >= (int)GetNbPorts()) // Sanity check
	{
		MSG0("ERROR in setParam OMX_ErrorBadParameter\n");
		OUTR(" Error", OMX_ErrorBadParameter);
		return;
	}

	portInformation*pInfo= GetPortInfo(portIndex);
	if (pInfo)
	{
		pInfo->fifoSize      = fifoSize;
		pInfo->width         = width;
		pInfo->height        = height;
		pInfo->colorFormat   = (OMX_COLOR_FORMATTYPE)colorFormat;
		pInfo->omxPortIndex  = portIndex;
		pInfo->stride        = stride;
		pInfo->direction     = (OMX_DIRTYPE) direction;
		pInfo->bufferSupplier= buffSupplierType;
		pInfo->omxPortIndex  = correspondingPortIndex;
	}
	OUT0("\n");
}

void openmax_processor::setParameter(t_sint32 nParamIndex, void* pParameterStructure)
//*************************************************************************************************************
{
	IN0("\n");
	if (nParamIndex == OMX_IndexParamPortDefinition)
	{
		OMX_PARAM_PORTDEFINITIONTYPE* portdef = (OMX_PARAM_PORTDEFINITIONTYPE*) pParameterStructure;
		delete portdef;
	}
	OUT0("\n");
}

void openmax_processor::sendCommand(OMX_COMMANDTYPE cmd, t_uword param)
//*************************************************************************************************************
{ // fsm.component.component.type interface sendcommand method sendCommand
	IN0("\n");
	Component::sendCommand(cmd, param) ;
	OUT0("\n");
}

void openmax_processor::processEvent(void)
//*************************************************************************************************************
{// fsm.component.component.type interface posteven method processEvent
	IN0("\n");
	Component::processEvent();
	OUT0("\n");
}


void openmax_processor::stop()
//*************************************************************************************************************
{ //Method called during destruction of OMX component
	IN0("\n");
	
	int res=UserClose();
	if (res!=OMX_ErrorNone)
	{ /*Need to report properly errors*/
		 ;
	}
	OUT0("\n");
}

const char *openmax_processor::GetComponentName() const
//*************************************************************************************************************
{
	return m_pName;
}

void openmax_processor::SetComponentName(const char *name)
//*************************************************************************************************************
{
	if (name)
		m_pName=name;
	else
		m_pName="";
}

size_t openmax_processor::GetNbPorts() const
//*************************************************************************************************************
{
	return(m_NbPorts);
}

int openmax_processor::ReportError(int error, const char *format, ...)
//*************************************************************************************************************
{
	IN0("\n");
	if (error == eError_NoError)
		return(0);
	if (m_LastError ==eError_NoError)
		m_LastError=error;  //Memorize the error
	va_list list;
	va_start(list, format);

#ifndef __SYMBIAN32__
	fprintf (stderr, "\n<Error-%s: %d=0x%X> ", m_pName, error, error);
	vfprintf(stderr, format, list);
#else
	static char ErrorString[1024];
	vsprintf(ErrorString, format, list);
	RDebug::Printf("\n<Error-%s: %d=0x%X: %s>", m_pName, error, error, ErrorString);
#endif
	va_end(list);
	//::ReportError(error, format); // Call global

	OUT0("\n");
	return(0);
}



/**
copy ExtraData zone
*/
int WRAPPER_OPENMAX_API openmax_processor::CopyExtraData(_tConvertionConfig &Config)
//*************************************************************************************************************
{ // copy the extra data zone
	IN0("\n");
	OMX_BUFFERHEADERTYPE* pOmxBufHdr=Config.Input.pBufferHdr;
	if (pOmxBufHdr==NULL)
		return(-1);
	OMX_U8* pInputEnd=pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen;
	OMX_U8* pInputExtraData = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	if ((pInputEnd==NULL) || (pInputExtraData==NULL)) 
		return(-2);
	if (pInputEnd <pInputExtraData) 
		return(-3);
	size_t nbr= pInputEnd - pInputExtraData;
	if (nbr == 0)
		return(S_OK); // No extra data

	pOmxBufHdr=Config.Output.pBufferHdr; //Now pOmxBufHdr point to output
	if (pOmxBufHdr==NULL)
		return(-4);
	OMX_U8* pOutputEnd= pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen;

	unsigned int Compute_nFillLen=pOmxBufHdr->nFilledLen; //NFilleLenght by default
	const _tImageInfo    *pImageInfo=Config.Output.pImageInfo;
	if ((pImageInfo!=NULL) && (pImageInfo->ImageSize!=0))
	{ // Override nFilledLen  with info coming from port
		if ( pOmxBufHdr->nFilledLen != 0)
		{ //check data
			if (pOmxBufHdr->nFilledLen != pImageInfo->ImageSize)
			{
				const char *pFormat="Unknow!";
				if ( (pImageInfo!=NULL) && (pImageInfo->pFormatDescription!=NULL) && (pImageInfo->pFormatDescription->Name !=NULL))
					pFormat=pImageInfo->pFormatDescription->Name;
				ReportError(eError_CorruptedMetada, "CopyExtradata potential problem with stride definition! port[%d] Output nFilledLen(%u) != Port.nFilledLen(%u). Image:%ux%u format %u '%s' stride=%u\n", pOmxBufHdr->nOutputPortIndex, pOmxBufHdr->nFilledLen, pImageInfo->ImageSize, Config.Output.Width, Config.Output.Height, Config.Output.Format, pFormat, Config.Output.Stride);
				WOM_ASSERT(pOmxBufHdr->nFilledLen == pImageInfo->ImageSize);
			}
		}
		else
		{
			Compute_nFillLen=pImageInfo->ImageSize; //take port info
		}
	}

	OMX_U8* pOutputExtraData;
	if (Compute_nFillLen== 0)
	{
		ReportError(eError_CorruptedMetada, "Output nFilledLen is null\n");
		return(-7);
	}
	//Use nFilledLen when possible
	pOutputExtraData = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + Compute_nFillLen;

	if ((pOutputEnd==NULL) || (pOutputExtraData==NULL)) 
		return(-5);
	if (pOutputEnd <pOutputExtraData) 
		return(-6);
	size_t nbr_dest= pOutputEnd - pOutputExtraData;
	if (nbr > nbr_dest)
		nbr = nbr_dest; // Cut  extra data

	MSG1("%d extradata copied\n",  nbr);
	memcpy(pOutputExtraData, pInputExtraData, nbr);
	OUT0("\n");
	return(S_OK);
}

/**
Return the ExtraData structur associated to given index
*/
int WRAPPER_OPENMAX_API openmax_processor::GetExtraData(const _tConvertionConfig &Config, const int aStructType, char *&pStruct)
//*************************************************************************************************************
{ 
#if 0
	/* get extradata start pointer */
	const OMX_BUFFERHEADERTYPE* pOmxBufHdr=Config.Input.pBufferHdr;
	OMX_U8* pEnd=pOmxBufHdr->pBuffer + pOmxBufHdr->nAllocLen;
	OMX_U8* pExtraData = pOmxBufHdr->pBuffer + pOmxBufHdr->nOffset + pOmxBufHdr->nFilledLen;
	OMX_OTHER_EXTRADATATYPE* pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pExtraData+ 3) & ~0x3);
	while ( ((OMX_U8*)pCurExtra < pEnd) &&  (pCurExtra->nSize>0) )
	{
		if( pCurExtra->eType==  (OMX_EXTRADATATYPE)aStructType)
		{ //found 
			unsigned int ComputeSize=(pCurExtra->nDataSize + sizeof(OMX_OTHER_EXTRADATATYPE) -1 + 3) &~3;
			if ( ComputeSize > pCurExtra->nSize)
			{ //Corrupted metadata
				MSG2("*** CORRUPTED metadata, sizes are not equal %d %d\n",  ComputeSize, pCurExtra->nSize);
				ReportError(eError_CorruptedMetada, "*** CORRUPTED metadata, sizes are not equal %d %d\n",  ComputeSize, pCurExtra->nSize);
			}
			pStruct=(char *)pCurExtra->data;
			return(S_OK);
		}
		pExtraData +=pCurExtra->nSize;
		//round pointer
		pCurExtra = (OMX_OTHER_EXTRADATATYPE*)(((OMX_U32)pExtraData+ 3) & ~0x3);
	}
	pStruct=NULL;
	return(-1);
#else
	return(::GetExtraData(Config.Input.pBufferHdr, aStructType, pStruct));
#endif
}

