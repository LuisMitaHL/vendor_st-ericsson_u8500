/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ste_omx_ens_component_loader.h"
#include "ext_ste_ens_imaging_components.h"

/** Definition of all the components this module will register to the STE ENS loader*/
static struct ste_omx_ens_component_def_array components_to_register =
    { {
	    {{"OMX.ST.ExtHSMCamera"}, {{"video_src.camera"}, {0}}, OMXExtCameraFactoryMethod},
	    {{"OMX.ST.ExtHSMCamera2"}, {{"video_src.seccamera"}, {0}}, OMXExtCameraSecondaryFactoryMethod},
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
