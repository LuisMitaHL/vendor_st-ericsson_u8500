/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup Upper_MAC_Core
 * \brief
 *
 */
/**
 * \file umac_utils.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_utils.c
 * \brief
 * The file implements some utility functions.
 * \ingroup Upper_MAC_Core
 * \date 30/04/08 14:10
 */

/******************************************************************************
		      INCLUDE FILES
******************************************************************************/
#include "section_wfm.h"
#include "umac_utils.h"

/******************************************************************************
 * NAME:	WFM_UMAC_GetNextHigherRate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns the next high rate from the given bitmask
 * \param Values  - Bit mask with all rates
 * \param Pos	  - Current Rate
 * \returns Next High rate if successful, otherwise zero.
 *****************************************************************************/
uint32 WFM_UMAC_GetNextHigherRate(uint32 Values, uint32 Pos)
{
	uint32 OrigPos = Pos;

	Pos = (Pos << 1);

	while (Pos) {
		if (Pos & Values)
			break;

		Pos = Pos << 1;
	}

	if (Pos > OrigPos)
		return Pos;
	else
		return OrigPos;
} /* end WFM_UMAC_GetNextHigherRate() */

/******************************************************************************
 * NAME:	WFM_UMAC_GetNextLowerRate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns the immediate lower rate.
 * \param Values  - Bit mask with all rates
 * \param Pos	  - Current Rate
 * \returns Next low rate if successful, otherwise zero.
 *****************************************************************************/
uint32 WFM_UMAC_GetNextLowerRate(uint32 Values, uint32 Pos)
{
	uint32 OrigPos = Pos;

	Pos = Pos >> 1;

	while (Pos) {
		if (Pos & Values)
			break;

		Pos = Pos >> 1;
	}

	if (Pos != 0) {
		/*found some valid non-zero rate */
		return Pos;
	} else {
		/*check if 0 rate is valid or not. */
		if (Values & 1) {
			/*yes, 0 is valid */
			return Pos;
		} else {
			/*
			  we cannot go down further because existing rate
			  was lowest
			*/
			return OrigPos;
		}
	}
} /* end WFM_UMAC_GetNextLowerRate() */

/******************************************************************************
 * NAME:	WFM_UMAC_GetHighestRate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns the highest rate.
 * \param Values  - Bit mask with all rates
 * \returns highest rate if successful, otherwise zero.
 *****************************************************************************/
uint32 WFM_UMAC_GetHighestRate(uint32 Values)
{
	uint32 Pos = 0x80000000;

	while (Pos) {
		if (Pos & Values)
			break;

		Pos = Pos >> 1;
	}

	return Pos;

} /* end WFM_UMAC_GetHighestRate() */

/******************************************************************************
 * NAME:	WFM_UMAC_GetLowestRate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns the lowest rate.
 * \param Values  - Bit mask with all rates
 * \returns lowest rate if successful, otherwise zero.
 *****************************************************************************/
uint32 WFM_UMAC_GetLowestRate(uint32 Values)
{
	uint32 Pos = 0x1;

	while (Pos) {
		if (Pos & Values)
			break;

		Pos = Pos << 1;
	}

	return Pos;

} /* end WFM_UMAC_GetLowestRate() */

/******************************************************************************
 * NAME:	WFM_UMAC_GetNumberOfOnes
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns the number of bits set in the given bitmap.
 * \param Values  - Bit mask to be counted.
 * \returns Number of bits set.
 *****************************************************************************/
uint32 WFM_UMAC_GetNumberOfOnes(uint32 Values)
{
	uint32 Mask = 0x80000000;
	uint32 Counter = 0;

	if (!Values)
		return 0;


	while (Mask) {
		if (Mask & Values)
			Counter++;

		Mask = (Mask >> 1);
	}

	return Counter;

} /* end WFM_UMAC_GetNumberOfOnes() */

/******************************************************************************
 * NAME:	WFM_UMAC_GetBitPosition
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns the offset of the least significant bit which is set.
 * \param Value  - The value.
 * \returns Offset of the least significant bit which is set.
 *****************************************************************************/
uint32 WFM_UMAC_GetBitPosition(uint32 Value)
{
	uint32 Mask = 0x00000001;
	uint32 Counter = 0;

	if (!Value)
		return 0;


	while (Mask) {
		if (Mask & Value)
			break;


		Counter++;
		Mask = (Mask << 1);
	}

	return Counter;

} /* end WFM_UMAC_GetBitPosition() */

/******************************************************************************
 * NAME:	WFM_UMAC_FindMiddleRateIndex
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function finds the middle index of the bits lower than the given index.
 * \param Mask			- The bitmask.
 * \param CurrentIndex	- Current index.
 * \returns Offset of the least significant bit which is set.
 *****************************************************************************/
uint32 WFM_UMAC_FindMiddleRateIndex(uint32 Mask, uint32 CurrentIndex)
{
	uint32 NumOfBitsSet = 0;
	uint32 PosMask = 0;
	uint32 Pos = (1 << (CurrentIndex - 1));
	uint32 i = 0;
	uint32 FinalIndex = (CurrentIndex);
	uint32 elementToFind = 0;
	uint32 NewMask = 0;

	if (!CurrentIndex)
		return 0;


	/*Get Number of ones in the right hand side of Current Index */
	i = (CurrentIndex - 1);

	do {
		PosMask = PosMask | (0x00000001 << i);
	} while (i--);

	NewMask = (PosMask & Mask);
	NumOfBitsSet = WFM_UMAC_GetNumberOfOnes(NewMask);

	elementToFind = NumOfBitsSet / 2;

	i = 0;
	do {
		FinalIndex--;
		if (NewMask & Pos) {
			i++;

			if (i == elementToFind)
				break;


		}

		Pos = (Pos >> 1);

	} while ((i < elementToFind));

	return FinalIndex;
} /* end WFM_UMAC_FindMiddleRateIndex() */
