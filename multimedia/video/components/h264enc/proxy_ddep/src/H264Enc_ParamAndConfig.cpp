/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ H264Enc_Proxy
#include "osi_trace.h"

#include "H264Enc_ParamAndConfig.h"
#include "H264Enc_Proxy.h"

#include "H264Enc_ParamAndConfig_Default.h"



// save and restore nSize, nVersion and nPortIndex from IL client
#define SET_DEFAULT_PARAM(current_param,default_param)      \
do  {                                                       \
        OMX_U32 size            = current_param->nSize;     \
        OMX_VERSIONTYPE omx_ver = current_param->nVersion;  \
        OMX_U32 index           = current_param->nPortIndex;\
        *current_param = default_param;                     \
        current_param->nSize        = size;                 \
        current_param->nVersion     = omx_ver;              \
        current_param->nPortIndex   = index;                \
    }                                                       \
while(0)

#ifndef H264ENC_TRACE_GROUP_PROXY
    #include "OstTraceDefinitions_ste.h"
    #include "OpenSystemTrace_ste.h"
    #ifdef OST_TRACE_COMPILER_IN_USE
        #include "video_components_h264enc_proxy_ddep_src_H264Enc_ParamAndConfigTraces.h"
    #endif
#endif


H264Enc_ParamAndConfig::H264Enc_ParamAndConfig(VFM_Component *pComponent) : VFM_ParamAndConfig(pComponent)
{
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::H264Enc_ParamAndConfig");

    setDefault();
    mpSequenceHeader = NULL;
    /* +Change for 369621 */
    port0_FrameRate = 0;
    port1_FrameRate = 0;
    /* -Change for 369621 */

    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::H264Enc_ParamAndConfig");
}


//  TODO: add size and version
void H264Enc_ParamAndConfig::setDefault()
{
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setDefault");

    // initialization of attributes of VFM_ParamAndConfig
    setProfileLevel(mParamVideoAvcDefault.eProfile, mParamVideoAvcDefault.eLevel);

    mFrameInfo.common_frameinfo.pic_width                 = 176;
    mFrameInfo.common_frameinfo.pic_height                = 144;
    mFrameInfo.common_frameinfo.eProfile                  = mParamVideoAvcDefault.eProfile;
    mFrameInfo.common_frameinfo.eLevel                    = mParamVideoAvcDefault.eLevel;
    mFrameInfo.specific_frameinfo.EncoderLevel            = mParamVideoAvcDefault.eLevel;
    mFrameInfo.specific_frameinfo.sfw                     = 176;
    mFrameInfo.specific_frameinfo.sfh                     = 144;
    mFrameInfo.specific_frameinfo.sho                     = 0;
    mFrameInfo.specific_frameinfo.svo                     = 0;
    mFrameInfo.specific_frameinfo.n                       = 0;
    mFrameInfo.specific_frameinfo.algo                    = 0;
    mFrameInfo.specific_frameinfo.OutFileMode             = 0;
    mFrameInfo.specific_frameinfo.IDRIntraEnable          = 1;
    mFrameInfo.specific_frameinfo.IntraPeriod             = mParamVideoAvcDefault.nPFrames + 1;
    mFrameInfo.specific_frameinfo.SliceMode               = mParamVideoSliceFMODefault.eSliceMode;
    mFrameInfo.specific_frameinfo.MbSliceSize             = mParamVideoAvcDefault.nSliceHeaderSpacing;
    mFrameInfo.specific_frameinfo.ByteSliceSize           = mConfigVideoNALSizeDefault.nNaluBytes;
    mFrameInfo.specific_frameinfo.UseConstrainedIntraPred = 0;
    mFrameInfo.specific_frameinfo.QPISlice                = mParamVideoQuantizationDefault.nQpI;
    mFrameInfo.specific_frameinfo.QPPSlice                = mParamVideoQuantizationDefault.nQpP;
    mFrameInfo.specific_frameinfo.PicOrderCntType         = 2;
    mFrameInfo.specific_frameinfo.BrcType                 = 0;  // 0 (constant QP), 2(CBR), or 4 (VBR).
    mFrameInfo.specific_frameinfo.VBRConfig               = 7;
    mFrameInfo.specific_frameinfo.Bitrate                 = mConfigVideoBitrateDefault.nEncodeBitrate;
    mFrameInfo.specific_frameinfo.FrameRate               = mConfigVideoFramerateDefault.xEncodeFramerate;
    mFrameInfo.specific_frameinfo.disableH4D              = mParamVideoAvcDefault.eLoopFilterMode;
    mFrameInfo.specific_frameinfo.SearchWindow            = 32;
    mFrameInfo.specific_frameinfo.EncoderComplexity       = 3;
    mFrameInfo.specific_frameinfo.LowComplexity           = 0;
    mFrameInfo.specific_frameinfo.ProfileIDC              = 66; // 66 => Baseline - 77 => Main - 100 => High
    mFrameInfo.specific_frameinfo.TransformMode           = 0;  // 0: only 4x4 transform - 1: 8x8 transform allowed for I, forced for P
    mFrameInfo.specific_frameinfo.IntraRefreshType        = 0;  // 0: no intra-refresh - 1: Air only - 2: Cir only - 3: intra-refresh both (AIR/CIR)
    mFrameInfo.specific_frameinfo.AirMbNum                = 0;
    mFrameInfo.specific_frameinfo.CirMbNum                = 0;
    mFrameInfo.specific_frameinfo.ForceIntraPredMode      = 0;  // for debug purpose - to be set to 0
    memset(mFrameInfo.specific_frameinfo.SliceLossFirstMb, 0, 8);
    memset(mFrameInfo.specific_frameinfo.SliceLossMbNum,   0, 8);
    mFrameInfo.specific_frameinfo.HrdSendMessages         = 1;
    mFrameInfo.specific_frameinfo.HeaderDataChanging      = 0; //change for ER 439847, not required to set this
    mFrameInfo.specific_frameinfo.video_full_range_flag   = 1;
    mFrameInfo.specific_frameinfo.colour_primaries        = 2;
    mFrameInfo.specific_frameinfo.frame_cropping_flag     = 0; // used for non multiple of 16 format
    mFrameInfo.specific_frameinfo.frame_cropping_rect_left_offset   = 0;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_right_offset  = 0;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_top_offset    = 0;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_bottom_offset = 0;
    // + code change for CR 361667
    mFrameInfo.specific_frameinfo.ForceIntra              = 0;
    // - code change for CR 361667
//+ code for step 2 of CR 332873
    mFrameInfo.specific_frameinfo.CBR_simplified_algo     = 0;
    mFrameInfo.specific_frameinfo.CBR_clipped             = 0;
    mFrameInfo.specific_frameinfo.CBR_clipped_min_QP      = 0;
    mFrameInfo.specific_frameinfo.CBR_clipped_max_QP      = 0;
    mFrameInfo.specific_frameinfo.CpbBufferSize           = 0;
//- code for step 2 of CR 332873
/* + Change for CR 343589 */
	mFrameInfo.specific_frameinfo.nRotation = 0;
/* - Change for CR 343589 */
    OstTraceInt0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setDefault");
}


