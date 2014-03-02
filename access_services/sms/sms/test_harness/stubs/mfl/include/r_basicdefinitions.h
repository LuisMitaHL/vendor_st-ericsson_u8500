#ifndef INCLUSION_GUARD_R_BASICDEFINITIONS_H
#define INCLUSION_GUARD_R_BASICDEFINITIONS_H

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* DESCRIPTION:
*
*     Portable types used for a consistent target platform.
*     The name should make it totally clear as to what they are used for.
*
*
*/

#include "t_basicdefinitions.h"

/********************
* General macro's
* These should be moved to a utility header when we have a good file name put it in.
*********************/
/**
 * Returns the smallest value of Val1 and Val2.
 *
 * @param [in] Val1 Value of any scalar type 
 * @param [in] Val2 Value of any scalar type
 *
 * @return The return type obeys the rules regarding binary conversion of the
 *         programming language C.
 *
 * @sigbased No - Macro, parameters expand more than one time
 */
#ifndef MIN
#define MIN(Val1, Val2) (((Val1) < (Val2)) ? (Val1) : (Val2))
#endif

/**
 * Returns the largest value of Val1 and Val2.
 *
 * @param [in] Val1 Value of any scalar type 
 * @param [in] Val2 Value of any scalar type
 *
 * @return The return type obeys the rules regarding binary conversion of the
 *         programming language C.
 *
 * @sigbased No - Macro, parameters expand more than one time
 */
#ifndef MAX
#define MAX(Val1, Val2) (((Val1) > (Val2)) ? (Val1) : (Val2))
#endif

/**
 * The return value is evaluated at compile time, not at run-time. The return
 * value can be used as dimension parameter in other declarations of other 
 * arrays. The return value can however not be used in expressions evaluated in
 * pre-processor directives (e.g. #if (ElementsOf(a) > 10)) since it is based on
 * the sizeof() operator.
 *
 * @param [in] Array Must be an array variable or array type
 *
 * @return Number of elements in the array passed as parameter.
 *
 * @sigbased No - Macro, parameters expand more than one time
 */
#ifndef ElementsOf
#define ElementsOf(Array) (sizeof(Array) / sizeof(Array[0]))
#endif


/*****************************
* Macro's to extract bytes from uint16 or sint16
******************************/
/**
 * Used to retrieve the least significant 8 bits of a 16 bit word.
 *
 * @param [in] Val A value of type uint16
 *
 * @return Returns value of type uint8 representing the low byte of the value
 *         passed as parameter.
 *
 * @sigbased No - Macro
 */
#define INT16_LBYTE(Val) (uint8)((Val) & 0xFF)

/**
 * Used to retrieve the most significant 8 bits of a 16 bit word.
 *
 * @param [in] Val A value of type uint16
 *
 * @return Returns value of type uint8 representing the high byte of the value
 *         passed as parameter.
 *
 * @sigbased No - Macro
 */
#define INT16_HBYTE(Val) (uint8)((Val) >> 8)

/*****************************
* Macro's to build int16 from two bytes (low & high)
******************************/
/**
 * Used to form a 16 bit signed value from two 8 bit values. The result will
 * be negative if the most significant bit (MSB) of the HighByte parameter is
 * a one, otherwise positive.
 *
 * @param [in] LowByte  Defines the value that will be in the low byte of the
 *                      returned sint16 value.
 * @param [in] HighByte Defines the value that will be in the high byte of the
 *                      returned sint16 value.
 *
 * @return Returns value of type sint16 constructed from two byte values.
 *
 * @sigbased No - Macro
 */
#define BYTES_TO_SINT16(LowByte,HighByte) (sint16)(((HighByte) << 8) + (LowByte))

/**
 * Used to form a 16 bit unsigned value from two 8 bit values.
 *
 * @param [in] LowByte  Defines the value that will be in the low byte of the
 *                      returned sint16 value.
 * @param [in] HighByte Defines the value that will be in the high byte of the
 *                      returned sint16 value.
 *
 * @return Returns value of type uint16 constructed from two byte values.
 *
 * @sigbased No - Macro
 */
#define BYTES_TO_UINT16(LowByte,HighByte) (uint16)(((HighByte) << 8) + (LowByte))

/*
**========================================================================
**
** MACRO TO AVOID WARNINGS FOR UNUSED PARAMETERS OR VARIABLE
**
** Example of use:
**
** int MyFunc(int p)
** {
**   int a = 42;
**   IDENTIFIER_NOT_USED(p); // This line must be placed after declaration of local variables.
**
**   return a;
** }
**
**========================================================================
*/


#ifdef IDENTIFIER_NOT_USED
#undef IDENTIFIER_NOT_USED
#endif

/* #define IDENTIFIER_NOT_USED(p) if (sizeof(&p)) {}
* this seems to work better under lint:  */
/**
 * Should be used to "touch" a variable or a formal parameter that is intentionally
 * not used and thereby avoid compiler warnings.
 *
 * @param [in] Id A symbol that represents a variable that is not used, usually a
 *                formal parameter.
 *
 * @sigbased No - Macro
 */
#define IDENTIFIER_NOT_USED(Id) (void)(Id);

#endif                          /* INCLUSION_GUARD_R_BASICDEFINITIONS_H */
