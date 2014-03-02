/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides HCL for the CRYP (Cryptographic processor)
*  Specification release related to this implementation: A_V2.2
*  Reference : Software Code Development, C Coding Rules, Guidelines 
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#include "cryp_p.h"

#ifdef __DEBUG

#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_CRYP
#define MY_DEBUG_ID             myDebugID_CRYP
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = CRYP_HCL_DBG_ID;

#endif /* __DEBUG */


PRIVATE t_cryp_system_context    g_cryp_system_context;

/*--------------------------------------------------------------------------*/
/*       NAME : CRYP_Init()                                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the CRYP registers, checks         */
/*              Peripheral and PCell Id                            .        */
/* PARAMETERS :                                                             */
/*        IN  : cryp_device_id                                              */
/*        IN  : cryp_base_address:CRYP registers base addres                */
/*         OUT  : None                                                      */
/*                                                                          */
/*       RETURN : CRYP_OK if peripheral id match with the defined ids.      */
/*              CRYP_UNSUPPORTED_HW if ids dont match with the specified ids*/
/*                                                                          */
/*  REENTRANCY: Non Re-entrant                                              */
/*--------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_Init(IN t_cryp_device_id cryp_device_id, IN t_logical_address cryp_base_address)
{  
    t_cryp_error error = CRYP_UNSUPPORTED_HW;
 
    DBGENTER1("CRYP logical address to set at (%lx)",cryp_base_address);

    if(NULL == cryp_base_address)
    {
        error = CRYP_INVALID_PARAMETER;
        DBGEXIT0(error);
        return error ;
    }   
    g_cryp_system_context.p_cryp_reg[cryp_device_id] = (t_cryp_register*)cryp_base_address;
    
    /* Check Peripheral and Pcell Id Register for CRYP */
    if ((CRYP_PERIPHERAL_ID0 == g_cryp_system_context.p_cryp_reg[cryp_device_id]->periphId0) &&
        (CRYP_PERIPHERAL_ID1 == g_cryp_system_context.p_cryp_reg[cryp_device_id]->periphId1) &&
        (CRYP_PERIPHERAL_ID2 == g_cryp_system_context.p_cryp_reg[cryp_device_id]->periphId2) &&
        (CRYP_PERIPHERAL_ID3 == g_cryp_system_context.p_cryp_reg[cryp_device_id]->periphId3) &&

        (CRYP_PCELL_ID0 == g_cryp_system_context.p_cryp_reg[cryp_device_id]->pcellId0) &&
        (CRYP_PCELL_ID1 == g_cryp_system_context.p_cryp_reg[cryp_device_id]->pcellId1) &&
        (CRYP_PCELL_ID2 == g_cryp_system_context.p_cryp_reg[cryp_device_id]->pcellId2) &&
        (CRYP_PCELL_ID3 == g_cryp_system_context.p_cryp_reg[cryp_device_id]->pcellId3))
    {
        error = CRYP_OK ;
    }
    else
    {
        error = CRYP_UNSUPPORTED_HW;
        DBGEXIT0(error);
        return error ;
    }        

    DBGEXIT0(error);
    return error ;
}

/*---------------------------------------------------------------------------*/
/*         NAME :    CRYP_SetDbgLevel()                                      */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different             */
/*                debug comment levels                                       */
/* PARAMETERS :                                                              */
/*         IN : cryp_dbg_level:identify CRYP debug level                     */
/*          OUT : None                                                       */
/*                                                                           */
/*       RETURN : CRYP_OK    : Always.                                       */
/*  REENTRANCY: Non-reentrant                                                */
/*---------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_SetDebugLevel(IN t_dbg_level cryp_dbg_level)
{
    t_cryp_error error;
    
    DBGENTER1("CRYP Setting Debug Level to %x",cryp_dbg_level);
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = cryp_dbg_level;
#endif
    error = CRYP_OK ;

    DBGEXIT0(error);
    return error;

}

/*---------------------------------------------------------------------------*/
/*         NAME :    CRYP_Reset()                                            */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine loads the cryp register with the default values */
/* PARAMETERS :                                                              */
/*          IN  : cryp_device_id                                             */
/*          OUT : None                                                       */
/*                                                                           */
/*       RETURN : CRYP_OK    : Always.                                       */
/*  REENTRANCY: Non-reentrant                                                */
/*---------------------------------------------------------------------------*/

