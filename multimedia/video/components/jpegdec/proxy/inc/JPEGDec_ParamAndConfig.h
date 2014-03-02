/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __JPEGDEC_PARAMANDCONFIG_H
#define __JPEGDEC_PARAMANDCONFIG_H

#include "VFM_Component.h"
#include "VFM_ParamAndConfig.h"

/*! Enumeration for Downsampling Factors
*/
typedef enum {
    /*! Downsampling factor = 1
	*/
	DOWNSAMPLING_FACTOR_1 = 1,
    /*! Downsampling factor = 2
	*/
    DOWNSAMPLING_FACTOR_2 = 2,
    /*! Downsampling factor = 4
	*/
    DOWNSAMPLING_FACTOR_4 = 4,
    /*! Downsampling factor = 8
	*/
    DOWNSAMPLING_FACTOR_8 = 8
}JPEGDec_Proxy_Downsampling_Factor;

// forward definition
class JPEGDec_Proxy;


/// @ingroup JPEGDecoder
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

class JPEGDec_ParamAndConfig: public VFM_ParamAndConfig, public TraceObject
{
public:
        /// @brief Constructor, that inializes the default configuration
        /// and set the OpenMax Component the configuration is related to
        JPEGDec_ParamAndConfig(VFM_Component *component);

public:

        /// @brief Set a structure of type OMX_CONFIG_SCALEFACTORTYPE
        /// @param [in] pt_org structure of type OMX_CONFIG_SCALEFACTORTYPE, that contains the
        /// downsampling to be set
        /// @param [out] Downsampling set
        /// @param [out] has_changed true if the value of the downsampling has been modified by
        /// this operation, false otherwise
        /// @return OMX_ErrorNone if no error occured, an error otherwise (OMX_ErrorBadParameter)
        OMX_ERRORTYPE setIndexConfigCommonScale(OMX_PTR pt_org, JPEGDec_Proxy_Downsampling_Factor *downsamplingFactor, OMX_BOOL *has_changed);

        /// @brief Get a structure of type OMX_CONFIG_SCALEFACTORTYPE
        /// @param [in,out] pt_org structure of type OMX_CONFIG_SCALEFACTORTYPE, that will contain the
        /// rotation to get
        /// @param [in] get Downsampling
		/// @return OMX_ErrorNone if no error occured, an error otherwise
        OMX_ERRORTYPE getIndexConfigCommonScale(OMX_PTR pt_org, JPEGDec_Proxy_Downsampling_Factor downsamplingFactor) const;

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

        OMX_ERRORTYPE reset();

		void initializeParamAndConfig();

		void jpegdec_param_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);

        OMX_ERRORTYPE setIndexParamResourceSvaMcps(OMX_PTR pMcpsType);

        OMX_ERRORTYPE getIndexParamResourceSvaMcps(OMX_PTR pMcpsType) const;

        OMX_U32 resourceSvaMcps() const;

        void resourceSvaMcps(const OMX_U32& mcps) ;

private:
		OMX_U32   m_nSvaMcps;


public:

		/// @brief height of the frame
		OMX_U16 frameHeight;

		/// @brief width of the frame
		OMX_U16 frameWidth;

        /// @brief Height of cropped frame. Equals frame height when no cropping is required
		OMX_U32 cropWindowHeight;

		/// @brief Width of cropped frame. Equals frame Width when no cropping is required
		OMX_U32 cropWindowWidth;

		/// @brief horizontal offset when cropping. 0 if no cropping
    	OMX_U32 horizontal_offset;

    	/// @brief vertical offset
        /// @todo before or after the rotation ?
    	OMX_U32 vertical_offset;

        /// @brief Downscaling required of decoded image size. Only 1,2,4,8 downampling factors are supported.
		/// By defualt 1 is selected.
		JPEGDec_Proxy_Downsampling_Factor downsamplingFactor;

        OMX_COLOR_FORMATTYPE outputBufferFormat;
		OMX_U8 isCroppingEnabled;
		OMX_U8 isCroppingInvalid;
		OMX_BOOL configCroppingChanged;

		OMX_U8 isDownsamplingEnabled;
		OMX_BOOL configScalingChanged;
};


#endif
