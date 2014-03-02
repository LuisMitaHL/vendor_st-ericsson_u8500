/*####################################################################*/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 *
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   spl_util.c
 * \brief
 * \author ST-Ericsson
 */
/**********************************************************************/
/**
  \file spl_util.c
  \brief data IO routines
  \author Zong Wenbo, Matthieu Durnerin
  \email wenbo.zong@st.com
  \date DEC 1,  2009
  \last rev Matthieu Durnerin March 3, 2010
*/
#include "spl_defines.h"
#ifndef M4

#include "audiolibs_common.h"
#include "spl_algo.h"
#include "spl_util.h"

/**********************************************************************/
/*!
  \date     DEC 1, 2009
  \author   ZONG Wenbo
  \brief    read data from system buffer to internal input buffer
  \return   void
  \param    pointer to the heap
*/
/**********************************************************************/
void spl_read(SPL_LOCAL_STRUCT_T * HeapPtr)
{
    int     i, ch, n_ch;
    int     BlockSize;
    int     OffsetInc;
    int     Samples;
    MMshort *InPtr;
    MMshort *MemPtr;


    n_ch      = HeapPtr->NumberChannel;
    OffsetInc = HeapPtr->DataInOffset;
    Samples   = HeapPtr->InBufSamp;
    BlockSize = MIN(HeapPtr->Tuning.BlockSize-Samples, HeapPtr->ValidSamples);
    BlockSize = MAX(0, BlockSize);

    if(BlockSize > 0)
    {
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(SPL_MAIN_CH_MAX)
#endif
        for(ch = 0; ch < n_ch; ch++)
        {
            MemPtr = HeapPtr->MainInBuf[ch] + Samples;
            InPtr  = HeapPtr->SysMainInPtr[ch];

#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for(i = 0; i < BlockSize; i++)
            {
                *MemPtr++ = *InPtr;
                InPtr += OffsetInc;
            }
        }
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
void spl_write(SPL_LOCAL_STRUCT_T * HeapPtr)
{
    int     i, ch, n_ch;
    int     Count;
    int     OutRdIdx;
    int     OffsetInc;
    MMshort *OutPtr;
#ifdef __flexcc2__
    MMshort __circ *MemPtr;
#else //
    MMshort *MemPtr;
#endif // __flexcc2__


    n_ch      = HeapPtr->NumberChannel;
    OffsetInc = HeapPtr->DataOutOffset;
    Count     = HeapPtr->ConsumedSamples;
    OutRdIdx  = HeapPtr->OutRdIdx;

    if(Count > 0)
    {
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(SPL_MAIN_CH_MAX)
#endif
        for(ch = 0; ch < n_ch; ch++)
        {
#ifdef __flexcc2__
            MemPtr = winit_circ_ptr(HeapPtr->MainOutBuf[ch] + OutRdIdx,
                                    HeapPtr->MainOutBuf[ch],
                                    HeapPtr->MainOutBuf[ch] + HeapPtr->Tuning.BlockSize * 2);
#else // __flexcc2__
            MemPtr = HeapPtr->MainOutBuf[ch] + OutRdIdx;
#endif // __flexcc2__
            OutPtr = HeapPtr->SysMainOutPtr[ch];

#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for(i = 0; i < Count; i++)
            {
                *OutPtr = *MemPtr;
                OutPtr += OffsetInc;
#ifdef __flexcc2__
                MemPtr++;
#else // __flexcc2__
                MemPtr = mod_add(MemPtr,
                                 1,
                                 HeapPtr->MainOutBuf[ch] + HeapPtr->Tuning.BlockSize * 2,
                                 HeapPtr->MainOutBuf[ch]);
#endif // __flexcc2__
            }
        }
    }

    // Write globals into heap
    OutRdIdx         += Count;
    HeapPtr->OutRdIdx = CIRC_ADD_H(OutRdIdx, HeapPtr->Tuning.BlockSize * 2);
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
void spl_move(SPL_LOCAL_STRUCT_T * HeapPtr)
{
    int     k, ch, n_ch;
    int     BlockSize;
    int     OutWtIdx;
    MMshort *Ptr;
#ifdef __flexcc2__
    MMshort __circ *MemOutPtr;
#else //
    MMshort *MemOutPtr;
#endif // __flexcc2__


    n_ch      = HeapPtr->NumberChannel;
    BlockSize = HeapPtr->Tuning.BlockSize;
    OutWtIdx  = HeapPtr->OutWtIdx;
    if(BlockSize > 0)
    {
#ifdef __flexcc2__
        #pragma loop minitercount(1)
        #pragma loop maxitercount(SPL_MAIN_CH_MAX)
#endif
        for (ch = 0; ch < n_ch; ch++)
        {
#ifdef __flexcc2__
            MemOutPtr = winit_circ_ptr(HeapPtr->MainOutBuf[ch] + OutWtIdx,
                                       HeapPtr->MainOutBuf[ch],
                                       HeapPtr->MainOutBuf[ch] + HeapPtr->Tuning.BlockSize * 2);
#else // __flexcc2__
            MemOutPtr = HeapPtr->MainOutBuf[ch] + OutWtIdx;
#endif // __flexcc2__
            Ptr = HeapPtr->MainInBuf[ch];


#ifdef __flexcc2__
            #pragma loop minitercount(1)
            #pragma loop maxitercount(SPL_BLOCK_SIZE_MAX)
#endif
            for(k = 0; k < BlockSize; k++)
            {
                *MemOutPtr = *Ptr++;
#ifdef __flexcc2__
                MemOutPtr++;
#else // __flexcc2__
                MemOutPtr  = mod_add(MemOutPtr,
                                     1,
                                     HeapPtr->MainOutBuf[ch] + HeapPtr->Tuning.BlockSize * 2,
                                     HeapPtr->MainOutBuf[ch]);
#endif // __flexcc2__
            }
        }
    }

    OutWtIdx          += BlockSize;
    HeapPtr->OutWtIdx  = CIRC_ADD_H(OutWtIdx, HeapPtr->Tuning.BlockSize * 2);
    HeapPtr->InBufSamp = 0; // clear the counter
}


/**********************************************************************/
/*!
  \date     March 3, 2010
  \author   DURNERIN Matthieu
  \brief    check parameter of type int, check the limit
  \         put val_in in val rounded between min and max, modify also val
  \return   int, error if < 0
*/
/**********************************************************************/
int spl_check_param_int(int *p_val, int *p_val_in, int min, int max) {
   int error = 0;
   *p_val = *p_val_in;
   if (*p_val > max) {
      *p_val = max;
      error = -1;
   }
   if (*p_val < min) {
      *p_val = min;
      error = -1;
   }
   *p_val_in = *p_val;
   return error;
}

/**********************************************************************/
/*!
  \date     March 3, 2010
  \author   DURNERIN Matthieu
  \brief    check parameter of type unsigned int, check the limit
  \         put val_in in val rounded between min and max, modify also val
  \return   int, error if < 0
*/
/**********************************************************************/

int spl_check_param_uint(unsigned int *p_val, unsigned int *p_val_in,
                         unsigned int min, unsigned int max) {
   int error = 0;
   *p_val = *p_val_in;
   if (*p_val > max) {
      *p_val = max;
      error = -1;
   }
   if (*p_val < min) {
      *p_val = min;
      error = -1;
   }
   *p_val_in = *p_val;
   return error;
}
#endif //ifndef M4
