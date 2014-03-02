/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */


#ifndef __INCLUDE_ALGO_STRINGS_H__
#define __INCLUDE_ALGO_STRINGS_H__

/*
 * Includes 
 */
#include "algotypedefs.h"
#include <string.h>


#ifdef __cplusplus
extern "C"
{
#endif  
/**
 * Function to calculate length of a string.
 *
 * @param aString       [const TUint8*] Input String.
 *
 * @return              [TUint32] Length of the string.
 */
TUint32 AlgoStrlen(const TUint8* aString);

/**
 * Function to copy source string into destination string.
 *
 * @param aTarget      [TUint8*] Pointer to the destination string buffer.
 * @param aSource      [const TUint8*] Pointer to the source string buffer.
 *
 * @return             [TUint8*] Destination buffer address.
 */
TUint8* AlgoStrcpy(TUint8* aTarget, const TUint8* aSource);

/**
 * Function to cancatenate source string into destination string.
 *
 * @param aDest        [TUint8*] Pointer to the destination string buffer.
 * @param aSrc         [const TUint8*] Pointer to the source string buffer.
 *
 * @return             [TUint8*] Destination buffer address.
 */
TUint8* AlgoStrcat(TUint8 *aDest, const TUint8 *aSrc);

/**
 * Function to compare the two strings.
 *
 * @param aStr1        [const TUint8*] Pointer to the first string buffer.
 * @param aStr2        [const TUint8*] Pointer to the second string buffer.
 *
 * @return             [TInt8] 0 when the strings are equal, a negative integer when aStr1 is less than aStr2, 
 *                              or a positive integer if aStr1 is greater than aStr2.
 */
TInt8 AlgoStrcmp(TUint8 *aStr1, const TUint8 *aStr2);

#ifdef __cplusplus
}
#endif 


#endif	//__INCLUDE_ALGO_STRINGS_H__