PUBLIC void CRYP_Reset(IN t_cryp_device_id cryp_device_id)
{
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->dmacr,CRYP_DMACR_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->imsc,CRYP_IMSC_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->din,CRYP_DIN_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->dout,CRYP_DOUT_DEFAULT);
    
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_1_l,CRYP_KEY_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_1_r,CRYP_KEY_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_2_l,CRYP_KEY_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_2_r,CRYP_KEY_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_3_l,CRYP_KEY_DEFAULT);        
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_3_r,CRYP_KEY_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_0_l,CRYP_INIT_VECT_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_0_r,CRYP_INIT_VECT_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_4_l,CRYP_KEY_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_4_r,CRYP_KEY_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_1_l,CRYP_INIT_VECT_DEFAULT);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_1_r,CRYP_INIT_VECT_DEFAULT);

    /*Last step since the protection mode bits need to be modified.*/
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,CRYP_CR_DEFAULT);
}

/*--------------------------------------------------------------------------*/
/*    NAME        : CRYP_GetVersion                                         */
/*--------------------------------------------------------------------------*/
/*    DESCRIPTION : Get the version of this CRYP HCL API.                   */
/*    IN            : None                                                  */
/*    INOUT        : None                                                   */
/*    OUT            : p_version        : The current software version.     */
/*    RETURN        : CRYP_INVALID_PARAMETER : if NULL pointer is passed.   */
/*                  CRYP_OK : Noraml operation                              */
/*    REENTRANCY    : Non-reentrant                                         */
/*--------------------------------------------------------------------------*/
PUBLIC t_cryp_error CRYP_GetVersion(OUT t_version *p_version)
{
    DBGENTER1("%lx",(t_uint32)p_version);
    
    /* Check if parameter is valid.*/
    if (NULL == p_version)
    {
        DBGEXIT0(CRYP_INVALID_PARAMETER);
        return CRYP_INVALID_PARAMETER;
    }
    
    p_version->version = CRYP_HCL_VERSION_ID;
    p_version->major = CRYP_HCL_MAJOR_ID;
    p_version->minor = CRYP_HCL_MINOR_ID;
    

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}


