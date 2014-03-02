/*****************************************************************************\
 *  Copyright (C) 2011 - Ericsson AB - All Rights Reserved.
 *
 *      Filename:       TEE_MathAPI.h
 *
 *      Created:        22/6/11 by Patrik Ekdahl
 *
 *      Project:        GlobalPlatform Math API test implementation.
 *
 *
 *      Authors:        Patrik Ekdahl (Ericsson AB)
 *
 *      Description:    Header file for the TEE Math API.
 *
 \*****************************************************************************/
#ifndef GUARD_TEE_MATHAPI_H
#define GUARD_TEE_MATHAPI_H

#include <stddef.h>     // for size_t
#include <stdint.h>     // for uint32_t and friends
#include <stdbool.h>    // for bool (!)


/*************************************************************
 *
 *  MACRO DEFINITIONS
 *
 *************************************************************/


/*------------------------------------------------------------
 *
 *  How functions are exported
 *
 */
#define TEE_MATHAPI_EXPORT


/*
 * The modes for String Conversion
 */
#define TEE_STRING_MODE_HEX_UC MPA_STRING_MODE_HEX_UC
#define TEE_STRING_MODE_HEX_LC MPA_STRING_MODE_HEX_UC


/*------------------------------------------------------------
 *
 *  Define IN, OUT, INBUF and OUTBUF to keep format from the spec.
 *
 */
#define IN const
#define OUT
#define INOUT
#define INBUF const
#define OUTBUF


/*************************************************************
 *
 *  MEMORY ALLOCATION AND SIZE
 *
 *************************************************************/

/*
 * THIS IS THE MAXIMUM NUMBER OF BITS THAT THE LIBRARY SUPPORTS.
 * It defines the size of the scratch memory pool for the underlying
 * mpa library.
 */
#define TEE_MAX_NUMBER_OF_SUPPORTED_BITS 2048




/*************************************************************
 *
 * INITIALIZATION FUNCTIONS
 *
 *************************************************************/


/*
 * !!! This function must be called before you do anything else !!!
 *  NOTE: Not part of the spec
 */
TEE_MATHAPI_EXPORT
void TEE_MathAPI_Init(void);

/*
 * NOTE: Not part of the spec
 */
TEE_MATHAPI_EXPORT
TEE_Result TEE_BigIntSet(
                         OUT   TEE_BigInt* _dest,
                         IN    TEE_BigInt* _src);



/*************************************************************
 *
 *  CONVERSION FUNCTIONS
 *
 *************************************************************/


/*
 * NOTE: Not part of the spec
 */
TEE_MATHAPI_EXPORT
int TEE_BigIntConvertFromString(
                                OUT TEE_BigInt*     _dest,
                                IN  char*           _src);

/*
 * NOTE: Not part of the spec
 */
TEE_MATHAPI_EXPORT
char *TEE_BigIntConvertToString(
                                OUT char*       _dest,
                                    int         _mode,
                                    int         _groupsize,
                                IN  TEE_BigInt* _src);



/*************************************************************
 *
 *  LOGICAL OPERATIONS
 *
 *************************************************************/

/*
 * NOTE: Not part of the spec
 */
TEE_MATHAPI_EXPORT
void TEE_BigIntShiftLeft(
                         OUT    TEE_BigInt*   _dest,
                         IN     TEE_BigInt*   _op,
                                size_t        _bits);



/*************************************************************
 *
 *  BASIC ARITHMETIC OPERATIONS
 *
 *************************************************************/

/*
 * NOTE: Not part of the spec
 */
TEE_MATHAPI_EXPORT
void TEE_BigIntAbs(
                   OUT   TEE_BigInt* _dest,
                   IN    TEE_BigInt* _src);



/*************************************************************
 *
 *  OTHER ARITHMETIC OPERATIONS
 *
 *************************************************************/


/*
 * Not part of the specs
 */
TEE_MATHAPI_EXPORT
void TEE_BigIntGCD(
                   OUT TEE_BigInt*  _gcd,
                   IN  TEE_BigInt*  _op1,
                   IN  TEE_BigInt*  _op2);


/*************************************************************
 *
 *  FAST MODULAR MULTIPLICATION
 *
 *************************************************************/


/*
 * NOTE: Not part of the spec
 */
TEE_MATHAPI_EXPORT
void TEE_BigIntExponentiateMod(
                               OUT TEE_BigInt*            _dest,
                               IN  TEE_BigInt*            _op1,
                               IN  TEE_BigInt*            _op2,
                               IN  TEE_BigInt*            _n,
                               IN  TEE_BigIntFMMContext*  _context);



#endif /* include guard */
