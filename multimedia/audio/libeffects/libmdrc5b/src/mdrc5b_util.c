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
  \file mdrc5b_util.c
  \brief data IO routines
  \author Zong Wenbo
  \email wenbo.zong@st.com
  \date DEC 1,  2009
  \last rev DEC 17, 2009
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
    int     i, ch;
    int     NumMainCh;
    int     BlockSize;
    int     OffsetInc;
    int     Samples;
    MMshort *InPtr;
    Word48  *MemPtr;


    OffsetInc = HeapPtr->DataInOffset;
    Samples   = HeapPtr->InBufSamp;
    BlockSize = MIN(HeapPtr->BlockSize - Samples, HeapPtr->ValidSamples);
    BlockSize = MAX(0, BlockSize);
    NumMainCh = HeapPtr->NumMainCh;
    InPtr     = (MMshort *) HeapPtr->SysMainInPtr;
#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
    for(ch = 0; ch < NumMainCh; ch++)
    {
        MemPtr  = HeapPtr->MainInBuf[ch];
        MemPtr += Samples;
#ifdef __flexcc2__
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
#endif
        for(i = 0; i < BlockSize; i++)
        {
            MemPtr[i] = (Word48) wmsr(*InPtr, HEADROOM);
            InPtr    += OffsetInc;
        }
        InPtr -= BlockSize * OffsetInc;
        InPtr++;            // next channel
    }

    // update pointers
    HeapPtr->ConsumedSamples = BlockSize;
    HeapPtr->InBufSamp      += BlockSize;
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
    int     i, ch;
    int     NumMainCh;
    int     Count;
    int     OutRdIdx;
    int     OffsetInc;
#ifdef __flexcc2__
    MMshort __circ *MemPtr;
#else // __flexcc2__
    MMshort *MemPtr;
#endif // __flexcc2__
    MMshort *OutPtr;


    OffsetInc = HeapPtr->DataOutOffset;
    Count     = HeapPtr->ConsumedSamples;
    NumMainCh = HeapPtr->NumMainCh;
    OutRdIdx  = HeapPtr->OutRdIdx;
    OutPtr    = (MMshort *) HeapPtr->SysMainOutPtr;
#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
    for(ch = 0; ch < NumMainCh; ch++)
    {
#ifdef __flexcc2__
        MemPtr = winit_circ_ptr(HeapPtr->MainOutBuf[ch] + OutRdIdx,
                                HeapPtr->MainOutBuf[ch],
                                HeapPtr->MainOutBuf[ch] + HeapPtr->BlockSize * 2);
#else // __flexcc2__
        MemPtr = HeapPtr->MainOutBuf[ch] + OutRdIdx;
#endif // __flexcc2__

#ifdef __flexcc2__
        #pragma loop maxitercount(MDRC5B_BLOCK_SIZE)
        // Count (=ConsumedSamples) is limited to MDRC5B_BLOCK_SIZE
        // because ConsumedSamples comes from mdrc5b_read() routine (see above)
        // which is equal to BlockSize which is limited to MDRC5B_BLOCK_SIZE
#endif
        for(i = 0; i < Count; i++)
        {
            *OutPtr = wmsl(*MemPtr, HEADROOM);
            OutPtr += OffsetInc;
#ifdef __flexcc2__
            MemPtr++;
#else // __flexcc2__
            MemPtr = mod_add(MemPtr,
                             1,
                             HeapPtr->MainOutBuf[ch] + HeapPtr->BlockSize * 2,
                             HeapPtr->MainOutBuf[ch]);
#endif // __flexcc2__
        }
        OutPtr -= Count * OffsetInc;
        OutPtr++;               // next channel
    }

    // Write globals into heap
    OutRdIdx         += Count;
    HeapPtr->OutRdIdx = CIRC_ADD_H(OutRdIdx, HeapPtr->BlockSize * 2);
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
void mdrc5b_move(MDRC5B_LOCAL_STRUCT_T *HeapPtr)
{
    int     k, ch;
    int     NumMainCh;
    int     OutWtIdx;
#ifdef __flexcc2__
    MMshort __circ * MemOutPtr;
#else //
    MMshort *MemOutPtr;
#endif // __flexcc2__
    Word48  *Ptr;


    NumMainCh = HeapPtr->NumMainCh;
    OutWtIdx  = HeapPtr->OutWtIdx;
#ifdef __flexcc2__
    #pragma loop minitercount(1)
    #pragma loop maxitercount(MDRC5B_MAIN_CH_MAX)
#endif
    for(ch = 0; ch < NumMainCh; ch++)
    {
#ifdef __flexcc2__
        MemOutPtr = winit_circ_ptr(HeapPtr->MainOutBuf[ch] + OutWtIdx,
                                   HeapPtr->MainOutBuf[ch],
                                   HeapPtr->MainOutBuf[ch] + HeapPtr->BlockSize * 2);
#else // __flexcc2__
        MemOutPtr = HeapPtr->MainOutBuf[ch] + OutWtIdx;
#endif // __flexcc2__

        Ptr = HeapPtr->MainInBuf[ch];
        for(k = 0; k < HeapPtr->BlockSize; k++)
        {
            *MemOutPtr = wextract_h(wL_msl(Ptr[k], 24)); // wL_msl does saturation if necessary
#ifdef __flexcc2__
            MemOutPtr++;
#else // __flexcc2__
            MemOutPtr  = mod_add(MemOutPtr,
                                 1,
                                 HeapPtr->MainOutBuf[ch] + HeapPtr->BlockSize * 2,
                                 HeapPtr->MainOutBuf[ch]);
#endif // __flexcc2__
        }
    }

    OutWtIdx         += HeapPtr->BlockSize;
    HeapPtr->OutWtIdx = CIRC_ADD_H(OutWtIdx, HeapPtr->BlockSize * 2);

    HeapPtr->InBufSamp = 0; // clear the counter
}
