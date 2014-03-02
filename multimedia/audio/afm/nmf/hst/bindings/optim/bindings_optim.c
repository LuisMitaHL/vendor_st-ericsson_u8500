/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   bindings_optim.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "bindings_optim.h"

#include "swap.idt"

#if defined(__arm__) || defined(__arm)
#if defined(__ARM_NEON__) || (__TARGET_ARCH_ARM >= 7)
#define OPTIM_ARM_NEON                  // use NEON vectorization intrinsics
#include <arm_neon.h>
#endif /* __ARM_NEON__ || (__TARGET_ARCH_ARM >= 7) */
#endif /* __arm__ || __arm */



////////////////////
// for pcmadapter //
////////////////////

AFM_API_EXPORT void _stereo2mono_16_optim(void *pBuffer, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= STEREO2MONO_16_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16, *p_out_int16;
        int16x8x2_t  in_16x8x2;
        int16x8_t    out_16x8;
        unsigned int i;

        p_in_int16  = (int16_t *) pBuffer;
        p_out_int16 = (int16_t *) pBuffer;
        for(i = size / 8; i > 0; i--)
        {
            in_16x8x2    = vld2q_s16(p_in_int16);
            p_in_int16  += 16;
            out_16x8     = vhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            vst1q_s16(p_out_int16, out_16x8);
            p_out_int16 += 8;
        }
        i = (size % 8);
        if(i > 0)
        {
            in_16x8x2 = vld2q_s16(p_in_int16);
            out_16x8  = vhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            switch(i)
            {
                case 7:
                    vst1q_lane_s16(p_out_int16 + 6, out_16x8, 6);
                case 6:
                    vst1q_lane_s16(p_out_int16 + 5, out_16x8, 5);
                case 5:
                    vst1q_lane_s16(p_out_int16 + 4, out_16x8, 4);
                case 4:
                    vst1_s16(p_out_int16, vget_low_s16(out_16x8));
                    break;
                case 3:
                    vst1q_lane_s16(p_out_int16 + 2, out_16x8, 2);
                case 2:
                    vst1q_lane_s16(p_out_int16 + 1, out_16x8, 1);
                case 1:
                    vst1q_lane_s16(p_out_int16 + 0, out_16x8, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in_16b_ptr  = (short *) pBuffer;
        short        *out_16b_ptr = (short *) pBuffer;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32          = (int) *in_16b_ptr++;
            tmp32         += (int) *in_16b_ptr++;
            tmp32        >>= 1;
            *out_16b_ptr++ = (short) tmp32;
        }
    }
}


AFM_API_EXPORT void _stereo2mono_16_rnd_optim(void *pBuffer, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= STEREO2MONO_16_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16, *p_out_int16;
        int16x8x2_t  in_16x8x2;
        int16x8_t    out_16x8;
        unsigned int i;

        p_in_int16  = (int16_t *) pBuffer;
        p_out_int16 = (int16_t *) pBuffer;
        for(i = size / 8; i > 0; i--)
        {
            in_16x8x2    = vld2q_s16(p_in_int16);
            p_in_int16  += 16;
            out_16x8     = vrhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            vst1q_s16(p_out_int16, out_16x8);
            p_out_int16 += 8;
        }
        i = (size % 8);
        if(i > 0)
        {
            in_16x8x2 = vld2q_s16(p_in_int16);
            out_16x8  = vrhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            switch(i)
            {
                case 7:
                    vst1q_lane_s16(p_out_int16 + 6, out_16x8, 6);
                case 6:
                    vst1q_lane_s16(p_out_int16 + 5, out_16x8, 5);
                case 5:
                    vst1q_lane_s16(p_out_int16 + 4, out_16x8, 4);
                case 4:
                    vst1_s16(p_out_int16, vget_low_s16(out_16x8));
                    break;
                case 3:
                    vst1q_lane_s16(p_out_int16 + 2, out_16x8, 2);
                case 2:
                    vst1q_lane_s16(p_out_int16 + 1, out_16x8, 1);
                case 1:
                    vst1q_lane_s16(p_out_int16 + 0, out_16x8, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in_16b_ptr  = (short *) pBuffer;
        short        *out_16b_ptr = (short *) pBuffer;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32          = (int) *in_16b_ptr++;
            tmp32         += (int) *in_16b_ptr++;
            tmp32          = (tmp32 + 1) >> 1;
            *out_16b_ptr++ = (short) tmp32;
        }
    }
}


AFM_API_EXPORT void _stereo2mono_32_optim(void *pBuffer, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= STEREO2MONO_32_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32, *p_out_int32;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        unsigned int i;

        p_in_int32  = (int32_t *) pBuffer;
        p_out_int32 = (int32_t *) pBuffer;
        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2    = vld2q_s32(p_in_int32);
            p_in_int32  += 8;
            out_32x4     = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2 = vld2q_s32(p_in_int32);
            out_32x4  = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in_32b_ptr  = (int *) pBuffer;
        int          *out_32b_ptr = (int *) pBuffer;
        long long    tmp64;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp64          = (long long) *in_32b_ptr++;
            tmp64         += (long long) *in_32b_ptr++;
            tmp64        >>= 1;
            *out_32b_ptr++ = (int) tmp64;
        }
    }
}


AFM_API_EXPORT void _stereo2mono_32_rnd_optim(void *pBuffer, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= STEREO2MONO_32_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32, *p_out_int32;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        unsigned int i;

        p_in_int32  = (int32_t *) pBuffer;
        p_out_int32 = (int32_t *) pBuffer;
        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2    = vld2q_s32(p_in_int32);
            p_in_int32  += 8;
            out_32x4     = vrhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2 = vld2q_s32(p_in_int32);
            out_32x4  = vrhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in_32b_ptr  = (int *) pBuffer;
        int          *out_32b_ptr = (int *) pBuffer;
        long long    tmp64;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp64          = (long long) *in_32b_ptr++;
            tmp64         += (long long) *in_32b_ptr++;
            tmp64          = (tmp64 + 1) >> 1;
            *out_32b_ptr++ = (int) tmp64;
        }
    }
}


AFM_API_EXPORT void _mono2stereo_16_optim(void *pBuffer, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= MONO2STEREO_16_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16, *p_out_int16;
        int16x8_t    in_16x8;
        int16x8x2_t  out_16x8x2;
        unsigned int i;

        p_in_int16  = (int16_t *) pBuffer;
        p_out_int16 = (int16_t *) pBuffer;
        i            = (size % 8);
        size        /=  8;
        p_in_int16  +=  8 * size;
        p_out_int16 += 16 * size;
        if(i > 0)
        {
            in_16x8    = vld1q_s16(p_in_int16);
            out_16x8x2 = vzipq_s16(in_16x8, in_16x8);
            switch(i)
            {
                case 7:
                    vst1q_lane_s16(p_out_int16 + 13, out_16x8x2.val[1], 5);
                    vst1q_lane_s16(p_out_int16 + 12, out_16x8x2.val[1], 4);
                case 6:
                    vst1_s16(p_out_int16 + 8, vget_low_s16(out_16x8x2.val[1]));
                    vst1q_s16(p_out_int16, out_16x8x2.val[0]);
                    break;
                case 5:
                    vst1q_lane_s16(p_out_int16 + 9, out_16x8x2.val[1], 1);
                    vst1q_lane_s16(p_out_int16 + 8, out_16x8x2.val[1], 0);
                case 4:
                    vst1q_s16(p_out_int16, out_16x8x2.val[0]);
                    break;
                case 3:
                    vst1q_lane_s16(p_out_int16 + 5, out_16x8x2.val[0], 5);
                    vst1q_lane_s16(p_out_int16 + 4, out_16x8x2.val[0], 4);
                case 2:
                    vst1_s16(p_out_int16, vget_low_s16(out_16x8x2.val[0]));
                    break;
                case 1:
                    vst1q_lane_s16(p_out_int16 + 1, out_16x8x2.val[0], 1);
                    vst1q_lane_s16(p_out_int16 + 0, out_16x8x2.val[0], 0);
                default:
                    break;
            }
        }
        for(i = size; i > 0; i--)
        {
            p_in_int16  -= 8;
            in_16x8      = vld1q_s16(p_in_int16);
            out_16x8x2   = vzipq_s16(in_16x8, in_16x8);
            p_out_int16 -= 8;
            vst1q_s16(p_out_int16, out_16x8x2.val[1]);
            p_out_int16 -= 8;
            vst1q_s16(p_out_int16, out_16x8x2.val[0]);
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in_16b_ptr  = ((short *) pBuffer) +     size - 1;
        short        *out_16b_ptr = ((short *) pBuffer) + 2 * size - 1;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp16          = *in_16b_ptr--;
            *out_16b_ptr-- = tmp16;
            *out_16b_ptr-- = tmp16;
        }
    }
}



