/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _T1XHV_RETARGET_H_
#define _T1XHV_RETARGET_H_


/*------------------------------------------------------------------------
 * Include
 *----------------------------------------------------------------------*/
#include "t1xhv_types.h"

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/
/**
 * \brief 	Type to redefine endianness from host for unsigned 32 bits words
 * \todo 	Should be removed when pb of endianness is resolved in MC
 *
 * Lsb is defined first to take into account Little endian for 32 bit 
 * words. Sign is not represented here... Must be taken into account 
 * after conversion
 **/
#ifdef ORIG_CODE
typedef struct t_ulong_value {
    t_uint16 lsb;		/**< \brief Less Significant word of 16 bits */
    t_uint16 msb;		/**< \brief Most Significant word of 16 bits */
} t_ulong_value ;
#else
typedef t_uint32  t_ulong_value;
#endif


/**
 * \brief 	Type to redefine endianness from host for 32 bits signed words
 * \todo 	Should be removed when pb of endianness is resolved in MC
 *
 * Lsb is defined first to take into account Little endian for 32 bit 
 * words. Sign is not represented here... Must be taken into account 
 * after conversion
 **/
#ifdef ORIG_CODE
typedef struct t_long_value {
    t_uint16 lsb;		/**< \brief Less Significant word of 16 bits */
    t_uint16 msb;		/**< \brief Most Significant word of 16 bits */
} t_long_value ;
#else
typedef t_sint32  t_long_value;
#endif

/**
 * \brief 	Type for Unsigned 16 bits words
 **/
typedef t_uint16 t_ushort_value;

/**
 * \brief 	Type for signed 16 bits words
 **/
typedef t_sint16 t_short_value;

/**
 * \brief 	Type used for ahb addresses 
 * \warning AHB addresses must be at least aligned word. 
 **/
typedef t_ulong_value t_ahb_address;

/**
 * \brief 	Time stamp used by timer (90kHz resolution)
 **/
typedef t_long_value t_time_stamp;


#endif /* _T1XHV_RETARGET_H_ */


