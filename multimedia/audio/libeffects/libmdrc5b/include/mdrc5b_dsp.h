/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: none.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mdrc5b_dsp_h_
#define _mdrc5b_dsp_h_

#include "mdrc5b.h"
#include "mdrc5b_rom.h"


#ifdef __flexcc2__
static
#pragma inline
#else // __flexcc2__
static inline
#endif // __flexcc2__
MMlong ivc_fmul48x24(MMlong a, MMshort b)
{
// if bit 23 of (a) is on, wextract_l(a) is negative and wround_L(a) = wextract_h(a) + 1
//   thus wfmul(b, wextract_l(a)) = wL_msr(wL_imulsu(b, wextract_l(a)), 23) - (b << 1)
//   and  wL_fmul(b, wround_L(a)) = wL_fmul(b, wextract_h(a)) + (b << 1)
// if bit 23 of (a) is off, wextract_l(a) is positive and wround_L(a) = wextract_h(a)
//   thus wfmul(b, wextract_l(a)) = wL_msr(wL_imulsu(b, wextract_l(a)), 23)
//   and  wL_fmul(b, wround_L(a)) = wL_fmul(b, wextract_h(a))
// thus in any case : wL_addsat(wL_fmul(b, wround_L(a)), wfmul(b, wextract_l(a)))
//                  = wL_addsat(wL_fmul(b, wextract_h(a)), wL_msr(wL_imulsu(b, wextract_l(a)), 23))

//     wL_addsat(wL_fmul(b, wround_L(a)), wfmul(b, wextract_l(a)))
  MMshort tmp1, tmp2;
  MMlong  param1, param2;
  MMlong  out;

  tmp1   = wextract_l(a);
  param2 = (MMlong)wfmul(b,tmp1);
  tmp2   = wround_L(a);
  param1 = wL_fmul(b,tmp2);
  out    = wL_addsat(param1, param2);

  return out;
}


#if 0   // not used
// input:  Q47
// result: Q16
#ifdef __flexcc2__
static
#pragma inline
#else // __flexcc2__
static inline
#endif // __flexcc2__
MMshort mdrc5b_log10(Word48 Input)
{
    int    n;
    Word48 X;
    MMshort Y;
    MMshort Tmp, SQY2, Y3;

    // log10(Input) = ln(Input)/ln(10)
    // Input = 2^n * X
    // ln(Input) = n*ln(2) + ln(X) = n*ln(2) + ln(1-Y), -1<=Y<1
    // ln(1-Y) = -x - x^2/2 - x^3/3

    X    = wL_abssat(Input);
    n    = wedge_X(X);        // Q47, # of bits to shift left
    X    = wL_msl(Input, n);
    X    = wL_subsat(Q47, X); // 1-x in Q47
    Y    = wround_L(X);       // 1-x in Q23
    Tmp  = wfmulr(Y, Y);
    Y3   = wfmulr(Tmp, Y);
    SQY2 = wmsr(Tmp, 1);
    Y3   = wfmulr(Y3, 2796203); // 1/3 in Q23
    X    = wL_imulss(-n, 2525223); // log10(2) in Q23, result in Q23
    X    = X - wfmul(Y, 3643126) - wfmul(SQY2, 3643126) - wfmul(Y3, 3643126); // 1/ln(10) in Q23
    X    = wL_msl(X, 17); // Q23 ==> Q40

    Y    = wround_L(X);   // Q40 ==>> Q16

    return Y;
}
#endif   // not used


#ifdef __flexcc2__
static
#pragma inline
#else // __flexcc2__
static inline
#endif // __flexcc2__
Word48 mdrc5b_log10_hd(Word48 Input)
{
    MMshort exp, idx, mnt24;
    Word48 mnt48;
    Word48 out;

    exp   = wedge_X(Input);       // Q47, # of bits to shift left
    mnt48 = wL_msl(Input, exp);   // Q47
    idx   = wL_msr(mnt48, 34);    // high 14 bits
    idx   = wand(idx, 0x0FFF);    // clear the highest 2 bits

    mnt24 = Log2Tab[idx];

    mnt48 = wL_neg(wL_msr(wL_fmul(mnt24, 2525223), 24));    // multiply with log2 in Q23
    out   = wL_subsat(mnt48, wL_imul(exp, 2525223));        // compensate for the normalization

    return out;
}


#define POW10_FRAC          16
#define POW10_FRAC_MASK     ((1<<POW10_FRAC)-1)
#define POW10_K             6966588  // 3.21, 3.321928
#define POW2_F3             662868   // 1.23, 0.07902
#define POW2_F2             1880105  // 1.23, 0.224126
#define POW2_F1             5845509  // 1.23, 0.696839
#define POW2_F0             8387031  // 1.23, 0.999812
#define POW10_GAIN_FRAC     22          // < POW10_FRAC
#define POW10_GAIN_INT      (24-POW10_GAIN_FRAC)