AFM_API_EXPORT void _mono2stereo_32_optim(void *pBuffer, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= MONO2STEREO_32_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32, *p_out_int32;
        int32x4_t    in_32x4;
        int32x4x2_t  out_32x4x2;
        unsigned int i;

        p_in_int32  = (int32_t *) pBuffer;
        p_out_int32 = (int32_t *) pBuffer;
        i            = (size % 4);
        size        /= 4;
        p_in_int32  += 4 * size;
        p_out_int32 += 8 * size;
        if(i > 0)
        {
            in_32x4    = vld1q_s32(p_in_int32);
            out_32x4x2 = vzipq_s32(in_32x4, in_32x4);
            switch(i)
            {
                case 3:
                    vst1_s32(p_out_int32 + 4, vget_low_s32(out_32x4x2.val[1]));
                case 2:
                    vst1q_s32(p_out_int32, out_32x4x2.val[0]);
                    break;
                case 1:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4x2.val[0]));
                default:
                    break;
            }
        }
        for(i = size; i > 0; i--)
        {
            p_in_int32  -= 4;
            in_32x4      = vld1q_s32(p_in_int32);
            out_32x4x2   = vzipq_s32(in_32x4, in_32x4);
            p_out_int32 -= 4;
            vst1q_s32(p_out_int32, out_32x4x2.val[1]);
            p_out_int32 -= 4;
            vst1q_s32(p_out_int32, out_32x4x2.val[0]);
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in_32b_ptr  = ((int *) pBuffer) +     size - 1;
        int          *out_32b_ptr = ((int *) pBuffer) + 2 * size - 1;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32          = *in_32b_ptr--;
            *out_32b_ptr-- = tmp32;
            *out_32b_ptr-- = tmp32;
        }
    }
}


AFM_API_EXPORT void _c16to32_optim(void *pBuffer, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= C16TO32_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16;
        int32_t      *p_out_int32;
        int16x4_t    in_16x4;
        int32x4_t    out_32x4;
        unsigned int i;

        p_in_int16   = (int16_t *) pBuffer;
        p_out_int32  = (int32_t *) pBuffer;
        i            = (size % 4);
        size        /= 4;
        p_in_int16  += 4 * size;
        p_out_int32 += 4 * size;
        if(i > 0)
        {
            in_16x4  = vld1_s16(p_in_int16);
            out_32x4 = vmovl_s16(in_16x4);
            out_32x4 = vshlq_n_s32(out_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
        for(i = size; i > 0; i--)
        {
            p_in_int16  -= 4;
            in_16x4      = vld1_s16(p_in_int16);
            out_32x4     = vmovl_s16(in_16x4);
            out_32x4     = vshlq_n_s32(out_32x4, 16);
            p_out_int32 -= 4;
            vst1q_s32(p_out_int32, out_32x4);
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in_16b_ptr  = ((short *) pBuffer) + size - 1;
        int          *out_32b_ptr = ((int   *) pBuffer) + size - 1;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            *out_32b_ptr-- = ((int) *in_16b_ptr--) << 16;
        }
    }
}


AFM_API_EXPORT void _c32to16_optim(void *pBuffer, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= C32TO16_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32;
        int16_t      *p_out_int16;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        p_in_int32  = (int32_t *) pBuffer;
        p_out_int16 = (int16_t *) pBuffer;
        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            out_16x4     = vshrn_n_s32(in_32x4, 16);
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4  = vld1q_s32(p_in_int32);
            out_16x4 = vshrn_n_s32(in_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in_32b_ptr  = (int   *) pBuffer;
        short        *out_16b_ptr = (short *) pBuffer;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32 = *in_32b_ptr++;
            *out_16b_ptr++ = (short) (tmp32 >> 16);
        }
    }
}


AFM_API_EXPORT void _c32to16_rnd_optim(void *pBuffer, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= C32TO16_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32;
        int16_t      *p_out_int16;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        p_in_int32  = (int32_t *) pBuffer;
        p_out_int16 = (int16_t *) pBuffer;
        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            out_16x4     = vqrshrn_n_s32(in_32x4, 16);
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4  = vld1q_s32(p_in_int32);
            out_16x4 = vqrshrn_n_s32(in_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in_32b_ptr  = (int   *) pBuffer;
        short        *out_16b_ptr = (short *) pBuffer;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32 = *in_32b_ptr++;
            if(tmp32 < 0x7FFF0000)
            {
                tmp32 += (1 << 15);
            }
            *out_16b_ptr++ = (short) (tmp32 >> 16);
        }
    }
}