// FIXME: valid only on output port (VPB+1)
OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_VIDEO_PARAM_AVCTYPE* aParamVideoAvc)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_AVCTYPE:setParam");

    // Do all param checks here
    // Unsigned int parameters are necessarily >=0. Concerned lines are commented to avoid warnings

    //RETURN_XXX_IF_WRONG(aParamVideoAvc->nSliceHeaderSpacing         >= 0,                               OMX_ErrorBadParameter);
    //RETURN_XXX_IF_WRONG(aParamVideoAvc->nPFrames                    >= 0,                               OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->nBFrames                    == 0,                               OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bUseHadamard                == OMX_TRUE,                        OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->nRefFrames                  == 1,                               OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->nRefIdx10ActiveMinus1       == 0,                               OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->nRefIdx11ActiveMinus1       == 0,                               OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bEnableUEP                  == OMX_FALSE,                       OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bEnableFMO                  == OMX_FALSE,                       OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bEnableASO                  == OMX_FALSE,                       OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bEnableRS                   == OMX_FALSE,                       OMX_ErrorBadParameter);
    // TODO: use GetSupportedProfileLevel() to check if given profile/level are supported
    RETURN_XXX_IF_WRONG( (aParamVideoAvc->eProfile == OMX_VIDEO_AVCProfileBaseline ) ||
                         (aParamVideoAvc->eProfile == OMX_VIDEO_AVCProfileMain ) ||
                         (aParamVideoAvc->eProfile == OMX_VIDEO_AVCProfileHigh ),                       OMX_ErrorBadParameter);
    //RETURN_XXX_IF_WRONG(aParamVideoAvc->eLevel                      == OMX_VIDEO_AVCLevel1,             OMX_ErrorBadParameter);
    // TODO: CHECK RETURN_XXX_IF_WRONG(aParamVideoAvc->nAllowedPictureTypes        == 0,                               OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bFrameMBsOnly               == OMX_TRUE,                        OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bMBAFF                      == OMX_FALSE,                       OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bEntropyCodingCABAC         == OMX_FALSE,                       OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bWeightedPPrediction        == OMX_FALSE,                       OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->nWeightedBipredicitonMode   == 0,                               OMX_ErrorBadParameter);
    // TODO: CHECK RETURN_XXX_IF_WRONG(aParamVideoAvc->bconstIpred                 == OMX_TRUE,                        OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bDirect8x8Inference         == OMX_FALSE,                       OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->bDirectSpatialTemporal      == OMX_FALSE,                       OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoAvc->nCabacInitIdc               == 0,                               OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG( (aParamVideoAvc->eLoopFilterMode == OMX_VIDEO_AVCLoopFilterEnable)  ||
                         (aParamVideoAvc->eLoopFilterMode == OMX_VIDEO_AVCLoopFilterDisable) ||
                         (aParamVideoAvc->eLoopFilterMode == OMX_VIDEO_AVCLoopFilterDisableSliceBoundary), OMX_ErrorBadParameter);

    if ( ((OMX_VIDEO_AVCPROFILETYPE)mFrameInfo.common_frameinfo.eProfile != aParamVideoAvc->eProfile) ||
         ((OMX_VIDEO_AVCLEVELTYPE)mFrameInfo.common_frameinfo.eLevel     != aParamVideoAvc->eLevel)  )
    {
        setProfileLevel(aParamVideoAvc->eProfile, aParamVideoAvc->eLevel);
    }

    // TODO: Send event to inform the client that parameter VideoSliceFMO has changed
    //if ( (aParamVideoAvc->nSliceHeaderSpacing > 0) &&
    //     (mFrameInfo.specific_frameinfo.SliceMode != 1) )  // 1 correspond to OMX_VIDEO_SLICEMODE_AVCMBSlice
    //{
    //    mFrameInfo.specific_frameinfo.SliceMode = 1; // OMX_VIDEO_SLICEMODE_AVCMBSlice
    //}

    updateParams(*aParamVideoAvc);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_AVCTYPE:setParam");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_VIDEO_PARAM_AVCTYPE* aParamVideoAvc) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_AVCTYPE:getParam");

    DBC_ASSERT(mFrameInfo.common_frameinfo.eProfile <= OMX_VIDEO_AVCProfileHigh444);
    DBC_ASSERT(mFrameInfo.common_frameinfo.eLevel   <= OMX_VIDEO_AVCLevel51);
    DBC_ASSERT(mFrameInfo.specific_frameinfo.disableH4D <= 2)

    SET_DEFAULT_PARAM(aParamVideoAvc,mParamVideoAvcDefault);

    aParamVideoAvc->eProfile = static_cast<OMX_VIDEO_AVCPROFILETYPE>(mFrameInfo.common_frameinfo.eProfile);
    aParamVideoAvc->eLevel   = static_cast<OMX_VIDEO_AVCLEVELTYPE>(mFrameInfo.common_frameinfo.eLevel);

    aParamVideoAvc->nPFrames            = mFrameInfo.specific_frameinfo.IntraPeriod -1;
    aParamVideoAvc->nSliceHeaderSpacing = mFrameInfo.specific_frameinfo.MbSliceSize;

    switch(mFrameInfo.specific_frameinfo.disableH4D)
    {
        case 0:
            aParamVideoAvc->eLoopFilterMode = OMX_VIDEO_AVCLoopFilterEnable;
            break;
        case 1:
            aParamVideoAvc->eLoopFilterMode = OMX_VIDEO_AVCLoopFilterDisable;
            break;
        case 2:
            aParamVideoAvc->eLoopFilterMode = OMX_VIDEO_AVCLoopFilterDisableSliceBoundary;
            break;
    }


    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_AVCTYPE:getParam");
    return OMX_ErrorNone;
}



OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE* aParamCpbBufferSize)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE:setParam");

    // todo: check on BufferSize
    //RETURN_XXX_IF_WRONG(aParamCpbBufferSize->nCpbBufferSize >= 0, OMX_ErrorBadParameter);

    mFrameInfo.specific_frameinfo.CpbBufferSize = aParamCpbBufferSize->nCpbBufferSize;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE:setParam");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE* aParamCpbBufferSize) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE:getParam");

    SET_DEFAULT_PARAM(aParamCpbBufferSize,mParamCpbBufferSizeDefault);

    if(mFrameInfo.specific_frameinfo.CpbBufferSize == 0)
    {
        aParamCpbBufferSize->nCpbBufferSize = getDefaultCpbBufferSize(mFrameInfo.specific_frameinfo.BrcType, mFrameInfo.specific_frameinfo.Bitrate);
    }
    else
    {
        aParamCpbBufferSize->nCpbBufferSize = mFrameInfo.specific_frameinfo.CpbBufferSize;
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE:getParam");
    return OMX_ErrorNone;
}
//- code for CR 332873

OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_VIDEO_PARAM_QUANTIZATIONTYPE* aParamVideoQuantization)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_QUANTIZATIONTYPE:setParam");

    // Do all param checks here
    // Unsigned int parameters are necessarily >=0. Concerned lines are commented to avoid warnings

    //RETURN_XXX_IF_WRONG(aParamVideoQuantization->nQpI >= 0,   OMX_ErrorBadParameter); // TODO: nQpI > 6
    RETURN_XXX_IF_WRONG(aParamVideoQuantization->nQpI <= 51,  OMX_ErrorBadParameter);

    //RETURN_XXX_IF_WRONG(aParamVideoQuantization->nQpP >= 0,   OMX_ErrorBadParameter); // TODO: nQpP > 6
    RETURN_XXX_IF_WRONG(aParamVideoQuantization->nQpP <= 51,  OMX_ErrorBadParameter);

    // TODO B frames not supported RETURN_XXX_IF_WRONG(aParamVideoQuantization->nQpB == 0,   OMX_ErrorBadParameter);

    updateParams(*aParamVideoQuantization);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_QUANTIZATIONTYPE:setParam");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_VIDEO_PARAM_QUANTIZATIONTYPE* aParamVideoQuantization) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_QUANTIZATIONTYPE:getParam");

    SET_DEFAULT_PARAM(aParamVideoQuantization,mParamVideoQuantizationDefault);

    aParamVideoQuantization->nQpI = mFrameInfo.specific_frameinfo.QPISlice;
    aParamVideoQuantization->nQpP = mFrameInfo.specific_frameinfo.QPPSlice;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_QUANTIZATIONTYPE:getParam");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_VIDEO_PARAM_AVCSLICEFMO* aParamVideoSliceFMO)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_AVCSLICEFMO:setParam");

    RETURN_XXX_IF_WRONG(aParamVideoSliceFMO->nNumSliceGroups    == 0,   OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoSliceFMO->nSliceGroupMapType == 0,   OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG( (aParamVideoSliceFMO->eSliceMode == OMX_VIDEO_SLICEMODE_AVCDefault)  ||
                         (aParamVideoSliceFMO->eSliceMode == OMX_VIDEO_SLICEMODE_AVCMBSlice)  ||
                         (aParamVideoSliceFMO->eSliceMode == OMX_VIDEO_SLICEMODE_AVCByteSlice)  , OMX_ErrorBadParameter);

    updateParams(*aParamVideoSliceFMO);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_AVCSLICEFMO:setParam");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_VIDEO_PARAM_AVCSLICEFMO* aParamVideoSliceFMO) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_AVCSLICEFMO:getParam");

    SET_DEFAULT_PARAM(aParamVideoSliceFMO,mParamVideoSliceFMODefault);

    switch(mFrameInfo.specific_frameinfo.SliceMode)
    {
        case 0:
            aParamVideoSliceFMO->eSliceMode = OMX_VIDEO_SLICEMODE_AVCDefault;
            break;
        case 1:
            aParamVideoSliceFMO->eSliceMode = OMX_VIDEO_SLICEMODE_AVCMBSlice;
            break;
        case 2:
            aParamVideoSliceFMO->eSliceMode = OMX_VIDEO_SLICEMODE_AVCByteSlice;
            break;
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_PARAM_AVCSLICEFMO:getParam");
    return OMX_ErrorNone;
}




OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE* aParamVideoFastUpdate)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setParam");

/*
    if(bEnableVFU == OMX_FALSE)
    {
        return OMX_ErrorNone;
    }

    // check last refreshed MB is not outside the frame
    int LastRefreshedMB = nFirstGOB * (WIDTH / 16) + nFirstMB + nNumMBs;
    int LastMBInFrame = (WIDTH / 16) * (HEIGHT / 16);

    RETURN_XXX_IF_WRONG(LastRefreshedMB <= LastMBInFrame, OMX_ErrorBadParameter)
*/

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setParam");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE* aParamVideoFastUpdate) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getParam");
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getParam");
    return OMX_ErrorNotImplemented;
}






OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_VIDEO_PARAM_INTRAREFRESHTYPE* aParamVideoIntraRefresh)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setParam");

    RETURN_XXX_IF_WRONG(((aParamVideoIntraRefresh->eRefreshMode == OMX_VIDEO_IntraRefreshBoth)||
                         (aParamVideoIntraRefresh->eRefreshMode == OMX_VIDEO_IntraRefreshCyclic)||
                         (aParamVideoIntraRefresh->eRefreshMode == OMX_VIDEO_IntraRefreshAdaptive)), OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG(aParamVideoIntraRefresh->nAirRef == 0,OMX_ErrorBadParameter);
    //RETURN_XXX_IF_WRONG(aParamVideoIntraRefresh->nCirMBs == 0,OMX_ErrorBadParameter);

    updateParams(*aParamVideoIntraRefresh);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setParam");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_VIDEO_PARAM_INTRAREFRESHTYPE* aParamVideoIntraRefresh) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getParam");

    SET_DEFAULT_PARAM(aParamVideoIntraRefresh,mParamVideoIntraRefreshDefault);

    if (mFrameInfo.specific_frameinfo.IntraRefreshType == 0)
    {
        aParamVideoIntraRefresh->eRefreshMode = OMX_VIDEO_IntraRefreshMax; // TODO: there is no OMX_VIDEO_IntraRefreshNone ????
        aParamVideoIntraRefresh->nAirMBs      = 0; //
    }
    else
    {
        aParamVideoIntraRefresh->eRefreshMode = OMX_VIDEO_IntraRefreshBoth; // TODO: there is no OMX_VIDEO_IntraRefreshNone ????
        aParamVideoIntraRefresh->nAirMBs      = mFrameInfo.specific_frameinfo.AirMbNum; //
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getParam");
    return OMX_ErrorNone;
}



OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_VIDEO_PARAM_BITRATETYPE* aParamVideoBitrate)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setParam");
    // TODO: check what values are really supported
    RETURN_XXX_IF_WRONG(aParamVideoBitrate->eControlRate == OMX_Video_ControlRateDisable ||
                        aParamVideoBitrate->eControlRate == OMX_Video_ControlRateVariable ||
                        aParamVideoBitrate->eControlRate == OMX_Video_ControlRateConstant ||
                        aParamVideoBitrate->eControlRate == OMX_Video_ControlRateVariableSkipFrames ||
                        aParamVideoBitrate->eControlRate == OMX_Video_ControlRateConstantSkipFrames, OMX_ErrorBadParameter );

    // TODO: limit is 20 MBits or 25 ?
    // TODO: if setting is > max, return BadParam or fix value to the max ???
    // h264 encoder does not support bitrate > 20 Mbits/s
    //RETURN_XXX_IF_WRONG(aParamVideoBitrate->nTargetBitrate <= 20 * 1024 * 1024, OMX_ErrorBadParameter);

    updateParams(*aParamVideoBitrate);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setParam");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_VIDEO_PARAM_BITRATETYPE* aParamVideoBitrate) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getParam");

    SET_DEFAULT_PARAM(aParamVideoBitrate,mParamVideoBitrateDefault);

    // todo: what about skip frames enum values
    switch(mFrameInfo.specific_frameinfo.BrcType)
    {
        case 0:
            aParamVideoBitrate->eControlRate = OMX_Video_ControlRateDisable;
            break;
        case 2:
            aParamVideoBitrate->eControlRate = OMX_Video_ControlRateConstant;
            break;
        case 4:
            aParamVideoBitrate->eControlRate = OMX_Video_ControlRateVariable;
            break;
        default: // only values 0 - 2 - 4 are possible
            return OMX_ErrorBadParameter;
        }

    aParamVideoBitrate->nTargetBitrate = mFrameInfo.specific_frameinfo.Bitrate;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getParam");
    return OMX_ErrorNone;
}




OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_VIDEO_PARAM_VBSMCTYPE* aParamVideoVbsmc)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setParam");

    OMX_U32 mask =  ((OMX_U32)aParamVideoVbsmc->b16x16)     |
                    ((OMX_U32)aParamVideoVbsmc->b16x8 << 1) |
                    ((OMX_U32)aParamVideoVbsmc->b8x16 << 2) |
                    ((OMX_U32)aParamVideoVbsmc->b8x8  << 3) |
                    ((OMX_U32)aParamVideoVbsmc->b8x4  << 4) |
                    ((OMX_U32)aParamVideoVbsmc->b4x8  << 5) |
                    ((OMX_U32)aParamVideoVbsmc->b4x4  << 6);

    RETURN_XXX_IF_WRONG( mask == 0x1 || // b16x16
                         mask == 0xF,   // b16x16, b16x8, b8x16, b8x8
                         OMX_ErrorBadParameter );

    // todo, map here with encoder complexity / low complexity
    if (mask == 0x1)
    {
        mFrameInfo.specific_frameinfo.EncoderComplexity = 1;
    }
    else if (mask == 0xF)
    {
        mFrameInfo.specific_frameinfo.EncoderComplexity = 3;
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setParam");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_VIDEO_PARAM_VBSMCTYPE* aParamVideoVbsmc) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getParam");

    SET_DEFAULT_PARAM(aParamVideoVbsmc,mParamVideoVbsmcDefault);

    switch (mFrameInfo.specific_frameinfo.EncoderComplexity)
    {
        case 1:
            aParamVideoVbsmc->b16x16 = OMX_TRUE;
            aParamVideoVbsmc->b16x16 = OMX_FALSE;
            aParamVideoVbsmc->b8x16  = OMX_FALSE;
            aParamVideoVbsmc->b8x8   = OMX_FALSE;
            aParamVideoVbsmc->b8x4   = OMX_FALSE;
            aParamVideoVbsmc->b4x8   = OMX_FALSE;
            aParamVideoVbsmc->b4x4   = OMX_FALSE;
            break;
        case 3:
            aParamVideoVbsmc->b16x16 = OMX_TRUE;
            aParamVideoVbsmc->b16x16 = OMX_TRUE;
            aParamVideoVbsmc->b8x16  = OMX_TRUE;
            aParamVideoVbsmc->b8x8   = OMX_TRUE;
            aParamVideoVbsmc->b8x4   = OMX_FALSE;
            aParamVideoVbsmc->b4x8   = OMX_FALSE;
            aParamVideoVbsmc->b4x4   = OMX_FALSE;
            break;
        default:
            DBC_ASSERT(0);
            break;
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getParam");
    return OMX_ErrorNone;
}





OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_VIDEO_PARAM_MOTIONVECTORTYPE* aParamVideoMotionVector)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setParam");

    //updateParams(*aParamVideoMotionVector);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setParam");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_VIDEO_PARAM_MOTIONVECTORTYPE* aParamVideoMotionVector) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getParam");

    SET_DEFAULT_PARAM(aParamVideoMotionVector,mParamVideoMotionVectorDefault);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getParam");
    return OMX_ErrorNone;
}



OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE* aParamColorPrimary) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getParam");

    SET_DEFAULT_PARAM(aParamColorPrimary,mParamColorPrimaryDefault);

    switch (mFrameInfo.specific_frameinfo.video_full_range_flag)
    {
        case 0: // limited range
            switch (mFrameInfo.specific_frameinfo.colour_primaries)
            {
                case 1:
                    aParamColorPrimary->eColorPrimary = OMX_SYMBIAN_ColorPrimaryBT709LimitedRange;
                    break;
                case 2:
                    //aParamColorPrimary->eColorPrimary = OMX_SYMBIAN_ColorPrimaryUndefinedRange;
                    aParamColorPrimary->eColorPrimary = OMX_SYMBIAN_ColorMax;
                    break;
                case 5:
                    aParamColorPrimary->eColorPrimary = OMX_SYMBIAN_ColorPrimaryBT601LimitedRange;
                    break;
                default:
                    DBC_ASSERT(0);
            }
            break;
        case 1: // full range
            switch (mFrameInfo.specific_frameinfo.colour_primaries)
            {
                case 1:
                    aParamColorPrimary->eColorPrimary = OMX_SYMBIAN_ColorPrimaryBT709FullRange;
                    break;
                case 2:
                    //aParamColorPrimary->eColorPrimary = OMX_SYMBIAN_ColorPrimaryUndefinedRange;
                    aParamColorPrimary->eColorPrimary = OMX_SYMBIAN_ColorMax;
                    break;
                case 5:
                    aParamColorPrimary->eColorPrimary = OMX_SYMBIAN_ColorPrimaryBT601FullRange;
                    break;
                default:
                    DBC_ASSERT(0);
            }
            break;
        default:
            DBC_ASSERT(0);
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getParam");

    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE* aParamColorPrimary)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setParam");

    RETURN_XXX_IF_WRONG(aParamColorPrimary->eColorPrimary == OMX_SYMBIAN_ColorPrimaryBT601LimitedRange  ||
                        aParamColorPrimary->eColorPrimary == OMX_SYMBIAN_ColorPrimaryBT601FullRange     ||
                        aParamColorPrimary->eColorPrimary == OMX_SYMBIAN_ColorPrimaryBT709LimitedRange  ||
                        aParamColorPrimary->eColorPrimary == OMX_SYMBIAN_ColorPrimaryBT709FullRange     ||
                        aParamColorPrimary->eColorPrimary == OMX_SYMBIAN_ColorMax, OMX_ErrorBadParameter );

    switch(aParamColorPrimary->eColorPrimary)
    {
        case OMX_SYMBIAN_ColorPrimaryBT601LimitedRange:
            mFrameInfo.specific_frameinfo.video_full_range_flag=0;
            mFrameInfo.specific_frameinfo.colour_primaries=5;
            break;
        case OMX_SYMBIAN_ColorPrimaryBT601FullRange:
            mFrameInfo.specific_frameinfo.video_full_range_flag=1;
            mFrameInfo.specific_frameinfo.colour_primaries=5;
            break;
        case OMX_SYMBIAN_ColorPrimaryBT709LimitedRange:
            mFrameInfo.specific_frameinfo.video_full_range_flag=0;
            mFrameInfo.specific_frameinfo.colour_primaries=1;
            break;
        case OMX_SYMBIAN_ColorPrimaryBT709FullRange:
            mFrameInfo.specific_frameinfo.video_full_range_flag=1;
            mFrameInfo.specific_frameinfo.colour_primaries=1;
            break;
        case OMX_SYMBIAN_ColorMax: // todo: should be OMX_SYMBIAN_ColorPrimaryUndefinedRange
            mFrameInfo.specific_frameinfo.video_full_range_flag=1;
            mFrameInfo.specific_frameinfo.colour_primaries=2;
            break;
        default:
            DBC_ASSERT(0);
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setParam");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_SYMBIAN_VIDEO_PARAM_AVCSEITYPE* aParamVideoAvcSei) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getParam");

    SET_DEFAULT_PARAM(aParamVideoAvcSei,mParamVideoAvcSeiDefault);

    switch(mFrameInfo.specific_frameinfo.HrdSendMessages)
    {
        case 0:
            aParamVideoAvcSei->bEnableSEI = OMX_FALSE;
            aParamVideoAvcSei->eSEI = OMX_SYMBIAN_VIDEO_AvcSeiBufferinPeriod;
            break;
        case 2:
            aParamVideoAvcSei->bEnableSEI = OMX_TRUE;
            aParamVideoAvcSei->eSEI = OMX_SYMBIAN_VIDEO_AvcSeiBufferinPeriod;
            break;
        default:
            aParamVideoAvcSei->bEnableSEI = OMX_FALSE;
            aParamVideoAvcSei->eSEI = OMX_SYMBIAN_VIDEO_AvcSeiBufferinPeriod;
            break;
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getParam");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_SYMBIAN_VIDEO_PARAM_AVCSEITYPE* aParamVideoAvcSei)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setParam");

    // todo: update when sei could be activated independently
    if(aParamVideoAvcSei->bEnableSEI == OMX_TRUE)
    {
        mFrameInfo.specific_frameinfo.HrdSendMessages = 2;
    }
    else //bEnableSEI == OMX_FALSE
    {
        mFrameInfo.specific_frameinfo.HrdSendMessages = 0;
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setParam");
    return OMX_ErrorNone;
}



OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(VFM_PARAM_HEADERDATACHANGING_TYPE* aParamHeaderDataChanging)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setParam");

    mFrameInfo.specific_frameinfo.HeaderDataChanging = aParamHeaderDataChanging->bHeaderDataChanging;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setParam");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(VFM_PARAM_HEADERDATACHANGING_TYPE* aParamHeaderDataChanging) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getParam");

    aParamHeaderDataChanging->bHeaderDataChanging = static_cast<OMX_BOOL>(mFrameInfo.specific_frameinfo.HeaderDataChanging);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getParam");
    return OMX_ErrorNone;
}



OMX_ERRORTYPE H264Enc_ParamAndConfig::setParam(OMX_PARAM_PIXELASPECTRATIO* aParamPixelAspectRatio)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setParam");

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setParam");
    return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_PARAM_PIXELASPECTRATIO* aParamPixelAspectRatio) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_PARAM_PIXELASPECTRATIO:getParam");

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_PARAM_PIXELASPECTRATIO:getParam");


    //SET_DEFAULT_PARAM(aParamPixelAspectRatio,mParamPixelAspectRatioDefault);


    return OMX_ErrorBadParameter;
}

/* + change for CR 343589 to support rotation */

OMX_ERRORTYPE H264Enc_ParamAndConfig::setConfig(OMX_CONFIG_ROTATIONTYPE* aConfigRotationType)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_CONFIG_ROTATIONTYPE setParam");
	RETURN_XXX_IF_WRONG( (aConfigRotationType->nRotation == 0 || aConfigRotationType->nRotation == 90 || aConfigRotationType->nRotation == 180 || aConfigRotationType->nRotation == 270 ||
						  aConfigRotationType->nRotation == -90 || aConfigRotationType->nRotation == -180 || aConfigRotationType->nRotation == -270), OMX_ErrorBadParameter);
	mFrameInfo.specific_frameinfo.nRotation = aConfigRotationType->nRotation;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_CONFIG_ROTATIONTYPE setParam");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getConfig(OMX_CONFIG_ROTATIONTYPE* aConfigRotationType) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_CONFIG_ROTATIONTYPE:getParam");
	SET_DEFAULT_PARAM(aConfigRotationType,mConfigVideoRotationDefault);
    aConfigRotationType->nRotation = mFrameInfo.specific_frameinfo.nRotation;
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_CONFIG_ROTATIONTYPE:getParam");

    return OMX_ErrorNone;
}

/* - change for CR 343589 to support rotation */

OMX_ERRORTYPE H264Enc_ParamAndConfig::setConfig(OMX_CONFIG_RECTTYPE* aConfigCommonInputCrop)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setConfig");
    RETURN_XXX_IF_WRONG( (aConfigCommonInputCrop->nLeft + aConfigCommonInputCrop->nWidth) <= mFrameInfo.common_frameinfo.pic_width, OMX_ErrorBadParameter);
    RETURN_XXX_IF_WRONG( (aConfigCommonInputCrop->nTop + aConfigCommonInputCrop->nHeight) <= mFrameInfo.common_frameinfo.pic_height, OMX_ErrorBadParameter);

    // refer to "Recommendation ITU-T H.264" document for more details
    const int CropUnitX = 1;
    const int CropUnitY = 2;

    mFrameInfo.specific_frameinfo.frame_cropping_flag = 1;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_left_offset     = aConfigCommonInputCrop->nLeft / CropUnitX;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_right_offset    = (mFrameInfo.common_frameinfo.pic_width - aConfigCommonInputCrop->nWidth - aConfigCommonInputCrop->nLeft) / CropUnitX;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_top_offset      = aConfigCommonInputCrop->nTop / CropUnitY;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_bottom_offset   = (mFrameInfo.common_frameinfo.pic_height - aConfigCommonInputCrop->nHeight - aConfigCommonInputCrop->nTop) / CropUnitY;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setConfig");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getConfig(OMX_CONFIG_RECTTYPE* aConfigCommonInputCrop) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getConfig");

    SET_DEFAULT_PARAM(aConfigCommonInputCrop,mConfigCommonInputCropDefault);

    // refer to "Recommendation ITU-T H.264" document for more details
    const int CropUnitX = 1;
    const int CropUnitY = 2;

    if(mFrameInfo.specific_frameinfo.frame_cropping_flag)
    {
        aConfigCommonInputCrop->nLeft   = CropUnitX * mFrameInfo.specific_frameinfo.frame_cropping_rect_left_offset;
        aConfigCommonInputCrop->nTop    = CropUnitY * mFrameInfo.specific_frameinfo.frame_cropping_rect_top_offset;
        aConfigCommonInputCrop->nWidth  = mFrameInfo.common_frameinfo.pic_width - CropUnitX *
                                          (mFrameInfo.specific_frameinfo.frame_cropping_rect_left_offset +
                                           mFrameInfo.specific_frameinfo.frame_cropping_rect_right_offset);
        aConfigCommonInputCrop->nHeight = mFrameInfo.common_frameinfo.pic_height - CropUnitY *
                                          (mFrameInfo.specific_frameinfo.frame_cropping_rect_top_offset +
                                           mFrameInfo.specific_frameinfo.frame_cropping_rect_bottom_offset);
    }
    else
    {
        aConfigCommonInputCrop->nLeft   = 0;
        aConfigCommonInputCrop->nTop    = 0;
        aConfigCommonInputCrop->nWidth  = mFrameInfo.common_frameinfo.pic_width;
        aConfigCommonInputCrop->nHeight = mFrameInfo.common_frameinfo.pic_height;
    }


	OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getConfig");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getParam(OMX_PARAM_PORTDEFINITIONTYPE* aParamPortDefinition) const
{
    IN0("");

    if (aParamPortDefinition->nPortIndex == 1)
    {
          if(mFrameInfo.specific_frameinfo.frame_cropping_flag)
          {
			  // refer to "Recommendation ITU-T H.264" document for more details
			  const int CropUnitX = 1;
			  const int CropUnitY = 2;

               aParamPortDefinition->format.video.nFrameWidth  = aParamPortDefinition->format.video.nFrameWidth - CropUnitX *
                                          (mFrameInfo.specific_frameinfo.frame_cropping_rect_left_offset +
                                           mFrameInfo.specific_frameinfo.frame_cropping_rect_right_offset);
               aParamPortDefinition->format.video.nFrameHeight = aParamPortDefinition->format.video.nFrameHeight - CropUnitY *
                                          (mFrameInfo.specific_frameinfo.frame_cropping_rect_top_offset +
                                           mFrameInfo.specific_frameinfo.frame_cropping_rect_bottom_offset);
    }

	}

    OUT0("");
    return OMX_ErrorNone;
}



