/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   synchronisation.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <misc/synchronisation.nmf>
#include "synchronisation.h"

#include "misc/synchronisation/synchronisation.idt"

void TSreset(AVSynchro_p AVInfo)
{
	AVInfo->nTimeStampl = 0;
	AVInfo->nTimeStamph = 0;
	AVInfo->SampleCounter = 0;
}

void TSinit(AVSynchro_p AVInfo, t_sint24 nTimeStamph, t_uint48 nTimeStampl)
{
	AVInfo->nTimeStampl = nTimeStampl;
	AVInfo->nTimeStamph = nTimeStamph;
	AVInfo->SampleCounter = 0;
}

void TSupdate(Buffer_p BufOut, t_sint32 Offset, t_uint32 SampleFreq, t_uint32 NbChannel)
{
	signed long time;
	unsigned long old_low_part = BufOut->nTimeStampl;

	time = (signed long)(((Offset * 10000) / mFrequency[SampleFreq]) * 100);

	if (NbChannel == 2)
	{
		time = time >> 1;
	}

	BufOut->nTimeStampl += time;

	if (Offset < 0)
	{
		if (BufOut->nTimeStampl > old_low_part)
		{
			BufOut->nTimeStamph -= 1;
		}
	}
	else
	{
		if (BufOut->nTimeStampl < old_low_part)
		{
			BufOut->nTimeStamph += 1;
		}
	}
}

void TScomputation(AVSynchro_p AVInfo, Buffer_p BufOut, t_uint32 Offset, t_uint32 SampleFreq, t_uint32 NbChannel)
{
	BufOut->nTimeStamph = AVInfo->nTimeStamph;
	BufOut->nTimeStampl = AVInfo->nTimeStampl;
	TSupdate(BufOut, (t_sint32)AVInfo->SampleCounter, SampleFreq, NbChannel);
	AVInfo->SampleCounter += Offset;	
}

void TSpropagation(Buffer_p BufOut, Buffer_p BufIn, t_uint32 BufInDataConsumed, t_uint32 BufOutOffset, t_uint32 SampleFreq, t_uint32 NbChannel)
{
	if (BufOutOffset == 0)
	{
		// copy bufferheadertype information from input to output
		BufOut->nTimeStamph = BufIn->nTimeStamph;
		BufOut->nTimeStampl = BufIn->nTimeStampl;
	
		// input data already read, thus increase TimeStamp of samples already read
		// if first write to output buffer
		if (BufInDataConsumed != 0)
		{	
			TSupdate(BufOut, (t_sint32)BufInDataConsumed, SampleFreq, NbChannel);			
		}
	}
	else
	{
		// data already written in output buffer before STARTTIME is received
		if (BufIn->flags & BUFFERFLAG_STARTTIME)
		{
			// copy bufferheadertype information from input to output
			BufOut->nTimeStamph = BufIn->nTimeStamph;
			BufOut->nTimeStampl = BufIn->nTimeStampl;

			// and update TimeStamp
			TSupdate(BufOut, (t_sint32)( - BufOutOffset), SampleFreq, NbChannel);		
		}
	}
}