AFM_API_EXPORT void _move_chunk_optim(void *pIn, void *pOut, unsigned int size)
{
#ifdef OPTIM_ARM_NEON
    if(size >= MOVE_CHUNK_NEON_SIZE_THRESHOLD)
    {
        int8_t       *p_in_int8  = (int8_t *) pIn;
        int8_t       *p_out_int8 = (int8_t *) pOut;
        int8x16_t    in_8x16;
        unsigned int i;

        for(i = size / 16; i > 0; i--)
        {
            in_8x16     = vld1q_s8(p_in_int8);
            p_in_int8  += 16;
            vst1q_s8(p_out_int8, in_8x16);
            p_out_int8 += 16;
        }
        i = (size % 16);
        if(i > 0)
        {
            in_8x16 = vld1q_s8(p_in_int8);
            switch(i)
            {
                case 15:
                    vst1q_lane_s8(p_out_int8 + 14, in_8x16, 14);
                case 14:
                    vst1q_lane_s8(p_out_int8 + 13, in_8x16, 13);
                case 13:
                    vst1q_lane_s8(p_out_int8 + 12, in_8x16, 12);
                case 12:
                    vst1q_lane_s8(p_out_int8 + 11, in_8x16, 11);
                case 11:
                    vst1q_lane_s8(p_out_int8 + 10, in_8x16, 10);
                case 10:
                    vst1q_lane_s8(p_out_int8 +  9, in_8x16,  9);
                case 9:
                    vst1q_lane_s8(p_out_int8 +  8, in_8x16,  8);
                case 8:
                    vst1_s8(p_out_int8, vget_low_s8(in_8x16));
                    break;
                case 7:
                    vst1q_lane_s8(p_out_int8 +  6, in_8x16,  6);
                case 6:
                    vst1q_lane_s8(p_out_int8 +  5, in_8x16,  5);
                case 5:
                    vst1q_lane_s8(p_out_int8 +  4, in_8x16,  4);
                case 4:
                    vst1q_lane_s8(p_out_int8 +  3, in_8x16,  3);
                case 3:
                    vst1q_lane_s8(p_out_int8 +  2, in_8x16,  2);
                case 2:
                    vst1q_lane_s8(p_out_int8 +  1, in_8x16,  1);
                case 1:
                    vst1q_lane_s8(p_out_int8 +  0, in_8x16,  0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        char         *p_in  = (char *) pIn;
        char         *p_out = (char *) pOut;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            *p_out++ = *p_in++;
        }
    }
}


////////////////////////
// for shmin & shmout //
////////////////////////

AFM_API_EXPORT void _swap_bytes_optim(void *buffer, unsigned int size, int swap_mode_int)
{
    t_swap_mode swap_mode = (t_swap_mode) swap_mode_int;

#ifdef OPTIM_ARM_NEON
    if(size >= SWAP_BYTES_NEON_SIZE_THRESHOLD)
    {
        unsigned int i;

        if(swap_mode == SWAP_16)
        {
            int8_t    *p_sample_int8 = (int8_t *) buffer;
            int8x16_t sample_8x16;

            size /= 2;
            for(i = size / 8; i > 0; i--)
            {
                sample_8x16    = vld1q_s8(p_sample_int8);
                sample_8x16    = vrev16q_s8(sample_8x16);
                vst1q_s8(p_sample_int8, sample_8x16);
                p_sample_int8 += 16;
            }
            i = (size % 8);
            if(i > 0)
            {
                int16_t   *p_sample_int16 = (int16_t *) p_sample_int8;
                int16x8_t sample_16x8;

                sample_8x16 = vld1q_s8(p_sample_int8);
                sample_8x16 = vrev16q_s8(sample_8x16);
                sample_16x8 = vreinterpretq_s16_s8(sample_8x16);
                switch(i)
                {
                    case 7:
                        vst1q_lane_s16(p_sample_int16 + 6, sample_16x8, 6);
                    case 6:
                        vst1q_lane_s16(p_sample_int16 + 5, sample_16x8, 5);
                    case 5:
                        vst1q_lane_s16(p_sample_int16 + 4, sample_16x8, 4);
                    case 4:
                        vst1_s16(p_sample_int16, vget_low_s16(sample_16x8));
                        break;
                    case 3:
                        vst1q_lane_s16(p_sample_int16 + 2, sample_16x8, 2);
                    case 2:
                        vst1q_lane_s16(p_sample_int16 + 1, sample_16x8, 1);
                    case 1:
                        vst1q_lane_s16(p_sample_int16 + 0, sample_16x8, 0);
                    default:
                        break;
                }
            }
        }
        else if(swap_mode == SWAP_32)
        {
            int8_t    *p_sample_int8 = (int8_t *) buffer;
            int8x16_t sample_8x16;

            size /= 4;
            for(i = size / 4; i > 0; i--)
            {
                sample_8x16    = vld1q_s8(p_sample_int8);
                sample_8x16    = vrev32q_s8(sample_8x16);
                vst1q_s8(p_sample_int8, sample_8x16);
                p_sample_int8 += 16;
            }
            i = (size % 4);
            if(i > 0)
            {
                int32_t   *p_sample_int32 = (int32_t *) p_sample_int8;
                int32x4_t sample_32x4;

                sample_8x16 = vld1q_s8(p_sample_int8);
                sample_8x16 = vrev32q_s8(sample_8x16);
                sample_32x4 = vreinterpretq_s32_s8(sample_8x16);
                switch(i)
                {
                    case 3:
                        vst1q_lane_s32(p_sample_int32 + 2, sample_32x4, 2);
                    case 2:
                        vst1_s32(p_sample_int32, vget_low_s32(sample_32x4));
                        break;
                    case 1:
                        vst1q_lane_s32(p_sample_int32 + 0, sample_32x4, 0);
                    default:
                        break;
                }
            }
        }
        else if(swap_mode == SWAP_32_16)
        {
            int16_t   *p_sample_int16 = (int16_t *) buffer;
            int16x8_t sample_16x8;

            size /= 4;
            for(i = size / 4; i > 0; i--)
            {
                sample_16x8     = vld1q_s16(p_sample_int16);
                sample_16x8     = vrev32q_s16(sample_16x8);
                vst1q_s16(p_sample_int16, sample_16x8);
                p_sample_int16 += 8;
            }
            i = (size % 4);
            if(i > 0)
            {
                int32_t   *p_sample_int32 = (int32_t *) p_sample_int16;
                int32x4_t sample_32x4;

                sample_16x8 = vld1q_s16(p_sample_int16);
                sample_16x8 = vrev32q_s16(sample_16x8);
                sample_32x4 = vreinterpretq_s32_s16(sample_16x8);
                switch(i)
                {
                    case 3:
                        vst1q_lane_s32(p_sample_int32 + 2, sample_32x4, 2);
                    case 2:
                        vst1_s32(p_sample_int32, vget_low_s32(sample_32x4));
                        break;
                    case 1:
                        vst1q_lane_s32(p_sample_int32 + 0, sample_32x4, 0);
                    default:
                        break;
                }
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        unsigned int i;

        if(swap_mode == SWAP_16)
        {
            short *buffer16 = (short *) buffer;
            int   tmp32;

            for(i = size / 2; i > 0; i--)
            {
                tmp32       = (int) *buffer16;
                tmp32       = (tmp32 << 8) | ((tmp32 >> 8) & 0xFF);
                *buffer16++ = (short) tmp32;
            }
        }
        else if(swap_mode == SWAP_32)
        {
            int *buffer32 = (int *) buffer;
            int tmp32;

            for(i = size / 4; i > 0; i--)
            {
                tmp32       = *buffer32;
                tmp32       = (tmp32 << 24) | ((tmp32 & 0xFF00) << 8) | ((tmp32 >> 8) & 0xFF00) | ((tmp32 >> 24) & 0xFF);
                *buffer32++ = tmp32;
            }
        }
        else if(swap_mode == SWAP_32_16)
        {
            int *buffer32 = (int *) buffer;
            int tmp32;

            for(i = size / 4; i > 0; i--)
            {
                tmp32       = *buffer32;
                tmp32       = (tmp32 << 16) | ((tmp32 >> 16) & 0xFFFF);
                *buffer32++ = tmp32;
            }
        }
    }
}


//////////////////////////////
// for shmpcmin & shmpcmout //
//////////////////////////////

AFM_API_EXPORT void _copy_downmix_in16b_out16b_swap_optim(void *in, void *out, unsigned int size)
{
// AABB CCDD -> (BBAA+DDCC)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN16B_OUT16B_SWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int16x8x2_t  in_16x8x2;
        int16x8_t    out_16x8;
        unsigned int i;

        for(i = size / 8; i > 0; i--)
        {
            in_16x8x2        = vld2q_s16(p_in_int16);
            p_in_int16       += 16;
            in_16x8x2.val[0] = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8x2.val[0])));
            in_16x8x2.val[1] = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8x2.val[1])));
            out_16x8         = vhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            vst1q_s16(p_out_int16, out_16x8);
            p_out_int16     += 8;
        }
        i = (size % 8);
        if(i > 0)
        {
            in_16x8x2        = vld2q_s16(p_in_int16);
            in_16x8x2.val[0] = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8x2.val[0])));
            in_16x8x2.val[1] = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8x2.val[1])));
            out_16x8         = vhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            switch(i)
            {
                case 7:
                    vst1q_lane_s16(p_out_int16 + 6, out_16x8, 6);
                case 6:
                    vst1q_lane_s16(p_out_int16 + 5, out_16x8, 5);
                case 5:
                    vst1q_lane_s16(p_out_int16 + 4, out_16x8, 4);
                case 4:
                    vst1_s16(p_out_int16, vget_low_s16(out_16x8));
                    break;
                case 3:
                    vst1q_lane_s16(p_out_int16 + 2, out_16x8, 2);
                case 2:
                    vst1q_lane_s16(p_out_int16 + 1, out_16x8, 1);
                case 1:
                    vst1q_lane_s16(p_out_int16 + 0, out_16x8, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        short        *out16_ptr = (short *) out;
        int          left, right, tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = (int) *in16_ptr++;
            right        = (int) *in16_ptr++;
            left         = ((left  << 24) | ((left  & 0xFF00) << 8)) >> 16;
            right        = ((right << 24) | ((right & 0xFF00) << 8)) >> 16;
            tmp32        = (left + right) >> 1;
            *out16_ptr++ = (short) tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in16b_out16b_swap_rnd_optim(void *in, void *out, unsigned int size)
{
// AABB CCDD -> (BBAA+DDCC+1)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN16B_OUT16B_SWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int16x8x2_t  in_16x8x2;
        int16x8_t    out_16x8;
        unsigned int i;

        for(i = size / 8; i > 0; i--)
        {
            in_16x8x2        = vld2q_s16(p_in_int16);
            p_in_int16       += 16;
            in_16x8x2.val[0] = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8x2.val[0])));
            in_16x8x2.val[1] = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8x2.val[1])));
            out_16x8         = vrhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            vst1q_s16(p_out_int16, out_16x8);
            p_out_int16     += 8;
        }
        i = (size % 8);
        if(i > 0)
        {
            in_16x8x2        = vld2q_s16(p_in_int16);
            in_16x8x2.val[0] = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8x2.val[0])));
            in_16x8x2.val[1] = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8x2.val[1])));
            out_16x8         = vrhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            switch(i)
            {
                case 7:
                    vst1q_lane_s16(p_out_int16 + 6, out_16x8, 6);
                case 6:
                    vst1q_lane_s16(p_out_int16 + 5, out_16x8, 5);
                case 5:
                    vst1q_lane_s16(p_out_int16 + 4, out_16x8, 4);
                case 4:
                    vst1_s16(p_out_int16, vget_low_s16(out_16x8));
                    break;
                case 3:
                    vst1q_lane_s16(p_out_int16 + 2, out_16x8, 2);
                case 2:
                    vst1q_lane_s16(p_out_int16 + 1, out_16x8, 1);
                case 1:
                    vst1q_lane_s16(p_out_int16 + 0, out_16x8, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        short        *out16_ptr = (short *) out;
        int          left, right, tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = (int) *in16_ptr++;
            right        = (int) *in16_ptr++;
            left         = ((left  << 24) | ((left  & 0xFF00) << 8)) >> 16;
            right        = ((right << 24) | ((right & 0xFF00) << 8)) >> 16;
            tmp32        = (left + right + 1) >> 1;
            tmp32        = (tmp32 > 0x7FFF ? 0x7FFF : tmp32);
            *out16_ptr++ = (short) tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in16b_out16b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABB CCDD -> (AABB+CCDD)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN16B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int16x8x2_t  in_16x8x2;
        int16x8_t    out_16x8;
        unsigned int i;

        for(i = size / 8; i > 0; i--)
        {
            in_16x8x2    = vld2q_s16(p_in_int16);
            p_in_int16   += 16;
            out_16x8     = vhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            vst1q_s16(p_out_int16, out_16x8);
            p_out_int16 += 8;
        }
        i = (size % 8);
        if(i > 0)
        {
            in_16x8x2 = vld2q_s16(p_in_int16);
            out_16x8  = vhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            switch(i)
            {
                case 7:
                    vst1q_lane_s16(p_out_int16 + 6, out_16x8, 6);
                case 6:
                    vst1q_lane_s16(p_out_int16 + 5, out_16x8, 5);
                case 5:
                    vst1q_lane_s16(p_out_int16 + 4, out_16x8, 4);
                case 4:
                    vst1_s16(p_out_int16, vget_low_s16(out_16x8));
                    break;
                case 3:
                    vst1q_lane_s16(p_out_int16 + 2, out_16x8, 2);
                case 2:
                    vst1q_lane_s16(p_out_int16 + 1, out_16x8, 1);
                case 1:
                    vst1q_lane_s16(p_out_int16 + 0, out_16x8, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        short        *out16_ptr = (short *) out;
        int          left, right, tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = (int) *in16_ptr++;
            right        = (int) *in16_ptr++;
            tmp32        = (left + right) >> 1;
            *out16_ptr++ = (short) tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in16b_out16b_noswap_rnd_optim(void *in, void *out, unsigned int size)
{
// AABB CCDD -> (AABB+CCDD+1)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN16B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int16x8x2_t  in_16x8x2;
        int16x8_t    out_16x8;
        unsigned int i;

        for(i = size / 8; i > 0; i--)
        {
            in_16x8x2    = vld2q_s16(p_in_int16);
            p_in_int16   += 16;
            out_16x8     = vrhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            vst1q_s16(p_out_int16, out_16x8);
            p_out_int16 += 8;
        }
        i = (size % 8);
        if(i > 0)
        {
            in_16x8x2 = vld2q_s16(p_in_int16);
            out_16x8  = vrhaddq_s16(in_16x8x2.val[0], in_16x8x2.val[1]);
            switch(i)
            {
                case 7:
                    vst1q_lane_s16(p_out_int16 + 6, out_16x8, 6);
                case 6:
                    vst1q_lane_s16(p_out_int16 + 5, out_16x8, 5);
                case 5:
                    vst1q_lane_s16(p_out_int16 + 4, out_16x8, 4);
                case 4:
                    vst1_s16(p_out_int16, vget_low_s16(out_16x8));
                    break;
                case 3:
                    vst1q_lane_s16(p_out_int16 + 2, out_16x8, 2);
                case 2:
                    vst1q_lane_s16(p_out_int16 + 1, out_16x8, 1);
                case 1:
                    vst1q_lane_s16(p_out_int16 + 0, out_16x8, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        short        *out16_ptr = (short *) out;
        int          left, right, tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = (int) *in16_ptr++;
            right        = (int) *in16_ptr++;
            tmp32        = (left + right + 1) >> 1;
            tmp32        = (tmp32 > 0x7FFF ? 0x7FFF : tmp32);
            *out16_ptr++ = (short) tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in16b_out32b_swapin_optim(void *in, void *out, unsigned int size)
{
// AABB CCDD -> (BBAA0000+DDCC0000)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN16B_OUT32B_SWAPIN_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int16x8_t    in_16x8;
        int32x4_t    out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_16x8      = vld1q_s16(p_in_int16);
            p_in_int16  += 8;
            in_16x8      = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8)));
            out_32x4     = vpaddlq_s16(in_16x8);
            out_32x4     = vshlq_n_s32(out_32x4, 15);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32 += 4;

        }
        i = (size % 4);
        if(i > 0)
        {
            in_16x8  = vld1q_s16(p_in_int16);
            in_16x8  = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8)));
            out_32x4 = vpaddlq_s16(in_16x8);
            out_32x4 = vshlq_n_s32(out_32x4, 15);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        int          *out32_ptr = (int   *) out;
        int          left, right;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = (int) *in16_ptr++;
            right        = (int) *in16_ptr++;
            left         = ((left  << 24) | ((left  & 0xFF00) << 8)) >> 1;
            right        = ((right << 24) | ((right & 0xFF00) << 8)) >> 1;
            *out32_ptr++ = left + right;
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in16b_out32b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABB CCDD -> (AABB0000+CCDD0000)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN16B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int16x8_t    in_16x8;
        int32x4_t    out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_16x8      = vld1q_s16(p_in_int16);
            p_in_int16  += 8;
            out_32x4     = vpaddlq_s16(in_16x8);
            out_32x4     = vshlq_n_s32(out_32x4, 15);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_16x8  = vld1q_s16(p_in_int16);
            out_32x4 = vpaddlq_s16(in_16x8);
            out_32x4 = vshlq_n_s32(out_32x4, 15);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        int          *out32_ptr = (int   *) out;
        int          left, right;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = (int) *in16_ptr++;
            right        = (int) *in16_ptr++;
            *out32_ptr++ = (left + right) << 15;
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in32b_out16b_swapin_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD EEFFGGHH -> ((DDCCBBAA+HHGGFFEE)/2)>>16
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN32B_OUT16B_SWAPIN_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2        = vld2q_s32(p_in_int32);
            p_in_int32       += 8;
            in_32x4x2.val[0] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[0])));
            in_32x4x2.val[1] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[1])));
            out_32x4         = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            out_16x4         = vshrn_n_s32(out_32x4, 16);
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16     += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2        = vld2q_s32(p_in_int32);
            in_32x4x2.val[0] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[0])));
            in_32x4x2.val[1] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[1])));
            out_32x4         = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            out_16x4         = vshrn_n_s32(out_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          left, right, tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = *in32_ptr++;
            right        = *in32_ptr++;
            left         = (left  << 24) | ((left  & 0xFF00) << 8) | ((left  >> 8) & 0xFF00) | ((left  >> 24) & 0xFF);
            right        = (right << 24) | ((right & 0xFF00) << 8) | ((right >> 8) & 0xFF00) | ((right >> 24) & 0xFF);
            tmp32        = (left >> 1) + (right >> 1);
            *out16_ptr++ = (short) (tmp32 >> 16);
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in32b_out16b_swapin_rnd_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD EEFFGGHH -> ((DDCCBBAA+HHGGFFEE)/2+2^15)>>16
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN32B_OUT16B_SWAPIN_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2        = vld2q_s32(p_in_int32);
            p_in_int32       += 8;
            in_32x4x2.val[0] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[0])));
            in_32x4x2.val[1] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[1])));
            out_32x4         = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            out_16x4         = vqrshrn_n_s32(out_32x4, 16);
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16     += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2        = vld2q_s32(p_in_int32);
            in_32x4x2.val[0] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[0])));
            in_32x4x2.val[1] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[1])));
            out_32x4         = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            out_16x4         = vqrshrn_n_s32(out_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          left, right, tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = *in32_ptr++;
            right        = *in32_ptr++;
            left         = (left  << 24) | ((left  & 0xFF00) << 8) | ((left  >> 8) & 0xFF00) | ((left  >> 24) & 0xFF);
            right        = (right << 24) | ((right & 0xFF00) << 8) | ((right >> 8) & 0xFF00) | ((right >> 24) & 0xFF);
            tmp32        = (left >> 1) + (right >> 1);
            if(tmp32 < 0x7FFF0000)
            {
                tmp32 += (1 << 15);
            }
            *out16_ptr++ = (short) (tmp32 >> 16);
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in32b_out16b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD EEFFGGHH -> ((AABBCCDD+EEFFGGHH)/2)>>16
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN32B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2    = vld2q_s32(p_in_int32);
            p_in_int32   += 8;
            out_32x4     = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            out_16x4     = vshrn_n_s32(out_32x4, 16);
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2 = vld2q_s32(p_in_int32);
            out_32x4  = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            out_16x4  = vshrn_n_s32(out_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          left, right, tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = *in32_ptr++;
            right        = *in32_ptr++;
            tmp32        = (left >> 1) + (right >> 1);
            *out16_ptr++ = (short) (tmp32 >> 16);
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in32b_out16b_noswap_rnd_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD EEFFGGHH -> ((AABBCCDD+EEFFGGHH)/2+2^15)>>16
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN32B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2    = vld2q_s32(p_in_int32);
            p_in_int32   += 8;
            out_32x4     = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            out_16x4     = vqrshrn_n_s32(out_32x4, 16);
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2 = vld2q_s32(p_in_int32);
            out_32x4  = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            out_16x4  = vqrshrn_n_s32(out_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          left, right, tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = *in32_ptr++;
            right        = *in32_ptr++;
            tmp32        = (left >> 1) + (right >> 1);
            if(tmp32 < 0x7FFF0000)
            {
                tmp32 += (1 << 15);
            }
            *out16_ptr++ = (short) (tmp32 >> 16);
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in32b_out32b_swap_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD EEFFGGHH -> (DDCCBBAA+HHGGFFEE)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN32B_OUT32B_SWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2        = vld2q_s32(p_in_int32);
            p_in_int32       += 8;
            in_32x4x2.val[0] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[0])));
            in_32x4x2.val[1] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[1])));
            out_32x4         = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32     += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2        = vld2q_s32(p_in_int32);
            in_32x4x2.val[0] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[0])));
            in_32x4x2.val[1] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[1])));
            out_32x4         = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int *) in;
        int          *out32_ptr = (int *) out;
        int          left, right;
        long long    tmp64;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = *in32_ptr++;
            right        = *in32_ptr++;
            left         = (left  << 24) | ((left  & 0xFF00) << 8) | ((left  >> 8) & 0xFF00) | ((left  >> 24) & 0xFF);
            right        = (right << 24) | ((right & 0xFF00) << 8) | ((right >> 8) & 0xFF00) | ((right >> 24) & 0xFF);
            tmp64        = (((long long) left) + ((long long) right)) >> 1;
            *out32_ptr++ = (int) tmp64;
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in32b_out32b_swap_rnd_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD EEFFGGHH -> (DDCCBBAA+HHGGFFEE+1)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN32B_OUT32B_SWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2        = vld2q_s32(p_in_int32);
            p_in_int32       += 8;
            in_32x4x2.val[0] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[0])));
            in_32x4x2.val[1] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[1])));
            out_32x4         = vrhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32     += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2        = vld2q_s32(p_in_int32);
            in_32x4x2.val[0] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[0])));
            in_32x4x2.val[1] = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4x2.val[1])));
            out_32x4         = vrhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int *) in;
        int          *out32_ptr = (int *) out;
        int          left, right;
        long long    tmp64;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = *in32_ptr++;
            right        = *in32_ptr++;
            left         = (left  << 24) | ((left  & 0xFF00) << 8) | ((left  >> 8) & 0xFF00) | ((left  >> 24) & 0xFF);
            right        = (right << 24) | ((right & 0xFF00) << 8) | ((right >> 8) & 0xFF00) | ((right >> 24) & 0xFF);
            tmp64        = (((long long) left) + ((long long) right) + 1) >> 1;
            tmp64        = (tmp64 > 0x7FFFFFFFL ? 0x7FFFFFFFL : tmp64);
            *out32_ptr++ = (int) tmp64;
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in32b_out32b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD EEFFGGHH -> (AABBCCDD+EEFFGGHH)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN32B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2    = vld2q_s32(p_in_int32);
            p_in_int32   += 8;
            out_32x4     = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2 = vld2q_s32(p_in_int32);
            out_32x4  = vhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int *) in;
        int          *out32_ptr = (int *) out;
        int          left, right;
        long long    tmp64;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = *in32_ptr++;
            right        = *in32_ptr++;
            tmp64        = (((long long) left) + ((long long) right)) >> 1;
            *out32_ptr++ = (int) tmp64;
        }
    }
}