/*--------------------------------------------------------------------------*/
/*         NAME :    CRYP_Activity()                                        */
/*--------------------------------------------------------------------------*/
/*   DESCRIPTION: This routine enables/disable the cryptography function.   */
/*                                                                          */
/*   PARAMETERS :                                                           */ 
/*           IN : cryp_device_id                                            */
/*           IN : cryp_activity: Enable/Disable functionality               */
/*          OUT : None                                                      */
/*       RETURN :    CRYP_OK    : Always                                    */
/*                                                                          */
/*  REENTRANCY: Non-Reentrant                                               */    
/*--------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_Activity(IN t_cryp_device_id cryp_device_id, t_cryp_activity cryp_activity)
{

    DBGENTER1("Enable disable the cryp functionality %x",cryp_activity);

    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,cryp_activity,CRYP_ACTIVITY_POS,CRYP_ACTIVITY_MASK);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

/*--------------------------------------------------------------------------*/
/*         NAME :    CRYP_Start()                                           */
/*--------------------------------------------------------------------------*/
/*   DESCRIPTION: This routine starts the computation in case of XTS or CBC */
/*                in case of stealing.                                      */
/*   PARAMETERS :                                                           */ 
/*           IN : cryp_device_id                                            */
/*           IN : cryp_start: Enable/Disable functionality                  */
/*          OUT : None                                                      */
/*       RETURN :    CRYP_OK    : Always                                    */
/*                                                                          */
/*  REENTRANCY: Non-Reentrant                                               */    
/*--------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_Start(IN t_cryp_device_id cryp_device_id, t_cryp_start cryp_start)
{

    DBGENTER1("Starts the cryp computation %x",cryp_start);

    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,cryp_start,CRYP_START_POS,CRYP_START_MASK);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

/*--------------------------------------------------------------------------*/
/*         NAME :    CRYP_InitSignal()                                      */
/*--------------------------------------------------------------------------*/
/*   DESCRIPTION: This routine submit the initialization values.            */
/*                                                                          */
/*   PARAMETERS :                                                           */ 
/*           IN : cryp_device_id                                            */
/*           IN : cryp_initbit: Enable/Disable init signal                  */
/*          OUT : None                                                      */
/*       RETURN :    CRYP_OK    : Always                                    */
/*                                                                          */
/*  REENTRANCY: Non-Reentrant                                               */    
/*--------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_InitSignal(IN t_cryp_device_id cryp_device_id, t_cryp_init cryp_initbit)
{

    DBGENTER1("Enable disable the cryp submit initialize values %x",cryp_initbit);

    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,cryp_initbit,CRYP_INIT_POS,CRYP_INIT_MASK);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

/*--------------------------------------------------------------------------*/
/*         NAME :    CRYP_KeyPreparation()                                         */
/*--------------------------------------------------------------------------*/
/*   DESCRIPTION: This routine prepares key for decryption.                 */
/*                                                                          */
/*   PARAMETERS :                                                           */ 
/*           IN : cryp_device_id                                            */
/*           IN : cryp_prepkey: Enable/Disable                              */
/*          OUT : None                                                      */
/*       RETURN :    CRYP_OK    : Always                                    */
/*                                                                          */
/*  REENTRANCY: Non-Reentrant                                               */    
/*--------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_KeyPreparation(IN t_cryp_device_id cryp_device_id, t_cryp_key_prep cryp_prepkey)
{

    DBGENTER1("Enable Disable the cryp preparation %x",cryp_prepkey);

    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,cryp_prepkey,CRYP_KSE_POS,CRYP_KSE_MASK);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

/*--------------------------------------------------------------------------*/
/*         NAME :    CRYP_FlushInOutFifo()                                  */
/*--------------------------------------------------------------------------*/
/*   DESCRIPTION: This function reset both the input and the output FIFOs.  */
/*                                                                          */
/*   PARAMETERS :                                                           */
/*           IN : cryp_device_id                                            */
/*          OUT : None                                                      */
/*                                                                          */
/*       RETURN :    CRYP_OK : Always                                       */
/*    REENTRANCY: Re-entrant                                                */
/*--------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_FlushInOutFifo(IN t_cryp_device_id cryp_device_id)
{
    DBGENTER1("Flush inout FIFO of cryp %x",CRYP_FlushInOutFifo);  

    CRYP_SET_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg, CRYP_FIFO_FLUSH_MASK);
    
    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

/*----------------------------------------------------------------------------*/
/*         NAME :    CRYP_SetConfiguration()                                         */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: This routine set the cnfg CRYP IP.                            */
/*                                                                                */
/* PARAMETERS :                                                                  */
/*         IN : cryp_device_id                                                    */
/*         IN : p_cryp_config : Pointer to the configuration parameter        */
/*        OUT : None                                                               */
/*                                                                               */
/*     RETURN :                                                                 */
/*            CRYP_INVALID_PARAMETER: If NULL == p_cryp_config.               */
/*            CRYP_OK: Ohterwise                                              */
/*  REENTRANCY: Non reentrant                                                  */
/*----------------------------------------------------------------------------*/
                                                                              
PUBLIC t_cryp_error CRYP_SetConfiguration(IN t_cryp_device_id cryp_device_id, IN t_cryp_config *p_cryp_config)
{
    t_cryp_error error = CRYP_OK;
    
    DBGENTER1("Control setting for CRYP %x",p_cryp_config);

    if (NULL == p_cryp_config)
    {
        error=CRYP_INVALID_PARAMETER;
        DBGEXIT0(error);
        return error;
    }

    /*Since more than one bit is written macro put_bits is used*/
    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,p_cryp_config->key_access ,CRYP_KEY_ACCESS_POS,CRYP_KEY_ACCESS_MASK);
    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,p_cryp_config->key_size   ,CRYP_KEY_SIZE_POS  ,CRYP_KEY_SIZE_MASK);
    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,p_cryp_config->data_type  ,CRYP_DATA_TYPE_POS ,CRYP_DATA_TYPE_MASK);

    /* Prepare key for decryption */
    if((CRYP_ALGORITHM_DECRYPT == p_cryp_config->encrypt_or_decrypt) && 
                                    ((CRYP_ALGO_AES_ECB == p_cryp_config->algo_mode)||
                                     (CRYP_ALGO_AES_CBC == p_cryp_config->algo_mode)))
    {
        CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg ,CRYP_ALGO_AES_ECB  ,CRYP_ALGO_POS  ,CRYP_ALGO_MASK);
        CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,CRYP_ACTIVITY_ENABLE,CRYP_ACTIVITY_POS,CRYP_ACTIVITY_MASK);
        CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg ,KSE_ENABLED  ,CRYP_KSE_POS  ,CRYP_KSE_MASK);
        while ((t_bool) ((g_cryp_system_context.p_cryp_reg[cryp_device_id]->status & CRYP_SR_BUSY_MASK) >> CRYP_SR_BUSY_POS))
            ; 
        CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,CRYP_ACTIVITY_DISABLE,CRYP_ACTIVITY_POS,CRYP_ACTIVITY_MASK);
    }
    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,p_cryp_config->algo_mode  ,CRYP_ALGO_POS  ,CRYP_ALGO_MASK); 
    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,p_cryp_config->encrypt_or_decrypt,CRYP_ENC_DEC_POS,CRYP_ENC_DEC_MASK);

    DBGEXIT0(error);
    return error;
}

