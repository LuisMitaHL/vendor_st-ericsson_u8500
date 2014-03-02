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
  \file mdrc5b_util_CA9.c
  \brief data IO routines
  \author Zong Wenbo,  Champsaur Ludovic
  \email wenbo.zong@st.com, ludovic.champsaur@stericsson.com
  \date DEC 1,  2009
  \last rev DEC 17, 2009
  \last rev NEON DEC 2, 2011
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

#include "mdrc5b_util.h"


/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    read data from system buffer to internal input buffer
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
void mdrc5b_read(MDRC5B_LOCAL_STRUCT_T *HeapPtr)
{
    unsigned int i;
    unsigned int NumMainCh;
    unsigned int BlockSize;
    unsigned int Samples;

    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_READ)

    Samples   = (unsigned int) HeapPtr->InBufSamp;
    BlockSize = (unsigned int) HeapPtr->BlockSize;
    if(BlockSize > Samples)
    {
        BlockSize = BlockSize - Samples;
        if(BlockSize > (unsigned int) HeapPtr->ValidSamples)
        {
            BlockSize = (unsigned int) HeapPtr->ValidSamples;
        }
    }
    else
    {
        return;
    }
    NumMainCh = (unsigned int) HeapPtr->NumMainCh;

    if(HeapPtr->sample_bit_size == 32)
    {
        int32_t   *InPtr = (int32_t *) HeapPtr->SysMainInPtr;
        int64x2_t shift_64x2;

#ifdef SAMPLES_24_BITS
        shift_64x2 = vdupq_n_s64(-(HEADROOM + 8));  // because up to now, arm version works on 24 bits samples
#else // SAMPLES_24_BITS
        shift_64x2 = vdupq_n_s64(-HEADROOM);        // with 32 bits samples
#endif // SAMPLES_24_BITS
        if(NumMainCh == 1)
        {
            int64_t   *MemPtr;
            int32x2_t sample_32x2;
            int64x2_t sample_64x2;

            MemPtr = &HeapPtr->MainInBuf[0][Samples];
            for(i = (BlockSize >> 1); i > 0; i--)
            {
                sample_32x2 = vld1_s32(InPtr);
                InPtr      += 2;
                sample_64x2 = vmovl_s32(sample_32x2);
                sample_64x2 = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_s64(MemPtr, sample_64x2);
                MemPtr     += 2;
            }
            if(BlockSize & 1)
            {
                sample_32x2 = vld1_s32(InPtr);
                sample_64x2 = vmovl_s32(sample_32x2);
                sample_64x2 = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_lane_s64(MemPtr, sample_64x2, 0);
            }
        }
        else if(NumMainCh == 2)
        {
            int64_t     *MemPtrL;
            int64_t     *MemPtrR;
            int32x2x2_t sample_32x2x2;
            int64x2_t   sample_64x2;

            MemPtrL = &HeapPtr->MainInBuf[0][Samples];
            MemPtrR = &HeapPtr->MainInBuf[1][Samples];
            for(i = (BlockSize >> 1); i > 0; i--)
            {
                sample_32x2x2 = vld2_s32(InPtr);
                InPtr        += 4;
                sample_64x2   = vmovl_s32(sample_32x2x2.val[0]);
                sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_s64(MemPtrL, sample_64x2);
                MemPtrL      += 2;
                sample_64x2   = vmovl_s32(sample_32x2x2.val[1]);
                sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_s64(MemPtrR, sample_64x2);
                MemPtrR      += 2;
            }
            if(BlockSize & 1)
            {
                sample_32x2x2 = vld2_s32(InPtr);
                sample_64x2   = vmovl_s32(sample_32x2x2.val[0]);
                sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_lane_s64(MemPtrL, sample_64x2, 0);
                sample_64x2   = vmovl_s32(sample_32x2x2.val[1]);
                sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_lane_s64(MemPtrR, sample_64x2, 0);
            }
        }
        else
        {
            unsigned int OffsetInc = (unsigned int) HeapPtr->DataInOffset;
            unsigned int ch;
            int64_t      *MemPtr;

            for(ch = 0; ch < NumMainCh; ch++)
            {
                MemPtr  = HeapPtr->MainInBuf[ch];
                MemPtr += Samples;

                for(i = 0; i < BlockSize; i++)
                {
#ifdef SAMPLES_24_BITS
                    MemPtr[i] = (int64_t) (*InPtr >> (HEADROOM + 8));   // because up to now, arm version works on 24 bits samples
#else // SAMPLES_24_BITS
                    MemPtr[i] = (int64_t) (*InPtr >> HEADROOM);         // with 32 bits samples
#endif // SAMPLES_24_BITS
                    InPtr    += OffsetInc;
                }
                InPtr -= BlockSize * OffsetInc;
                InPtr++;            // next channel
            }
        }
    }
    else
    {
        int16_t   *InPtr = (int16_t *) HeapPtr->SysMainInPtr;
        int64x2_t shift_64x2;

#ifdef SAMPLES_24_BITS
        shift_64x2 = vdupq_n_s64(-(HEADROOM + 7));  // because up to now, arm version works on 24 bits samples
#else // SAMPLES_24_BITS
        shift_64x2 = vdupq_n_s64(-(HEADROOM - 1));  // with 32 bits samples
#endif // SAMPLES_24_BITS
        if(NumMainCh == 1)
        {
            int64_t   *MemPtr;
            int16x4_t sample_16x4;
            int32x4_t sample_32x4;
            int32x2_t sample_32x2;
            int64x2_t sample_64x2;

            MemPtr = &HeapPtr->MainInBuf[0][Samples];
            for(i = (BlockSize >> 2); i > 0; i--)
            {
                sample_16x4 = vld1_s16(InPtr);
                InPtr      += 4;
                sample_32x4 = vshll_n_s16(sample_16x4, 15);
                sample_32x2 = vget_low_s32(sample_32x4);
                sample_64x2 = vmovl_s32(sample_32x2);
                sample_64x2 = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_s64(MemPtr, sample_64x2);
                MemPtr     += 2;
                sample_32x2 = vget_high_s32(sample_32x4);
                sample_64x2 = vmovl_s32(sample_32x2);
                sample_64x2 = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_s64(MemPtr, sample_64x2);
                MemPtr     += 2;
            }
            i = (BlockSize & 3);
            if(i > 0)
            {
                sample_16x4 = vld1_s16(InPtr);
                sample_32x4 = vshll_n_s16(sample_16x4, 15);
                sample_32x2 = vget_low_s32(sample_32x4);
                sample_64x2 = vmovl_s32(sample_32x2);
                sample_64x2 = vqrshlq_s64(sample_64x2, shift_64x2);   
                switch(i)
                {
                    case 1:
                        vst1q_lane_s64(MemPtr, sample_64x2, 0);
                        break;
                    case 2:
                        vst1q_s64(MemPtr, sample_64x2);
                        break;
                    case 3:
                        vst1q_s64(MemPtr, sample_64x2);
                        MemPtr     += 2;
                        sample_32x2 = vget_high_s32(sample_32x4);
                        sample_64x2 = vmovl_s32(sample_32x2);
                        sample_64x2 = vqrshlq_s64(sample_64x2, shift_64x2);   
                        vst1q_lane_s64(MemPtr, sample_64x2, 0);
                        break;
                    default:
                        break;
                }
            }
        }
        else if(NumMainCh == 2)
        {
            int64_t     *MemPtrL;
            int64_t     *MemPtrR;
            int16x4x2_t sample_16x4x2;
            int32x4_t   sample_32x4;
            int32x2_t   sample_32x2;
            int64x2_t   sample_64x2;

            MemPtrL = &HeapPtr->MainInBuf[0][Samples];
            MemPtrR = &HeapPtr->MainInBuf[1][Samples];
            for(i = (BlockSize >> 2); i > 0; i--)
            {
                sample_16x4x2 = vld2_s16(InPtr);
                InPtr        += 8;
                sample_32x4   = vshll_n_s16(sample_16x4x2.val[0], 15);
                sample_32x2   = vget_low_s32(sample_32x4);
                sample_64x2   = vmovl_s32(sample_32x2);
                sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_s64(MemPtrL, sample_64x2);
                MemPtrL      += 2;
                sample_32x2   = vget_high_s32(sample_32x4);
                sample_64x2   = vmovl_s32(sample_32x2);
                sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_s64(MemPtrL, sample_64x2);
                MemPtrL      += 2;
                sample_32x4   = vshll_n_s16(sample_16x4x2.val[1], 15);
                sample_32x2   = vget_low_s32(sample_32x4);
                sample_64x2   = vmovl_s32(sample_32x2);
                sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_s64(MemPtrR, sample_64x2);
                MemPtrR      += 2;
                sample_32x2   = vget_high_s32(sample_32x4);
                sample_64x2   = vmovl_s32(sample_32x2);
                sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                vst1q_s64(MemPtrR, sample_64x2);
                MemPtrR      += 2;
            }
            i = (BlockSize & 3);
            if(i > 0)
            {
                sample_16x4x2 = vld2_s16(InPtr);
                switch(i)
                {
                    case 1:
                        sample_32x4   = vshll_n_s16(sample_16x4x2.val[0], 15);
                        sample_32x2   = vget_low_s32(sample_32x4);
                        sample_64x2   = vmovl_s32(sample_32x2);
                        sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                        vst1q_lane_s64(MemPtrL, sample_64x2, 0);
                        sample_32x4   = vshll_n_s16(sample_16x4x2.val[1], 15);
                        sample_32x2   = vget_low_s32(sample_32x4);
                        sample_64x2   = vmovl_s32(sample_32x2);
                        sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                        vst1q_lane_s64(MemPtrR, sample_64x2, 0);
                        break;
                    case 2:
                        sample_32x4   = vshll_n_s16(sample_16x4x2.val[0], 15);
                        sample_32x2   = vget_low_s32(sample_32x4);
                        sample_64x2   = vmovl_s32(sample_32x2);
                        sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                        vst1q_s64(MemPtrL, sample_64x2);
                        sample_32x4   = vshll_n_s16(sample_16x4x2.val[1], 15);
                        sample_32x2   = vget_low_s32(sample_32x4);
                        sample_64x2   = vmovl_s32(sample_32x2);
                        sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                        vst1q_s64(MemPtrR, sample_64x2);
                        break;
                    case 3:
                        sample_32x4   = vshll_n_s16(sample_16x4x2.val[0], 15);
                        sample_32x2   = vget_low_s32(sample_32x4);
                        sample_64x2   = vmovl_s32(sample_32x2);
                        sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                        vst1q_s64(MemPtrL, sample_64x2);
                        MemPtrL      += 2;
                        sample_32x2   = vget_high_s32(sample_32x4);
                        sample_64x2   = vmovl_s32(sample_32x2);
                        sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                        vst1q_lane_s64(MemPtrL, sample_64x2, 0);
                        sample_32x4   = vshll_n_s16(sample_16x4x2.val[1], 15);
                        sample_32x2   = vget_low_s32(sample_32x4);
                        sample_64x2   = vmovl_s32(sample_32x2);
                        sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                        vst1q_s64(MemPtrR, sample_64x2);
                        MemPtrR      += 2;
                        sample_32x2   = vget_high_s32(sample_32x4);
                        sample_64x2   = vmovl_s32(sample_32x2);
                        sample_64x2   = vqrshlq_s64(sample_64x2, shift_64x2);   
                        vst1q_lane_s64(MemPtrR, sample_64x2, 0);
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
            unsigned int OffsetInc = (unsigned int) HeapPtr->DataInOffset;
            unsigned int ch;
            int          sample;
            int64_t      *MemPtr;

            for(ch = 0; ch < NumMainCh; ch++)
            {
                MemPtr  = HeapPtr->MainInBuf[ch];
                MemPtr += Samples;

                for(i = 0; i < BlockSize; i++)
                {
                    sample    = (((int) *InPtr) << 16);
#ifdef SAMPLES_24_BITS
                    MemPtr[i] = (int64_t) (sample >> (HEADROOM + 8));   // because up to now, arm version works on 24 bits samples
#else // SAMPLES_24_BITS
                    MemPtr[i] = (int64_t) (sample >> HEADROOM);         // with 32 bits samples
#endif // SAMPLES_24_BITS
                    InPtr    += OffsetInc;
                }
                InPtr -= BlockSize * OffsetInc;
                InPtr++;            // next channel
            }
        }
    }

    // update pointers
    HeapPtr->ConsumedSamples = BlockSize;
    HeapPtr->InBufSamp      += BlockSize;

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_READ)
}


