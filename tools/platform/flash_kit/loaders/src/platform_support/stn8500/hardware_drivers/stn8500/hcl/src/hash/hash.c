/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides some support routines for the HASH processor
*  Specification release related to this implementation: A_V2.2
*  Reference : Software Code Development, C Coding Rules, Guidelines 
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "hash_p.h"

/*--------------------------------------------------------------------------*
 * Defines                                                                    *
 *--------------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME my_debug_level_hash
#define MY_DEBUG_ID             my_debug_id_hash

/*--------------------------------------------------------------------------*
 * Global Variables                                                            *
 *--------------------------------------------------------------------------*/
/* For debug HCL */
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = HASH_HCL_DBG_ID;
#endif /* __DEBUG */

/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_hash_system_context   g_hash_system_context;

/*--------------------------------------------------------------------------*
 * Public functions                                                         *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME            :      HASH_Init                                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :     This routine initializes HASH register base address.    */
/*                    It also checks for peripheral Ids and PCell Ids.        */
/*                                                                          */
/* PARAMETERS    :                                                           */
/*         IN      :    hash_base_address     : HASH registers base address     */
/*         OUT     :    none                                                    */
/*                                                                          */
/* RETURN        :    HASH_UNSUPPORTED_HW   : This is returned  if peripheral */
/*                                           ids don't match                 */
/*                    HASH_INVALID_PARAMETER: This values is returned if base    */
/*                                             address passed is zero            */
/*                    HASH_OK               :    Returns this if no error        */
/*                                          detected                        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                      */
/* REENTRANCY ISSUES:                                                        */
/*            1)        Global variable p_hash_register(register base address)    */
/*                    is being modified                                        */
/****************************************************************************/
PUBLIC t_hash_error HASH_Init(IN t_hash_device_id hash_device_id, IN t_logical_address hash_base_address)
{
    t_hash_error    hash_error = HASH_OK;

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    
    DBGENTER1("HASH Base Address: (%lx)", hash_base_address);
    if (NULL != hash_base_address)
    {
        /*--------------------------------------*
        * Initializing the registers structure *
        *--------------------------------------*/
        g_hash_system_context.p_hash_register[hash_device_id] = (t_hash_register *) hash_base_address;

        /*--------------------------*
        * Checking Peripheral Ids  *
        *--------------------------*/
        if
        (
            (HASH_P_ID0 == g_hash_system_context.p_hash_register[hash_device_id]->hash_periphid0)
        &&  (HASH_P_ID1 == g_hash_system_context.p_hash_register[hash_device_id]->hash_periphid1)
        &&  (HASH_P_ID2 == g_hash_system_context.p_hash_register[hash_device_id]->hash_periphid2)
        &&  (HASH_P_ID3 == g_hash_system_context.p_hash_register[hash_device_id]->hash_periphid3)
        &&  (HASH_CELL_ID0 == g_hash_system_context.p_hash_register[hash_device_id]->hash_cellid0)
        &&  (HASH_CELL_ID1 == g_hash_system_context.p_hash_register[hash_device_id]->hash_cellid1)
        &&  (HASH_CELL_ID2 == g_hash_system_context.p_hash_register[hash_device_id]->hash_cellid2)
        &&  (HASH_CELL_ID3 == g_hash_system_context.p_hash_register[hash_device_id]->hash_cellid3)
        )
        {
            /* Resetting the values of global variables except the p_hash_register */
            hash_InitializeGlobals(hash_device_id);
            hash_error = HASH_OK;
            DBGEXIT0(hash_error);
            return(hash_error);
        }
        else
        {
            hash_error = HASH_UNSUPPORTED_HW;
            DBGEXIT0(hash_error);
            return(hash_error);
        }
    }   /* end if */
    else
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
}

