/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file boot_secure_types.h
 * \brief ROM codes' common types definition
 * \author STMicroelectronics
 *
 * This file covers all types common to secure and boot ROM codes.
 * 
 * @{
 */
/*---------------------------------------------------------------------------*/

#ifndef _BOOT_SECURE_TYPES_H
#define _BOOT_SECURE_TYPES_H

#include "hcl_defs.h"


/*-----------------------------------------------------------------------------
 * Types definition				                                   
 *---------------------------------------------------------------------------*/
typedef unsigned long long  t_bitfield64;

#define YES				1
#define NO				0


#define OK				1
#define FAIL			0


 #define ONE_KB        (1024)
 #define ONE_MB		   (ONE_KB * ONE_KB)


/*-----------------------------------------------------------------------------
 * Bit setting or clearing
 *---------------------------------------------------------------------------*/
#define SET_BITS(reg,mask)			((reg) |=  (mask))
#define CLEAR_BITS(reg,mask)		((reg) &= ~(mask))
#define READ_BITS(reg,mask)			((reg) &   (mask))
#define WRITE_BITS(reg,val,mask)	((reg) =   (((reg) & ~(mask)) | ((val) & (mask))))
#define READ_REG(reg)				(reg)
#define WRITE_REG(reg,val)			((reg) = (val))


/*-----------------------------------------------------------------------------
 * Byte management
 *---------------------------------------------------------------------------*/
#define WRITE_BYTE(word, byte, id)	(word = (word & ~(MASK_BYTE<<(id*BIT_TO_BYTE))) | ((t_uint32)byte<<(id*BIT_TO_BYTE)))


/*-----------------------------------------------------------------------------
 * Types conversion
 * BYTE: 8 bits, WORD: 16 bits, DWORD: 32 bits, LONGWORD: 64 bits
 *---------------------------------------------------------------------------*/
#define DWORD_TO_LONGWORD	2
#define DWORD_TO_BYTE		4
#define BYTE_TO_DWORD		4
#define BYTE_TO_BIT			8
#define BIT_TO_BYTE			8
#define WORD_TO_BIT			16
#define BIT_TO_WORD			16
#define DWORD_TO_BIT		32
#define BIT_TO_DWORD		32


/*-----------------------------------------------------------------------------
 * Address alignment
 * To align address on 32-bits boundaries
 *---------------------------------------------------------------------------*/ 
#define ADDR_ALIGN( addr ) ((addr >> 2) << 2)
#define ADDR_ALIGN_16B( addr ) (((addr) >> 4) << 4)


/*-----------------------------------------------------------------------------
 * Tests defitions
 *---------------------------------------------------------------------------*/
#define IS_NULL_POINTER(ptr) ( (ptr == NULL) ? TRUE : FALSE )
  
#endif		// _BOOT_SECURE_TYPES_H
/** @} */