OMX_ERRORTYPE H264Enc_ParamAndConfig::setConfig(OMX_VIDEO_CONFIG_NALSIZE* aConfigVideoNALSize)
{
	OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_CONFIG_NALSIZE:setConfig");

    //RETURN_XXX_IF_WRONG(aConfigVideoNALSize->nNaluBytes >=  0,   OMX_ErrorBadParameter);
    /*
    if (aConfigVideoNALSize->nNaluBytes ==  0)
    {
        // slice_mode must be 0 (OMX_VIDEO_SLICEMODE_AVCDefault) or 1 (OMX_VIDEO_SLICEMODE_AVCMBSlice)
        RETURN_XXX_IF_WRONG( (mFrameInfo.specific_frameinfo.SliceMode == 0) ||
                             (mFrameInfo.specific_frameinfo.SliceMode == 1),   OMX_ErrorBadParameter);
    }
    else
    {
        // slice_mode must be 2 (OMX_VIDEO_SLICEMODE_AVCByteSlice)
        RETURN_XXX_IF_WRONG( mFrameInfo.specific_frameinfo.SliceMode == 2,   OMX_ErrorBadParameter);
    }
    */

    updateParams(*aConfigVideoNALSize);

	OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_CONFIG_NALSIZE:setConfig");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::getConfig(OMX_VIDEO_CONFIG_NALSIZE* aConfigVideoNALSize) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_CONFIG_NALSIZE:getConfig");

    SET_DEFAULT_PARAM(aConfigVideoNALSize,mConfigVideoNALSizeDefault);

    aConfigVideoNALSize->nNaluBytes = mFrameInfo.specific_frameinfo.ByteSliceSize;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_CONFIG_NALSIZE:getConfig");
    return OMX_ErrorNone;
}





OMX_ERRORTYPE H264Enc_ParamAndConfig::setConfig(OMX_VIDEO_CONFIG_BITRATETYPE* aConfigVideoBitrate)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::OMX_VIDEO_CONFIG_BITRATETYPE:setConfig");

    // todo: check that encode bitrate is not too far away from previous value
    // todo: forward value to port (port setting change event)

    mFrameInfo.specific_frameinfo.Bitrate = aConfigVideoBitrate->nEncodeBitrate;
	//+ code for CR 332873
    mFrameInfo.specific_frameinfo.CpbBufferSize = mFrameInfo.specific_frameinfo.Bitrate; //by default CPB size is equal to bitrate
	//- code for CR 332873

	OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::OMX_VIDEO_CONFIG_BITRATETYPE:setConfig");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getConfig(OMX_VIDEO_CONFIG_BITRATETYPE* aConfigVideoBitrate) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getConfig");

    SET_DEFAULT_PARAM(aConfigVideoBitrate,mConfigVideoBitrateDefault);

    aConfigVideoBitrate->nEncodeBitrate = mFrameInfo.specific_frameinfo.Bitrate;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getConfig");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::setConfig(OMX_CONFIG_FRAMERATETYPE* aConfigVideoFramerate)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setConfig");

    // todo: forward value to port (port setting change event)

    RETURN_XXX_IF_WRONG(aConfigVideoFramerate->xEncodeFramerate >=  (1<<16),   OMX_ErrorBadParameter);

    mFrameInfo.specific_frameinfo.FrameRate = aConfigVideoFramerate->xEncodeFramerate;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setConfig");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getConfig(OMX_CONFIG_FRAMERATETYPE* aConfigVideoFramerate) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getConfig");

    SET_DEFAULT_PARAM(aConfigVideoFramerate,mConfigVideoFramerateDefault);

    aConfigVideoFramerate->xEncodeFramerate = mFrameInfo.specific_frameinfo.FrameRate;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getConfig");
    return OMX_ErrorNone;
}

// + code change for CR 361667

OMX_ERRORTYPE H264Enc_ParamAndConfig::setConfig(OMX_CONFIG_INTRAREFRESHVOPTYPE* aConfigVideoIntraVOPRefresh)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setConfig");

    mFrameInfo.specific_frameinfo.ForceIntra = aConfigVideoIntraVOPRefresh->IntraRefreshVOP;
    OstTraceFiltInst1(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setConfig : ForcteIntra : %d",mFrameInfo.specific_frameinfo.ForceIntra);
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setConfig");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getConfig(OMX_CONFIG_INTRAREFRESHVOPTYPE* aConfigVideoIntraVOPRefresh) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getConfig");

    aConfigVideoIntraVOPRefresh->IntraRefreshVOP = (OMX_BOOL)mFrameInfo.specific_frameinfo.ForceIntra;
    OstTraceFiltInst1(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getConfig : ForcteIntra : %d",mFrameInfo.specific_frameinfo.ForceIntra);

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getConfig");
    return OMX_ErrorNone;
}

// - code change for CR 361667

