/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __MPEG4ENC_PARAMANDCONFIG_H
#define __MPEG4ENC_PARAMANDCONFIG_H

#include "VFM_ParamAndConfig.h"
#include "VFM_Component.h"
#define OMX_PARAM_COLORPRIMARY OMX_CONFIG_COLORPRIMARY
#define OMX_VIDEO_CONFIG_SEQUENCEHEADERTYPE OMX_VIDEO_PARAM_SEQUENCEHEADER

typedef enum t_ptr_type
{
	BITRATE_TYPE = 0,
	FRAMERATE_TYPE,
	DYN_BITRATE_TYPE
}t_ptr_type;

#define HEADER_BUFFER_SIZE (0x38)

/// @ingroup MPEG4Encoder
/// @brief This class contains the parameters and configuration of the MPEG4 Encode OpenMax Component
/// It is also used to set and get the configuration, without processing.
///
/// It is derived from VFM_ParamAndConfig.
/// @note The size of the frame (input and output) is stored inside the VFM_Port associated
/// with the MPEG4 Encoder Port (that can be accessed through the attribute mComponent).
/// Port VPB+0 contains the size of the original frame, and port VPB+1 contains the
/// size of the cropped encoded frame, and can be retrived using
/// mComponent->getFrameHeight(VPB+0), mComponent->getFrameWidth(VPB+0),
/// mComponent->getFrameHeight(VPB+1) and mComponent->getFrameWidth(VPB+1)

class MPEG4Enc_ParamAndConfig: public VFM_ParamAndConfig, public TraceObject
{
	friend class MPEG4Enc_Proxy;
	friend class MPEG4Enc_NmfMpc_ProcessingComponent;
	friend class MPEG4Enc_ArmNmf_ProcessingComponent;
	friend class MPEG4Enc_ProcessingComponent;
	friend class MPEG4Enc_Port;

public:
	/// @brief Constructor, that inializes the default configuration
	/// and set the OpenMax Component the configuration is related to
	MPEG4Enc_ParamAndConfig(VFM_Component *component);

private:
	/// @brief Get a structure of type OMX_PARAM_PORTDEFINITIONTYPE. It makes use
	/// of the mComponent attribute.
	/// @param [in,out] pt_org structure of type OMX_PARAM_PORTDEFINITIONTYPE,
	/// that will contain the information to get
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	OMX_ERRORTYPE getIndexParamPortDefinition(OMX_PTR pt_org) const;
	
	OMX_ERRORTYPE setInitialCropParams();

	OMX_ERRORTYPE reset();

	OMX_U32   m_nSvaMcps;

private:
	/// @brief cropping parameter. all 0 if no cropping
	OMX_CONFIG_RECTTYPE m_crop_param;

	//< VFM_VIDEO_PARAM_WRITEVOSHEADER m_vos_header;
	/// @brief Quantization parameter. valid range is [2,31]
	OMX_VIDEO_PARAM_QUANTIZATIONTYPE m_Qp;
	/// @brief Fast update parameters.
	OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE m_fast_update;
	/// @brief Bitrate parameters. Valid range is (0, 12000000] bits per second.
	OMX_VIDEO_PARAM_BITRATETYPE   m_bitrate;
	/// @brief Motion vector parameters.
	OMX_VIDEO_PARAM_MOTIONVECTORTYPE m_motion_vec;
	/// @brief Adaptive/Cyclic Intra Refresh parameters.
	OMX_VIDEO_PARAM_INTRAREFRESHTYPE m_intra_ref;
	/// @brief Error Correction parameters. Enable/Disable Data Partitioning here.
	OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE m_err_corr;
	/// @brief Basic Video Encoder (SP/SH) Parameters parameters.
	OMX_VIDEO_PARAM_MPEG4TYPE m_enc_param;
	/// @brief Extra SH Video Encoder Parameters parameters.
	OMX_VIDEO_PARAM_H263TYPE m_enc_h263_param;
	/// @brief Framerate parameters. To be supplied in Q16 format, valid range (0,30]
	OMX_CONFIG_FRAMERATETYPE m_framerate;
	/// @brief Force Intra parameters. True if next encoded frame is to be forced as Intra otherwise false
	OMX_CONFIG_INTRAREFRESHVOPTYPE m_force_intra;
	/// @brief Error Map parameters.
	OMX_CONFIG_MACROBLOCKERRORMAPTYPE m_mb_errormap;
	/// @brief Deblocking parameters
	OMX_PARAM_DEBLOCKINGTYPE m_deblocking;

