/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __MPEG4ENC_ARM_MPC_BRC_H
#define __MPEG4ENC_ARM_MPC_BRC_H

#ifdef __MPEG4ENC_ARM_NMF //for ARM-NMF

	#undef METH
	#define METH(x) mpeg4enc_arm_nmf_brc::x
	
	#define COMP_BRC(x) mpeg4enc_arm_nmf_brc::x

#else //for MPC build
	
	#define COMP_BRC(x) x
	
#endif // for __MPEG4ENC_ARM_NMF


#define FIXME
#ifdef FIXME
	#define TRUE 1
	#define FALSE 0
#endif

#define I_TYPE 0
#define P_TYPE 1

#define ONE_KB        (1024)
#define ONE_MB        (ONE_KB * ONE_KB)


#define BRC_CONST_QP 0
#define BRC_FRAME_BASED 1
#define BRC_CBR 2
#define BRC_VBR 3

// Define the clockSlot value for short header in 90Khz value
#define MP4_SH_CLOCK_SLOT           (3003)

// Define the rounding value for temporal reference computation
#define MP4_SH_ROUND_VALUE          (1500)

/*
 * Define BRCMIN / BRCMAX macro
 */ 
#define BRCMIN(a,b)     (((a)<(b))?a:b)
#define BRCMAX(a,b)     (((a)>(b))?a:b)

/*
 * Define value brc type
 */
#define SVA_BRC_QP_BUFFERING_NONE                       0
#define SVA_BRC_FRAME_BASE                              1
#define SVA_BRC_CBR                                     2
#define SVA_BRC_VBR                                     3
#define SVA_BRC_QP_CONSTANT_VBV_ANNEX_G                 4
#define SVA_BRC_QP_CONSTANT_HRD                         6

#define TS_VECTOR_SIZE 6

//Common enums/structs

typedef enum
{
	SVA_SPATIAL_QUALITY_NONE,
	SVA_SPATIAL_QUALITY_LOW,
	SVA_SPATIAL_QUALITY_MEDIUM,
	SVA_SPATIAL_QUALITY_HIGH
} t_sva_brc_spatial_quality;

/*
 * Define data save on an EOT
 */
typedef struct
{
	t_uint32 bitstreamSizeInBits;
	t_sint32 bufferFullness;
	t_uint32 skipPrev;
	t_uint32 skipCurrent;
} t_sva_brc_eot_data;

/*
 * Define eot fifo to have a similar behaviour as in ref model
 */
typedef struct
{
	t_uint32 ptrWrite;
	t_sva_brc_eot_data eotData[2];
} t_sva_brc_eot_fifo;


/*
 * Define output of brc. These data will be use to fill param_in structures
 */
typedef struct
{
	/*define directly in param in*/
	t_uint16 pictureCodingType;
	t_uint16 quant;
	t_uint16 brcType;
	t_uint32 brcFrameTarget;
	t_uint32 brcTargetMinPred;
	t_uint32 brcTargetMaxPred;
	t_uint32 skipCount;
	t_uint32 bitRate;
	t_uint16 frameRate;
	t_sint32 deltaTarget;
	t_uint16 minQp;
	t_uint16 maxQp;
	t_uint16 vopTimeIncrementResolution;
	t_uint16 fixedVopTimeIncrement;
	t_uint32 smax;
	t_uint16 minBaseQuality;
	t_uint16 minFrameRate;
	t_uint32 maxBuffLevel;
	t_uint32 tsSeconds;
	t_uint32 tsModulo;
	t_uint16 firstISkippedFlag;
	t_sint16 initTsModuloOld;
	/*data need by algo part for header writing*/
	t_uint32 vbvBufferSizeIn16384BitsUnit;
	t_sint32 bufferSizeForVbv;
} t_sva_brc_out;	//< FIXME

typedef struct
{
	/* hcl cbr variable */
	t_uint32 pictureCounter;
	t_sva_timestamp_value prevPts;
	t_uint32 skipCount[2]; /*need to handle already program subtask without know prev was skipped*/
	t_uint32 pictureCodingType[2]; /*need to handle already program subtask without know prev was skipped*/

	t_uint32 brcTargetMinPred[2]; /*need to handle already program subtask without know prev was skipped*/
	t_uint32 ptsCor; /*need to handle pts correction when first pictures skipped*/
	t_uint32 skipPrevCount;	/*need to handle pts correction when first pictures skipped*/

	t_sva_brc_out saveBrcOut;
	/* reference model variable */
	t_uint16 frameRate;	/*compute at init. avoid duplicate calculation*/
	t_uint16 vopTimeIncrementResolution; /*compute at init. avoid duplicate calculation*/
	t_uint16 fixedVopTimeIncrement;	/*compute at init. avoid duplicate calculation*/
	t_uint32 sMax; /*could be remove : always pBrcParam->swisBuffer*/
	t_uint32 picTarget;	/*compute at init. avoid duplicate calculation. Dyn param in HCL ?*/
	t_uint32 targetBuffLevel; /*compute at init. avoid duplicate calculation*/
	t_uint32 buffer; /* need to keep state*/
	t_uint32 prevBuffer; /*could be remove : could be local in postPic*/
	t_uint16 bufferMod;	/* need to keep state*/
	t_uint32 bufferDepletion; /* need to keep state*/
	t_sint16 prevVopTimeIncrement; /* need to keep state*/
	t_uint16 oldModuloTimeBase;	/* need to keep state*/
	t_uint32 maxBufferLevel; /* could be remove*/
	t_uint32 deltaTicks; /* need to keep state*/
	t_uint32 intraPeriod; /*compute at init. avoid duplicate calculation*/
	t_uint16 prevStrategicSkip;	/* indicate if picture n-1 has been strategic skip*/
	t_sint16 initTsModuloOld; /*init ts value. Negative value.*/

	/* variable need to handle dynamic command*/
	t_uint16 nextFrameRate;

	/* GT: Dynamic Bitrate 05/05/2006 */
	t_uint32 bitRateDelayed;

	t_uint32 deltaTimeStamp;

} t_sva_brc_qpConstant_state;

