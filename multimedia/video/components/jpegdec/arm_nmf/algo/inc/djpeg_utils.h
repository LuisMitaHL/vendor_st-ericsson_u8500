/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_utils.h
 * \brief 	 utility printing functions for debuging purpose
 * \author  ST-Ericsson
 *    
 *  This file declares utility printing functions for debuging purpose
 */
/*****************************************************************************/


#ifndef _DJPEG_UTILS_H_
#define  _DJPEG_UTILS_H_


/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "djpeg_common.h"
#include "djpeg_hamac.h"

/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/
#define RESET		0
#define BRIGHT 	1
#define DIM		2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE	7
#define HIDDEN		8

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA	5
#define CYAN		6
#define	WHITE		7

/** \brief zigzag to natural re-ordering index */
#define ZZ_TO_NATURAL(zz_index) jpeg_natural_order[(zz_index)]


/** \brief terminal printf coloring function   */
#ifdef COLORS
#define TEXT_COLOR(attr,fg,bg) ut_textcolor((attr),(fg),(bg))
#else
#define TEXT_COLOR(attr,fg,bg)
#endif

/*------------------------------------------------------------------------
 * Variables							       
 *----------------------------------------------------------------------*/

extern const t_sint16 jpeg_natural_order[64];

/*------------------------------------------------------------------------
 * Functions							       
 *----------------------------------------------------------------------*/


void ut_textcolor(int attr, int fg, int bg);

void ut_print_quant_table( t_uint16 p_quant_table[]);
void ut_print_block( t_sint16 p_samples[]);
void ut_print_dc_huff_table (t_uint16 p_code[], t_uint16 p_size[]);
void ut_print_ordered_huff_table(tps_huff_elt p_ordered_table[]);

#endif /* _DJPEG_UTILS_H_ */
