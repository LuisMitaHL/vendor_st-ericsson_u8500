/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides HCL for the SKE (Scroll key and Keypad Encoder)
*  Specification release related to this implementation: A_V2.2
*  Reference : Software Code Development, C Coding Rules, Guidelines 
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#include "ske.h"
#include "ske_hwp.h"
#include "ske_p.h"

#ifdef __DEBUG
PRIVATE t_dbg_level myDebugLevel_SKE = DEBUG_LEVEL0;
PRIVATE t_dbg_id    myDebugID_SKE = SKE_HCL_DBG_ID;
#endif
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_SKE
#define MY_DEBUG_ID             myDebugID_SKE

/* Global SKE system context variable */
PRIVATE volatile t_ske_system_context    g_ske_system_context =
{
    0,
    (t_uint8) SKE_KEYPAD_COLUMN_MAX,
    (t_uint8) SKE_KEYPAD_ROW_MAX
};

/*******************************************************************************************/
/* NAME:  SKE_Init                                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is called for Initialisation of SKE Hardware base address    */
/* PARAMETERS:                                                                             */
/* IN:    ske_base_address: Base address of SKE                                            */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*              SKE_UNSUPPORTED_HW   :  If SKE peripheral ids don't match                  */
/*              SKE_INVALID_PARAMETER:	If SKE base address passed is zero                 */
/*              SKE_OK               :	If no error                                        */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_Init(IN t_logical_address ske_base_address)
{
    DBGENTER1("SKE Base Address :%lx", ske_base_address);   /* Debug Macros defined in debug.h */
    if (0 == ske_base_address)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);                    /* Debug Macros defined in debug.h */
        return(SKE_INVALID_PARAMETER);
    }

    /* Saving SKE base address to global ske system context variable */
    g_ske_system_context.p_ske_register = (t_ske_register *) ske_base_address;

    /* Checking SKE Peripheral IDs & PCellID */
    if
    (
        (SKE_PERIPH_ID_0 != (g_ske_system_context.p_ske_register->ske_periphid0))
    ||  (SKE_PERIPH_ID_1 != (g_ske_system_context.p_ske_register->ske_periphid1))
    ||  (SKE_PERIPH_ID_2 != (g_ske_system_context.p_ske_register->ske_periphid2))
    ||  (SKE_PERIPH_ID_3 != (g_ske_system_context.p_ske_register->ske_periphid3))
    ||  (SKE_P_CELL_ID_0 != (g_ske_system_context.p_ske_register->ske_pcellid0))
    ||  (SKE_P_CELL_ID_1 != (g_ske_system_context.p_ske_register->ske_pcellid1))
    ||  (SKE_P_CELL_ID_2 != (g_ske_system_context.p_ske_register->ske_pcellid2))
    ||  (SKE_P_CELL_ID_3 != (g_ske_system_context.p_ske_register->ske_pcellid3))
    )
    {
        DBGEXIT0(SKE_UNSUPPORTED_HW);                       /* Debug Macros defined in debug.h */
        return(SKE_UNSUPPORTED_HW);
    }

    DBGEXIT0(SKE_OK);   /* Debug Macros defined in debug.h */
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_GetVersion                                                                   */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is called for getting the SKE HCL version used               */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   p_ske_hcl_version: SKE HCL version currently used                                */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If address passed for writing SKE HCL version is zero */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetVersion(OUT t_version *p_ske_hcl_version)
{
    DBGENTER1("Address of output variable(for SKE HCL version) :%lx", p_ske_hcl_version);
    if (0 == p_ske_hcl_version)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    p_ske_hcl_version->version = SKE_HCL_VERSION_ID;
    p_ske_hcl_version->major = SKE_HCL_MAJOR_ID;
    p_ske_hcl_version->minor = SKE_HCL_MINOR_ID;

    DBGEXIT3(SKE_OK, "SKE HCL version : %d.%d.%d", SKE_HCL_VERSION_ID, SKE_HCL_MAJOR_ID, SKE_HCL_MINOR_ID);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_SetDbgLevel                                                                  */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the debug level for SKE HCL                              */
/*              Debug levels are defined in debug.h file                                   */
/* PARAMETERS:                                                                             */
/* IN:    dbg_level: SKE debug level                                                       */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_UNSUPPORTED_FEATURE: If not compiled for debug                           */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_SetDbgLevel(IN t_dbg_level dbg_level)
{
#if defined(__DEBUG)
    DBGENTER1("Debug Level :%d", dbg_level);
    myDebugLevel_SKE = dbg_level;
    DBGEXIT0(SKE_OK);
    return(SKE_OK);
#else
    /* dbg_level = dbg_level;   To remove warnings TBD */
    return(SKE_UNSUPPORTED_FEATURE);
#endif
}

/*******************************************************************************************/
/* NAME:  SKE_EnableScrollKey                                                              */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function enables the required Scroll key encoder                      */
/* PARAMETERS:                                                                             */
/* IN:    scroll_device: Scroll Device(Scroll Key encoder 0 or Scroll Key encoder 1)       */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If wrong value for scroll device is passed            */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_EnableScrollKey(IN t_ske_scroll_device scroll_device)
{
    DBGENTER1("Scroll Device :%d", scroll_device);
    if (SKE_SCROLL_DEVICE_0 == scroll_device)
    {
        HCL_SET_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_SKEN0_MASK);
    }
    else if (SKE_SCROLL_DEVICE_1 == scroll_device)
    {
        HCL_SET_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_SKEN1_MASK);
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_DisableScrollKey                                                             */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function disables the required Scroll key encoder                     */
/* PARAMETERS:                                                                             */
/* IN:    scroll_device: Scroll Device(Scroll Key encoder 0 or Scroll Key encoder 1)       */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If wrong value for scroll device is passed            */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_DisableScrollKey(IN t_ske_scroll_device scroll_device)
{
    DBGENTER1("Scroll Device :%d", scroll_device);
    if (SKE_SCROLL_DEVICE_0 == scroll_device)
    {
        HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_SKEN0_MASK);
    }
    else if (SKE_SCROLL_DEVICE_1 == scroll_device)
    {
        HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_SKEN1_MASK);
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_SetScrollKeyCount                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function sets the count value for the required Scroll key encoder     */
/* PARAMETERS:                                                                             */
/* IN:    scroll_device: Scroll Device(Scroll Key encoder 0 or Scroll Key encoder 1)       */
/*        count_value : Count value to set                                                 */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If wrong value for scroll device is passed            */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_SetScrollKeyCount(IN t_ske_scroll_device scroll_device, IN t_uint8 count_value)
{
    DBGENTER2("Scroll Device :%d & Count value : %d", scroll_device, count_value);
    if (SKE_SCROLL_DEVICE_0 == scroll_device)
    {
        HCL_WRITE_BITS
        (
            g_ske_system_context.p_ske_register->ske_val0,
            (count_value << SKE_SKEVAL0_SHIFT),
            SKE_SKEVAL0_MASK
        );
    }
    else if (SKE_SCROLL_DEVICE_1 == scroll_device)
    {
        HCL_WRITE_BITS
        (
            g_ske_system_context.p_ske_register->ske_val1,
            (count_value << SKE_SKEVAL1_SHIFT),
            SKE_SKEVAL1_MASK
        );
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_GetScrollKeyCount                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function sets the count value for the required Scroll key encoder     */
/* PARAMETERS:                                                                             */
/* IN:    scroll_device: Scroll Device(Scroll Key encoder 0 or Scroll Key encoder 1)       */
/* INOUT: None                                                                             */
/* OUT:   p_count_value : Count value returned                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If wrong value for scroll device is passed  or        */
/*                                   If output pointer variable is zero                    */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetScrollKeyCount(IN t_ske_scroll_device scroll_device, OUT t_uint8 *p_count_value)
{
    DBGENTER2("Scroll Device :%d & Address of output variable(for Count value) : %lx", scroll_device, p_count_value);
    if (0 == p_count_value)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    if (SKE_SCROLL_DEVICE_0 == scroll_device)
    {
        *p_count_value =
            (
                (t_uint8)(HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_val0, SKE_SKEVAL0_MASK)) >>
                SKE_SKEVAL0_SHIFT
            );
    }
    else if (SKE_SCROLL_DEVICE_1 == scroll_device)
    {
        *p_count_value =
            (
                (t_uint8)(HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_val1, SKE_SKEVAL1_MASK)) >>
                SKE_SKEVAL1_SHIFT
            );
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT1(SKE_OK, "Count value returned :%d", *p_count_value);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_ClearScrollKeyOverflowFlag                                                   */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function clears the overflow flag for the required Scroll key encoder */
/* PARAMETERS:                                                                             */
/* IN:    scroll_device: Scroll Device(Scroll Key encoder 0 or Scroll Key encoder 1)       */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If wrong value for scroll device is passed            */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_ClearScrollKeyOverflowFlag(IN t_ske_scroll_device scroll_device)
{
    DBGENTER1("Scroll Device :%d", scroll_device);
    if (SKE_SCROLL_DEVICE_0 == scroll_device)
    {
        HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_val0, SKE_OVF0_MASK);
    }
    else if (SKE_SCROLL_DEVICE_1 == scroll_device)
    {
        HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_val1, SKE_OVF1_MASK);
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_GetScrollKeyOverflowStatus                                                   */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets the overflow status for the required Scroll key encoder */
/* PARAMETERS:                                                                             */
/* IN:    scroll_device: Scroll Device(Scroll Key encoder 0 or Scroll Key encoder 1)       */
/* INOUT: None                                                                             */
/* OUT:   p_overflow_state : Overflow status                                               */
/*          TRUE - if overflow flag is set                                                 */
/*          FALSE - if overflow flag is cleared                                            */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If wrong value for scroll device is passed or         */
/*                                   If output pointer variable is zero                    */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetScrollKeyOverflowStatus(IN t_ske_scroll_device scroll_device, OUT t_bool *p_overflow_state)
{
    DBGENTER2
    (
        "Scroll Device :%d & Address of output variable(for overflow flag state) : %lx",
        scroll_device,
        p_overflow_state
    );
    if (0 == p_overflow_state)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    if (SKE_SCROLL_DEVICE_0 == scroll_device)
    {
        if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_val0, SKE_OVF0_MASK))
        {
            *p_overflow_state = TRUE;
        }
        else
        {
            *p_overflow_state = FALSE;
        }
    }
    else if (SKE_SCROLL_DEVICE_1 == scroll_device)
    {
        if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_val1, SKE_OVF1_MASK))
        {
            *p_overflow_state = TRUE;
        }
        else
        {
            *p_overflow_state = FALSE;
        }
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT1(SKE_OK, "Overflow state :%d", *p_overflow_state);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_ClearScrollKeyUnderflowFlag                                                  */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:This function clears the underflow flag for the required Scroll key encoder */
/* PARAMETERS:                                                                             */
/* IN:    scroll_device: Scroll Device(Scroll Key encoder 0 or Scroll Key encoder 1)       */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If wrong value for scroll device is passed            */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_ClearScrollKeyUnderflowFlag(IN t_ske_scroll_device scroll_device)
{
    DBGENTER1("Scroll Device :%d", scroll_device);
    if (SKE_SCROLL_DEVICE_0 == scroll_device)
    {
        HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_val0, SKE_UDF0_MASK);
    }
    else if (SKE_SCROLL_DEVICE_1 == scroll_device)
    {
        HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_val1, SKE_UDF1_MASK);
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_GetScrollKeyUnderflowStatus                                                  */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:This function gets the underflow status for the required Scroll key encoder */
/* PARAMETERS:                                                                             */
/* IN:    scroll_device: Scroll Device(Scroll Key encoder 0 or Scroll Key encoder 1)       */
/* INOUT: None                                                                             */
/* OUT:   p_underflow_state: Underflow status                                              */
/*          TRUE - if underflow flag is set                                                */
/*          FALSE - if underflow flag is cleared                                           */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If wrong value for scroll device is passed or         */
/*                                   If output pointer variable is zero                    */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetScrollKeyUnderflowStatus(IN t_ske_scroll_device scroll_device, OUT t_bool *p_underflow_state)
{
    DBGENTER2
    (
        "Scroll Device :%d & Address of output variable(for underflow flag state) : %lx",
        scroll_device,
        p_underflow_state
    );
    if (0 == p_underflow_state)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    if (SKE_SCROLL_DEVICE_0 == scroll_device)
    {
        if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_val0, SKE_UDF0_MASK))
        {
            *p_underflow_state = TRUE;
        }
        else
        {
            *p_underflow_state = FALSE;
        }
    }
    else if (SKE_SCROLL_DEVICE_1 == scroll_device)
    {
        if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_val1, SKE_UDF1_MASK))
        {
            *p_underflow_state = TRUE;
        }
        else
        {
            *p_underflow_state = FALSE;
        }
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT1(SKE_OK, "Underflow state :%d", *p_underflow_state);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_SetScrollKeyDebounce                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function sets the debounce value for the required Scroll key encoder  */
/* PARAMETERS:                                                                             */
/* IN:    debounce_value :Debounce value to set(Same for both Scroll Key Encoders)         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SKE_SetScrollKeyDebounce(IN t_uint8 debounce_value)
{
    DBGENTER1("Debounce value :%d", debounce_value);
    HCL_WRITE_BITS
    (
        g_ske_system_context.p_ske_register->ske_dbcr,
        (debounce_value << SKE_SKEDBC_SHIFT),
        SKE_SKEDBC_MASK
    );
    DBGEXIT0(0);
}

/*******************************************************************************************/
/* NAME:  SKE_GetScrollKeyDebounce                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets the debounce value for the required Scroll key encoder  */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   p_debounce_value :Debounce value returned                                        */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If output pointer variable is zero                    */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetScrollKeyDebounce(OUT t_uint8 *p_debounce_value)
{
    DBGENTER1("Address of output variable(for Debounce value) :%lx", p_debounce_value);
    if (0 != p_debounce_value)
    {
        *p_debounce_value =
            (
                (t_uint8)(HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_dbcr, SKE_SKEDBC_MASK)) >>
                SKE_SKEDBC_SHIFT
            );
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT1(SKE_OK, "Debounce value returned : %d", *p_debounce_value);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_SetKeypadDebounce                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function sets the debounce value for the Keypad encoder               */
/* PARAMETERS:                                                                             */
/* IN:    debounce_value :Debounce value to set for the Keypad                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SKE_SetKeypadDebounce(IN t_uint8 debounce_value)
{
    DBGENTER1("Debounce value :%d", debounce_value);
    HCL_WRITE_BITS(g_ske_system_context.p_ske_register->ske_dbcr, (debounce_value << SKE_KPDBC_SHIFT), SKE_KPDBC_MASK);
    DBGEXIT0(0);
}

/*******************************************************************************************/
/* NAME:  SKE_GetKeypadDebounce                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets the debounce value for the Keypad encoder               */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   p_debounce_value : Debounce value                                                */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If output pointer variable is zero                    */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetKeypadDebounce(OUT t_uint8 *p_debounce_value)
{
    DBGENTER1("Address of output variable(for Debounce value) :%lx", p_debounce_value);
    if (0 != p_debounce_value)
    {
        *p_debounce_value =(t_uint8)
            (
                (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_dbcr, SKE_KPDBC_MASK)) >>
                SKE_KPDBC_SHIFT
            );
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT1(SKE_OK, "Debounce value returned : %d", *p_debounce_value);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_EnableKeypadAutoScan                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function enables Autoscan for the Keypad encoder                      */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SKE_EnableKeypadAutoScan(void)
{
    DBGENTER0();
    HCL_SET_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPASEN_MASK);
    DBGEXIT0(0);
}

/*******************************************************************************************/
/* NAME:  SKE_DisableKeypadAutoScan                                                        */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function disables Autoscan for the Keypad encoder                     */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SKE_DisableKeypadAutoScan(void)
{
    DBGENTER0();
    HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPASEN_MASK);
    DBGEXIT0(0);
}

