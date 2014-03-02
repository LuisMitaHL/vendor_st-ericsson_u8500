/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: none.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**********************************************************************/
/**
  \file mdrc5b_dsp.c
  \brief mathematical and signal processing routines
  \author Zong Wenbo
  \email wenbo.zong@st.com
  \date DEC 1,  2009
  \last rev DEC 7, 2009
*/
/*####################################################################*/
/* Copyright  STMicroelectonics, HED/HVD/AA&VoIP @ Singapore          */
/* This program is property of HDE/HVD/Audio Algorithms & VOIP        */
/* Singapore from STMicroelectronics.                                 */
/* It should not be communicated outside STMicroelectronics           */
/* without authorization.                                             */
/*####################################################################*/


#ifdef _NMF_MPC_
#include "libeffects/libmdrc5b.nmf"
#endif

#include "mdrc5b_dsp.h"


#ifdef __flexcc2__
#pragma inline
#endif // __flexcc2__
static void mdrc5b_biquad(MMshort                   *inptr,
                          int     const             OffsetIn,
                          MMshort                   *outptr,
                          int                       sampcount,
                          MMshort const FILTER_COEF *filtcfs,
                          MMshort       FILTER_MEM  *filtvars)
{

    WORD56                   accum;
    WORD56                   p;
    WORD24                   k;
    WORD24                   x1;
    WORD24                   x2;
    WORD24                   y;
    WORD24 const FILTER_COEF *coeff_ptr;
    int                      i;
    int                      OffsetOut, OffsetOutx2;


    OffsetOut      = 1;
    OffsetOutx2    = 2;

    x2             = *filtvars++;
    x1             = *filtvars;

    filtvars      += 2;

    coeff_ptr     = filtcfs;

    k             = *coeff_ptr++;   /*B2*/

    p             = wX_fmul(k, x2);
    x2            = x1;
    k             = *coeff_ptr++;   /*B1*/


    accum         = p;
    p             = wX_fmul(k, x1);
    k             = *coeff_ptr++;   /*B0*/

    x1            = *inptr;
    inptr        += OffsetIn;

    accum         = wX_subl(p,accum);
    p             = wX_fmul(k, x1);
    k             = *coeff_ptr++;   /*A1*/
    y             = *filtvars--;    /*y1*/

    accum        -= p;
    p             = wX_fmul(k, y);
    y             = *filtvars++;    /*y2*/
    k             = *coeff_ptr;     /*A2*/
    coeff_ptr     = filtcfs;


    //--------------------------- unrolled cycle  -----------------------

    accum         = wX_subl(p, accum);
    p             = wX_fmul(k, y);
    k             = *coeff_ptr++;   /*B2*/

    y             = wsubr(accum, p);
    p             = wX_fmul(k, x2);
    x2            = x1;
    k             = *coeff_ptr++;   /*B1*/

    accum         = p;
    p             = wX_fmul(k, x1);
    k             = *coeff_ptr++;   /*B0*/

    x1            = *inptr;
    inptr        += OffsetIn;

    *outptr       = y;
    outptr       += OffsetOut;

    accum         = wX_subl(p, accum);
    p             = wX_fmul(k, x1);
    k             = *coeff_ptr++;   /*A1*/

    accum        -= p;
    p             = wX_fmul(k, y);
    k             = *coeff_ptr;     /*A2*/
    coeff_ptr     = filtcfs;
    y             = *filtvars;      /*y2*/

#ifdef __flexcc2__
#pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
    for(i = 2; i < sampcount; i++)
    {
        accum     = wX_subl(p, accum);
        p         = wX_fmul(k, y);
        k         = *coeff_ptr++;   /*B2*/

        y         = wsubr(accum, p);
        p         = wX_fmul(k, x2);
        x2        = x1;
        k         = *coeff_ptr++;   /*B1*/

        accum     = p;
        p         = wX_fmul(k, x1);
        k         = *coeff_ptr++;   /*B0*/

        x1        = *inptr;
        inptr    += OffsetIn;

        *outptr   = y;
        outptr   -= OffsetOut;

        accum     = wX_subl(p, accum);
        p         = wX_fmul(k, x1);
        k         = *coeff_ptr++;   /*A1*/

        accum    -= p;
        p         = wX_fmul(k, y);
        y         = *outptr;
        outptr   += OffsetOutx2;

        k         = *coeff_ptr;     /*A2*/
        coeff_ptr = filtcfs;
    }

    accum         = wX_subl(p, accum);
    p             = wX_fmul(k, y);

    y             = wsubr(accum, p);

    *outptr       = y;
    outptr       -= OffsetOut;

    *filtvars--   = y;
    *filtvars--   = *outptr;
    *filtvars--   = x1;
    *filtvars     = x2;
} // end mdrc5b_biquad



void mdrc5b_biquad2(MMshort                   *inptr,
                    int     const             OffsetIn,
                    MMshort                   *outptr,
                    int                       sampcount,
                    MMshort const FILTER_COEF *filtcfs,
                    MMshort       FILTER_MEM  *filtvars)
{
    mdrc5b_biquad(inptr,  OffsetIn, outptr, sampcount, filtcfs, filtvars);
    mdrc5b_biquad(outptr,        1, outptr, sampcount, filtcfs, filtvars + 4);
} // end mdrc5b_biquad2
