/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson OUTR(" " and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "womDemoCpt_shared.h"
#include <stdio.h>
#include <stdarg.h>

#undef    OMXCOMPONENT
#define   OMXCOMPONENT "womDemoCpt_component"
#undef    OMX_TRACE_UID
#define   OMX_TRACE_UID 0x8
#include "osi_trace.h"

#include "wom_lib.h"
#include "womDemoCpt_component.h"


womDemoCpt_Component::womDemoCpt_Component(const char *name, unsigned int instanceNb):
	Wom_Component(name, instanceNb)
//*************************************************************************************************************
{
#if (OMX_BUFFER_SHARING!=0)
	m_bUseBufferSharing=true;
#else
	m_bUseBufferSharing=false;
#endif
#if 0
	// For initializing a OMX_STRUCT
	// Make default initialisation for IFM_CONFIG_NORCOS_CONTROLMODE struct
	m_Rotation.nSize    = sizeof(OMX_CONFIG_ROTATIONTYPE);
	m_Rotation.nRotation= 0; //set default value
	getOmxIlSpecVersion(m_Rotation.nVersion);
#endif
}

womDemoCpt_Component::~womDemoCpt_Component()
//*************************************************************************************************************
{
	;
}

OMX_ERRORTYPE womDemoCpt_Component::getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const
//*************************************************************************************************************
{ // If want to manage specific index
#if 0
	if (0 == strcmp(cParameterName, "OMX_INDEX_SPLITTER_CONFIG"))
	{
		*pIndexType = (OMX_INDEXTYPE)IFM_IndexConfigSetting;
		return OMX_ErrorNone;
	}
	return (CExtension::getExtensionIndex(cParameterName, pIndexType));
#else
	return(OMX_ErrorNone);
#endif
}

/**
Make construction of the ImgEns_Component by giving the nbr of ports and their configuration
*/
int womDemoCpt_Component::Construct(OMX_COMPONENTTYPE &Handle, const char *name)
//*************************************************************************************************************
{ // Construct ports
	IN0("\n");
	int  error;
	int status=eNoError;
	const int MinBufferCount = 1;
	const size_t MinBuffSize = 0;
	const int NbPorts        = 2;
	const OMX_U32 nRoleCount = 1;

	Wom_Component::construct(NbPorts, nRoleCount); //Construct ports

	int portNb=0;

	//Input port
	portNb=0;
	const ImgEns_CommonPortData PortDataIn(portNb, OMX_DirInput, MinBufferCount, MinBuffSize, OMX_PortDomainVideo, OMX_BufferSupplyInput /*OMX_BufferSupplyUnspecified*/);
	error=CreatePort(PortDataIn);
	if (error!=OMX_ErrorNone)
	{
		ReportError(eError_CannotCreateInputPort, "Cannot create Input port[%d] (video)", portNb);
		status=error;
	}

	//Output port
	portNb=1;
	const ImgEns_CommonPortData PortDataOut(portNb, OMX_DirOutput, MinBufferCount, MinBuffSize, OMX_PortDomainVideo, OMX_BufferSupplyOutput /*OMX_BufferSupplyUnspecified*/);
	error=CreatePort(PortDataOut);
	if (error!=OMX_ErrorNone)
	{
		ReportError(eError_CannotCreateOutputPort, "Cannot create Output port[%d]  (other)", portNb);
		status=error;
	}

	if (status==eNoError)
	{ // do other initialisation
		ImgEns_Port *pPort=this->getPort(1);
		if (pPort!=NULL)
		{
			//pPort->GmParamPortDefinition.nSize=sizeof(RER_metadata); //No taccessible
		}
	}

	if (m_bUseBufferSharing==true)
	{	/* code for buffer sharing */
		const int in=0, out=1;
		MSG2("\n*****************************\n Initialise buffer sharing for womDemoCpt [%d]->[%d]\n", in, out);
		error = connectBufferSharingPorts(in, out); /*Todo set right ports here */
		if(error!=OMX_ErrorNone) 
		{
			ReportError(eError_CannotCreateOutputPort, "Cannot make buffer sharing between ports %d and %d", in, out);
			status=error;
		}
		MSG2("\n Port %d and %d connected\n*****************************\n", in, out);
	}

	// Finally call base class
	Wom_Component::Construct(Handle, name);

	OUTR("", (status));
	return(status);
}

OMX_PTR womDemoCpt_Component::getConfigAddr(OMX_INDEXTYPE nIndex, OMX_PTR pParameter, size_t *pStructSize, int *pOmxStructOffset) const
//*************************************************************************************************************
{ //return address of the internal config struct attached to the Index.
  // *pStructSize will contain the size of internal struct (can be different from omx one)
  // *pOmxStructOffset is the offset in the omx struct for having intersting data. 
	IN0("\n");
	size_t size=0;
	OMX_PTR pStructure=NULL;
	int offset=0;
	switch ((int)nIndex)
	{
/* sample of code
	case OMX_IndexConfigCommonMirror:
		size=sizeof(OMX_CONFIG_MIRRORTYPE);
		pStructure= (void *)&m_Mirror;
		break;
	case OMX_IndexConfigCommonRotate:
		size=sizeof(bool);
		pStructure= (void *)&m_bRotate;
		offset=offsetof(OMX_CONFIG_ROTATETYPE, bRotated); //Keep only boolean value
		break;
*/
	case 0: // a virer
		break;
	default : // call base class
		return( Wom_Component::getConfigAddr(nIndex, pParameter, pStructSize, pOmxStructOffset));
	}
	OUTR("", (OMX_ErrorNone));
	if (pStructSize)
	{ //return the size of the struct
		*pStructSize=size;
	}
	if (pOmxStructOffset)
	{ //return the offset in the setparam
		*pOmxStructOffset=offset;
	}

	return pStructure;
}



/**
call when a set param has been done on a port
*/
int womDemoCpt_Component::PortFormatChanged(ImgEns_Port & /*port*/)
//*************************************************************************************************************
{//Check if setting are ok
	/*
	PortVideo &Port=(PortVideo &)port;
	OMX_PARAM_PORTDEFINITIONTYPE&  PortDef=Port.getParamPortDefinition();
	if (PortDef.format.video.eColorFormat != xxx)
	{
		return( -2);
	}
	*/
	//By default done nothing
	return(S_OK);
}