/*---------------------------------------------------------------------------*/
/*         NAME :    CRYP_GetConfiguration()                                      */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the parameter of the control register of IP*/
/*                                                                               */
/* PARAMETERS :                                                                 */
/*         IN : cryp_device_id                                                    */
/*        OUT : p_cryp_config: Gets the configuration parameter from cryp ip.     */
/*                                                                              */
/*     RETURN :  CRYP_INVALID_PARAMTER: if NULL==p_cryp_config                 */
/*              CRYP_OK: Otherwise.                                          */                    
/*  REENTRANCY: Non reentrant                                                 */
/*---------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_GetConfiguration(IN t_cryp_device_id cryp_device_id, IN t_cryp_config *p_cryp_config)
{
    t_cryp_error error = CRYP_OK;
    
    DBGENTER1("Get control setting of cryp register in  %lx",p_cryp_config);

    if (NULL == p_cryp_config)
    {
        error=CRYP_INVALID_PARAMETER;
        DBGEXIT0(error);
        return error;
    } 

    p_cryp_config->key_access = (((g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg) & CRYP_KEY_ACCESS_MASK) ?CRYP_STATE_ENABLE :CRYP_STATE_DISABLE);
    p_cryp_config->key_size   = (t_cryp_key_size )(((g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg) & CRYP_KEY_SIZE_MASK) >> CRYP_KEY_SIZE_POS) ;

    p_cryp_config->encrypt_or_decrypt = (((g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg) & CRYP_ENC_DEC_MASK) ? CRYP_ALGORITHM_DECRYPT:CRYP_ALGORITHM_ENCRYPT);

    p_cryp_config->data_type  = (t_cryp_data_type)(((g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg) & CRYP_DATA_TYPE_MASK) >> CRYP_DATA_TYPE_POS);
    p_cryp_config->algo_mode  = (t_cryp_algo_mode)(((g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg) & CRYP_ALGO_MASK) >> CRYP_ALGO_POS);
        
    DBGEXIT0(error);
    return (error);
}

/*---------------------------------------------------------------------------*/
/*         NAME :    CRYP_ConfigureProtection                                       */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine set the protection bits in the CRYP logic.      */
/* PARAMETERS :                                                                 */
/*         IN : cryp_device_id                                             */
/*         IN : p_protect_config:Pointer to the protection mode and secure   */
/*              mode configuration.                                          */
/*        OUT : None                                                              */
/*                                                                              */
/*     RETURN : CRYP_INVALID_PARAMETER: If NULL == p_protect_config.           */
/*            CRYP_OK: Otherwise                                             */
/*  REENTRANCY: Non reentrant                                                 */
/*---------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_ConfigureProtection(IN t_cryp_device_id cryp_device_id, t_cryp_protection_config *p_protect_config)
{   
    
    DBGENTER1("Pointer for the protection config %x",p_protect_config);

    if (NULL == p_protect_config)
    {
        DBGEXIT0(CRYP_INVALID_PARAMETER);
        return CRYP_INVALID_PARAMETER;
    }

    CRYP_WRITE_BIT(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,(t_uint32)p_protect_config->secure_access,CRYP_SECURE_MASK);
    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,p_protect_config->privilege_access,CRYP_PRLG_POS,CRYP_PRLG_MASK);

    DBGEXIT0(CRYP_OK);
    return(CRYP_OK);
}

/*--------------------------------------------------------------------------*/
/*       NAME :    CRYP_IsLogicBusy                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the busy status of the CRYP logic      */
/* PARAMETERS :                                                                */
/*         IN : cryp_device_id                                             */
/*        OUT : None                                                            */
/*                                                                             */
/*     RETURN : TRUE / FALSE If the cryp logic is busy/ free.                 */
/*               CRYP_REQUEST_NOT_APPLICABLE : Wrong bank number.                */
/*  REENTRANCY: Non reentrant                                                */
/*--------------------------------------------------------------------------*/

