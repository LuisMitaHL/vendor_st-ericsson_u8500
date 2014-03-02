/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264Dec_PARAMANDCONFIG_H
#define __H264Dec_PARAMANDCONFIG_H

#include "VFM_Component.h"
#include "VFM_ParamAndConfig.h"


// forward definition
class H264Dec_Proxy;


/// @ingroup H264Decoder
/// @brief This class contains the parameters and configuration of the H264 Decoder OpenMax Component
/// It is also used to set and get the configuration, without processing.
///
/// It is derived from VFMEnc_ParamAndConfig.
/// @note The size of the frame (input and output) is stored inside the VFM_Port associated
/// with the H264 Deocder Proxy (that can be accessed through the attribute mComponent).
/// Port IPB+0 contains the size of the original frame, and port IPB+1 contains the
/// size of the cropped encoded frame, and can be retrived using
/// mComponent->getFrameHeight(VPB+0), mComponent->getFrameWidth(VPB+0),
/// mComponent->getFrameHeight(VPB+1) and mComponent->getFrameWidth(VPB+1)

class H264Dec_ParamAndConfig: public VFM_ParamAndConfig
{
    public:
        inline OMX_BOOL isDeblockingOn()            { return mIsDeblockingOn; }
        inline OMX_BOOL getParallelDeblocking()     { return mParallelDeblocking; }
        inline OMX_NALUFORMATSTYPE getNalFormat()   { return mNalFormat; }
// +CR324558 CHANGE START FOR
		inline OMX_BOOL isDpbSizeStatic()            { return mIsDPBSizeStatic; }
// -CR324558 CHANGE END OF
    public:
        /// @brief Constructor, that inializes the default configuration
        /// and set the OpenMax Component the configuration is related to
        H264Dec_ParamAndConfig(VFM_Component *pComponent);

        OMX_ERRORTYPE reset();

        OMX_ERRORTYPE getIndexConfigMemory(OMX_PTR pt_org) const;
        OMX_ERRORTYPE setIndexParamCommonDeblocking(OMX_PTR pt_org);
	    OMX_ERRORTYPE getIndexParamCommonDeblocking(OMX_PTR pt_org) const;
        OMX_ERRORTYPE setIndexParamParallelDeblocking(OMX_PTR pt_org);
	    OMX_ERRORTYPE getIndexParamParallelDeblocking(OMX_PTR pt_org) const;
#ifdef PACKET_VIDEO_SUPPORT
        IMPORT_C virtual OMX_ERRORTYPE getPVCapability(OMX_PTR pt_org);
#endif

        OMX_ERRORTYPE setIndexParamNalStreamFormat(OMX_PTR pt_org);
        OMX_ERRORTYPE getIndexParamNalStreamFormat(OMX_PTR pt_org) const;
        OMX_ERRORTYPE getIndexParamNalStreamFormatSupported(OMX_PTR pt_org) const;

        OMX_ERRORTYPE getIndexConfigCommonOutputCrop(OMX_PTR pt_org) const;

// +CR324558 CHANGE START FOR
		OMX_ERRORTYPE setIndexParamStaticDpbSize(OMX_PTR pt_org);
		OMX_ERRORTYPE getIndexParamStaticDpbSize(OMX_PTR pt_org) const;
// -CR324558 CHANGE END OF
// +CR349396 CHANGE START FOR
		OMX_ERRORTYPE setRecyclingDelay(OMX_PTR pt_org);
// -CR349396 CHANGE START FOR
		OMX_U16 getLevelCapabilities() { return mRestrictMaxlevel;}
		void setLevelCapabilities(OMX_U16 level) { mRestrictMaxlevel = level;}

    protected:
        OMX_BOOL mIsDeblockingOn;
        OMX_BOOL mParallelDeblocking;
        OMX_NALUFORMATSTYPE mNalFormat;
// +CR324558 CHANGE START FOR
		OMX_BOOL mIsDPBSizeStatic;
// -CR324558 CHANGE END OF
		OMX_U16 mRestrictMaxlevel;
	public:
        /* +Change for CR 369593 */
        OMX_U32 CropWidth;
        OMX_U32 CropHeight;
        /* -Change for CR 369593 */
};

#endif
