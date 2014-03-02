/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_EXIFMixer
#include "osi_trace.h"

#include "EXIFMixer_ParamAndConfig.h"



//EXIFMixer_ParamAndConfig::EXIFMixer_ParamAndConfig(VFM_Component *component)
EXIFMixer_ParamAndConfig::EXIFMixer_ParamAndConfig(VFM_Component *component):VFM_ParamAndConfig(component)
{
    // initialization of attributes of VFM_ParamAndConfig
    setProfileLevel(0, 0);
}


OMX_ERRORTYPE EXIFMixer_ParamAndConfig::setIndexParamImagePortFormat(OMX_PTR pt_org, OMX_BOOL *has_changed)
{
    return OMX_ErrorNone;
}

OMX_ERRORTYPE EXIFMixer_ParamAndConfig::getIndexParamImagePortFormat(OMX_PTR pt_org) const
{
    return OMX_ErrorNone;
}

