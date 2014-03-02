/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_UTILS_H_
#define _VC1D_UTILS_H_


/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_co_bitstream.h"


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

/** \brief terminal printf coloring function   */
#ifdef COLORS
#define TEXT_COLOR(attr,fg,bg) ut_textcolor((attr),(fg),(bg))
#define COLOR_ERROR(value) ut_textcolor(RESET, RED, BLUE); printf value ; ut_textcolor(RESET, WHITE, BLUE);
#else
#define TEXT_COLOR(attr,fg,bg)
#endif

#if defined(PRINT_ALL) \
||  defined(PRINT_BLOCK_COEFF) \
||  defined(PRINT_BLOCK_ZZ) \
||  defined(PRINT_BLOCK_PRED) \
||  defined(PRINT_BLOCK_IQ) \
||  defined(PRINT_BLOCK_ITRANS) \
||  defined(PRINT_BLOCK_OVERLAP) \
||  defined(PRINT_BLOCK_REC) \
||  defined(PRINT_BLOCK_INTERP) \
||  defined(PRINT_BLOCK_DEBLOCK) 


#define DBG_PRINT_BLOCK(samples,blk_id,state) if (Dbg) print_block(samples,blk_id,state)
#define BLOCK_NAME(blk_id,string) if (Dbg) print_block_name((blk_id),(string))
#else
#define DBG_PRINT_BLOCK(samples,blk_id,state) ((void)(0))
#define BLOCK_NAME(blk_id,string) 
#endif

/*------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------*/
void print_btst_error_value(t_btst_error_code err);
void ut_textcolor(int attr, int fg, int bg);
void print_block(t_sint16 samples[],t_sint16 blk_id,char *state);
void print_block_name(t_sint16 blk_id,char *string);


#endif /* _VC1D_UTILS_H__ */
