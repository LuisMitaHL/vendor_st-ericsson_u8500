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
 * File name        : STxP70_core_asm.s                                       *
 * Purpose : General assembly function for STxP70 system library              *
 * History          : 2004/11/18 - First implementation.                      *
 *		              2005/05/16 - Added idle instruction + modify names      *	
 *                    2007/10/12 - Added headers to functions.                *
 *----------------------------------------------------------------------------*/
 //General Assembly function for STxP70 system library



/*----------------------------------------------------------------------------*
 * Name    : _asm_readsfr_sr		                                          *
 * Purpose : this function reads the sr register of STxP70 core.              *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  none                                                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *    Read value                                                              *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_readsfr_sr
_asm_readsfr_sr:
    movesfr2r r0,sr
	rts
.type _asm_readsfr_sr,@function
.size _asm_readsfr_sr,_asm_readsfr_sr-.

/*----------------------------------------------------------------------------*
 * Name    : _asm_readsfr_pcs		                                          *
 * Purpose : this function reads the pcs register of STxP70 core.             *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  none                                                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *    Read value                                                              *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_readsfr_pcs
_asm_readsfr_pcs:
    movesfr2r r0,pcs
	rts
.type _asm_readsfr_pcs,@function
.size _asm_readsfr_pcs,_asm_readsfr_pcs-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_writesfr_sr		                                          *
 * Purpose : this function writes the sr register of STxP70 core.             *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  value to write                                                  *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *       None                                                                 *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_writesfr_sr
_asm_writesfr_sr:
    mover2sfr sr,r0
	rts
.type _asm_writesfr_sr,@function
.size _asm_writesfr_sr,_asm_writesfr_sr-.

/*----------------------------------------------------------------------------*
 * Name    : _asm_mover2e_psta		                                              *
 * Purpose : this function writes the PSTA register of PCACHE                 *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  value to write                                                  *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *       None                                                                 *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_mover2e_psta
_asm_mover2e_psta:
    //lw psta,r0
    mover2e ER24, r0
	rts
.type _asm_mover2e_psta,@function
.size _asm_mover2e_psta,_asm_mover2e_psta-.
/*----------------------------------------------------------------------------*
 * Name    : _asm_writesfr_pcs		                                          *
 * Purpose : this function writes the pcs register of STxP70 core.            *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  value to write                                                  *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *       None                                                                 *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_writesfr_pcs
_asm_writesfr_pcs:
    mover2sfr pcs,r0
	rts
.type _asm_writesfr_pcs,@function
.size _asm_writesfr_pcs,_asm_writesfr_pcs-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_barrier		                                              *
 * Purpose : This function calls the barrier instruction of STxP70 core       *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  None                                                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  None                                                                      *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_barrier
_asm_barrier:
	barrier
	rts
.type _asm_barrier,@function
.size _asm_barrier,_asm_barrier-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_idle_mode0		                                          *
 * Purpose : This functions sets the core to idle mode 0                      *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  None                                                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  None                                                                      *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_idle_mode0
_asm_idle_mode0:
	idle 0
	rts
.type _asm_idle_mode0,@function
.size _asm_idle_mode0,_asm_idle_mode0-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_idle_mode1		                                          *
 * Purpose : This functions sets the core to idle mode 1                      *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  None                                                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  None                                                                      *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_idle_mode1
_asm_idle_mode1:
	idle 1
	rts
.type _asm_idle_mode1,@function
.size _asm_idle_mode1,_asm_idle_mode1-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_idle_mode2		                                          *
 * Purpose : This functions sets the core to idle mode 2                      *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  None                                                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  None                                                                      *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_idle_mode2
_asm_idle_mode2:
	idle 2
	rts
.type _asm_idle_mode2,@function
.size _asm_idle_mode2,_asm_idle_mode2-.


/*----------------------------------------------------------------------------*
 * Name    : _asm_idle_mode3		                                          *
 * Purpose : This functions sets the core to idle mode 3                      *
 *	    								                                      *
 *----------------------------------------------------------------------------*
 * Inputs  :  None                                                            *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  None                                                                      *
 *----------------------------------------------------------------------------*/
.text
.entry
.globl _asm_idle_mode3
_asm_idle_mode3:
	idle 3
	rts
.type _asm_idle_mode3,@function
.size _asm_idle_mode3,_asm_idle_mode3-.
