/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264Enc_PARAMANDCONFIG_H
#define __H264Enc_PARAMANDCONFIG_H

#include "VFM_Component.h"
#include "VFM_ParamAndConfig.h"
#include "video_chipset_api_index.h"
#include "video_generic_chipset_api.h"

typedef OMX_CONFIG_COLORPRIMARY OMX_PARAM_COLORPRIMARY;

#include <frameinfo.idt>


// forward definition
class H264Enc_Proxy;


/// @ingroup H264Encoder
/// @brief This class contains the parameters and configuration of the H264 Encoder OpenMax Component
/// It is also used to set and get the configuration, without processing.
///
/// It is derived from VFM_ParamAndConfig.
/// @note The size of the frame (input and output) is stored inside the VFM_Port associated
/// with the H264 Encoder Proxy (that can be accessed through the attribute mComponent).
/// Port IPB+0 contains the size of the original frame, and port IPB+1 contains the
/// size of the cropped encoded frame, and can be retrived using
/// mComponent->getFrameHeight(VPB+0), mComponent->getFrameWidth(VPB+0),
/// mComponent->getFrameHeight(VPB+1) and mComponent->getFrameWidth(VPB+1)

class H264Enc_ParamAndConfig: public VFM_ParamAndConfig, public TraceObject
{
    public:
        t_frameinfo getParams();

        /// @brief Constructor, that inializes the default configuration
        /// and set the OpenMax Component the configuration is related to
        H264Enc_ParamAndConfig(VFM_Component *pComponent);

        void setDefault();

        OMX_ERRORTYPE setParam(OMX_VIDEO_PARAM_AVCTYPE*             aParamVideoAvc);
        OMX_ERRORTYPE setParam(OMX_VIDEO_PARAM_QUANTIZATIONTYPE*    aParamVideoQuantization);
        OMX_ERRORTYPE setParam(OMX_VIDEO_PARAM_AVCSLICEFMO*         aParamVideoSliceFMO);
        OMX_ERRORTYPE setParam(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE* aParamVideoFastUpdate);
        OMX_ERRORTYPE setParam(OMX_VIDEO_PARAM_INTRAREFRESHTYPE*    aParamVideoIntraRefresh);
        OMX_ERRORTYPE setParam(OMX_VIDEO_PARAM_BITRATETYPE*         aParamVideoBitrate);
        OMX_ERRORTYPE setParam(OMX_VIDEO_PARAM_VBSMCTYPE*           aParamVideoVbsmc);
        OMX_ERRORTYPE setParam(OMX_VIDEO_PARAM_MOTIONVECTORTYPE*    aParamVideoMotionVector);
        // SHAI extension
        OMX_ERRORTYPE setParam(OMX_PARAM_PIXELASPECTRATIO*          aParamPixelAspectRatio);
        OMX_ERRORTYPE setParam(OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE*  aParamColorPrimary);
        OMX_ERRORTYPE setParam(OMX_SYMBIAN_VIDEO_PARAM_AVCSEITYPE*  aParamVideoAvcSei);
        // Private extension
        OMX_ERRORTYPE setParam(VFM_PARAM_HEADERDATACHANGING_TYPE*   aParamHeaderDataChanging);
		//+ code for CR 332873
        OMX_ERRORTYPE setParam(OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE*   aParamCpbBufferSize);
		//- code for CR 332873
        OMX_ERRORTYPE setConfig(OMX_VIDEO_CONFIG_NALSIZE*           aConfigVideoNALSize);
        OMX_ERRORTYPE setConfig(OMX_VIDEO_CONFIG_BITRATETYPE*       aConfigVideoBitrate);
        OMX_ERRORTYPE setConfig(OMX_CONFIG_FRAMERATETYPE*           aConfigVideoFramerate);
        OMX_ERRORTYPE setConfig(OMX_CONFIG_INTRAREFRESHVOPTYPE*     aConfigVideoIntraVOPRefresh);
        OMX_ERRORTYPE setConfig(OMX_CONFIG_MACROBLOCKERRORMAPTYPE*  aConfigVideoMacroBlockErrorMap);
        OMX_ERRORTYPE setConfig(OMX_CONFIG_RECTTYPE*                aConfigCommonInputCrop);
        // SHAI extension
        OMX_ERRORTYPE setConfig(OMX_SYMBIAN_VIDEO_PARAM_SEQUENCEHEADERTYPE* aConfigVideoSequenceHeader);


