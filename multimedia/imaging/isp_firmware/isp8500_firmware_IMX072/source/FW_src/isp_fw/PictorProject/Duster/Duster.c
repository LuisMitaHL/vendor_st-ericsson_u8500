/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Duster.c
 *
 *  Created on: Mar 20, 2010
 *      Author: mamtac
 *   \brief This file is a part of the duster module release code. It implements a driver layer for the duster.
 *
 \ingroup Duster
 *
 */

// includes
// ========
#include "Duster.h"
#include "Duster_ip_interface.h"
#include "Duster_op_interface.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Duster_DusterTraces.h"
#endif

/*
Scythe_Shared_Damper_ts Scythe_Shared_Damper;
CenterWeight_Shared_Damper_ts   CenterWeight_Shared_Damper;
RingWeight_Shared_Damper_ts     RingWeight_Shared_Damper;
SigmaWeight_Shared_Damper_ts    SigmaWeight_Shared_Damper;
GaussianWeight_Shared_Damper_ts GaussianWeight_Shared_Damper;
*/

/**********************************************************************************/

/*  Exported VARIABLES                                                            */

/**********************************************************************************/

DusterControl_ts                g_DusterControl =
{
    DEFAULT_USER_FRAME_SIGMA,
    DEFAULT_DUSTER_BYPASS_CTRL,
    DEFAULT_DUSTER_ENABLE_DISABLE,
    DEFAULT_DUSTER_GAUSSIAN_WEIGHT,
    DEFAULT_DUSTER_SIGMA_WEIGHT,
    DEFAULT_SCYTHECONTROL_SCYT_SMOOTH_HI,
    DEFAULT_SCYTHECONTROL_SCYT_RANK_LO,
    DEFAULT_CENTERCORRECTION_SIGMA_FACTOR,
    DEFAULT_RING_CORRECTION_NORMAL_THR,
};

DusterStatus_ts                 g_DusterStatus =
{
    DEFAULT_DUSTEROUTPUT_FRAME_SIGMA,
    DEFAULT_DUSTEROUTPUT_GAUSSIAN_TH1,
    DEFAULT_DUSTEROUTPUT_GAUSSIAN_TH2,
    DEFAULT_DUSTEROUTPUT_GAUSSIAN_TH3,
    DEFAULT_DUSTEROUTPUT_S0,
    DEFAULT_DUSTEROUTPUT_S1,
    DEFAULT_DUSTEROUTPUT_S2,
    DEFAULT_DUSTEROUTPUT_SCYT_RANK_HI,
    DEFAULT_DUSTEROUTPUT_SCYT_RANK_LO,
    DEFAULT_DUSTEROUTPUT_SCYT_SMOOTH_HI,
    DEFAULT_DUSTEROUTPUT_SCYT_SMOOTH_LO,
    DEFAULT_DUSTEROUTPUT_CC_LOCAL_SIGMA_TH,
    DEFAULT_DUSTEROUTPUT_RC_NORM_TH,
    DEFAULT_DUSTEROUTPUT_SIGMA_GAUSSIAN,
    DEFAULT_DUSTEROUTPUT_GAUSSIAN_WEIGHT
};


/******************************************************
- Function:  Duster_Commit
- Use:       Program Duster Registers
- params     no input parameters
- return     void
********************************************************/
void
Duster_Commit(void)
{
   if (Flag_e_TRUE == g_DusterControl.e_Flag_DusterEnable)
   {
      Duster_Set_GAUSSIAN_TH1(g_DusterStatus.u16_Gaussian_Th1);
      Duster_Set_GAUSSIAN_TH2(g_DusterStatus.u16_Gaussian_Th2);
      Duster_Set_GAUSSIAN_TH3(g_DusterStatus.u16_Gaussian_Th3);
      Duster_Set_SIGMA_GAUSSIAN(g_DusterStatus.u8_SigmaGaussian);
      Duster_Set_ISP_DUSTER_BYPASS_CTRL(g_DusterControl.u16_Duster_ByPass_Ctrl);

      Duster_Set_SCYT_SMOOTH_HI(g_DusterStatus.u8_ScytheSmoothControl_hi);
      Duster_Set_SCYT_SMOOTH_LO(g_DusterStatus.u8_ScytheSmoothControl_lo);

      Duster_Set_SCYT_RANK_HI(g_DusterStatus.u8_ScytheRank_hi);
      Duster_Set_SCYT_RANK_LO(g_DusterStatus.u8_ScytheRank_lo);

      Duster_Set_CC_LOCAL_SIGMA_TH(g_DusterStatus.u8_CCLocalSigma_Th);
      Duster_Set_RC_NORM_TH(g_DusterStatus.u8_RCNorm_Th);

      Duster_EnableHardware();
   }
   else
   {
      Duster_DisableHardware();
   }
}



/******************************************************
- Function:  Duster_UpdateGaussian
- Use:        Update values of duster hw register input parameters
                  on the basis of HOST provided inputs
- params     no input parameters
- return     void
********************************************************/
void Duster_UpdateGaussian()
{
   uint32_t u32_Value = 0;
   if (Flag_e_TRUE == g_DusterControl.e_Flag_DusterEnable)
   {
     OstTraceInt0(TRACE_DEBUG, "Duster is enabled");

     g_DusterStatus.u8_GaussianWeight = g_DusterControl.u8_GaussianWeight;
     g_DusterStatus.u16_FrameSigma =    g_DusterControl.u16_FrameSigma;
     u32_Value = g_DusterStatus.u16_FrameSigma * g_DusterStatus.u8_GaussianWeight;

     g_DusterStatus.u16_Gaussian_Th1 = (u32_Value + 64) >> 7;
     g_DusterStatus.u16_Gaussian_Th2 = ((u32_Value << 1) + 64) >> 7;
     g_DusterStatus.u16_Gaussian_Th3 = ((u32_Value * 3) + 64) >> 7;
     g_DusterStatus.u8_SigmaGaussian = (g_DusterControl.u16_FrameSigma * g_DusterControl.u8_SigmaWeight + 8) >> 4;

     g_DusterStatus.u8_ScytheSmoothControl_hi = g_DusterControl.u8_ScytheControl_hi & 0x0F;
     g_DusterStatus.u8_ScytheRank_hi = (g_DusterControl.u8_ScytheControl_hi >> 4) & 0x07;

     g_DusterStatus.u8_ScytheSmoothControl_lo = g_DusterControl.u8_ScytheControl_lo & 0x0F;
     g_DusterStatus.u8_ScytheRank_lo = (g_DusterControl.u8_ScytheControl_lo >> 4) & 0x07;

     g_DusterStatus.u8_CCLocalSigma_Th = g_DusterControl.u8_CenterCorrectionSigmaFactor;
     g_DusterStatus.u8_RCNorm_Th = g_DusterControl.u8_RingCorrectionNormThr;
   }
}