AFM_API_EXPORT void _copy_downmix_in32b_out32b_noswap_rnd_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD EEFFGGHH -> (AABBCCDD+EEFFGGHH+1)/2
    size /= 2;

#ifdef OPTIM_ARM_NEON
    if(size >= COPY_DOWNMIX_IN32B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int32x4x2_t  in_32x4x2;
        int32x4_t    out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4x2    = vld2q_s32(p_in_int32);
            p_in_int32   += 8;
            out_32x4     = vrhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4x2 = vld2q_s32(p_in_int32);
            out_32x4  = vrhaddq_s32(in_32x4x2.val[0], in_32x4x2.val[1]);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int *) in;
        int          *out32_ptr = (int *) out;
        int          left, right;
        long long    tmp64;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            left         = *in32_ptr++;
            right        = *in32_ptr++;
            tmp64        = (((long long) left) + ((long long) right) + 1) >> 1;
            tmp64        = (tmp64 > 0x7FFFFFFFL ? 0x7FFFFFFFL : tmp64);
            *out32_ptr++ = (int) tmp64;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in16b_out16b_swap_optim(void *in, void *out, unsigned int size)
{
// AABB -> BBAA BBAA
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN16B_OUT16B_SWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int16x8_t    in_16x8;
        int16x8x2_t  out_16x8x2;
        unsigned int i;

        for(i = size / 8; i > 0; i--)
        {
            in_16x8           = vld1q_s16(p_in_int16);
            p_in_int16       += 8;
            in_16x8           = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8)));
            out_16x8x2.val[0] = in_16x8;
            out_16x8x2.val[1] = in_16x8;
            vst2q_s16(p_out_int16, out_16x8x2);
            p_out_int16      += 16;
        }
        i = (size % 8);
        if(i > 0)
        {
            int16x4x2_t out1_16x4x2, out2_16x4x2;

            in_16x8            = vld1q_s16(p_in_int16);
            in_16x8            = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8)));
            out1_16x4x2.val[0] = vget_low_s16 (in_16x8);
            out1_16x4x2.val[1] = vget_low_s16 (in_16x8);
            out2_16x4x2.val[0] = vget_high_s16(in_16x8);
            out2_16x4x2.val[1] = vget_high_s16(in_16x8);
            switch(i)
            {
                case 7:
                    vst2_lane_s16(p_out_int16 + 12, out2_16x4x2, 2);
                case 6:
                    vst2_lane_s16(p_out_int16 + 10, out2_16x4x2, 1);
                case 5:
                    vst2_lane_s16(p_out_int16 +  8, out2_16x4x2, 0);
                case 4:
                    vst2_s16(p_out_int16, out1_16x4x2);
                    break;
                case 3:
                    vst2_lane_s16(p_out_int16 +  4, out1_16x4x2, 2);
                case 2:
                    vst2_lane_s16(p_out_int16 +  2, out1_16x4x2, 1);
                case 1:
                    vst2_lane_s16(p_out_int16 +  0, out1_16x4x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = (int) *in16_ptr++;
            tmp32        = (tmp32 << 24) | ((tmp32 & 0xFF00) << 8);
            tmp16        = (short) (tmp32 >> 16);
            *out16_ptr++ = tmp16;
            *out16_ptr++ = tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in16b_out16b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABB -> AABB AABB
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN16B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int16x8_t    in_16x8;
        int16x8x2_t  out_16x8x2;
        unsigned int i;

        for(i = size / 8; i > 0; i--)
        {
            in_16x8           = vld1q_s16(p_in_int16);
            p_in_int16       += 8;
            out_16x8x2.val[0] = in_16x8;
            out_16x8x2.val[1] = in_16x8;
            vst2q_s16(p_out_int16, out_16x8x2);
            p_out_int16      += 16;
        }
        i = (size % 8);
        if(i > 0)
        {
            int16x4x2_t out1_16x4x2, out2_16x4x2;

            in_16x8            = vld1q_s16(p_in_int16);
            out1_16x4x2.val[0] = vget_low_s16 (in_16x8);
            out1_16x4x2.val[1] = vget_low_s16 (in_16x8);
            out2_16x4x2.val[0] = vget_high_s16(in_16x8);
            out2_16x4x2.val[1] = vget_high_s16(in_16x8);
            switch(i)
            {
                case 7:
                    vst2_lane_s16(p_out_int16 + 12, out2_16x4x2, 2);
                case 6:
                    vst2_lane_s16(p_out_int16 + 10, out2_16x4x2, 1);
                case 5:
                    vst2_lane_s16(p_out_int16 +  8, out2_16x4x2, 0);
                case 4:
                    vst2_s16(p_out_int16, out1_16x4x2);
                    break;
                case 3:
                    vst2_lane_s16(p_out_int16 +  4, out1_16x4x2, 2);
                case 2:
                    vst2_lane_s16(p_out_int16 +  2, out1_16x4x2, 1);
                case 1:
                    vst2_lane_s16(p_out_int16 +  0, out1_16x4x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        short        *out16_ptr = (short *) out;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp16        = *in16_ptr++;
            *out16_ptr++ = tmp16;
            *out16_ptr++ = tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in16b_out32b_swapin_optim(void *in, void *out, unsigned int size)
{
// AABB -> BBAA0000 BBAA0000
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN16B_OUT32B_SWAPIN_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int16x4_t    in_16x4;
        int32x4_t    out_32x4;
        int32x4x2_t  out_32x4x2;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_16x4           = vld1_s16(p_in_int16);
            p_in_int16       += 4;
            in_16x4           = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(in_16x4)));
            out_32x4          = vmovl_s16(in_16x4);
            out_32x4          = vshlq_n_s32(out_32x4, 16);
            out_32x4x2.val[0] = out_32x4;
            out_32x4x2.val[1] = out_32x4;
            vst2q_s32(p_out_int32, out_32x4x2);
            p_out_int32      += 8;
        }
        i = (size % 4);
        if(i > 0)
        {
            int32x2x2_t out1_32x2x2, out2_32x2x2;

            in_16x4            = vld1_s16(p_in_int16);
            in_16x4            = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(in_16x4)));
            out_32x4           = vmovl_s16(in_16x4);
            out_32x4           = vshlq_n_s32(out_32x4, 16);
            out1_32x2x2.val[0] = vget_low_s32 (out_32x4);
            out1_32x2x2.val[1] = vget_low_s32 (out_32x4);
            out2_32x2x2.val[0] = vget_high_s32(out_32x4);
            out2_32x2x2.val[1] = vget_high_s32(out_32x4);
            switch(i)
            {
                case 3:
                    vst2_lane_s32(p_out_int32 + 4, out2_32x2x2, 0);
                case 2:
                    vst2_s32(p_out_int32, out1_32x2x2);
                    break;
                case 1:
                    vst2_lane_s32(p_out_int32 + 0, out1_32x2x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        int          *out32_ptr = (int   *) out;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = (int) *in16_ptr++;
            tmp32        = (tmp32 << 24) | ((tmp32 & 0xFF00) << 8);
            *out32_ptr++ = tmp32;
            *out32_ptr++ = tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in16b_out32b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABB -> AABB0000 AABB0000
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN16B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int16x4_t    in_16x4;
        int32x4_t    out_32x4;
        int32x4x2_t  out_32x4x2;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_16x4           = vld1_s16(p_in_int16);
            p_in_int16       += 4;
            out_32x4          = vmovl_s16(in_16x4);
            out_32x4          = vshlq_n_s32(out_32x4, 16);
            out_32x4x2.val[0] = out_32x4;
            out_32x4x2.val[1] = out_32x4;
            vst2q_s32(p_out_int32, out_32x4x2);
            p_out_int32      += 8;
        }
        i = (size % 4);
        if(i > 0)
        {
            int32x2x2_t out1_32x2x2, out2_32x2x2;

            in_16x4            = vld1_s16(p_in_int16);
            out_32x4           = vmovl_s16(in_16x4);
            out_32x4           = vshlq_n_s32(out_32x4, 16);
            out1_32x2x2.val[0] = vget_low_s32 (out_32x4);
            out1_32x2x2.val[1] = vget_low_s32 (out_32x4);
            out2_32x2x2.val[0] = vget_high_s32(out_32x4);
            out2_32x2x2.val[1] = vget_high_s32(out_32x4);
            switch(i)
            {
                case 3:
                    vst2_lane_s32(p_out_int32 + 4, out2_32x2x2, 0);
                case 2:
                    vst2_s32(p_out_int32, out1_32x2x2);
                    break;
                case 1:
                    vst2_lane_s32(p_out_int32 + 0, out1_32x2x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        int          *out32_ptr = (int   *) out;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = ((int) *in16_ptr++) << 16;
            *out32_ptr++ = tmp32;
            *out32_ptr++ = tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in32b_out16b_swapin_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> DDCC DDCC
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN32B_OUT16B_SWAPIN_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        int16x4x2_t  out_16x4x2;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            p_in_int32       += 4;
            out_16x4          = vmovn_s32(in_32x4);
            out_16x4          = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(out_16x4)));
            out_16x4x2.val[0] = out_16x4;
            out_16x4x2.val[1] = out_16x4;
            vst2_s16(p_out_int16, out_16x4x2);
            p_out_int16      += 8;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            out_16x4          = vmovn_s32(in_32x4);
            out_16x4          = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(out_16x4)));
            out_16x4x2.val[0] = out_16x4;
            out_16x4x2.val[1] = out_16x4;
            switch(i)
            {
                case 3:
                    vst2_lane_s16(p_out_int16 + 4, out_16x4x2, 2);
                case 2:
                    vst2_lane_s16(p_out_int16 + 2, out_16x4x2, 1);
                case 1:
                    vst2_lane_s16(p_out_int16 + 0, out_16x4x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            tmp32        = (tmp32 << 8) | ((tmp32 >> 8) & 0xFF);
            tmp16        = (short) tmp32;
            *out16_ptr++ = tmp16;
            *out16_ptr++ = tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in32b_out16b_swapin_rnd_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> (DDCCBBAA+2^15)>>16 (DDCCBBAA+2^15)>>16
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN32B_OUT16B_SWAPIN_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        int16x4x2_t  out_16x4x2;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            p_in_int32       += 4;
            in_32x4           = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4)));
            out_16x4          = vqrshrn_n_s32(in_32x4, 16);
            out_16x4x2.val[0] = out_16x4;
            out_16x4x2.val[1] = out_16x4;
            vst2_s16(p_out_int16, out_16x4x2);
            p_out_int16      += 8;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            in_32x4           = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4)));
            out_16x4          = vqrshrn_n_s32(in_32x4, 16);
            out_16x4x2.val[0] = out_16x4;
            out_16x4x2.val[1] = out_16x4;
            switch(i)
            {
                case 3:
                    vst2_lane_s16(p_out_int16 + 4, out_16x4x2, 2);
                case 2:
                    vst2_lane_s16(p_out_int16 + 2, out_16x4x2, 1);
                case 1:
                    vst2_lane_s16(p_out_int16 + 0, out_16x4x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            tmp32        = (tmp32 << 24) | ((tmp32 & 0xFF00) << 8) | ((tmp32 >> 8) & 0xFF00) | ((tmp32 >> 24) & 0xFF);
            if(tmp32 < 0x7FFF0000)
            {
                tmp32 += (1 << 15);
            }
            tmp16        = (short) (tmp32 >> 16);
            *out16_ptr++ = tmp16;
            *out16_ptr++ = tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in32b_out16b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> AABB AABB
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN32B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        int16x4x2_t  out_16x4x2;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            p_in_int32       += 4;
            out_16x4          = vshrn_n_s32(in_32x4, 16);
            out_16x4x2.val[0] = out_16x4;
            out_16x4x2.val[1] = out_16x4;
            vst2_s16(p_out_int16, out_16x4x2);
            p_out_int16      += 8;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            out_16x4          = vshrn_n_s32(in_32x4, 16);
            out_16x4x2.val[0] = out_16x4;
            out_16x4x2.val[1] = out_16x4;
            switch(i)
            {
                case 3:
                    vst2_lane_s16(p_out_int16 + 4, out_16x4x2, 2);
                case 2:
                    vst2_lane_s16(p_out_int16 + 2, out_16x4x2, 1);
                case 1:
                    vst2_lane_s16(p_out_int16 + 0, out_16x4x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            tmp16        = (short) (tmp32 >> 16);
            *out16_ptr++ = tmp16;
            *out16_ptr++ = tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in32b_out16b_noswap_rnd_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> (AABBCCDD+2^15)>>16 (AABBCCDD+2^15)>>16
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN32B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        int16x4x2_t  out_16x4x2;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            p_in_int32       += 4;
            out_16x4          = vqrshrn_n_s32(in_32x4, 16);
            out_16x4x2.val[0] = out_16x4;
            out_16x4x2.val[1] = out_16x4;
            vst2_s16(p_out_int16, out_16x4x2);
            p_out_int16      += 8;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            out_16x4          = vqrshrn_n_s32(in_32x4, 16);
            out_16x4x2.val[0] = out_16x4;
            out_16x4x2.val[1] = out_16x4;
            switch(i)
            {
                case 3:
                    vst2_lane_s16(p_out_int16 + 4, out_16x4x2, 2);
                case 2:
                    vst2_lane_s16(p_out_int16 + 2, out_16x4x2, 1);
                case 1:
                    vst2_lane_s16(p_out_int16 + 0, out_16x4x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            if(tmp32 < 0x7FFF0000)
            {
                tmp32 += (1 << 15);
            }
            tmp16        = (short) (tmp32 >> 16);
            *out16_ptr++ = tmp16;
            *out16_ptr++ = tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in32b_out32b_swap_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> DDCCBBAA DDCCBBAA
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN32B_OUT32B_SWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int32x4_t    in_32x4;
        int32x4x2_t  out_32x4x2;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            p_in_int32       += 4;
            in_32x4           = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4)));
            out_32x4x2.val[0] = in_32x4;
            out_32x4x2.val[1] = in_32x4;
            vst2q_s32(p_out_int32, out_32x4x2);
            p_out_int32      += 8;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            in_32x4           = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4)));
            out_32x4x2.val[0] = in_32x4;
            out_32x4x2.val[1] = in_32x4;
            switch(i)
            {
                case 3:
                    vst2q_lane_s32(p_out_int32 + 4, out_32x4x2, 2);
                case 2:
                    vst2q_lane_s32(p_out_int32 + 2, out_32x4x2, 1);
                case 1:
                    vst2q_lane_s32(p_out_int32 + 0, out_32x4x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int *) in;
        int          *out32_ptr = (int *) out;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            tmp32        = (tmp32 << 24) | ((tmp32 & 0xFF00) << 8) | ((tmp32 >> 8) & 0xFF00) | ((tmp32 >> 24) & 0xFF);
            *out32_ptr++ = tmp32;
            *out32_ptr++ = tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_upmix_in32b_out32b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> AABBCCDD AABBCCDD
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_UPMIX_IN32B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int32x4_t    in_32x4;
        int32x4x2_t  out_32x4x2;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            p_in_int32       += 4;
            out_32x4x2.val[0] = in_32x4;
            out_32x4x2.val[1] = in_32x4;
            vst2q_s32(p_out_int32, out_32x4x2);
            p_out_int32      += 8;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4           = vld1q_s32(p_in_int32);
            out_32x4x2.val[0] = in_32x4;
            out_32x4x2.val[1] = in_32x4;
            switch(i)
            {
                case 3:
                    vst2q_lane_s32(p_out_int32 + 4, out_32x4x2, 2);
                case 2:
                    vst2q_lane_s32(p_out_int32 + 2, out_32x4x2, 1);
                case 1:
                    vst2q_lane_s32(p_out_int32 + 0, out_32x4x2, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int *) in;
        int          *out32_ptr = (int *) out;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            *out32_ptr++ = tmp32;
            *out32_ptr++ = tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_in16b_out16b_swap_optim(void *in, void *out, unsigned int size)
{
// AABB -> BBAA
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN16B_OUT16B_SWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int16x8_t    in_16x8;
        int16x8_t    out_16x8;
        unsigned int i;

        for(i = size / 8; i > 0; i--)
        {
            in_16x8      = vld1q_s16(p_in_int16);
            p_in_int16  += 8;
            out_16x8     = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8)));
            vst1q_s16(p_out_int16, out_16x8);
            p_out_int16 += 8;
        }
        i = (size % 8);
        if(i > 0)
        {
            in_16x8  = vld1q_s16(p_in_int16);
            out_16x8 = vreinterpretq_s16_s8(vrev16q_s8(vreinterpretq_s8_s16(in_16x8)));
            switch(i)
            {
                case 7:
                    vst1q_lane_s16(p_out_int16 + 6, out_16x8, 6);
                case 6:
                    vst1q_lane_s16(p_out_int16 + 5, out_16x8, 5);
                case 5:
                    vst1q_lane_s16(p_out_int16 + 4, out_16x8, 4);
                case 4:
                    vst1_s16(p_out_int16, vget_low_s16(out_16x8));
                    break;
                case 3:
                    vst1q_lane_s16(p_out_int16 + 2, out_16x8, 2);
                case 2:
                    vst1q_lane_s16(p_out_int16 + 1, out_16x8, 1);
                case 1:
                    vst1q_lane_s16(p_out_int16 + 0, out_16x8, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = (int) *in16_ptr++;
            tmp32        = (tmp32 << 24) | ((tmp32 & 0xFF00) << 8);
            tmp16        = (short) (tmp32 >> 16);
            *out16_ptr++ = tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_in16b_out16b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABB -> AABB
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN16B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int16x8_t    in_16x8;
        unsigned int i;

        for(i = size / 8; i > 0; i--)
        {
            in_16x8      = vld1q_s16(p_in_int16);
            p_in_int16  += 8;
            vst1q_s16(p_out_int16, in_16x8);
            p_out_int16 += 8;
        }
        i = (size % 8);
        if(i > 0)
        {
            in_16x8 = vld1q_s16(p_in_int16);
            switch(i)
            {
                case 7:
                    vst1q_lane_s16(p_out_int16 + 6, in_16x8, 6);
                case 6:
                    vst1q_lane_s16(p_out_int16 + 5, in_16x8, 5);
                case 5:
                    vst1q_lane_s16(p_out_int16 + 4, in_16x8, 4);
                case 4:
                    vst1_s16(p_out_int16, vget_low_s16(in_16x8));
                    break;
                case 3:
                    vst1q_lane_s16(p_out_int16 + 2, in_16x8, 2);
                case 2:
                    vst1q_lane_s16(p_out_int16 + 1, in_16x8, 1);
                case 1:
                    vst1q_lane_s16(p_out_int16 + 0, in_16x8, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        short        *out16_ptr = (short *) out;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            *out16_ptr++ = *in16_ptr++;
        }
    }
}


AFM_API_EXPORT void _copy_in16b_out32b_swapin_optim(void *in, void *out, unsigned int size)
{
// AABB -> BBAA0000
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN16B_OUT32B_SWAPIN_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int16x4_t    in_16x4;
        int32x4_t    out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_16x4      = vld1_s16(p_in_int16);
            p_in_int16  += 4;
            in_16x4      = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(in_16x4)));
            out_32x4     = vmovl_s16(in_16x4);
            out_32x4     = vshlq_n_s32(out_32x4, 16);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_16x4  = vld1_s16(p_in_int16);
            in_16x4  = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(in_16x4)));
            out_32x4 = vmovl_s16(in_16x4);
            out_32x4 = vshlq_n_s32(out_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        int          *out32_ptr = (int   *) out;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = (int) *in16_ptr++;
            tmp32        = (tmp32 << 24) | ((tmp32 & 0xFF00) << 8);
            *out32_ptr++ = tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_in16b_out32b_swapout_optim(void *in, void *out, unsigned int size)
{
// AABB -> 0000BBAA
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN16B_OUT32B_SWAPOUT_NEON_SIZE_THRESHOLD)
    {
        uint16_t     *p_in_int16  = (uint16_t *) in;
        uint32_t     *p_out_int32 = (uint32_t *) out;
        uint16x4_t   in_16x4;
        uint32x4_t   out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_16x4      = vld1_u16(p_in_int16);
            p_in_int16  += 4;
            in_16x4      = vreinterpret_u16_u8(vrev16_u8(vreinterpret_u8_u16(in_16x4)));
            out_32x4     = vmovl_u16(in_16x4);
            vst1q_u32(p_out_int32, out_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_16x4  = vld1_u16(p_in_int16);
            in_16x4  = vreinterpret_u16_u8(vrev16_u8(vreinterpret_u8_u16(in_16x4)));
            out_32x4 = vmovl_u16(in_16x4);
            switch(i)
            {
                case 3:
                    vst1q_lane_u32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_u32(p_out_int32, vget_low_u32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_u32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        unsigned short *in16_ptr  = (unsigned short *) in;
        unsigned int   *out32_ptr = (unsigned int   *) out;
        unsigned short tmp16;
        unsigned int   i;

        for(i = size; i > 0; i--)
        {
            tmp16        = *in16_ptr++;
            tmp16        = (tmp16 << 8) | (tmp16 >> 8);
            *out32_ptr++ = (unsigned int) tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_in16b_out32b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABB -> AABB0000
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN16B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int16_t      *p_in_int16  = (int16_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int16x4_t    in_16x4;
        int32x4_t    out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_16x4      = vld1_s16(p_in_int16);
            p_in_int16  += 4;
            out_32x4     = vmovl_s16(in_16x4);
            out_32x4     = vshlq_n_s32(out_32x4, 16);
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_16x4  = vld1_s16(p_in_int16);
            out_32x4 = vmovl_s16(in_16x4);
            out_32x4 = vshlq_n_s32(out_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        short        *in16_ptr  = (short *) in;
        int          *out32_ptr = (int   *) out;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            *out32_ptr++ = ((int) *in16_ptr++) << 16;
        }
    }
}


AFM_API_EXPORT void _copy_in32b_out16b_swapin_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> DDCC
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN32B_OUT16B_SWAPIN_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            out_16x4     = vmovn_s32(in_32x4);
            out_16x4     = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(out_16x4)));
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4  = vld1q_s32(p_in_int32);
            out_16x4 = vmovn_s32(in_32x4);
            out_16x4 = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(out_16x4)));
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            tmp32        = (tmp32 << 8) | ((tmp32 >> 8) & 0xFF);
            *out16_ptr++ = (short) tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_in32b_out16b_swapin_rnd_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> (DDCCBBAA+2^15)>>16
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN32B_OUT16B_SWAPIN_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            in_32x4      = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4)));
            out_16x4     = vqrshrn_n_s32(in_32x4, 16);
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4  = vld1q_s32(p_in_int32);
            in_32x4  = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4)));
            out_16x4 = vqrshrn_n_s32(in_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            tmp32        = (tmp32 << 24) | ((tmp32 & 0xFF00) << 8) | ((tmp32 >> 8) & 0xFF00) | ((tmp32 >> 24) & 0xFF);
            if(tmp32 < 0x7FFF0000)
            {
                tmp32 += (1 << 15);
            }
            *out16_ptr++ = (short) (tmp32 >> 16);
        }
    }
}