/*******************************************************************************************/
/* NAME:  SKE_IsKeypadAutoScanEnabled                                                      */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:This function checks if Autoscan is enabled in the Keypad Encoder           */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_bool  which can take any of following values -                                */
/*          TRUE - if Auto Scan is enabled                                                 */
/*          FALSE - if Auto Scan disabled                                                  */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_bool SKE_IsKeypadAutoScanEnabled(void)
{
    DBGENTER0();
    if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPASEN_MASK))
    {
        DBGEXIT0(TRUE);
        return(TRUE);
    }
    else
    {
        DBGEXIT0(FALSE);
        return(FALSE);
    }
}

/*******************************************************************************************/
/* NAME:  SKE_IsAutoScanOngoing                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:This function checks if Autoscan is on-going in the Keypad Encoder          */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_bool  which can take any of following values -                                */
/*          TRUE - if Auto Scan is on-going                                                */
/*          FALSE - if Auto Scan completed or disabled                                     */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_bool SKE_IsAutoScanOngoing(void)
{
    DBGENTER0();
    if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPASON_MASK))
    {
        DBGEXIT0(TRUE);
        return(TRUE);
    }
    else
    {
        DBGEXIT0(FALSE);
        return(FALSE);
    }
}

/*******************************************************************************************/
/* NAME:  SKE_EnableMultiKeyPress                                                          */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function enables Multi-keypress feature of the Keypad encoder         */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SKE_EnableMultiKeyPress(void)
{
    DBGENTER0();
    HCL_SET_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPMLT_MASK);
    DBGEXIT0(0);
}