PUBLIC t_bool CRYP_IsLogicBusy(IN t_cryp_device_id cryp_device_id)
{    
    t_bool status;

    DBGENTER1("Checking for status register %x",CRYP_IsLogicBusy);
    status= (t_bool)(CRYP_TEST_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->status,CRYP_BUSY_STATUS_MASK)? TRUE:FALSE);

    DBGEXIT0(status);
    return (status);    
}

/*-----------------------------------------------------------------------------*/
/*    NAME        : CRYP_GetStatus                                             */
/*-----------------------------------------------------------------------------*/
/*    DESCRIPTION : This routine returns the complete status of the cryp logic */
/*    IN          : cryp_device_id                                             */
/*    INOUT       : None                                                       */
/*    OUT         : None                                                       */
/*    RETURN      : t_cryp_status (ORed values of cryp FIFO & Busy status)     */
/*    REENTRANCY  : Non-reentrant                                              */
/*-----------------------------------------------------------------------------*/

PUBLIC  t_cryp_status CRYP_GetStatus(IN t_cryp_device_id cryp_device_id)
{
    t_cryp_status status;

    DBGENTER1("Get Cryp status %x",CRYP_GetStatus);

    status = (t_uint32)g_cryp_system_context.p_cryp_reg[cryp_device_id]->status;

    DBGEXIT0(status);
    return (status);
}

/*--------------------------------------------------------------------------*/
/*    NAME        : CRYP_ConfigureForDma                                    */
/*--------------------------------------------------------------------------*/
/*    DESCRIPTION : This routine configures the CRYP IP for DMA operation.  */
/*    IN            :  cryp_device_id                                       */
/*    IN            : dma_req: Specifies the DMA request type value.        */
/*                  mem_type                                                */
/*    INOUT        : None                                                   */
/*    OUT            : None                                                 */
/*    RETURN        : CRYP_OK: Always                                       */
/*    REENTRANCY    : Non-reentrant                                         */
/*--------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_ConfigureForDma(IN t_cryp_device_id cryp_device_id, t_cryp_dma_req_type dma_req)
{
    
    DBGENTER1("DMA request bits %x",dma_req);

    CRYP_SET_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->dmacr,(t_uint32)dma_req);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

/*-----------------------------------------------------------------------------*/
/*    NAME        : CRYP_ConfigureKeyValues                                    */
/*-----------------------------------------------------------------------------*/
/*    DESCRIPTION : This routine configures the key values for CRYP operations */
/*    IN          : cryp_device_id                                             */
/*    IN          : key_reg_index: Key value index register.                   */
/*                key_value: The key value struct.                             */
/*    INOUT       : None                                                       */
/*    OUT         : None                                                       */
/*    RETURN      : CRYP_LOGIC_BUSY:If the logic is busy keys could not be     */
/*                  written.                                                   */
/*                  CRYP_OK:Always                                             */
/*    REENTRANCY  : Non-reentrant                                              */
/*-----------------------------------------------------------------------------*/
PUBLIC t_cryp_error CRYP_ConfigureKeyValues(IN t_cryp_device_id cryp_device_id,
                                            IN t_cryp_key_reg_index key_reg_index,
                                            IN t_cryp_key_value     key_value)