/****************************************************************************/
/* NAME            :    HASH_GetVersion                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    Gives the current version of the HASH HCL                  */
/*                                                                          */
/* PARAMETERS    :                                                            */
/*         IN        :    None                                                      */
/*         OUT        :    p_version             : Structure which will consist of */
/*                                             the version of the current HCL  */
/* RETURN        :    HASH_INVALID_PARAMETER:    This is returned  if p_version  */
/*                                            is NULL                          */
/*                    HASH_OK               :    Returns this if no error        */
/*                                          detected                        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :    Re-entrant                                          */
/* REENTRANCY ISSUES:    No Issues                                            */
/****************************************************************************/
PUBLIC t_hash_error HASH_GetVersion(OUT t_version *p_version)
{
    t_hash_error    hash_error = HASH_OK;
    DBGENTER1("Version Pointer: ( %p )", (void *) p_version);
    if (NULL != p_version)
    {
        p_version->version = HASH_HCL_VERSION_ID;
        p_version->major = HASH_HCL_MAJOR_ID;
        p_version->minor = HASH_HCL_MINOR_ID;

        hash_error = HASH_OK;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    else
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
}

#ifdef __DEBUG  /* Debug APIs */

/****************************************************************************/
/* NAME            :    HASH_SetDbgLevel                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine sets the debug level for the HASH HCL.        */
/*                                                                          */
/* PARAMETERS    :                                                             */
/*         IN        :    debug_level :   debug level from DEBUG_LEVEL0 to        */
/*                                  DEBUG_LEVEL4                            */
/*         OUT        :    none                                                      */
/*                                                                          */
/* RETURN        :    HASH_OK     :   Returns this if no error detected       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :    Re-entrant                                          */
/* REENTRANCY ISSUES:    No Issues                                            */
/****************************************************************************/
PUBLIC t_hash_error HASH_SetDbgLevel(IN t_dbg_level debug_level)
{
    DBGENTER1("Debug Level: ( %x )", debug_level);
    MY_DEBUG_LEVEL_VAR_NAME = debug_level;
    DBGEXIT0(HASH_OK);
    return(HASH_OK);
}

/****************************************************************************/
/* NAME            :    HASH_GetDbgLevel                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine enables to choose between different debug  */
/*                  comments levels                                         */
/* PARAMETERS    :                                                             */
/*         IN        :    none                                                    */
/*         OUT        :    p_debug_level   : Pointer to the current debug level    */
/*                                                                          */
/* RETURN        :    HASH_OK                 : Returns this if no error      */
/*                                            detected                      */
/*                    HASH_INVALID_PARAMETER  : This value is returned if     */
/*                                            p_debug_level == NULL         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :    Re-entrant                                          */
/* REENTRANCY ISSUES:    No Issues                                            */
/****************************************************************************/
PUBLIC t_hash_error HASH_GetDbgLevel(OUT t_dbg_level *p_debug_level)
{
    DBGENTER1("Pointer to Debug Level: ( %p )", p_debug_level);
    if (NULL == p_debug_level)
    {
        DBGEXIT0(HASH_INVALID_PARAMETER);
        return(HASH_INVALID_PARAMETER);
    }

    *p_debug_level = MY_DEBUG_LEVEL_VAR_NAME;
    DBGEXIT0(HASH_OK);
    return(HASH_OK);
}
#endif /* __DEBUG */

/****************************************************************************/
/* NAME            :    HASH_Reset                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine will reset the global variable to default  */
/*                    reset value and HASH registers to their Power on reset  */
/*                    values                                                  */
/* PARAMETERS    :                                                              */
/*         IN        :    none                                                    */
/*         OUT        :    none                                                      */
/*                                                                          */
/* RETURN        :    HASH_OK  :    Returns this if no error detected           */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                      */
/* REENTRANCY ISSUES:                                                        */
/*                1)    Global structure g_hash_system_context elements are        */
/*                    being modified                                            */
/****************************************************************************/
PUBLIC t_hash_error HASH_Reset(IN t_hash_device_id hash_device_id)
{
    t_hash_error    hash_error = HASH_OK;
    t_uint8         loop_count;

    DBGENTER0();

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    /* Resetting the values of global variables except the p_hash_register */
    hash_InitializeGlobals(hash_device_id);

    /* Resetting HASH control register to power-on-reset values */
    g_hash_system_context.p_hash_register[hash_device_id]->hash_str = HASH_RESET_START_REG_VALUE;
    for (loop_count = 0; loop_count < (HASH_CSR_COUNT - 2); loop_count++)
    {
        g_hash_system_context.p_hash_register[hash_device_id]->hash_csrx[loop_count] = HASH_RESET_CSRX_REG_VALUE;
    }

    g_hash_system_context.p_hash_register[hash_device_id]->hash_csfull = HASH_RESET_CSFULL_REG_VALUE;
    g_hash_system_context.p_hash_register[hash_device_id]->hash_csdatain = HASH_RESET_CSDATAIN_REG_VALUE;

    /*----------------------------------------------------------------------*
     * Resetting the HASH Control reg. This also reset the PRIVn and SECn   *
     * bits and hence the device registers will not be accessed anymore and *
     * should be done in the last HASH register access statement.           *
     *----------------------------------------------------------------------*/
    g_hash_system_context.p_hash_register[hash_device_id]->hash_cr = HASH_RESET_CONTROL_REG_VALUE;
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME            :    HASH_ConfigureDmaRequest                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine Enables/Disables the DMA request.            */
/* PARAMETERS    :                                                              */
/*         IN        :    request_state    : Whether to Enable/Disable DMA request */
/*         OUT        :    none                                                      */
/*                                                                          */
/* RETURN        :    HASH_OK  :    Returns this if no error detected           */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                      */
/* REENTRANCY ISSUES:                                                        */
/*                1)    Global variable p_hash_register(HASH control register)  */
/*                  is being modified                                       */
/****************************************************************************/
PUBLIC t_hash_error HASH_ConfigureDmaRequest(IN t_hash_device_id hash_device_id, IN t_hash_dma_req request_state)
{
    t_hash_error    hash_error = HASH_OK;

    DBGENTER1("DMA request state: ( %x )", request_state);

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    if (HASH_DISABLE_DMA_REQ == request_state)
    {
        HCL_CLEAR_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_DMAE_MASK);
    }
    else
    {
        HCL_SET_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_DMAE_MASK);
    }

    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME            :    HASH_ConfigureLastValidBits                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine configures the number of Valid Bits in     */
/*                  last word of the message.                                */
/* PARAMETERS    :                                                              */
/*         IN        :    nblw_val    : Number of valid bits in last word of the  */
/*                                message                                   */
/*         OUT        :    none                                                      */
/*                                                                          */
/* RETURN        :   HASH_INVALID_PARAMETER: This values is returned if the    */
/*                                             NBLW value exceeds              */
/*                                          HASH_NBLW_MAX_VAL (0x1F)        */
/*                  HASH_REQUEST_NOT_APPLICABLE : The HASH message digest   */
/*                                          calculation is still ongoing so */
/*                                          the HASH processor can't process*/
/*                                          the current request.            */
/*                    HASH_OK               :    Returns this if no error        */
/*                                          detected                        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                      */
/* REENTRANCY ISSUES:                                                        */
/*                1)    Global variable p_hash_register(HASH control register)  */
/*                  is being modified                                       */
/*--------------------------------------------------------------------------*/
/* NOTE         :   The NBLW bit field is not changed when DCAL bit is set, */
/*                  i.e. when DCAL = 1b. So the user must ensure that the   */
/*                  DCAL bit is clear before calling this API.              */
/****************************************************************************/
PUBLIC t_hash_error HASH_ConfigureLastValidBits(IN t_hash_device_id hash_device_id, IN t_uint8 nblw_val)
{
    t_hash_error    hash_error = HASH_OK;
    DBGENTER1("Number of Valid Bits in Last Word of the Message : ( %x )", nblw_val);

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    
    if (nblw_val > HASH_NBLW_MAX_VAL)
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    else if ((t_bool) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_str & HASH_STR_DCAL_MASK) >> HASH_STR_DCAL_POS))
    {
        hash_error = HASH_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    else
    {
        HCL_WRITE_BITS
        (
            g_hash_system_context.p_hash_register[hash_device_id]->hash_str,
            ((t_uint32) nblw_val << HASH_STR_NBLW_POS),
            HASH_STR_NBLW_MASK
        );
    }
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME         :   HASH_ConfigureDigestCal                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This API Set/Reset the DCAL bit of HASH start register. */
/*                  Written DCAL bit with 1b, starts the message padding,   */
/*                  using previously written value of NBLW, and starts      */
/*                  calculation of the final message digest with all data   */
/*                  words written to HASH_DIN register since last time INIT */
/*                  bit was written to 1b.                                  */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   hash_dcal_state : State of DCAL bit (Set/Reset)         */
/*                                                                          */
/* RETURN       :   HASH_OK     : Returns this if no error detected         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                      */
/* REENTRANCY ISSUES:                                                        */
/*                1)    Global variable p_hash_register(HASH control register)  */
/*                  is being modified                                       */
/****************************************************************************/
PUBLIC t_hash_error HASH_ConfigureDigestCal(IN t_hash_device_id hash_device_id, IN t_hash_dcal_state hash_dcal_state)
{
    t_hash_error    hash_error = HASH_OK;

    DBGENTER1("Digest calculation state: ( %x )", hash_dcal_state);

    if(!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    if (HASH_DISABLE_DCAL == hash_dcal_state)
    {
        HCL_CLEAR_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_str, HASH_STR_DCAL_MASK);
    }
    else
    {
        HCL_SET_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_str, HASH_STR_DCAL_MASK);
    }
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/*--------------------------------------------------------------------------*/
/*  NAME        :    HASH_ConfigureProtection                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures the protection bits in the HASH */
/*                  logic.                                                  */
/* PARAMETERS   :                                                            */
/*      IN      :   p_protect_config    : Pointer to the protection mode    */
/*                                        and secure mode configuration.    */
/*        OUT     :   None                                                    */
/*                                                                             */
/*     RETURN     :   HASH_INVALID_PARAMETER  : If NULL == p_protect_config.  */
/*                  HASH_OK                 : Otherwise                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC t_hash_error HASH_ConfigureProtection(IN t_hash_device_id hash_device_id, IN t_hash_protection_config *p_protect_config)
{
    t_hash_error    hash_error = HASH_OK;
    
    DBGENTER1("Pointer for the protection config: ( %x )", p_protect_config);

    if (NULL == p_protect_config)
    {
        DBGEXIT0(HASH_INVALID_PARAMETER);
        return(HASH_INVALID_PARAMETER);
    }

    if(!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    
    HCL_WRITE_BITS
    (
        g_hash_system_context.p_hash_register[hash_device_id]->hash_cr,
        ((t_uint32) p_protect_config->hash_secure_access << HASH_CR_SECN_POS),
        HASH_CR_SECN_MASK
    );
    HCL_WRITE_BITS
    (
        g_hash_system_context.p_hash_register[hash_device_id]->hash_cr,
        ((t_uint32) p_protect_config->hash_privilege_access << HASH_CR_PRIVN_POS),
        HASH_CR_PRIVN_MASK
    );
    
    DBGEXIT0(HASH_OK);
    return(HASH_OK);
}

/****************************************************************************/
/* NAME            :    HASH_SetConfiguration                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine sets the required configuration for HASH   */
/*                  controller.                                             */
/*                                                                            */
/* PARAMETERS    :                                                              */
/*         IN        :    p_config:  Pointer to the configuration for HASH        */
/*         OUT        :    none                                                      */
/*                                                                          */
/* RETURN        :    HASH_OK : Returns this if no error detected                */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                      */
/* REENTRANCY ISSUES:                                                        */
/*                1)    Global variable p_hash_register(cofiguration register,  */
/*                  parameter register, divider register) is being modified */
/*--------------------------------------------------------------------------*/
/* COMMENTS     1.  : User need to call HASH_Begin API after calling this   */
/*                    API i.e. the current configuration is set only when   */
/*                    bit INIT is set and we set INIT bit in HASH_Begin.    */
/*                    Changing the configuration during a computation has   */
/*                    no effect so we first set configuration by calling    */
/*                    this API and then set the INIT bit for the HASH       */
/*                    processor and the curent configuration is taken into  */
/*                    account. As reading INIT bit (with correct protection */
/*                    rights) will always return 0b so we can't make a check*/
/*                    at software level. So the user has to initialize the  */
/*                    device for new configuration to take in to effect.    */
/*              2.    The default value of data format is 00b ie the format */
/*                    of data entered in HASH_DIN register is 32-bit data.  */
/*                    The data written in HASH_DIN is used directly by the  */
/*                    HASH processing, without re ordering.                 */
/****************************************************************************/
PUBLIC t_hash_error HASH_SetConfiguration(IN t_hash_device_id hash_device_id, IN t_hash_config *p_config)
{
    t_hash_error    hash_error = HASH_OK;
    DBGENTER1("HASH Configuration Structure pointer: ( %p )", p_config);

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    
    HASH_SET_DATA_FORMAT(p_config->hash_config_data_format);

    HCL_SET_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_EMPTYMSG_MASK);

    /* This bit selects between SHA-1 or SHA-2 algorithm */
    if (HASH_ALGO_SHA2 == p_config->hash_config_algorithm)  /* SHA-2 algorithm */
    {
        HCL_CLEAR_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_ALGO_MASK);
    }
    else    /* SHA1 algorithm */
    {
        HCL_SET_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_ALGO_MASK);
    }

    /* This bit selects between HASH or HMAC mode for the selected algorithm */
    if (HASH_OPER_MODE_HASH == p_config->hash_config_oper_mode) /* HASH mode */
    {
        HCL_CLEAR_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_MODE_MASK);
    }
    else    /* HMAC mode */
    {
        HCL_SET_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_MODE_MASK);

        /* This bit selects between short key (<= 64 bytes) or long key (>64 bytes) in HMAC mode */
        if (HASH_SHORT_KEY == p_config->hash_config_hmac_key)
        {
            HCL_CLEAR_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_LKEY_MASK);
        }
        else
        {
            HCL_SET_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_LKEY_MASK);
        }
    }
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME         :   HASH_ClockGatingOff                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This API is used when the user need to disable the      */
/*                  internal clock gating.                                  */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   t_hash_device_id                                        */
/*                                                                          */
/* RETURN       :   None                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                  */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_hash_error HASH_ClockGatingOff(IN t_hash_device_id hash_device_id)
{
    DBGENTER0();    

    /* Switch On bit is set for clock gating disable, by default the clock gating is enabled internally */
    HCL_SET_BITS(g_hash_system_context.p_hash_register[hash_device_id]->hash_cr, HASH_CR_SWITCHON_MASK);
    
    DBGEXIT0(HASH_OK);
    return(HASH_OK);
}