/*******************************************************************************************/
/* NAME:  SKE_DisableMultiKeyPress                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:  This function disables Multi-keypress feature of the Keypad encoder       */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SKE_DisableMultiKeyPress(void)
{
    DBGENTER0();
    HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPMLT_MASK);
    DBGEXIT0(0);
}

/*******************************************************************************************/
/* NAME:  SKE_IsMultiKeyPressEnabled                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:This function checks if Multi-key press is enabled in the Keypad Encoder    */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_bool  which can take any of following values -                                */
/*          TRUE - if Multi-key press is enabled                                           */
/*          FALSE - if Multi-key press is disabled                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_bool SKE_IsMultiKeyPressEnabled(void)
{
    DBGENTER0();
    if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPMLT_MASK))
    {
        DBGEXIT0(TRUE);
        return(TRUE);
    }
    else
    {
        DBGEXIT0(FALSE);
        return(FALSE);
    }
}

/*******************************************************************************************/
/* NAME:  SKE_SetAutoScanColumns                                                           */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function set columns to scan in Autoscan mode for the Keypad encoder  */
/* PARAMETERS:                                                                             */
/* IN:    scan_columns :Columns which needs Auto scanning                                  */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SKE_SetAutoScanColumns(IN t_ske_scan_column scan_columns)
{
    DBGENTER1("Autoscan Columns : %d", scan_columns);
    HCL_WRITE_BITS
    (
        g_ske_system_context.p_ske_register->ske_cr,
        (((t_uint32) scan_columns) << SKE_KPCN_SHIFT),
        SKE_KPCN_MASK
    );
    DBGEXIT0(0);
}

