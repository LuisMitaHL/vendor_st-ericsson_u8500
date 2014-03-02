/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the PWL
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*
 * PWL low level driver provides various methods for enabling, disabling, pwl.
 * Also to get the PWL Level, and set the PWL Level.
 * Some assumptions are made:
 *
 * - WARNING: no public function must be called before the PWL_Init(),
 *   obviously, but this is not handled at this level.
 */
/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
 

#include "pwl.h"
#include "pwlp.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME my_debuglevel_pwl
#define MY_DEBUG_ID             my_debug_id_pwl

/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/
/* For debug HCL */
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = PWL_HCL_DBG_ID;
#endif /* __DEBUG */

/* Pointer to PWL registers structure */
PRIVATE t_pwl_system_context    g_pwl_system_context;

/*****************************************************************************/
/* NAME:    t_pwl_error PWL_SetDbgLevel(t_dbg_level debug_level)             */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different debug       */
/*              comments levels                                              */
/*                                                                           */
/* PARAMETERS :                                                              */
/* IN         : t_dbg_level debug_level: set PWL debug level                 */
/*                                                                           */
/* OUT        : None                                                         */
/*                                                                           */
/*                                                                           */
/* RETURN     : t_pwl_error: PWL_OK                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */
/*****************************************************************************/
PUBLIC t_pwl_error PWL_SetDbgLevel(IN t_dbg_level debug_level)
{
    DBGENTER1("debug_level = %u\r\n", debug_level);

#ifdef __DEBUG
    my_debuglevel_pwl = debug_level;
#endif /* __DEBUG */

    DBGEXIT0(PWL_OK);

    return(PWL_OK);
}

/*****************************************************************************/
/* NAME:    t_pwl_error PWL_GetDbgLevel(t_dbg_level *p_debug_level)          */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different debug       */
/*              comments levels                                              */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :  None                                                                */
/*                                                                           */
/* OUT: t_dbg_level p_debug_level: identify PWL debug level                  */
/*                                                                           */
/* RETURN: t_pwl_error           : PWL_OK                                    */
/*                               : PWL_INVALID_PARAMETER: if null pointer is */
/*                                                        passed             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */
/*****************************************************************************/
PUBLIC t_pwl_error PWL_GetDbgLevel(OUT t_dbg_level *p_debug_level)
{
    DBGENTER1("p_debug_level = @%p\r\n", (void *) p_debug_level);

    if (NULL == p_debug_level)
    {
        DBGEXIT0(PWL_INVALID_PARAMETER);
        return(PWL_INVALID_PARAMETER);
    }

#ifdef __DEBUG
    * p_debug_level = my_debuglevel_pwl;
#endif /* __DEBUG */

    DBGEXIT0(PWL_OK);
    return(PWL_OK);
}

/*****************************************************************************/
/* NAME:  t_pwl_error PWL_GetVersion(t_version *p_version)                   */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the PWL HCL version                     */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN        : None                                                          */
/*                                                                           */
/* OUT       : t_version *p_version: The PWL HCL version                     */
/*                                                                           */
/* RETURN: t_pwl_error: PWL_OK                                               */
/*                    : PWL_INVALID_PARAMETER :if null pointer is passed     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */
/*****************************************************************************/
PUBLIC t_pwl_error PWL_GetVersion(OUT t_version *p_version)
{
    DBGENTER1("p_version = @%p\r\n", (void *) p_version);

    if (NULL == p_version)
    {
        DBGEXIT0(PWL_INVALID_PARAMETER);
        return(PWL_INVALID_PARAMETER);
    }

    p_version->version = PWL_HCL_VERSION_ID;
    p_version->major = PWL_HCL_MAJOR_ID;
    p_version->minor = PWL_HCL_MINOR_ID;

    DBGEXIT3
    (
        PWL_OK,
        "major=%08d, minor=%08d, version=%08d\r\n",
        p_version->version,
        p_version->major,
        p_version->minor
    );

    return(PWL_OK);
}

