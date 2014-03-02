/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
File: vector_ms_CA9.c 

Description:
------------
The file contains Mid-Side stereo processing for Cortex A9 and Neon.

Flags used:
1. ARM        : contains implementation for fixed point for CA9.
2. NEON       : contains vector fixed point implementation for Neon
                    ( intrinsics/cmodels code are seperated using VECT flag)
*******************************************************************************/

#ifdef ARM

#include "vector.h"
static const int EXTERN one_over_sqrt2 = 1518500224; 

#ifdef NEON

void vector_ms_neon(int *m, int * s, int len)
{
  unsigned int i, new_loop_cnt;
  int          mval ;
  int          sval ;
  int          temp,temp2 ,r;
  int          add_temp,sub_temp;

  int *src,*dst;   

  temp  = one_over_sqrt2;
  temp2 = one_over_sqrt2;

  src = m;
  dst = s;
  new_loop_cnt = (unsigned int)(((unsigned int)len>>2) -1 );

  r = len&3;
  if (r != len)
  {       
    int32x4_t L128_tmp1,L128_tmp2,L128_coef,L128_sum;
    int32x4_t L128_tmp11,L128_tmp22,L128_dif;

    L128_coef =vdupq_n_s32(temp);   

    L128_tmp1 =  vld1q_s32(src);
    L128_tmp2 =  vld1q_s32(dst); 

    for(i= new_loop_cnt ;i > 0; i--)
    {
      L128_sum  = vaddq_s32(L128_tmp1,L128_tmp2);
      L128_dif  = vsubq_s32(L128_tmp1,L128_tmp2); 

      L128_tmp11 = vqdmulhq_s32(L128_sum, L128_coef);
      L128_tmp22 = vqdmulhq_s32(L128_dif, L128_coef);

      L128_tmp1 =  vld1q_s32(src+4);
      L128_tmp2 =  vld1q_s32(dst+4);        

      vst1q_s32(src, L128_tmp11);src+=4;  
      vst1q_s32(dst, L128_tmp22);dst+=4;

    }

    L128_sum  = vaddq_s32(L128_tmp1,L128_tmp2);
    L128_dif  = vsubq_s32(L128_tmp1,L128_tmp2); 

    L128_tmp11 = vqdmulhq_s32(L128_sum, L128_coef);
    L128_tmp22 = vqdmulhq_s32(L128_dif, L128_coef);
    vst1q_s32(src, L128_tmp11);src+=4;  
    vst1q_s32(dst, L128_tmp22);dst+=4;

  }
  for(i=r ;i >0 ; i--)
  {
    mval = *src;
    sval = *dst;

    add_temp=mval + sval;
    sub_temp=mval - sval;

    *src++ = (int)( ((long long)add_temp * temp) >> 32)<<1;
    *dst++ = (int)( ((long long)sub_temp * temp2) >> 32)<<1;
  }

  return;
}   /* end vector_ms_simd() */

#elif (defined VECT)