/****************************************************************************/
/* NAME         :   HASH_Begin                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine resets some blobals and initializes the    */
/*                  HASH processor core                                     */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   none                                                    */
/*      OUT     :   none                                                    */
/*                                                                          */
/* RETURN       :   HASH_OK     : Returns this if no error detected         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/*--------------------------------------------------------------------------*/
/* COMMENTS     1.  : User need to call HASH_SetConfiguration API before    */
/*                    calling this API i.e. the current configuration is set*/
/*                    only when bit INIT is set and we set INIT bit in      */
/*                    HASH_Begin. Changing the configuration during a       */
/*                    computation has no effect so we first set             */
/*                    configuration by calling this API and then set the    */
/*                    INIT bit for the HASH processor and the curent        */
/*                    configuration is taken into account. As reading INIT  */
/*                    bit (with correct protection rights) will always      */
/*                    return 0b so we can't make a check at software level. */
/*                    So the user has to initialize the device for new      */
/*                    configuration to take in to effect.                   */
/****************************************************************************/
PUBLIC t_hash_error HASH_Begin(IN t_hash_device_id hash_device_id)
{
    t_hash_error    hash_error = HASH_OK;

    /* HW and SW initializations */
    /* Note: there is no need to initialize buffer and digest members */
    DBGENTER0();

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    g_hash_system_context.hash_state[hash_device_id].hash_state_index = 0;
    g_hash_system_context.hash_state[hash_device_id].hash_state_bit_index = 0;
    g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_high_word = 0;
    g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_low_word = 0;

    HASH_INITIALIZE;
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME         :   HASH_ReadMessageDigest                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This API will return the message digest result.         */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      OUT     :   digest  : Array for message digest to be read.          */
/*                                                                          */
/* RETURN       :   HASH_OK : Returns this if no error detected             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                      */
/* REENTRANCY ISSUES:                                                        */
/*                1)    Global variable p_hash_register(HASH control register)  */
/*                  is being modified                                       */
/*--------------------------------------------------------------------------*/
/* NOTE         :   This API should be called after digest calculation has  */
/*                  finished already to know the final message digest,      */
/*                  otherwise it will return intermediate message digest    */
/*                  value.                                                  */
/****************************************************************************/
PUBLIC t_hash_error HASH_ReadMessageDigest(IN t_hash_device_id hash_device_id, OUT t_uint8 digest[HASH_MSG_DIGEST_SIZE])
{
    t_uint32        temp_hx_val, count;
    t_hash_error    hash_error = HASH_OK;

    DBGENTER1("Message digest array: ( %p )", digest);

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    
    /* Copy result into digest array */
    for (count = 0; count < (HASH_MSG_DIGEST_SIZE / sizeof(t_uint32)); count++)
    {
        temp_hx_val = HASH_GET_HX(count);
        digest[count * 4] = (t_uint8) ((temp_hx_val >> 24) & 0xFF);
        digest[count * 4 + 1] = (t_uint8) ((temp_hx_val >> 16) & 0xFF);
        digest[count * 4 + 2] = (t_uint8) ((temp_hx_val >> 8) & 0xFF);
        digest[count * 4 + 3] = (t_uint8) ((temp_hx_val >> 0) & 0xFF);
    }
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME         :   HASH_GetDeviceStatus                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine returns the current status of the device   */
/*                  (DINF, NBW, and NBLW bits status)                       */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   none                                                    */
/*      OUT     :   none                                                    */
/*                                                                          */
/* RETURN       :   device_status   : Status of the device                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC t_hash_device_status HASH_GetDeviceStatus(IN t_hash_device_id hash_device_id)
{
    t_hash_device_status    device_status = {(t_hash_dinf_status)0, (t_hash_nbw_status)0,(t_uint8)0};
    
    DBGENTER0();

    device_status.hash_dinf_status = (t_hash_dinf_status) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_cr & HASH_CR_DINF_MASK) >> HASH_CR_DINF_POS);
    device_status.hash_nbw_status = (t_hash_nbw_status) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_cr & HASH_CR_NBW_MASK) >> HASH_CR_NBW_POS);
    device_status.hash_nblw_status = (t_uint8) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_str & HASH_STR_NBLW_MASK) >> HASH_STR_NBLW_POS);

    DBGEXIT0(device_status.hash_nblw_status);
    return(device_status);
}

/****************************************************************************/
/* NAME         :   HASH_IsDcalOngoing                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This API checks whether the message digest calculation  */
/*                  is still ongoing or finished already.                   */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   None                                                    */
/*                                                                          */
/* RETURN       :   FALSE   : If digest calculation has finished.           */
/*                  TRUE    : If digest calculation is still ongoing.       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                      */
/* REENTRANCY ISSUES:                                                        */
/*                1)    Global variable p_hash_register(HASH control register)  */
/*                  is being modified                                       */
/*--------------------------------------------------------------------------*/
/* NOTE         :   Reading will return 1b as long as the calculation is on */
/*                  going, and returns to 0b when new HASH_H0...4 are       */
/*                  available. DCAL bit is set by hardware when the DMA is  */
/*                  enabled (DMAE = 1b in HASH_CR register) and last word   */
/*                  has been written by DMA controller (HASHDMATC = HIGH    */
/*                  when HASHDMACLR = HIGH).                                */
/****************************************************************************/
PUBLIC t_bool HASH_IsDcalOngoing(IN t_hash_device_id hash_device_id)
{
    t_bool  flag = TRUE;  
    
    DBGENTER0();    

    flag = (t_bool) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_str & HASH_STR_DCAL_MASK) >> HASH_STR_DCAL_POS);
    
    DBGEXIT0(flag);
    return(flag);
}

