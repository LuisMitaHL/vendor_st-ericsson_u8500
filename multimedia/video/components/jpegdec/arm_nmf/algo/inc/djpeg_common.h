/****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_common.h
 * \brief 	jpeg decoder host-hamac common header file
 * \author  ST-Ericsson
 *  
 *  
 *  This declares common types, macros, defines for hamac and host.
 * 
 */
/*****************************************************************************/


#ifndef _DJPEG_COMMON_H_
#define _DJPEG_COMMON_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include <stdio.h> /*  for FILE definition */

#include "djpeg_target.h"


/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/


#ifdef _DEBUG_
#define DBG_PRINTF(value) printf value /**< \brief debug printf function: use double (( when calling   */
#else
#define DBG_PRINTF(value)  
#endif /* _DEBUG_ */

/** \brief macro to round up division result   */
#define DIV_UP(a,b) ( (a)%(b) == 0 ? ( (a)/(b) ) : ( 1L + (a)/(b) ) )

/* /\** \brief macro to clip a value between min and max*\/ */
/* #define CLIP(value,min,max)   (value)<(min)?(min):((value)>(max)?max:(value))   */

/** \brief maximum number of quantization tables   */
#define NB_OF_QUANT_TABLES      4  

/** \brief maximum number of ac/dc huffman tables   
 *  \note 3 would be sufficient but encoders could
 *        count tables from 1 to 3 instead of 0 to 2
 */

#define NB_OF_HUFF_TABLES    4   

/** \brief maximum number of huffman tables 3x (AC + DC)  */
#define NB_OF_COMPONENT_HUFF_TABLES      3  


/** \brief number of coefficient in quant table   */
#define QUANT_TABLE_SIZE        64 

/** \brief size of DC huffman table   */
#define DC_HUFF_TABLE_SIZE        12 

/** \brief size of AC huffman table   */
#define AC_HUFF_TABLE_SIZE        256 

/** \brief size of DC huffman table   */
#define HUFF_BITS_TABLE_SIZE        16

/** \brief size of DC huffman table   */
#define DC_HUFFVAL_TABLE_SIZE        12 

/** \brief size of AC huffman table   */
#define AC_HUFFVAL_TABLE_SIZE        256

/** \brief max huff code size (+1)   */
#define MAX_HUFF_CODE_SIZE        17


/** \brief number of component in a scan    */
#define MAX_SCAN_COMPONENT      3

/** \brief number of component in a frame    */
#define MAX_FRAME_COMPONENT 3

/** \brief default value for ACE strength    */
#define DEFAULT_ACE_STRENGTH 6
/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/


#ifndef _BASE_TYPE_
#define _BASE_TYPE_

/** @{ \name base types definition */
/*---------------------------------*/

typedef  unsigned char       t_uint8; /**< \brief unsigned 8 bits type   */
typedef    signed char       t_sint8; /**< \brief signed 8 bits type   */

typedef  unsigned short     t_uint16; /**< \brief unsigned 16 bits type   */
typedef    signed short     t_sint16; /**< \brief signed 16 bits type   */

typedef  unsigned long int  t_uint32; /**< \brief unsigned 32 bits type   */
typedef    signed long int  t_sint32; /**< \brief signed 32 bits type   */
typedef  t_sint16              T_BOOL; /**< \brief boolean type   */

//typedef t_uint32              t_ulong_value; /**< \brief used to exchange addresses between hamac and host   */

typedef t_uint32 t_ulong_value;/** * \brief 	Type to redefine endianness from host for 32 bits signed words * \todo 	Should be removed when pb of endianness is resolved in MC * * Lsb is defined first to take into account Little endian for 32 bit  * words. Sign is not represented here... Must be taken into account  * after conversion **/
typedef t_sint32 t_long_value;/** * \brief 	Type for Unsigned 16 bits words **/
typedef t_uint16 t_ushort_value;/** * \brief 	Type for signed 16 bits words **/
typedef t_sint16 t_short_value;/** * \brief 	Type used for ahb addresses  * \warning AHB addresses must be at least aligned word.  **/
typedef t_uint32 t_ahb_address;

#ifndef TRUE 
#define TRUE 1
#endif 

#ifndef FALSE
#define FALSE 0
#endif 
/** @}*/

#endif /* _BASE_TYPE_ */
/*Global variable indicating if yes or no there is the no slice mode*/
/* We didn't put it in the Host or Hamac Structure directly because it's the interface between FW and HCL
   and we can't break it by adding a variable */

/** \brief Image analysis struct */

#ifndef __NMF


/** \brief Decoder error codes */
typedef enum
{
    /** \brief all is ok!   */
    FW_DJPEG_NO_ERROR=0,         

    /** \brief end of bitstream data reached in current bitstream buffer:
     * Handled by the host main: -> refresh of the bitstream buffer or exit
     */
    FW_DJPEG_END_OF_DATA,   

    /** \brief error opening bitstream file for reading
     * Handled by the host main: -> exit
     */
    FW_DJPEG_FILE_IN_ERROR,    

    /** \brief error result file for writing
     * Handled by the host main: -> exit
     */
    FW_DJPEG_FILE_OUT_ERROR,    

    /** \brief no sufficient memory to store bitstream 
     *  Handled by the host main: -> exit 
     */
    FW_DJPEG_MALLOC_ERROR,  

    /** \brief end of file reached   
     * handled by the host main: -> exit
     */
    FW_DJPEG_FILE_END,      

    /** \brief unsupported mode 
     * Handled by the host main: -> exit
     */
    FW_DJPEG_UNSUPPORTED_MODE,     

    /** \brief end of image reached:  
     * Handled by the host main: -> normal exit
     */
    FW_DJPEG_END_OF_IMAGE,      

   /** \brief 16 bit precision not supported 
    * Handled by the host main: -> exit
    */
    FW_DJPEG_INVALID_PQ,           

    /** \brief JFIF not compliant 
     * Handled by the host main: ->exit
     */
    FW_DJPEG_NOT_JFIF,             

    /** \brief invalid coefficient position in block   
     * Handled by the host main: ->exit
     */

    FW_DJPEG_BAD_HUFF_POS,           

    /** \brief invalid huff dc code      
     * Handled by the hamac main: -> return INVALID_BITSTREAM
     */
    FW_DJPEG_BAD_HUFF_DC,            

    /** \brief invalid huff ac code       
     * Handled by the hamac main: -> return INVALID_BITSTREAM
     */
    FW_DJPEG_BAD_HUFF_AC,            

    /** \brief unexpected restart marker          
     * Handled by the hamac main: -> return INVALID_BITSTREAM
     */
    FW_DJPEG_BAD_RESTART_VAL,        

    /** \brief indexed Huff or quant table not relevant            
     * Handled by the hamac main: -> return INVALID_BITSTREAM
     */
    FW_DJPEG_BAD_TABLE_INDEX,

    /** \brief the markers read are not sufficient to decompress the image (missing tables)
     * Handled by the host main: ->exit
     */
    FW_DJPEG_BAD_MARKER,

    /** \brief bad  command line arguments
     * Handled by the host main: ->exit
     */
    FW_DJPEG_BAD_ARG,
	
    /** \brief start of scan 
	 */
	FW_DJPEG_START_OF_SCAN


    
} t_djpeg_error_code;

#endif



#endif /* _DJPEG_COMMON_H_ */