{  
   t_bool     status;
          
   DBGENTER1("CRYP Key Index %x",key_reg_index);

   status= (t_bool)(CRYP_TEST_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->status,CRYP_BUSY_STATUS_MASK)? TRUE:FALSE);
   if(TRUE == status)
   {
        DBGEXIT (CRYP_LOGIC_BUSY);
        return  (CRYP_LOGIC_BUSY);
   }
   
   switch (key_reg_index)
   {   
        case CRYP_KEY_REG_1:
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_1_l,key_value.key_value_left);
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_1_r,key_value.key_value_right);
                 break;
                                                
        case CRYP_KEY_REG_2:
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_2_l,key_value.key_value_left);
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_2_r,key_value.key_value_right);
                 break;
                    
        case CRYP_KEY_REG_3:
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_3_l,key_value.key_value_left);
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_3_r,key_value.key_value_right);
                 break;

        case CRYP_KEY_REG_4:
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_4_l,key_value.key_value_left);
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_4_r,key_value.key_value_right);
                 break;
                                                            
        default: DBGEXIT(CRYP_INVALID_PARAMETER);         
                 return(CRYP_INVALID_PARAMETER);
    }

   DBGEXIT(CRYP_OK); 
   return(CRYP_OK);
   
}

/*----------------------------------------------------------------------------*/
/*         NAME :    CRYP_ConfigureInitVector(t_cryp_bank_index)              */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the initialization vector register    */
/* PARAMETERS :                                                               */
/*         IN : cryp_device_id                                                */
/*         IN : init_vector_index: Specifies the index of the init vector.    */
/*              init_vector_value: Specifies the value for the init vector.   */
/*        OUT :                                                               */
/*                                                                            */
/*     RETURN :CRYP_LOGIC_BUSY: If the logic is busy, IV could not be written */
/*            CRYP_OK: Always.                                                */
/*  REENTRANCY: Re-entrant                                                    */
/*----------------------------------------------------------------------------*/

PUBLIC t_cryp_error CRYP_ConfigureInitVector(IN t_cryp_device_id cryp_device_id,
                                             IN t_cryp_init_vector_index init_vector_index, 
                                             IN t_cryp_init_vector_value init_vector_value)

{  
   t_bool     status;
   
   DBGENTER1("CRYP INIT VECT Index %x",init_vector_index);

   status= (t_bool)(CRYP_TEST_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->status,CRYP_BUSY_STATUS_MASK)? TRUE:FALSE);
   if(TRUE == status)
   {
        DBGEXIT (CRYP_LOGIC_BUSY);
        return  (CRYP_LOGIC_BUSY);
   }
   
   switch (init_vector_index)
   {   
        case CRYP_INIT_VECTOR_INDEX_0:
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_0_l,init_vector_value.init_value_left);
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_0_r,init_vector_value.init_value_right);
                 break;

        case CRYP_INIT_VECTOR_INDEX_1:
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_1_l,init_vector_value.init_value_left);
                 CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_1_r,init_vector_value.init_value_right);
                 break;

        default: DBGEXIT(CRYP_INVALID_PARAMETER);
                 return(CRYP_INVALID_PARAMETER);         
    }

   DBGEXIT(CRYP_OK); 
   return(CRYP_OK);
   
}


/*----------------------------------------------------------------------------*/
/*       NAME : CRYP_SaveDeviceContext(void)                                  */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: Store hardware registers and other device context parameter   */
/* PARAMETERS :                                                               */
/*         IN : cryp_device_id                                                */
/*        OUT : None                                                          */
/*                                                                            */
/*     RETURN :                                                               */
/*  REENTRANCY: Re-entrant                                                    */
/*----------------------------------------------------------------------------*/

PUBLIC void CRYP_SaveDeviceContext(IN t_cryp_device_id cryp_device_id)
{
        
    /*--------------------------------------------------------------------
        Power Management saving of the registers.
    --------------------------------------------------------------------*/

    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].cnfg,g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].dmacr,g_cryp_system_context.p_cryp_reg[cryp_device_id]->dmacr);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].imsc,g_cryp_system_context.p_cryp_reg[cryp_device_id]->imsc);

    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].key_1_l,g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_1_l);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].key_1_r,g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_1_r);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].key_2_l,g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_2_l);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].key_2_r,g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_2_r);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].key_3_l,g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_3_l);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].key_3_r,g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_3_r);

    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].init_vect_0_l,g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_0_l);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].init_vect_0_r,g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_0_r);

    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].key_4_l,g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_4_l);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].key_4_r,g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_4_r);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].init_vect_1_l,g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_1_l);
    CRYP_WRITE_REG(g_cryp_system_context.device_context[cryp_device_id].init_vect_1_r,g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_1_r);

}