#ifdef __HASH_ELEMENTARY    /* Elementary layer APIs */

/****************************************************************************/
/* NAME         :   HASH_SetInputData                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine writes the input data to the HASH_DIN      */
/*                  register.                                               */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   input_data  : Data to be written in DIN register for    */
/*                                message digest calculation.               */
/*      OUT     :   none                                                    */
/*                                                                          */
/* RETURN       :   HASH_OK     : Returns this if no error detected         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/*--------------------------------------------------------------------------*/
/* NOTE         :   1. When written, current register content is pushed     */
/*                     inside the HASH core, and register takes new value   */
/*                     presented on AHB data bus.                           */
/*                  2. The HASH_DIN register only accept little-endian data */
/*                     representation, both for read and write operations.  */
/****************************************************************************/
PUBLIC t_hash_error HASH_SetInputData(IN t_hash_device_id hash_device_id, IN t_uint32 input_data)
{
    t_hash_error    hash_error = HASH_OK;

    DBGENTER1("HASH input data: ( %lx )", input_data);

    if !((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    g_hash_system_context.p_hash_register[hash_device_id]->hash_cr = input_data;
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME         :   HASH_GetDataInput                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine reads the input data from the HASH_DIN     */
/*                  register.                                               */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   none                                                    */
/*      OUT     :   p_data_input    : Pointer to 32 bit variable to return  */
/*                                    the current Data present in the DIN   */
/*                                    register                              */
/*                                                                          */
/* RETURN       :   HASH_OK         : Returns this if no error detected     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/*--------------------------------------------------------------------------*/
/* NOTE         :   1. The HASH_DIN is the data input register and is 32-bit*/
/*                     wide. When HASH_DIN is read in supervisor mode, the  */
/*                     last word written in this location is accessed (zero */
/*                     after reset).                                        */
/*                  2. The HASH_DIN register only accept little-endian data */
/*                     representation, both for read and write operations.  */
/****************************************************************************/
PUBLIC t_hash_error HASH_GetDataInput(IN t_hash_device_id hash_device_id, OUT t_uint32 *p_data_input)
{
    t_hash_error    hash_error = HASH_OK;

    DBGENTER1("HASH input data: ( %lx )", p_data_input);

    if !((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    *p_data_input = g_hash_system_context.p_hash_register[hash_device_id]->hash_cr;
    
    DBGEXIT0(hash_error);
    return(hash_error);
}
#endif /* __HASH_ELEMENTARY */

#ifdef __HASH_ENHANCED  /* Enhanced layer APIs */

/****************************************************************************/
/* NAME         :   HASH_Update                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Updates current HASH computation hashing another part   */
/*                  of the message.                                         */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   p_data_buffer   : Byte array containing the message to  */
/*                                    be hashed (caller allocated)          */
/*                  msg_length      : Length of the message (in the form of */
/*                                    number of bits) to be hashed          */
/*      OUT     :   none                                                    */
/*                                                                          */
/* RETURN       :   HASH_INVALID_PARAMETER      : If p_data_buffer is NULL  */
/*                  HASH_MSG_LENGTH_OVERFLOW    : If the total length of    */
/*                                                the message exceeds       */
/*                                                2^64 - 1 bits             */
/*                  HASH_OK                     : Returns this if no error  */
/*                                                detected                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC t_hash_error HASH_Update(IN t_hash_device_id hash_device_id, IN const t_uint8 *p_data_buffer, IN t_uint32 msg_length)
{
    t_hash_error    hash_error = HASH_OK;
    t_uint8         index;
    t_uint8         *p_buffer;
    t_uint32        count;

    DBGENTER2("HASH input buffer pointer: ( %p ), Message length: ( %x )", p_data_buffer, msg_length);

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    index = g_hash_system_context.hash_state[hash_device_id].hash_state_index;
        
    p_buffer = (t_uint8 *) g_hash_system_context.hash_state[hash_device_id].hash_state_buffer;

    /* Storing number of extra bits from msg_length */
    g_hash_system_context.hash_state[hash_device_id].hash_state_bit_index += (msg_length % 8);

    /* Number of bytes in the message */
    msg_length /= 8;

    /* Increment message bytes if number of bits are more than 8 */
    msg_length += (g_hash_system_context.hash_state[hash_device_id].hash_state_bit_index / 8);

    /* Put the remaining bits in the global variable so that it can be used in HASH_End API */
    g_hash_system_context.hash_state[hash_device_id].hash_state_bit_index %= 8;

    /* Check parameters */
    if (NULL == p_data_buffer)
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    /* Check if g_hash_system_context.hash_state.hash_state_length + msg_length overflows */
    if
    (
        msg_length > (g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_low_word + msg_length)
    &&  HASH_HIGH_WORD_MAX_VAL == (g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_high_word)
    )
    {
        hash_error = HASH_MSG_LENGTH_OVERFLOW;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    /* Main loop */
    while (0 != msg_length)
    {
        if ((index + msg_length) < HASH_BLOCK_SIZE)
        {
            for (count = 0; count < msg_length; count++)
            {
                p_buffer[index + count] = *(p_data_buffer + count);
            }

            index += msg_length;
            msg_length = 0;
        }
        else
        {
            /*------------------------------------------------------------------*
             * if 'p_data_buffer' is four byte aligned and local buffer does    *
             * not have any data, we can write data directly from               *
             * 'p_data_buffer' to HW periph., otherwise we first copy data to   *
             * local buffer                                                     *
             *------------------------------------------------------------------*/
            if ((0 == (((t_uint32) p_data_buffer) % 4)) && (0 == index))
            {
                hash_ProcessBlock(hash_device_id,(const t_uint32 *) p_data_buffer);
            }
            else
            {
                for (count = 0; count < (t_uint32) (HASH_BLOCK_SIZE - index); count++)
                {
                    p_buffer[index + count] = *(p_data_buffer + count);
                }

                hash_ProcessBlock(hash_device_id,(const t_uint32 *) p_buffer);
            }

            hash_IncrementLength(hash_device_id,HASH_BLOCK_SIZE);
            p_data_buffer += (HASH_BLOCK_SIZE - index);
            msg_length -= (HASH_BLOCK_SIZE - index);
            index = 0;
        }
    }

    g_hash_system_context.hash_state[hash_device_id].hash_state_index = index;

    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME         :   HASH_End                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Ends current HASH computation, passing back the hash to */
/*                  the user.                                               */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   None                                                    */
/*      OUT     :   digest  : Caller allocated buffer in which the digest   */
/*                            is copied                                     */
/*                                                                          */
/* RETURN       :   HASH_INVALID_PARAMETER : If digest is NULL              */
/*                  HASH_OK                : Returns this if no error       */
/*                                           detected                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC t_hash_error HASH_End(IN t_hash_device_id hash_device_id, OUT t_uint8 digest[HASH_MSG_DIGEST_SIZE])
{
    t_hash_error    hash_error = HASH_OK;
    t_uint32        count;
                    /* Standard SHA-1 digest for null string for HASH mode */
    t_uint8         zero_message_hash_sha1[HASH_MSG_DIGEST_SIZE] =
                    {
                        0xDA, 0x39, 0xA3, 0xEE,
                        0x5E, 0x6B, 0x4B, 0x0D,
                        0x32, 0x55, 0xBF, 0xEF,
                        0x95, 0x60, 0x18, 0x90,
                        0xAF, 0xD8, 0x07, 0x09,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00
                    };
                    /* Standard SHA-2 digest for null string for HASH mode */
    t_uint8         zero_message_hash_sha2[HASH_MSG_DIGEST_SIZE] =
                    {
                        0xD4, 0x1D, 0x8C, 0xD9,
                        0x8F, 0x00, 0xB2, 0x04,
                        0xE9, 0x80, 0x09, 0x98,
                        0xEC, 0xF8, 0x42, 0x7E,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00
                    };
                    /* Standard SHA-1 digest for null string for HMAC mode,with no key */
    t_uint8         zero_message_hmac_sha1[HASH_MSG_DIGEST_SIZE] =
                    {
                        0xFB, 0xDB, 0x1D, 0x1B,
                        0x18, 0xAA, 0x6C, 0x08,
                        0x32, 0x4B, 0x7D, 0x64,
                        0xB7, 0x1F, 0xB7, 0x63,
                        0x70, 0x69, 0x0E, 0x1D,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00
                    };
                    /* Standard SHA2 digest for null string for HMAC mode,with no key */
    t_uint8         zero_message_hmac_sha2[HASH_MSG_DIGEST_SIZE] =
                    {
                        0x74, 0xE6, 0xF7, 0x29,
                        0x8A, 0x9C, 0x2D, 0x16,
                        0x89, 0x35, 0xF5, 0x8C,
                        0x00, 0x1B, 0xAD, 0x88,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00
                    };

    DBGENTER1("Message digest array: ( %p )", digest);
    if (NULL == digest)
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    
    
    if
    (
        0 == g_hash_system_context.hash_state[hash_device_id].hash_state_index
    &&  0 == g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_high_word
    &&  0 == g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_low_word
    )
    {
        if (g_hash_system_context.p_hash_register[hash_device_id]->hash_cr & HASH_CR_MODE_MASK)     /* HMAC Mode */
        {
            if (g_hash_system_context.p_hash_register[hash_device_id]->hash_cr & HASH_CR_ALGO_MASK) /* SHA1 algo */
            {
                /* HASH hash of an empty message was requested */
                for (count = 0; count < HASH_MSG_DIGEST_SIZE; count++)
                {
                    digest[count] = zero_message_hmac_sha1[count];
                }
            }
            else    /* SHA-2 algo */
            {
                /* HASH hash of an empty message was requested */
                for (count = 0; count < HASH_MSG_DIGEST_SIZE; count++)
                {
                    digest[count] = zero_message_hmac_sha2[count];
                }
            }
        }
        else        /* HASH mode */
        {
            if (g_hash_system_context.p_hash_register[hash_device_id]->hash_cr & HASH_CR_ALGO_MASK) /* SHA1 algo */
            {
                /* HASH hash of an empty message was requested */
                for (count = 0; count < HASH_MSG_DIGEST_SIZE; count++)
                {
                    digest[count] = zero_message_hash_sha1[count];
                }
            }
            else    /* SHA-2 algo */
            {
                /* HASH hash of an empty message was requested */
                for (count = 0; count < HASH_MSG_DIGEST_SIZE; count++)
                {
                    digest[count] = zero_message_hash_sha2[count];
                }
            }
        }           /* end if */

        HASH_SET_DCAL;
    }
    else
    {
        hash_MessagePad
        (
            hash_device_id,
            g_hash_system_context.hash_state[hash_device_id].hash_state_buffer,
            g_hash_system_context.hash_state[hash_device_id].hash_state_index
        );

        /*------------------------------------------------------------------*
         * Wait till the DCAL bit get cleared, So that we get the final     *
         * message digest not intermediate value.                           *
         *------------------------------------------------------------------*/
        while ((t_bool) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_str & HASH_STR_DCAL_MASK) >> HASH_STR_DCAL_POS))
            ;
        hash_error = HASH_ReadMessageDigest(hash_device_id,digest);

        /* Reset the HASH state */
        g_hash_system_context.hash_state[hash_device_id].hash_state_index = 0;
        g_hash_system_context.hash_state[hash_device_id].hash_state_bit_index = 0;
        for (count = 0; count < HASH_BLOCK_SIZE / sizeof(t_uint32); count++)
        {
            g_hash_system_context.hash_state[hash_device_id].hash_state_buffer[count] = 0;
        }

        g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_high_word = 0;
        g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_low_word = 0;
    }
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME         :   HASH_HashCompute                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Performs a complete HASH calculation on the message     */
/*                  passed                                                  */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   p_data_buffer   : Byte array containing the message to  */
/*                                    be hashed (caller allocated)          */
/*                  msg_length      : Length of the message (in the form of */
/*                                    number of bits) to be hashed          */
/*                  p_hash_config   : Pointer to the configuration for the  */
/*                                    current HASH calculation              */
/*      OUT     :   digest          : Caller allocated buffer in which the  */
/*                                    digest is copied                      */
/*                                                                          */
/* RETURN       :   HASH_INVALID_PARAMETER  : If p_data_buffer is NULL      */
/*                  HASH_OK                 : Returns this if no error      */
/*                                            detected                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC t_hash_error HASH_Compute
(
    IN t_hash_device_id hash_device_id,
    IN const t_uint8    *p_data_buffer,
    IN t_uint32         msg_length,
    IN t_hash_config    *p_hash_config,
    OUT t_uint8         digest[HASH_MSG_DIGEST_SIZE]
)
{
    t_hash_error    hash_error = HASH_OK;

    DBGENTER3
    (
        "Data buffer pointer: ( %p ), message length: ( %x ), Data format: ( %x ),\
         Message digest array: ( %p )", p_data_buffer, msg_length, digest
    );

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    /*------------------------------------------* 
     * WARNING: return code must be checked if  *
     * behaviour of HASH_Begin changes.         *
     *------------------------------------------*/
    hash_error = HASH_SetConfiguration(hash_device_id, p_hash_config);
    if (HASH_OK != hash_error)
    {
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    hash_error = HASH_Begin(hash_device_id);
    if (HASH_OK != hash_error)
    {
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    hash_error = HASH_Update(hash_device_id, p_data_buffer, msg_length);
    if (HASH_OK != hash_error)
    {
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    hash_error = HASH_End(hash_device_id, digest);
    if (HASH_OK != hash_error)
    {
        DBGEXIT0(hash_error);
        return(hash_error);
    }   
    
    DBGEXIT0(hash_error);
    return(hash_error);
}
#endif /* __HASH_ENHANCED */

/****************************************************************************/
/* NAME         :   HASH_SaveDeviceState                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Saves the state of the current HASH computation.        */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   None                                                    */
/*      OUT     :   hash_state  : Caller allocated structure in which the   */
/*                                current computation state is copied       */
/*                                                                          */
/* RETURN       :   HASH_INVALID_PARAMETER  : If hash_state is NULL         */
/*                  HASH_OK                    : Returns this if no error      */
/*                                            detected.                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC t_hash_error HASH_SaveDeviceState(IN t_hash_device_id hash_device_id, OUT t_hash_state *hash_device_state)
{
    t_uint32        temp_cr;
    t_uint32        count;
    t_hash_error    hash_error = HASH_OK;

    DBGENTER1("Pointer to device state: ( %p )", hash_device_state);
    if (NULL == hash_device_state)
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
    
    for (count = 0; count < HASH_BLOCK_SIZE / sizeof(t_uint32); count++)
    {
        hash_device_state->hash_state_buffer[count] = g_hash_system_context.hash_state[hash_device_id].hash_state_buffer[count];
    }

    hash_device_state->hash_state_index = g_hash_system_context.hash_state[hash_device_id].hash_state_index;
    hash_device_state->hash_state_bit_index = g_hash_system_context.hash_state[hash_device_id].hash_state_bit_index;
    hash_device_state->hash_state_length = g_hash_system_context.hash_state[hash_device_id].hash_state_length;

    /* Write dummy value to force digest intermediate calculation */

    while ((t_bool) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_str & HASH_STR_DCAL_MASK) >> HASH_STR_DCAL_POS))
        ;

    temp_cr = g_hash_system_context.p_hash_register[hash_device_id]->hash_cr;
    hash_device_state->hash_state_str_reg = g_hash_system_context.p_hash_register[hash_device_id]->hash_str;


    if (g_hash_system_context.p_hash_register[hash_device_id]->hash_cr & HASH_CR_DINF_MASK)
    {
        hash_device_state->hash_state_din_reg = g_hash_system_context.p_hash_register[hash_device_id]->hash_din;
        for (count = 0; count < (HASH_CSR_COUNT - 2); count++)  /* for csr0-csr51 */
        {
            if
            (
                (count >= 36)
            &&  (t_bool) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_cr & HASH_CR_MODE_MASK) >> HASH_CR_MODE_POS)
            )
            {
                break;
            }

            hash_device_state->hash_state_csr[count] = g_hash_system_context.p_hash_register[hash_device_id]->hash_csrx[count];
        }   /* end for */

        hash_device_state->hash_state_csfull = g_hash_system_context.p_hash_register[hash_device_id]->hash_csfull;   /* Status flags */
        hash_device_state->hash_state_csdatain = g_hash_system_context.p_hash_register[hash_device_id]->hash_csdatain;   /* Input data */
    }   /* end if */


    hash_device_state->hash_state_temp_cr = temp_cr;
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/****************************************************************************/
/* NAME         :   HASH_ResumeDeviceState                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Resumes the state of the current HASH computation.      */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   hash_state  : Caller allocated structure from which the */
/*                                computation state is restored.            */
/*      OUT     :   none                                                    */
/*                                                                          */
/* RETURN       :   HASH_INVALID_PARAMETER  : If hash_state is NULL or      */
/*                                            *hash_state has internal      */
/*                                            incongruences                 */
/*                  HASH_OK                    : Returns this if no error      */
/*                                            detected.                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC t_hash_error HASH_ResumeDeviceState(IN t_hash_device_id hash_device_id, IN const t_hash_state *hash_device_state)
{
    t_uint32        temp_cr;
    t_hash_error    hash_error = HASH_OK;
    t_sint32        count;

    DBGENTER1("Pointer to device state: ( %p )", hash_device_state);
    if (NULL == hash_device_state)
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }
   
    if (!((HASH_DEVICE_ID_0 == hash_device_id) || (HASH_DEVICE_ID_1 == hash_device_id)))
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    /* Check correctness of index and length members */
    if
    (
        hash_device_state->hash_state_index > HASH_BLOCK_SIZE
    ||  (hash_device_state->hash_state_length.hash_low_word % HASH_BLOCK_SIZE) != 0
    )
    {
        hash_error = HASH_INVALID_PARAMETER;
        DBGEXIT0(hash_error);
        return(hash_error);
    }

    for (count = 0; count < (t_sint32) (HASH_BLOCK_SIZE / sizeof(t_uint32)); count++)
    {
        g_hash_system_context.hash_state[hash_device_id].hash_state_buffer[count] = hash_device_state->hash_state_buffer[count];
    }

    g_hash_system_context.hash_state[hash_device_id].hash_state_index = hash_device_state->hash_state_index;
    g_hash_system_context.hash_state[hash_device_id].hash_state_bit_index = hash_device_state->hash_state_bit_index;
    g_hash_system_context.hash_state[hash_device_id].hash_state_length = hash_device_state->hash_state_length;

    HASH_INITIALIZE;    /* Initialize the device */

    temp_cr = hash_device_state->hash_state_temp_cr;
    g_hash_system_context.p_hash_register[hash_device_id]->hash_cr = temp_cr & HASH_CR_RESUME_MASK;

    if (temp_cr & HASH_CR_DINF_MASK)
    {
        for (count = 0; count < (HASH_CSR_COUNT - 2); count++)  /* for csr0-csr51 */
        {
            if
            (
                (count >= 36)
            &&  (t_bool) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_cr & HASH_CR_MODE_MASK) >> HASH_CR_MODE_POS)
            )
            {
                break;
            }

            g_hash_system_context.p_hash_register[hash_device_id]->hash_csrx[count] = hash_device_state->hash_state_csr[count];
        }   /* end for */

        g_hash_system_context.p_hash_register[hash_device_id]->hash_csfull = hash_device_state->hash_state_csfull;   /* Status flags */
        g_hash_system_context.p_hash_register[hash_device_id]->hash_csdatain = hash_device_state->hash_state_csdatain;   /* Input data */
    }                   /* end if */

    g_hash_system_context.p_hash_register[hash_device_id]->hash_str = hash_device_state->hash_state_str_reg;
    g_hash_system_context.p_hash_register[hash_device_id]->hash_cr = temp_cr;
    
    DBGEXIT0(hash_error);
    return(hash_error);
}

