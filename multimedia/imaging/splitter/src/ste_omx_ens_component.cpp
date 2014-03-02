/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ste_omx_ens_component_loader.h"
#include <ENS_Component.h>
//#include "ste_ens_imaging_components.h"
#include "../src/splitter_shared.h"


/** Definition of all the components this module will register to the STE ENS loader*/
static struct ste_omx_ens_component_def_array components_to_register = 
{ {
	{ {IMAGESPLITTER_BROADCAST_NAME},  { {"video_src.splitter"},            {0} }, imageSplitterFactoryMethod },
	{ {IMAGESPLITTER_SEQUENTIAL_NAME}, { {"video_src.splitter_sequential"}, {0} }, imageSplitter_Sequential_FactoryMethod },
	{{0}, {{0}}, 0}
} };

extern "C"
{
	/** The function the OMX ENS loader is calling to register the components of this module*/
	void ste_omx_ens_component_register(struct ste_omx_ens_component_def_array **refs)
	{
		*refs = &components_to_register;
	}
}
