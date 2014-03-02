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
 * File name        : STxP70_itc.c                                            *
 * Purpose : Interrupt controller driver                                      *
 * History          : 2004/12/8 - First implementation.                       *
 *		              2005/05/17 - add new functions + renaming of functions  *
 *		              2007/05/01 - Optimization for code size                 *
 *----------------------------------------------------------------------------*/

/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 

#include "stxp70_itc.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_STXP70_stxp70_itcTraces.h"
#endif




/*----------------------------------------------------------------------------*
 * Name    : ITC_Disable_Enable		                                          *
 * Purpose : this function enable/Disable ITs                          	      *
 *	   by setting/clearing ie bits for maskable interrupts		              *
 *         For NMI: it set NMI (if NMI.ie bit is cleared)		              *
 *----------------------------------------------------------------------------*
 * Inputs  : 								                                  *
 *	IDmask: mask for enabling/disabking selected irq numbers	              * 
 *	Enable: bit set or clear for Enable/Disable			                      *     
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  	None							                                      * 
 *----------------------------------------------------------------------------*/
 
void ITC_Disable_Enable(stxP70_word32 IDmask, stxP70_boolean Enable)
{
    stxP70_word32 ReadMask = 0;

    /* Reading of all IE bits of the 31 Maskable interrupts and the NMI IE bit value */
    ReadMask = _asm_get_it_enable();  
    
    if (Enable) IDmask = ReadMask|IDmask; /* Set IE bit / set NMI */
    else IDmask = ReadMask & (~IDmask);   /* Clear IE bit */
   _asm_set_it_enable(IDmask);
} 

/*----------------------------------------------------------------------------*
 * Name    : ITC_Pending			                                          *
 * Purpose : this function return the state of the interrupt line	          *                          	      
 *----------------------------------------------------------------------------*
 * Inputs  : 								                                  *
 *	ID: irq number							                                  *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  	Status of the interrupt (0/1)					                      * 
 *----------------------------------------------------------------------------*/
stxP70_int32 ITC_Pending(stxP70_word32 ID)
{
    stxP70_word32 conf = 0;   
    
    if (ID > IT_NMI) {	
    	return(-100);
	} 
   
    conf = _asm_get_it(ID);
    if (conf&ITC_IST) return 1;
    else return 0;
}

/*----------------------------------------------------------------------------*
 * Name    : ITC_Clear_Set_Pending			                                  *
 * Purpose : this function clear/set IST bit of the interrupt line	          *
 *----------------------------------------------------------------------------*
 * Inputs  : 								                                  *
 *	ID: irq number							                                  * 
 *	Set: boolean to set or clear IST bit   				                      *
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  	0: no problem							                              *
 *	-101 : error 							                                  *
 *----------------------------------------------------------------------------*/
stxP70_int32 ITC_Clear_Set_Pending(stxP70_word32 ID,stxP70_boolean Set)
{
    stxP70_word32 conf = 0;    
 
   if (ID > IT_NMI) {	
    	return(-101);
	}

    conf = _asm_get_it(ID);
    if (Set) conf = conf | ITC_IST; /* Set IST bit */
    else conf = conf & (~ITC_IST); /* Clear IST bit */
    _asm_set_it(ID,conf);
    return 0;
}
	

/*----------------------------------------------------------------------------*
 * Name    : ITC_Set_Level			                                          *
 * Purpose : this function set the priority level of the interrupt line	      *                          	     
 *----------------------------------------------------------------------------*
 * Inputs  : 								                                  *
 *	ID: irq number							                                  *
 *	Level: priority level of the interrupt				                      *			   
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  	0: no problem							                              *
 *	-102: error							                                      *
 *----------------------------------------------------------------------------*/
stxP70_int32 ITC_Set_Level(stxP70_word32 ID, stxP70_word32 Level)
{
    stxP70_word32 conf = 0;    
   
   if (ID > IT_NMI) {	
    	return(-102);
	}
	
   if (ID!=IT_NMI) {
   	conf = _asm_get_it(ID);
   	STXP70_WRITE_BIT(conf,Level,ITC_IL); /* Set IL bits */
    _asm_set_it(ID,conf);
   	}
    return 0;
}

/*------------------------------------------------------------------------------------*
 * Name    : ITC_Clear_Set_Synchro			                                          *
 * Purpose : this function clear/set the synchronization bit of the interrupt line    *                          	     
 *------------------------------------------------------------------------------------*
 * Inputs  : 								                                          *
 *	ID: irq number							                                          *
 *	Set: flag to clear/set synchronization bit isy				                      *
 *------------------------------------------------------------------------------------*
 * Outputs :                                                                          *
 *  	0: no problem								                                  *
 *      -103: error								                                      *
 *------------------------------------------------------------------------------------*/
stxP70_int32 ITC_Clear_Set_Synchro(stxP70_word32 ID, stxP70_boolean Set)
{
    stxP70_word32 conf = 0;    
   
   if (ID > IT_NMI) {	
    	return(-103);
	}
   conf = _asm_get_it(ID);	
   if (Set) STXP70_SET_BIT(conf,ITC_ISY); /* Set ISY bit */
   else STXP70_CLEAR_BIT(conf,ITC_ISY);
 
    _asm_set_it(ID,conf);
    return 0; 
}



/*------------------------------------------------------------------------------------*
 * Name    : ITC_Init_StaticIrq			                                                  *
 * Purpose : this function initializes the handler interrupt, set the interrupt	      *
 * line configuration by initializing the Level and setting/clearing the synchro bit  *                     	     
 *------------------------------------------------------------------------------------*
 * Inputs  : 									                                      *
 *	Handler: address of the Handler						                              *
 *	base: base address of the vector table					                          *
 *	ID: irq number							                                          *
 *      Level: priority Level of interrupt ID					                      *
 *	Synchro: synchronisation bit set/clear					                          *
 *------------------------------------------------------------------------------------*
 * Outputs :                                                                          *
 *  	0: No problem							                                      *
 *	-105: error								                                          *
 *------------------------------------------------------------------------------------*/
void ITC_Init_StaticIrq(stxP70_tRegister ID, stxP70_tRegister Level, stxP70_boolean Synchro, stxP70_boolean Enable, stxP70_boolean Trigger)
{
  stxP70_tRegister conf = 0; 
	
	conf = _asm_get_it(ID);
  
	if (ID!=IT_NMI)
	{
		/* Set priority Level of interrupt line */
		STXP70_WRITE_BIT(conf,Level,ITC_IL); /* Set IL bits */
	  
		/* Set/Clear synchronization bit of interrupt line */
		if (Synchro)
		{
			STXP70_SET_BIT(conf,ITC_ISY); /* Set ISY bit */
		}
		else
		{
			STXP70_CLEAR_BIT(conf,ITC_ISY);
		}
		
		/* Enable the IRQ if  Enable = 1 */
		if (Enable)
		{
			STXP70_SET_BIT(conf,ITC_IE);
		}
		else
		{
			STXP70_CLEAR_BIT(conf,ITC_IE);
		}

		/* Clear IST bit of interrupt line if Clear_Ist_Enable=1 */
		if (Trigger)
		{
			STXP70_SET_BIT(conf,ITC_IST);
		}
		else
		{
			STXP70_CLEAR_BIT(conf,ITC_IST);
		}
	  
  }
    
	/* Write the configuration in the MIC/NMIC register */	
	_asm_set_it(ID,conf);
   	_asm_barrier(); 	
}
