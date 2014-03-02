/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/**
 \defgroup Scalar Filter Response Services

 \detail This module provides an interface to the General Purpose Scalar.
*/

/**
 \file Scalar_FilterResponse.c
 \brief This file is a part of the scalar module release code. It implements a driver
        layer for the general purpose scalar filter response function.
 \ingroup Scalar
*/




#include "Platform.h"
#include "Scalar.h"
#include "Scalar_IPInterface.h"
#include "Scalar_OPInterface.h"
#include "ScalarPlatformSpecific.h"


extern float_t const g_f_coslut[];
/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void FilterResponseManager ( Scalar_UserParams_ts *UserParams,
							 Scalar_InternalParams_ts *InternalParams,
							 Scalar_Inputs_ts *Inputs,
							 Scalar_Output_ts *Output )
  \brief
  \return 	void
  \param 	Scalar_UserParams_ts *UserParams			:	Pointer to user parameter structure
  \param 	Scalar_InternalParams_ts *InternalParams	:	Pointer to internal parameter structure
  \param 	Scalar_Inputs_ts *Inputs					:	Pointer to scalar inputs structure
  \param 	Scalar_Output_ts *Output					:	Pointer to scalar output structure
  \callgraph
  \callergraph
  \ingroup Scalar
  \endif
*/
void FilterResponseManager ( Scalar_UserParams_ts *UserParams,
							 Scalar_InternalParams_ts *InternalParams,
							 Scalar_Inputs_ts *Inputs,
							 Scalar_Output_ts *Output )
{
  uint8_t  u8_MSize, u8_NSizeD2, u8_Mshp0, u8_PhysicalMSize, u8_Mshm1, u8_Mshp1, u8_Mshp2, u8_IPtr;
  uint8_t  u8_I, u8_IInc, u8_M, u8_MPtr, u8_N, u8_FoldedIPtr;

  uint8_t   u8_WAddr;

  uint16_t u16_SuperNSize, u16_Mrnd1, u16_Mrnd2, u16_PhysRegOffs, u16_PhysRegMask, u16_PhysRegOffsD2;

  float_t  f_MSize, f_CutOff, f_SafeZone, f_InvSuperNSize, f_CutxPi, f_SDenom, f_CutxPt575;
  float_t  f_SArg, f_SArg0, f_WArg, f_SRes, f_SLut1, f_SLut2, f_Tmp, f_SInt, f_SVal, f_InvsDenom;
  float_t  f_WRes, f_wLut1, f_wLut2, f_WInt, f_WVal, f_Sum, f_Normer;

  bool_t   bo_SNegative, bo_WDownCounting, bo_SincApprox, bo_SDownCounting, bo_TransParentAZ;

  int16_t  s16_SAddr, s16_ISum0, s16_ISum1, s16_SiTmp, s16_LVal, s16_ITmp, s16_IDif0, s16_IDif1;
  int16_t  s16_Aug, s16_RVal;

  float_t  f_Y[MAX_COF_COUNT];

  int16_t  s16_Y[MAX_COF_COUNT];

  u8_MSize = 1 << Output->u8_LogMSize;

  f_MSize = (float_t)u8_MSize;

  /* dimensions */

  u8_Mshp0 = Output->u8_LogMSize;

  /* preliminary: manage the input variables */

  // nsized2 = *nsize >> 1;
  u8_NSizeD2 = Output->u8_NSize >> 1;

  // super_nsize = *nsize + (*overhang << 1);
  u16_SuperNSize = Output->u8_NSize + (InternalParams->u8_Overhang << 1);


  u8_PhysicalMSize = u8_MSize >> 1;

  // osize = physical_msize * *nsize;
  Output->u8_CofCount = u8_PhysicalMSize * Output->u8_NSize;

  // mshm1 = mshp0 - 1;
  u8_Mshm1 = u8_Mshp0 - 1;

  // mshp1 = mshp0 + 1; mrnd1 = (1 << mshp0);
  u8_Mshp1 = u8_Mshp0 + 1;

  // mshp2 = mshp0 + 2; mrnd2 = (1 << mshp1);
  u8_Mshp2 = u8_Mshp0 + 2;

  u16_Mrnd1 = 1 << u8_Mshp0;

  u16_Mrnd2 = 1 << u8_Mshp1;

  // phys_regoffs = (1 << *phys_regwidth);
  u16_PhysRegOffs = (1 << UserParams->u8_PhysRegWidth);

  // phys_regmask = (phys_regoffs - 1);
  // phys_regoffsd2 = (phys_regoffs >> 1);
  u16_PhysRegMask = (u16_PhysRegOffs - 1);
  u16_PhysRegOffsD2 = (u16_PhysRegOffs >> 1);

  /* invert & scale step */
  // f_cutoff = FW_FP_UNITY / f_localstep;
  f_CutOff = FW_FP_UNITY / Inputs->f_Step;

  /* crispness */
  // f_cutoff *= (double)(*crisp + 8) / 16.0;
  f_CutOff *= (UserParams->u8_Crisp + 8) / 16.0;

  /* safezone */
  // f_safezone = *antizip ? SCALED_INV_AZ_SAFEZONE : SCALED_UNITY;
  f_SafeZone = (Flag_e_TRUE == UserParams->e_Flag_AntiZipEnable) ? SCALED_INV_AZ_SAFEZONE : SCALED_UNITY;

  if (f_CutOff > f_SafeZone)
  {
	  f_CutOff = f_SafeZone;
  }

  /* expand to M-th band */
  // f_cutoff /= f_msize;
  f_CutOff /= f_MSize;

  /* invert super_nsize & scale to M-th band */
/*
  f_inv_super_nsize =  *no_polyph ? FW_FP_UNITY / (double)(super_nsize * msize)
  									: FW_FP_TWO / (double)(super_nsize * msize);

*/

  f_InvSuperNSize = FW_FP_TWO / (u16_SuperNSize * u8_MSize);

  /* PASS 1.1: (1-D): generate prototype (cross-product of sinc & window functions) */
  u8_IPtr = Output->u8_CofCount;

  if (Output->u8_NSize > 2)
  {
    // f_cutxpi = 3.141593 * f_cutoff;
    f_CutxPi = PI * f_CutOff;

    // f_sdenom = f_cutxpi / 2.0;
    f_SDenom = f_CutxPi / 2.0;

    // f_cutxpt575 = 0.575 * f_cutoff;
    f_CutxPt575 = 0.575 * f_CutOff;

    // f_sarg0 = (*no_polyph) ? 0.0 : f_cutxpt575 / 2.0;
    f_SArg0 = f_CutxPt575 / 2.0;

    // f_sarg = (*no_polyph) ? FW_FP_AHALF : FW_FP_AHALF - f_cutoff / 2.0;
    f_SArg = FW_FP_AHALF - f_CutOff / 2.0;

    // f_warg = (*no_polyph) ? 0.0 : f_inv_super_nsize / 2.0;
    f_WArg = f_InvSuperNSize / 2.0;

    // s_negative = w_downcounting = FALSE;
    bo_SNegative = bo_WDownCounting = FALSE;

    // sinc_approx = s_downcounting = TRUE
    bo_SincApprox = bo_SDownCounting = TRUE;
  }

  bo_TransParentAZ = (Flag_e_TRUE == InternalParams->e_Flag_Transparent) && (Flag_e_TRUE == UserParams->e_Flag_AntiZipEnable);

  /* some magnification parameters (assumes we start fully zoomed-out) */
  for(u8_I = 0; u8_I < Output->u8_CofCount; u8_I++)
  {
    if (bo_TransParentAZ)
    {
    	--u8_IPtr;
    	// f_y[iptr] = (iptr >= physical_msize) ? 0.5 : 0.0;
    	f_Y[u8_IPtr] = (u8_IPtr >= u8_PhysicalMSize) ? 0.5 : 0.0;
    }
    else if (Output->u8_NSize > 2)
    {
      if (bo_SincApprox)
      {
        // saddr = (int)(f_sarg0 + .5);
    	s16_SAddr = (int16_t)(f_SArg0 + 0.5);

    	// f_sres = f_sarg0 - (double)saddr
    	f_SRes = f_SArg0 - (float_t)s16_SAddr;
    	/* printf("A: FSRES %f FSARG0 %f FMSIZE %f FSADDR %f\n",f_sres,f_sarg0,f_msize,(double)saddr); */
      }
      else
      {
    	// saddr = (int)(f_sarg + .5);
    	s16_SAddr = (int16_t)(f_SArg + 0.5);

    	// f_sres = f_sarg - (double)saddr;
    	f_SRes = f_SArg - (float_t)s16_SAddr;
    	/* printf("S: FSRES %f FSARG  %f FMSIZE %f FSADDR %f\n",f_sres,f_sarg,f_msize,(double)saddr); */
      }

      /* printf("SADDR %5d SRES %12.8f WADDR %5d WRES %12.8f\n", saddr,f_sres,waddr,f_wres); */

      // f_slut1 = f_coslut[saddr];
      f_SLut1 = g_f_coslut[s16_SAddr];

      if (f_SRes < 0.0)
      {
    	// f_slut2 = f_coslut[saddr - 1];
    	f_SLut2 = g_f_coslut[s16_SAddr - 1];
    	// f_sres = -f_sres;
    	f_SRes = -f_SRes;
      }
      else
      {
    	// f_slut2 = f_coslut[saddr + 1];
    	f_SLut2 = g_f_coslut[s16_SAddr + 1];
      }

      // f_tmp = (f_sres * (f_slut2 - f_slut1));
      f_Tmp = (f_SRes * (f_SLut2 - f_SLut1));

      // f_sint = f_slut1 + f_tmp;
      f_SInt = f_SLut1 + f_Tmp;

      if (bo_SincApprox)
      {
    	// f_sval = f_sint;
    	f_SVal = f_SInt;
      }
      else
      {
    	// f_invsdenom = FW_FP_UNITY / f_sdenom;
    	f_InvsDenom = FW_FP_UNITY / f_SDenom;
    	// f_sincprod = f_sint * f_invsdenom;
    	f_InvsDenom *= f_SInt;
    	// f_sval = (s_negative ? -f_sincprod : f_sincprod);
    	f_SVal = (bo_SNegative ? -f_InvsDenom : f_InvsDenom);
      }

      /* printf("SVAL %8.4f from SINT %8.4f",f_sval,f_sint); */

      // waddr = (int)(f_warg + .5);
      u8_WAddr = (uint8_t)(f_WArg + 0.5);

      // f_wres = f_warg - (double)waddr;
      f_WRes = f_WArg - u8_WAddr;

      // f_wlut1 = f_coslut[waddr];
      f_wLut1 = g_f_coslut[u8_WAddr];

      if (f_WRes < 0.0)
      {
    	// f_wlut2 = f_coslut[waddr - 1];
    	f_wLut2 = g_f_coslut[u8_WAddr - 1];
    	// f_wres = -f_wres;
    	f_WRes = -f_WRes;
      }
      else
      {
    	// f_wlut2 = f_coslut[waddr + 1];
    	f_wLut2 = g_f_coslut[u8_WAddr + 1];
      }

      // f_tmp = f_wres * (f_wlut2 - f_wlut1);
      f_Tmp = f_WRes * (f_wLut2 - f_wLut1);

      // f_wint = f_wlut1 + f_tmp;
      f_WInt = f_wLut1 + f_Tmp;

      // f_wval = (1.0 + (w_downcounting ? -f_wint : f_wint)) / 2.0;
      f_WVal = (1.0 + (bo_WDownCounting ? -f_WInt : f_WInt)) / 2.0;

      /* printf("WVAL %8.4f from WINT %8.4f (%8.4f + %12.5f x %8.4f)\n", */
      /* f_wval,f_wint,f_wlut1,f_wres,f_wlut2-f_wlut1); */

      // f_y[--iptr] = f_wval * f_sval;
      f_Y[--u8_IPtr] = f_WVal * f_SVal;

      /* printf("YVAL %8.4f from WARG %8.4f SARG %8.4f\n",f_y[iptr],f_warg,sinc_approx?f_sarg0:f_sarg); */
      if(bo_SDownCounting)
      {
        // f_sarg -= f_cutoff;
    	f_SArg -= f_CutOff;
        if(f_SArg < 0.0)
        {
          // f_sarg = -f_sarg;
          f_SArg = -f_SArg;

          // s_downcounting = FALSE;
          bo_SDownCounting = FALSE;
        }
      }
      else
      {
        // f_sarg += f_cutoff;
    	f_SArg += f_CutOff;
    	if(f_SArg > FW_FP_AHALF)
    	{
    	  // f_sarg = FW_FP_UNITY - f_sarg;
          f_SArg = FW_FP_UNITY - f_SArg;

          // s_downcounting = TRUE;
    	  bo_SDownCounting = TRUE;

    	  // s_negative = !s_negative;
    	  bo_SNegative = !bo_SNegative;
    	}
      }

      // sinc_approx = ((f_sdenom += f_cutxpi) < FW_FP_UNITY);
      bo_SincApprox = ((f_SDenom += f_CutxPi) < FW_FP_UNITY);

      if (bo_SincApprox)
      {
    	// f_sarg0 += f_cutxpt575;
        f_SArg0 += f_CutxPt575;
      }
      if((!bo_WDownCounting) && ((f_WArg += f_InvSuperNSize) > FW_FP_AHALF))
      {
        // f_warg = FW_FP_UNITY - f_warg;
    	f_WArg = FW_FP_UNITY - f_WArg;
      	// w_downcounting = TRUE;
    	bo_WDownCounting = TRUE;
      }
      else if((bo_WDownCounting) && ((f_WArg -= f_InvSuperNSize) < 0.0))
      {
        // f_warg = -f_warg;
    	f_WArg = -f_WArg;
      	// w_downcounting = FALSE;
    	bo_WDownCounting = FALSE;
      }
    } // else if (Output->u8_NSize > 2)
    else
    {
      // f_y[--iptr] = ((double)iptr + 0.5) / f_msize;
      --u8_IPtr;
      f_Y[u8_IPtr] = ((float_t)u8_IPtr + 0.5) / f_MSize;      
    }
  } // for(b_I = 0; b_I < Output->u8_CofCount; b_I++)

  /* complete min-phase filter by copying */
  /* PASS 1.2: (1-D): normalise prototype */
  /* sum the vector */

  // f_sum = f_y[0];
  f_Sum = f_Y[0];
  for(u8_I = 1; u8_I < Output->u8_CofCount; u8_I++)
  {
    // f_sum += f_y[i];
	f_Sum += f_Y[u8_I];
  }

  /* normalise the vector */
  // f_normer = f_msize * (double)FW_CLUT_AHALF / f_sum;
  f_Normer = f_MSize * FW_CLUT_AHALF / f_Sum;

  for(u8_I = 0; u8_I < Output->u8_CofCount; u8_I++)
  {
    // f_tmp = f_y[i] * f_normer;
	f_Tmp = f_Y[u8_I] * f_Normer;
	// y[i] = (short int)(f_tmp + .5);
	s16_Y[u8_I] = (int16_t)(f_Tmp + .5);
  }

  /* ========================= start mini_decompose ============================ */

  // iinc = (*no_polyph) ? 2 : msize;
  u8_IInc = u8_MSize;

  for (u8_M = 0; u8_M < u8_PhysicalMSize; u8_M++)
  {
    // isum0 = isum1 = 0;
	s16_ISum0 = s16_ISum1 = 0;

	// iptr = mptr = m;
	u8_IPtr = u8_MPtr = u8_M;

	for (u8_N = 0; u8_N < Output->u8_NSize; u8_N++)
	{
      // folded_iptr = iptr >= osize ? (osize << 1) - (iptr + 1) : iptr;
	  u8_FoldedIPtr = (u8_IPtr >= Output->u8_CofCount) ? (Output->u8_CofCount << 1) - (u8_IPtr + 1) : u8_IPtr;

	  // sitmp = (y[folded_iptr] + (1<<(13-*phys_regwidth))) >> (14 - *phys_regwidth);
	  s16_SiTmp = (s16_Y[u8_FoldedIPtr] + (1 << (13 - UserParams->u8_PhysRegWidth))) >> (14 - UserParams->u8_PhysRegWidth);

	  // sitmp = (sitmp > phys_regmask) ? phys_regmask : sitmp;
	  s16_SiTmp = (s16_SiTmp > u16_PhysRegMask) ? u16_PhysRegMask : s16_SiTmp;

	  if ((Flag_e_TRUE == UserParams->e_Flag_AntiZipEnable) && (u8_N & 1))
	  {
		// isum1 += sitmp;
		s16_ISum1 += s16_SiTmp;
	  }
	  else
	  {
		// isum0 += sitmp;
		s16_ISum0 += s16_SiTmp;
	  }

	  // nvec[mptr] = sitmp;
	  Output->ptrs16_Cof[u8_MPtr] = s16_SiTmp;

	  // iptr += iinc;
	  u8_IPtr += u8_IInc;

	  // mptr += physical_msize;
	  u8_MPtr += u8_PhysicalMSize;
	} // for (u8_N = 0; u8_N < Output->u8_NSize; u8_N++)

	// lval = ((m<<1)+1);
	s16_LVal = ((u8_M << 1) + 1);

	if (Flag_e_TRUE == UserParams->e_Flag_AntiZipEnable)
	{
      if (!(u8_NSizeD2 & 1))
      {
    	/* swap roles of isum0 & isum1 for coding simplicity */
    	// itmp = isum0;
    	s16_ITmp = s16_ISum0;

    	// isum0 = isum1;
    	s16_ISum0 = s16_ISum1;

    	// isum1 = itmp;
    	s16_ISum1 = s16_ITmp;
      }

      // idif0 = phys_regoffsd2 - isum0;
      s16_IDif0 = u16_PhysRegOffsD2 - s16_ISum0;

      // idif1 = phys_regoffsd2 - isum1;
      s16_IDif1 = u16_PhysRegOffsD2 - s16_ISum1;
	}
	else
	{
      /* dump the DC compensation */
      // idif0 = phys_regoffsd2 - (isum0 >> 1);
      s16_IDif0 = u16_PhysRegOffsD2 - (s16_ISum0 >> 1);

      // idif1 = phys_regoffsd2 + (isum0 >> 1) - isum0;
      s16_IDif1 = u16_PhysRegOffsD2 - (s16_ISum0 >> 1) - s16_ISum0;
	}

	if (Output->u8_NSize & 1)
	{
	  /* spread DC/Nyquist compensation over centre 3 taps, preserving group delay */
	  // nvec[m + ((nsized2-1) << mshm1)] +=
	  // 							(aug = ((lval * idif0 + mrnd1) >> mshp1));
	  Output->ptrs16_Cof[u8_M + ((u8_NSizeD2 - 1) << u8_Mshm1)] +=
		  (s16_Aug = ((s16_LVal * s16_IDif0 + u16_Mrnd1) >> u8_Mshp1));

	  // nvec[m + (nsized2 << mshm1)] += idif1;
	  Output->ptrs16_Cof[u8_M + (u8_NSizeD2 << u8_Mshm1)] += s16_IDif1;

	  // nvec[m + ((nsized2+1) << mshm1)] += idif0 - aug;
	  Output->ptrs16_Cof[u8_M + ((u8_NSizeD2 + 1) << u8_Mshm1)] += s16_IDif0 - s16_Aug;
	}
	else if (2 == Output->u8_NSize)
	{
      // nvec[m] += idif0;
	  Output->ptrs16_Cof[u8_M] += s16_IDif0;
	  // nvec[u8_M + physical_msize] += idif1;
	  Output->ptrs16_Cof[u8_M + u8_PhysicalMSize] += s16_IDif1;
	}
	else
	{
	  /* spread DC/Nyquist compensation over centre 4 taps, preserving group delay */
	  // rval = (physical_msize << 3) - lval;
	  s16_RVal = (u8_PhysicalMSize << 3) - s16_LVal;

	  // nvec[m + ((nsized2-2) << mshm1)] +=
	  // 		(aug = (lval * idif1 + mrnd2) >> mshp2);
	  Output->ptrs16_Cof[u8_M + ((u8_NSizeD2 - 2) << u8_Mshm1)] +=
		  (s16_Aug = ((s16_LVal * s16_IDif1 + u16_Mrnd2) >> u8_Mshp2));

	  // nvec[m + (nsized2 << mshm1)] += idif1 - aug;
	  Output->ptrs16_Cof[u8_M + (u8_NSizeD2 << u8_Mshm1)] += s16_IDif1 - s16_Aug;

	  // nvec[m + ((nsized2+1) << mshm1)] +=
	  // 	(aug = (rval * idif0 + mrnd2) >> mshp2);
	  Output->ptrs16_Cof[u8_M + ((u8_NSizeD2 + 1) << u8_Mshm1)] +=
		  (s16_Aug = ((s16_RVal * s16_IDif0 + u16_Mrnd2) >> u8_Mshp2));

	  // nvec[m + ((nsized2-1) << mshm1)] += idif0 - aug;
	  Output->ptrs16_Cof[u8_M + ((u8_NSizeD2 - 1) << u8_Mshm1)] += s16_IDif0 - s16_Aug;
	}

	/* PASS 2.3: (2-D): code for virtual negatives (control variable for SGS sims only) */
	// mptr = m;
	u8_MPtr = u8_M;

	for (u8_N = 0; u8_N < Output->u8_NSize; u8_N++)
	{
	  if (Output->ptrs16_Cof[u8_MPtr] < 0)
	  {
		  Output->ptrs16_Cof[u8_MPtr] += u16_PhysRegOffs;
	  }
	  u8_MPtr += u8_PhysicalMSize;
	}

  } // for (u8_M = 0; u8_M < u8_PhysicalMSize; u8_M++)

  return;

}
