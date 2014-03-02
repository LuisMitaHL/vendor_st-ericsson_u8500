/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _MACROS_H_
#define _MACROS_H_

/*
 * Includes
 */
#include <assert.h>
#include "t1xhv_types.h"

/*
 * Defines							       
 */
#ifdef USE_PRINTF
#define PRINTF(value) printf value /**< \brief debug printf function: use double (( when calling   */
#else
#define PRINTF(value)  
#endif /* USE_PRINTF */

#ifdef USE_ASSERT
#define ASSERT(value) assert(value) /**< \brief use this macro to conditionnaly insert assertions   */
#else
#define ASSERT(value)  
#endif /* _USE_ASSERT_ */

#ifdef MMDSP
/* For XIOready */
#define XIOCLASS __XIO
#endif


/* Adresses re-mapping */
/** @{ \name Parameters for Host */
#define HOST_OFFSET 			0x40000UL     /* changed for 8815, now on 32 bits !! */
#define MMDSP_OFFSET 			0x0
/** @} */


/****************************************************************************/
/**
 * \brief  align the value on the next (upper) align multiple
 * \author jean-marc volle
 * \param  value: value to align
 * \param  align: alignement size
 * \return value aligned on next align multiple if need be, value otherwise
 *  
 *  example of alignement: ALIGN_SIZE (25,8) = 32, ALIGN_SIZE(24,8) = 24
 */
/****************************************************************************/
#define ALIGN_SIZE(value,align) ( (value) & ((align)-1)) ? ( (value) +((align) - ((value) & ((align)-1)) )) :(value)  

/****************************************************************************/
/**
 * \brief   extract low order 16 bits of a 32 bits word
 * \author  jean-marc volle
 * \param   val 32 bits unsigned word
 * \return  16 bits unsigned word
 * 
 */
/****************************************************************************/
#pragma inline
static t_uint16 LS_SHORT(t_uint32 val)
{
    return wextract_l(val);
}

/****************************************************************************/
/**
 * \brief   extract high order 16 bits of a 32 bits word
 * \author  jean-marc volle
 * \param   val 32 bits unsigned word
 * \return  16 bits unsigned word
 * 
 */
/****************************************************************************/
#pragma inline
static t_uint16 MS_SHORT(t_uint32 val)
{
    return wextract_h(val);
}

/****************************************************************************/
/**
 * \brief   Returns absolute value
 * \author  Serge Backert
 * \param   val 16 bits unsigned word
 * \return  16 bits unsigned word
 * 
 */
/****************************************************************************/
#pragma inline
static t_uint16 ABS(t_uint16 val)
{
    return (t_uint16)wabssat((t_uint16)val);
}
/****************************************************************************/
/**
 * \brief   Returns absolute value for 32 bits
 * \author  Loic Habrial
 * \param   val 32 bits signed word
 * \return  32 bits unsigned word
 * 
 */
/****************************************************************************/
#pragma inline
static t_uint32 ABS_32(t_sint32 val)
{
    if (val >=0)
    {
        return (t_uint32)val;
    }
    else
    {
        return (t_uint32)-(val);
    }
}
/****************************************************************************/
/**
 * \brief   Make a 32 bits register with 2 16 bits registers
 * \author  Cyril Enault
 * \param   msp 16 bits signed word (MSB)
 * \param   lsp 16 bits signed word (LSB)
 * \return  32 bits unsigned word
 * 
 */
/****************************************************************************/
 asm long MAKE_LONG(int msp, int lsp) {
    mv @{msp}, @{}.1
    mv @{lsp}, @{}.0
    }

/*****************************************************************************/
/**
 *                         MACROS TO CONVERT FROM HOST                
 */
/*****************************************************************************/
/** @{ \name HOST Macros. */
/*****************************************************************************/
/** \brief  Convert 32-bit address coming from host
 *          into MMDSP internal address 
 *  \param  addr LSB of AHB address 
 *  \return Address inside MMDSP Memory. 			 	
 */
/*****************************************************************************/
#define HOST_CONVERT_ADDR(addr) ((((addr) - HOST_OFFSET) >> 1) + MMDSP_OFFSET)

/*****************************************************************************/
/** \brief  Convert address from MMDSP to 32-bit host
 *          address 
 *  \param  addr LSB of AHB address 
 *  \return Address inside MMDSP Memory. 			 	
 */
/*****************************************************************************/
#define HOST_UNCONVERT_ADDR(addr) ((((addr) - MMDSP_OFFSET) << 1) + HOST_OFFSET)

/*****************************************************************************/
/** \brief  Convert a 32 bit word from host (structure)
 *          into a 32 bit word for MMDSP BIG Endian 
 *  \param  value structure to convert 
 *  \return structure converted.				        
 */
/*****************************************************************************/
#ifdef __T1XHV_NMF_ENV
#define CONVERT_ENDIANNESS(value) (value)
#else /* __T1XHV_NMF_ENV */
#define CONVERT_ENDIANNESS(value) (((t_uint32)value.msb)<<16 | value.lsb)
#endif /* __T1XHV_NMF_ENV */

/*****************************************************************************/
/** \brief  Convert a 32 bit word into a 32 bit word 
 *          LITTLE ENDIAN for host (structure)
 *  \param  value 32 bit word to convert 
 *  \param  output Structure LITTLE ENDIAN resulting	
 */
/*****************************************************************************/
#ifdef __T1XHV_NMF_ENV
#define UNCONVERT_ENDIANNESS(value,output) output=value;
#else /* __T1XHV_NMF_ENV */
#define UNCONVERT_ENDIANNESS(value,output) output.lsb = wextract_l(value); output.msb = wextract_h(value);
#endif /* __T1XHV_NMF_ENV */

/*****************************************************************************/
/** \brief  Check if data is a multiple of XXX
 *  \param  data data to check
 *  \param  shift Number to shift 2 to find mult
 *  \return Not a mult if different from 0 	
 */
/*****************************************************************************/
#define CHECK_SOFT_ERR(data,shift) (data & ((1 << shift) - 1))

/** @} */


#endif /* _MACROS_H_ */