/*--------------------------------------------------------------------------*
 * Private functions                                                        *
 *--------------------------------------------------------------------------*/
#ifdef __HASH_ENHANCED  /* Enhanced layer APIs */

/****************************************************************************/
/* NAME         :   hash_ProcessBlock                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This function processes a single block of 512 bits      */
/*                  (64 bytes), word aligned, starting at message.          */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   message : Block (512 bits) of message to be written to  */
/*                            the HASH core                                 */
/*                                                                          */
/* RETURN       :                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PRIVATE void hash_ProcessBlock(IN t_hash_device_id hash_device_id, IN const t_uint32 *message)
{
    t_uint32    count;

        /* Partially unrolled loop */
        for (count = 0; count < (HASH_BLOCK_SIZE / sizeof(t_uint32)); count += 4)
        {
            HASH_SET_DIN(message[0]);
            HASH_SET_DIN(message[1]);
            HASH_SET_DIN(message[2]);
            HASH_SET_DIN(message[3]);
            message += 4;
        }
}

/****************************************************************************/
/* NAME         :   hash_MessagePad                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This function manages the final part of the digest      */
/*                  calculation, when less than 512 bits (64 bytes) remain  */
/*                  in message. This means num_of_bytes < 64.               */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   message     :   Final part of the message               */
/*              :   index_bytes :   Number of bytes of the final message    */
/*                                                                          */
/* RETURN       :                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PRIVATE void hash_MessagePad(IN t_hash_device_id hash_device_id, IN const t_uint32 *message, IN t_uint8 index_bytes)
{
    /* Main loop */
    while (index_bytes >= 4)
    {
        HASH_SET_DIN(message[0]);
        index_bytes -= 4;
        message++;
    }

    /* Intentional fall-through; 0 <= num_of_bytes <= 3 */
    switch (index_bytes)
    {
        case 3:
        case 2:
        case 1:
            HASH_SET_DIN(message[0]);

        case 0:
            while ((t_bool) ((g_hash_system_context.p_hash_register[hash_device_id]->hash_str & HASH_STR_DCAL_MASK) >> HASH_STR_DCAL_POS))
                ;

            /* num_of_bytes == 0 => NBLW <- 0 (32 bits valid in DATAIN) */
            HASH_SET_NBLW((index_bytes * 8) + g_hash_system_context.hash_state[hash_device_id].hash_state_bit_index);
            HASH_SET_DCAL;
            break;

        default:
            break;
    } 
}