/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    read data from internal output buffer to system buffer
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
void mdrc5b_write(MDRC5B_LOCAL_STRUCT_T *HeapPtr)
{
    unsigned int i, ch;
    unsigned int NumMainCh;
    unsigned int Count, Count1, Count2;
    unsigned int OutRdIdx;
    unsigned int OffsetInc;
    unsigned int BlockSize;
    int32x4_t    shift_32x4;

    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_WRITE)

#ifdef SAMPLES_24_BITS
    shift_32x4 = vdupq_n_s32(HEADROOM + 8); // because up to now, arm version works on 24 bits samples
#else // SAMPLES_24_BITS
    shift_32x4 = vdupq_n_s32(HEADROOM);     // with 32 bits samples
#endif // SAMPLES_24_BITS


    OffsetInc = (unsigned int) HeapPtr->DataOutOffset;
    Count     = (unsigned int) HeapPtr->ConsumedSamples;
    NumMainCh = (unsigned int) HeapPtr->NumMainCh;
    OutRdIdx  = (unsigned int) HeapPtr->OutRdIdx;
    BlockSize = (unsigned int) HeapPtr->BlockSize;

    if(OutRdIdx + Count >= BlockSize * 2)
    {
        Count1             = BlockSize * 2 - OutRdIdx;
        Count2             = Count - Count1;
        HeapPtr->OutRdIdx  = (int) Count2;
    }
    else
    {
        Count1             = Count;
        Count2             = 0;
        HeapPtr->OutRdIdx += (int) Count;
    }

    if(HeapPtr->sample_bit_size == 32)
    {
        int32_t *OutPtr = (int32_t *) HeapPtr->SysMainOutPtr;

        if(NumMainCh == 1)
        {
            int32x4_t sample_32x4;
            int32_t   *MemPtr;

            MemPtr = HeapPtr->MainOutBuf[0] + OutRdIdx;
            for(i = (Count1 >> 2); i > 0; i--)
            {
                sample_32x4 = vld1q_s32(MemPtr);
                MemPtr     += 4;
                sample_32x4 = vqrshlq_s32(sample_32x4, shift_32x4);
                vst1q_s32(OutPtr, sample_32x4);
                OutPtr    += 4;
            }
            i = (Count1 & 3);
            if(i > 0)
            {
                sample_32x4 = vld1q_s32(MemPtr);
                sample_32x4 = vqrshlq_s32(sample_32x4, shift_32x4);
                switch(i)
                {
                    case 3:
                        vst1q_lane_s32(OutPtr + 2, sample_32x4, 2);
                    case 2:
                        vst1_s32(OutPtr, vget_low_s32(sample_32x4));
                        break;
                    case 1:
                        vst1q_lane_s32(OutPtr + 0, sample_32x4, 0);
                    default:
                        break;
                }
                OutPtr += i;
            }
            MemPtr = HeapPtr->MainOutBuf[0];
            for(i = (Count2 >> 2); i > 0; i--)
            {
                sample_32x4 = vld1q_s32(MemPtr);
                MemPtr     += 4;
                sample_32x4 = vqrshlq_s32(sample_32x4, shift_32x4);
                vst1q_s32(OutPtr, sample_32x4);
                OutPtr    += 4;
            }
            i = (Count2 & 3);
            if(i > 0)
            {
                sample_32x4 = vld1q_s32(MemPtr);
                sample_32x4 = vqrshlq_s32(sample_32x4, shift_32x4);
                switch(i)
                {
                    case 3:
                        vst1q_lane_s32(OutPtr + 2, sample_32x4, 2);
                    case 2:
                        vst1_s32(OutPtr, vget_low_s32(sample_32x4));
                        break;
                    case 1:
                        vst1q_lane_s32(OutPtr + 0, sample_32x4, 0);
                    default:
                        break;
                }
            }
        }
        else if(NumMainCh == 2)
        {
            int32x4x2_t sample_32x4x2;
            int32_t     *MemPtrL, *MemPtrR;

            MemPtrL = HeapPtr->MainOutBuf[0] + OutRdIdx;
            MemPtrR = HeapPtr->MainOutBuf[1] + OutRdIdx;
            for(i = (Count1 >> 2); i > 0; i--)
            {
                sample_32x4x2.val[0] = vld1q_s32(MemPtrL);
                MemPtrL             += 4;
                sample_32x4x2.val[0] = vqrshlq_s32(sample_32x4x2.val[0], shift_32x4);
                sample_32x4x2.val[1] = vld1q_s32(MemPtrR);
                MemPtrR             += 4;
                sample_32x4x2.val[1] = vqrshlq_s32(sample_32x4x2.val[1], shift_32x4);
                vst2q_s32(OutPtr, sample_32x4x2);
                OutPtr              += 8;
            }
            i = (Count1 & 3);
            if(i > 0)
            {
                sample_32x4x2.val[0] = vld1q_s32(MemPtrL);
                sample_32x4x2.val[0] = vqrshlq_s32(sample_32x4x2.val[0], shift_32x4);
                sample_32x4x2.val[1] = vld1q_s32(MemPtrR);
                sample_32x4x2.val[1] = vqrshlq_s32(sample_32x4x2.val[1], shift_32x4);
                switch(i)
                {
                    case 3:
                        vst2q_lane_s32(OutPtr + 4, sample_32x4x2, 2);
                    case 2:
                        vst2q_lane_s32(OutPtr + 2, sample_32x4x2, 1);
                    case 1:
                        vst2q_lane_s32(OutPtr + 0, sample_32x4x2, 0);
                    default:
                        break;
                }
                OutPtr += i * 2;
            }
            MemPtrL = HeapPtr->MainOutBuf[0];
            MemPtrR = HeapPtr->MainOutBuf[1];
            for(i = (Count2 >> 2); i > 0; i--)
            {
                sample_32x4x2.val[0] = vld1q_s32(MemPtrL);
                MemPtrL             += 4;
                sample_32x4x2.val[0] = vqrshlq_s32(sample_32x4x2.val[0], shift_32x4);
                sample_32x4x2.val[1] = vld1q_s32(MemPtrR);
                MemPtrR             += 4;
                sample_32x4x2.val[1] = vqrshlq_s32(sample_32x4x2.val[1], shift_32x4);
                vst2q_s32(OutPtr, sample_32x4x2);
                OutPtr              += 8;
            }
            i = (Count2 & 3);
            if(i > 0)
            {
                sample_32x4x2.val[0] = vld1q_s32(MemPtrL);
                sample_32x4x2.val[0] = vqrshlq_s32(sample_32x4x2.val[0], shift_32x4);
                sample_32x4x2.val[1] = vld1q_s32(MemPtrR);
                sample_32x4x2.val[1] = vqrshlq_s32(sample_32x4x2.val[1], shift_32x4);
                switch(i)
                {
                    case 3:
                        vst2q_lane_s32(OutPtr + 4, sample_32x4x2, 2);
                    case 2:
                        vst2q_lane_s32(OutPtr + 2, sample_32x4x2, 1);
                    case 1:
                        vst2q_lane_s32(OutPtr + 0, sample_32x4x2, 0);
                    default:
                        break;
                }
            }
        }
        else
        {
            for(ch = 0; ch < NumMainCh; ch++)
            {
                int32_t *MemPtr = HeapPtr->MainOutBuf[ch] + OutRdIdx;

                for(i = Count1; i > 0; i--)
                {
#ifdef SAMPLES_24_BITS
                    *OutPtr = (*MemPtr << (HEADROOM + 8));  // because up to now, arm version works on 24 bits samples
#else // SAMPLES_24_BITS
                    *OutPtr = (*MemPtr << HEADROOM);        // with 32 bits samples
#endif // SAMPLES_24_BITS
                    MemPtr++;
                    OutPtr += OffsetInc;
                }
                MemPtr = HeapPtr->MainOutBuf[ch];
                for(i = Count2; i > 0; i--)
                {
#ifdef SAMPLES_24_BITS
                    *OutPtr = (*MemPtr << (HEADROOM + 8));  // because up to now, arm version works on 24 bits samples
#else // SAMPLES_24_BITS
                    *OutPtr = (*MemPtr << HEADROOM);        // with 32 bits samples
#endif // SAMPLES_24_BITS
                    MemPtr++;
                    OutPtr += OffsetInc;
                }
                OutPtr -= Count * OffsetInc;
                OutPtr++;           // next channel
            }
        }
    }
    else
    {
        int16_t *OutPtr = (int16_t *) HeapPtr->SysMainOutPtr;

        if(NumMainCh == 1)
        {
            int32x4_t sample_32x4;
            int16x4_t sample_16x4;
            int32_t   *MemPtr;

            MemPtr = HeapPtr->MainOutBuf[0] + OutRdIdx;
            for(i = (Count1 >> 2); i > 0; i--)
            {
                sample_32x4 = vld1q_s32(MemPtr);
                MemPtr     += 4;
                sample_32x4 = vqrshlq_s32(sample_32x4, shift_32x4);
                sample_16x4 = vrshrn_n_s32(sample_32x4, 16);
                vst1_s16(OutPtr, sample_16x4);
                OutPtr    += 4;
            }
            i = (Count1 & 3);
            if(i > 0)
            {
                sample_32x4 = vld1q_s32(MemPtr);
                sample_32x4 = vqrshlq_s32(sample_32x4, shift_32x4);
                sample_16x4 = vrshrn_n_s32(sample_32x4, 16);
                switch(i)
                {
                    case 3:
                        vst1_lane_s16(OutPtr + 2, sample_16x4, 2);
                    case 2:
                        vst1_lane_s16(OutPtr + 1, sample_16x4, 1);
                    case 1:
                        vst1_lane_s16(OutPtr + 0, sample_16x4, 0);
                    default:
                        break;
                }
                OutPtr += i;
            }
            MemPtr = HeapPtr->MainOutBuf[0];
            for(i = (Count2 >> 2); i > 0; i--)
            {
                sample_32x4 = vld1q_s32(MemPtr);
                MemPtr     += 4;
                sample_32x4 = vqrshlq_s32(sample_32x4, shift_32x4);
                sample_16x4 = vrshrn_n_s32(sample_32x4, 16);
                vst1_s16(OutPtr, sample_16x4);
                OutPtr    += 4;
            }
            i = (Count2 & 3);
            if(i > 0)
            {
                sample_32x4 = vld1q_s32(MemPtr);
                sample_32x4 = vqrshlq_s32(sample_32x4, shift_32x4);
                sample_16x4 = vrshrn_n_s32(sample_32x4, 16);
                switch(i)
                {
                    case 3:
                        vst1_lane_s16(OutPtr + 2, sample_16x4, 2);
                    case 2:
                        vst1_lane_s16(OutPtr + 1, sample_16x4, 1);
                    case 1:
                        vst1_lane_s16(OutPtr + 0, sample_16x4, 0);
                    default:
                        break;
                }
            }
        }
        else if(NumMainCh == 2)
        {
            int32x4x2_t sample_32x4x2;
            int16x4x2_t sample_16x4x2;
            int32_t     *MemPtrL, *MemPtrR;

            MemPtrL = HeapPtr->MainOutBuf[0] + OutRdIdx;
            MemPtrR = HeapPtr->MainOutBuf[1] + OutRdIdx;
            for(i = (Count1 >> 2); i > 0; i--)
            {
                sample_32x4x2.val[0] = vld1q_s32(MemPtrL);
                MemPtrL             += 4;
                sample_32x4x2.val[0] = vqrshlq_s32(sample_32x4x2.val[0], shift_32x4);
                sample_16x4x2.val[0] = vrshrn_n_s32(sample_32x4x2.val[0], 16);
                sample_32x4x2.val[1] = vld1q_s32(MemPtrR);
                MemPtrR             += 4;
                sample_32x4x2.val[1] = vqrshlq_s32(sample_32x4x2.val[1], shift_32x4);
                sample_16x4x2.val[1] = vrshrn_n_s32(sample_32x4x2.val[1], 16);
                vst2_s16(OutPtr, sample_16x4x2);
                OutPtr              += 8;
            }
            i = (Count1 & 3);
            if(i > 0)
            {
                sample_32x4x2.val[0] = vld1q_s32(MemPtrL);
                sample_32x4x2.val[0] = vqrshlq_s32(sample_32x4x2.val[0], shift_32x4);
                sample_16x4x2.val[0] = vrshrn_n_s32(sample_32x4x2.val[0], 16);
                sample_32x4x2.val[1] = vld1q_s32(MemPtrR);
                sample_32x4x2.val[1] = vqrshlq_s32(sample_32x4x2.val[1], shift_32x4);
                sample_16x4x2.val[1] = vrshrn_n_s32(sample_32x4x2.val[1], 16);
                switch(i)
                {
                    case 3:
                        vst2_lane_s16(OutPtr + 4, sample_16x4x2, 2);
                    case 2:
                        vst2_lane_s16(OutPtr + 2, sample_16x4x2, 1);
                    case 1:
                        vst2_lane_s16(OutPtr + 0, sample_16x4x2, 0);
                    default:
                        break;
                }
                OutPtr += i * 2;
            }
            MemPtrL = HeapPtr->MainOutBuf[0];
            MemPtrR = HeapPtr->MainOutBuf[1];
            for(i = (Count2 >> 2); i > 0; i--)
            {
                sample_32x4x2.val[0] = vld1q_s32(MemPtrL);
                MemPtrL             += 4;
                sample_32x4x2.val[0] = vqrshlq_s32(sample_32x4x2.val[0], shift_32x4);
                sample_16x4x2.val[0] = vrshrn_n_s32(sample_32x4x2.val[0], 16);
                sample_32x4x2.val[1] = vld1q_s32(MemPtrR);
                MemPtrR             += 4;
                sample_32x4x2.val[1] = vqrshlq_s32(sample_32x4x2.val[1], shift_32x4);
                sample_16x4x2.val[1] = vrshrn_n_s32(sample_32x4x2.val[1], 16);
                vst2_s16(OutPtr, sample_16x4x2);
                OutPtr              += 8;
            }
            i = (Count2 & 3);
            if(i > 0)
            {
                sample_32x4x2.val[0] = vld1q_s32(MemPtrL);
                sample_32x4x2.val[0] = vqrshlq_s32(sample_32x4x2.val[0], shift_32x4);
                sample_16x4x2.val[0] = vrshrn_n_s32(sample_32x4x2.val[0], 16);
                sample_32x4x2.val[1] = vld1q_s32(MemPtrR);
                sample_32x4x2.val[1] = vqrshlq_s32(sample_32x4x2.val[1], shift_32x4);
                sample_16x4x2.val[1] = vrshrn_n_s32(sample_32x4x2.val[1], 16);
                switch(i)
                {
                    case 3:
                        vst2_lane_s16(OutPtr + 4, sample_16x4x2, 2);
                    case 2:
                        vst2_lane_s16(OutPtr + 2, sample_16x4x2, 1);
                    case 1:
                        vst2_lane_s16(OutPtr + 0, sample_16x4x2, 0);
                    default:
                        break;
                }
            }
        }
        else
        {
            int32_t sample;

            for(ch = 0; ch < NumMainCh; ch++)
            {
                int32_t *MemPtr = HeapPtr->MainOutBuf[ch] + OutRdIdx;

                for(i = Count1; i > 0; i--)
                {
#ifdef SAMPLES_24_BITS
                    sample = (*MemPtr << (HEADROOM + 8)); // because up to now, arm version works on 24 bits samples
#else // SAMPLES_24_BITS
                    sample = (*MemPtr << HEADROOM);       // with 32 bits samples
#endif // SAMPLES_24_BITS
                    MemPtr++;
                    *OutPtr = (int16_t) ((sample + (sample < 0x7FFF8000 ? (1 << 15) : 0)) >> 16);
                    OutPtr += OffsetInc;
                }
                MemPtr = HeapPtr->MainOutBuf[ch];
                for(i = Count2; i > 0; i--)
                {
#ifdef SAMPLES_24_BITS
                    sample = (*MemPtr << (HEADROOM + 8)); // because up to now, arm version works on 24 bits samples
#else // SAMPLES_24_BITS
                    sample = (*MemPtr << HEADROOM);       // with 32 bits samples
#endif // SAMPLES_24_BITS
                    MemPtr++;
                    *OutPtr = (int16_t) ((sample + (sample < 0x7FFF8000 ? (1 << 15) : 0)) >> 16);
                    OutPtr += OffsetInc;
                }
                OutPtr -= Count * OffsetInc;
                OutPtr++;           // next channel
            }
        }
    }

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_WRITE)
}


