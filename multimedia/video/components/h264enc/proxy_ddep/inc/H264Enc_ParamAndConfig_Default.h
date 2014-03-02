/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264Enc_PARAMANDCONFIG_DEFAULT_H
#define __H264Enc_PARAMANDCONFIG_DEFAULT_H


#define DEFAULT_PARAM_HEADER(param,port)    \
    sizeof(param),                          \
    {{0,0,0,0}},                            \
    port                                    \


const OMX_VIDEO_PARAM_AVCTYPE H264Enc_ParamAndConfig::mParamVideoAvcDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_PARAM_AVCTYPE,1),
    0,                              // nSliceHeaderSpacing
    1,                              // nPFrames
    0,                              // nBFrames
    OMX_TRUE,                       // bUseHadamard
    1,                              // nRefFrames
    0,                              // nRefIdx10ActiveMinus1
    0,                              // nRefIdx11ActiveMinus1
    OMX_FALSE,                      // bEnableUEP
    OMX_FALSE,                      // bEnableFMO
    OMX_FALSE,                      // bEnableASO
    OMX_FALSE,                      // bEnableRS
    OMX_VIDEO_AVCProfileBaseline,   // eProfile
    OMX_VIDEO_AVCLevel31,            // eLevel
    0,                              // nAllowedPictureTypes
    OMX_TRUE,                       // bFrameMBsOnly
    OMX_FALSE,                      // bMBAFF
    OMX_FALSE,                      // bEntropyCodingCABAC
    OMX_FALSE,                      // bWeightedPPrediction
    0,                              // nWeightedBipredicitonMode
    OMX_TRUE,                       // bconstIpred
    OMX_FALSE,                      // bDirect8x8Inference
    OMX_FALSE,                      // bDirectSpatialTemporal
    0,                              // nCabacInitIdc
    OMX_VIDEO_AVCLoopFilterDisable  // eLoopFilterMode
};


const OMX_VIDEO_PARAM_QUANTIZATIONTYPE H264Enc_ParamAndConfig::mParamVideoQuantizationDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_PARAM_QUANTIZATIONTYPE,1),
    25,     // nQpI
    25,     // nQpP
    0       // nQpB
};

const OMX_VIDEO_PARAM_AVCSLICEFMO H264Enc_ParamAndConfig::mParamVideoSliceFMODefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_PARAM_AVCSLICEFMO,1),
    0,                                  // nNumSliceGroups
    0,                                  // nSliceGroupMapType
    OMX_VIDEO_SLICEMODE_AVCDefault      // eSliceMode
};


const OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE H264Enc_ParamAndConfig::mParamVideoFastUpdateDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE,1),
    OMX_FALSE,  // bEnableVFU
    0,          // nFirstGOB
    0,          // nFirstMB
    0           // nNumMBs
};


const OMX_VIDEO_PARAM_INTRAREFRESHTYPE H264Enc_ParamAndConfig::mParamVideoIntraRefreshDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_PARAM_INTRAREFRESHTYPE,1),
    OMX_VIDEO_IntraRefreshBoth, // eRefreshMode;
    0,                          // nAirMBs;
    0,                          // nAirRef;
    0                           // nCirMBs;
};


const OMX_VIDEO_PARAM_BITRATETYPE H264Enc_ParamAndConfig::mParamVideoBitrateDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_PARAM_BITRATETYPE,1),
    OMX_Video_ControlRateDisable,   // eControlRate;
    0                               // nTargetBitrate;
};

const OMX_VIDEO_PARAM_VBSMCTYPE H264Enc_ParamAndConfig::mParamVideoVbsmcDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_PARAM_VBSMCTYPE,1),
    OMX_TRUE,   // OMX_BOOL b16x16;
    OMX_TRUE,   // OMX_BOOL b16x8;
    OMX_TRUE,   // OMX_BOOL b8x16;
    OMX_TRUE,   // OMX_BOOL b8x8;
    OMX_FALSE,  // OMX_BOOL b8x4;
    OMX_FALSE,  // OMX_BOOL b4x8;
    OMX_FALSE   // OMX_BOOL b4x4;
};


// TODO: check values
const OMX_VIDEO_PARAM_MOTIONVECTORTYPE H264Enc_ParamAndConfig::mParamVideoMotionVectorDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_PARAM_MOTIONVECTORTYPE,1),
    OMX_Video_MotionVectorQuarterPel,   // OMX_VIDEO_MOTIONVECTORTYPE eAccuracy;
    OMX_TRUE,                           // OMX_BOOL bUnrestrictedMVs;
    OMX_FALSE,                          // OMX_BOOL bFourMV;
    16,                                 // OMX_S32 sXSearchRange;
    16                                  // OMX_S32 sYSearchRange;
};