/*----------------------------------------------------------------------------*/
/*       NAME : CRYP_RestoreDeviceContext(void)                               */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: Retore hardware registers and other device context parameter  */
/* PARAMETERS :                                                               */
/*         IN : cryp_device_id                                                */
/*        OUT : None                                                          */
/*                                                                            */
/*     RETURN :                                                               */
/*  REENTRANCY: Re-entrant                                                    */
/*----------------------------------------------------------------------------*/

PUBLIC void CRYP_RestoreDeviceContext(IN t_cryp_device_id cryp_device_id)
{  
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,g_cryp_system_context.device_context[cryp_device_id].cnfg);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->dmacr,g_cryp_system_context.device_context[cryp_device_id].dmacr);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->imsc,g_cryp_system_context.device_context[cryp_device_id].imsc);
    
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_1_l,g_cryp_system_context.device_context[cryp_device_id].key_1_l);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_1_r,g_cryp_system_context.device_context[cryp_device_id].key_1_r);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_2_l,g_cryp_system_context.device_context[cryp_device_id].key_2_l);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_2_r,g_cryp_system_context.device_context[cryp_device_id].key_2_r);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_3_l,g_cryp_system_context.device_context[cryp_device_id].key_3_l);    
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_3_r,g_cryp_system_context.device_context[cryp_device_id].key_3_r);
    
    
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_0_l,g_cryp_system_context.device_context[cryp_device_id].init_vect_0_l);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_0_r,g_cryp_system_context.device_context[cryp_device_id].init_vect_0_r);

    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_4_l,g_cryp_system_context.device_context[cryp_device_id].key_4_l);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->key_4_r,g_cryp_system_context.device_context[cryp_device_id].key_4_r);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_1_l,g_cryp_system_context.device_context[cryp_device_id].init_vect_1_l);
    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->init_vect_1_r,g_cryp_system_context.device_context[cryp_device_id].init_vect_1_r);
}

/*----------------------------------------------------------------------------*/
/*       NAME : CRYP_WriteInData()                                            */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: This routine writes 32 bit data into the data input register  */
/*              of the cryptography IP.                                       */
/* PARAMETERS :                                                               */
/*         IN : cryp_device_id                                                */
/*         IN : write_data: Specifies the data to be written into the IN FIFO.*/
/*        OUT : None                                                          */
/*                                                                            */
/*     RETURN : CRYP_OK: Always.                                              */
/*                                                                            */
/*  REENTRANCY: Re-entrant                                                    */ 
/*----------------------------------------------------------------------------*/
PUBLIC t_cryp_error CRYP_WriteInData(IN t_cryp_device_id cryp_device_id, t_uint32 write_data)
{
    
    DBGENTER1("Data to write %x",write_data); 

    CRYP_WRITE_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->din,write_data);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

/*----------------------------------------------------------------------------*/
/*       NAME : CRYP_ReadInData                                               */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: This routine reads the 32 bit data from the data input registe*/
/*              into the specified location.                                  */
/* PARAMETERS :                                                               */
/*         IN : cryp_device_id                                                */
/*        OUT : *p_read_data: Read the data from the input FIFO.              */
/*                                                                            */
/*     RETURN : CRYP_INVALID_PARAMETER: if NULL == p_read_data.               */
/*             CRYP_OK: Always.                                               */
/*                                                                            */
/*  REENTRANCY: Re-entrant                                                    */ 
/*----------------------------------------------------------------------------*/
PUBLIC t_cryp_error CRYP_ReadInData(IN t_cryp_device_id cryp_device_id, t_uint32 *p_read_data)
{
    
    DBGENTER1("Data to read in pointer %x",p_read_data);  
    
    if(NULL == p_read_data)
    {
        DBGEXIT(CRYP_INVALID_PARAMETER);
        return CRYP_INVALID_PARAMETER;
    }

    *p_read_data = CRYP_READ_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->din);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