AFM_API_EXPORT void _copy_in32b_out16b_swapout_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> BBAA
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN32B_OUT16B_SWAPOUT_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            out_16x4     = vshrn_n_s32(in_32x4, 16);
            out_16x4     = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(out_16x4)));
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4  = vld1q_s32(p_in_int32);
            out_16x4 = vshrn_n_s32(in_32x4, 16);
            out_16x4 = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(out_16x4)));
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int            *in32_ptr  = (int   *) in;
        short          *out16_ptr = (short *) out;
        int            tmp32;
        unsigned short tmp16;
        unsigned int   i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            tmp16        = (unsigned short) (tmp32 >> 16);
            tmp16        = (tmp16 << 8) | (tmp16 >> 8);
            *out16_ptr++ = (short) tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_in32b_out16b_swapout_rnd_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> FFEE with EEFF=(AABBCCDD+2^15)>>16
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN32B_OUT16B_SWAPOUT_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            out_16x4     = vqrshrn_n_s32(in_32x4, 16);
            out_16x4     = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(out_16x4)));
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4  = vld1q_s32(p_in_int32);
            out_16x4 = vqrshrn_n_s32(in_32x4, 16);
            out_16x4 = vreinterpret_s16_s8(vrev16_s8(vreinterpret_s8_s16(out_16x4)));
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int            *in32_ptr  = (int   *) in;
        short          *out16_ptr = (short *) out;
        int            tmp32;
        unsigned short tmp16;
        unsigned int   i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            if(tmp32 < 0x7FFF0000)
            {
                tmp32 += (1 << 15);
            }
            tmp16        = (unsigned short) (tmp32 >> 16);
            tmp16        = (tmp16 << 8) | (tmp16 >> 8);
            *out16_ptr++ = (short) tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_in32b_out16b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> AABB
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN32B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            out_16x4     = vshrn_n_s32(in_32x4, 16);
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4  = vld1q_s32(p_in_int32);
            out_16x4 = vshrn_n_s32(in_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            tmp16        = (short) (tmp32 >> 16);
            *out16_ptr++ = tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_in32b_out16b_noswap_rnd_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> (AABBCCDD+2^15)>>16
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN32B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int16_t      *p_out_int16 = (int16_t *) out;
        int32x4_t    in_32x4;
        int16x4_t    out_16x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            out_16x4     = vqrshrn_n_s32(in_32x4, 16);
            vst1_s16(p_out_int16, out_16x4);
            p_out_int16 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4  = vld1q_s32(p_in_int32);
            out_16x4 = vqrshrn_n_s32(in_32x4, 16);
            switch(i)
            {
                case 3:
                    vst1_lane_s16(p_out_int16 + 2, out_16x4, 2);
                case 2:
                    vst1_lane_s16(p_out_int16 + 1, out_16x4, 1);
                case 1:
                    vst1_lane_s16(p_out_int16 + 0, out_16x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int   *) in;
        short        *out16_ptr = (short *) out;
        int          tmp32;
        short        tmp16;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            if(tmp32 < 0x7FFF0000)
            {
                tmp32 += (1 << 15);
            }
            tmp16        = (short) (tmp32 >> 16);
            *out16_ptr++ = tmp16;
        }
    }
}


