/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides prototype for CRYP Private functions and structrue
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __CRYP_IRQP_H_
#define __CRYP_IRQP_H_

#include "cryp_irq.h"

#ifdef __cplusplus
extern "C"
{	/* In case C++ needs to use this header.*/
#endif

/*-----------------------------------------------------------------------------
	NOT to be exported.
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------

	CRYP Registers - Offset mapping
 
            +-----------------+
        00h | CRYP_CR         |  Configuration register
            +-----------------+
        04h | CRYP_SR         |  Status register
            +-----------------+
        08h | CRYP_DIN        |  Data In register
            +-----------------+
        0ch | CRYP_DOUT       |  Data out register
            +-----------------+
        10h | CRYP_DMACR      |  DMA control register
            +-----------------+
        14h | CRYP_IMSC       |  IMSC
            +-----------------+
        18h | CRYP_RIS        |  Raw interrupt status
            +-----------------+
        1ch | CRYP_MIS        |  Masked interrupt status.
            +-----------------+
            Key registers
            IVR registers
            Peripheral 
            Cell IDs

        ********Refer data structure for other register map***********    
-----------------------------------------------------------------------------*/
typedef volatile struct  
{
	t_uint32 cnfg;		/* Configuration register   */
	t_uint32 status;    /* Status register          */ 
	t_uint32 din;		/* Data input register      */
	t_uint32 din_size;  /* Data input size register */
	t_uint32 dout;      /* Data output register     */
	t_uint32 dout_size; /* Data output size register */
	t_uint32 dmacr;		/* Dma control register     */
	t_uint32 imsc;      /* Interrupt mask set/clear register*/
	t_uint32 ris;		/* Raw interrupt status             */
	t_uint32 mis;       /* Masked interrupt statu register  */
 	
 	t_uint32 key_1_l;    /*Key register 1 L*/
 	t_uint32 key_1_r;    /*Key register 1 R*/
 	t_uint32 key_2_l;    /*Key register 2 L*/
 	t_uint32 key_2_r;    /*Key register 2 R*/
 	t_uint32 key_3_l;    /*Key register 3 L*/
    t_uint32 key_3_r;    /*Key register 3 R*/
    t_uint32 key_4_l;    /*Key register 4 L*/
    t_uint32 key_4_r;    /*Key register 4 R*/
          
 	t_uint32 init_vect_0_l;     /*init vector 0 L*/
 	t_uint32 init_vect_0_r;     /*init vector 0 R*/
 	t_uint32 init_vect_1_l;     /*init vector 1 L*/
 	t_uint32 init_vect_1_r;     /*init vector 1 R*/

    t_uint32 cryp_unused1[(0x80-0x58)>>2]; /* unused registers */
    t_uint32 itcr;        		/*Integration test control register */
    t_uint32 itip;        		/*Integration test input register */
    t_uint32 itop;        		/*Integration test output register */
	t_uint32 cryp_unused2[(0xFE0-0x8C)>>2]; /* unused registers */
	
    t_uint32 periphId0; /* FE0	CRYP Peripheral Identication Register*/
	t_uint32 periphId1;	/* FE4*/
	t_uint32 periphId2;	/* FE8*/
	t_uint32 periphId3;	/* FEC*/
						
	t_uint32 pcellId0;	/* FF0	CRYP PCell Identication Register*/
	t_uint32 pcellId1;	/* FF4*/
	t_uint32 pcellId2;	/* FF8*/
	t_uint32 pcellId3;	/* FFC*/
} t_cryp_register;

#ifdef __cplusplus
} /*Allow C++ to use this header */
#endif  /* __cplusplus   */
#endif
