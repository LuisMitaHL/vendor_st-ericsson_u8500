/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __EXIF_MIXER_PARAMANDCONFIG_H
#define __EXIF_MIXER_PARAMANDCONFIG_H

#include "VFM_Component.h"
#include "VFM_ParamAndConfig.h"


/// @ingroup EXIFMixer
/// @brief This class contains the parameters and configuration of the JPEG Encode OpenMax Component
/// It is also used to set and get the configuration, without processing.
///
/// It is derived from VFMEnc_ParamAndConfig.
/// @note The size of the frame (input and output) is stored inside the VFM_Port associated
/// with the JPEG Encoder Proxy (that can be accessed through the attribute mComponent).
/// Port IPB+0 contains the size of the original frame, and port IPB+1 contains the
/// size of the cropped encoded frame, and can be retrived using
/// mComponent->getFrameHeight(VPB+0), mComponent->getFrameWidth(VPB+0),
/// mComponent->getFrameHeight(VPB+1) and mComponent->getFrameWidth(VPB+1)

class EXIFMixer_ParamAndConfig: public VFM_ParamAndConfig
{
    friend class EXIFMixer_Proxy;
   	friend class EXIFMixer_ArmNmfProcessingComponent;
   	friend class EXIFMixer_Port;

    public:
        /// @brief Constructor, that inializes the default configuration
        /// and set the OpenMax Component the configuration is related to
        EXIFMixer_ParamAndConfig(VFM_Component *component);

    private:
        /// @brief Set a structure of type OMX_IMAGE_PARAM_PORTFORMATTYPE. It makes use
        /// of the mComponent attribute.
        /// @param [in] pt_org structure of type OMX_IMAGE_PARAM_PORTFORMATTYPE, that contains the
        /// rotation to set
        /// @param [out] has_changed true if the parameters have been modified by
        /// this operation, false otherwise
        /// @return OMX_ErrorNone if no error occured, an error otherwise (OMX_ErrorBadParameter)
        OMX_ERRORTYPE setIndexParamImagePortFormat(OMX_PTR pt_org, OMX_BOOL *has_changed);

        /// @brief Get a structure of type OMX_IMAGE_PARAM_PORTFORMATTYPE. It makes use
        /// of the mComponent attribute.
        /// @param [in,out] pt_org structure of type OMX_IMAGE_PARAM_PORTFORMATTYPE,
        /// that will contain the information to get
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        OMX_ERRORTYPE getIndexParamImagePortFormat(OMX_PTR pt_org) const;

        /// @brief Get a structure of type OMX_PARAM_PORTDEFINITIONTYPE. It makes use
        /// of the mComponent attribute.
        /// @param [in,out] pt_org structure of type OMX_PARAM_PORTDEFINITIONTYPE,
        /// that will contain the information to get
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        OMX_ERRORTYPE getIndexParamPortDefinition(OMX_PTR pt_org) const;
};


#endif