	/* +Change start for CR338066 MPEG4Enc DCI */
	/// @brief Enable/disable Short DCI of 28bytes for MPEG4 Encoder
	VFM_PARAM_MPEG4SHORTDCITYPE m_short_dci;
	/* -Change end for CR338066 MPEG4Enc DCI */

	/// @brief Pixel Aspect Ratio parameters.
	OMX_PARAM_PIXELASPECTRATIO m_pixel_aspect_ratio;
	/// @brief Color Primary Info parameters.
	// BIG FIXME: Using the enum instead of the container structure to overcome the video_chipset_apis and shai related compilation error problem
	//>OMX_PARAM_COLORPRIMARY m_color_primary;
	OMX_COLORPRIMARYTYPE m_color_primary;

	/* +Change start for CR343589 Rotation */
	/// @brief Rotation parameters.
	OMX_CONFIG_ROTATIONTYPE m_rotation;
	/* -Change end for CR343589 Rotation */
	
	OMX_ERRORTYPE setIndexParamVideoQuantization(OMX_VIDEO_PARAM_QUANTIZATIONTYPE *p_Qp, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamVideoFastUpdate(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *p_fast_update, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamVideoBitrate(OMX_VIDEO_PARAM_BITRATETYPE *p_bitrate, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamVideoMotionVector(OMX_VIDEO_PARAM_MOTIONVECTORTYPE *p_motion_vec, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamVideoIntraRefresh(OMX_VIDEO_PARAM_INTRAREFRESHTYPE *p_intra_ref , OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamVideoErrorCorrection(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *p_err_corr, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamVideoMpeg4(OMX_VIDEO_PARAM_MPEG4TYPE *p_enc_param, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamVideoH263(OMX_VIDEO_PARAM_H263TYPE *p_enc_h263_param, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamPixelAspectRatio(OMX_PARAM_PIXELASPECTRATIO *p_pixel_aspect_ratio, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamColorPrimary(OMX_PARAM_COLORPRIMARY *p_color_primary, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexParamCommonDeblocking(OMX_PARAM_DEBLOCKINGTYPE *p_deblocking, OMX_BOOL *p_has_changed);
	/* +Change start for CR338066 MPEG4Enc DCI */
	OMX_ERRORTYPE setIndexParamMpeg4ShortDCI(VFM_PARAM_MPEG4SHORTDCITYPE *p_short_dci, OMX_BOOL *p_has_changed);
	/* -Change end for CR338066 MPEG4Enc DCI */
	OMX_ERRORTYPE setIndexParamResourceSvaMcps(OMX_PTR pMcpsType);

	OMX_ERRORTYPE setIndexConfigVideoBitrate(OMX_VIDEO_CONFIG_BITRATETYPE *p_dyn_bitrate, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexConfigVideoFramerate(OMX_CONFIG_FRAMERATETYPE *p_framerate, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexConfigVideoIntraVOPRefresh(OMX_CONFIG_INTRAREFRESHVOPTYPE *p_force_intra, OMX_BOOL *has_changed);
	OMX_ERRORTYPE setIndexConfigVideoMacroBlockErrorMap(OMX_CONFIG_MACROBLOCKERRORMAPTYPE *p_mb_errormap, OMX_BOOL *has_changed);
	/* +Change start for CR343589 Rotation */
	OMX_ERRORTYPE setIndexConfigCommonRotate(OMX_CONFIG_ROTATIONTYPE *p_rotation, OMX_BOOL *p_has_changed);
	/* -Change end for CR343589 Rotation */

	OMX_ERRORTYPE getIndexParamVideoQuantization(OMX_VIDEO_PARAM_QUANTIZATIONTYPE *p_Qp) const;
	OMX_ERRORTYPE getIndexParamVideoFastUpdate(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *p_fast_update) const;
	OMX_ERRORTYPE getIndexParamVideoBitrate(OMX_VIDEO_PARAM_BITRATETYPE *p_bitrate) const;
	OMX_ERRORTYPE getIndexParamVideoMotionVector(OMX_VIDEO_PARAM_MOTIONVECTORTYPE *p_motion_vec) const;
	OMX_ERRORTYPE getIndexParamVideoIntraRefresh(OMX_VIDEO_PARAM_INTRAREFRESHTYPE *p_intra_ref) const;
	OMX_ERRORTYPE getIndexParamVideoErrorCorrection(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *p_err_corr) const;
	OMX_ERRORTYPE getIndexParamVideoMpeg4(OMX_VIDEO_PARAM_MPEG4TYPE *p_enc_param) const;
	OMX_ERRORTYPE getIndexParamVideoH263(OMX_VIDEO_PARAM_H263TYPE *p_enc_h263_param) const;
	OMX_ERRORTYPE getIndexParamPixelAspectRatio(OMX_PARAM_PIXELASPECTRATIO *p_pixel_aspect_ratio) const;
	OMX_ERRORTYPE getIndexParamColorPrimary(OMX_PARAM_COLORPRIMARY *p_color_primary) const;
	OMX_ERRORTYPE getIndexParamCommonDeblocking(OMX_PARAM_DEBLOCKINGTYPE *p_deblocking) const;
	OMX_ERRORTYPE getIndexParamResourceSvaMcps(OMX_PTR pMcpsType) const;

#ifdef PACKET_VIDEO_SUPPORT
	virtual OMX_ERRORTYPE getPVCapability(OMX_PTR pt_org);
#endif

	/* +Change start for CR338066 MPEG4Enc DCI */
	OMX_ERRORTYPE getIndexParamMpeg4ShortDCI(VFM_PARAM_MPEG4SHORTDCITYPE *p_short_dci) const;
	/* -Change end for CR338066 MPEG4Enc DCI */
	OMX_ERRORTYPE getIndexConfigVideoBitrate(OMX_VIDEO_CONFIG_BITRATETYPE *p_dyn_bitrate) const;
	OMX_ERRORTYPE getIndexConfigVideoFramerate(OMX_CONFIG_FRAMERATETYPE *p_framerate) const;
	OMX_ERRORTYPE getIndexConfigVideoIntraVOPRefresh(OMX_CONFIG_INTRAREFRESHVOPTYPE *p_force_intra) const;
	OMX_ERRORTYPE getIndexConfigVideoMacroBlockErrorMap(OMX_CONFIG_MACROBLOCKERRORMAPTYPE *p_mb_errormap) const;
	/* +Change start for CR343589 Rotation */
	OMX_ERRORTYPE getIndexConfigCommonRotate(OMX_CONFIG_ROTATIONTYPE *p_rotation) const;
	/* -Change end for CR343589 Rotation */
	
	OMX_U32 resourceSvaMcps() const;
	void resourceSvaMcps(const OMX_U32& mcps);

	OMX_ERRORTYPE check_profile_level(t_ptr_type ptr_type, OMX_PTR ptr);
};

inline OMX_U32 MPEG4Enc_ParamAndConfig::resourceSvaMcps() const	{ return m_nSvaMcps; }

inline void MPEG4Enc_ParamAndConfig::resourceSvaMcps(const OMX_U32& mcps) { m_nSvaMcps = mcps; }

#endif