        OMX_ERRORTYPE getParam(OMX_VIDEO_PARAM_AVCTYPE*             aParamVideoAvc) const;
        OMX_ERRORTYPE getParam(OMX_VIDEO_PARAM_QUANTIZATIONTYPE*    aParamVideoQuantization) const;
        OMX_ERRORTYPE getParam(OMX_VIDEO_PARAM_AVCSLICEFMO*         aParamVideoSliceFMO) const;
        OMX_ERRORTYPE getParam(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE* aParamVideoFastUpdate) const;
        OMX_ERRORTYPE getParam(OMX_VIDEO_PARAM_INTRAREFRESHTYPE*    aParamVideoIntraRefresh) const;
        OMX_ERRORTYPE getParam(OMX_VIDEO_PARAM_BITRATETYPE*         aParamVideoBitrate) const;
        OMX_ERRORTYPE getParam(OMX_VIDEO_PARAM_VBSMCTYPE*           aParamVideoVbsmc) const;
        OMX_ERRORTYPE getParam(OMX_VIDEO_PARAM_MOTIONVECTORTYPE*    aParamVideoMotionVector) const;
        // SHAI extension
        OMX_ERRORTYPE getParam(OMX_PARAM_PIXELASPECTRATIO*          aParamPixelAspectRatio) const;
        OMX_ERRORTYPE getParam(OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE*  aParamColorPrimary) const;
        OMX_ERRORTYPE getParam(OMX_SYMBIAN_VIDEO_PARAM_AVCSEITYPE*  aParamVideoAvcSei) const;
        // Private extension
        OMX_ERRORTYPE getParam(VFM_PARAM_HEADERDATACHANGING_TYPE*   aParamHeaderDataChanging) const;
		//+ code for CR 332873
        OMX_ERRORTYPE getParam(OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE*   aParamCpbBufferSize) const;
		//- code for CR 332873
		OMX_ERRORTYPE getParam(OMX_PARAM_PORTDEFINITIONTYPE* aParamPortDefinition) const;
        OMX_ERRORTYPE getConfig(OMX_VIDEO_CONFIG_NALSIZE*           aConfigVideoNALSize) const;
        OMX_ERRORTYPE getConfig(OMX_VIDEO_CONFIG_BITRATETYPE*       aConfigVideoBitrate) const;
        OMX_ERRORTYPE getConfig(OMX_CONFIG_FRAMERATETYPE*           aConfigVideoFramerate) const;
        OMX_ERRORTYPE getConfig(OMX_CONFIG_INTRAREFRESHVOPTYPE*     aConfigVideoIntraVOPRefresh) const;
        OMX_ERRORTYPE getConfig(OMX_CONFIG_MACROBLOCKERRORMAPTYPE*  aConfigVideoMacroBlockErrorMap) const;
        OMX_ERRORTYPE getConfig(OMX_CONFIG_RECTTYPE*                aConfigCommonInputCrop) const;
        // SHAI extension
        OMX_ERRORTYPE getConfig(OMX_SYMBIAN_VIDEO_PARAM_SEQUENCEHEADERTYPE* aConfigVideoSequenceHeader) const;
		/* + change for CR 343589 to support rotation */
        OMX_ERRORTYPE setConfig(OMX_CONFIG_ROTATIONTYPE* aConfigRotationType);
        OMX_ERRORTYPE getConfig(OMX_CONFIG_ROTATIONTYPE* aConfigRotationType) const;
        /* - change for CR 343589 to support rotation */

        OMX_ERRORTYPE reset();

        void updateParams(OMX_VIDEO_PARAM_AVCTYPE&              aParamVideoAvc);
        void updateParams(OMX_VIDEO_PARAM_QUANTIZATIONTYPE&     aParamVideoQuantization);
        void updateParams(OMX_VIDEO_PARAM_AVCSLICEFMO&          aParamVideoSliceFMO);
        void updateParams(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE&  aParamVideoFastUpdate);
        void updateParams(OMX_VIDEO_PARAM_INTRAREFRESHTYPE&     aParamVideoIntraRefresh);
        void updateParams(OMX_VIDEO_PARAM_BITRATETYPE&          aParamVideoBitrate);

