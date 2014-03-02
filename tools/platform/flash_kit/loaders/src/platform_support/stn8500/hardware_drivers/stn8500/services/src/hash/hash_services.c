/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides some support routines for the test environnement
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hash_services.h"
#include "services.h"

/*--------------------------------------------------------------------------*
 * Public functions: Called by services.c									*
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	SER_HASH_Init											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:	This routine initialize the HASH						*/
/*					1. Set HASH base address.								*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/*      IN      :   hash_device_id  : HASH number to be initialized         */
/*                                                                          */
/* RETURN		:	void													*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	: 	Non Re-entrant                                          */
/* COMMENTS		:	Should be done after enabling VIC services				*/
/****************************************************************************/
PUBLIC void SER_HASH_Init(IN t_uint8 hash_device_id)
{
    t_hash_error    hash_error;

    //if(HASH_DEVICE_ID_0 == hash_device_id)
    if(INIT_HASH0 == hash_device_id)
    {
    	*((volatile t_uint32 *)PRCC_6_CTRL_REG_BASE_ADDR) |= 0x00000004;
    	*((volatile t_uint32 *)(PRCC_6_CTRL_REG_BASE_ADDR + 0x08)) |= 0x00000004;

    	/* Setting base address */
        hash_error = HASH_Init(HASH_DEVICE_ID_0,HASH_0_BASE_ADDR);
        if (HASH_OK != hash_error)
        {
            PRINT("Error SER_HASH: From SER_HASH_Init: %x\n", hash_error);
            return;
        }
    }
    //else if(HASH_DEVICE_ID_1 == hash_device_id)
    else if(INIT_HASH1 == hash_device_id)
    {
    	*((volatile t_uint32 *)PRCC_6_CTRL_REG_BASE_ADDR) |= 0x00000020;
    	*((volatile t_uint32 *)(PRCC_6_CTRL_REG_BASE_ADDR + 0x08)) |= 0x00000020;

    	/* Setting base address */
        hash_error = HASH_Init(HASH_DEVICE_ID_1,HASH_1_BASE_ADDR);
        if (HASH_OK != hash_error)
        {
            PRINT("Error SER_HASH: From SER_HASH_Init: %x\n", hash_error);
            return;
        }
    }        
        
}

/****************************************************************************/
/* NAME			:	SER_HASH_Close											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:	This routine close services for HASH					*/
/*                                                                          */
/* PARAMETERS	:	void													*/
/*                                                                          */
/* RETURN		:	void													*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	: 	Non Re-entrant                                          */
/* COMMENTS		:	GPIO alternate function need not be disabled for MEKs	*/
/****************************************************************************/
PUBLIC void SER_HASH_Close(IN t_uint8 hash_device_id)
{
    /* Disabling the Secured and Privileged access */
   // ser_hash_DisablePrivilegeAccess();

    if(INIT_HASH0 == hash_device_id)
    {
    	/* Reset the HASH global variables and put Power on reset values to registers */
        if (HASH_OK != HASH_Reset(HASH_DEVICE_ID_0))
        {
            PRINT("Error HASH_SER: From SER_HASH_Close. Proceeding anyway\n");
        }
        else
        {
        	PRINT("Info SER_HASH0 : SER_HASH0_Close : Exit\n");
        }
        
    }
    else if(INIT_HASH1 == hash_device_id)
    {
    	/* Reset the HASH global variables and put Power on reset values to registers */
        if (HASH_OK != HASH_Reset(HASH_DEVICE_ID_1))
        {
            PRINT("Error HASH_SER: From SER_HASH_Close. Proceeding anyway\n");
        }
        else
        {
        	PRINT("Info SER_HASH1 : SER_HASH1_Close : Exit\n");
        }
    }

    
}
#if 0 
/****************************************************************************/
/* NAME			:	ser_hash_DisablePrivilegeAccess 			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:	Disable Privilege Access                                */
/* PARAMETERS	:															*/
/* IN           :	void         :                                          */
/*                                                                          */
/* RETURN VALUE	:	None													*/
/* TYPE			: 															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY	:	NA														*/
/****************************************************************************/
PRIVATE void ser_hash_DisablePrivilegeAccess(void)
{
    t_uint32    p_old_datum, p_null_pointer;

    t_vic_error vic_error;

    vic_error = VIC_DisableItLine(VIC_DMA_LINE);
    vic_error = VIC_ChangeDatum(VIC_DMA_LINE, (t_uint32) ser_hash_MoverToIrqMode, &p_old_datum);
    vic_error = VIC_SetSoftItLine(VIC_DMA_LINE);
    vic_error = VIC_EnableItLine(VIC_DMA_LINE);

    vic_error = VIC_DisableItLine(VIC_DMA_LINE);
    vic_error = VIC_ChangeDatum(VIC_DMA_LINE, p_old_datum, &p_null_pointer);
    vic_error = VIC_EnableItLine(VIC_DMA_LINE);

    vic_error = vic_error;  /* Dummy read to remove the PC lint warning */
    
}

/****************************************************************************/
/* NAME			:	ser_hash_MoverToIrqMode 			                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:	DMA Interrupt handaler                                  */
/* PARAMETERS	:															*/
/* IN           :	None                                                    */
/*                                                                          */
/* RETURN VALUE	:	None													*/
/* TYPE			: 															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY	:	NA														*/
/****************************************************************************/
PRIVATE void ser_hash_MoverToIrqMode(void)
{
    t_hash_protection_config    protection_config;
   
    t_hash_error                hash_error;
    t_vic_error                 vic_error;

    protection_config.hash_privilege_access = HASH_STATE_ENABLE;
    protection_config.hash_secure_access = HASH_STATE_ENABLE;
    hash_error = HASH_ConfigureProtection(&protection_config);
    vic_error = VIC_ClearSoftItLine(VIC_DMA_LINE);
    vic_error = VIC_AcknowledgeItLine(VIC_DMA_LINE);

    hash_error = hash_error;    /* Dummy read to remove the PC lint warning */
    vic_error = vic_error;      /* Dummy read to remove the PC lint warning */
}
#endif
/* End of file - hash_services.c */

