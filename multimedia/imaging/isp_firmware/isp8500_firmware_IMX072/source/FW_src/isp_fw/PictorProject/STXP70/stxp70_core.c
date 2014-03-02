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
 * File name        : STxP70_core.c                                           *
 * Purpose : global STxP70 system library definition                          *
 * History          : 2004/11/18 - First implementation.                      *
 *		      2005/05/16 - Modify names      			      *	
 *----------------------------------------------------------------------------*/
 
/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 


#include "stxp70_core.h"
#include "stxp70_type.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_STXP70_stxp70_coreTraces.h"
#endif
 
void STxP70_Init(void)
{
    #ifdef SYS_LIB_DEBUG
        printf("\r\n STxP70 System Libray version: %s \r\n"STXP70_SYSTEM_LIB_VERSION);
    #endif  
}

/*----------------------------------------------------------------------------*
 * Name    : STxP70_Set_Iml		                                              *
 * Purpose : This function sets the IML field of STxP70 core sfr registers.    *
 *           It is used to set the current priority and core will accept any  * 
 *           interrupt with higher priority.                                  *
 *----------------------------------------------------------------------------*
 * Inputs  : 								                                  *
 *	new_iml: New IML value to be set.                       	              * 
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  	None							                                      * 
 *----------------------------------------------------------------------------*/
void STxP70_Set_Iml(stxP70_word32 new_iml)
{
    stxP70_word32 sr;
    sr = _asm_readsfr_sr();
    sr = ((sr&(~SR_IML))|(new_iml&SR_IML));
    _asm_writesfr_sr(sr);
}

/*----------------------------------------------------------------------------*
 * Name    : STxP70_Get_Iml		                                              *
 * Purpose : This function gets the IML field of STxP70 core sfr registers.    *
 *----------------------------------------------------------------------------*
 * Inputs  : 								                                  *
 *          None                                                              *  
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  	iml value   					                                      * 
 *----------------------------------------------------------------------------*/
stxP70_word32 STxP70_Get_Iml(void)
{
    stxP70_word32 iml;
    iml = ((_asm_readsfr_sr())&SR_IML);
    return iml;
}

/*----------------------------------------------------------------------------*
 * Name    : STxP70_Set_PrecisionMode		                                  *
 * Purpose : This function sets the PCS.NPE field of STxP70 core sfr          *
 *  *           registers. It is used to set Precise or non-precise exception *
 *           mode.                                                            *
 *----------------------------------------------------------------------------*
 * Inputs  : 								                                  *
 *	PrecisionMode: Precision mode value.                       	                  * 
 *----------------------------------------------------------------------------*
 * Outputs :                                                                  *
 *  	None							                                      * 
 *----------------------------------------------------------------------------*/
void STxP70_Set_PrecisionMode(stxP70_word32 PrecisionMode)
{
    stxP70_word32 pcs;
    pcs = _asm_readsfr_pcs();
    pcs = ((pcs&(~PCS_NONPRE_EXCEP))|(PrecisionMode));
    _asm_writesfr_pcs(pcs);
}