/*******************************************************************************************/
/* NAME:  SKE_GetAutoScanColumns                                                           */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function get columns which will be scanned in Autoscan mode           */
/*               for the Keypad encoder                                                    */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   p_scan_columns : Columns which will be scanned in Autoscan mode                  */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If output pointer variable is zero                    */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetAutoScanColumns(OUT t_ske_scan_column *p_scan_columns)
{
    DBGENTER1("Address of output variable(for scan columns) : %lx", p_scan_columns);
    if (0 != p_scan_columns)
    {
        *p_scan_columns = (t_ske_scan_column) ((HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPCN_MASK)) >> SKE_KPCN_SHIFT);
    }
    else
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT1(SKE_OK, "Scan columns returned :%d", *p_scan_columns);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_SetKeypadOutputDriveBulk                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function sets the column to drive and columns not to drive            */
/*              the keypad matrix for the Keypad encoder                                   */
/* PARAMETERS:                                                                             */
/* IN:    p_drive_column :Keypad column which needs to be driven and                       */
/*                        those which need not be driven                                   */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If input pointer variable is zero                     */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_SetKeypadOutputDriveBulk(IN t_ske_output_drive *p_drive_column)
{
    t_uint8     count;
    t_uint32    mask = 0;

    DBGENTER1("Address of input variable(for drive columns) : %lx", p_drive_column);
    if (0 == p_drive_column)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    for (count = 0; count < (t_uint8) SKE_KEYPAD_COLUMN_MAX; count++)
    {
        switch (count)
        {
            case 0:
                mask = SKE_KPOC0_MASK;
                break;

            case 1:
                mask = SKE_KPOC1_MASK;
                break;

            case 2:
                mask = SKE_KPOC2_MASK;
                break;

            case 3:
                mask = SKE_KPOC3_MASK;
                break;

            case 4:
                mask = SKE_KPOC4_MASK;
                break;

            case 5:
                mask = SKE_KPOC5_MASK;
                break;

            case 6:
                mask = SKE_KPOC6_MASK;
                break;

            case 7:
                mask = SKE_KPOC7_MASK;
                break;

            default:
                break;
        }

        if (TRUE == p_drive_column->keypad_drive_column[count])
        {
            HCL_SET_BITS(g_ske_system_context.p_ske_register->ske_cr, mask);
        }
        else
        {
            HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_cr, mask);
        }
    }

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_SetKeypadOutputDriveSingle                                                   */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function sets any one column to drive/not to drive the Keypad matrix  */
/*               for the Keypad encoder                                                    */
/* PARAMETERS:                                                                             */
/* IN:    drive_column :Keypad column which needs to be driven or not to be driven         */
/*        to_drive :to drive or not                                                        */
/*                  TRUE - set to drive , FALSE - set for HiZ(not to drive)                */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SKE_SetKeypadOutputDriveSingle(IN t_ske_keypad_column drive_column, IN t_bool to_drive)
{
    t_uint32    mask = 0;
    DBGENTER2("Column %d needs output drive state to be %d", drive_column, to_drive);
    switch (drive_column)
    {
        case SKE_KEYPAD_COLUMN_0:
            mask = SKE_KPOC0_MASK;
            break;

        case SKE_KEYPAD_COLUMN_1:
            mask = SKE_KPOC1_MASK;
            break;

        case SKE_KEYPAD_COLUMN_2:
            mask = SKE_KPOC2_MASK;
            break;

        case SKE_KEYPAD_COLUMN_3:
            mask = SKE_KPOC3_MASK;
            break;

        case SKE_KEYPAD_COLUMN_4:
            mask = SKE_KPOC4_MASK;
            break;

        case SKE_KEYPAD_COLUMN_5:
            mask = SKE_KPOC5_MASK;
            break;

        case SKE_KEYPAD_COLUMN_6:
            mask = SKE_KPOC6_MASK;
            break;

        case SKE_KEYPAD_COLUMN_7:
            mask = SKE_KPOC7_MASK;
            break;

        default:
            break;
    }

    if (TRUE == to_drive)
    {
        HCL_SET_BITS(g_ske_system_context.p_ske_register->ske_cr, mask);
    }
    else
    {
        HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_cr, mask);
    }

    DBGEXIT0(0);
}

