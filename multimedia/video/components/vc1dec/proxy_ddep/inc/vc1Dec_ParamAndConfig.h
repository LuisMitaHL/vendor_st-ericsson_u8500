/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \brief   Contains specific parameter and configuration setting, from the
 *          OMX point of view, of the OMX component vc1dec
 * \author  ST-Ericsson
 */

#ifndef __vc1Dec_PARAMANDCONFIG_H
#define __vc1Dec_PARAMANDCONFIG_H

#include "VFM_Component.h"
#include "VFM_ParamAndConfig.h"
#include <vc1dec/arm_nmf/api/nmftype.idt>

// forward definition
class vc1Dec_Proxy;


/// @ingroup vc1Decoder
/// @brief This class contains the parameters and configuration of the vc1 Decoder OpenMax Component
/// It is also used to set and get the configuration, without processing.
///
/// It is derived from VFM_ParamAndConfig.
/// @note The size of the frame (input and output) is stored inside the VFM_Port associated
/// with the vc1 Decoder Proxy (that can be accessed through the attribute pComponent).


class vc1Dec_ParamAndConfig: public VFM_ParamAndConfig, public TraceObject
{
    friend class vc1Dec_Proxy;
    friend class vc1Dec_Port;
    
	public:
// +CR349396 CHANGE START FOR
		OMX_ERRORTYPE setRecyclingDelay(OMX_PTR pt_org);
// -CR349396 CHANGE START FOR
        OMX_ERRORTYPE getIndexConfigCommonOutputCrop(OMX_PTR pt_org) const;
        OMX_U32 CropWidth;
        OMX_U32 CropHeight;

    protected:
        /// @brief Constructor, that inializes the default configuration
        /// and set the OpenMax Component the configuration is related to
        vc1Dec_ParamAndConfig(VFM_Component *pComponent);
    
        OMX_ERRORTYPE reset();

#ifdef PACKET_VIDEO_SUPPORT
        IMPORT_C virtual OMX_ERRORTYPE getPVCapability(OMX_PTR pt_org);
#endif
};


#endif