void vector_ms_vect(int *m, int * s, int len)
{
  unsigned int i, new_loop_cnt;
  int          mval ;
  int          sval ;
  int          temp,temp2 ,r;
  int          add_temp,sub_temp;

  int          *src,*dst;   

  temp  = one_over_sqrt2;
  temp2 = one_over_sqrt2;

  src = m;
  dst = s;
  new_loop_cnt = (unsigned int)(((unsigned int)len>>2) -1 );

  r = len&3;
  if (r != len)
  {       
    int L128_tmp1[4],L128_tmp2[4],L128_coef[4],L128_sum[4],L128_dif[4];
    int L128_tmp11[4],L128_tmp22[4];

    L128_coef[0] =  temp;
    L128_coef[1] =  temp;
    L128_coef[2] =  temp;
    L128_coef[3] =  temp;

    L128_tmp1[0] =  src[0];
    L128_tmp1[1] =  src[1];
    L128_tmp1[2] =  src[2];
    L128_tmp1[3] =  src[3];

    L128_tmp2[0] =  dst[0];
    L128_tmp2[1] =  dst[1];
    L128_tmp2[2] =  dst[2];
    L128_tmp2[3] =  dst[3];

    for(i= new_loop_cnt ;i > 0; i--)
    {  
      L128_sum[0] = L128_tmp1[0] + L128_tmp2[0];
      L128_sum[1] = L128_tmp1[1] + L128_tmp2[1];
      L128_sum[2] = L128_tmp1[2] + L128_tmp2[2];
      L128_sum[3] = L128_tmp1[3] + L128_tmp2[3];

      L128_dif[0] = L128_tmp1[0] - L128_tmp2[0];
      L128_dif[1] = L128_tmp1[1] - L128_tmp2[1];
      L128_dif[2] = L128_tmp1[2] - L128_tmp2[2];
      L128_dif[3] = L128_tmp1[3] - L128_tmp2[3];

      L128_tmp11[0] = builtin_mpf_32(L128_sum[0],L128_coef[0]);
      L128_tmp11[1] = builtin_mpf_32(L128_sum[1],L128_coef[1]);
      L128_tmp11[2] = builtin_mpf_32(L128_sum[2],L128_coef[2]);
      L128_tmp11[3] = builtin_mpf_32(L128_sum[3],L128_coef[3]);

      L128_tmp22[0] = builtin_mpf_32(L128_dif[0],L128_coef[0]);
      L128_tmp22[1] = builtin_mpf_32(L128_dif[1],L128_coef[1]);
      L128_tmp22[2] = builtin_mpf_32(L128_dif[2],L128_coef[2]);
      L128_tmp22[3] = builtin_mpf_32(L128_dif[3],L128_coef[3]);

      L128_tmp1[0] =  src[4];
      L128_tmp1[1] =  src[5];
      L128_tmp1[2] =  src[6];
      L128_tmp1[3] =  src[7];

      L128_tmp2[0] =  dst[4];
      L128_tmp2[1] =  dst[5];
      L128_tmp2[2] =  dst[6];
      L128_tmp2[3] =  dst[7];

      src[0] = L128_tmp11[0];
      src[1] = L128_tmp11[1];
      src[2] = L128_tmp11[2];
      src[3] = L128_tmp11[3];src+=4;

      dst[0] = L128_tmp22[0];
      dst[1] = L128_tmp22[1];
      dst[2] = L128_tmp22[2];
      dst[3] = L128_tmp22[3];dst+=4;

    }

    L128_sum[0] = L128_tmp1[0] + L128_tmp2[0];
    L128_sum[1] = L128_tmp1[1] + L128_tmp2[1];
    L128_sum[2] = L128_tmp1[2] + L128_tmp2[2];
    L128_sum[3] = L128_tmp1[3] + L128_tmp2[3];

    L128_dif[0] = L128_tmp1[0] - L128_tmp2[0];
    L128_dif[1] = L128_tmp1[1] - L128_tmp2[1];
    L128_dif[2] = L128_tmp1[2] - L128_tmp2[2];
    L128_dif[3] = L128_tmp1[3] - L128_tmp2[3];

    L128_tmp11[0] = builtin_mpf_32(L128_sum[0],L128_coef[0]);
    L128_tmp11[1] = builtin_mpf_32(L128_sum[1],L128_coef[1]);
    L128_tmp11[2] = builtin_mpf_32(L128_sum[2],L128_coef[2]);
    L128_tmp11[3] = builtin_mpf_32(L128_sum[3],L128_coef[3]);

    L128_tmp22[0] = builtin_mpf_32(L128_dif[0],L128_coef[0]);
    L128_tmp22[1] = builtin_mpf_32(L128_dif[1],L128_coef[1]);
    L128_tmp22[2] = builtin_mpf_32(L128_dif[2],L128_coef[2]);
    L128_tmp22[3] = builtin_mpf_32(L128_dif[3],L128_coef[3]);

    src[0] = L128_tmp11[0];
    src[1] = L128_tmp11[1];
    src[2] = L128_tmp11[2];
    src[3] = L128_tmp11[3];src+=4;

    dst[0] = L128_tmp22[0];
    dst[1] = L128_tmp22[1];
    dst[2] = L128_tmp22[2];
    dst[3] = L128_tmp22[3];dst+=4;
  }

  for(i=r ;i >0 ; i--)
  {
    mval = *src;
    sval = *dst;

    add_temp=mval + sval;
    sub_temp=mval - sval;

    *src++ = (int)( ((long long)add_temp * temp) >> 32)<<1;
    *dst++ = (int)( ((long long)sub_temp * temp2) >> 32)<<1;
  }

  return;

}   /* end vector_ms_simd() */

#else

void vector_ms_arm(int *m, int *s, int len)
{
  unsigned int i;
  int mval ;
  int sval ;
  int temp,temp2 ;
  int add_temp,sub_temp;

  temp = one_over_sqrt2;
  temp2 = one_over_sqrt2;

  for(i=len;i>0;i--)
  {
    mval = *m;
    sval = *s;
    add_temp=mval + sval;
    sub_temp=mval - sval;

    *m++ = (int)( ((long long)add_temp * temp) >> 31);
    *s++ = (int)( ((long long)sub_temp * temp2) >> 31);
  }

  return;
}   /* end vector_ms() */

#endif /* NEON */ 
#endif /* ARM */