/*******************************************************************************************/
/* NAME:  SKE_SetAllKeypadOutputDrive                                                  */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function sets the all columns to drive                                */
/*              the keypad matrix for the Keypad encoder                                   */
/* PARAMETERS:                                                                             */
/* IN: to drive or not to drive                                                            */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN:None                                                                             */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SKE_SetAllKeypadOutputDrive(IN t_bool to_drive)
{
    if (TRUE == to_drive)
    {
        HCL_SET_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPOC_MASK);
    }
    else
    {
        HCL_CLEAR_BITS(g_ske_system_context.p_ske_register->ske_cr, SKE_KPOC_MASK);
    }

}

/*******************************************************************************************/
/* NAME:  SKE_GetKeypadOutputDriveBulk                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets the output drive state for all columns                  */
/*              of the Keypad encoder                                                      */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   p_driven_column :Keypad column which is being driven and                         */
/*                         those which are not driven                                      */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If output pointer variable is zero                    */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetKeypadOutputDriveBulk(OUT t_ske_output_drive *p_driven_column)
{
    t_uint8     count;
    t_uint32    mask = 0;

    DBGENTER1("Address of output variable(for drive state of columns) : %lx", p_driven_column);
    if (0 == p_driven_column)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    //for (count = 0; count < (g_ske_system_context.ske_keypad_max_columns); count++)
    for (count = 0; count < SKE_KEYPAD_COLUMN_MAX; count++)
    {
        switch (count)
	        {
            case 0:
                mask = SKE_KPOC0_MASK;
                break;

            case 1:
                mask = SKE_KPOC1_MASK;
                break;

            case 2:
                mask = SKE_KPOC2_MASK;
                break;

            case 3:
                mask = SKE_KPOC3_MASK;
                break;

            case 4:
                mask = SKE_KPOC4_MASK;
                break;

            case 5:
                mask = SKE_KPOC5_MASK;
                break;

            case 6:
                mask = SKE_KPOC6_MASK;
                break;

            case 7:
                mask = SKE_KPOC7_MASK;
                break;

            default:
                break;
        }

        if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_cr, mask))
        {
            p_driven_column->keypad_drive_column[count] = TRUE;
        }
        else
        {
            p_driven_column->keypad_drive_column[count] = FALSE;
        }
    }

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_GetKeypadOutputDriveSingle                                                   */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets the drive state for any one column                      */
/*               of the Keypad encoder                                                     */
/* PARAMETERS:                                                                             */
/* IN:    Keypad column whose drive state is required                                      */
/* INOUT: None                                                                             */
/* OUT:   Drive state : TRUE - if the given column drives the keypad matrix,               */
/*                      FALSE - otherwise                                                  */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If output pointer variable is zero or                 */
/*                                   If wrong value for keypad column input                */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetKeypadOutputDriveSingle(IN t_ske_keypad_column drive_column, OUT t_bool *p_driven)
{
    t_uint32    mask = 0;

    DBGENTER2("Keypad Column :%d & Address of output variable(for drive state) : %lx", drive_column, p_driven);
    if (0 == p_driven)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    switch (drive_column)
    {
        case SKE_KEYPAD_COLUMN_0:
            mask = SKE_KPOC0_MASK;
            break;

        case SKE_KEYPAD_COLUMN_1:
            mask = SKE_KPOC1_MASK;
            break;

        case SKE_KEYPAD_COLUMN_2:
            mask = SKE_KPOC2_MASK;
            break;

        case SKE_KEYPAD_COLUMN_3:
            mask = SKE_KPOC3_MASK;
            break;

        case SKE_KEYPAD_COLUMN_4:
            mask = SKE_KPOC4_MASK;
            break;

        case SKE_KEYPAD_COLUMN_5:
            mask = SKE_KPOC5_MASK;
            break;

        case SKE_KEYPAD_COLUMN_6:
            mask = SKE_KPOC6_MASK;
            break;

        case SKE_KEYPAD_COLUMN_7:
            mask = SKE_KPOC7_MASK;
            break;

        default:
            DBGEXIT0(SKE_INVALID_PARAMETER);
            return(SKE_INVALID_PARAMETER);
    }

    if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_cr, mask))
    {
        *p_driven = TRUE;
    }
    else
    {
        *p_driven = FALSE;
    }

    DBGEXIT1(SKE_OK, "Drive state %d", *p_driven);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_GetAutoScanResult                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function gets Autoscan result for the column given as input           */