/****************************************************************************/
/* NAME         :   hash_IncrementLength                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Increments the length of current message. Overflow      */
/*                  cannot occur, because conditions for overflow are       */
/*                  checked in HASH_Update.                                 */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   incr    : Length of message processed already           */
/*                                                                          */
/* RETURN       :                                                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/****************************************************************************/
PRIVATE void hash_IncrementLength(IN t_hash_device_id hash_device_id, IN t_uint32 incr)
{
    g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_low_word += incr;

    /* Check for wrap-around */
    if (g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_low_word < incr)
    {
        g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_high_word++;
    }
}
#endif /* __HASH_ENHANCED */

/****************************************************************************/
/* NAME         :   hash_InitializeGlobals                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This function intialize all the global variables to     */
/*                  their default reset value                               */
/*                                                                          */
/* PARAMETERS   :                                                           */
/*      IN      :   None                                                    */
/*                                                                          */
/* RETURN       :   None                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   Non Re-entrant                                          */
/* REENTRANCY ISSUES:                                                        */
/*                1)    Global structure g_hash_system_context elements are        */
/*                    being modified                                            */
/****************************************************************************/
PRIVATE void hash_InitializeGlobals(IN t_hash_device_id hash_device_id)
{
    t_uint8 loop_count;

    /* Resetting the values of global variables except the p_hash_register */
    g_hash_system_context.hash_state[hash_device_id].hash_state_temp_cr = HASH_RESET_INDEX_VAL;
    g_hash_system_context.hash_state[hash_device_id].hash_state_str_reg = HASH_RESET_INDEX_VAL;
    g_hash_system_context.hash_state[hash_device_id].hash_state_din_reg = HASH_RESET_INDEX_VAL;

    for (loop_count = 0; loop_count < (HASH_CSR_COUNT - 2); loop_count++)
    {
        g_hash_system_context.hash_state[hash_device_id].hash_state_csr[loop_count] = HASH_RESET_CSRX_REG_VALUE;
    }

    g_hash_system_context.hash_state[hash_device_id].hash_state_csfull = HASH_RESET_CSFULL_REG_VALUE;
    g_hash_system_context.hash_state[hash_device_id].hash_state_csdatain = HASH_RESET_CSDATAIN_REG_VALUE;

    for (loop_count = 0; loop_count < (HASH_BLOCK_SIZE / sizeof(t_uint32)); loop_count++)
    {
        g_hash_system_context.hash_state[hash_device_id].hash_state_buffer[loop_count] = HASH_RESET_BUFFER_VAL;
    }

    g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_high_word = HASH_RESET_LEN_HIGH_VAL;
    g_hash_system_context.hash_state[hash_device_id].hash_state_length.hash_low_word = HASH_RESET_LEN_LOW_VAL;
    g_hash_system_context.hash_state[hash_device_id].hash_state_index = HASH_RESET_INDEX_VAL;
    g_hash_system_context.hash_state[hash_device_id].hash_state_bit_index = HASH_RESET_BIT_INDEX_VAL;
}

/* End of file - hash.c */