OMX_ERRORTYPE H264Enc_ParamAndConfig::setConfig(OMX_CONFIG_MACROBLOCKERRORMAPTYPE* aConfigVideoMacroBlockErrorMap)
{
    int width_mb  = (mFrameInfo.common_frameinfo.pic_width  % 16) ? (mFrameInfo.common_frameinfo.pic_width/16)+1
                                                                  : mFrameInfo.common_frameinfo.pic_width/16;

    int height_mb = (mFrameInfo.common_frameinfo.pic_height % 16) ? (mFrameInfo.common_frameinfo.pic_height/16)+1
                                                                  : mFrameInfo.common_frameinfo.pic_height/16;

    int nb_mb = width_mb * height_mb;

    int size_map = (nb_mb % 8) ? nb_mb/8 + 1 : nb_mb/8; // table of char. 1 bit per MB

    // Warning if client sends a table too small => possible memory corruption
    RETURN_XXX_IF_WRONG( aConfigVideoMacroBlockErrorMap->nErrMapSize >= size_map * sizeof(char), OMX_ErrorBadParameter);

    // Copy the error map, and reset it
    getErrorMap()->lockMutex();
    getErrorMap()->reset_with_size(size_map);
    memcpy(getErrorMap()->mErrorMap, aConfigVideoMacroBlockErrorMap->ErrMap, size_map);
    getErrorMap()->releaseMutex();

//    mFrameInfo.specific_frameinfo.has_errormap_changed = true; // common_frameinfo ???

    return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getConfig(OMX_CONFIG_MACROBLOCKERRORMAPTYPE* aConfigVideoMacroBlockErrorMap) const
{
    return OMX_ErrorNotImplemented;
}


OMX_ERRORTYPE H264Enc_ParamAndConfig::setConfig(OMX_SYMBIAN_VIDEO_PARAM_SEQUENCEHEADERTYPE* aConfigVideoSequenceHeader)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::setConfig");

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::setConfig");
    return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE H264Enc_ParamAndConfig::getConfig(OMX_SYMBIAN_VIDEO_PARAM_SEQUENCEHEADERTYPE* aParamVideoSequenceHeader) const
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getConfig");

    //SET_DEFAULT_PARAM(aParamVideoSequenceHeader,mParamVideoSequenceHeaderDefault);
    OMX_ERRORTYPE error=OMX_ErrorBadParameter;

    //lock mutex
    if(mpSequenceHeader == NULL)
    {
        error=OMX_ErrorBadParameter; // todo: what kind of error in such case ?
    }
    else if(aParamVideoSequenceHeader->nSequenceHeaderSize == 0)
    {
        aParamVideoSequenceHeader->nSequenceHeaderSize = mpSequenceHeader->size;
        error=OMX_ErrorNone;
    }
    else if(aParamVideoSequenceHeader->nSequenceHeaderSize < mpSequenceHeader->size)
    {
        error=OMX_ErrorUnsupportedSetting;
    }
    else
    {
        aParamVideoSequenceHeader->nSequenceHeaderSize = mpSequenceHeader->size;
        memcpy(aParamVideoSequenceHeader->SequenceHeader, mpSequenceHeader->data, mpSequenceHeader->size);
        error=OMX_ErrorNone;
    }
    //release mutex

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getConfig");
    return error;
}



void H264Enc_ParamAndConfig::updateParams(OMX_VIDEO_PARAM_AVCTYPE& aParamVideoAvc)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::updateParams");

    mFrameInfo.common_frameinfo.eProfile = aParamVideoAvc.eProfile;
    mFrameInfo.common_frameinfo.eLevel   = aParamVideoAvc.eLevel;

    switch(aParamVideoAvc.eProfile)
    {
        case OMX_VIDEO_AVCProfileBaseline:
            mFrameInfo.specific_frameinfo.ProfileIDC = 66;
            break;
        case OMX_VIDEO_AVCProfileMain:
            mFrameInfo.specific_frameinfo.ProfileIDC = 77;
            break;
        case OMX_VIDEO_AVCProfileHigh:
            mFrameInfo.specific_frameinfo.ProfileIDC = 100;
            break;
        default:
            DBC_ASSERT(0);
    }

    mFrameInfo.specific_frameinfo.IntraPeriod   = aParamVideoAvc.nPFrames + 1;
    mFrameInfo.specific_frameinfo.MbSliceSize   = aParamVideoAvc.nSliceHeaderSpacing;

    switch(aParamVideoAvc.eLoopFilterMode)
    {
        case OMX_VIDEO_AVCLoopFilterEnable:
            mFrameInfo.specific_frameinfo.disableH4D = 0;
            break;
        case OMX_VIDEO_AVCLoopFilterDisable:
            mFrameInfo.specific_frameinfo.disableH4D = 1;
            break;
        case OMX_VIDEO_AVCLoopFilterDisableSliceBoundary:
            mFrameInfo.specific_frameinfo.disableH4D = 2;
            break;
        default:
            DBC_ASSERT(0);
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::updateParams");
}


void H264Enc_ParamAndConfig::updateParams(OMX_VIDEO_PARAM_QUANTIZATIONTYPE& aParamVideoQuantization)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::updateParams");

    mFrameInfo.specific_frameinfo.QPISlice                  = aParamVideoQuantization.nQpI;
    mFrameInfo.specific_frameinfo.QPPSlice                  = aParamVideoQuantization.nQpP;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::updateParams");
}


void H264Enc_ParamAndConfig::updateParams(OMX_DIRTYPE aDir, OMX_VIDEO_PORTDEFINITIONTYPE& aPortDefinitionType)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::updateParams");

    if (aDir == OMX_DirInput)
    {
        mFrameInfo.common_frameinfo.pic_width   = aPortDefinitionType.nFrameWidth;
        mFrameInfo.common_frameinfo.pic_height  = aPortDefinitionType.nFrameHeight;
        mFrameInfo.specific_frameinfo.sfw       = aPortDefinitionType.nFrameWidth;
        mFrameInfo.specific_frameinfo.sfh       = aPortDefinitionType.nFrameHeight;
        mFrameInfo.specific_frameinfo.sho       = 0;
        mFrameInfo.specific_frameinfo.svo       = 0;
        /* +Change for 369621 */
        port0_FrameRate = aPortDefinitionType.xFramerate;
        /* -Change for 369621 */
    }

    if (aDir == OMX_DirOutput)
    {
        mFrameInfo.specific_frameinfo.Bitrate   = aPortDefinitionType.nBitrate;
        /* +Change for 369621 */
        port1_FrameRate = aPortDefinitionType.xFramerate;
        /* -Change for 369621 */
    }

    /* +Change for 369621 */
    if ((port0_FrameRate != (15<<16)) && (port0_FrameRate))
    {
		mFrameInfo.specific_frameinfo.FrameRate = port0_FrameRate;
		OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] FrameRate set from input port");
	}
    else if ((port1_FrameRate != (15<<16)) && (port1_FrameRate))
	{
		mFrameInfo.specific_frameinfo.FrameRate = port1_FrameRate;
		OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] FrameRate set from output port");
	}
	/* -Change for 369621 */

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::updateParams");
}


void H264Enc_ParamAndConfig::updateParams(OMX_VIDEO_PARAM_AVCSLICEFMO& aParamVideoSliceFMO)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::updateParams");

    switch(aParamVideoSliceFMO.eSliceMode)
    {
        case OMX_VIDEO_SLICEMODE_AVCDefault:
            mFrameInfo.specific_frameinfo.SliceMode = 0;
            break;
        case OMX_VIDEO_SLICEMODE_AVCMBSlice:
            mFrameInfo.specific_frameinfo.SliceMode = 1;
            break;
        case OMX_VIDEO_SLICEMODE_AVCByteSlice:
            mFrameInfo.specific_frameinfo.SliceMode = 2;
            break;
        default:
            DBC_ASSERT(0);
    }

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::updateParams");
}