/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    read data from internal input buffer to internal output buffer
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
static void __forceinline mdrc5b_move_inline(int64_t *p_in64, int32_t *p_out32, unsigned int Count)
{
    unsigned int i;
    int64x2_t    sample_64x2;
    int32x2_t    sample_32x2;

    for(i = (Count >> 1); i > 0; i--)
    {
        sample_64x2 = vld1q_s64(p_in64);
        p_in64     += 2;
#ifdef SAMPLES_24_BITS
        sample_64x2 = vqshlq_n_s64(sample_64x2, 40);    // because up to now, arm version works on 24 bits samples
        sample_64x2 = vshrq_n_s64 (sample_64x2,  8);    //
#else // SAMPLES_24_BITS
        sample_64x2 = vqshlq_n_s64(sample_64x2, 32);    // with 32 bits samples
#endif // SAMPLES_24_BITS
        sample_32x2 = vrshrn_n_s64(sample_64x2, 32);
        vst1_s32(p_out32, sample_32x2);
        p_out32    += 2;
    }
    if(Count & 1)
    {
        sample_64x2 = vld1q_s64(p_in64);
#ifdef SAMPLES_24_BITS
        sample_64x2 = vqshlq_n_s64(sample_64x2, 40);    // because up to now, arm version works on 24 bits samples
        sample_64x2 = vshrq_n_s64 (sample_64x2,  8);    //
#else // SAMPLES_24_BITS
        sample_64x2 = vqshlq_n_s64(sample_64x2, 32);    // with 32 bits samples
#endif // SAMPLES_24_BITS
        sample_32x2 = vrshrn_n_s64(sample_64x2, 32);
        vst1_lane_s32(p_out32, sample_32x2, 0);
    }
}