AFM_API_EXPORT void _copy_in32b_out32b_swap_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> DDCCBBAA
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN32B_OUT32B_SWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int32x4_t    in_32x4;
        int32x4_t    out_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            out_32x4     = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4)));
            vst1q_s32(p_out_int32, out_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4  = vld1q_s32(p_in_int32);
            out_32x4 = vreinterpretq_s32_s8(vrev32q_s8(vreinterpretq_s8_s32(in_32x4)));
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, out_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(out_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, out_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int *) in;
        int          *out32_ptr = (int *) out;
        int          tmp32;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            tmp32        = *in32_ptr++;
            tmp32        = (tmp32 << 24) | ((tmp32 & 0xFF00) << 8) | ((tmp32 >> 8) & 0xFF00) | ((tmp32 >> 24) & 0xFF);
            *out32_ptr++ = tmp32;
        }
    }
}


AFM_API_EXPORT void _copy_in32b_out32b_noswap_optim(void *in, void *out, unsigned int size)
{
// AABBCCDD -> AABBCCDD
#ifdef OPTIM_ARM_NEON
    if(size >= COPY_IN32B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD)
    {
        int32_t      *p_in_int32  = (int32_t *) in;
        int32_t      *p_out_int32 = (int32_t *) out;
        int32x4_t    in_32x4;
        unsigned int i;

        for(i = size / 4; i > 0; i--)
        {
            in_32x4      = vld1q_s32(p_in_int32);
            p_in_int32  += 4;
            vst1q_s32(p_out_int32, in_32x4);
            p_out_int32 += 4;
        }
        i = (size % 4);
        if(i > 0)
        {
            in_32x4 = vld1q_s32(p_in_int32);
            switch(i)
            {
                case 3:
                    vst1q_lane_s32(p_out_int32 + 2, in_32x4, 2);
                case 2:
                    vst1_s32(p_out_int32, vget_low_s32(in_32x4));
                    break;
                case 1:
                    vst1q_lane_s32(p_out_int32 + 0, in_32x4, 0);
                default:
                    break;
            }
        }
    }
    else
#endif /* OPTIM_ARM_NEON */
    {
        int          *in32_ptr  = (int *) in;
        int          *out32_ptr = (int *) out;
        unsigned int i;

        for(i = size; i > 0; i--)
        {
            *out32_ptr++ = *in32_ptr++;
        }
    }
}
