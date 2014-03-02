/*----------------------------------------------------------------------------*
 *      Copyright 2005, STMicroelectronics, Incorporated.                     *
 *      All rights reserved.                                                  *
 *                                                                            *
 *        STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION            *
 * This software is supplied under the terms of a license agreement or nondis-*
 * closure agreement with STMicroelectronics and may not be copied or disclo- *
 * sed except in accordance with the terms of that agreement.                 *
 *----------------------------------------------------------------------------*
 * System           : STxP70                                                  *
 * Project component: System Library                                          *
 * File name        : STxP70_itc_asm.s                                        *
 * Purpose : General assembly function for STxP70 ITC X3 extension            *
 * History          : 2005/05/17 - First implementation.                      *
 *----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*
 * Name    : _asm_set_it		                                      *
 * Purpose : this function initialize the interrupt line irqn                 *
 *	     Initialization of the MICn register			      *
 *----------------------------------------------------------------------------*
 * Inputs  :                                                                  *
 *  irqn   (r0) : line number                                                 *
 *  conf   (r1)	: configuration (level/ie/isy/ist)                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *      none                                                                  *
 *----------------------------------------------------------------------------*/
 
.text
.entry
.globl _asm_set_it
_asm_set_it:
	mover2ici r0,r1
	rts
.type _asm_set_it,@function
.size _asm_set_it,_asm_set_it-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_set_nmi		                                      *
 * Purpose : this function initialize the nmi                                 *
 *	     Initialization of the NMIC register			      *
 *----------------------------------------------------------------------------*
 * Inputs  :                                                                  *
 *  conf	: configuration (level/ie/isy/ist)                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *      none                                                                  *
 *----------------------------------------------------------------------------*/

.text
.entry
.globl _asm_set_nmi
_asm_set_nmi:
	mover2ic ir31,r0
	rts
.type _asm_set_nmi,@function
.size _asm_set_nmi,_asm_set_nmi-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_set_it_enable		                                      *
 * Purpose : this function set the IE bit of selected irqn   	              *			     
 *----------------------------------------------------------------------------*
 * Inputs  :                                                                  *                                      
 *  IDmask	: mask to select none/one/several/all interrupt lines for     *
 *		  which IE bit will be set				      *
 *		  and to enable NMI 					      *			     
 *		set/clear ie bit of MIC register			      *
 *		set NMI ie bit						      *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *      none                                                                  *
 *----------------------------------------------------------------------------*/
 
 .text
.entry
.globl _asm_set_it_enable
_asm_set_it_enable:
	mover2ie r0
	rts
.type _asm_set_it_enable,@function
.size _asm_set_it_enable,_asm_set_it_enable-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_get_it  		                                      *
 * Purpose : this function get the configuration of the interrupt irqn	      *			     
 *----------------------------------------------------------------------------*
 * Inputs  :                                                                  *                                      
 *  irqn    (r0) : line number                                                *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  conf (r0)	: configuration of MIC register(level/ie/isy/ist)             *                                                                   
 *----------------------------------------------------------------------------*/

.text
.entry
.globl _asm_get_it
_asm_get_it:
	moveic2ri r1,r0
	add r0, r1, 0
	rts
.type _asm_get_it,@function
.size _asm_get_it,_asm_get_it-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_get_nmi		                                      *
 * Purpose : this function get the nmi configuration                          *
 *	     get the NMIC register value				      *
 *----------------------------------------------------------------------------*
 * Inputs  : none                                                             *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  conf	: configuration of NMIC register(level/ie/isy/ist)            *    
 *----------------------------------------------------------------------------*/

.text
.entry
.globl _asm_get_nmi
_asm_get_nmi:
	moveic2r r0,ir31
	rts
.type _asm_get_nmi,@function
.size _asm_get_nmi,_asm_get_nmi-.

/*----------------------------------------------------------------------------*
 * Name    : _asm_get_it_enable		                                      *
 * Purpose : this function get the enable status of the 32 interrupt lines    *
 *	    								      *
 *----------------------------------------------------------------------------*
 * Inputs  :  none                                                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  IDmask	: mask containing the values of ie bits			      *
 *----------------------------------------------------------------------------*/

.text
.entry
.globl _asm_get_it_enable
_asm_get_it_enable:
	moveie2r r0
	rts
.type _asm_get_it_enable,@function
.size _asm_get_it_enable,_asm_get_it_enable-.