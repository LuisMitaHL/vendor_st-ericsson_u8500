/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "ste_omx_ens_component_loader.h"
#include <ENS_Component.h>
#include "../src/armivproc_shared.h"

#ifdef _MSC_VER
	#define WEAK_ATTRIBUTE
#else
	#define WEAK_ATTRIBUTE __attribute__((weak))
#endif

OMX_ERRORTYPE Factory_ArmIVProc(ENS_Component ** ppENSComponent) WEAK_ATTRIBUTE;


/** Definition of all the components this module will register to the STE ENS loader*/
static struct ste_omx_ens_component_def_array components_to_register = 
{ 
	{
		{ {ARMIVPROC_NAME1   /*"OMX.STE.VISUALPROCESSOR.2D-OPERATIONS.SW"*/}, { {"image_src.visual2Dprocessor"}, {0} }, Factory_ArmIVProc},
		{ {ARMIVPROC_NAME_OBSOLETE                   /*"OMX.ST.ARMIVPROC"*/}, { {"image_src.armivproc"}        , {0} }, Factory_ArmIVProc},
		{ {0}, { {0} }, 0}
	} 
};

extern "C"
{
	/** The function the OMX ENS loader is calling to register the components of this module*/
	void ste_omx_ens_component_register(struct ste_omx_ens_component_def_array **refs)
	{
		*refs = &components_to_register;
	}
}
