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
  \file spl_util_M4.c
  \brief data IO routines
  \author Zong Wenbo, Matthieu Durnerin
  \email wenbo.zong@st.com
  \date DEC 1,  2009
  \last rev Matthieu Durnerin March 3, 2010
  */
#include "spl_defines.h"
#ifdef M4
#include "audiolibs_common.h"
#include "spl_algo_M4.h"
#include "spl_util_M4.h"
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
	int *InPtr;
	int *MemPtr;
	n_ch      = HeapPtr->NumberChannel;
	OffsetInc = HeapPtr->DataInOffset;
	Samples   = HeapPtr->InBufSamp;
	BlockSize = MIN(HeapPtr->Tuning.BlockSize-Samples, HeapPtr->ValidSamples);
	BlockSize = MAX(0, BlockSize);
	if(BlockSize > 0)
	{
		for(ch = 0; ch < n_ch; ch++)
		{
			MemPtr = HeapPtr->MainInBuf[ch] + Samples;
			InPtr  = HeapPtr->SysMainInPtr[ch];
			for(i = 0; i < BlockSize; i++)
			{
				*MemPtr++ = (*InPtr)>>8;
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
	int *OutPtr;
	int *MemPtr;
	int len1;
	n_ch      = HeapPtr->NumberChannel;
	OffsetInc = HeapPtr->DataOutOffset;
	Count     = HeapPtr->ConsumedSamples;
	OutRdIdx  = HeapPtr->OutRdIdx;
	if(Count > 0)
	{
		for(ch = 0; ch < n_ch; ch++)
		{
			MemPtr = HeapPtr->MainOutBuf[ch] + OutRdIdx;
			OutPtr = HeapPtr->SysMainOutPtr[ch];
			//-------------- replace mod_add_M4 to gain MIPS ----------------------
			len1=(HeapPtr->MainOutBuf[ch] + HeapPtr->Tuning.BlockSize * 2)-MemPtr;
			len1=(len1>Count ? Count : len1);
			for(i=0;i<len1;i++)
			{
				*OutPtr=(__ssat((*MemPtr++),24))<<8;
				OutPtr += OffsetInc;
			}
			MemPtr=HeapPtr->MainOutBuf[ch];
			for(;i<Count;i++)
			{
				*OutPtr=(__ssat((*MemPtr++),24))<<8;
				OutPtr += OffsetInc;
			}
			//--------------------------------------------------------------------
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
	int *Ptr;
	int *MemOutPtr;
	int len1;
	n_ch      = HeapPtr->NumberChannel;
	BlockSize = HeapPtr->Tuning.BlockSize;
    OutWtIdx  = HeapPtr->OutWtIdx;
	if(BlockSize > 0)
	{
		for (ch = 0; ch < n_ch; ch++)
		{
			MemOutPtr = HeapPtr->MainOutBuf[ch] + OutWtIdx;
			Ptr = HeapPtr->MainInBuf[ch];
			//-------------- replace mod_add_M4 to gain MIPS ----------------------
			len1=(HeapPtr->MainOutBuf[ch] + HeapPtr->Tuning.BlockSize * 2)-MemOutPtr;
			len1=(len1>BlockSize ? BlockSize : len1);
			for(k=0;k<len1;k++)
			{
				*MemOutPtr++=*Ptr++;
			}
			MemOutPtr=HeapPtr->MainOutBuf[ch];
			for(;k<BlockSize;k++)
			{
				*MemOutPtr++=*Ptr++;
			}
			//--------------------------------------------------------------------
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
#endif //ifdef M4
