/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
 * Duster_op_interface.h
 *
 *  Created on: Mar 19, 2010
 *      Author: mamtac
 */
#ifndef DUSTER_OP_INTERFACE_H_
#   define DUSTER_OP_INTERFACE_H_

/**
 \defgroup Duster Module

 \brief Duster module takes input from Host and apply it to ISP pixel pipe.
*/

/**
 \file Duster_op_interface.h
 \brief  This file is a part of the Duster module release code and provide an
         interface to the module. All functionalities offered by the module are
         available through this file. The file also declare the page elements
         to be used in virtual register list.

 \ingroup Duster
*/
#   include "Platform.h"
#   include "PictorhwReg.h"

/**********************************************************************************/

/*  Exported structures                                                           */

/**********************************************************************************/

// Enum for the Duster Bypass control
typedef enum
{
    DusterBypassControl_DusterEnable_AllModule  = 0x00,
    DusterBypassControl_DusterRC_Bypass         = 0x01,
    DusterBypassControl_DusterCC_Bypass         = 0x02,
    DusterBypassControl_DusterDetect_CCBypass   = 0x04,
    DusterBypassControl_DusterDefcor_Bypass     = 0x08,
    DusterBypassControl_DusterGaussian_Bypass   = 0x10,
    DusterBypassControl_DusterRCUse_Simplified  = 0x20,
    DusterBypassControl_DusterRC_EnableGrad     = 0x40,
} DusterBypassControl_te;

typedef enum
{
    DusterBypassControlScytheControl_DefectMap      = 0x00,
    DusterBypassControlScytheControl_ForceScythe    = 0x01,
    DusterBypassControlScytheControl_BypassScythe   = 0x02
} DusterBypassScytheControl_te;

// Page to accept host inputs for Duster block control
typedef struct
{
// Host specified frame sigma estimate
uint16_t u16_FrameSigma;

// Duster bypass control
// duster_scythe_mode: define scythe mode defect_map, force_scythe, bypass_scythe
// 00: (SCYTHEMAP) Scythe is used with defect pixel mapped only
// 01: (SCYTHEANDMAP) Scythe is used with defect pixel mapped and defect detected pixel
// 10: (BYPASSSCYTHE) Scythe is bypassed
// 11: (FORCESCYTHE) Scythe filter is always enable
// [6] duster_rc_enablegrad: enable gradiant correction
// [5] duster_rc_use_simplified: RC Threshold as it is program in register bank
// [4] duster_gaussian_bypass: Bypass gaussian filtering
// [3] duster_defcor_bypass: Bypass deftect correction
// [2] duster_detect_cc_bypass: Bypass Detection of defect Central Pixel
// [1] duster_cc_bypass: Bypass Central Corrector
// [0] duster_rc_bypass: Bypass Ring Corrector
uint16_t u16_Duster_ByPass_Ctrl;

// Flag to control enable/disable Duster block
uint8_t e_Flag_DusterEnable;

// Duster gaussian weight
uint8_t u8_GaussianWeight;

// Duster sigma weight
uint8_t u8_SigmaWeight;

// Scythe control for high population
uint8_t u8_ScytheControl_hi;

// Scythe control for low population
uint8_t u8_ScytheControl_lo;

// Defcor centre corrector threshold control (number of local sigma, coded as ux.2)
uint8_t u8_CenterCorrectionSigmaFactor;

// Defcor ring corrector normalized threshold control
uint8_t u8_RingCorrectionNormThr;
} DusterControl_ts;


// Page to hold the computed Duster hardware parameters
typedef struct
{
// Frame sigma
uint16_t u16_FrameSigma;

// Gaussian Th1
uint16_t u16_Gaussian_Th1;

// Gaussian Th2
uint16_t u16_Gaussian_Th2;

// Gaussian Th3
uint16_t u16_Gaussian_Th3;

// Sigma0 parameter
uint16_t u16_S0;

// Sigma1 parameter
uint16_t u16_S1;

// Sigma2 parameter
uint16_t u16_S2;

// scythe rank for high population
uint8_t u8_ScytheRank_hi;

// scythe rank for low population
uint8_t u8_ScytheRank_lo;

// scythe smooth control for high population
uint8_t u8_ScytheSmoothControl_hi;

// scythe smooth control for low population
uint8_t u8_ScytheSmoothControl_lo;

// center correction sigma CC_LOCAL_SIGMA_TH
uint8_t u8_CCLocalSigma_Th;

/// Ring Correction Normalised Threshold RC_NORM_TH
uint8_t u8_RCNorm_Th;

/// gaussian filter control SIGMA_GAUSSIAN
uint8_t u8_SigmaGaussian;

/// Gaussian Sigma Weight promoted value
uint8_t u8_GaussianWeight;
} DusterStatus_ts;

/************************ Exported Page elements *********************/

/// Control Page element for Duster
extern DusterControl_ts             g_DusterControl;
//extern DusterScytheControl_ts       g_DusterScytheControl;
//extern DusterRingCenterWeight_ts    g_DusterRingCenterWeight;
//extern DusterSigmaGaussianWeight_ts g_DusterSigmaGaussianWeight;
extern DusterStatus_ts              g_DusterStatus;

#if 0
extern void 						Duster_Isr (void);
extern void 						Duster_DamperUpdate (void);
extern void 						Duster_CheckFrameSync (void);
#endif

#endif /* DUSTER_OP_INTERFACE_H_ */