/*****************************************************************************/
/* NAME:  t_pwl_error PWL_Init(t_logical_address pwl_base_address)           */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the PWL registers. The level is     */
/*              reset to 0x00 and the pwl is disabled. It also initializes   */
/*              internal data structures of the driver.                      */
/* PARAMETERS:                                                               */
/* IN :         pwl_base_address :regsiter base address                      */
/*                                                                           */
/* OUT :        None                                                         */
/*                                                                           */
/* RETURN: t_pwl_error: PWL_OK                                               */
/*                    : PWL_INVALID_PARAMETER: if null pointer is passed     */
/*                    : PWL_UNSUPPORTED_HW: if defined incompatible platforms*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */
/*****************************************************************************/
PUBLIC t_pwl_error PWL_Init(IN t_logical_address pwl_base_address)
{
    DBGENTER1("pwl_base_address @= %p\r\n", pwl_base_address);

    /* Initialize PWL registers structure */
    if (NULL != pwl_base_address)
    {
        g_pwl_system_context.p_pwl_register = (t_pwl_register *) pwl_base_address;

        /* Checking Peripheral Ids *
         *-------------------------*/
        if
        (
            (PWL_P_ID0 == g_pwl_system_context.p_pwl_register->pwl_periphid0) &&
            (PWL_P_ID1 == g_pwl_system_context.p_pwl_register->pwl_periphid1) &&
            (PWL_P_ID2 == g_pwl_system_context.p_pwl_register->pwl_periphid2) &&
            (PWL_P_ID3 == g_pwl_system_context.p_pwl_register->pwl_periphid3) &&
            (PWL_CELL_ID0 == g_pwl_system_context.p_pwl_register->pwl_cellid0) &&
            (PWL_CELL_ID1 == g_pwl_system_context.p_pwl_register->pwl_cellid1) &&
            (PWL_CELL_ID2 == g_pwl_system_context.p_pwl_register->pwl_cellid2) &&
            (PWL_CELL_ID3 == g_pwl_system_context.p_pwl_register->pwl_cellid3)
        )
        {
             /* Disable PWL */
             DISABLE_PWL;

             /* Set level to 0x00UL */
             SET_PWL_LEVEL(PWL_MIN_LEVEL);

	     DBGEXIT0(PWL_OK);
             return(PWL_OK);
        }
        else
        {
             DBGEXIT0(PWL_UNSUPPORTED_HW);
             return(PWL_UNSUPPORTED_HW);
        }
    }   /* end if */

    else
    {
        DBGEXIT0(PWL_INVALID_PARAMETER);
	return(PWL_INVALID_PARAMETER);
    }
    
}

/*****************************************************************************/
/* NAME:  t_pwl_error PWL_SetLevel(  t_uint32  pwl_level )                   */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to set PWL level.                        */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :      t_uint32 pwl_level :This value is set as PWL level              */
/*                                                                           */
/* OUT :     None                                                            */
/*                                                                           */
/* RETURN: t_pwl_error: PWL_INVALID_PARAMETER: If pwl_level is out of range  */ 
/*                    : PWL_OK Otherwise                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                     */
/*****************************************************************************/
PUBLIC t_pwl_error PWL_SetLevel(IN t_uint32 pwl_level)
{
    DBGENTER1(" PWl Level = %x\r\n", pwl_level);

    /* Check for out of range level values */
    if (PWL_MAX_LEVEL < pwl_level)
    {
        DBGEXIT0(PWL_INVALID_PARAMETER);
        return(PWL_INVALID_PARAMETER);
    }

    /* Set level to pwl_level */
    SET_PWL_LEVEL(pwl_level);

    DBGEXIT0(PWL_OK);
    return(PWL_OK);
}

/*****************************************************************************/
/* NAME:  t_pwl_error PWL_GetLevel(  t_uint32  *pwl_level )                  */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to set PWL level.                        */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :      t_uint32 pwl_level :PWL level is read into this argument        */
/*                                                                           */
/* OUT :     None                                                            */
/*                                                                           */
/* RETURN: t_pwl_error: PWL_OK                                               */
/*                    : PWL_INVALID_PARAMETER :if null pointer is passed     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                     */
/*****************************************************************************/
PUBLIC t_pwl_error PWL_GetLevel(OUT t_uint32 *pwl_level)
{
    DBGENTER1("pwl_level @= %p\r\n", pwl_level);

    /* Initialize PWL registers structure */
    if (NULL != pwl_level)
    {
        /* Get pwl level */
        GET_PWL_LEVEL(*pwl_level);
    }
    else
    {
        DBGEXIT0(PWL_INVALID_PARAMETER);
        return(PWL_INVALID_PARAMETER);
    }

    DBGEXIT0(PWL_OK);
    return(PWL_OK);
}