/*----------------------------------------------------------------------------*/
/*       NAME : CRYP_ReadOutData                                              */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: This routine reads the data from the data output register of  */
/*              the CRYP logic                                                */
/* PARAMETERS :                                                               */
/*         IN :  cryp_device_id                                               */
/*        OUT : *p_read_data: Read the data from the input FIFO.              */
/*                                                                            */
/*     RETURN : CRYP_INVALID_PARAMETER: if NULL == p_read_data.               */
/*             CRYP_OK: Otherwise                                             */
/*                                                                            */
/*  REENTRANCY: Re-entrant                                                    */ 
/*----------------------------------------------------------------------------*/
PUBLIC t_cryp_error CRYP_ReadOutData(IN t_cryp_device_id cryp_device_id, t_uint32 *p_read_data)
{
    
    DBGENTER1("Data to read in pointer %x",p_read_data);  
    
    if(NULL == p_read_data)
    {
        DBGEXIT(CRYP_INVALID_PARAMETER);
        return CRYP_INVALID_PARAMETER;
    }

    *p_read_data = CRYP_READ_REG(g_cryp_system_context.p_cryp_reg[cryp_device_id]->dout);
    
    DBGEXIT0(CRYP_OK);
    return CRYP_OK;  
}

#if ((defined ST_HREFV2) || (defined ST_8500V2) || (defined __PEPS_8500_V2))
/*----------------------------------------------------------------------------*/
/*       NAME : CRYP_KeyLock                                                  */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:   															  */
/*   Enable / Disable AES Keys Lock, which protects key registers against	  */
/*   un-authorized access                                                	  */
/* PARAMETERS :                                                               */
/*         IN : t_cryp_device_id cryp_device_id, t_cryp_key_lock cryp_keylock */
/*        OUT : None                                                          */
/*                                                                            */
/*     RETURN : t_cryp_error  (CRYP_OK always)                                */
/*                                                                            */
/*  REENTRANCY: Non Re-entrant                                                */
/*----------------------------------------------------------------------------*/
PUBLIC t_cryp_error CRYP_SetKeyLock(IN t_cryp_device_id cryp_device_id, t_cryp_key_lock cryp_keylock)
{

    DBGENTER1("Enable / disable cryp AES keys %x",cryp_keylock);

    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,cryp_keylock,CRYP_KEY_LOCK_POS,CRYP_KEY_LOCK_MASK);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

#endif

/*----------------------------------------------------------------------------*/
/*       NAME : CRYP_SetStealingMode                                          */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:   															  */
/*   Enable / Disable AES Keys Lock, which protects key registers against	  */
/*   un-authorized access                                                	  */
/* PARAMETERS :                                                               */
/*         IN : t_cryp_device_id cryp_device_id, t_cryp_stealing cryp_stealing*/
/*        OUT : None                                                          */
/*                                                                            */
/*     RETURN : t_cryp_error   (CRYP_OK always)                               */
/*                                                                            */
/*  REENTRANCY: Non Re-entrant                                                */
/*----------------------------------------------------------------------------*/
PUBLIC t_cryp_error CRYP_SetStealingMode(IN t_cryp_device_id cryp_device_id, t_cryp_stealing cryp_stealing)
{

    DBGENTER1("Enable / disable cryp AES Stealing Mode %x",cryp_stealing);

    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,cryp_stealing,CRYP_STEALING_POS,CRYP_STEALING_MASK);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

/*----------------------------------------------------------------------------*/
/*       NAME : CRYP_SetSecondLastBlock                                       */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:   															  */
/*   Enable / Disable cryp Second last input block (when stealing is enabled).*/
/*   This is to identify whether the submitted block is second last or not.   */
/* PARAMETERS :                                                               */
/*         IN : t_cryp_device_id , t_cryp_secondlast                          */
/*        OUT : None                                                          */
/*                                                                            */
/*     RETURN : t_cryp_error  (CRYP_OK always)                                */
/*                                                                            */
/*  REENTRANCY: Non Re-entrant                                                */
/*----------------------------------------------------------------------------*/
PUBLIC t_cryp_error CRYP_SetSecondLastBlock(IN t_cryp_device_id cryp_device_id, t_cryp_secondlast cryp_secondlast)
{

    DBGENTER1("Enable / disable cryp AES-XTS Second last input block %x",cryp_secondlast);

    CRYP_PUT_BITS(g_cryp_system_context.p_cryp_reg[cryp_device_id]->cnfg,cryp_secondlast,CRYP_SECOND_LAST_POS,CRYP_SECOND_LAST_MASK);

    DBGEXIT0(CRYP_OK);
    return CRYP_OK;
}

