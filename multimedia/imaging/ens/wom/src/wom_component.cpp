/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "wom_shared.h"
#include <stdio.h>
#include <omxil/OMX_Types.h>
#include <omxil/OMX_Core.h>
#include "ImgEns_ProcessingComponent.h"
//Omx definitions
#include <omxil/OMX_Component.h>

#include "ImgEns_Port.h"
#include "ImgEns_Component.h"
#include "wom_port.h"
#include "wom_component.h"
#include "Img_EnsWrapper.h"

Wom_Component::Wom_Component(const char *name, unsigned int instanceNb)
	: ImgEns_Component(name, instanceNb)
	, m_pWrapper(NULL)
	, m_bUseBufferSharing(false)
//*******************************************************************************
{
	WOM_ASSERT(Global_ImgWrapperVersion == GLOBAL_IMGWRAPPER_VERSION);
}

Wom_Component::~Wom_Component()
//*******************************************************************************
{
/* No because it's the responsability of the wrapper to 
	if (m_pWrapper)
	{
		delete m_pWrapper;
		m_pWrapper= NULL;
	} */
}

int Wom_Component::Construct(OMX_COMPONENTTYPE &Handle, const char *name)
//*******************************************************************************
{ //Make default initialisation
	if (m_pWrapper == NULL)
	{
		Img_EnsWrapper *pWrapper= new Img_EnsWrapper(this, &Handle);
		SetWrapper(pWrapper); // By this way it will not be created by bellagio loader
	}
	if (name == NULL)
		name="Wom_Component";
	return(S_OK);
}


int Wom_Component::Destruct()
//*******************************************************************************
{ //clean default initialisation
/*	if (m_pWrapper == NULL)
	{
		delete m_pWrapper;
		m_pWrapper= NULL;
	}   */
	return(S_OK);
}

void Wom_Component::SetWrapper(Img_EnsWrapper *pWrapper)
//*******************************************************************************
{
	if (m_pWrapper)
		delete m_pWrapper;
	m_pWrapper = pWrapper;
}

/**
Create relevant port versus domain
*/
int Wom_Component::CreatePort (const ImgEns_CommonPortData &aPortData)
//*******************************************************************************
{
	if (aPortData.mPortIndex >= getPortCount())
	{
		ReportError(OMX_ErrorBadPortIndex, "Cannot create port %d >= %d\n", aPortData.mPortIndex, getPortCount());
		return OMX_ErrorBadPortIndex;
	}

	ImgEns_Port* port=NULL;
	switch(aPortData.mPortDomain)
	{
	case OMX_PortDomainVideo:
		port= new Wom_PortVideo(aPortData, *this);
		break;
	case OMX_PortDomainOther:
		port= new Wom_PortOther(aPortData, *this);
		break;
	default:
		// not supported today: OMX_PortDomainAudio, OMX_PortDomainImage, OMX_PortDomainOther,OMX_PortDomainKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */ 
		ReportError(eError_UnsupportedDomain, "Domain %d not supported\n", aPortData.mPortDomain);
		port=NULL;
		return eError_UnsupportedDomain;
	}
	if (port == 0)
	{
		ReportError(OMX_ErrorInsufficientResources, "creation of port %d fails\n", aPortData.mPortIndex);
		return OMX_ErrorInsufficientResources;
	}
	else
		addPort(port);

	return OMX_ErrorNone;
}

OMX_PTR Wom_Component::getConfigAddr(OMX_INDEXTYPE nIndex, OMX_PTR pParameter, size_t *pStructSize, int *pOmxStructOffset) const
//*******************************************************************************
{
	return(NULL);
}

