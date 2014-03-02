/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __MPEG4DEC_PARAMANDCONFIG_H
#define __MPEG4DEC_PARAMANDCONFIG_H

#include "VFM_ParamAndConfig.h"
#include "VFM_Component.h"
/// @ingroup MPEG4Decoder
/// @brief This class contains the parameters and configuration of the MPEG4 Decode OpenMax Component
/// It is also used to set and get the configuration, without processing.

class MPEG4Dec_ParamAndConfig: public VFM_ParamAndConfig, public TraceObject
{
    public:
        /// @brief Constructor, that inializes the default configuration
        MPEG4Dec_ParamAndConfig(VFM_Component *component);

        OMX_ERRORTYPE getIndexParamPortDefinition(OMX_PTR pt_org) const;

        OMX_ERRORTYPE reset();

        //+CR369593
        OMX_U32 CropWidth;
        OMX_U32 CropHeight;
        //-CR369593

        OMX_VIDEO_PARAM_MPEG4TYPE mMpeg4Param;

        OMX_VIDEO_PARAM_PROFILELEVELTYPE mMpeg4ProfileAndLevel;

};


#endif /* __MPEG4DEC_PARAMANDCONFIG_H */