/* + change for CR 343589 to support rotation */
const OMX_CONFIG_ROTATIONTYPE H264Enc_ParamAndConfig::mConfigVideoRotationDefault =
{
    DEFAULT_PARAM_HEADER(OMX_CONFIG_ROTATIONTYPE,1),
    0     // nRotation
};
/* - change for CR 343589 to support rotation */

const OMX_VIDEO_CONFIG_NALSIZE H264Enc_ParamAndConfig::mConfigVideoNALSizeDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_CONFIG_NALSIZE,1),
    2000,   // nNaluBytes
};


const OMX_VIDEO_CONFIG_BITRATETYPE H264Enc_ParamAndConfig::mConfigVideoBitrateDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_CONFIG_BITRATETYPE,1),
    64000   // nEncodeBitrate;
};


const OMX_CONFIG_FRAMERATETYPE H264Enc_ParamAndConfig::mConfigVideoFramerateDefault =
{
    DEFAULT_PARAM_HEADER(OMX_CONFIG_FRAMERATETYPE,1),
    15<<16  // xEncodeFramerate; /* Q16 format */
};


// OMX_IndexConfigVideoIntraVOPRefresh
const OMX_CONFIG_INTRAREFRESHVOPTYPE H264Enc_ParamAndConfig::mConfigVideoIntraVOPRefreshDefault =
{
    DEFAULT_PARAM_HEADER(OMX_CONFIG_INTRAREFRESHVOPTYPE,1),
    OMX_FALSE   // IntraRefreshVOP
};


const OMX_CONFIG_MACROBLOCKERRORMAPTYPE H264Enc_ParamAndConfig::mConfigVideoMacroBlockErrorMapDefault =
{
    DEFAULT_PARAM_HEADER(OMX_CONFIG_MACROBLOCKERRORMAPTYPE,1),
    0,      // nErrMapSize
    {0}  // ErrMap[1]
};

const OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE H264Enc_ParamAndConfig::mParamColorPrimaryDefault =
{
    DEFAULT_PARAM_HEADER(OMX_SYMBIAN_PARAM_COLORPRIMARYTYPE,1),
    OMX_SYMBIAN_ColorMax // eColorPrimary
};

const OMX_SYMBIAN_VIDEO_PARAM_SEQUENCEHEADERTYPE H264Enc_ParamAndConfig::mParamVideoSequenceHeaderDefault =
{
    DEFAULT_PARAM_HEADER(OMX_SYMBIAN_VIDEO_PARAM_SEQUENCEHEADERTYPE,1),
    0,      // nSequenceHeaderSize
    {0}  // SequenceHeader[1]
};

const OMX_SYMBIAN_VIDEO_PARAM_AVCSEITYPE H264Enc_ParamAndConfig::mParamVideoAvcSeiDefault =
{
    DEFAULT_PARAM_HEADER(OMX_SYMBIAN_VIDEO_PARAM_AVCSEITYPE,1),
    OMX_FALSE,                              // bEnableSEI
    OMX_SYMBIAN_VIDEO_AvcSeiBufferinPeriod  // eSEI
};


const OMX_PARAM_PIXELASPECTRATIO H264Enc_ParamAndConfig::mParamPixelAspectRatioDefault =
{
    DEFAULT_PARAM_HEADER(OMX_PARAM_PIXELASPECTRATIO,1),
    1,  // nHorizontal
    1   // nVertical
};

const OMX_CONFIG_RECTTYPE H264Enc_ParamAndConfig::mConfigCommonInputCropDefault =
{
    DEFAULT_PARAM_HEADER(OMX_CONFIG_RECTTYPE,0),
    0,  // OMX_S32 nLeft;
    0,  // OMX_S32 nTop;
    0,  // OMX_U32 nWidth;
    0   // OMX_U32 nHeight;
};

//+ code for CR 332873
// extension: "OMX.ST.VFM.CpbBufferSize"
const OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE H264Enc_ParamAndConfig::mParamCpbBufferSizeDefault =
{
    DEFAULT_PARAM_HEADER(OMX_VIDEO_PARAM_CPBBUFFERSIZETYPE,1),
    64000,          // default value of bitrate = nCpbBufferSize
};
//- code for CR 332873
#endif  // __H264Enc_PARAMANDCONFIG_DEFAULT_H