        void updateParams(OMX_VIDEO_CONFIG_NALSIZE&             aConfigVideoNALSize);
        void updateParams(OMX_VIDEO_CONFIG_BITRATETYPE&         aConfigVideoBitrate);
        void updateParams(OMX_CONFIG_FRAMERATETYPE&             aConfigVideoFramerate);
        void updateParams(OMX_CONFIG_INTRAREFRESHVOPTYPE&       aConfigVideoIntraVOPRefresh);
        void updateParams(OMX_CONFIG_MACROBLOCKERRORMAPTYPE&    aConfigVideoMacroBlockErrorMap);

        void updateParams(OMX_DIRTYPE aDir, OMX_VIDEO_PORTDEFINITIONTYPE& aPortDefinitionType);

        /* +Change for 372717 */
        void update_cropping_Params(t_specific_frameinfo frame_info);
        /* -Change for 372717 */

        inline void setSpsPps(t_sps_pps* pSequenceHeader) {mpSequenceHeader = pSequenceHeader;};

		/* + change for CR 343589 */
        inline void setProxyHandle(H264Enc_Proxy *proxyPTR) {mComponent = proxyPTR;}
        /* - change for CR 343589 */

        //+ code for CR 332873
        inline OMX_U32 getDefaultCpbBufferSize(t_uint16 aBrcType, t_uint32 aBitrate) const
        {
            if(aBrcType == 4) // VBR
                return (aBitrate * 3); // 3 seconds delay
            else // CBR
                return (aBitrate >> 1); // 500 milliseconds delay
        }
        //- code for CR 332873

    protected:
#ifdef PACKET_VIDEO_SUPPORT
        virtual OMX_ERRORTYPE getPVCapability(OMX_PTR pt_org);
#endif

    private:
        /// @brief H264 Encoder OpenMax Component this class is related to
        H264Enc_Proxy *mComponent;

        t_frameinfo  mFrameInfo;  // parameters as expected by the encoder.
        t_sps_pps*   mpSequenceHeader;

        /* +Change for 369621 */
        OMX_U32 port0_FrameRate;
        OMX_U32 port1_FrameRate;
        /* -Change for 369621 */

        static const OMX_VIDEO_PARAM_AVCTYPE                    mParamVideoAvcDefault;
        static const OMX_VIDEO_PARAM_QUANTIZATIONTYPE           mParamVideoQuantizationDefault;
        static const OMX_VIDEO_PARAM_AVCSLICEFMO                mParamVideoSliceFMODefault;
        static const OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE        mParamVideoFastUpdateDefault;
        static const OMX_VIDEO_PARAM_INTRAREFRESHTYPE           mParamVideoIntraRefreshDefault;
        static const OMX_VIDEO_PARAM_BITRATETYPE                mParamVideoBitrateDefault;
        static const OMX_VIDEO_PARAM_VBSMCTYPE                  mParamVideoVbsmcDefault;
        static const OMX_VIDEO_PARAM_MOTIONVECTORTYPE           mParamVideoMotionVectorDefault;
        static const OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE         mParamColorPrimaryDefault;
        static const OMX_SYMBIAN_VIDEO_PARAM_SEQUENCEHEADERTYPE mParamVideoSequenceHeaderDefault;
        static const OMX_SYMBIAN_VIDEO_PARAM_AVCSEITYPE         mParamVideoAvcSeiDefault;
        static const OMX_PARAM_PIXELASPECTRATIO                 mParamPixelAspectRatioDefault;
		//+ code for CR 332873
        static const OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE          mParamCpbBufferSizeDefault;
		//- code for CR 332873
        static const OMX_VIDEO_CONFIG_NALSIZE                   mConfigVideoNALSizeDefault;
        static const OMX_VIDEO_CONFIG_BITRATETYPE               mConfigVideoBitrateDefault;
        static const OMX_CONFIG_FRAMERATETYPE                   mConfigVideoFramerateDefault;
        static const OMX_CONFIG_INTRAREFRESHVOPTYPE             mConfigVideoIntraVOPRefreshDefault;
        static const OMX_CONFIG_MACROBLOCKERRORMAPTYPE          mConfigVideoMacroBlockErrorMapDefault;
        static const OMX_CONFIG_RECTTYPE                        mConfigCommonInputCropDefault;
        /* + change for CR 343589 to support rotation */
        static const OMX_CONFIG_ROTATIONTYPE 					mConfigVideoRotationDefault;
        /* - change for CR 343589 to support rotation */
};


#endif



