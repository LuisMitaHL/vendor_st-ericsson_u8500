/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_co_bitstream_parser.h
 * \brief 	ommon jpeg bistream parsing functions declaration
 * \author  ST-Ericsson
 *    
 *  This file declares jpeg bitstream parsing functions. Those functions are
 *  built on top of djpeg_bitstream.c functions which handle low-level parsing
 */
/*****************************************************************************/


#ifndef _DJPEG_CO_BITSTREAM_PARSER_H_
#define _DJPEG_CO_BITSTREAM_PARSER_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/

#include "djpeg_co_bitstream.h"  /* for bitstream struct declaration */

/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/



/** \brief quant table struct definition CCITT_Rec_T81 B2.4.1  */
/*-------------------------------------------------------------*/
typedef struct quant_table
{
    t_uint16 q[QUANT_TABLE_SIZE]; /**< \brief quantization table element in natural order   */

} ts_quant_table, *tps_quant_table;

//typedef ts_quant_table * tps_quant_table; /**< \brief pointer on quant table   */


/** \brief compressed huffman table struct definition CCITT_Rec_T81 B2.4.2  */
/*--------------------------------------------------------------------------*/

typedef struct dht_table
{
    t_uint16 bits[17]; /**< \brief BITS list */
    t_uint16 huffval[256];  /**< \brief HUFFVAL list   */
} ts_dht_table, *tps_dht_table;

/** \brief component compressed huffman table struct definition   */
/*----------------------------------------------------------------*/
typedef struct component_dht_table
{
    ts_dht_table dc;
    ts_dht_table ac;

} ts_component_dht_table, *tps_component_dht_table;

//typedef ts_component_dht_table *tps_component_dht_table; /**< \brief pointer on component huffman table   */

typedef struct huffman_table
{
    t_uint16 code[256];
    t_uint16 size[256];
} ts_huffman_table ,*tps_huffman_table;

typedef struct component_huffman_table
{
    ts_huffman_table dc;
    ts_huffman_table ac;
} ts_component_huffman_table, *tps_component_huffman_table;



/*------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------*/


#endif /* _DJPEG_CO_BITSTREAM_PARSER_H_ */