/*               for the Keypad encoder                                                    */
/* PARAMETERS:                                                                             */
/* IN:    Keypad column for which row values are needed                                    */
/* INOUT: None                                                                             */
/* OUT:   Result(Row values) for input column in Autoscan mode                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If wrong value for keypad column input  or            */
/*                                   If output pointer variable is zero                    */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_GetAutoScanResult(IN t_ske_keypad_column keypad_column, OUT t_ske_row_result *p_row_value)
{
    t_uint8 row_value, count;

    DBGENTER2("Keypad Column :%d & Address of output variable(for row result) : %lx", keypad_column, p_row_value);
    if (0 == p_row_value)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    switch (keypad_column)
    {
        case SKE_KEYPAD_COLUMN_0:
            row_value = (t_uint8) ((HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_asr0, SKE_KPRVAL0_MASK)) >> SKE_KPRVAL0_SHIFT);
            break;

        case SKE_KEYPAD_COLUMN_1:
            row_value = (t_uint8) ((HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_asr0, SKE_KPRVAL1_MASK)) >> SKE_KPRVAL1_SHIFT);
            break;

        case SKE_KEYPAD_COLUMN_2:
            row_value = (t_uint8) ((HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_asr1, SKE_KPRVAL2_MASK)) >> SKE_KPRVAL2_SHIFT);
            break;

        case SKE_KEYPAD_COLUMN_3:
            row_value = (t_uint8) ((HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_asr1, SKE_KPRVAL3_MASK)) >> SKE_KPRVAL3_SHIFT);
            break;

        case SKE_KEYPAD_COLUMN_4:
            row_value = (t_uint8) ((HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_asr2, SKE_KPRVAL4_MASK)) >> SKE_KPRVAL4_SHIFT);
            break;

        case SKE_KEYPAD_COLUMN_5:
            row_value = (t_uint8) ((HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_asr2, SKE_KPRVAL5_MASK)) >> SKE_KPRVAL5_SHIFT);
            break;

        case SKE_KEYPAD_COLUMN_6:
            row_value = (t_uint8) ((HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_asr3, SKE_KPRVAL6_MASK)) >> SKE_KPRVAL6_SHIFT);
            break;

        case SKE_KEYPAD_COLUMN_7:
            row_value = (t_uint8) ((HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_asr3, SKE_KPRVAL7_MASK)) >> SKE_KPRVAL7_SHIFT);
            break;

        default:
            DBGEXIT0(SKE_INVALID_PARAMETER);
            return(SKE_INVALID_PARAMETER);
    }

    for (count = 0; count < (g_ske_system_context.ske_keypad_max_rows); count++)
    {
        if (row_value & (MASK_BIT0 << count))
        {
            p_row_value->keypad_row_set[count] = TRUE;
        }
        else
        {
            p_row_value->keypad_row_set[count] = FALSE;
        }
    }

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_SetAutoScanResult                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function sets Autoscan result for the column given as input           */
/*               for the Keypad encoder                                                    */
/* PARAMETERS:                                                                             */
/* IN:    Keypad column for which row values need to be set & the row values               */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If input pointer variable is zero or                  */
/*                                   If wrong value for keypad column input                */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_SetAutoScanResult(IN t_ske_keypad_column keypad_column, IN t_ske_row_result *p_row_value)
{
    t_uint8 count, row_value = 0;

    DBGENTER2("Keypad Column :%d & Address of input variable(for row value) : %lx", keypad_column, p_row_value);
    if (0 == p_row_value)
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    for (count = 0; count < (g_ske_system_context.ske_keypad_max_rows); count++)
    {
        if (TRUE == p_row_value->keypad_row_set[count])
        {
            row_value |= (MASK_BIT0 << count);
        }
        else
        {
            row_value &= ~(MASK_BIT0 << count);
        }
    }

    switch (keypad_column)
    {
        case SKE_KEYPAD_COLUMN_0:
            HCL_WRITE_BITS
            (
                g_ske_system_context.p_ske_register->ske_asr0,
                (row_value << SKE_KPRVAL0_SHIFT),
                SKE_KPRVAL0_MASK
            );
            break;

        case SKE_KEYPAD_COLUMN_1:
            HCL_WRITE_BITS
            (
                g_ske_system_context.p_ske_register->ske_asr0,
                (row_value << SKE_KPRVAL1_SHIFT),
                SKE_KPRVAL1_MASK
            );
            break;

        case SKE_KEYPAD_COLUMN_2:
            HCL_WRITE_BITS
            (
                g_ske_system_context.p_ske_register->ske_asr1,
                (row_value << SKE_KPRVAL2_SHIFT),
                SKE_KPRVAL2_MASK
            );
            break;

        case SKE_KEYPAD_COLUMN_3:
            HCL_WRITE_BITS
            (
                g_ske_system_context.p_ske_register->ske_asr1,
                (row_value << SKE_KPRVAL3_SHIFT),
                SKE_KPRVAL3_MASK
            );
            break;

        case SKE_KEYPAD_COLUMN_4:
            HCL_WRITE_BITS
            (
                g_ske_system_context.p_ske_register->ske_asr2,
                (row_value << SKE_KPRVAL4_SHIFT),
                SKE_KPRVAL4_MASK
            );
            break;

        case SKE_KEYPAD_COLUMN_5:
            HCL_WRITE_BITS
            (
                g_ske_system_context.p_ske_register->ske_asr2,
                (row_value << SKE_KPRVAL5_SHIFT),
                SKE_KPRVAL5_MASK
            );
            break;

        case SKE_KEYPAD_COLUMN_6:
            HCL_WRITE_BITS
            (
                g_ske_system_context.p_ske_register->ske_asr3,
                (row_value << SKE_KPRVAL6_SHIFT),
                SKE_KPRVAL6_MASK
            );
            break;

        case SKE_KEYPAD_COLUMN_7:
            HCL_WRITE_BITS
            (
                g_ske_system_context.p_ske_register->ske_asr3,
                (row_value << SKE_KPRVAL7_SHIFT),
                SKE_KPRVAL7_MASK
            );
            break;

        default:
            DBGEXIT0(SKE_INVALID_PARAMETER);
            return(SKE_INVALID_PARAMETER);
    }

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_SetKeypadMatrixSize                                                          */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:This function saves keypad matrix size used (Max Column x Max Row)          */
/*             NOTE: This function need not be called for 8 X 8 Keypad Matrix and can be   */
/*                   called for Key Matrix of lesser order                                 */
/* PARAMETERS:                                                                             */
/* IN:    Maximum Columns & Rows used by the keypad matrix                                 */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_ske_error  which can take any of following values -                           */
/*            SKE_INVALID_PARAMETER: If input number rows/columns are greater than 8 or    */
/*                                   equal to 0                                            */
/*            SKE_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_ske_error SKE_SetKeypadMatrixSize(IN t_uint8 max_columns, IN t_uint8 max_rows)
{
    DBGENTER2("Max Column :%d & Max Row : %lx", max_columns, max_rows);
    if
    (
        ((t_uint8) SKE_KEYPAD_COLUMN_MAX < max_columns)
    ||  (0 == max_columns)
    ||  ((t_uint8) SKE_KEYPAD_ROW_MAX < max_rows)
    ||  (0 == max_rows)
    )
    {
        DBGEXIT0(SKE_INVALID_PARAMETER);
        return(SKE_INVALID_PARAMETER);
    }

    g_ske_system_context.ske_keypad_max_columns = max_columns;
    g_ske_system_context.ske_keypad_max_rows = max_rows;

    DBGEXIT0(SKE_OK);
    return(SKE_OK);
}

/*******************************************************************************************/
/* NAME:  SKE_SaveDeviceContext                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:    Saves the SKE register and global variables in memory                   */
/* PARAMETERS:                                                                             */
/* IN:                                                                                     */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN:                                                                                 */
/*                                                                                         */
/*                                                                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:    Re-entrant                                                               */

/*******************************************************************************************/
PUBLIC void SKE_SaveDeviceContext(void)
{
    g_ske_system_context.ske_device_context[0] = g_ske_system_context.p_ske_register->ske_cr;
    g_ske_system_context.ske_device_context[1] = g_ske_system_context.p_ske_register->ske_val0;
    g_ske_system_context.ske_device_context[2] = g_ske_system_context.p_ske_register->ske_val1;
    g_ske_system_context.ske_device_context[3] = g_ske_system_context.p_ske_register->ske_dbcr;
    g_ske_system_context.ske_device_context[4] = g_ske_system_context.p_ske_register->ske_imsc;
}

/*******************************************************************************************/
/* NAME:  SKE_RestoreDeviceContext                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:    Restore the SKE register and global variables in memory                 */
/* PARAMETERS:                                                                             */
/* IN:                                                                                     */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN:                                                                                 */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:     Re-entrant                                                              */

/*******************************************************************************************/
PUBLIC void SKE_RestoreDeviceContext(void)
{
    g_ske_system_context.p_ske_register->ske_cr = g_ske_system_context.ske_device_context[0];
    g_ske_system_context.p_ske_register->ske_val0 = g_ske_system_context.ske_device_context[1];
    g_ske_system_context.p_ske_register->ske_val1 = g_ske_system_context.ske_device_context[2];
    g_ske_system_context.p_ske_register->ske_dbcr = g_ske_system_context.ske_device_context[3];
    g_ske_system_context.p_ske_register->ske_imsc = g_ske_system_context.ske_device_context[4];
}

/*******************************************************************************************/
/* NAME:  SKE_SKE_IsSoftKeyPressed                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:This function checks if Soft Key is Pressed in the Keypad Encoder    */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_bool  which can take any of following values -                                */
/*          TRUE - if Soft Key is Pressed                                           */
/*          FALSE - if Soft Key is not Pressed                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_bool SKE_IsSoftKeyPressed(void)
{
    DBGENTER0();
    if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_ris, SKE_KPRISS_MASK))
    {
        DBGEXIT0(TRUE);
        return(TRUE);
    }
    else
    {
        DBGEXIT0(FALSE);
        return(FALSE);
    }
}

/*******************************************************************************************/
/* NAME:  SKE_SKE_IsAutoKeyPressed                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:This function checks if Auto Key is Pressed in the Keypad Encoder    */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_bool  which can take any of following values -                                */
/*          TRUE - if Auto Key is Pressed                                           */
/*          FALSE - if Auto Key is not Pressed                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_bool SKE_IsAutoKeyPressed(void)
{
    DBGENTER0();
    if (HCL_READ_BITS(g_ske_system_context.p_ske_register->ske_ris, SKE_KPRISA_MASK))
    {
        DBGEXIT0(TRUE);
        return(TRUE);
    }
    else
    {
        DBGEXIT0(FALSE);
        return(FALSE);
    }
}