/*****************************************************************************/
/* NAME:  t_pwl_error PWL_Enable()                                           */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine Enables PWL                                     */
/* PARAMETERS:                                                               */
/* IN :         None                                                         */
/*                                                                           */
/* OUT :        None                                                         */
/*                                                                           */
/* RETURN: t_pwl_error : PWL_OK                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                     */
/*****************************************************************************/
PUBLIC t_pwl_error PWL_Enable(void)
{
    DBGENTER0();

    /* Enable PWL */
    ENABLE_PWL;

    DBGEXIT0(PWL_OK);
    return(PWL_OK);
}

/*****************************************************************************/
/* NAME:  t_pwl_error PWL_Disable()                                          */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine Disables PWL                                    */
/* PARAMETERS:                                                               */
/* IN :         None                                                         */
/*                                                                           */
/* OUT :        None                                                         */
/*                                                                           */
/* RETURN: t_pwl_error : PWL_OK                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                     */
/*****************************************************************************/
PUBLIC t_pwl_error PWL_Disable(void)
{
    DBGENTER0();

    /* Disable PWL */
    DISABLE_PWL;

    DBGEXIT0(PWL_OK);
    return(PWL_OK);
}

/*****************************************************************************/
/* NAME:  t_pwl_error PWL_Reset()                                            */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine Resets PWL                                      */
/* PARAMETERS:                                                               */
/* IN :         None                                                         */
/*                                                                           */
/* OUT :        None                                                         */
/*                                                                           */
/* RETURN: t_pwl_error : PWL_OK                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                     */
/*****************************************************************************/
PUBLIC t_pwl_error PWL_Reset(void)
{
    DBGENTER0();

    g_pwl_system_context.p_pwl_register->pwl_cr = 0x0UL;
    g_pwl_system_context.p_pwl_register->pwl_lvr = 0x0UL;
    g_pwl_system_context.p_pwl_register = NULL;

    DBGEXIT0(PWL_OK);
    return(PWL_OK);
}

/****************************************************************************/
/* NAME       :         PWL_SetIntegrationTestMode                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: It Enables/Disables PWL Integration test mode               */
/* PARAMETERS :                                                             */
/* IN         : pwl_integ_test_mode: Enable/Disable Test mode               */
/* OUT        : none                                                        */                                                   
/* RETURN     : PWL_OK always                                               */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:Re-entrant                                                    */

/****************************************************************************/
PUBLIC t_pwl_error PWL_SetIntegrationTestMode(IN t_pwl_integ_test_mode pwl_integ_test_mode)
{
    DBGENTER1("Test Mode Control = %d\n", (t_uint32)pwl_integ_test_mode);

    PWL_WRITE_BITS( g_pwl_system_context.p_pwl_register->pwl_itcr, (t_uint32)pwl_integ_test_mode, ONE_BIT, PWL_TEST_MODE_ENABLE);

    DBGEXIT(PWL_OK);
    return(PWL_OK);
}

/****************************************************************************/
/* NAME       :         PWL_ProgramIntegrationTestOutputs                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: It enables the PWLOUT signals to be driven directly in the  */
/*              integration test mode                                       */
/* PARAMETERS :                                                             */
/* IN         : pwl_drive_level    : Drive of 0/1 at the output PWLOUT      */
/* OUT        : none                                                        */                                                   
/* RETURN     : PWL_OK always                                               */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:Re-entrant                                                    */

/****************************************************************************/
PUBLIC t_pwl_error PWL_ProgramIntegrationTestOutputs(IN t_pwl_drive_level pwl_drive_level)
{
    DBGENTER1("PWLOUT Drive = %d\r\n", (t_uint32)pwl_drive_level);

    PWL_WRITE_BITS( g_pwl_system_context.p_pwl_register->pwl_itop, (t_uint32)pwl_drive_level, ONE_BIT, PWL_OUTPUT_PWLOUT);

    DBGEXIT(PWL_OK);
    return(PWL_OK);
}
