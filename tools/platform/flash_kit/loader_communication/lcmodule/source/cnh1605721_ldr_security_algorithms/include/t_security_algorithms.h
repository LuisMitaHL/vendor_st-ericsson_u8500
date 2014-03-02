/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _T_SECURITY_ALGORITHMS_H_
#define _T_SECURITY_ALGORITHMS_H_
/**
 * \addtogroup ldr_security_algorithms
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Defined value for HASH error. */
#define HASH_ERROR 0xffffffff

/** Length of SHA256 algorithm results */
#define SHA256_LENGTH 32

/** Length of CRC16 algorithm results */
#define CRC16_LENGTH  2

/** */
#define MAX_NR_IN_QUEUE_TOKENS   16

/** */
typedef void (*HashCallback_fn)(void *Data_p, uint32 Length, uint8 *Hash_p, void *Param_p);

/** Type of hash algorithm */
typedef enum {
    HASH_SHA256 = 0x0001,         /**< 4LSB SHA-256 algorithm */
    HASH_CRC16  = 0x0002,         /**< CRC16 */
    HASH_SIMPLE_XOR = 0x0003,      /**< 4LSB Simple XOR algorithm */
    HASH_NONE = 0x0004         /**< Communication is not hash secured */
} HashType_e;

/** @} */
#endif /*_T_SECURITY_ALGORITHMS_H_*/
