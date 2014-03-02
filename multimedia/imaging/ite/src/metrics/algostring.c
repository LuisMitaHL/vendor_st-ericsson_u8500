/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algomemoryroutines.c
* \brief    Contains Algorithm Library Memory Routines
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#include "algostring.h"

/**
 * Function to calculate length of a string.
 *
 * @param aString       [const TUint8*] Input String.
 *
 * @return              [TUint32] Length of the string.
 */
TUint32 AlgoStrlen(const TUint8* aString) 
    {
	TUint32 count=0;
	while(aString[count] != '\0')
		count++;
	return count;
    }

/**
 * Function to copy source string into destination string.
 *
 * @param aTarget      [TUint8*] Pointer to the destination string buffer.
 * @param aSource      [const TUint8*] Pointer to the source string buffer.
 *
 * @return             [TUint8*] Destination buffer address.
 */
TUint8* AlgoStrcpy(TUint8* aTarget, const TUint8* aSource)
    {
    TUint8 *orig_target = aTarget;
    while(*aSource != 0)
	{
		*aTarget++ = *aSource++;
	}
	*aTarget = 0;
    return orig_target;
    }
	
/**
 * Function to cancatenate source string into destination string.
 *
 * @param aDest        [TUint8*] Pointer to the destination string buffer.
 * @param aSrc         [const TUint8*] Pointer to the source string buffer.
 *
 * @return             [TUint8*] Destination buffer address.
 */
TUint8* AlgoStrcat(TUint8 *aDest, const TUint8 *aSrc)
    {
    AlgoStrcpy(aDest + AlgoStrlen(aDest), aSrc);
    return aDest;
    }

/**
 * Function to compare the two strings.
 *
 * @param aStr1        [const TUint8*] Pointer to the first string buffer.
 * @param aStr2        [const TUint8*] Pointer to the second string buffer.
 *
 * @return             [TUint8] 0 when the strings are equal, a negative integer when aStr1 is less than aStr2, 
 *                              or a positive integer if aStr1 is greater than aStr2.
 */
TInt8 AlgoStrcmp(TUint8 *aStr1, const TUint8 *aStr2)
    {
    for(; *aStr1 == *aStr2; ++aStr1, ++aStr2)
        if(*aStr1 == 0)
            return 0;
    return *(unsigned char *)aStr1 < *(unsigned char *)aStr2 ? -1 : 1;
    }