typedef struct
{
	/* hcl cbr variable */
	t_uint32 pictureCounter;
	t_sva_timestamp_value prevPts;
	t_uint32 skipCount[2]; /*need to handle already program subtask without know prev was skipped*/
	t_uint32 pictureCodingType[2]; /*need to handle already program subtask without know prev was skipped*/

	t_uint32 brcTargetMinPred[2]; /*need to handle already program subtask without know prev was skipped*/
	t_uint32 govFlag;
	t_uint32 prevPictureCodingType;
	/* stuff to handle partly optimal time stamp (main part)*/
	t_uint32 fakeFlag;
	t_uint32 ptsDiff;

	t_sva_brc_out saveBrcOut;
	/* reference model variable */
	t_uint16 frameRate;	/*compute at init. avoid duplicate calculation*/
	t_uint16 vopTimeIncrementResolution; /*compute at init. avoid duplicate calculation*/
	t_uint16 fixedVopTimeIncrement;	/*compute at init. avoid duplicate calculation*/
	t_uint32 sMax; /*could be remove : always pBrcParam->swissBuffer*/
	t_uint32 picTarget;	/*compute at init. avoid duplicate calculation. Dyn param in HCL ?*/
	t_uint32 targetBuffLevel; /*compute at init. avoid duplicate calculation*/
	t_uint32 buffer; /* need to keep state*/
	t_uint32 bufferFakeTs; /* need when parly optimal time stamp management active */
	t_uint32 prevBuffer; /*could be remove : could be local in postPic*/
	t_uint16 bufferMod;	/* need to keep state*/
	t_uint32 bufferDepletion; /* need to keep state*/
	t_sint16 prevVopTimeIncrement; /* need to keep state*/
	t_uint16 oldModuloTimeBase;	/* need to keep state*/
	t_uint32 maxBufferLevel; /* could be remove*/
	t_uint32 deltaTicks; /* need to keep state*/
	t_uint32 intraPeriod; /*compute at init. avoid duplicate calculation*/
	t_uint16 prevStrategicSkip;	/* indicate if picture n-1 has been strategic skip*/
	t_sint16 initTsModuloOld; 

	/* variable need to handle dynamic command*/
	t_uint16 nextFrameRate;

	/* GT: Dynamic Bitrate 05/05/2006 */
	t_uint32 bitRateDelayed;

	t_uint32 deltaTimeStamp;

} t_sva_brc_cbr_state;

typedef struct
{
	/* hcl vbr variable */
	t_uint32 pictureCounter;
	t_sva_timestamp_value prevPts;
	t_uint32 skipCount[2]; /*need to handle already program subtask without know prev was skipped*/
	t_uint32 pictureCodingType[2]; /*need to handle already program subtask without know prev was skipped*/
	t_uint32 brcTargetMinPred[2]; /*need to handle already program subtask without know prev was skipped*/
	t_uint32 ptsCor; /*need to handle pts correction when first pictures skipped*/
	t_uint32 skipPrevCount;	/*need to handle pts correction when first pictures skipped*/

	t_sva_brc_out saveBrcOut;

	/* reference model variable */
	t_uint16 frameRate;	/*compute at init. avoid duplicate calculation*/
	t_uint16 vopTimeIncrementResolution; /*compute at init. avoid duplicate calculation*/
	t_uint16 fixedVopTimeIncrement;	/*compute at init. avoid duplicate calculation*/
	t_uint32 sMax; /*could be remove : always pBrcParam->swissBuffer*/
	t_uint16 minBaseQuality; /*compute at init. avoid duplicate calculation. Dyn param in HCL ?*/
	t_uint16 minFrameRate; /*compute at init. avoid duplicate calculation*/
	t_uint32 picTarget;	/*compute at init. avoid duplicate calculation. Dyn param in HCL ?*/
	t_uint32 targetBuffLevel; /*compute at init. avoid duplicate calculation*/
	t_uint32 buffer; /* need to keep state*/
	t_uint32 prevBuffer; /*could be remove : could be local in postPic*/
	t_uint16 bufferMod;	/* need to keep state*/
	t_uint32 bufferDepletion; /* need to keep state*/
	t_sint16 prevVopTimeIncrement; /* need to keep state*/
	t_uint16 oldModuloTimeBase;	/* need to keep state*/
	t_uint32 maxBufferLevel; /* could be remove*/
	t_uint32 deltaTicks; /* need to keep state*/
	t_uint32 intraPeriod; /*compute at init. avoid duplicate calculation*/
	t_sint16 initTsModuloOld; /*init ts value. Negative value.*/

	/* variable need to handle dynamic command*/
	t_uint16 nextFrameRate;

	/* GT: Dynamic Bitrate 05/05/2006 */
	t_uint32 bitRateDelayed;

	t_uint32 deltaTimeStamp;

} t_sva_brc_vbr_state;

/*
 * Define input of brc. These data will be use to finish a picture brc update
 */
typedef struct
{
	/*bitstream size info*/
	t_uint32 bitstreamSize;
	t_uint32 stuffingBits;
	/*skipping info*/
	t_uint16 brcSkipPrev;
	t_uint32 skipCurrent;
} t_sva_brc_in;

#endif //for ifdef __MPEG4ENC_ARM_MPC_BRC_H
