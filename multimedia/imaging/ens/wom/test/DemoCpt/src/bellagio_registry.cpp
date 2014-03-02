/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#ifdef _MSC_VER
	#include <afx.h>
#endif

#include "omxil/OMX_Core.h"
#include "omxil/OMX_Component.h"
#include "Img_omxloader_interface.h"
#include "../src/womDemoCpt_shared.h"

extern WOMDEMOCPT_API int  Factory_womDemoCpt(OMX_COMPONENTTYPE &Handle);

const char *womDemoCpt_Roles[]={ "womDemoCpt.role0", "womDemoCpt.role1", NULL /* NULL is mandatory */ };

/** Definition of all the components this module will register to the STE ENS loader*/
static _tImgOmxFactoryDescription womDemoCptFactories[] =
{     // Names                                                         , Roles[]                               , Factory
	{ WOMDEMOCPT_NAME0   , womDemoCpt_Roles  , (int (*)(void *)) Factory_womDemoCpt },
	{ WOMDEMOCPT_NAME1   , womDemoCpt_Roles  , (int (*)(void *)) Factory_womDemoCpt },
//		{NULL, {NULL}, NULL}, //Last element, just in case of
};

extern "C" WOMDEMOCPT_API int ste_omx_img_component_register (const _tImgOmxFactoryDescription *&pFactoryDescription)
{
#ifdef _MSC_VER
	_crtBreakAlloc=826;
#endif
		int nbr= sizeof(womDemoCptFactories) / sizeof(womDemoCptFactories[0]);
	pFactoryDescription = womDemoCptFactories;
	return(nbr);
}
