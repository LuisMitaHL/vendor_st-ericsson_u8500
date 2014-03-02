/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef __NORCOS_OMX_EXTENSIONS_H
#define __NORCOS_OMX_EXTENSIONS_H

typedef enum
{
	Norcos_Decim = 0,
	Norcos_Bilin = 1,
	Norcos_Max   = 0x7FFFFFFF    /* not to be used: force enum as 32-bit */
} NorcosDownSamplingMethod_e;

typedef struct
{
	OMX_S32                    s32_FlatParam;
	OMX_S32                    s32_FlatDetectGrade;
	OMX_BOOL                   b_Downsampling;
	NorcosDownSamplingMethod_e e_DownSamplingMethod;
} NorcosTuningParams_t;

/* Norcos internal tunning parameters */
typedef struct {
	OMX_U32                    nSize;
	OMX_VERSIONTYPE            nVersion;
	OMX_U32                    nPortIndex;
	NorcosTuningParams_t       t_TuningParameters;
} IFM_CONFIG_NORCOS_TUNING;

/* Norcos external control mode */ 
typedef enum
{
   Norcos_OFF  = 0,
   Norcos_ON   = 1,
   Norcos_AUTO = 2
} NorcosControlMode_e;

typedef struct{
	OMX_U32 nSize;
	OMX_VERSIONTYPE     nVersion;
	OMX_U32             nPortIndex;
	NorcosControlMode_e nNorcosMode;
} IFM_CONFIG_NORCOS_CONTROLMODE;

#endif //__NORCOS_OMX_EXTENSIONS_H
