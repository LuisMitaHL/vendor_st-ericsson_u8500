/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _driver_getbit_h_
#define _driver_getbit_h_

#include "audiolibs_common.h"

/*----------- MEANING OF THE LEVEL OF THE FUNCTIONS ------------*/
/* --Level0 	: Mandatory function defining the buffer that 	*/
/* all others levels functions are going to use.				*/
/* --Level1 	: Give only 1 read access to bits inside the  	*/
/* buffer and give a counter of bits read.						*/
/* --Level2		: More general purposes functions that enable 	*/
/* user to define as many read access to bits inside the buffer */
/* as he wants, and that enable him to make calculation between */
/* those read access.											*/
/* --Level3 	: Enable to configure level1 read access from	*/
/* level2 functions results.									*/
/*--------------------------------------------------------------*/

/****************************************************************/
/* Typedef used to locate the position of a bit in RAM,			*/
/* for level2 functions.										*/
/****************************************************************/
typedef struct
{
	unsigned int    *add_p;	/* address of the 24 bits word in RAM */
	int             available;	/* nb bits available in this word [1;24] */
} POSITION_T;

/*-------------------- LEVEL0 FUNCTIONS -----------------------*/

/****************************************************************/
/* --Level0 function--											*/
/* Mandatory init before using any other function.				*/
/* User define the buffer he wants to use.						*/
/* By default, all read & count actions will start at the 		*/
/* beginning of this buffer.									*/
/****************************************************************/
extern void drb_init(unsigned int *buf_add, int buf_size);


/*--------------------- LEVEL1 FUNCTIONS -----------------------*/

/****************************************************************/
/* --Level1 function--											*/
/* Get from 0 to 24 bits, from current position.				*/
/****************************************************************/
extern int drb_getbits(int nbit);
	
/****************************************************************/
/* --Level1 function--											*/
/* Get from 16 to 24 bits, from current position.				*/
/****************************************************************/
extern int drb_getbits_long(int nbit);

/****************************************************************/
/* --Level1 function--											*/
/* Get 1 bit from posdriver position.							*/
/****************************************************************/
extern int drb_get_1_bit(void);

/****************************************************************/
/* --Level1 function--											*/
/* Reset bit counter of driver.									*/
/****************************************************************/
extern void drb_reset_bit_count(void);

/****************************************************************/
/* --Level1 function--											*/
/* Return nb of bits read since last reset of counter.			*/
/****************************************************************/
extern int drb_get_bit_count(void);

/****************************************************************/
/* --Level1 function--											*/
/* Go fast nbit forward.										*/
/****************************************************************/
extern void drb_fast_forward(int nbit);
	
/****************************************************************/
/* --Level1 function--											*/
/* Save position of getbit.										*/
/****************************************************************/
extern void drb_save_getbit(void);
	
/****************************************************************/
/* --Level1 function--											*/
/* Restore position of getbit.									*/
/****************************************************************/
extern void drb_restore_getbit(void);
	
/****************************************************************/
/* --Level1 function--											*/
/* Byte align the read access starting from latest reset of		*/
/* bit_count.													*/
/****************************************************************/
extern void drb_byte_align(void);

/****************************************************************/
/* --Level1 function--											*/
/* Return value of next 24 bits, without moving read pointer.	*/
/****************************************************************/
extern int drb_lookup_24bits(void);
	

/*-------------------- LEVEL2 FUNCTIONS -----------------------*/

/****************************************************************/
/* --Level2 function--											*/
/* Set a position from an address in RAM.						*/
/****************************************************************/
extern void drb_set_pos_from_add(POSITION_T *pos, unsigned int *add);

/****************************************************************/
/* --Level2 function--											*/
/*	Put position inside buffer limit.							*/
/****************************************************************/
extern void drb_check_position(POSITION_T *pos);

/****************************************************************/
/* --Level2 function--											*/
/* Get from 0 to 24 ubits : version 9/12/15 cy.					*/
/****************************************************************/
extern int drb_getbits_pos(POSITION_T *pos, int nbit);

/****************************************************************/
/* --Level2 function--											*/
/* Get 1 ubit : version 8/9 cy.									*/
/****************************************************************/
extern int drb_get_1_bit_pos(POSITION_T *pos);

/****************************************************************/
/* --Level2 function--											*/
/* Get 8 ubits previously aligned : version 8/9 cy.				*/
/****************************************************************/
extern int drb_get_1byte_aligned_pos(POSITION_T *pos, int nbit);

/****************************************************************/
/* --Level2 function--											*/
/* Align position on next byte limit available :  11/16 cy.		*/
/****************************************************************/
extern void drb_byte_align_pos(POSITION_T *pos);

/****************************************************************/
/* --Level2 function--											*/
/* Return number of bits between pos1 & pos2 (pos1 > pos2).		*/
/****************************************************************/
extern int drb_delta_bit(POSITION_T pos1, POSITION_T pos2);

/****************************************************************/
/* --Level2 function--											*/
/* Return position in buffer result of an offset of offsetbit 	*/
/* bits from old position.										*/
/****************************************************************/
extern POSITION_T drb_pos_offset_inbit(POSITION_T pos, int offsetbit);


/*-------------------- LEVEL3 FUNCTIONS ------------------------*/

/****************************************************************/
/* --Level2 function--											*/
/* Initialise posdriver & poscountdriver at beginning of buffer.*/
/****************************************************************/
extern void drb_reset_getbit(void);

/****************************************************************/
/* --Level3 function--											*/
/* Initialise posdriver & poscountdriver from a position.		*/
/****************************************************************/
extern void drb_read_start_at_pos(POSITION_T pos);

/****************************************************************/
/* --Level3 function--											*/
/* Initialise posdriver & poscountdriver from an address in		*/
/* the buffer.													*/
/****************************************************************/
extern void drb_read_start_at_add(unsigned int *add);

/****************************************************************/
/* --Level3 function--											*/
/* Get current position of posdriver.							*/
/****************************************************************/
extern POSITION_T drb_get_pos_of_read(void);
	
/*-------------------- LEVEL5 FUNCTIONS ------------------------*/

/****************************************************************/
/* --Level5 function--											*/
/* Return 1 audio sample of 24 bits.							*/
/****************************************************************/
extern int	drb_get_audio_sample(void);

/****************************************************************/
/* --Level5 function--											*/
/* Return nbit MSB bits of current read address.				*/
/****************************************************************/
extern int	drb_get_left_aligned_sample(int nbit);

/****************************************************************/
/* --Level5 function--											*/
/* Return nbit LSB bits of current read address.				*/
/****************************************************************/
extern int	drb_get_right_aligned_sample(int nbit);

#endif /* driver_getbit.h */