// z: 8.16
// result: 2.22
#ifdef __flexcc2__
static
#pragma inline
#else // __flexcc2__
static inline
#endif // __flexcc2__
MMshort mdrc5b_pow2(MMshort z)
{
    MMshort a;
    MMshort b;
    MMshort pow2;
    MMshort res;

    // 2^z
    // a = floor(z)
    // b = z - a
    // 2^z = 2^(a+b) = 2^a * 2^b
    // 2^b = f3 * b^3 + f2 * b^2 + f1 * b^1 + f0

    a = ((z & (~POW10_FRAC_MASK)) >> POW10_FRAC);
    b = (z & POW10_FRAC_MASK); // 8.16
    b = (b << (23-POW10_FRAC)); // 1.23

    if (b != 0)
    {
        pow2 = wfmul(b, POW2_F3); // 1.23
        pow2 = ((pow2 + POW2_F2)>>1); // 2.22
        pow2 = wfmul(b, pow2); // 2.22
        pow2 = pow2 + (POW2_F1>>1); // 2.22
        pow2 = wfmul(b, pow2); // 2.22
        pow2 = pow2 + (POW2_F0>>1); // 2.22
    }
    else
    {
        pow2 = (1<<22); // 2.22
    }

    a = (a > (POW10_GAIN_INT-2) ? (POW10_GAIN_INT-2) : (a < -POW10_GAIN_FRAC ? -POW10_GAIN_FRAC : a));
    //pow2 = (pow2 >> (30-POW10_GAIN_FRAC)); // 2.22
    //res = (a > 0? pow2 << a : pow2 >> (-a));
    // the above two steps can be combined into one
    res = wmsr(pow2, (22-POW10_GAIN_FRAC - a)); // (22-POW10_GAIN_FRAC - a) = # of bits to shift right, in the range [0 22]

    return res;
}


// X: 8.16
// result: 1.23
#ifdef __flexcc2__
static
#pragma inline
#else // __flexcc2__
static inline
#endif // __flexcc2__
void mdrc5b_pow10(MMshort X, int *exp, MMshort *mant)
{
    MMshort z;

    // 10^X = 2^(k*x) = 2^z
    // a = floor(z)
    // b = z - a
    // 2^z = 2^(a+b) = 2^a * 2^b
    // 2^b = f3 * b^3 + f2 * b^2 + f1 * b^1 + f0

    z     = wfmul(X, POW10_K); // 8.16 * 3.21 -> 10.14
    *exp  = wmsr(z, 14);
    *mant = z - wmsl(*exp, 14);
    *mant = wmsl(*mant, 2); // 8.16

    *mant = mdrc5b_pow2(*mant); // 2.22

    *exp = *exp + 1; // convert mantissa to 1.23

    return ;
}


//#define BETTER_POW10_ACCURACY
// input:  Q23, good approximation for the range [-0.05 0.05]; supported range [-0.5 0.5]
// result: Q21
#ifdef __flexcc2__
static
#pragma inline
#else // __flexcc2__
static inline
#endif // __flexcc2__
Word24 mdrc5b_pow10_coef(Word24 Input)
{
    Word24 Base;
    Word24 tmp;
#ifdef BETTER_POW10_ACCURACY
    Word48 tmpL; // better accuracy
#endif // BETTER_POW10_ACCURACY
    int i;
    int t;

    // 10^X = e^(X*ln(10))
    // e^y = 1 + y + y^2/2

    Base = Input;
    t = 0;

    while(wabssat(Base) > 3643120) // to prevent overflow when multiplying ln10
    {
        Base = wmsr(Base, 1);
        t++;
    }

    tmp  = wfmulr(4828871, Base);                          // ln10 in Q21
    Base = tmp;                                            // Q21
#ifdef BETTER_POW10_ACCURACY
    tmpL = wL_fmul(tmp, tmp);                              // y^2 in Q21xQ21 ==>> Q43
    tmp  = wround_L(wL_msl(tmp, 1));                       // y^2/2 in Q21
    tmp  = waddsat(waddsat(Base, (1 << 21)), tmp);         // Q21
#else // BETTER_POW10_ACCURACY
    tmp = wfmulr(tmp, tmp);                                // Q21xQ21 ==>> Q19
    tmp = waddsat(waddsat(Base, (1 << 21)), wmsl(tmp, 1)); // Q21
#endif // BETTER_POW10_ACCURACY

    // 10^(X/(2m)) = (10^X)^(1/(2m)) = A
    // 10^X = A^(2m) = (A^2)^m
    for(i = 0; i < t; i++)
    {
#ifdef BETTER_POW10_ACCURACY
        tmpL = wL_fmul(tmp, tmp);  // Q21xQ21 ==>> Q43
        tmpL = wL_msl(tmpL, 2);    // Q45
        tmp  = wround_L(tmp);      // Q21
#else // BETTER_POW10_ACCURACY
        tmp = wfmul(tmp, tmp);  // Q21xQ21 ==>> Q19
        tmp = wmsl(tmp, 2);     // Q21
#endif // BETTER_POW10_ACCURACY
    }

    return tmp;
}


extern void mdrc5b_biquad2(MMshort *MemInPtr, int OffsetInc, MMshort *OutPtr, int Samples, const MMshort FILTER_COEF *Filt, MMshort FILTER_MEM *FiltHist);

#endif /* _mdrc5b_dsp_h_ */