void H264Enc_ParamAndConfig::updateParams(OMX_VIDEO_PARAM_INTRAREFRESHTYPE& aParamVideoIntraRefresh)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::updateParams");

    if((aParamVideoIntraRefresh.nCirMBs)&&(aParamVideoIntraRefresh.nAirMBs))
    {
        mFrameInfo.specific_frameinfo.IntraRefreshType = 3;// Both AIR and CIR
        mFrameInfo.specific_frameinfo.AirMbNum         = aParamVideoIntraRefresh.nAirMBs;
        mFrameInfo.specific_frameinfo.CirMbNum         = aParamVideoIntraRefresh.nCirMBs;
    }
    else if(aParamVideoIntraRefresh.nCirMBs)
    {
        mFrameInfo.specific_frameinfo.IntraRefreshType = 2;// Only CIR
        mFrameInfo.specific_frameinfo.AirMbNum         = 0;
        mFrameInfo.specific_frameinfo.CirMbNum         = aParamVideoIntraRefresh.nCirMBs;
    }
    else if(aParamVideoIntraRefresh.nAirMBs)
    {
        mFrameInfo.specific_frameinfo.IntraRefreshType = 1;// Only AIR
        mFrameInfo.specific_frameinfo.AirMbNum         = aParamVideoIntraRefresh.nAirMBs;
        mFrameInfo.specific_frameinfo.CirMbNum         = 0;
    }
    else
    {
        mFrameInfo.specific_frameinfo.IntraRefreshType = 0;// Neither CIR nor AIR
        mFrameInfo.specific_frameinfo.AirMbNum         = 0;
        mFrameInfo.specific_frameinfo.CirMbNum         = 0;
    }
    OstTraceFiltInst3(H264ENC_TRACE_GROUP_PROXY, "[PROXY] : H264Enc_ParamAndConfig::updateParams : IntraRefreshType : %d, AirMbNum : %d, CirMbNum : %d",mFrameInfo.specific_frameinfo.IntraRefreshType,mFrameInfo.specific_frameinfo.AirMbNum,mFrameInfo.specific_frameinfo.CirMbNum);
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::updateParams");
}


void H264Enc_ParamAndConfig::updateParams(OMX_VIDEO_PARAM_BITRATETYPE& aParamVideoBitrate)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::updateParams");

    switch(aParamVideoBitrate.eControlRate)
    {
        case OMX_Video_ControlRateDisable:
            mFrameInfo.specific_frameinfo.BrcType = 0;
            break;
        case OMX_Video_ControlRateVariable:
            mFrameInfo.specific_frameinfo.BrcType = 4;
            break;
        case OMX_Video_ControlRateConstant:
            mFrameInfo.specific_frameinfo.BrcType = 2;
            break;
        case OMX_Video_ControlRateVariableSkipFrames:
            mFrameInfo.specific_frameinfo.BrcType = 4;
            break;
        case OMX_Video_ControlRateConstantSkipFrames:
            mFrameInfo.specific_frameinfo.BrcType = 2;
            break;
        default:
            DBC_ASSERT(0);
    }

    mFrameInfo.specific_frameinfo.Bitrate = aParamVideoBitrate.nTargetBitrate;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::updateParams");
}


void H264Enc_ParamAndConfig::updateParams(OMX_VIDEO_CONFIG_NALSIZE& aConfigVideoNALSize)
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::updateParams");

    mFrameInfo.specific_frameinfo.ByteSliceSize   = aConfigVideoNALSize.nNaluBytes;

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::updateParams");
}

/* +Change for 372717 */
void H264Enc_ParamAndConfig::update_cropping_Params(t_specific_frameinfo frame_info)
{
    mFrameInfo.specific_frameinfo.frame_cropping_flag                 =     frame_info.frame_cropping_flag;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_left_offset     =     frame_info.frame_cropping_rect_left_offset;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_right_offset    =     frame_info.frame_cropping_rect_right_offset;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_top_offset      =     frame_info.frame_cropping_rect_top_offset;
    mFrameInfo.specific_frameinfo.frame_cropping_rect_bottom_offset   =     frame_info.frame_cropping_rect_bottom_offset;
}
/* -Change for 372717 */

t_frameinfo H264Enc_ParamAndConfig::getParams()
{
    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] > H264Enc_ParamAndConfig::getParams");

    OstTraceFiltInst0(H264ENC_TRACE_GROUP_PROXY, "[PROXY] < H264Enc_ParamAndConfig::getParams");
    return mFrameInfo;
}


// TODO when should this method be called ?
OMX_ERRORTYPE H264Enc_ParamAndConfig::reset()
{
    //setDefault();
    return OMX_ErrorNone;
}



#ifdef PACKET_VIDEO_SUPPORT
// cf document openmax_call_sequences.pdf
// as well as omx_core_integration_guide.pdf
OMX_ERRORTYPE H264Enc_ParamAndConfig::getPVCapability(OMX_PTR pt_org)
{
    PV_OMXComponentCapabilityFlagsType *pt = (PV_OMXComponentCapabilityFlagsType *)pt_org;
    if (pt == NULL) {
        return OMX_ErrorBadParameter;
    }

    // iIsOMXComponentMultiThreaded=OMX_TRUE as the OMX component can run in a different thread
    // from the thread in which PV framework is running
    pt->iIsOMXComponentMultiThreaded = OMX_TRUE;

    // iOMXComponentSupportsExternalOutputBufferAlloc=OMX_FALSE as we mandates to allocate the output
    // buffers as we require HW buffers internally
    pt->iOMXComponentSupportsExternalOutputBufferAlloc = OMX_FALSE;

    // iOMXComponentSupportsExternalInputBufferAlloc=OMX_TRUE as the buffer can provide from
    // the camera component. Note: buffers can't come from an other source as camera.
    pt->iOMXComponentSupportsExternalInputBufferAlloc = OMX_TRUE;

    // iOMXComponentSupportsMovableInputBuffers=OMX_TRUE
    // this is mandatory for the optimized data path selection at opencore level.
    // if we get this flag as false a buffer copy will be done from camera output to encoder input
    pt->iOMXComponentSupportsMovableInputBuffers = OMX_TRUE;

    // iOMXComponentSupportsPartialFrames=OMX_FALSE as input buffers should contain complete frames
    pt->iOMXComponentSupportsPartialFrames = OMX_FALSE;

    // iOMXComponentUsesNALStartCode=OMX_TRUE
    pt->iOMXComponentUsesNALStartCode = OMX_TRUE;

    // iOMXComponentCanHandleIncompleteFrames=OMX_FALSE. The input frames have to be complete YUV frames.
    pt->iOMXComponentCanHandleIncompleteFrames = OMX_FALSE;

    // iOMXComponentUsesFullAVCFrames=OMX_TRUE as we only supports frame mode, and not nal mode
    pt->iOMXComponentUsesFullAVCFrames = OMX_TRUE;

    return OMX_ErrorNone;
}
#endif


