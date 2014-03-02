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
#define   OMXCOMPONENT "womDemoCpt_proxy"
#undef    OMX_TRACE_UID
#define   OMX_TRACE_UID 0x8
#include "osi_trace.h"

#include "wom_lib.h"

#include "womDemoCpt_component.h"
#include "womDemoCpt_proxy.h"
#include "womDemoCpt_process.h"
//#include "extradata.h" // used for both extradata & extensions

//global variable for ost trace
#ifndef DBGT_VAR_INIT
	#define DBGT_VAR_INIT 0
#endif
int mDebugTraceVarwomDemoCpt=DBGT_VAR_INIT;

Wom_Processor *Create_womDemoCpt_processor(const char *name, void *usrptr)
//*************************************************************************************************************
{ // function Factory for womDemoCpt_process
	// The construction of this class must be delayed till the component go in idle
	return(new womDemoCpt_processor(name, usrptr));
}

int Factory_womDemoCpt(OMX_COMPONENTTYPE &Handle)
//*************************************************************************************************************
{  //factory method for Red Eye Detection
	GET_AND_SET_TRACE_LEVEL(womDemoCpt); // Declare and use environment variable 'dbgt_womDemoCpt_level' 
	// that can be enabled by setenv dbgt_womDemoCpt_level on linux, or export dbgt_womDemoCpt_level on symbian

	Wom_Component * pEnsComponent =  new womDemoCpt_Component();
	if (pEnsComponent  == 0) 
	{
		ReportError(OMX_ErrorInsufficientResources, "new womDemoCpt_Proxy failed");
		OUTR("", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}

	// now try to really construct the component
	if (pEnsComponent->Construct(Handle)!= eNoError)
	{
		ReportError(OMX_ErrorInsufficientResources, "womDemoCpt_Proxy::construct failed");
		delete pEnsComponent;
		OUTR("", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}

	womDemoCpt_Proxy * pProxy = new womDemoCpt_Proxy(*pEnsComponent);
	if (pProxy == 0) 
	{
		ReportError(OMX_ErrorInsufficientResources, "new penMax_Proxy failed for womDemoCpt");
		delete pEnsComponent;
		OUTR("", (OMX_ErrorInsufficientResources));
		return OMX_ErrorInsufficientResources;
	}
	return OMX_ErrorNone;
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
#undef    OMXCOMPONENT
#ifdef __SYMBIAN32__
	#define   OMXCOMPONENT "womDemoCpt_Proxy"
#else
	#define   OMXCOMPONENT GetComponentName()
#endif

womDemoCpt_Proxy::womDemoCpt_Proxy(ImgEns_Component &enscomp)
: Wom_Proxy("womDemoCpt_proxy", enscomp, mI_ToEnsComponent, Create_womDemoCpt_processor)
, mI_ToEnsComponent(enscomp)
//*************************************************************************************************************
{
	// Init default parameters
}

womDemoCpt_Proxy::~womDemoCpt_Proxy()
//*************************************************************************************************************
{
	;
}