void mdrc5b_move(MDRC5B_LOCAL_STRUCT_T *HeapPtr)
{
    unsigned int ch;
    unsigned int NumMainCh;
    unsigned int OutWtIdx;
    unsigned int BlockSize;
    unsigned int Count1, Count2;


    START_PMU_MEASURE(PMU_MEASURE_MRDC5B_MOVE)

    NumMainCh = (unsigned int) HeapPtr->NumMainCh;
    OutWtIdx  = (unsigned int) HeapPtr->OutWtIdx;
    BlockSize = (unsigned int) HeapPtr->BlockSize;

    if(OutWtIdx >= BlockSize)
    {
        Count1             = BlockSize * 2 - OutWtIdx;
        Count2             = BlockSize - Count1;
        HeapPtr->OutWtIdx  = (int) Count2;
    }
    else
    {
        Count1             = BlockSize;
        Count2             = 0;
        HeapPtr->OutWtIdx += (int) Count1;
    }

    for(ch = 0; ch < NumMainCh; ch++)
    {
        mdrc5b_move_inline(HeapPtr->MainInBuf[ch],          HeapPtr->MainOutBuf[ch] + OutWtIdx, Count1);
        mdrc5b_move_inline(HeapPtr->MainInBuf[ch] + Count1, HeapPtr->MainOutBuf[ch],            Count2);
    }

    HeapPtr->InBufSamp = 0; // clear the counter

    STOP_PMU_MEASURE(PMU_MEASURE_MRDC5B_MOVE)
}
