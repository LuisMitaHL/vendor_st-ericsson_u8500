/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This File contains HAL Routines for UART (U8500)
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "uart_hwp.h"


/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME my_debuglevel_uart
#define MY_DEBUG_ID             my_debug_id_uart


/*--------------------------------------------------------------------------*
 * Global Variables                                                         *
 *--------------------------------------------------------------------------*/
/* For debug HCL */
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = UART_HCL_DBG_ID;
#endif /* __DEBUG */


/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_uart_system_context   g_uart_system_context;



/*--------------------------------------------------------------------------*
 * Public functions                                                         *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME           :      UART_Init                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :   This routine initializes UART register base address.  */
/*                    It also checks for peripheral Ids and PCell Ids.      */
/*                                                                          */
/*  PARAMETERS    :                                                         */
/*          IN    :   uart_device_id        : UART to Identify              */
/*                :   uart_base_address     : UART registers base address   */
/*          OUT   :   none                                                  */
/*                                                                          */
/*       RETURN   :   UART_UNSUPPORTED_HW   : This is returned if peripheral*/
/*                                            ids don't match               */
/*                :   UART_INVALID_PARAMETER: This values is returned if    */
/*                                            base address passed is zero   */
/*                :   UART_OK               : Returns this if no error      */
/*                                            detected                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY         :     Non Re-entrant                                  */
/* REENTRANCY ISSUES  :                                                     */
/*            1)      Global variable p_uart_register(register base address)*/
/*                    is being modified                                     */
/****************************************************************************/
PUBLIC t_uart_error UART_Init(IN t_uart_device_id uart_device_id, IN t_logical_address uart_base_address)
{
    t_uart_error    error_status = UART_OK;
    DBGENTER2("UART number:  (%x), UART Base Address: (%x)", uart_device_id, uart_base_address);

    if (NULL != uart_base_address)
    {
        switch (uart_device_id)
        {
            case UART_DEVICE_ID_0:
                /* Initializing the registers structure
                    ---------------------------------------*/
                g_uart_system_context.p_uart0_register = (t_uart_register *) uart_base_address;

                /* Checking Peripheral Ids *
                     *-------------------------*/
                if(!UART0_CHECK_PERIPH_ID_OK)    
                {
                    error_status = UART_UNSUPPORTED_HW;
                }

                /* Initialise Globals */
    #ifdef __UART_ENHANCED
                g_uart_system_context.uart0_nrx = 0;
                g_uart_system_context.uart0_ntx = 0;
                g_uart_system_context.uart0_need_to_call_config = TRUE;
                g_uart_system_context.uart0_rx_status = (t_uart_receive_status) UART_RECEIVE_OK;
                g_uart_system_context.uart0_trigger_tx = 0;
                g_uart_system_context.uart0_trigger_rx = 0;
    #endif /* __UART_ENHANCED */
                break;

            case UART_DEVICE_ID_1:
                /* Initializing the registers structure
                    ---------------------------------------*/
                g_uart_system_context.p_uart1_register = (t_uart_register *) uart_base_address;

                /* Checking Peripheral Ids *
                     *-------------------------*/
                if(!UART1_CHECK_PERIPH_ID_OK)    
                {
                    error_status = UART_UNSUPPORTED_HW;
                }               

                /* Initialise Globals */
    #ifdef __UART_ENHANCED
                g_uart_system_context.uart1_nrx = 0;
                g_uart_system_context.uart1_ntx = 0;
                g_uart_system_context.uart1_need_to_call_config = TRUE;
                g_uart_system_context.uart1_rx_status = (t_uart_receive_status) UART_RECEIVE_OK;
                g_uart_system_context.uart1_trigger_tx = 0;
                g_uart_system_context.uart1_trigger_rx = 0;
    #endif /* __UART_ENHANCED */
                break;

            case UART_DEVICE_ID_2:
                /* Initializing the registers structure *
                     *--------------------------------------*/
                g_uart_system_context.p_uart2_register = (t_uart_register *) uart_base_address;

                /* Checking Peripheral Ids *
                     *-------------------------*/
                if(!UART2_CHECK_PERIPH_ID_OK)    
                {
                    error_status = UART_UNSUPPORTED_HW;
                }

                /* Initialise Globals */
    #ifdef __UART_ENHANCED
                g_uart_system_context.uart2_nrx = 0;
                g_uart_system_context.uart2_ntx = 0;
                g_uart_system_context.uart2_need_to_call_config = TRUE;
                g_uart_system_context.uart2_rx_status = (t_uart_receive_status) UART_RECEIVE_OK;
                g_uart_system_context.uart2_trigger_tx = 0;
                g_uart_system_context.uart2_trigger_rx = 0;
    #endif /* __UART_ENHANCED */
                break;
        
	    case UART_DEVICE_ID_INVALID:
	    default:
                DBGEXIT0(UART_INVALID_PARAMETER);
                return(UART_INVALID_PARAMETER);	         	
	}           /* end switch */
    }               /* end if */
    else
    {
        error_status = UART_INVALID_PARAMETER;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_ManageSendBreak                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine enables to "pause" UART's transmission     */
/*   ARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to access               */
/*              :   command         : Command to manage break               */
/*                                                                          */
/*      RETURN  :   UART_INVALID_PARAMETER  : UART number do not match      */
/*              :   UART_OK                 : Returned if no error detected */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_ManageSendBreak(IN t_uart_device_id uart_device_id, IN t_uart_set_or_clear command)
{
    t_uart_error    error_status = UART_OK;
    t_uart_register *lp_uart_register;
    DBGENTER2(" UART DEVICE No %d, Command (CLEAR/SET/NO_CHANGE) %d", uart_device_id, command);
    switch (uart_device_id) /* Choose suitable UART base adress  */
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    switch (command)
    {
        case UART_SET:
            lp_uart_register->uartx_lcrh_tx = (lp_uart_register->uartx_lcrh_tx | UART_LINEC_MASK_SB);
            break;

        case UART_CLEAR:
            lp_uart_register->uartx_lcrh_tx = (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_SB);
            break;

        case UART_NO_CHANGE:
            break;

        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_SetTestMode                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine sets UART's functional mode or test mode   */
/*                  (3 test modes)                                          */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to access               */
/*              :   test_mode       : Gives UARTx "test" mode               */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_SetTestMode(IN t_uart_device_id uart_device_id, IN t_uart_test_mode test_mode)
{
    t_uart_error    error_status = UART_OK;
    t_uint8         loop_back_enable = 0;
    t_uart_register *lp_uart_register;
    
    DBGENTER2(" UART DEVICE %d, TEST MODE (FUNC/LOOPBACK) %d", uart_device_id, test_mode);
    switch (uart_device_id) /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    switch (test_mode)
    {
        case UART_FUNC_MODE:
        {
            loop_back_enable = 0;
            break;
        }

        case UART_LOOP_BACK_MODE:
        {
            loop_back_enable = 1;
            break;
        }

        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    lp_uart_register->uartx_cr = (   (t_uint32) (lp_uart_register->uartx_cr &~UART_CONTROL_MASK_LOOPB) |
                                   ( (t_uint32) (loop_back_enable << UART_CONTROL_SHIFT_LOOPB) & (t_uint32) UART_CONTROL_MASK_LOOPB) );

/*#ifdef __UART_ENHANCED
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_UEN;         Enable Uart functionality: Fix for Bug 
#endif
  */  

    DBGEXIT0(error_status);
    return(error_status);
}

#ifdef __DEBUG              /* Debug APIs */
/****************************************************************************/
/* NAME           :    UART_SetDbgLevel                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine sets the debug level for the UART HCL   */
/*                                                                          */
/*  PARAMETERS    :                                                         */
/*          IN    :    debug_level :   debug level from DEBUG_LEVEL0 to     */
/*                                     DEBUG_LEVEL4                         */
/*          OUT   :    none                                                 */
/*                                                                          */
/*       RETURN   :    UART_OK     :   Returns this if no error detected    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/****************************************************************************/
PUBLIC t_uart_error UART_SetDbgLevel(IN t_dbg_level debug_level)
{
    DBGENTER1("Debug Level: (%x)", debug_level);
    MY_DEBUG_LEVEL_VAR_NAME = debug_level;
    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME           :    UART_GetDbgLevel                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine enables to choose between different dbg */
/*                     comments levels                                      */
/*  PARAMETERS    :                                                         */
/*          IN    :    none                                                 */
/*         OUT    :    p_debug_level   : Pointer to the current debug level */
/*      RETURN    :    UART_OK         : Returns this if no error detected  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/****************************************************************************/
PUBLIC t_uart_error UART_GetDbgLevel(OUT t_dbg_level *p_debug_level)
{
    DBGENTER1("Pointer to Debug Level: (%p)", p_debug_level);
    if (NULL == p_debug_level)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    *p_debug_level = MY_DEBUG_LEVEL_VAR_NAME;
    DBGEXIT0(UART_OK);
    return(UART_OK);
}
#endif /* __DEBUG */


/****************************************************************************/
/* NAME         :   UART_GetProductId                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine reads the Product ID, Integration Number(H)*/
/*                  , Revision Number(X.Y) and Provider ID of the UART IP   */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to access               */
/*         OUT  :   uart_ip_id      : UART IP details                       */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_GetProductId(IN t_uart_device_id  uart_device_id, OUT t_uart_ip_id *uart_ip_id)
{
    t_uart_error    error_status = UART_OK;
    t_uart_register *lp_uart_register;
    t_uint32 temp_reg;
    
    DBGENTER1(" UART Device ID (%d)", uart_device_id);

    if (NULL == uart_ip_id)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    switch (uart_device_id)                     /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            {
                lp_uart_register = g_uart_system_context.p_uart0_register;
                break;
            }

        case UART_DEVICE_ID_1:
            {
                lp_uart_register = g_uart_system_context.p_uart1_register;
                break;
            }

        case UART_DEVICE_ID_2:
            {
                lp_uart_register = g_uart_system_context.p_uart2_register;
                break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    /* Read IP IDs */
    temp_reg                        = lp_uart_register->uartx_id_product_h_xy;
    
    uart_ip_id->minor_rev_y         = (t_uint8)  ( (temp_reg & UART_PRODUCT_MASK_Y) >> UART_PRODUCT_SHIFT_Y);   
    uart_ip_id->sw_complaincy_rev_x = (t_uint8)  ( (temp_reg & UART_PRODUCT_MASK_X) >> UART_PRODUCT_SHIFT_X);   
    uart_ip_id->hw_rev_h            = (t_uint8)  ( (temp_reg & UART_PRODUCT_MASK_H) >> UART_PRODUCT_SHIFT_H);
    uart_ip_id->product_id          = (t_uint8)  ( (temp_reg & UART_PRODUCT_MASK_ID) >> UART_PRODUCT_SHIFT_ID);
    
    temp_reg                        = lp_uart_register->uartx_id_provider;
    
    uart_ip_id->provider_id         = (t_uint16) ( (temp_reg & UART_PROVIDER_MASK_ID) >> UART_PROVIDER_SHIFT_ID);   
      
    DBGEXIT0(error_status);
    return(error_status);
}


/****************************************************************************/
/* NAME         :   UART_DTEGetInputSignals                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   To get the Input modem signals status                   */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   uart_ri         : Ring Indicator                        */
/*              :   uart_dsr        : Data Set Ready                        */
/*              :   uart_dcd        : Data Carrier Detect                   */
/*              :   uart_cts        : Clear To Send                         */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_DTEGetInputSignals
(
    IN t_uart_device_id     uart_device_id,
    OUT t_uart_set_or_clear *p_ri,
    OUT t_uart_set_or_clear *p_dsr,
    OUT t_uart_set_or_clear *p_dcd,
    OUT t_uart_set_or_clear *p_cts
)
{
    DBGENTER1( "UART DEVICE %d", uart_device_id);

    if (NULL == p_ri || NULL == p_dsr || NULL == p_dcd || NULL == p_cts)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    *p_ri = UART_NO_CHANGE;
    *p_dsr = UART_NO_CHANGE;
    *p_dcd = UART_NO_CHANGE;

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            if (UART_DTE_MODE == g_uart_system_context.uart0_config.uart_mode)
            {
                *p_ri = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart0_register->uartx_fr & UART_FLAG_MASK_RI)) ? UART_SET : UART_CLEAR);
                *p_dsr = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart0_register->uartx_fr & UART_FLAG_MASK_DSR)) ? UART_SET : UART_CLEAR);
                *p_dcd = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart0_register->uartx_fr & UART_FLAG_MASK_DCD)) ? UART_SET : UART_CLEAR);
            }
            *p_cts = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart0_register->uartx_fr & UART_FLAG_MASK_CTS)) ? UART_SET : UART_CLEAR);
            break;

        case UART_DEVICE_ID_1:
            *p_cts = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart1_register->uartx_fr & UART_FLAG_MASK_CTS)) ? UART_SET : UART_CLEAR);
            break;

        case UART_DEVICE_ID_2:
            *p_cts = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart2_register->uartx_fr & UART_FLAG_MASK_CTS)) ? UART_SET : UART_CLEAR);
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGEXIT4(UART_OK, " RI %d, DSR %d, DCD %d, CTS %d", *p_ri, *p_dsr, *p_dcd, *p_cts);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   UART_DTESetOutputSignals                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   To set the Output modem signals of DTE                  */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   uart_dtr        : Data Terminal Ready                   */
/*              :   uart_rts        : Request To Send                       */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_DTESetOutputSignals
(
    IN t_uart_device_id     uart_device_id,
    IN t_uart_set_or_clear  uart_dtr,
    IN t_uart_set_or_clear  uart_rts
)
{
    DBGENTER3(" UART DEVICE %d, DTR (CLEAR/SET) %d,  RTS(CLEAR/SET) %d", uart_device_id, uart_dtr, uart_rts);

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            if (UART_DTE_MODE == g_uart_system_context.uart0_config.uart_mode)
            {
                switch (uart_dtr)
                {
                    case UART_SET:
                        g_uart_system_context.p_uart0_register->uartx_cr |= UART_CONTROL_MASK_DTR;
                        break;

                    case UART_CLEAR:
                        g_uart_system_context.p_uart0_register->uartx_cr &= ~UART_CONTROL_MASK_DTR;
                        break;

                    default:
                        break;
                }
            }
            switch (uart_rts)
            {
                case UART_SET:
                    g_uart_system_context.p_uart0_register->uartx_cr |= UART_CONTROL_MASK_RTS;
                    break;

                case UART_CLEAR:
                    g_uart_system_context.p_uart0_register->uartx_cr &= ~UART_CONTROL_MASK_RTS;
                    break;

                default:
                    break;
            }
            break;

        case UART_DEVICE_ID_1:
            switch (uart_rts)
            {
                case UART_SET:
                    g_uart_system_context.p_uart1_register->uartx_cr |= UART_CONTROL_MASK_RTS;
                    break;

                case UART_CLEAR:
                    g_uart_system_context.p_uart1_register->uartx_cr &= ~UART_CONTROL_MASK_RTS;
                    break;

                default:
                    break;
            }
            break;

        case UART_DEVICE_ID_2:
            switch (uart_rts)
            {
                case UART_SET:
                    g_uart_system_context.p_uart2_register->uartx_cr |= UART_CONTROL_MASK_RTS;
                    break;

                case UART_CLEAR:
                    g_uart_system_context.p_uart2_register->uartx_cr &= ~UART_CONTROL_MASK_RTS;
                    break;

                default:
                    break;
            }
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   UART_DCEGetInputSignals                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   To get the Input modem signals' status of DCE           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   uart_dtr        : Data Terminal Ready                   */
/*              :   uart_cts        : UART_CLEAR To Send                    */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_DCEGetInputSignals
(
    IN t_uart_device_id     uart_device_id,
    OUT t_uart_set_or_clear *p_dtr,
    OUT t_uart_set_or_clear *p_cts
)
{
    DBGENTER1("UART DEVICE %d", uart_device_id);
    if (NULL == p_dtr || NULL == p_cts)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    *p_dtr = UART_NO_CHANGE;

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            if (UART_DCE_MODE == g_uart_system_context.uart0_config.uart_mode)
            {
                *p_dtr = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart0_register->uartx_fr & UART_FLAG_MASK_DSR)) ? UART_SET : UART_CLEAR);
            }
            *p_cts = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart0_register->uartx_fr & UART_FLAG_MASK_CTS)) ? UART_SET : UART_CLEAR);
            break;

        case UART_DEVICE_ID_1:
            *p_cts = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart1_register->uartx_fr & UART_FLAG_MASK_CTS)) ? UART_SET : UART_CLEAR);
            break;


        case UART_DEVICE_ID_2:
            *p_cts = (t_uart_set_or_clear) ((0 != (g_uart_system_context.p_uart2_register->uartx_fr & UART_FLAG_MASK_CTS)) ? UART_SET : UART_CLEAR);
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGEXIT2(UART_OK, "DTR %d,  CTS %d", *p_dtr, *p_cts);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   UART_DCESetOutputSignals                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   To set the Output modem signals of DCE                  */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   uart_ri         : Ring Indicator                        */
/*              :   uart_dsr        : Data Set Ready                        */
/*              :   uart_dcd        : Data Carrier Detect                   */
/*              :   uart_rts        : Request To Send                       */
/*                                                                          */
/*     RETURN   :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_DCESetOutputSignals
(
    IN t_uart_device_id     uart_device_id,
    IN t_uart_set_or_clear  uart_ri,
    IN t_uart_set_or_clear  uart_dsr,
    IN t_uart_set_or_clear  uart_dcd,
    IN t_uart_set_or_clear  uart_rts
)
{
    DBGENTER5(" DEVICE ID %d, RI (CLEAR/SET) %d, DSR (CLEAR/SET) %d, DCD (CLEAR/SET) %d, RTS (CLEAR/SET) %d", uart_device_id, uart_ri, uart_dsr, uart_dcd, uart_rts);
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            if (UART_DCE_MODE == g_uart_system_context.uart0_config.uart_mode)
            {
                if (UART_SET == uart_ri)
                {
                    g_uart_system_context.p_uart0_register->uartx_cr |= UART_CONTROL_MASK_URI;
                }
                else if (UART_CLEAR == uart_ri)
                {
                    g_uart_system_context.p_uart0_register->uartx_cr &= ~UART_CONTROL_MASK_URI;
                }

                if (UART_SET == uart_dcd)
                {
                    g_uart_system_context.p_uart0_register->uartx_cr |= UART_CONTROL_MASK_DTR;
                }
                else if (UART_CLEAR == uart_dsr)
                {
                    g_uart_system_context.p_uart0_register->uartx_cr &= ~UART_CONTROL_MASK_DTR;
                }

                if (UART_SET == uart_dcd)
                {
                    g_uart_system_context.p_uart0_register->uartx_cr |= UART_CONTROL_MASK_UDCD;
                }
                else if (UART_CLEAR == uart_dcd)
                {
                    g_uart_system_context.p_uart0_register->uartx_cr &= ~UART_CONTROL_MASK_UDCD;
                }
            }
            if (UART_SET == uart_rts)
            {
                g_uart_system_context.p_uart0_register->uartx_cr |= UART_CONTROL_MASK_RTS;
            }
            else if (UART_CLEAR == uart_rts)
            {
                g_uart_system_context.p_uart0_register->uartx_cr &= ~UART_CONTROL_MASK_RTS;
            }
            break;

        case UART_DEVICE_ID_1:
            if (UART_SET == uart_rts)
            {
                g_uart_system_context.p_uart1_register->uartx_cr |= UART_CONTROL_MASK_RTS;
            }
            else if (UART_CLEAR == uart_rts)
            {
                g_uart_system_context.p_uart1_register->uartx_cr &= ~UART_CONTROL_MASK_RTS;
            }
            break;


        case UART_DEVICE_ID_2:
            if (UART_SET == uart_rts)
            {
                g_uart_system_context.p_uart2_register->uartx_cr |= UART_CONTROL_MASK_RTS;
            }
            else if (UART_CLEAR == uart_rts)
            {
                g_uart_system_context.p_uart2_register->uartx_cr &= ~UART_CONTROL_MASK_RTS;
            }
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME           :    UART_GetVersion                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    Gives the current version of the UART HCL            */
/*                                                                          */
/*  PARAMETERS    :                                                         */
/*          IN    :    None                                                 */
/*         OUT    :    p_version             : Structure which will consist */
/*                                             the version of current HCL   */
/*      RETURN    :    UART_INVALID_PARAMETER: Returned  if p_version       */
/*                                             is NULL                      */
/*                :    UART_OK               : Returns this if no error     */
/*                                             detected                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/****************************************************************************/
PUBLIC t_uart_error UART_GetVersion(OUT t_version *p_version)
{
    DBGENTER0();
    if (NULL != p_version)
    {
        p_version->version = UART_HCL_VERSION_ID;
        p_version->major = UART_HCL_MAJOR_ID;
        p_version->minor = UART_HCL_MINOR_ID;

        DBGEXIT0(UART_OK);
        return(UART_OK);
    }
    else
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }
}

/****************************************************************************/
/* NAME         :   UART_TransmitxCharacters                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to send x characters through UARTx  */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id      : Identify UART to configure        */
/*              :   num_of_char_to_be_tx: Number of characters to be        */
/*                                        transmitted                       */
/*              :   p_data_char         : Pointer on the first character to */
/*                                        transmit                          */
/*                                                                          */
/*      RETURN  :    t_uart_error        : UART error code                  */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_TransmitxCharacters
(
    IN t_uart_device_id uart_device_id,
    IN t_uint32         num_of_char_to_be_tx,
    IN t_uint8          *p_data_char
)
{
    return(DoTransmit(uart_device_id, num_of_char_to_be_tx, p_data_char));
}

/****************************************************************************/
/* NAME         :   UART_ReceivexCharacters                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to receive x characters through     */
/*                  UARTx                                                   */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id          : Identify UART to configure    */
/*              :   num_of_char_to_be_recd  : Number of characters to be    */
/*                                            received                      */
/*         OUT  :   p_data_char             : Pointer on the first buffer   */
/*                                            byte                          */
/*              :   p_num_of_char_received  : Number of characters really   */
/*                                            received                      */
/*              :   p_receive_error         : Error status related to       */
/*                                            receive data                  */
/*                                                                          */
/*      RETURN  :   t_uart_error            : UART error code               */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_ReceivexCharacters
(
    IN t_uart_device_id         uart_device_id,
    IN t_uint32                 num_of_char_to_be_recd,
    OUT t_uint8                 *p_data_char,
    OUT t_uint32                *p_num_of_char_received,
    OUT t_uart_receive_status   *p_receive_status
)
{
    return(DoReceive(uart_device_id, num_of_char_to_be_recd, p_data_char, p_num_of_char_received, p_receive_status));
}

/****************************************************************************/
/* NAME         :   UART_PowerOn                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This function is called by the upper layer to handle    */
/*                  the power on mode during power management by            */
/*                  reconfiguring the UARTs UART_IT should be called after  */
/*                  PowerOff API.                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : The UART number                       */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   UART_REQUEST_NOT_APPLICABLE : When UART_PowerOn API is  */
/*                                                called despite UART_IT is */
/*                                                in the power on mode      */
/*              :   UART_OK                     : Otherwise                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :   REENTRANT                                           */
/* REENTRANCY ISSUES:   No Issues                                           */
/*--------------------------------------------------------------------------*/
/****************************************************************************/
PUBLIC t_uart_error UART_PowerOn(IN t_uart_device_id uart_device_id)
{
    return(DoPowerOn(uart_device_id));
}

/****************************************************************************/
/* NAME         :   UART_PowerOff                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This function is called by the upper layer to handle    */
/*                  power off mode during power management.                 */
/*                  Three types of contexts needed to be stored             */
/*                  1) Transmission On Going - then stop the transmission   */
/*                  2) Reception On Going - then stop the reception         */
/*                  3) Autobaud On Going - then stop automatic format       */
/*                     extraction process                                   */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : The UART number                       */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   UART_REQUEST_NOT_APPLICABLE : When UART_PowerOff API is */
/*                                                called despite UART_IT is */
/*                                                in the power off mode.    */
/*              :   UART_OK                     : Otherwise                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :   NON-REENTRANT                                       */
/* REENTRANCY ISSUES:   This Function should be called in the CRITICAL      */
/*                      SECTION by the upper layer to maintain the          */
/*                      integrity of the context information stored.        */
/****************************************************************************/
PUBLIC t_uart_error UART_PowerOff(IN t_uart_device_id uart_device_id)
{
    return(DoPowerOff(uart_device_id));
}

/****************************************************************************/
/* NAME         :   uart_EnableFifo                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART FIFOs                      */
/*  PARAMETERS  :                                                           */
/*          IN  :   p_config  : Pointer to configuration Struct             */
/*                  lp_uart_register: UART register Set                     */
/*         OUT  :   none                                                    */
/*                                                                          */
/* RETURN       :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void  uart_EnableFifo(IN t_uart_config *p_config, IN t_uart_register *lp_uart_register)
{
        lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_ENFIFOS) |
            (
                (t_uint32) ((t_uint32) p_config->tx_enable_fifo << UART_LINEC_SHIFT_ENFIFOS) &
                    (t_uint32) UART_LINEC_MASK_ENFIFOS
            )
        );
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_ENFIFOS) |
            (
                (t_uint32) ((t_uint32) p_config->rx_enable_fifo << UART_LINEC_SHIFT_ENFIFOS) &
                    (t_uint32) UART_LINEC_MASK_ENFIFOS
            )
        ); 

    return;
}


/****************************************************************************/
/* NAME         :   uart_SetBaudRate                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART Baud Rates                 */
/*  PARAMETERS  :                                                           */
/*          IN  :   p_config  : Pointer to configuration Struct             */
/*                  lp_uart_register: UART register Set                     */
/*         OUT  :   none                                                    */
/*                                                                          */
/* RETURN       :   t_uart_error                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_SetBaudRate(IN t_uart_config *p_config, IN t_uart_register *lp_uart_register)
{
    t_uint32             baud_rate = (t_uint32) (p_config->baud_rate);
    t_uint32             baud = 0x00, integer_part=0x00;

    /*Added to support the baud rate above 2.5MBPS  */
    if(   (baud_rate == (t_uint32)UART_BR_2500000_BAUD) || (baud_rate == (t_uint32)UART_BR_3000000_BAUD)
       || (baud_rate == (t_uint32)UART_BR_3686400_BAUD) || (baud_rate == (t_uint32)UART_BR_4800000_BAUD)
      )
    {
        /*Set the Over sampling factor bit in control register  */
        lp_uart_register->uartx_cr |=UART_CONTROL_MASK_OVSFACT;
    }

    /* Configure baud_rate (MUST be performed before a LCRH write) */
    if(baud_rate > (t_uint32)UART_BR_2400000_BAUD)
    {
         baud = UART_BAUDVAL_OVSFACT1(baud_rate) ;
    }
    else
    {
         baud = UART_BAUDVAL_OVSFACT0(baud_rate);
    }

    integer_part =  UART_CONVERT_IBRD(baud);
    lp_uart_register->uartx_ibrd = integer_part;    /* Program baud_rate: integer part */

    lp_uart_register->uartx_fbrd = UART_CONVERT_FBRD(baud, integer_part); /* Program baud_rate: fractional part */ 
    
    DBGEXIT0(UART_OK);
    return(UART_OK);
}


/****************************************************************************/
/* NAME         :   uart_SetParity                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART Parity                     */
/*  PARAMETERS  :                                                           */
/*          IN  :   p_config  : Pointer to configuration Struct             */
/*                  lp_uart_register: UART register Set                     */
/*         OUT  :   none                                                    */
/*                                                                          */
/* RETURN       :   t_uart_error                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_SetParity(IN t_uart_config *p_config, IN t_uart_register *lp_uart_register)
{
    t_uint8             tx_parity_enable = 0x0;
    t_uint8             tx_stick_parity_select = 0x0;
    t_uint8             tx_even_parity_select = 0x0;
    t_uint8             rx_parity_enable = 0x0;
    t_uint8             rx_stick_parity_select = 0x0;
    t_uint8             rx_even_parity_select = 0x0;

    switch (p_config->tx_parity_bit)   /* Program parity */
    {
        case UART_NO_PARITY_BIT:
            {
                tx_parity_enable = 0x0;
                tx_stick_parity_select = 0x0;
                tx_even_parity_select = 0x0;
                break;
            }

        case UART_EVEN_PARITY_BIT:
            {
                tx_parity_enable = 0x1;
                tx_stick_parity_select = 0x0;
                tx_even_parity_select = 0x1;
                break;
            }

        case UART_ODD_PARITY_BIT:
            {
                tx_parity_enable = 0x1;
                tx_stick_parity_select = 0x0;
                tx_even_parity_select = 0x0;
                break;
            }

        case UART_STICK_PARITY_0_BIT:
            {
                tx_parity_enable = 0x1;
                tx_stick_parity_select = 0x1;
                tx_even_parity_select = 0x1;
                break;
            }

        case UART_STICK_PARITY_1_BIT:
            {
                tx_parity_enable = 0x1;
                tx_stick_parity_select = 0x1;
                tx_even_parity_select = 0x0;
                break;
            }

        default:
            DBGPRINT(DBGL_ERROR, "Tx Side Parity INVALID!");
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

   
    /* Set the parity for Rx side */
    switch (p_config->rx_parity_bit)   /* Program parity */
    {
        case UART_NO_PARITY_BIT:
            {
                rx_parity_enable = 0x0;
                rx_stick_parity_select = 0x0;
                rx_even_parity_select = 0x0;
                break;
            }

        case UART_EVEN_PARITY_BIT:
            {
                rx_parity_enable = 0x1;
                rx_stick_parity_select = 0x0;
                rx_even_parity_select = 0x1;
                break;
            }

        case UART_ODD_PARITY_BIT:
            {
                rx_parity_enable = 0x1;
                rx_stick_parity_select = 0x0;
                rx_even_parity_select = 0x0;
                break;
            }

        case UART_STICK_PARITY_0_BIT:
            {
                rx_parity_enable = 0x1;
                rx_stick_parity_select = 0x1;
                rx_even_parity_select = 0x1;
                break;
            }

        case UART_STICK_PARITY_1_BIT:
            {
                rx_parity_enable = 0x1;
                rx_stick_parity_select = 0x1;
                rx_even_parity_select = 0x0;
                break;
            }

        default:
            DBGPRINT(DBGL_ERROR, "Rx Side Parity INVALID!");
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }    

    /* Enable parity */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_PAR) |
            ((t_uint32) (tx_parity_enable << UART_LINEC_SHIFT_PAR) & (t_uint32) UART_LINEC_MASK_PAR)
        );

    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_PAR) |
            ((t_uint32) (rx_parity_enable << UART_LINEC_SHIFT_PAR) & (t_uint32) UART_LINEC_MASK_PAR)
        );

    

    /* Select parity bit information */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_EVEN) |
            ((t_uint32) (tx_even_parity_select << UART_LINEC_SHIFT_EVEN) & (t_uint32) UART_LINEC_MASK_EVEN)
        );

    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_EVEN) |
            ((t_uint32) (rx_even_parity_select << UART_LINEC_SHIFT_EVEN) & (t_uint32) UART_LINEC_MASK_EVEN)
        );

    
    /* Sticky Prity */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_SPAR) |
            ((t_uint32) (tx_stick_parity_select << UART_LINEC_SHIFT_SPAR) & (t_uint32) UART_LINEC_MASK_SPAR)
        );
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_SPAR) |
            ((t_uint32) (rx_stick_parity_select << UART_LINEC_SHIFT_SPAR) & (t_uint32) UART_LINEC_MASK_SPAR)
        );   

    DBGEXIT0(UART_OK);
    return(UART_OK); 

}

/****************************************************************************/
/* NAME         :   uart_SetFifoTrigger                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART Interrupt trigger for fifo */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   p_config        : Pointer to trigger structure          */
/*         OUT  :   none                                                    */
/*                                                                          */
/* RETURN       :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_SetFifoTrigger(IN t_uart_device_id uart_device_id, IN t_uart_config *p_config)
{
    t_uart_fifo_trigger  fifo_trigger = { 0, 0, 0, 0, 0, 0 };
    t_uart_dmawm_trigger uart_dmawm_trigger = {(t_uart_tx_trigger)0, (t_uart_rx_trigger)0}; 
    t_uart_error error_status = UART_OK;
    
    if ( (TRUE == p_config->tx_enable_fifo) || (TRUE == p_config->rx_enable_fifo) )
    {
	if(FALSE == p_config->tx_enable_fifo)
	{
            p_config->tx_trigger = UART_TX_TRIG_ONE_BYTE_EMPTY;
	}

        else if(FALSE == p_config->rx_enable_fifo)
	{
            p_config->rx_trigger = UART_RX_TRIG_ONE_BYTE_FULL;
	}	

        error_status = uart_ChooseFifoTriggerLevel_Nomadik
            (
                uart_device_id,
                p_config->tx_trigger,
                p_config->rx_trigger,
                &fifo_trigger
            );
        if (UART_OK != error_status)
        {
#ifdef __UART_ENHANCED
            switch (uart_device_id)
            {
                case UART_DEVICE_ID_0:
                    g_uart_system_context.uart0_need_to_call_config = TRUE;
                    break;

                case UART_DEVICE_ID_1:
                    g_uart_system_context.uart1_need_to_call_config = TRUE;
                    break;

                case UART_DEVICE_ID_2:
                    g_uart_system_context.uart2_need_to_call_config = TRUE;
                    break;
   
		case UART_DEVICE_ID_INVALID:    
                default:
                    DBGEXIT0(UART_INVALID_PARAMETER);
                    return(UART_INVALID_PARAMETER);
	    }
#endif	    
            DBGEXIT0(error_status);
            return(error_status);
        }

        error_status = uart_SetFifoTriggerLevel(uart_device_id, fifo_trigger);
	if (UART_OK != error_status)
        {
            DBGEXIT0(error_status);
            return(error_status);
        }

	uart_dmawm_trigger.uart_tx_trigger = p_config->tx_dmawm_trigger;
	uart_dmawm_trigger.uart_rx_trigger = p_config->rx_dmawm_trigger;
	error_status = UART_SetDmaWatermarkLevel(uart_device_id, uart_dmawm_trigger);
	if (UART_OK != error_status)
        {
            DBGEXIT0(error_status);
            return(error_status);
        }
    }
    
    DBGEXIT0(UART_OK);
    return(UART_OK);
}


/****************************************************************************/
/* NAME         :   uart_SetFlowControl                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART Flow Control               */
/*  PARAMETERS  :                                                           */
/*          IN  :   p_config  : Pointer to configuration Struct             */
/*                  lp_uart_register: UART register structure               */
/*         OUT  :   none                                                    */
/*                                                                          */
/* RETURN       :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_SetFlowControl(IN t_uart_config *p_config, IN t_uart_register *lp_uart_register)
{
    if (UART_HW_FLOW_CONTROL == p_config->flow_control) /* Program Flow Control: HW */
    {
        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_CTSFLOW;
        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_RTSFLOW;
    }
    else /* Reset CTSEN and RTSEN bits if either NO flow control or SW flow control */
    {
        lp_uart_register->uartx_cr &= (~(UART_CONTROL_MASK_CTSFLOW));
        lp_uart_register->uartx_cr &= (~(UART_CONTROL_MASK_RTSFLOW));
    }

    if (UART_SW_FLOW_CONTROL == p_config->flow_control)
    {
#if (defined(UART0_AUTO_SW_FLOW_CONTROL) || defined(UART1_AUTO_SW_FLOW_CONTROL) || defined(UART2_AUTO_SW_FLOW_CONTROL))
        /* Program Xon1, Xoff1 characters to be used in TX/RX software flow control */
        if ((p_config->sw_fc_mode & (t_uint32) UART_RX_SW_FC1) || (p_config->sw_fc_mode & (t_uint32) UART_TX_SW_FC1))
        {
            lp_uart_register->uartx_xon1 = p_config->xon1_char;
            lp_uart_register->uartx_xoff1 = p_config->xoff1_char;
        }   /* end if */

        /* Program Xon2, Xoff2 characters to be used in TX/RX software flow control */
        if ((p_config->sw_fc_mode & (t_uint32) UART_RX_SW_FC2) || (p_config->sw_fc_mode & (t_uint32) UART_TX_SW_FC2))
        {
            lp_uart_register->uartx_xon2 = p_config->xon2_char;
            lp_uart_register->uartx_xoff2 = p_config->xoff2_char;
        }   /* end if */

        /* Xon1, Xoff1 characters are used in receive software flow control */
        if (p_config->sw_fc_mode & (t_uint32) UART_RX_SW_FC1)
        {
            lp_uart_register->uartx_xfcr |= UART_XFCR_MASK_SFRMOD1;
        }   /* end if */

        /* Xon2, Xoff2 characters are used in receive software flow control */
        if (p_config->sw_fc_mode & (t_uint32) UART_RX_SW_FC2)
        {
            lp_uart_register->uartx_xfcr |= UART_XFCR_MASK_SFRMOD2;
        }   /* end if */

        /* Xon1, Xoff1 characters are used in transmit software flow control */
        if (p_config->sw_fc_mode & (t_uint32) UART_TX_SW_FC1)
        {
            lp_uart_register->uartx_xfcr |= UART_XFCR_MASK_SFTMOD1;
        }   /* end if */

        /* Xon2, Xoff2 characters are used in transmit software flow control */
        if (p_config->sw_fc_mode & (t_uint32) UART_TX_SW_FC2)
        {
            lp_uart_register->uartx_xfcr |= UART_XFCR_MASK_SFTMOD2;
        }   /* end if */

        /* Xon-any bit */
        if (p_config->sw_fc_mode & (t_uint32) UART_XON_ANY)
        {
            lp_uart_register->uartx_xfcr |= UART_XFCR_MASK_XON_ANY;
        }   /* end if */

        /* Special Character detection bit */
        if (p_config->sw_fc_mode & (t_uint32) UART_SPEC_CHAR)
        {
            lp_uart_register->uartx_xfcr |= UART_XFCR_MASK_SPEC_CHAR;
        }   /* end if */

        /* Software Flow control Enable */
        lp_uart_register->uartx_xfcr |= UART_XFCR_MASK_SFCEN;

        /* Transmit Enable */
        if ((p_config->sw_fc_mode & (t_uint32) UART_TX_SW_FC1) || (p_config->sw_fc_mode & (t_uint32) UART_TX_SW_FC2))
        {
            lp_uart_register->uartx_cr |= UART_CONTROL_MASK_TXEN;
        }   /* end if */

        /* Receive Enable */
        if ((p_config->sw_fc_mode & (t_uint32) UART_RX_SW_FC1) || (p_config->sw_fc_mode & (t_uint32) UART_RX_SW_FC2))
        {
            lp_uart_register->uartx_cr |= UART_CONTROL_MASK_RXEN;
        }   /* end if */
#endif /* end UART0-2_AUTO_SW_FLOW_CONTROL */
    }       /* end if UART_SW_FLOW_CONTROL */
    return;

}

/****************************************************************************/
/* NAME         :   UART_SetConfiguration                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART registers                  */
/*                  It also flush Tx and Rx FIFO to enable a new transfer   */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   p_config        : Pointer on global configuration       */
/*                                    structure                             */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error: UART error code                           */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_SetConfiguration(IN t_uart_device_id uart_device_id, IN t_uart_config *p_config)
{
    return(DoSetConfiguration(uart_device_id, p_config));
}

/****************************************************************************/
/* NAME         :   UART_SetTransferMode                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine set the transfer mode for the UART         */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   flags           : Flags                                 */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error: UART error code                           */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_SetTransferMode(IN t_uart_device_id uart_device_id, IN t_uint32 flags)
{
    return(DoSetTransferMode(uart_device_id, flags));
}

/****************************************************************************/
/* NAME         :   UART_ConfigureAutobaud                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to set/UART_CLEAR the autobaud     */
/*                  configuration                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   uart_status     : To set or UART_CLEAR autobaud config  */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_ConfigureAutoBaud(IN t_uart_device_id uart_device_id, IN t_uart_set_or_clear uart_status)
{
    return(DoConfigureAB(uart_device_id, uart_status));
}


/****************************************************************************/
/* NAME         :   uart_GetParity                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to extract the parity settings of  */
/*                  the UART from the parity register settings              */
/*  PARAMETERS  :                                                           */
/*          IN  :   lp_uart_register: UART Register Set (DeviceID)          */
/*         OUT  :   p_tx_parity_bit : Extracted Tx Side Parity Settings     */
/*              :   p_rx_parity_bit : Extracted Rx Side Parity Settings     */
/*                                                                          */
/*      RETURN  :   t_uart_error                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_GetParity(IN t_uart_register *lp_uart_register, OUT t_uart_parity_bit *p_tx_parity_bit, 
		                                                OUT t_uart_parity_bit *p_rx_parity_bit)
{
    if (0 != (lp_uart_register->uartx_lcrh_tx & UART_LINEC_MASK_PAR))
    {
        switch ((lp_uart_register->uartx_lcrh_tx & UART_LINEC_MASK_EVEN) >> UART_LINEC_SHIFT_EVEN)
        {
            case 0:
                *p_tx_parity_bit = UART_ODD_PARITY_BIT;
                break;

            case 1:
                *p_tx_parity_bit = UART_EVEN_PARITY_BIT;
                break;

            default:
                DBGEXIT0(UART_INVALID_PARAMETER);
                return(UART_INVALID_PARAMETER);
        }
    }
    else
    {
        *p_tx_parity_bit = UART_NO_PARITY_BIT;
    }

    if (0 != (lp_uart_register->uartx_lcrh_rx & UART_LINEC_MASK_PAR))
    {
        switch ((lp_uart_register->uartx_lcrh_rx & UART_LINEC_MASK_EVEN) >> UART_LINEC_SHIFT_EVEN)
        {
            case 0:
                *p_rx_parity_bit = UART_ODD_PARITY_BIT;
                break;

            case 1:
                *p_rx_parity_bit = UART_EVEN_PARITY_BIT;
                break;

            default:
                DBGEXIT0(UART_INVALID_PARAMETER);
                return(UART_INVALID_PARAMETER);
        }
    }
    else
    {
        *p_rx_parity_bit = UART_NO_PARITY_BIT;
    }

    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   UART_GetFormatDetails                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine gives status of stop_bits, data_bits,      */
/*                  parity bits, baud_rate and fifo enabled                 */
/* PARAMETERS   :                                                           */
/*         IN   :   uart_device_id      : Identify UART to configure        */
/*        OUT   :   p_tx_stop_bits      : Tx Stop bits information          */
/*              :   p_tx_data_bits      : Tx Data bits information          */
/*              :   p_tx_parity_bit     : Tx Parity bit information         */
/*              :   p_rx_stop_bits      : Rx Stop bits information          */
/*              :   p_rx_data_bits      : Rx Data bits information          */
/*              :   p_rx_parity_bit     : Rx Parity bit information         */
/*              :   p_baud_rate         : Baud_rate information             */
/*              :   p_tx_enable_fifo    : Tx Fifo Enabled information       */
/*              :   p_rx_enable_fifo    : Rx Fifo Enabled information       */
/*              :   p_is_autobaud_done  : Autobaud Done information         */
/*                                                                          */
/*    RETURN    :   t_uart_error        : UART error code                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_GetFormatDetails
(
    IN t_uart_device_id     uart_device_id,
    OUT t_uart_stop_bits    *p_tx_stop_bits,
    OUT t_uart_data_bits    *p_tx_data_bits,
    OUT t_uart_parity_bit   *p_tx_parity_bit,
    OUT t_uart_stop_bits    *p_rx_stop_bits,
    OUT t_uart_data_bits    *p_rx_data_bits,
    OUT t_uart_parity_bit   *p_rx_parity_bit,
    OUT t_uart_baud_rate    *p_baud_rate,
    OUT t_bool              *p_tx_enable_fifo,
    OUT t_bool              *p_rx_enable_fifo,
    OUT t_bool              *p_is_autobaud_done
)
{
    return
        (
            DoGetFormat
            (
                uart_device_id,
                p_tx_stop_bits,
                p_tx_data_bits,
                p_tx_parity_bit,
                p_rx_stop_bits,
                p_rx_data_bits,
                p_rx_parity_bit,
		p_baud_rate,
                p_tx_enable_fifo,
                p_rx_enable_fifo,
                p_is_autobaud_done
            )
        );
}

/****************************************************************************/
/* NAME         :   UART_Disable                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to disable UART                     */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to access               */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_Disable(IN t_uart_device_id uart_device_id)
{
    return(DoDisable(uart_device_id));
}

/****************************************************************************/
/* NAME         :   uart_SetFifoTriggerLevel                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART Interrupt trigger for fifo */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   fifo_trigger    : Pointer on global trigger structure   */
/*         OUT  :   none                                                    */
/*                                                                          */
/* RETURN       :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_SetFifoTriggerLevel(IN t_uart_device_id uart_device_id, IN t_uart_fifo_trigger fifo_trigger)
{
    t_uart_error    error_status = UART_OK;
    DBGENTER2("UART DEVICE %x, fifo trigger struct pointer %p", uart_device_id, (t_uint32 *)(&fifo_trigger) );
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            {
                g_uart_system_context.p_uart0_register->uartx_ifls =
                    (
                        (t_uint32) (g_uart_system_context.p_uart0_register->uartx_ifls &~(MASK_BIT0 | MASK_BIT1 | MASK_BIT2)) |
                        ((t_uint32) fifo_trigger.trig0_tx & (t_uint32) (MASK_BIT0 | MASK_BIT1 | MASK_BIT2))
                    );  /* Program tx trigger */
                g_uart_system_context.p_uart0_register->uartx_ifls =
                    (
                        (t_uint32) (g_uart_system_context.p_uart0_register->uartx_ifls &~(MASK_BIT3 | MASK_BIT4 | MASK_BIT5)) |
                        ((t_uint32) (fifo_trigger.trig0_rx << UART_FIFO_TRIGGER_RX_SHIFT) & (t_uint32) (MASK_BIT3 | MASK_BIT4 | MASK_BIT5))
                    );  /* Program rx trigger */
                break;
            }

        case UART_DEVICE_ID_1:
            {
                g_uart_system_context.p_uart1_register->uartx_ifls =
                    (
                        (t_uint32) (g_uart_system_context.p_uart1_register->uartx_ifls &~(MASK_BIT0 | MASK_BIT1 | MASK_BIT2)) |
                        ((t_uint32) fifo_trigger.trig1_tx & (t_uint32) (MASK_BIT0 | MASK_BIT1 | MASK_BIT2))
                    );  /* Program tx trigger */
                g_uart_system_context.p_uart1_register->uartx_ifls =
                    (
                        (t_uint32) (g_uart_system_context.p_uart1_register->uartx_ifls &~(MASK_BIT3 | MASK_BIT4 | MASK_BIT5)) |
                        ((t_uint32) (fifo_trigger.trig1_rx << UART_FIFO_TRIGGER_RX_SHIFT) & (t_uint32) (MASK_BIT3 | MASK_BIT4 | MASK_BIT5))
                    );  /* Program rx trigger */
                break;
            }


        case UART_DEVICE_ID_2:
            {
                g_uart_system_context.p_uart2_register->uartx_ifls =
                    (
                        (t_uint32) (g_uart_system_context.p_uart2_register->uartx_ifls &~(MASK_BIT0 | MASK_BIT1 | MASK_BIT2)) |
                        ((t_uint32) fifo_trigger.trig2_tx & (t_uint32) (MASK_BIT0 | MASK_BIT1 | MASK_BIT2))
                    );  /* Program tx trigger */
                g_uart_system_context.p_uart2_register->uartx_ifls =
                    (
                        (t_uint32) (g_uart_system_context.p_uart2_register->uartx_ifls &~(MASK_BIT3 | MASK_BIT4 | MASK_BIT5)) |
                        ((t_uint32) (fifo_trigger.trig2_rx << UART_FIFO_TRIGGER_RX_SHIFT) & (t_uint32) (MASK_BIT3 | MASK_BIT4 | MASK_BIT5))
                    );  /* Program rx trigger */
                break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            error_status = UART_INVALID_PARAMETER;
            break;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_SetDmaWatermarkLevel                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART Fifo Watermark Level for   */
/*                  DMA transfers                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id     : Identify UART to configure         */
/*              :   uart_dmawm_trigger : DMA Watermark trigger structure    */
/*         OUT  :   none                                                    */
/*                                                                          */
/* RETURN       :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_SetDmaWatermarkLevel(IN t_uart_device_id uart_device_id, IN t_uart_dmawm_trigger uart_dmawm_trigger)
{
    t_uart_error    error_status = UART_OK;
    DBGENTER2(" UART DEVICE %d, DMAWM_trigger pointer %x ", uart_device_id, (t_uint32) (&uart_dmawm_trigger));
    
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            {
                g_uart_system_context.p_uart0_register->uartx_dmawm =
                    (
                        (t_uint32) (g_uart_system_context.p_uart0_register->uartx_dmawm &~(MASK_BIT0 | MASK_BIT1 | MASK_BIT2)) |
                        ((t_uint32) (uart_dmawm_trigger.uart_tx_trigger) & (t_uint32) (MASK_BIT0 | MASK_BIT1 | MASK_BIT2))
                    );  /* Program tx trigger */
                g_uart_system_context.p_uart0_register->uartx_dmawm =
                    (
                        (t_uint32) (g_uart_system_context.p_uart0_register->uartx_dmawm &~(MASK_BIT3 | MASK_BIT4 | MASK_BIT5)) |
                        (( (t_uint32) (uart_dmawm_trigger.uart_rx_trigger) << UART_FIFO_TRIGGER_RX_SHIFT) & (t_uint32) (MASK_BIT3 | MASK_BIT4 | MASK_BIT5))
                    );  /* Program rx trigger */
          
	  	g_uart_system_context.uart0_config.tx_dmawm_trigger =  uart_dmawm_trigger.uart_tx_trigger; 
                g_uart_system_context.uart0_config.rx_dmawm_trigger =  uart_dmawm_trigger.uart_rx_trigger; 
                break;
            }

        case UART_DEVICE_ID_1:
            {
                g_uart_system_context.p_uart1_register->uartx_dmawm =
                    (
                        (t_uint32) (g_uart_system_context.p_uart1_register->uartx_dmawm &~(MASK_BIT0 | MASK_BIT1 | MASK_BIT2)) |
                        ((t_uint32) (uart_dmawm_trigger.uart_tx_trigger) & (t_uint32) (MASK_BIT0 | MASK_BIT1 | MASK_BIT2))
                    );  /* Program tx trigger */
                g_uart_system_context.p_uart1_register->uartx_dmawm =
                    (
                        (t_uint32) (g_uart_system_context.p_uart1_register->uartx_dmawm &~(MASK_BIT3 | MASK_BIT4 | MASK_BIT5)) |
                        (( (t_uint32) (uart_dmawm_trigger.uart_rx_trigger) << UART_FIFO_TRIGGER_RX_SHIFT) & (t_uint32) (MASK_BIT3 | MASK_BIT4 | MASK_BIT5))
                    );  /* Program rx trigger */
      
      		g_uart_system_context.uart1_config.tx_dmawm_trigger =  uart_dmawm_trigger.uart_tx_trigger; 
                g_uart_system_context.uart1_config.rx_dmawm_trigger =  uart_dmawm_trigger.uart_rx_trigger; 
		break;
            }


        case UART_DEVICE_ID_2:
            {
                g_uart_system_context.p_uart2_register->uartx_dmawm =
                    (
                        (t_uint32) (g_uart_system_context.p_uart2_register->uartx_dmawm &~(MASK_BIT0 | MASK_BIT1 | MASK_BIT2)) |
                        ((t_uint32) (uart_dmawm_trigger.uart_tx_trigger) & (t_uint32) (MASK_BIT0 | MASK_BIT1 | MASK_BIT2))
                    );  /* Program tx trigger */
                g_uart_system_context.p_uart2_register->uartx_dmawm =
                    (
                        (t_uint32) (g_uart_system_context.p_uart2_register->uartx_dmawm &~(MASK_BIT3 | MASK_BIT4 | MASK_BIT5)) |
                        (( (t_uint32) (uart_dmawm_trigger.uart_rx_trigger) << UART_FIFO_TRIGGER_RX_SHIFT) & (t_uint32) (MASK_BIT3 | MASK_BIT4 | MASK_BIT5))
                    );  /* Program rx trigger */
            
	    	g_uart_system_context.uart2_config.tx_dmawm_trigger =  uart_dmawm_trigger.uart_tx_trigger; 
                g_uart_system_context.uart2_config.rx_dmawm_trigger =  uart_dmawm_trigger.uart_rx_trigger; 
		break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            error_status = UART_INVALID_PARAMETER;
            break;
    }
    
    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_ReConfigureTx                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine re-configures UART Transmitter             */
/*                  independently of the receiver for parity, data, fifo    */
/*                  enable and stop-bits                                    */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : Identify UART to configure           */
/*              :   uart_txrx_config : Transmitter configuration structure  */
/*         OUT  :   none                                                    */
/*                                                                          */
/* RETURN       :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_ReConfigureTx(IN t_uart_device_id uart_device_id, IN t_uart_txrx_config uart_tx_config)
{
    t_uart_error    error_status           = UART_OK;
    t_uart_register *lp_uart_register;
    t_uart_config   *uart_config           = NULL;
    t_uint8         tx_parity_enable       = 0x0;
    t_uint8         tx_stick_parity_select = 0x0;
    t_uint8         tx_even_parity_select  = 0x0;    

    DBGENTER2(" UART DEVICE ID %d, TX_CONFIG STRUCT POINTER %x) ", uart_device_id, (t_uint32)(&uart_tx_config));
    
    /* Initialization phase */
    switch (uart_device_id)                                     /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
        {
            lp_uart_register = g_uart_system_context.p_uart0_register;
	    uart_config      = &(g_uart_system_context.uart0_config); 
            break;
        }

        case UART_DEVICE_ID_1:
        {
            lp_uart_register = g_uart_system_context.p_uart1_register;
	    uart_config      = &(g_uart_system_context.uart1_config); 
            break;
        }


        case UART_DEVICE_ID_2:
        {
            lp_uart_register = g_uart_system_context.p_uart2_register;
	    uart_config      = &(g_uart_system_context.uart2_config); 
            break;
        }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }
   
    /* Configure line control register */
    lp_uart_register->uartx_lcrh_tx = lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_SB;   /* Disable sendbreak */


    uart_config->tx_parity_bit = uart_tx_config.uart_parity_bits; 
    uart_config->tx_data_bits  = uart_tx_config.uart_data_bits; 
    uart_config->tx_stop_bits  = uart_tx_config.uart_stop_bits; 
    uart_config->tx_enable_fifo= uart_tx_config.uart_enable_fifo; 
    
    /* Enable FIFOs */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_ENFIFOS) |
            (
                (t_uint32) ((t_uint32) (uart_tx_config.uart_enable_fifo) << UART_LINEC_SHIFT_ENFIFOS) &
                    (t_uint32) UART_LINEC_MASK_ENFIFOS
            )
        );                      /* To enable Fifo's */

    

    /* Set data bits */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_WL) |
            ((t_uint32) ((t_uint32) (uart_tx_config.uart_data_bits) << UART_LINEC_SHIFT_WL) & (t_uint32) UART_LINEC_MASK_WL)
        );                      /* program data_bits number */

    
    /* Set stop bits */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_STOP) |
            ((t_uint32) ((t_uint32) (uart_tx_config.uart_stop_bits) << UART_LINEC_SHIFT_STOP) & (t_uint32) UART_LINEC_MASK_STOP)
        );                      /* program stop_bits number */

	

    /* Set the parity for Tx side */
    switch (uart_tx_config.uart_parity_bits)   /* Program parity */
    {
        case UART_NO_PARITY_BIT:
            {
                tx_parity_enable = 0x0;
                tx_stick_parity_select = 0x0;
                tx_even_parity_select = 0x0;
                break;
            }

        case UART_EVEN_PARITY_BIT:
            {
                tx_parity_enable = 0x1;
                tx_stick_parity_select = 0x0;
                tx_even_parity_select = 0x1;
                break;
            }

        case UART_ODD_PARITY_BIT:
            {
                tx_parity_enable = 0x1;
                tx_stick_parity_select = 0x0;
                tx_even_parity_select = 0x0;
                break;
            }

        case UART_STICK_PARITY_0_BIT:
            {
                tx_parity_enable = 0x1;
                tx_stick_parity_select = 0x1;
                tx_even_parity_select = 0x1;
                break;
            }

        case UART_STICK_PARITY_1_BIT:
            {
                tx_parity_enable = 0x1;
                tx_stick_parity_select = 0x1;
                tx_even_parity_select = 0x0;
                break;
            }

        default:
            DBGPRINT(DBGL_ERROR, "Invalid Parity Configuration: ");
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }


    /* Enable parity */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_PAR) |
            ((t_uint32) (tx_parity_enable << UART_LINEC_SHIFT_PAR) & (t_uint32) UART_LINEC_MASK_PAR)
        );

    

    /* Select parity bit information */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_EVEN) |
            ((t_uint32) (tx_even_parity_select << UART_LINEC_SHIFT_EVEN) & (t_uint32) UART_LINEC_MASK_EVEN)
        );


    
    /* Sticky Prity */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_SPAR) |
            ((t_uint32) (tx_stick_parity_select << UART_LINEC_SHIFT_SPAR) & (t_uint32) UART_LINEC_MASK_SPAR)
        );

    DBGEXIT0(error_status);
    return(error_status);
}


/****************************************************************************/
/* NAME         :   UART_ReConfigureRx                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine re-configures UART Reciever                */
/*                  independently of the transmitter for parity, data, fifo */
/*                  enable and stop-bits                                    */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : Identify UART to configure           */
/*              :   uart_txrx_config : Reciever configuration structure     */
/*         OUT  :   none                                                    */
/*                                                                          */
/* RETURN       :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_ReConfigureRx(IN t_uart_device_id uart_device_id, IN t_uart_txrx_config uart_rx_config)
{
    t_uart_error    error_status           = UART_OK;
    t_uart_register *lp_uart_register;
    t_uart_config   *uart_config           = NULL;
    t_uint8         rx_parity_enable       = 0x0;
    t_uint8         rx_stick_parity_select = 0x0;
    t_uint8         rx_even_parity_select  = 0x0;    

    DBGENTER2(" UART DEVICE ID %d, RX CONFIG STRUCT POINTER %x ", uart_device_id, (t_uint32)(&uart_rx_config));
    
    /* Initialization phase */
    switch (uart_device_id)                                     /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
        {
            lp_uart_register = g_uart_system_context.p_uart0_register;
	    uart_config      = &(g_uart_system_context.uart0_config); 
            break;
        }

        case UART_DEVICE_ID_1:
        {
            lp_uart_register = g_uart_system_context.p_uart1_register;
	    uart_config      = &(g_uart_system_context.uart1_config); 
            break;
        }


        case UART_DEVICE_ID_2:
        {
            lp_uart_register = g_uart_system_context.p_uart2_register;
	    uart_config      = &(g_uart_system_context.uart2_config); 
            break;
        }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }
   

    uart_config->rx_parity_bit = uart_rx_config.uart_parity_bits; 
    uart_config->rx_data_bits  = uart_rx_config.uart_data_bits; 
    uart_config->rx_stop_bits  = uart_rx_config.uart_stop_bits; 
    uart_config->rx_enable_fifo= uart_rx_config.uart_enable_fifo; 

    /* Enable FIFOs */
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_ENFIFOS) |
            (
                (t_uint32) ((t_uint32) (uart_rx_config.uart_enable_fifo) << UART_LINEC_SHIFT_ENFIFOS) &
                    (t_uint32) UART_LINEC_MASK_ENFIFOS
            )
        );                      /* To enable Fifo's */

    

    /* Set data bits */
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_WL) |
            ((t_uint32) ((t_uint32) (uart_rx_config.uart_data_bits) << UART_LINEC_SHIFT_WL) & (t_uint32) UART_LINEC_MASK_WL)
        );                      /* program data_bits number */

    
    /* Set stop bits */
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_STOP) |
            ((t_uint32) ((t_uint32) (uart_rx_config.uart_stop_bits) << UART_LINEC_SHIFT_STOP) & (t_uint32) UART_LINEC_MASK_STOP)
        );                      /* program stop_bits number */

	

    /* Set the parity for Rx side */
    switch (uart_rx_config.uart_parity_bits)   /* Program parity */
    {
        case UART_NO_PARITY_BIT:
            {
                rx_parity_enable = 0x0;
                rx_stick_parity_select = 0x0;
                rx_even_parity_select = 0x0;
                break;
            }

        case UART_EVEN_PARITY_BIT:
            {
                rx_parity_enable = 0x1;
                rx_stick_parity_select = 0x0;
                rx_even_parity_select = 0x1;
                break;
            }

        case UART_ODD_PARITY_BIT:
            {
                rx_parity_enable = 0x1;
                rx_stick_parity_select = 0x0;
                rx_even_parity_select = 0x0;
                break;
            }

        case UART_STICK_PARITY_0_BIT:
            {
                rx_parity_enable = 0x1;
                rx_stick_parity_select = 0x1;
                rx_even_parity_select = 0x1;
                break;
            }

        case UART_STICK_PARITY_1_BIT:
            {
                rx_parity_enable = 0x1;
                rx_stick_parity_select = 0x1;
                rx_even_parity_select = 0x0;
                break;
            }

        default:
            DBGPRINT(DBGL_ERROR, "Invalid Parity Configuration: ");
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }


    /* Enable parity */
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_PAR) |
            ((t_uint32) (rx_parity_enable << UART_LINEC_SHIFT_PAR) & (t_uint32) UART_LINEC_MASK_PAR)
        );

    

    /* Select parity bit information */
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_EVEN) |
            ((t_uint32) (rx_even_parity_select << UART_LINEC_SHIFT_EVEN) & (t_uint32) UART_LINEC_MASK_EVEN)
        );


    
    /* Sticky Prity */
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_SPAR) |
            ((t_uint32) (rx_stick_parity_select << UART_LINEC_SHIFT_SPAR) & (t_uint32) UART_LINEC_MASK_SPAR)
        );

    
    DBGEXIT0(error_status);
    return(error_status);
}


#ifdef __UART_ELEMENTARY    /* Elementary layer APIs */

/****************************************************************************/
/* NAME         :   UART_isTxfifoFull                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine gives the status on the Tx Fifo fullness   */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to access               */
/*                                                                          */
/*      RETURN  :   t_bool: TRUE    : If transmit FIFO is FULL              */
/*                        : FALSE   : If transmit FIFO is not FULL          */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_bool UART_isTxfifoFull(IN t_uart_device_id uart_device_id)
{
    t_uart_register *lp_uart_register;
    DBGENTER1(" UART DEVICE %d ", uart_device_id);

    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        lp_uart_register = g_uart_system_context.p_uart0_register;
    }

    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        lp_uart_register = g_uart_system_context.p_uart1_register;
    }
    else
    {
        lp_uart_register = g_uart_system_context.p_uart2_register;
    }

    DBGEXIT1
    (
        UART_OK,
        " TX FIFO FULL Status Bit=%x ",
        (((lp_uart_register->uartx_fr & UART_FLAG_MASK_TXF) >> UART_FLAG_SHIFT_TXF) & MASK_BIT0)
    );
    return((t_bool) ((lp_uart_register->uartx_fr & UART_FLAG_MASK_TXF) >> UART_FLAG_SHIFT_TXF));
}

/****************************************************************************/
/* NAME         :   UART_isTxfifoEmpty                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine gives the status on the Tx FIFO            */
/*                  emptinessness                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to access               */
/*      RETURN  :   t_bool:  TRUE   : If transmit FIFO is FULL              */
/*                        :  FALSE  : If transmit FIFO is not FULL          */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_bool UART_isTxfifoEmpty(IN t_uart_device_id uart_device_id)
{
    t_uart_register *lp_uart_register;
    DBGENTER1(" UART DEVICE %d ", uart_device_id);
    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        lp_uart_register = g_uart_system_context.p_uart0_register;
    }

    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        lp_uart_register = g_uart_system_context.p_uart1_register;
    }
    else
    {
        lp_uart_register = g_uart_system_context.p_uart2_register;
    }

    DBGEXIT1
    (
        UART_OK,
        "TX FIFO EMPTY BIT=%x ",
        (((lp_uart_register->uartx_fr & UART_FLAG_MASK_TXE) >> UART_FLAG_SHIFT_TXE) & MASK_BIT0)
    );
    return((t_bool) ((lp_uart_register->uartx_fr & UART_FLAG_MASK_TXE) >> UART_FLAG_SHIFT_TXE));
}

/****************************************************************************/
/* NAME         :   UART_isRxfifoEmpty                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine gives the status on the Rx FIFO            */
/*                  emptinessness                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id : Identify UART to access                */
/*      RETURN  :   TRUE           : If receive FIFO is FULL                */
/*              :   FALSE          : If receive FIFO is not FULL            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_bool UART_isRxfifoEmpty(IN t_uart_device_id uart_device_id)
{
    t_uart_register *lp_uart_register;
    DBGENTER1(" UART DEVICE %d ", uart_device_id);
    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        lp_uart_register = g_uart_system_context.p_uart0_register;
    }

    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        lp_uart_register = g_uart_system_context.p_uart1_register;
    }
    else
    {
        lp_uart_register = g_uart_system_context.p_uart2_register;
    }

    DBGEXIT1
    (
        UART_OK,
        " RX FIFO Empty Bit = %x ",
        (((lp_uart_register->uartx_fr & UART_FLAG_MASK_RXE) >> UART_FLAG_SHIFT_RXE) & MASK_BIT0)
    );
    return((t_bool) ((lp_uart_register->uartx_fr & UART_FLAG_MASK_RXE) >> UART_FLAG_SHIFT_RXE));
}

/****************************************************************************/
/* NAME         :   uart_elem_TransmitxCharacters                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to send x characters through UARTx  */
/*                  as an elementary routine.                               */
/*                                                                          */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id      : Identify UART to configure        */
/*              :   num_of_char_to_be_tx: Number of characters to be        */
/*                                        transmitted                       */
/*              :   p_data_char         : Pointer on the first character to */
/*                                        transmit                          */
/*                                                                          */
/*      RETURN  :   t_uart_error        : UART error code                   */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_elem_TransmitxCharacters
(
    IN t_uart_device_id uart_device_id,
    IN t_uint32         num_of_char_to_be_tx,
    IN t_uint8          *p_data_char
)
{
    t_uint32        count = 0;
    t_uart_error    error_status = UART_OK;
    t_uart_register *lp_uart_register;

    DBGENTER3(" UART DEVICE %d, NO OF CHARS TX %x, DATA POINTER %x)", uart_device_id, num_of_char_to_be_tx, (t_uint32) p_data_char);
    if (NULL == p_data_char)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    switch (uart_device_id)                 /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    while (count < num_of_char_to_be_tx)    /* Stops writing if transfer's end */
    {
        /* 1 byte writing */
        lp_uart_register->uartx_dr = (t_uint32) * (p_data_char + count);
        count++;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   uart_elem_ReceivexCharacters                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to receive x characters through     */
/*                  UARTx as a elementary routine.                          */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id          : Identify UART to configure    */
/*              :   num_of_char_to_be_recd  : Number of characters to be    */
/*                                            received                      */
/*         OUT  :   p_data_char             : Pointer on the first buffer   */
/*                                            byte                          */
/*              :   p_num_of_char_recd      : Number of characters really   */
/*                                            received                      */
/*              :   p_receive_error         : Error status related to       */
/*                                            receive data                  */
/*      RETURN  :   t_uart_error            : UART error code               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_elem_ReceivexCharacters
(
    IN t_uart_device_id         uart_device_id,
    IN t_uint32                 num_of_char_to_be_recd,
    OUT t_uint8                 *p_data_char,
    OUT t_uint32                *p_num_of_char_recd,
    OUT t_uart_receive_status   *p_receive_error
)
{
    t_uint32        count = 0;              /* Counts number of data read */
    t_uart_error    error_status = UART_OK;
    t_uint32        data_received;
    t_uart_register *lp_uart_register;

    DBGENTER3(" (UART Device ID : %d, Number of character to be received : %x, \
	       Pointer to the Data Buffer : %p)", uart_device_id, num_of_char_to_be_recd, p_data_char);

    if (NULL == p_data_char || NULL == p_num_of_char_recd || NULL == p_receive_error)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    /* Initialisation of receive status */
    *p_receive_error = (t_uart_receive_status) UART_RECEIVE_OK;

    switch (uart_device_id)                 /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    while
    (
        (count < num_of_char_to_be_recd)
    &&  ((t_uart_receive_status) UART_RECEIVE_OK == *p_receive_error)
    &&  !UART_isRxfifoEmpty(uart_device_id) /* Exit if Rx fifo empty in UART_IT mode (timeout UART_IT) */
    )
    {
        /* 1 byte reading */
        data_received = lp_uart_register->uartx_dr;

        *(p_data_char + count) = (t_uint8) data_received;

        *p_receive_error = (data_received >> SHIFT_BYTE1) & MASK_QUARTET;

        count++;
    }

    *p_num_of_char_recd = count;
    if ((t_uart_receive_status) UART_RECEIVE_OK != *p_receive_error)
    {
        DBGPRINT(DBGL_ERROR, "Error in Recieve!");
        error_status = UART_RECEIVE_ERROR;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_GetRxChar                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine receive one byte of data from Rx FIFO      */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to access               */
/*         OUT  :   p_data_char     : To store the received data            */
/*              :   p_receive_error : To store the receive error status     */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_GetRxChar
(
    IN t_uart_device_id         uart_device_id,
    OUT t_uint8                 *p_data_char,
    OUT t_uart_receive_status   *p_receive_error
)
{
    t_uart_error    error_status = UART_OK;
    t_uint32        data_received;
    t_uart_register *lp_uart_register;

    DBGENTER1(" UART DEVICE %d ", uart_device_id );
    if (NULL == p_data_char || NULL == p_receive_error)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    *p_receive_error = (t_uart_receive_status) UART_RECEIVE_OK;

    do
    {
    } while (TRUE == UART_ISRXFIFOEMPTY());

    /* 1 byte reading */
    data_received = lp_uart_register->uartx_dr;

    *p_receive_error = (data_received >> SHIFT_BYTE1) & MASK_QUARTET;

    *p_data_char = (t_uint8) data_received;

    if ((t_uart_receive_status) UART_RECEIVE_OK != *p_receive_error)
    {
        DBGPRINT(DBGL_ERROR, "Error in Recieve!");
        error_status = UART_RECEIVE_ERROR;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_SetTxChar                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine send one byte of data to Tx FIFO           */
/*                                                                          */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id : Identify UART to access                */
/*              :   char_to_send   : Character ot send                      */
/*                                                                          */
/*      RETURN  :   t_uart_error   : UART error code                        */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_SetTxChar(IN t_uart_device_id uart_device_id, IN t_uint8 char_to_send)
{
    t_uart_register *lp_uart_register;
    DBGENTER2(" UART DEVICE ID %d, CHARS TO SEND %x ", uart_device_id, char_to_send);
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    do
    {
    } while (TRUE == UART_ISTXFIFOFULL());

    lp_uart_register->uartx_dr = (t_uint32) char_to_send;
    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   uart_elem_PowerOn                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This function is called by the upper layer to handle    */
/*                  the power on mode during power management by            */
/*                  reconfiguring the UARTs. UART_IT should be called after */
/*                  PowerOff. It is an elementary routine.                  */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id : The uart number                        */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   UART_REQUEST_NOT_APPLICABLE : When uart_elem_PowerOn is */
/*                                                called despite UART_IT is */
/*                                                in the power on mode.     */
/*              :   UART_OK                     : Otherwise                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/****************************************************************************/
PRIVATE t_uart_error uart_elem_PowerOn(IN t_uart_device_id uart_device_id)
{
    t_uart_error    error_status;
    DBGENTER1(" UART DEVICE (%d) ", uart_device_id);
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            if (FALSE == g_uart_system_context.uart0_is_power_off)
            {
                DBGPRINT(DBGL_ERROR, "Already in Power-ON mode!");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart0_is_power_off = FALSE;
            error_status = uart_elem_SetConfiguration(uart_device_id, &g_uart_system_context.uart0_config);
            g_uart_system_context.p_uart0_register->uartx_imsc |= g_uart_system_context.uart0_device_context.uart_imsc_reg;
            break;

        case UART_DEVICE_ID_1:
            if (FALSE == g_uart_system_context.uart1_is_power_off)
            {
                DBGPRINT(DBGL_ERROR, "Already in Power-ON mode!");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart1_is_power_off = FALSE;
            error_status = uart_elem_SetConfiguration(uart_device_id, &g_uart_system_context.uart1_config);
            g_uart_system_context.p_uart1_register->uartx_imsc |= g_uart_system_context.uart1_device_context.uart_imsc_reg;
            break;

        case UART_DEVICE_ID_2:
            if (FALSE == g_uart_system_context.uart2_is_power_off)
            {
                DBGPRINT(DBGL_ERROR, "Already in Power-ON mode!");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart2_is_power_off = FALSE;
            error_status = uart_elem_SetConfiguration(uart_device_id, &g_uart_system_context.uart2_config);
            g_uart_system_context.p_uart2_register->uartx_imsc |= g_uart_system_context.uart2_device_context.uart_imsc_reg;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   uart_elem_PowerOff                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This function is called by the upper layer to handle    */
/*                  the power off mode during power management. It is an    */
/*                  elementary routine. context of uartx_imsc is saved.     */
/*                  It is an elementary routine.                            */
/*  PARAMETERS   :                                                          */
/*          IN   :  uart_device_id : The uart number                        */
/*         OUT   :  none                                                    */
/*                                                                          */
/*      RETURN   :  UART_REQUEST_NOT_APPLICABLE : When UART_PowerOff is     */
/*                                                called despite UART_IT is */
/*                                                in the power off mode.    */
/*               :  UART_OK                     : Otherwise                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :     Non Re-entrant                                    */
/* REENTRANCY ISSUES:                                                       */
/*              1)  This Function should be called in the CRITICAL SECTION  */
/*                  by the upper layer to maintain the integrity of the     */
/*                  context information stored.                             */
/****************************************************************************/
PRIVATE t_uart_error uart_elem_PowerOff(IN t_uart_device_id uart_device_id)
{
    t_uart_register *lp_uart_register;
    DBGENTER1(" UART DEVICE (%d) ", uart_device_id);

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            if (TRUE == g_uart_system_context.uart0_is_power_off)
            {
                DBGPRINT(DBGL_ERROR, "Already in Power-OFF mode!");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart0_is_power_off = TRUE;

            g_uart_system_context.uart0_device_context.uart_imsc_reg = lp_uart_register->uartx_imsc;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            if (TRUE == g_uart_system_context.uart1_is_power_off)
            {
                DBGPRINT(DBGL_ERROR, "Already in Power-OFF mode!");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart1_is_power_off = TRUE;

            g_uart_system_context.uart1_device_context.uart_imsc_reg = lp_uart_register->uartx_imsc;
            break;


        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            if (TRUE == g_uart_system_context.uart2_is_power_off)
            {
                DBGPRINT(DBGL_ERROR, "Already in Power-OFF mode!");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart2_is_power_off = TRUE;

            g_uart_system_context.uart2_device_context.uart_imsc_reg = lp_uart_register->uartx_imsc;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }
                
    DBGPRINT(DBGL_HCL_DEV, "Powering OFF UART NOW!");

    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_RTSFLOW;
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_CTSFLOW;
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_RTS;    /* Deassert the RTS signal */
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_UEN;    /* Disable Uart functionality */
    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   uart_elem_SetConfiguration                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART registers                  */
/*                  It also flush Tx and Rx fifo to enable a new transfer.  */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   p_config        : Pointer on global configuration struct*/
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_elem_SetConfiguration(IN t_uart_device_id uart_device_id, IN t_uart_config *p_config)
{
    t_uart_register     *lp_uart_register;
    t_uart_error        error_status = UART_OK;
    volatile t_uint32   data_flushed = 0;                       /* Variable to flush Rx fifo */

    DBGENTER2(" UART DEVICE %d, CONFIG STRUCT POINTER %x", uart_device_id, (t_uint32) p_config);

    if (NULL == p_config)
    {
        DBGPRINT(DBGL_ERROR, "Config Structure is NULL");
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    DBGPRINT(DBGL_HCL_DEV, "Resolving device IDs");
    
    /* Initialization phase */
    switch (uart_device_id)                                     /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
        {
            lp_uart_register = g_uart_system_context.p_uart0_register;
            g_uart_system_context.uart0_config = *p_config;
            g_uart_system_context.uart0_transfer_flags = UART_TX_IT_MODE | UART_RX_IT_MODE;
            break;
        }

        case UART_DEVICE_ID_1:
        {
            lp_uart_register = g_uart_system_context.p_uart1_register;
            g_uart_system_context.uart1_config = *p_config;
            g_uart_system_context.uart1_transfer_flags = UART_TX_IT_MODE | UART_RX_IT_MODE;
            break;
        }

        case UART_DEVICE_ID_2:
        {
            lp_uart_register = g_uart_system_context.p_uart2_register;
            g_uart_system_context.uart2_config = *p_config;
            g_uart_system_context.uart2_transfer_flags = UART_TX_IT_MODE | UART_RX_IT_MODE;
            break;
        }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }
   
    DBGPRINT(DBGL_HCL_DEV, "Resetting main registers!!");
    /* Reset main registers */

    UART_RESET_REG(lp_uart_register->uartx_lcrh_tx);
    UART_RESET_REG(lp_uart_register->uartx_lcrh_rx);
    UART_RESET_REG(lp_uart_register->uartx_imsc);
    UART_RESET_REG(lp_uart_register->uartx_cr);
    UART_RESET_REG(lp_uart_register->uartx_itcr);
    UART_RESET_REG(lp_uart_register->uartx_rsr_ecr);
    UART_RESET_REG(lp_uart_register->uartx_xfcr);
    UART_RESET_REG(lp_uart_register->uartx_xon1);
    UART_RESET_REG(lp_uart_register->uartx_xon2);
    UART_RESET_REG(lp_uart_register->uartx_xoff1);
    UART_RESET_REG(lp_uart_register->uartx_xoff2);
    UART_RESET_REG(lp_uart_register->uartx_dmacr);
    UART_RESET_REG(lp_uart_register->uartx_abimsc);
    lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);  

    /* Set the BAUD RATE */
    error_status = uart_SetBaudRate(p_config, lp_uart_register);
    if(UART_OK != error_status)
    {
        DBGEXIT0(error_status);
        return(error_status);
    }

    /* Configure line control register */
    lp_uart_register->uartx_lcrh_tx = lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_SB;   /* Disable sendbreak */

    DBGPRINT(DBGL_HCL_DEV, "Enable FIFOs");
    
    /* Enable FIFOs */
    uart_EnableFifo(p_config, lp_uart_register);

    DBGPRINT(DBGL_HCL_DEV, "Set DATA Bits");

    /* Set data bits */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_WL) |
            ((t_uint32) ((t_uint32) p_config->tx_data_bits << UART_LINEC_SHIFT_WL) & (t_uint32) UART_LINEC_MASK_WL)
        );                      /* program data_bits number */
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_WL) |
            ((t_uint32) ((t_uint32) p_config->rx_data_bits << UART_LINEC_SHIFT_WL) & (t_uint32) UART_LINEC_MASK_WL)
        );                      /* program data_bits number */

    
    DBGPRINT(DBGL_HCL_DEV, "Set Stop Bits");
    /* Set stop bits */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_STOP) |
            ((t_uint32) ((t_uint32) p_config->tx_stop_bits << UART_LINEC_SHIFT_STOP) & (t_uint32) UART_LINEC_MASK_STOP)
        );                      /* program stop_bits number */
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_STOP) |
            ((t_uint32) ((t_uint32) p_config->rx_stop_bits << UART_LINEC_SHIFT_STOP) & (t_uint32) UART_LINEC_MASK_STOP)
        );                      /* program stop_bits number */

	
    DBGPRINT(DBGL_HCL_DEV, "Set Parity");

    /* Configure Parity */    
    error_status = uart_SetParity(p_config, lp_uart_register);
    if(UART_OK != error_status)
    {
         DBGEXIT0(error_status);
	 return(error_status);
    }

    /* Configure FIFO Trigger levels */
    error_status = uart_SetFifoTrigger(uart_device_id, p_config);
    if(UART_OK != error_status)
    {
         DBGEXIT0(error_status);
	 return(error_status);
    }
    
    DBGPRINT(DBGL_HCL_DEV, "Program Flow Control!");
    uart_SetFlowControl(p_config, lp_uart_register);

    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_UEN;        /* Enable Uart functionality */

    DBGPRINT(DBGL_HCL_DEV, "Enable Tx & Rx!");
    /* Enable transmitter and/or receiver + Flush associated fifo */
    if (p_config->receive_enable)
    {
        /* Flush Rxfifo: Perform required Rx fifo reading to have Rx fifo empty */
        do
        {
            data_flushed = lp_uart_register->uartx_dr;
        } while (!UART_isRxfifoEmpty(uart_device_id));


        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_RXEN;   /* Enable RX */
    }

    if (p_config->transmit_enable)
    {
        if (!UART_isTxfifoEmpty(uart_device_id))
        {
            DBGPRINT(DBGL_ERROR, "Tx fifo was not empty");
            DBGEXIT0(UART_INTERNAL_ERROR);
            return(UART_INTERNAL_ERROR);
        }

        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_TXEN;   /* Enable TX */
    }

    /* All UART_IT except Tx INT are cleared */
    lp_uart_register->uartx_icr |= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);

    DBGEXIT0(error_status);
    return(error_status);
}


/****************************************************************************/
/* NAME         :   uart_elem_SetTxModeFlags_Tx                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to set the transfer mode flags of  */
/*                  the UART Transmitter                                    */
/*  PARAMETERS  :                                                           */
/*          IN  :   lp_uart_register: UART Register Set (DeviceID)          */
/*              :   flags           : Flags to be set                       */
/*              :   p_transfer_flags: Transfer Flags already set            */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_elem_SetTxModeFlags_Tx(IN t_uint32 flags, IN t_uart_register *lp_uart_register, 
		                                                      IN t_uint32 *p_transfer_flags)
{
    if (flags & UART_TX_IT_MODE)
    {
        if (*p_transfer_flags & UART_TX_DMA_MODE)
        {
            lp_uart_register->uartx_dmacr &= ~(MASK_BIT1);
            *p_transfer_flags &= ~UART_TX_DMA_MODE;
        }
        else if (*p_transfer_flags & UART_TX_POLL_MODE)
        {
            *p_transfer_flags &= ~UART_TX_POLL_MODE;
        }

        *p_transfer_flags |= UART_TX_IT_MODE;
    }
    else if (flags & UART_TX_DMA_MODE)
    {
        if ((*p_transfer_flags & (UART_TX_IT_MODE | UART_TX_POLL_MODE)))
        {
            *p_transfer_flags &= ~(UART_TX_IT_MODE | UART_TX_POLL_MODE);
            lp_uart_register->uartx_dmacr |= (MASK_BIT1 | MASK_BIT2);
        }

        *p_transfer_flags |= UART_TX_DMA_MODE;
    }
    else if (flags & UART_TX_POLL_MODE)
    {
        if (*p_transfer_flags & UART_TX_DMA_MODE)
        {
            lp_uart_register->uartx_dmacr &= ~(MASK_BIT1);
            *p_transfer_flags &= ~UART_TX_DMA_MODE;
        }
        else if ((*p_transfer_flags & UART_TX_IT_MODE))
        {
            *p_transfer_flags &= ~UART_TX_IT_MODE;
        }

        *p_transfer_flags |= UART_TX_POLL_MODE;
    }

}


/****************************************************************************/
/* NAME         :   uart_elem_SetTxModeFlags_Rx                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to set the transfer mode flags of  */
/*                  the UART Receiver                                       */
/*  PARAMETERS  :                                                           */
/*          IN  :   lp_uart_register: UART Register Set (DeviceID)          */
/*              :   flags           : Flags to be set                       */
/*              :   p_transfer_flags: Transfer Flags already set            */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_elem_SetTxModeFlags_Rx(IN t_uint32 flags, IN t_uart_register *lp_uart_register, 
		                                                       IN t_uint32 *p_transfer_flags)
{
    if (flags & UART_RX_IT_MODE)
    {
        if (*p_transfer_flags & UART_RX_DMA_MODE)
        {
            lp_uart_register->uartx_dmacr &= ~(MASK_BIT0);
            *p_transfer_flags &= ~UART_RX_DMA_MODE;
        }
        else if (*p_transfer_flags & UART_RX_POLL_MODE)
        {
            *p_transfer_flags &= ~UART_RX_POLL_MODE;
        }

        *p_transfer_flags |= UART_RX_IT_MODE;
    }
    else if (flags & UART_RX_DMA_MODE)
    {
        if ((*p_transfer_flags & (UART_RX_IT_MODE | UART_RX_POLL_MODE)))
        {
            *p_transfer_flags &= ~(UART_RX_IT_MODE | UART_RX_POLL_MODE);
            lp_uart_register->uartx_dmacr |= (MASK_BIT0 | MASK_BIT2);
        }

        *p_transfer_flags |= UART_RX_DMA_MODE;
    }
    else if (flags & UART_RX_POLL_MODE)
    {
        if (*p_transfer_flags & UART_RX_DMA_MODE)
        {
            lp_uart_register->uartx_dmacr &= ~(MASK_BIT0);
            *p_transfer_flags &= ~UART_RX_DMA_MODE;
        }
        else if ((*p_transfer_flags & UART_RX_IT_MODE))
        {
            *p_transfer_flags &= ~UART_RX_IT_MODE;
        }

        *p_transfer_flags |= UART_RX_POLL_MODE;
    }
}

/****************************************************************************/
/* NAME         :   uart_elem_SetTransferMode                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine set the transfer mode for the UART.        */
/*                  This is an elementary API                               */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id : Identify UART to configure             */
/*              :   flags          : Flags                                  */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error: UART error code                           */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_elem_SetTransferMode(IN t_uart_device_id uart_device_id, IN t_uint32 flags)
{
    t_uart_register *lp_uart_register;
    t_uint32        *p_transfer_flags;

    DBGENTER2("UART DEVICE %x, flags %x", uart_device_id, flags);
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            p_transfer_flags = &g_uart_system_context.uart0_transfer_flags;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            p_transfer_flags = &g_uart_system_context.uart1_transfer_flags;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            p_transfer_flags = &g_uart_system_context.uart2_transfer_flags;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGPRINT(DBGL_HCL_DEV, "Setting Tx Side!");
    uart_elem_SetTxModeFlags_Tx(flags, lp_uart_register, p_transfer_flags);

    DBGPRINT(DBGL_HCL_DEV, "Setting Recieve Side");
    uart_elem_SetTxModeFlags_Rx(flags, lp_uart_register, p_transfer_flags);

    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   uart_elem_ConfigureAutobaud                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to set/UART_CLEAR the autobaud     */
/*                  configuration                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   uart_action     : To set or UART_CLEAR autobaud config  */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_elem_ConfigureAutoBaud(IN t_uart_device_id uart_device_id, IN t_uart_set_or_clear uart_action)
{
    t_uart_error    error_status = UART_OK;
    t_uart_register *lp_uart_register;
    DBGENTER2("UART DEVICE %d, CLEAR/SET %d", uart_device_id, uart_action);

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

 	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    switch (uart_action)
    {
        case UART_SET:
            /* Disable UART */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_UEN;

            /* UART_CLEAR & Enable all the Autobaud Interrupts */
            lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
            lp_uart_register->uartx_abimsc |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);

            lp_uart_register->uartx_abcr |= (MASK_BIT0 | MASK_BIT1 | MASK_BIT2);
            break;

        case UART_CLEAR:
            /* UART_CLEAR & Disable all the Autobaud Interrupts */
            lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
            UART_RESET_REG(lp_uart_register->uartx_abimsc);

            lp_uart_register->uartx_abcr = MASK_NULL16;
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_UEN;
            break;

        case UART_NO_CHANGE:
            error_status = UART_OK;
            break;

        default:
            error_status = UART_INVALID_PARAMETER;
            break;
    }

    DBGEXIT0(error_status);
    return(error_status);
}
 

/****************************************************************************/
/* NAME         :   uart_elem_GetFormatDetails                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine gives status of stop_bits, data_bits,      */
/*                  parity bits, baud_rate and fifo enabled                 */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   p_tx_stop_bits  : Tx Stop bits information              */
/*              :   p_tx_data_bits  : Tx Data bits information              */
/*              :   p_tx_parity_bit : Tx Parity bit information             */
/*              :   p_rx_stop_bits  : Rx Stop bits information              */
/*              :   p_rx_data_bits  : Rx Data bits information              */
/*              :   p_rx_parity_bit : Rx Parity bit information             */
/*              :   p_baudrate      : Baud_rate information                 */
/*              :   t_bool          : Fifo Enabled information(Tx/Rx)       */
/*              :   t_bool          : Autobaud Done information             */
/*                                                                          */
/*     RETURN   :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_elem_GetFormatDetails
(
    IN t_uart_device_id     uart_device_id,
    OUT t_uart_stop_bits    *p_tx_stop_bits,
    OUT t_uart_data_bits    *p_tx_data_bits,
    OUT t_uart_parity_bit   *p_tx_parity_bit,
    OUT t_uart_stop_bits    *p_rx_stop_bits,
    OUT t_uart_data_bits    *p_rx_data_bits,
    OUT t_uart_parity_bit   *p_rx_parity_bit,
    OUT t_uart_baud_rate    *p_baud_rate,
    OUT t_bool              *p_tx_fifo_enabled,
    OUT t_bool              *p_rx_fifo_enabled,
    OUT t_bool              *p_is_autobaud_done
)
{
    t_uart_error    error_status = UART_OK;
    t_uart_register *lp_uart_register;
    
    DBGENTER1("UART DEVICE (%d )", uart_device_id);

    if
    (
        NULL == p_tx_stop_bits
    ||  NULL == p_tx_data_bits
    ||  NULL == p_tx_parity_bit
    ||  NULL == p_rx_stop_bits
    ||  NULL == p_rx_data_bits
    ||  NULL == p_rx_parity_bit
    ||  NULL == p_baud_rate
    ||  NULL == p_tx_fifo_enabled
    ||  NULL == p_rx_fifo_enabled
    ||  NULL == p_is_autobaud_done
    )
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            *p_baud_rate     = g_uart_system_context.uart0_config.baud_rate; 
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            *p_baud_rate     = g_uart_system_context.uart1_config.baud_rate; 
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            *p_baud_rate     = g_uart_system_context.uart2_config.baud_rate; 
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }
    
    *p_is_autobaud_done = (t_bool) ((0 != (lp_uart_register->uartx_absr & MASK_BIT12)) ? TRUE : FALSE);

    error_status = uart_GetParity(lp_uart_register, p_tx_parity_bit, p_rx_parity_bit);   
    if(UART_OK != error_status)
    {
        DBGEXIT0(error_status);
        return(error_status);
    }    
    
    *p_tx_data_bits = (t_uart_data_bits) ((lp_uart_register->uartx_lcrh_tx & UART_LINEC_MASK_WL) >> UART_LINEC_SHIFT_WL);
    *p_rx_data_bits = (t_uart_data_bits) ((lp_uart_register->uartx_lcrh_rx & UART_LINEC_MASK_WL) >> UART_LINEC_SHIFT_WL);

    *p_tx_stop_bits = (t_uart_stop_bits) ((lp_uart_register->uartx_lcrh_tx & UART_LINEC_MASK_STOP) >> UART_LINEC_SHIFT_STOP);
    *p_rx_stop_bits = (t_uart_stop_bits) ((lp_uart_register->uartx_lcrh_rx & UART_LINEC_MASK_STOP) >> UART_LINEC_SHIFT_STOP);

    *p_tx_fifo_enabled = (t_bool) ((lp_uart_register->uartx_lcrh_tx & UART_LINEC_MASK_ENFIFOS) >> UART_LINEC_SHIFT_ENFIFOS);
    *p_rx_fifo_enabled = (t_bool) ((lp_uart_register->uartx_lcrh_rx & UART_LINEC_MASK_ENFIFOS) >> UART_LINEC_SHIFT_ENFIFOS);

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   uart_elem_Disable                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to disable UART                     */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to access               */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_elem_Disable(IN t_uart_device_id uart_device_id)
{
    t_uart_error    error_status = UART_OK;
    t_uart_register *lp_uart_register;
    DBGENTER1(" UART DEVICE (%d)", uart_device_id);
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            {
                lp_uart_register = g_uart_system_context.p_uart0_register;
                break;
            }

        case UART_DEVICE_ID_1:
            {
                lp_uart_register = g_uart_system_context.p_uart1_register;
                break;
            }


        case UART_DEVICE_ID_2:
            {
                lp_uart_register = g_uart_system_context.p_uart2_register;
                break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~(UART_CONTROL_MASK_RTSFLOW | UART_CONTROL_MASK_CTSFLOW | UART_CONTROL_MASK_RTS);

    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_UEN;    /* Disable UART functionality  */
    DBGEXIT0(error_status);
    return(error_status);
}
#endif /* __UART_ELEMENTARY */

#ifdef __UART_ENHANCED

/****************************************************************************/
/* NAME           :    UART_GetIRQSrcStatus                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This function updates the t_uart_irq_status structure*/
/*                     according to t_uart_irq_src parameter.               */
/*  PARAMETERS    :                                                         */
/*          IN    :    irq_src : Id of the interrupt(s) for which p_status  */
/*                               structure is to be updated.                */
/*         OUT    :    p_status: Pointer to t_uart_irq_status structure.    */
/*                                                                          */
/*      RETURN    :    void                                                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/*--------------------------------------------------------------------------*/
/* COMMENTS     :   The return type for this API is void so an error code   */
/*                  can't return UART_INVALID_PARAMETER when the pointer    */
/*                  passed is NULL                                          */
/****************************************************************************/
PUBLIC void UART_GetIRQSrcStatus(IN t_uart_irq_src irq_src, OUT t_uart_irq_status *p_status)
{
    t_uint32        irq_mask;
    t_uart_register *lp_uart_register;

    DBGENTER3
    (
        "(Interrput Request: %x, interrupt State: %x, Pending interrupt: %x)",
        irq_src,
        p_status->irq_state,
        p_status->pending_irq
    );

    if (NULL == p_status)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return;
    }   /* end if p_status */

    switch (UART_GETDEVICE(irq_src))
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
	    break;

	case UART_DEVICE_ID_INVALID:
	default:
	    DBGEXIT0(UART_INVALID_PARAMETER);
	    return;
	    
    }

    irq_mask = (lp_uart_register->uartx_mis & UART_IRQ_SRC_ALL_IT);
    irq_mask |=
            (
                (lp_uart_register->uartx_abmis & (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE)) <<
                UART_AUTOBAUD_INTR_SHIFT
            );

    irq_mask = irq_mask & irq_src;
    irq_mask |= (((t_uint32) UART_GETDEVICE(irq_src)) << UART_ID_SHIFT);

    p_status->initial_irq = irq_mask;
    p_status->irq_state = UART_IRQ_STATE_NEW;
    p_status->pending_irq = p_status->initial_irq;
    DBGEXIT2(UART_OK, " irq src=%x,  p_status->pending_irq = %x", irq_src, p_status->pending_irq);
    return;
}

/****************************************************************************/
/* NAME           :    UART_IsIRQSrcActive                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This function updates the t_uart_irq_status structure*/
/*                     according to t_uart_irq_src parameter, if relevant   */
/*                     interrupt is active.                                 */
/*                                                                          */
/*  PARAMETERS    :                                                         */
/*          IN    :    irq_src :    Id of the interrupt(s) for which the    */
/*                                  p_status structure is to be updated.    */
/*         OUT    :    p_status:    pointer to t_uart_irq_status structure. */
/*                                                                          */
/*      RETURN    :    TRUE    :   Returned when the requested interrupt    */
/*                                 source is active                         */
/*                :    FALSE   :   Returned when the requested interrupt    */
/*                              source is not active                        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/*--------------------------------------------------------------------------*/
/* COMMENTS     :   The return type for this API is t_bool so an error code */
/*                  can't return UART_INVALID_PARAMETER when the pointer    */
/*                  passed is NULL                                          */
/****************************************************************************/
PUBLIC t_bool UART_IsIRQSrcActive(IN t_uart_irq_src irq_src, OUT t_uart_irq_status *p_status)
{
    t_uint32        irq_mask;
    t_uart_register *lp_uart_register;

    DBGENTER2("(Interrupt Request: %x, Pointer to Interrupt status: %p)", irq_src, (void *) p_status);

    /* No NULL Checking required */
    switch (UART_GETDEVICE(irq_src))
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
	    break;
	    
	case UART_DEVICE_ID_INVALID:
	default:
	     DBGEXIT0(UART_INVALID_PARAMETER);
             return(FALSE);

    }

    irq_mask = (lp_uart_register->uartx_mis & UART_IRQ_SRC_ALL_IT);
    irq_mask |=
            (
                (lp_uart_register->uartx_abmis & (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE)) <<
                UART_AUTOBAUD_INTR_SHIFT
            );

    irq_mask = irq_mask & irq_src;

    if (0 != irq_mask)
    {
        if (NULL != p_status)
        {
            p_status->initial_irq = irq_mask | (((t_uint32) UART_GETDEVICE(irq_src)) << UART_ID_SHIFT);
            p_status->pending_irq = p_status->initial_irq;
            p_status->irq_state = UART_IRQ_STATE_NEW;
        }

        DBGEXIT3(UART_OK, "IRQ ACTIVE STATUS %s, irq_src %x, p_status %x", "TRUE", irq_src, (t_uint32) p_status);
        return(TRUE);
    }

    DBGEXIT3(UART_OK, "IRQ ACTIVE STATUS %s, irq_src %x, p_status %x", "FALSE", irq_src, (t_uint32) p_status);
    return(FALSE);
}

/****************************************************************************/
/* NAME           :    UART_ProcessIRQSrc                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :   This routine allows to process the UART interrupt     */
/*                    sources identified through the p_status structure. It */
/*                    processes all the interrupts, it updates p_status     */
/*                    structure                                             */
/*  PARAMETERS    :                                                         */
/*          IN    :   None                                                  */
/*       INOUT    :   p_status :  pointer to the structure where the status */
/*                                of the interrupts are stored              */
/*         OUT    :   None                                                  */
/*                                                                          */
/*      RETURN    :   UART_NO_MORE_PENDING_EVENT(Positive value): Indicate  */
/*                    that all processing associated with the p_status      */
/*                    is done. The interrupt source can be reenabled.       */
/*                                                                          */
/*                :   UART_NO_PENDING_EVENT_ERROR: When there is no         */
/*                    interrupt to process                                  */
/*                                                                          */
/*                :   UART_INVALID_PARAMETER: This is returned  if pointer  */
/*                                            to interrupt status is NULL   */
/*                                                                          */
/*        NOTE    :   Before using this function ,it is necessasry to call  */
/*                    UART_GetIRQSrcStatus or UART_IsIrqSrcActive functions */
/*                    to intialize p_status structure.                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY           :    Non Re-Entrant                                 */
/* REENTRANCY ISSUES    :    Modifying global variable like                 */
/*                           g_uart_system_context.uart_active_event_status */
/****************************************************************************/
PUBLIC t_uart_error UART_ProcessIRQSrc(INOUT t_uart_irq_status *p_status)
{
    t_uart_error    error_status;
    DBGENTER1(" p_status POINTER %x ", (t_uint32) p_status);
    if (NULL == p_status)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    error_status = uart_ProcessIt(p_status);
    DBGEXIT0(error_status);
    return(error_status);
}


/****************************************************************************/
/* NAME         :   uart_SetPendingIrq                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to extract the pending Interrupts  */
/*                  from FilterProcess of IRQ                               */
/*  PARAMETERS  :                                                           */
/*          IN  :   temp_filter: Filter set for Interupts                   */
/*         OUT  :   irq_status : IRQ status filled in                       */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_SetPendingIrq(OUT t_uart_irq_status *irq_status, IN t_uint32 temp_filter)
{
    if (temp_filter & UART_IRQ_SRC_AUTOBAUD_ERROR)
    {
        irq_status->pending_irq = UART_IRQ_SRC_AUTOBAUD_ERROR;
    }
    else if (temp_filter & UART_IRQ_SRC_AUTOBAUD_DONE)
    {
        irq_status->pending_irq = UART_IRQ_SRC_AUTOBAUD_DONE;
    }
    else if (temp_filter & UART_IRQ_SRC_FE)
    {
        irq_status->pending_irq = UART_IRQ_SRC_FE;
    }
    else if (temp_filter & UART_IRQ_SRC_PE)
    {
        irq_status->pending_irq = UART_IRQ_SRC_PE;
    }
    else if (temp_filter & UART_IRQ_SRC_BE)
    {
        irq_status->pending_irq = UART_IRQ_SRC_BE;
    }
    else if (temp_filter & UART_IRQ_SRC_OE)
    {
        irq_status->pending_irq = UART_IRQ_SRC_OE;
    }
    else if (temp_filter & UART_IRQ_SRC_RX)
    {
        irq_status->pending_irq = UART_IRQ_SRC_RX;
    }
    else if (temp_filter & UART_IRQ_SRC_RXTO)
    {
        irq_status->pending_irq = UART_IRQ_SRC_RXTO;
    }
    else if (temp_filter & UART_IRQ_SRC_TX)
    {
        irq_status->pending_irq = UART_IRQ_SRC_TX;
    }
    else if (temp_filter & UART_IRQ_SRC_CTS)
    {
        irq_status->pending_irq = UART_IRQ_SRC_CTS;
    }
    else if (temp_filter & UART_IRQ_SRC_RI)
    {
        irq_status->pending_irq = UART_IRQ_SRC_RI;
    }
    else if (temp_filter & UART_IRQ_SRC_DCD)
    {
        irq_status->pending_irq = UART_IRQ_SRC_DCD;
    }
    else if (temp_filter & UART_IRQ_SRC_DSR)
    {
        irq_status->pending_irq = UART_IRQ_SRC_DSR;
    }
    else if (temp_filter & UART_IRQ_SRC_XOFF)
    {
        irq_status->pending_irq = UART_IRQ_SRC_XOFF;
    }
    else if (temp_filter & UART_IRQ_SRC_TXFE)
    {
        irq_status->pending_irq = UART_IRQ_SRC_TXFE;
    }
    return;

}
/****************************************************************************/
/* NAME           :    UART_FilterProcessIRQSrc                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :  This iterative routine allows to process the current   */
/*                   UART device interrupt sources identified through the   */
/*                   p_status. It processes the interrupt sources one by    */
/*                   one, updates the p_status to keep an internal history. */
/*                   and generates the events. This function must be called */
/*                   untill all processing is not done or all events are not*/
/*                   generated.                                             */
/*                                                                          */
/*  PARAMETERS    :                                                         */
/*          IN    :  filter_mode : Filter to selectively process the        */
/*                                 interrupt                                */
/*       INOUT    :  p_status    : Status of the interrupt                  */
/*                :  p_event     : Current event that is handled by the     */
/*                                routine                                   */
/*      RETURN    :  UART_INTERNAL_EVENT(Positive value): It indicates that */
/*                   all the processing associated with p_status has been   */
/*                   done all the events are not yet processed  i.e all the */
/*                   events have not been acknowledged.                     */
/*                                                                          */
/*                :  UART_NO_MORE_PENDING_EVENT(Positive value): Indicate   */
/*                   that all processing associated with the p_status       */
/*                   (without filter option) is done .The interrupt source  */
/*                   can be reenabled.                                      */
/*                                                                          */
/*                :  UART_NO_MORE_FILTER_PENDING_EVENT(Positive value)      */
/*                   indicates that all the processing associated with      */
/*                   p_status with the filter option is done                */
/*                                                                          */
/*                :  UART_NO_PENDING_EVENT_ERROR :when there is no interrupt*/
/*                   to process                                             */
/*                                                                          */
/*                :  UART_REMAINING_PENDING_EVENTS(Positive value): Indicate*/
/*                   that the UART_FilterProcessIRQSrc function must be     */
/*                   re-called to complete the processing. Other events     */
/*                   must be generated/processed.                           */
/*                                                                          */
/*                :  UART_INVALID_PARAMETER: This is returned  if pointer to*/
/*                   interrupt status is NULL                               */
/*                                                                          */
/*      NOTE      :  Before using this function ,it is necessasry to call   */
/*                   UART_GetIRQSrcStatus or UART_IsIrqSrcActive functions  */
/*                   to intialize p_status structure.                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY           :    Non Re-Entrant                                 */
/* REENTRANCY ISSUES    :    Modifying global variable                      */
/*                           g_uart_system_context.uart_active_event_status */
/****************************************************************************/
PUBLIC t_uart_error UART_FilterProcessIRQSrc
(
    INOUT t_uart_irq_status *p_status,
    INOUT t_uart_event      *p_event,
    IN t_uart_filter_mode   filter_mode
)
{
    t_uart_device_id    uart_device_id;
    t_uart_irq_status   irq_status;
    t_uint32            temp_filter;
    t_uint32            *lp_Event;
    t_uart_error        error_status;

    DBGENTER3("p_status %p, p_event %p, filter_mode %x", (t_uint32 *)p_status, (t_uint32 *)p_event, (t_uint32)filter_mode);
    if (NULL == p_status || NULL == p_event)
    {
        DBGPRINT(DBGL_ERROR, "Some OUT parameter NULL!");
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    if ((UART_IRQ_STATE_NEW == p_status->irq_state) && (0 == p_status->initial_irq))
    {
        DBGPRINT(DBGL_ERROR, "NO PENDING EVENTS!");
        DBGEXIT0(UART_NO_PENDING_EVENT_ERROR);
        return(UART_NO_PENDING_EVENT_ERROR);
    }

    p_status->irq_state = UART_IRQ_STATE_OLD;
    DBGPRINT(DBGL_HCL_DEV, "Call UART_GETDEVICE!");
   
    uart_device_id = UART_GETDEVICE(p_status->pending_irq);
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_Event = &g_uart_system_context.uart0_event;
            break;

        case UART_DEVICE_ID_1:
            lp_Event = &g_uart_system_context.uart1_event;
            break;

        case UART_DEVICE_ID_2:
            lp_Event = &g_uart_system_context.uart2_event;
            break;

        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    filter_mode = filter_mode & UART_IRQ_SRC_DEVICE_ALL_INTERRUPT;
    if (UART_NO_FILTER_MODE == filter_mode)
    {
        temp_filter = p_status->pending_irq & UART_IRQ_SRC_DEVICE_ALL_INTERRUPT;
        if (0 == temp_filter)
        {
            if (0 == (*lp_Event))
            {
                DBGEXIT0(UART_NO_MORE_PENDING_EVENT);
                return(UART_NO_MORE_PENDING_EVENT);
            }
            else
            {
                DBGEXIT0(UART_INTERNAL_EVENT);
                return(UART_INTERNAL_EVENT);
            }
        }
    }
    else
    {
        temp_filter = filter_mode & p_status->pending_irq;
        if (0 == temp_filter)
        {
            if (0 == ((*lp_Event) & filter_mode))
            {
                if (0 == (*lp_Event))
                {
                    DBGEXIT0(UART_NO_MORE_PENDING_EVENT);
                    return(UART_NO_MORE_PENDING_EVENT);
                }
                else
                {
                    DBGEXIT0(UART_NO_MORE_FILTER_PENDING_EVENT);
                    return(UART_NO_MORE_FILTER_PENDING_EVENT);
                }
            }
            else
            {
                DBGEXIT0(UART_INTERNAL_EVENT);
                return(UART_INTERNAL_EVENT);
            }
        }
    }

    irq_status.pending_irq = 0;
    uart_SetPendingIrq(&irq_status, temp_filter);

    (*lp_Event) |= irq_status.pending_irq;
    p_status->pending_irq &= ~(irq_status.pending_irq);
    irq_status.pending_irq = (irq_status.pending_irq | (((t_uint32) uart_device_id) << UART_ID_SHIFT));
    *p_event = irq_status.pending_irq;
    DBGPRINT(DBGL_HCL_DEV, "Call uart_ProcessIt!");

    error_status = uart_ProcessIt(&irq_status);
    /*coverity[self_assign]*/
    error_status = error_status;

    DBGEXIT0(UART_REMAINING_PENDING_EVENTS);
    return(UART_REMAINING_PENDING_EVENTS);
}

/****************************************************************************/
/* NAME           :    UART_IsEventActive                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    Checks whether the event in parameter is active or   */
/*                     not by checking the global event variable of the HCL */
/*  PARAMETERS    :                                                         */
/*          IN    :    p_event :  Event to be checked.                      */
/*         OUT    :    none                                                 */
/*                                                                          */
/*      RETURN    :    TRUE    :   Returned when the requested interrupt    */
/*                                 event is active                          */
/*                :    FALSE   :   Returned when the requested interrupt    */
/*                                 event is not active                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/*--------------------------------------------------------------------------*/
/* COMMENTS     :   The return type for this API is t_bool so an error code */
/*                  can't return UART_INVALID_PARAMETER when the pointer    */
/*                  passed is NULL                                          */
/****************************************************************************/
PUBLIC t_bool UART_IsEventActive(IN t_uart_event *p_event)
{
    t_uart_device_id    uart_device_id;

    DBGENTER1("(Event Pointer: %p)", (void *) p_event);
    if (NULL == p_event)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(FALSE);
    }
    DBGPRINT(DBGL_HCL_DEV, "Call UART_GETDEVICE!");

    uart_device_id = UART_GETDEVICE(*p_event);

    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        if (0 != ((*p_event) & g_uart_system_context.uart0_event))
        {
            DBGEXIT2(UART_OK, "EVENT %x, EVENT OCCURED %s", *p_event, "TRUE");
            return(TRUE);
        }
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        if (0 != ((*p_event) & g_uart_system_context.uart1_event))
        {
            DBGEXIT2(UART_OK, "EVENT %x, EVENT OCCURED %s", *p_event, "TRUE");
            return(TRUE);
        }
    }

    else
    {
        if (0 != ((*p_event) & g_uart_system_context.uart2_event))
        {
            DBGEXIT2(UART_OK, "EVENT %x, EVENT OCCURED %s", *p_event, "TRUE");
            return(TRUE);
        }
    }
    DBGEXIT2(UART_OK, "EVENT %x, EVENT OCCURED %s", *p_event, "FALSE");
    return(FALSE);
}

/****************************************************************************/
/* NAME           :    UART_AcknowledgeEvent                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    AcknowledgeEvent is called by the upper layer, after */
/*                     processing the given event. This routine resets the  */
/*                     event bit of the global event variable.              */
/*  PARAMETERS    :                                                         */
/*          IN    :    p_event    :  Event to be acknowledged.              */
/*         OUT    :    none                                                 */
/*                                                                          */
/*      RETURN    :    None                                                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY           :     Non-ReEntrant                                 */
/* REENTRANCY ISSUES    :    g_uart_system_context.uart_active_event_status */
/*                             global event status is being modified        */
/*--------------------------------------------------------------------------*/
/* COMMENTS     :   The return type for this API is void so an error code   */
/*                  can't return UART_INVALID_PARAMETER when the pointer    */
/*                  passed is NULL                                          */
/****************************************************************************/
PUBLIC void UART_AcknowledgeEvent(IN t_uart_event *p_event)
{
    t_uart_device_id    uart_device_id;

    DBGENTER1(" (Event Pointer: %p)", (void *) p_event);
    if (NULL == p_event)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return;
    }

    DBGPRINT(DBGL_HCL_DEV, "Call UART_GETDEVICE!");
    uart_device_id = UART_GETDEVICE(*p_event);
    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        g_uart_system_context.uart0_event &= ~(*p_event & UART_IRQ_SRC_DEVICE_ALL_INTERRUPT);
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        g_uart_system_context.uart1_event &= ~(*p_event & UART_IRQ_SRC_DEVICE_ALL_INTERRUPT);
    }

    else
    {
        g_uart_system_context.uart2_event &= ~(*p_event & UART_IRQ_SRC_DEVICE_ALL_INTERRUPT);
    }
    DBGEXIT1(UART_OK, " EVENT (%x) ", *p_event);
}



/****************************************************************************/
/* NAME         :   uart_ProcessErrorIt                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to process the pending Error       */
/*                  Interrupts                                              */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id: UART Device ID                          */
/*              :   lp_uart_register : UART register Set                    */
/*              :   interrupt_mask : Errors to be processed as a bitmask    */
/*         OUT  :   p_comm_status : Communication Status to be updated      */
/*                                                                          */
/*      RETURN  :   t_uart_error                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_ProcessErrorIt(IN t_uart_device_id uart_device_id, IN t_uart_register *lp_uart_register, 
		                         IN t_uart_irq_src interrupt_mask, OUT t_uart_it_communication_status  *p_comm_status)
{
    /* Force HCL user to call UART_SetConfiguration to UART_CLEAR error */
    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        /* Mask all Interrupts except Transmit UART_IT */
        lp_uart_register->uartx_imsc &= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
        g_uart_system_context.uart0_need_to_call_config = TRUE;
        if (interrupt_mask & UART_IRQ_SRC_FE)
        {
            DBGPRINT(DBGL_HCL_DEV, "FRAMING ERROR");
            g_uart_system_context.uart0_rx_status = (t_uart_receive_status) UART_FRAMING_ERROR;

	    /* UART_CLEAR FE Interrupt */
            lp_uart_register->uartx_icr |= UART_IRQ_SRC_FE;
        }

        if (interrupt_mask & UART_IRQ_SRC_PE)
        {
            DBGPRINT(DBGL_HCL_DEV, "PARITY ERROR");
            g_uart_system_context.uart0_rx_status = (t_uart_receive_status) UART_PARITY_ERROR;

            /* UART_CLEAR PE Interrupt */
            lp_uart_register->uartx_icr |= UART_IRQ_SRC_PE;
        }

        if (interrupt_mask & UART_IRQ_SRC_OE)
        {
            DBGPRINT(DBGL_HCL_DEV, "OVERRUN ERROR");
            g_uart_system_context.uart0_rx_status = (t_uart_receive_status) UART_OVERRUN_ERROR;

            /* UART_CLEAR OE Interrupt */
            lp_uart_register->uartx_icr |= UART_IRQ_SRC_OE;
        }
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        /* Mask all Interrupts except Transmit UART_IT */
        lp_uart_register->uartx_imsc &= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
        g_uart_system_context.uart1_need_to_call_config = TRUE;
        if (interrupt_mask & UART_IRQ_SRC_FE)
        {
            DBGPRINT(DBGL_HCL_DEV, "FRAMING ERROR");
            g_uart_system_context.uart1_rx_status = (t_uart_receive_status) UART_FRAMING_ERROR;
            /* UART_CLEAR FE Interrupt */
            lp_uart_register->uartx_icr |= UART_IRQ_SRC_FE;
        }

        if (interrupt_mask & UART_IRQ_SRC_PE)
        {
            DBGPRINT(DBGL_HCL_DEV, "PARITY ERROR");
            g_uart_system_context.uart1_rx_status = (t_uart_receive_status) UART_PARITY_ERROR;

            /* UART_CLEAR PE Interrupt */
            lp_uart_register->uartx_icr |= UART_IRQ_SRC_PE;
        }

        if (interrupt_mask & UART_IRQ_SRC_OE)
        {
            DBGPRINT(DBGL_HCL_DEV, "OVERRUN ERROR");
            g_uart_system_context.uart1_rx_status = (t_uart_receive_status) UART_OVERRUN_ERROR;

            /* UART_CLEAR OE Interrupt */
            lp_uart_register->uartx_icr |= UART_IRQ_SRC_OE;
        }
    }

    else
    {
        /* Mask all Interrupts except Transmit UART_IT */
        lp_uart_register->uartx_imsc &= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
        g_uart_system_context.uart2_need_to_call_config = TRUE;
        if (interrupt_mask & UART_IRQ_SRC_FE)
        {
            DBGPRINT(DBGL_HCL_DEV, "FRAMING ERROR");
            g_uart_system_context.uart2_rx_status = (t_uart_receive_status) UART_FRAMING_ERROR;

            /* UART_CLEAR FE Interrupt */
            lp_uart_register->uartx_icr |= UART_IRQ_SRC_FE;
        }

        if (interrupt_mask & UART_IRQ_SRC_PE)
        {
            DBGPRINT(DBGL_HCL_DEV, "PARITY ERROR");
            g_uart_system_context.uart2_rx_status = (t_uart_receive_status) UART_PARITY_ERROR;

	    /* UART_CLEAR PE Interrupt */
            lp_uart_register->uartx_icr |= UART_IRQ_SRC_PE;
        }

        if (interrupt_mask & UART_IRQ_SRC_OE)
        {
            DBGPRINT(DBGL_HCL_DEV, "OVERRUN ERROR");
            g_uart_system_context.uart2_rx_status = (t_uart_receive_status) UART_OVERRUN_ERROR;

            /* UART_CLEAR OE Interrupt */
            lp_uart_register->uartx_icr |= UART_IRQ_SRC_OE;
        }
    }

    /* Inform HCL user about receive's end */
    p_comm_status->rx_comm_status = UART_RECEIVE_ENDED;
    DBGPRINT(DBGL_ERROR, " Receive Error UART_IT");
    DBGEXIT0(UART_ERROR);
    return(UART_ERROR);

}


/****************************************************************************/
/* NAME         :   uart_ProcessDCEMode                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to process the pending DCE Mode    */
/*                  Interrupts                                              */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : UART Device ID                       */
/*              :   lp_uart_register : UART register Set                    */
/*              :   transfer_flags   : Transfer Mode of UART presently      */
/*              :   flow_control     : Flow Control Mode Set                */
/*              :   p_comm_status    : Communication Status                 */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_ProcessDCEMode(IN t_uart_device_id uart_device_id, IN t_uart_register *lp_uart_register, 
		                 IN t_uart_it_communication_status *p_comm_status, IN t_uint32 transfer_flags,
				 IN t_uart_flow_control flow_control)
{
    t_bool                          is_xoff_recd;
    t_bool                          is_xon_recd;
    t_uart_set_or_clear             uart_cts;
    t_uart_set_or_clear             uart_dtr;
    t_uart_error                    error_status;

    error_status = UART_DCEGetInputSignals(uart_device_id, &uart_dtr, &uart_cts);

    /*coverity[uninit_use]*/
    if (UART_SET == uart_dtr)
    {
	DBGPRINT(DBGL_HCL_DEV, "SET DTR");
        if ((transfer_flags & UART_TX_IT_MODE) && UART_TRANSMIT_ON_GOING == p_comm_status->tx_comm_status)
        {
            error_status = UART_GetFlowControlStatus(uart_device_id, &is_xoff_recd, &is_xon_recd);
            /*coverity[uninit_use]*/
            if
            (
                UART_NO_FLOW_CONTROL == flow_control
            ||  (UART_SW_FLOW_CONTROL == flow_control && FALSE == is_xoff_recd)
            )
            {
                lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
            }
        }
        if ((transfer_flags & UART_RX_IT_MODE) && UART_RECEIVE_ON_GOING == p_comm_status->rx_comm_status)
        {
            lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
        }
    }
    else
    {
        DBGPRINT(DBGL_HCL_DEV, "CLEAR DTR");
        error_status = UART_DCESetOutputSignals(uart_device_id,UART_NO_CHANGE,UART_CLEAR,UART_NO_CHANGE,UART_NO_CHANGE);
        if ((transfer_flags & UART_TX_IT_MODE) && UART_TRANSMIT_ON_GOING == p_comm_status->tx_comm_status)
        {
            lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
        }
        if ((transfer_flags & UART_RX_IT_MODE) && UART_RECEIVE_ON_GOING == p_comm_status->rx_comm_status)
        {
            lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
        }
    }
   
    /*coverity[self_assign]*/
    error_status = error_status; 
    return;
}

/****************************************************************************/
/* NAME         :   uart_ProcessDSRIt                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to process the pending DSR         */
/*                  Interrupts                                              */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : UART Device ID                       */
/*              :   lp_uart_register : UART register Set                    */
/*              :   transfer_flags   : Transfer Mode of UART presently      */
/*              :   flow_control     : Flow Control Mode Set                */
/*              :   p_comm_status    : Communication Status                 */
/*              :   uart_mode        : DCE/DTE Mode                         */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_ProcessDSRIt(IN t_uart_device_id uart_device_id, IN t_uart_register *lp_uart_register, 
		                       IN t_uart_it_communication_status *p_comm_status, IN t_uart_mode uart_mode, 
			 	       IN t_uint32 transfer_flags, IN t_uart_flow_control flow_control)
{
    t_uart_set_or_clear             uart_dsr;
    t_uart_set_or_clear             uart_dcd;
    t_uart_set_or_clear             uart_ri;
    t_uart_set_or_clear             uart_cts;
    t_uart_error                    error_status=UART_OK;
    t_bool                          is_xoff_recd;
    t_bool                          is_xon_recd;

    lp_uart_register->uartx_icr |= UART_IRQ_SRC_DSR;

    if (UART_DCE_MODE == uart_mode)
    {
        uart_ProcessDCEMode(uart_device_id, lp_uart_register, p_comm_status, transfer_flags, flow_control);
        DBGPRINT(DBGL_HCL_DEV, " DTR UART_IT in DCE in ProcessIt ");
        DBGEXIT0(UART_NO_MORE_PENDING_EVENT);
        /* Notify the upper layer */
        return(UART_NO_MORE_PENDING_EVENT);
    }

    if (UART_DTE_MODE == uart_mode)
    {
        error_status = UART_DTEGetInputSignals(uart_device_id, &uart_ri, &uart_dsr, &uart_dcd, &uart_cts);
        /*coverity[uninit_use]*/
	if (UART_SET == uart_dcd)
        {
            if ((transfer_flags & UART_TX_IT_MODE) && UART_TRANSMIT_ON_GOING == p_comm_status->tx_comm_status)
            {
                error_status = UART_GetFlowControlStatus(uart_device_id, &is_xoff_recd, &is_xon_recd);
                /*coverity[uninit_use]*/
		if
                (
                    UART_NO_FLOW_CONTROL == flow_control
                ||  (UART_SW_FLOW_CONTROL == flow_control && FALSE == is_xoff_recd)
                )
                {
                    lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
                }
            }

            if ((transfer_flags & UART_RX_IT_MODE) && UART_RECEIVE_ON_GOING == p_comm_status->rx_comm_status)
            {
                lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
            }
        }
        else
        {
            error_status = UART_DTESetOutputSignals(uart_device_id, UART_CLEAR, UART_NO_CHANGE);
	    if ((transfer_flags & UART_TX_IT_MODE) && UART_TRANSMIT_ON_GOING == p_comm_status->tx_comm_status)
            {
                lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
            }

            if ((transfer_flags & UART_RX_IT_MODE) && UART_RECEIVE_ON_GOING == p_comm_status->rx_comm_status)
            {
                lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
            }
        }
    }
    /*coverity[self_assign]*/
    error_status = error_status; 
    DBGPRINT(DBGL_HCL_DEV, " DSR UART_IT in DTE in ProcessIt ");
    DBGEXIT0(UART_OK);
    return(UART_OK);
}


/****************************************************************************/
/* NAME         :   uart_ProcessDCDIt                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to process the pending DCD         */
/*                  Interrupts                                              */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : UART Device ID                       */
/*              :   lp_uart_register : UART register Set                    */
/*              :   transfer_flags   : Transfer Mode of UART presently      */
/*              :   flow_control     : Flow Control Mode Set                */
/*              :   p_comm_status    : Communication Status                 */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_ProcessDCDIt(IN t_uart_device_id uart_device_id, IN t_uart_register *lp_uart_register, 
		               IN t_uart_it_communication_status *p_comm_status, IN t_uint32 transfer_flags, 
			       IN t_uart_flow_control flow_control)
{
    t_uart_set_or_clear             uart_dsr;
    t_uart_set_or_clear             uart_dcd;
    t_uart_set_or_clear             uart_ri;
    t_uart_set_or_clear             uart_cts;
    t_uart_error                    error_status;
    t_bool                          is_xoff_recd;
    t_bool                          is_xon_recd;

    error_status = UART_DTEGetInputSignals(uart_device_id, &uart_ri, &uart_dsr, &uart_dcd, &uart_cts);
    /*coverity[uninit_use]*/
    if (UART_SET == uart_dcd)
    {
        if ((transfer_flags & UART_TX_IT_MODE) && UART_TRANSMIT_ON_GOING == p_comm_status->tx_comm_status)
        {
            error_status = UART_GetFlowControlStatus(uart_device_id, &is_xoff_recd, &is_xon_recd);
            /*coverity[uninit_use]*/
	    if
            (
                UART_NO_FLOW_CONTROL == flow_control
            ||  (UART_SW_FLOW_CONTROL == flow_control && FALSE == is_xoff_recd)
            )
            {
                lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
            }
        }

        if ((transfer_flags & UART_RX_IT_MODE) && UART_RECEIVE_ON_GOING == p_comm_status->rx_comm_status)
        {
            lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
        }
    }
    else
    {
        DBGPRINT(DBGL_HCL_DEV, "Clear DTR");
        /* UART_CLEAR DTR in DTE */
        error_status = UART_DTESetOutputSignals(uart_device_id, UART_CLEAR, UART_NO_CHANGE);
	if ((transfer_flags & UART_TX_IT_MODE) && UART_TRANSMIT_ON_GOING == p_comm_status->tx_comm_status)
        {
            lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
        }

        if ((transfer_flags & UART_RX_IT_MODE) && UART_RECEIVE_ON_GOING == p_comm_status->rx_comm_status)
        {
            lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
        }
    }
    /*coverity[self_assign]*/
    error_status = error_status; 
    lp_uart_register->uartx_icr |= UART_IRQ_SRC_DCD;
    return;
}

/****************************************************************************/
/* NAME         :   uart_ProcessTxIt                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to process the pending Tx          */
/*                  Interrupts                                              */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : UART Device ID                       */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_ProcessTxIt(IN t_uart_device_id uart_device_id)
{
    t_uart_it_communication_status  *p_comm_status=0x00;
    t_uart_error                    error_status = UART_ERROR;
    t_uint32                        uart_fifo_trigger_tx = 0;
    t_uint32                        ntx = 0;                        /* Variables related to receive */
    t_uint32                        num_of_char_to_be_written = 0;
    t_uint32                        uart_number_of_char_to_be_tx=0x00;   
    t_uart_register                 *lp_uart_register=0x00;
    t_uint8                         *p_transmit_uart = 0x00; 
    t_uint32                        num_of_char_written = 0;

    switch (uart_device_id)                             /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            {
                lp_uart_register = g_uart_system_context.p_uart0_register;
                p_comm_status = &g_uart_system_context.uart0_comm_status;
                ntx = g_uart_system_context.uart0_ntx;  /* Update context for UART_DEVICE_ID_0 */
                p_transmit_uart = g_uart_system_context.p_uart0_transmit_buffer;
                uart_number_of_char_to_be_tx = g_uart_system_context.uart0_number_of_char_to_be_tx;
                uart_fifo_trigger_tx = g_uart_system_context.uart0_trigger_tx;
                break;
            }

        case UART_DEVICE_ID_1:
            {
                lp_uart_register = g_uart_system_context.p_uart1_register;
                p_comm_status = &g_uart_system_context.uart1_comm_status;
                ntx = g_uart_system_context.uart1_ntx;  /* Update context for UART_DEVICE_ID_1 */
                p_transmit_uart = g_uart_system_context.p_uart1_transmit_buffer;
                uart_number_of_char_to_be_tx = g_uart_system_context.uart1_number_of_char_to_be_tx;
                uart_fifo_trigger_tx = g_uart_system_context.uart1_trigger_tx;
                break;
            }


        case UART_DEVICE_ID_2:
            {
                lp_uart_register = g_uart_system_context.p_uart2_register;
                p_comm_status = &g_uart_system_context.uart2_comm_status;
                ntx = g_uart_system_context.uart2_ntx;  /* Update context for UART_DEVICE_ID_1 */
                p_transmit_uart = g_uart_system_context.p_uart2_transmit_buffer;
                uart_number_of_char_to_be_tx = g_uart_system_context.uart2_number_of_char_to_be_tx;
                uart_fifo_trigger_tx = g_uart_system_context.uart2_trigger_tx;
                break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return;
    }

    if ((uart_number_of_char_to_be_tx - ntx) <=  uart_fifo_trigger_tx)
    {
         num_of_char_to_be_written = (uart_number_of_char_to_be_tx - ntx);
    }
    else
    {
         num_of_char_to_be_written =  uart_fifo_trigger_tx;
    }

    DBGPRINT(DBGL_HCL_DEV, "Calling uart_WritexCharacters due to TX FIFO Empty Interrupt");
    error_status = uart_WritexCharacters(uart_device_id, p_transmit_uart + ntx, 
		                                 num_of_char_to_be_written, &num_of_char_written);
    ntx = ntx + num_of_char_written;
    if (ntx == uart_number_of_char_to_be_tx)        /* Writing's End */
    {
        p_comm_status->tx_comm_status = UART_WRITTINGS_ENDED;
        /* Disable Transmit  and Tx FIFO Empty Interrupt */
        lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
    }

    if (UART_DEVICE_ID_0 == uart_device_id)         /* Update UART dependant variables */
    {
        g_uart_system_context.uart0_ntx = ntx;      /* Update context for UART_DEVICE_ID_0 */
    }
            
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        g_uart_system_context.uart1_ntx = ntx;      /* Update context for UART_DEVICE_ID_1 */
    }

    else
    {
        g_uart_system_context.uart2_ntx = ntx;      /* Update context for UART_DEVICE_ID_1 */
    }

    /*coverity[self_assign]*/
    error_status = error_status; 
    return;

}


/****************************************************************************/
/* NAME         :   uart_ProcessRxIt                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to process the pending Rx          */
/*                  Interrupts                                              */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : UART Device ID                       */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_ProcessRxIt(IN t_uart_device_id uart_device_id)
{
    t_uart_error error_status = UART_OK;
    t_uart_it_communication_status  *p_comm_status = 0x00;
    t_uint32                         uart_fifo_trigger_rx = 0;
    t_uint32                        nrx = 0;                        /* Variables related to receive */
    t_uint32                        char_number_read = 0;
    t_uint32                        char_number_to_read = 0;
    t_uart_receive_status           *p_receive_error = 0x00;
    t_uart_receive_status           rec_status = (t_uart_receive_status) UART_RECEIVE_OK;
    t_uint8                         *p_receive_uart = 0x00;                /* Stores pointer on current receive buffer (UART_DEVICE_ID_0 or 1) */
    t_uint32                        uart_number_of_char_to_be_rx = 0x00;   /* Stores bytes number to receive depending on UART number */
    t_uart_register                 *lp_uart_register = 0x00;
    p_receive_error = &rec_status;

    switch (uart_device_id)                             /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            {
                lp_uart_register = g_uart_system_context.p_uart0_register;
                p_comm_status = &g_uart_system_context.uart0_comm_status;
                nrx = g_uart_system_context.uart0_nrx;
                p_receive_uart = g_uart_system_context.p_uart0_receive_buffer;
                uart_number_of_char_to_be_rx = g_uart_system_context.uart0_number_of_char_to_be_rx;
                uart_fifo_trigger_rx = g_uart_system_context.uart0_trigger_rx;
                break;
            }

        case UART_DEVICE_ID_1:
            {
                lp_uart_register = g_uart_system_context.p_uart1_register;
                p_comm_status = &g_uart_system_context.uart1_comm_status;
                nrx = g_uart_system_context.uart1_nrx;
                p_receive_uart = g_uart_system_context.p_uart1_receive_buffer;
                uart_number_of_char_to_be_rx = g_uart_system_context.uart1_number_of_char_to_be_rx;
                uart_fifo_trigger_rx = g_uart_system_context.uart1_trigger_rx;
                break;
            }


        case UART_DEVICE_ID_2:
            {
                lp_uart_register = g_uart_system_context.p_uart2_register;
                p_comm_status = &g_uart_system_context.uart2_comm_status;
                nrx = g_uart_system_context.uart2_nrx;
                p_receive_uart = g_uart_system_context.p_uart2_receive_buffer;
                uart_number_of_char_to_be_rx = g_uart_system_context.uart2_number_of_char_to_be_rx;
                uart_fifo_trigger_rx = g_uart_system_context.uart2_trigger_rx;
                break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);  

    }

    if ((uart_number_of_char_to_be_rx - nrx) <=  uart_fifo_trigger_rx)
    {
        char_number_to_read = (uart_number_of_char_to_be_rx - nrx);
    }
    else
    {
        (char_number_to_read =  uart_fifo_trigger_rx);
    }
        
    DBGPRINT(DBGL_HCL_DEV, "Call uart_ReadxCharacters");

    error_status = uart_ReadxCharacters
    (
        uart_device_id,
        char_number_to_read,
        p_receive_uart + nrx,
        &char_number_read,
        p_receive_error
    );
    if ((t_uart_receive_status) UART_RECEIVE_OK != *p_receive_error)
    {
        DBGPRINT(DBGL_ERROR, " Receive Error in ProcessIt after ReadxChar ");
        DBGEXIT0(UART_ERROR);
        return(UART_ERROR);
    }

    nrx = nrx + char_number_read;
    if (nrx == uart_number_of_char_to_be_rx)        /* Receive End */
    {
        p_comm_status->rx_comm_status = UART_RECEIVE_ENDED;
        lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
    }

    if (UART_DEVICE_ID_0 == uart_device_id)         /* Update UART dependant variables */
    {
        g_uart_system_context.uart0_nrx = nrx;      /* Update context for UART_DEVICE_ID_0 */
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        g_uart_system_context.uart1_nrx = nrx;      /* Update context for UART_DEVICE_ID_1 */
    }

    else
    {
        g_uart_system_context.uart2_nrx = nrx;      /* Update context for UART_DEVICE_ID_1 */
    }

#if ((defined __PEPS_8500) || (defined __PEPS_8500_V1) || (defined __PEPS_8500_V2))  
    lp_uart_register->uartx_icr = (UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);  /* Change for PEPS */
#endif	

    /*coverity[self_assign]*/
    error_status = error_status; 
    DBGEXIT0(UART_OK);
    return(UART_OK);
}



/****************************************************************************/
/* NAME         :   uart_ProcessAutobaudIt                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to process the pending Rx          */
/*                  Interrupts                                              */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : UART Device ID                       */
/*              :   lp_uart_register : UART Register Set                    */
/*              :   interrupt_mask   : Mask of pending ITs                  */
/*         OUT  :   error_status     : Result of Operation                  */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_ProcessAutobaudIt(IN t_uart_irq_src interrupt_mask, IN t_uart_register *lp_uart_register, 
		                    OUT volatile t_uart_error *error_status, IN t_uart_device_id uart_device_id)
{
    t_uart_config *uart_config = NULL;
    t_uint32 integer_div = 0x00, frac_div = 0x00, ovsfact = 0x00;
	
    if (0 != (interrupt_mask & UART_IRQ_SRC_AUTOBAUD_ERROR))
    /* Autobaud Error Interrupt */
    {
        DBGPRINT(DBGL_HCL_DEV, "Autobaud Error Interrupt: ");
        lp_uart_register->uartx_abicr |= UART_IRQ_SRC_ABE;
        *error_status = UART_NO_MORE_PENDING_EVENT;
    }

    if (0 != (interrupt_mask & UART_IRQ_SRC_AUTOBAUD_DONE))
    {
        DBGPRINT(DBGL_HCL_DEV, "Autobaud Done Interrupt: ");

        /* UART_CLEAR & Disable all the autobaud interrupts */
        lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
        lp_uart_register->uartx_abimsc &= ~(UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);

        lp_uart_register->uartx_abcr = MASK_NULL16;
        if (UART_DEVICE_ID_0 == uart_device_id)
        {
            g_uart_system_context.uart0_is_autobaud_done = TRUE;
	    uart_config = &(g_uart_system_context.uart0_config);
        }
        else if (UART_DEVICE_ID_1 == uart_device_id)
        {
            g_uart_system_context.uart1_is_autobaud_done = TRUE;
	    uart_config = &(g_uart_system_context.uart1_config);
        }

        else
        {
            g_uart_system_context.uart2_is_autobaud_done = TRUE;
	    uart_config = &(g_uart_system_context.uart2_config);
        }

	integer_div = lp_uart_register->uartx_ibrd;  /* Get Integer Divisor    */
	frac_div    = lp_uart_register->uartx_fbrd;  /* Get Fractional Divisor */
        ovsfact     = ((lp_uart_register->uartx_absr & MASK_BIT12)>>SHIFT_QUARTET3) + 1;         
	
	/* Now resolve the Baud Rate(76800 Baud is not supported in Software) in System Context */
	if( (UART_INTEGER_DIV_19200_BAUD ==integer_div) && (UART_FRACTION_DIV_19200_BAUD == frac_div) )
	{
             uart_config->baud_rate = (t_uart_baud_rate) ((t_uint32)UART_BR_19200_BAUD*ovsfact); 
	}
	else if( (UART_INTEGER_DIV_38400_BAUD ==integer_div) && (UART_FRACTION_DIV_38400_BAUD == frac_div) )
	{
             uart_config->baud_rate = (t_uart_baud_rate) ((t_uint32)UART_BR_38400_BAUD*ovsfact); 
	}
	else if( (UART_INTEGER_DIV_57600_BAUD ==integer_div) && (UART_FRACTION_DIV_57600_BAUD == frac_div) )
	{
             uart_config->baud_rate = (t_uart_baud_rate) ((t_uint32)UART_BR_57600_BAUD*ovsfact); 
	}
	else if( (UART_INTEGER_DIV_115200_BAUD ==integer_div) && (UART_FRACTION_DIV_115200_BAUD == frac_div) )
	{
             uart_config->baud_rate = (t_uart_baud_rate) ((t_uint32)UART_BR_115200_BAUD*ovsfact); 
	}
	
	lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_UEN;

	*error_status = UART_NO_MORE_PENDING_EVENT;
    }
    return;
}


/****************************************************************************/
/* NAME         :   uart_ProcessBaseAddr                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to extract certain UART settings   */
/*                  before servicing the Interrupts                         */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : UART Device ID                       */
/*         OUT  :   lp_uart_register : UART Register Set                    */
/*              :   p_comm_status    : Present status of Transmit/Receive   */
/*              :   flow_control     : Flow Control used                    */
/*              :   transfer_flags   : Flags indicating Mode of transfer    */
/*              :   uart_mode        : DCE/DTE mode                         */
/*                                                                          */
/*      RETURN  :   t_uart_error                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error  uart_ProcessBaseAddr(IN t_uart_device_id uart_device_id, OUT t_uart_register **lp_uart_register, 
		                           OUT t_uart_it_communication_status **p_comm_status, volatile OUT t_uart_mode *uart_mode, 
					   volatile OUT t_uart_flow_control *flow_control, volatile OUT t_uint32 *transfer_flags)
{
    switch (uart_device_id)                            
    {
        case UART_DEVICE_ID_0:
            {
                *lp_uart_register = g_uart_system_context.p_uart0_register;
                *p_comm_status = &g_uart_system_context.uart0_comm_status;
                *uart_mode = g_uart_system_context.uart0_config.uart_mode;
                *flow_control = g_uart_system_context.uart0_config.flow_control;
                *transfer_flags = g_uart_system_context.uart0_transfer_flags;
                break;
            }

        case UART_DEVICE_ID_1:
            {
                *lp_uart_register = g_uart_system_context.p_uart1_register;
                *p_comm_status = &g_uart_system_context.uart1_comm_status;
                *uart_mode = g_uart_system_context.uart1_config.uart_mode;
                *flow_control = g_uart_system_context.uart1_config.flow_control;
                *transfer_flags = g_uart_system_context.uart1_transfer_flags;
                break;
            }


        case UART_DEVICE_ID_2:
            {
                *lp_uart_register = g_uart_system_context.p_uart2_register;
                *p_comm_status = &g_uart_system_context.uart2_comm_status;
                *uart_mode = g_uart_system_context.uart2_config.uart_mode;
                *flow_control = g_uart_system_context.uart2_config.flow_control;
                *transfer_flags = g_uart_system_context.uart2_transfer_flags;
                break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGPRINT(DBGL_ERROR, "INVALID UART ID!");
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }
    
    DBGEXIT0(UART_OK);
    return(UART_OK);

}


/****************************************************************************/
/* NAME         :   uart_ProcessTXFEIt                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to service the Tx FIFO Empty       */
/*                  Interrupt                                               */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id   : UART Device ID                       */
/*              :   lp_uart_register : UART Register Set                    */
/*              :   p_comm_status    : Present status of Transmit/Receive   */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_ProcessTXFEIt(IN t_uart_device_id uart_device_id, IN t_uart_register *lp_uart_register, 
		                IN t_uart_it_communication_status  *p_comm_status )
{
    if(UART_TRANSMIT_ON_GOING == p_comm_status->tx_comm_status)
    {
        uart_ProcessTxIt(uart_device_id);
    }
    else
    {
        lp_uart_register->uartx_icr |= UART_IRQ_SRC_TXFE;
    }

    return;
}

/****************************************************************************/
/* NAME         :   uart_ProcessIt                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to process all the interrupts that  */
/*                  are high. The status of the interrupt is provided by    */
/*                  t_uart_irq_status.                                      */
/*  PARAMETERS  :                                                           */
/*       INOUT  :   p_status        : Status of the interrupt               */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error    : Error status                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_ProcessIt(INOUT t_uart_irq_status *p_status)
{
    t_uart_it_communication_status  *p_comm_status=NULL;
    volatile t_uart_error           error_status = UART_NO_MORE_PENDING_EVENT;
    t_uart_irq_src                  interrupt_mask;
    volatile t_uart_mode            uart_mode;
    volatile t_uart_flow_control    flow_control;
    t_uart_set_or_clear             uart_dsr;
    t_uart_set_or_clear             uart_dcd;
    t_uart_set_or_clear             uart_ri;
    t_uart_set_or_clear             uart_cts;
    t_uart_register        *lp_uart_register = NULL;
    t_uart_device_id                uart_device_id;
    volatile t_uint32               transfer_flags = 0x00;

    uart_device_id = UART_GETDEVICE(p_status->pending_irq);
    DBGENTER1(" UART DEVICE (%d) ", uart_device_id);
   
    /* Choose suitable UART base adress */
    error_status = uart_ProcessBaseAddr(uart_device_id, &lp_uart_register, &p_comm_status, 
		                                    &uart_mode, &flow_control, &transfer_flags);
    if(UART_OK != error_status)
    {
        DBGEXIT0(error_status);
	return(error_status);
    }

    /* Get the Normal Interrupt Status */
    interrupt_mask = p_status->pending_irq & UART_IRQ_SRC_DEVICE_ALL_INTERRUPT;
    if (0 == interrupt_mask)
    {
        DBGPRINT(DBGL_ERROR, "Interrupt MASK is 0");
        DBGEXIT0(UART_NO_PENDING_EVENT_ERROR);
        return(UART_NO_PENDING_EVENT_ERROR);
    }
 
    /*Process AutoBaud Int */
    uart_ProcessAutobaudIt(interrupt_mask, lp_uart_register, &error_status, uart_device_id);

    if ((0 != (interrupt_mask & UART_IRQ_SRC_XOFF)) /* Xoff UART_IT */ )
    {
        /* TBD */
    }

    if ((0 != (interrupt_mask & UART_IRQ_SRC_TXFE)) /* TX FIFO Empty UART_IT */ )
    {
        uart_ProcessTXFEIt(uart_device_id, lp_uart_register, p_comm_status);
        error_status = UART_NO_MORE_PENDING_EVENT;
    }

    /* Receive error UART_IT */
    if
    ( (0 != (interrupt_mask & UART_IRQ_SRC_FE)) ||  (0 != (interrupt_mask & UART_IRQ_SRC_PE)) 
      ||  (0 != (interrupt_mask & UART_IRQ_SRC_OE)) )
    {
        error_status = uart_ProcessErrorIt(uart_device_id, lp_uart_register, interrupt_mask, p_comm_status);
	if(UART_OK != error_status)
	{
             DBGEXIT0(error_status);
	     return(error_status);
	}
    }

    /* RECEIVE OR RECEIVE TIMEOUT UART_IT ON */
    if( (0 != (interrupt_mask & UART_IRQ_SRC_RX)) || (0 != (interrupt_mask & UART_IRQ_SRC_RXTO)) )
    {
        error_status = uart_ProcessRxIt(uart_device_id);
	if(UART_OK!=error_status)
	{
             DBGEXIT0(error_status);
	     return(error_status);
	}
        error_status = UART_NO_MORE_PENDING_EVENT;
    }

    if (0 != (interrupt_mask & UART_IRQ_SRC_TX))        /* TRANSMIT UART_IT ON */
    {
        uart_ProcessTxIt(uart_device_id);
        error_status = UART_NO_MORE_PENDING_EVENT;
    }

    if (0 != (interrupt_mask & UART_IRQ_SRC_RI))        /* Ring Indicator in DTE */
    {
        error_status = UART_DTEGetInputSignals(uart_device_id, &uart_ri, &uart_dsr, &uart_dcd, &uart_cts);
        /*coverity[uninit_use]*/
        if (UART_SET == uart_ri)
        {   /* Set DTR */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_DTR;
        }

        lp_uart_register->uartx_icr |= UART_IRQ_SRC_RI;
        DBGEXIT0(UART_NO_MORE_PENDING_EVENT);
        return(UART_NO_MORE_PENDING_EVENT);
    }

    if (0 != (interrupt_mask & UART_IRQ_SRC_CTS))   /* CLEAR To Send */
    {
        lp_uart_register->uartx_icr |= UART_IRQ_SRC_CTS;
        error_status = UART_NO_MORE_PENDING_EVENT;
    }

    if (0 != (interrupt_mask & UART_IRQ_SRC_DCD))   /* Data Carrier Detect in DTE */
    {
        /*coverity[uninit_use_in_call]*/
        uart_ProcessDCDIt(uart_device_id, lp_uart_register, p_comm_status,transfer_flags, flow_control);
        error_status = UART_NO_MORE_PENDING_EVENT;
    }

    if (0 != (interrupt_mask & UART_IRQ_SRC_BE))    /* Break condition detected */
    {
        lp_uart_register->uartx_icr |= UART_IRQ_SRC_BE;
        error_status = UART_NO_MORE_PENDING_EVENT;
    }

    if (0 != (interrupt_mask & UART_IRQ_SRC_DSR))   /* Data Set Ready in DTE */
    /* Data Terminal Ready in DCE */
    {
        /*coverity[uninit_use_in_call]*/
        error_status = uart_ProcessDSRIt(uart_device_id, lp_uart_register, p_comm_status, 
			                                     uart_mode, transfer_flags, flow_control);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :    uart_FillFifoTx                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to fill UART’s transmit fifo        */
/*                  Returns the number of characters effectively written    */
/*                  This routine is used to initiate a Tx transfer in       */
/*                  UART_IT mode                                            */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id              : Identify UART to configure*/
/*              :   p_data_char                 : Pointer on the first      */
/*                                                character to transmit     */
/*              :   num_of_char_to_be_written   : Number of characters to be*/
/*                                                transmitted               */
/*         OUT  :   p_num_of_char_written       : Number of characters      */
/*                                                written could be          */
/*                                                num_of_char_to_be_written */
/*                                                or less if tx_fifo becomes*/
/*                                                full during transfer      */
/*                                                                          */
/*      RETURN  :   t_uart_error                : UART error code           */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_FillFifoTx
(
    IN t_uart_device_id uart_device_id,
    IN t_uint8          *p_data_char,
    IN t_uint32         num_of_char_to_be_written,
    OUT t_uint32        *p_num_of_char_written
)
{
    t_uint32        count = 0;
    t_uart_error    error_status = UART_OK;
    t_uart_register *lp_uart_register;

    DBGENTER3("UART DEVICE %x, p_data_char %p, no of chars Tx %x", uart_device_id, p_data_char, num_of_char_to_be_written);
    switch (uart_device_id) /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    while ((count < num_of_char_to_be_written) && (FALSE == UART_ISTXFIFOFULL()))   /* Stops writing if transfer's end or fifo full */
    {
        /* 1 byte writing */
        lp_uart_register->uartx_dr = (t_uint32) * (p_data_char + count);
        count++;
    }

    *p_num_of_char_written = count;
	   
    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :    uart_WritexCharacters                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to write x characters in UART’s     */
/*                    transmit fifo Wait also for free space in UART_POLLING*/
/*                  mode                                                    */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id              : Identify UART to configure*/
/*              :   p_data_char                 : Pointer on the first      */
/*                                                character to transmit     */
/*              :   num_of_char_to_be_written   : Number of characters to   */
/*                                                be transmitted            */
/*                                                                          */
/*      RETURN  :   t_uart_error                : UART error code           */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_WritexCharacters
(
    IN  t_uart_device_id    uart_device_id,
    IN  t_uint8                *p_data_char,
    IN  t_uint32             num_of_char_to_be_written,
    OUT t_size                *p_num_of_char_written
)
{
    t_uint32        count = 0;
    t_uart_error    error_status = UART_OK;
    t_uint32        transfer_flags;
    t_uart_register *lp_uart_register;

    DBGENTER3("UART DEVICE %x, p_data_char %p, no of chars Tx %x", uart_device_id, p_data_char, num_of_char_to_be_written);
    
    switch (uart_device_id)                     /* Choose suitable UART base adress  */
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            transfer_flags = g_uart_system_context.uart0_transfer_flags;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            transfer_flags = g_uart_system_context.uart1_transfer_flags;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            transfer_flags = g_uart_system_context.uart2_transfer_flags;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    while (count < num_of_char_to_be_written)   /* Stops writing if transfer's end */
    {
        if ((transfer_flags & UART_TX_POLL_MODE))
        {
            do
            {
            } while (TRUE == UART_ISTXFIFOFULL());
        }
        else
        {
            if(TRUE == UART_ISTXFIFOFULL())
            {
                *p_num_of_char_written = count;
                 
                DBGPRINT(DBGL_HCL_DEV, "TXFIFO FULL in non-polling mode! No chars TX");
		DBGEXIT0(error_status);
		return(error_status);
            }
        }

        /* 1 byte writing */
        lp_uart_register->uartx_dr = (t_uint32) * (p_data_char + count);
        count++;
    }

    *p_num_of_char_written = count;

    DBGEXIT0(error_status);
    return(error_status);
}


/****************************************************************************/
/* NAME         :   uart_AnalyseReadError                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to analyse the Errors during Tx/Rx */
/*                                                                          */
/*  PARAMETERS  :                                                           */
/*          IN  :   data_received  : Error Data to analyse                  */
/*              :   perr_condition : Error Handling Set in UART upon errors */
/*         OUT  :   temp_flag      : Flag if error occured                  */
/*              :   p_receive_error: ERROR that occured                     */
/*                                                                          */
/*      RETURN  :   t_uart_error                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_AnalyseReadError(OUT t_uint32 *data_received, OUT t_uart_receive_status *p_receive_error, 
		                           OUT volatile t_bool *temp_flag, IN t_uart_perr_condition perr_condition)
{
    if (1 == ((*data_received) & UART_DATA_MASK_FE) >> UART_DATA_SHIFT_FE)
    {
        DBGPRINT(DBGL_HCL_DEV, "FRAMING ERROR ");
        *p_receive_error |= (t_uart_receive_status) UART_FRAMING_ERROR;
    }

    if (1 == ((*data_received) & UART_DATA_MASK_BE) >> UART_DATA_SHIFT_BE)
    {
        DBGPRINT(DBGL_HCL_DEV, "BREAK ERROR ");
        *p_receive_error |= (t_uart_receive_status) UART_BREAK_ERROR;
    }

    if (1 == ((*data_received) & UART_DATA_MASK_OE) >> UART_DATA_SHIFT_OE)
    {
        DBGPRINT(DBGL_HCL_DEV, "OVERRUN ERROR ");
        *p_receive_error |= (t_uart_receive_status) UART_OVERRUN_ERROR;
    }

    if (1 == ((*data_received) & UART_DATA_MASK_PE) >> UART_DATA_SHIFT_PE)
    {
        switch (perr_condition)
        {
            case UART_PERR_IGNORE:
                    *temp_flag = TRUE;
                    break;

            case UART_PERR_REPLACECHAR:
                    *data_received = (t_uint32) UART_PARITY_ERROR_CHAR;
                    break;

            case UART_PERR_ENDREAD:
                    *p_receive_error |= (t_uart_receive_status) UART_PARITY_ERROR;
                    break;

            default:
                    DBGPRINT(DBGL_ERROR, "INVALID PARITY ERROR Condition!");
	            DBGEXIT0(UART_INVALID_PARAMETER);
                    return(UART_INVALID_PARAMETER);
        }
    }
    DBGEXIT0(UART_OK);
    return(UART_OK);

}


/****************************************************************************/
/* NAME         :   uart_AnalyseReadChar                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to analyse the Errors during Tx/Rx */
/*                                                                          */
/*  PARAMETERS  :                                                           */
/*          IN  : data_received    : Error Data to analyse                  */
/*              : transfer_flags   : Present Transfer mode                  */
/*              : xoff1_char       : Xoff1 Char set                         */
/*              : xon1_char        : Xon1 Char set                          */
/*              : lp_uart_register : UART Register set                      */
/*              : flow_control     : UART Flow Control set                  */
/*              : uart_device_id   : UART Device                            */
/*              : comm_status      : State of Tx/Rx                         */
/*         OUT  : temp_flag        : Flag if error occured                  */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_AnalyseReadChar(IN t_uint32 data_received, IN t_uint32 transfer_flags, 
		                  IN t_uint8 xoff1_char, IN t_uint8 xon1_char,  IN t_uart_register *lp_uart_register,
				  IN t_uart_flow_control flow_control, IN t_uart_device_id uart_device_id,  
				  IN volatile t_uart_it_communication_status  comm_status, OUT volatile t_bool *temp_flag)
{
    if (UART_SW_FLOW_CONTROL == flow_control)
    {
        *temp_flag = TRUE;
        if (data_received == xoff1_char)
        {
            if ((transfer_flags & UART_TX_IT_MODE) && UART_TRANSMIT_ON_GOING == comm_status.tx_comm_status)
            {
                lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
            }
    	    if (UART_DEVICE_ID_0 == uart_device_id)
            {
#ifndef UART0_AUTO_SW_FLOW_CONTROL
                DBGPRINT(DBGL_HCL_DEV, "Xoff RECEIVED for UART0 ");
                g_uart_system_context.uart0_is_xoff_recd = TRUE;
                g_uart_system_context.uart0_is_xon_recd = FALSE;
#endif /*  UART0_AUTO_SW_FLOW_CONTROL */
            }
            else if (UART_DEVICE_ID_1 == uart_device_id)
            {
#ifndef UART1_AUTO_SW_FLOW_CONTROL
                DBGPRINT(DBGL_HCL_DEV, "Xoff RECEIVED for UART1 ");
                g_uart_system_context.uart1_is_xoff_recd = TRUE;
                g_uart_system_context.uart1_is_xon_recd = FALSE;
#endif /*  UART1_AUTO_SW_FLOW_CONTROL */
            }
            else
            {
#ifndef UART2_AUTO_SW_FLOW_CONTROL
                DBGPRINT(DBGL_HCL_DEV, "Xoff RECEIVED for UART2 ");
                g_uart_system_context.uart2_is_xoff_recd = TRUE;
                g_uart_system_context.uart2_is_xon_recd = FALSE;
#endif /*  UART2_AUTO_SW_FLOW_CONTROL */
            }
        }
        else if (data_received == xon1_char)
        {
            if ((transfer_flags & UART_TX_IT_MODE) && UART_TRANSMIT_ON_GOING == comm_status.tx_comm_status)
            {
                lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
            }
            if (UART_DEVICE_ID_0 == uart_device_id)
            {
#ifndef UART0_AUTO_SW_FLOW_CONTROL
                DBGPRINT(DBGL_HCL_DEV, "Xon RECEIVED for UART0 ");
                g_uart_system_context.uart0_is_xon_recd = TRUE;
                g_uart_system_context.uart0_is_xoff_recd = FALSE;
#endif /*  UART0_AUTO_SW_FLOW_CONTROL */
            }
            else if (UART_DEVICE_ID_1 == uart_device_id)
            {
#ifndef UART1_AUTO_SW_FLOW_CONTROL
                DBGPRINT(DBGL_HCL_DEV, "Xon RECEIVED for UART1 ");
                g_uart_system_context.uart1_is_xon_recd = TRUE;
                g_uart_system_context.uart1_is_xoff_recd = FALSE;
#endif /*  UART1_AUTO_SW_FLOW_CONTROL */
            }
            else
            {
#ifndef UART2_AUTO_SW_FLOW_CONTROL
                DBGPRINT(DBGL_HCL_DEV, "Xon RECEIVED for UART2 ");
                g_uart_system_context.uart2_is_xon_recd = TRUE;
                g_uart_system_context.uart2_is_xoff_recd = FALSE;
#endif /*  UART2_AUTO_SW_FLOW_CONTROL */
            }
        }
    }
}

/****************************************************************************/
/* NAME         :   uart_ReadxCharacters                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to read x characters in UART’s      */
/*                  receive fifo and get corresponding status               */
/*                  UART_IT could stop before complete reading if fiforx    */
/*                  becomes empty or if a receive error occurs              */
/*                  In UART_POLLING mode, if Rx fifo empty: wait then for   */
/*                  incoming data                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id      : Identify UART to configure        */
/*              :   char_number_to_read : Number of characters to be read   */
/*         OUT  :   p_data_char         : Pointer on the reading buffer     */
/*              :   p_char_number_read  : Number of characters read         */
/*              :   p_receive_error     : Error status corresponding to     */
/*                                        data read                         */
/*                                                                          */
/*      RETURN  :   t_uart_error        : UART error code                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_ReadxCharacters
(
    IN t_uart_device_id         uart_device_id,
    IN t_uint32                 char_number_to_read,
    OUT t_uint8                 *p_data_char,
    OUT t_uint32                *p_char_number_read,
    OUT t_uart_receive_status   *p_receive_error
)
{
    t_uint32                        count = 0;  /* Counts number of data read */
    t_uart_error                    error_status = UART_OK;
    t_uint32                        data_received;
    t_uint32                        transfer_flags;
    t_uart_perr_condition           perr_condition;
    t_uart_flow_control             flow_control;
    volatile t_uart_it_communication_status  comm_status;
    t_uint8                         xoff1_char;
    t_uint8                         xon1_char;
    t_uart_register                 *lp_uart_register;
    volatile t_bool temp_flag = FALSE;

    DBGENTER2("UART DEVICE %x, no of chars to READ %x", uart_device_id, char_number_to_read);

    /* Initialisation of receive status */
    *p_receive_error = (t_uart_receive_status) UART_RECEIVE_OK;

    switch (uart_device_id)                     /* Choose suitable UART base adress  */
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            transfer_flags = g_uart_system_context.uart0_transfer_flags;
            perr_condition = g_uart_system_context.uart0_config.perr_condition;
            flow_control = g_uart_system_context.uart0_config.flow_control;
            xoff1_char = g_uart_system_context.uart0_config.xoff1_char;
            xon1_char = g_uart_system_context.uart0_config.xon1_char;
            comm_status = g_uart_system_context.uart0_comm_status;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            transfer_flags = g_uart_system_context.uart1_transfer_flags;
            perr_condition = g_uart_system_context.uart1_config.perr_condition;
            flow_control = g_uart_system_context.uart1_config.flow_control;
            xoff1_char = g_uart_system_context.uart1_config.xoff1_char;
            xon1_char = g_uart_system_context.uart1_config.xon1_char;
            comm_status = g_uart_system_context.uart1_comm_status;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            transfer_flags = g_uart_system_context.uart2_transfer_flags;
            perr_condition = g_uart_system_context.uart2_config.perr_condition;
            flow_control = g_uart_system_context.uart2_config.flow_control;
            xoff1_char = g_uart_system_context.uart2_config.xoff1_char;
            xon1_char = g_uart_system_context.uart2_config.xon1_char;
            comm_status = g_uart_system_context.uart2_comm_status;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    while
    (
        (count < char_number_to_read)
    &&  ((t_uart_receive_status) UART_RECEIVE_OK == *p_receive_error)
    &&  ((FALSE == UART_ISRXFIFOEMPTY()) || (transfer_flags & UART_RX_POLL_MODE))   /* Exit if Rx fifo empty in UART_IT mode (timeout UART_IT) */
    )
    {
        /* Check that Rx Fifo not empty in UART_POLLING mode: in this case, wait for a new byte */
        /* If (transfer_flags & UART_TX_POLL_MODE)*/
        if (transfer_flags & UART_RX_POLL_MODE)        
        {
            DBGPRINT(DBGL_HCL_DEV, "WAIT for RXFIFO Empty in POLL Mode ");
            do
            {
            } while (TRUE == UART_ISRXFIFOEMPTY());
        }

        /* 1 byte reading */
        data_received = lp_uart_register->uartx_dr;

        error_status=uart_AnalyseReadError(&data_received, p_receive_error, &temp_flag, perr_condition);
	if(UART_OK != error_status)
	{
             DBGEXIT0(error_status);
	     return(error_status);
	}
	uart_AnalyseReadChar(data_received, transfer_flags, 
		             xoff1_char, xon1_char, lp_uart_register, flow_control, 
			     uart_device_id, comm_status, &temp_flag);
        
	*(p_data_char + count) = (t_uint8) data_received;

        if (TRUE == temp_flag)
        {
            temp_flag = FALSE;
        }
        else
        {
            count++;
        }
    }

    *p_char_number_read = count;

    /* Update globals related to receive status */
    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        g_uart_system_context.uart0_rx_status = *p_receive_error;
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        g_uart_system_context.uart1_rx_status = *p_receive_error;
    }

    else
    {
        g_uart_system_context.uart2_rx_status = *p_receive_error;
    }
    if ((t_uart_receive_status) UART_RECEIVE_OK != *p_receive_error)
    {
        DBGPRINT(DBGL_ERROR, "UART Receive error detected!! ");
        error_status = UART_RECEIVE_ERROR;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_SendXoff                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   To send XOFF Character                                  */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_SendXoff(IN t_uart_device_id uart_device_id)
{
    t_uart_error        error_status = UART_OK;
    t_uint8             xoff1_char;
    t_uart_mode         uart_mode;
    t_uart_set_or_clear uart_dsr;
    t_uart_set_or_clear uart_dcd;
    t_uart_set_or_clear uart_ri;
    t_uart_set_or_clear uart_cts;
    t_uart_set_or_clear uart_dtr;
    t_uart_register     *lp_uart_register;
    
    DBGENTER1("UART DEVICE (%d)", uart_device_id);
    
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            xoff1_char = g_uart_system_context.uart0_config.xoff1_char;
            uart_mode = g_uart_system_context.uart0_config.uart_mode;
            if (UART_SW_FLOW_CONTROL != g_uart_system_context.uart0_config.flow_control)
            {
                DBGPRINT(DBGL_ERROR, "No SW Flow Control Enabled in SendXoff");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            xoff1_char = g_uart_system_context.uart1_config.xoff1_char;
            uart_mode = g_uart_system_context.uart1_config.uart_mode;
            if (UART_SW_FLOW_CONTROL != g_uart_system_context.uart1_config.flow_control)
            {
                DBGPRINT(DBGL_ERROR, "No SW Flow Control Enabled in SendXoff");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            xoff1_char = g_uart_system_context.uart2_config.xoff1_char;
            uart_mode = g_uart_system_context.uart2_config.uart_mode;
            if (UART_SW_FLOW_CONTROL != g_uart_system_context.uart2_config.flow_control)
            {
                DBGPRINT(DBGL_ERROR, "No SW Flow Control Enabled in SendXoff");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    if (UART_DTE_MODE == uart_mode)
    {
        error_status = UART_DTEGetInputSignals(uart_device_id, &uart_ri, &uart_dsr, &uart_dcd, &uart_cts);
        /*coverity[uninit_use]*/
        if (UART_CLEAR == uart_dsr && UART_CLEAR == uart_dcd)
        {
            /* Assert DTR */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_DTR;
            DBGPRINT(DBGL_ERROR, " Line is not Active / DSR and DCD are down in DTE/ in SendXoff ");
            DBGEXIT0(UART_MODEM_ERROR);
            return(UART_MODEM_ERROR);
        }
    }

    if (UART_DCE_MODE == uart_mode)
    {
        error_status = UART_DCEGetInputSignals(uart_device_id, &uart_dtr, &uart_cts);
        /*coverity[uninit_use]*/
        if (UART_CLEAR == uart_dtr)
        {
            /* Deassert the DSR and quit */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_DTR;
            DBGPRINT(DBGL_ERROR, " Line is not Active / DTR is down in DCE/ in SendXoff ");
            DBGEXIT0(UART_MODEM_ERROR);
            return(UART_MODEM_ERROR);
        }
    }

    DBGPRINT(DBGL_HCL_DEV, "Wait for UART Rx FIFO Empty ");
    do
    {
    } while (TRUE == UART_ISTXFIFOFULL());

    /* 1 byte writing */
    lp_uart_register->uartx_dr = (t_uint32) xoff1_char;

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_SendXon                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   To send XON Character                                   */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_SendXon(IN t_uart_device_id uart_device_id)
{
    t_uart_error        error_status = UART_OK;
    t_uint8             xon1_char;
    t_uart_flow_control flow_control;
    t_uart_mode         uart_mode;
    t_uart_set_or_clear uart_dsr;
    t_uart_set_or_clear uart_dcd;
    t_uart_set_or_clear uart_ri;
    t_uart_set_or_clear uart_cts;
    t_uart_set_or_clear uart_dtr;
    t_uart_register     *lp_uart_register;

    DBGENTER1(" UART DEVICE (%d)", uart_device_id);

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            xon1_char = g_uart_system_context.uart0_config.xon1_char;
            flow_control = g_uart_system_context.uart0_config.flow_control;
            uart_mode = g_uart_system_context.uart0_config.uart_mode;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            xon1_char = g_uart_system_context.uart1_config.xon1_char;
            flow_control = g_uart_system_context.uart1_config.flow_control;
            uart_mode = g_uart_system_context.uart1_config.uart_mode;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            xon1_char = g_uart_system_context.uart2_config.xon1_char;
            flow_control = g_uart_system_context.uart2_config.flow_control;
            uart_mode = g_uart_system_context.uart2_config.uart_mode;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    if (UART_SW_FLOW_CONTROL != flow_control)
    {
        DBGPRINT(DBGL_ERROR, "No SW Flow Control Enabled in SendXon");
        DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
        return(UART_REQUEST_NOT_APPLICABLE);
    }

    if (UART_DTE_MODE == uart_mode)
    {
        error_status = UART_DTEGetInputSignals(uart_device_id, &uart_ri, &uart_dsr, &uart_dcd, &uart_cts);
        /*coverity[uninit_use]*/
        if (UART_CLEAR == uart_dsr && UART_CLEAR == uart_dcd)
        {
            /* Assert DTR */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_DTR;
            DBGPRINT(DBGL_ERROR, " Line is not Active / DSR and DCD are down in DTE/ in SendXon ");
            DBGEXIT0(UART_MODEM_ERROR);
            return(UART_MODEM_ERROR);
        }
    }

    if (UART_DCE_MODE == uart_mode)
    {
        error_status = UART_DCEGetInputSignals(uart_device_id, &uart_dtr, &uart_cts);
        /*coverity[uninit_use]*/
        if (UART_CLEAR == uart_dtr)
        {
            /* Deassert the DSR and quit */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_DTR;
            DBGPRINT(DBGL_ERROR, " Line is not Active / DTR is down in DCE/ in SendXon ");
            DBGEXIT0(UART_MODEM_ERROR);
            return(UART_MODEM_ERROR);
        }
    }
    
    DBGPRINT(DBGL_HCL_DEV, "Wait for UART Tx FIFO Empty ");
    do
    {
    } while (TRUE == UART_ISTXFIFOFULL());

    /* 1 byte writing */
    lp_uart_register->uartx_dr = (t_uint32) xon1_char;

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_GetFlowControlStatus                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine gives status of whether Xon/Xoff Character */
/*                  is received                                             */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   p_xoff_char     : Pointer to the boolean value          */
/*                                    representing the reception of Xoff    */
/*                                    Character                             */
/*              :   p_xon_char      : Pointer to the boolean value          */
/*                                    representing the reception of Xon     */
/*                                    Character                             */
/*                                                                          */
/*      RETURN  :    t_uart_error   : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_GetFlowControlStatus
(
    IN t_uart_device_id uart_device_id,
    OUT t_bool          *p_xoff_char,
    OUT t_bool          *p_xon_char
)
{
    t_uart_error        error_status = UART_OK;
    t_uart_flow_control flow_control;
    DBGENTER1(" UART DEVICE %d ", uart_device_id);

    if (NULL == p_xoff_char || NULL == p_xon_char)
    {
        DBGPRINT(DBGL_ERROR, "OUT parameters NULL ");
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            flow_control = g_uart_system_context.uart0_config.flow_control;
            break;

        case UART_DEVICE_ID_1:
            flow_control = g_uart_system_context.uart1_config.flow_control;
            break;

        case UART_DEVICE_ID_2:
            flow_control = g_uart_system_context.uart2_config.flow_control;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    if (UART_SW_FLOW_CONTROL != flow_control)
    {
        DBGPRINT(DBGL_ERROR, "No SW Flow Control Enabled in GetFlowControlStatus");
        DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
        return(UART_REQUEST_NOT_APPLICABLE);
    }

    if (UART_DEVICE_ID_0 == uart_device_id)
    {
#ifndef UART0_AUTO_SW_FLOW_CONTROL
        *p_xoff_char = g_uart_system_context.uart0_is_xoff_recd;
        *p_xon_char = g_uart_system_context.uart0_is_xon_recd;
#endif /* UART0_AUTO_SW_FLOW_CONTROL */
#ifdef UART0_AUTO_SW_FLOW_CONTROL
        if(UART_IRQ_SRC_XOFF == (UART_IRQ_SRC_XOFF & g_uart_system_context.p_uart0_register->uartx_ris))
        {
            *p_xoff_char = TRUE;
            *p_xon_char = FALSE;
        } /* end if */
        else
        {
            *p_xoff_char = FALSE;
            *p_xon_char = TRUE;
        } /* end else */
#endif /* UART0_AUTO_SW_FLOW_CONTROL */
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
#ifndef UART1_AUTO_SW_FLOW_CONTROL
        *p_xoff_char = g_uart_system_context.uart1_is_xoff_recd;
        *p_xon_char = g_uart_system_context.uart1_is_xon_recd;
#endif /* UART1_AUTO_SW_FLOW_CONTROL */
#ifdef UART1_AUTO_SW_FLOW_CONTROL
        if(UART_IRQ_SRC_XOFF == (UART_IRQ_SRC_XOFF & g_uart_system_context.p_uart1_register->uartx_ris))
        {
            *p_xoff_char = TRUE;
            *p_xon_char = FALSE;
        } /* end if */
        else
        {
            *p_xoff_char = FALSE;
            *p_xon_char = TRUE;
        } /* end else */
#endif /* UART1_AUTO_SW_FLOW_CONTROL */
    }

    else
    {
#ifndef UART2_AUTO_SW_FLOW_CONTROL
        *p_xoff_char = g_uart_system_context.uart2_is_xoff_recd;
        *p_xon_char = g_uart_system_context.uart2_is_xon_recd;
#endif /* UART2_AUTO_SW_FLOW_CONTROL */
#ifdef UART2_AUTO_SW_FLOW_CONTROL
        if(UART_IRQ_SRC_XOFF == (UART_IRQ_SRC_XOFF & g_uart_system_context.p_uart2_register->uartx_ris))
        {
            *p_xoff_char = TRUE;
            *p_xon_char = FALSE;
        } /* end if */
        else
        {
            *p_xoff_char = FALSE;
            *p_xon_char = TRUE;
        } /* end else */
#endif /* UART2_AUTO_SW_FLOW_CONTROL */
    }

    DBGEXIT2(error_status, "Xoff Char %d, Xon Char %d", *p_xoff_char, *p_xon_char);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_BytesReceived                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine returns the number of bytes received from  */
/*                  the interface                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   p_bytes         : Bytes received                        */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_BytesReceived(IN t_uart_device_id uart_device_id, OUT t_uint32 *p_bytes)
{
    DBGENTER1("UART DEVICE ( %d )", uart_device_id);
    if (NULL == p_bytes)
    {
        DBGPRINT(DBGL_ERROR, "OUT parameter NULL ");
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        *p_bytes = g_uart_system_context.uart0_nrx;
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        *p_bytes = g_uart_system_context.uart1_nrx;
    }

    else if (UART_DEVICE_ID_2 == uart_device_id)
    {
        *p_bytes = g_uart_system_context.uart2_nrx;
    }
    else
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   UART_BytesTransmitted                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine returns the number of bytes transmitted to */
/*                  the interface                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   p_bytes         : Bytes transmitted                     */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_BytesTransmitted(IN t_uart_device_id uart_device_id, OUT t_uint32 *p_bytes)
{
    DBGENTER1("UART DEVICE ( %d )", uart_device_id);
    if (NULL == p_bytes)
    {
        DBGPRINT(DBGL_ERROR, "OUT parameter NULL ");
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        *p_bytes = g_uart_system_context.uart0_ntx;
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        *p_bytes = g_uart_system_context.uart1_ntx;
    }

    else if (UART_DEVICE_ID_2 == uart_device_id)
    {
        *p_bytes = g_uart_system_context.uart2_ntx;
    }
    else
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   UART_ReceiveCancel                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   To cancel the receiving operation                       */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_ReceiveCancel(IN t_uart_device_id uart_device_id)
{
    t_uart_error    error_status = UART_OK;
    DBGENTER1("UART DEVICE (%d)", uart_device_id);
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            if (UART_RECEIVE_ON_GOING == g_uart_system_context.uart0_comm_status.rx_comm_status)
            {
                g_uart_system_context.p_uart0_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
                g_uart_system_context.uart0_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
            }
            break;

        case UART_DEVICE_ID_1:
            if (UART_RECEIVE_ON_GOING == g_uart_system_context.uart1_comm_status.rx_comm_status)
            {
                g_uart_system_context.p_uart1_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
                g_uart_system_context.uart1_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
            }
            break;

        case UART_DEVICE_ID_2:
            if (UART_RECEIVE_ON_GOING == g_uart_system_context.uart2_comm_status.rx_comm_status)
            {
                g_uart_system_context.p_uart2_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
                g_uart_system_context.uart2_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
            }
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            error_status = UART_INVALID_PARAMETER;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_TransmitCancel                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   To cancel the transmit operation                        */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error: UART error code                           */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_TransmitCancel(IN t_uart_device_id uart_device_id)
{
    t_uart_error    error_status = UART_OK;
    DBGENTER1("UART DEVICE (%d)", uart_device_id);

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart0_comm_status.tx_comm_status)
            {
                g_uart_system_context.p_uart0_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
                g_uart_system_context.uart0_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
            }
            break;

        case UART_DEVICE_ID_1:
            if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart1_comm_status.tx_comm_status)
            {
                g_uart_system_context.p_uart1_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
                g_uart_system_context.uart1_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
            }
            break;

        case UART_DEVICE_ID_2:
            if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart2_comm_status.tx_comm_status)
            {
                g_uart_system_context.p_uart2_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
                g_uart_system_context.uart2_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
            }
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            error_status = UART_INVALID_PARAMETER;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_GetReceiveStatus                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine provides a status on the receive of the    */
/*                  last transfer                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   p_rx_status     : Give status on last receive transfer  */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_GetReceiveStatus(IN t_uart_device_id uart_device_id, OUT t_uart_receive_status *p_rx_status)
{
    t_uart_error    error_status = UART_OK;
    DBGENTER1(" UART DEVICE (%d)", uart_device_id);
    if (NULL == p_rx_status)
    {
        DBGPRINT(DBGL_ERROR, "OUT parameter empty ");
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    switch (uart_device_id) /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            *p_rx_status = g_uart_system_context.uart0_rx_status;
            break;

        case UART_DEVICE_ID_1:
            *p_rx_status = g_uart_system_context.uart1_rx_status;
            break;

        case UART_DEVICE_ID_2:
            *p_rx_status = g_uart_system_context.uart2_rx_status;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGEXIT1(error_status, " RX Status (%x)", *p_rx_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   UART_GetComStatus                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine provides a status on the end of the last   */
/*                  transmit operation                                      */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   p_comm_status   : Give status on last transmit. This    */
/*                                    parameter has only a role of          */
/*                                    exporting the status outside the HCL  */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_GetComStatus
(
    IN t_uart_device_id                 uart_device_id,
    OUT t_uart_it_communication_status  *p_comm_status
)
{
    t_uart_error                    error_status = UART_OK;
    t_uart_it_communication_status  *lp_comm_status = NULL;
    t_uart_register                 *lp_uart_register;
    DBGENTER1(" UART DEVICE (%d)", uart_device_id);
     
    if (NULL == p_comm_status)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    switch (uart_device_id) /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            lp_comm_status = &g_uart_system_context.uart0_comm_status;
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_comm_status = &g_uart_system_context.uart1_comm_status;
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_comm_status = &g_uart_system_context.uart2_comm_status;
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    if (UART_TRANSMIT_ENDED != lp_comm_status->tx_comm_status)
    {
        if ((UART_WRITTINGS_ENDED == (lp_comm_status->tx_comm_status)) && (TRUE == UART_ISTXFIFOEMPTY()))
        {
            lp_comm_status->tx_comm_status = UART_TRANSMIT_ENDED;
        }
    }

    *p_comm_status = *lp_comm_status;
    DBGEXIT1(error_status, " Comm Status (%x)", (t_uint32) p_comm_status);
    return(error_status);
}


/****************************************************************************/
/* NAME         :   uart_GetTransmitInternalData                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine is used to extract the UART internal system  */
/*                context data during transmission                          */
/*  PARAMETERS  :                                                           */
/*          IN  : uart_device_id       : UART Device                        */
/*              : num_of_char_to_be_tx : chars to be Tx                     */
/*              : p_data_char          : Location of Tx Buffer              */
/*         OUT  : transfer_flags       : Present Transfer mode              */
/*              : uart_fifo_size       : Total FIFO size                    */
/*              : lp_uart_register     : UART Register set                  */
/*              : uart_mode            : UART Mode Set                      */
/*                                                                          */
/*      RETURN  : t_uart_error                                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_GetTransmitInternalData(IN t_uart_device_id uart_device_id, volatile OUT t_uart_mode *uart_mode, 
		                                  volatile OUT t_uint8 *uart_fifo_size, OUT t_uint32 *transfer_flags, 
				                  OUT t_uart_register **lp_uart_register, IN t_uint32 num_of_char_to_be_tx,
				                  IN  t_uint8 *p_data_char)
{
    switch (uart_device_id)         /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            {
                if (TRUE == g_uart_system_context.uart0_need_to_call_config)
                {
                    DBGPRINT(DBGL_ERROR, " Need to call UART_SetConfig Error");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                *lp_uart_register = g_uart_system_context.p_uart0_register;
                if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart0_comm_status.tx_comm_status)
                {
                    DBGPRINT(DBGL_ERROR, " Last transmit not achieved Error");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                /* If SW Flow control and XOFF Character received, then avoid transmission */
                if
                (
                    UART_SW_FLOW_CONTROL == g_uart_system_context.uart0_config.flow_control
                &&  TRUE == g_uart_system_context.uart0_is_xoff_recd
                )
                {
                    DBGPRINT(DBGL_ERROR, " SW Flowed Off /XOFF Character Recd/ in TransmitxChar ");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                *uart_mode = g_uart_system_context.uart0_config.uart_mode;
                g_uart_system_context.uart0_number_of_char_to_be_tx = num_of_char_to_be_tx; /* Keep data in static global: */
                g_uart_system_context.p_uart0_transmit_buffer = p_data_char;                /* Bytes number and pointer on first byte */
                *uart_fifo_size = UART0_FIFO_SIZE;
                g_uart_system_context.uart0_ntx = 0;    /* Initialize current byte number */
                *transfer_flags = g_uart_system_context.uart0_transfer_flags;
                break;
            }

        case UART_DEVICE_ID_1:
            {
                if (TRUE == g_uart_system_context.uart1_need_to_call_config)
                {
                    DBGPRINT(DBGL_ERROR, " Need to call UART_SetConfig Error");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                *lp_uart_register = g_uart_system_context.p_uart1_register;
                if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart1_comm_status.tx_comm_status)
                {
                    DBGPRINT(DBGL_ERROR, " Last transmit not achieved Error");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                /* If SW Flow control and XOFF Character received, then avoid transmission */
                if
                (
                    UART_SW_FLOW_CONTROL == g_uart_system_context.uart1_config.flow_control
                &&  TRUE == g_uart_system_context.uart1_is_xoff_recd
                )
                {
                    DBGPRINT(DBGL_ERROR, " SW Flowed Off /XOFF Character Recd/ in TransmitxChar ");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                *uart_mode = g_uart_system_context.uart1_config.uart_mode;
                g_uart_system_context.uart1_number_of_char_to_be_tx = num_of_char_to_be_tx; /* Keep data in static global: */
                g_uart_system_context.p_uart1_transmit_buffer = p_data_char;                /* Bytes number and pointer on first byte */
                *uart_fifo_size = UART1_FIFO_SIZE;
                g_uart_system_context.uart1_ntx = 0;    /* Initialize current byte number */
                *transfer_flags = g_uart_system_context.uart1_transfer_flags;
                break;
            }


        case UART_DEVICE_ID_2:
            {
                if (TRUE == g_uart_system_context.uart2_need_to_call_config)
                {
                    DBGPRINT(DBGL_ERROR, " Need to call UART_SetConfig Error");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                *lp_uart_register = g_uart_system_context.p_uart2_register;
                if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart2_comm_status.tx_comm_status)
                {
                    DBGPRINT(DBGL_ERROR, " Last transmit not achieved Error");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                /* If SW Flow control and XOFF Character received, then avoid transmission */
                if
                (
                    UART_SW_FLOW_CONTROL == g_uart_system_context.uart2_config.flow_control
                &&  TRUE == g_uart_system_context.uart2_is_xoff_recd
                )
                {
                    DBGPRINT(DBGL_ERROR, " SW Flowed Off /XOFF Character Recd/ in TransmitxChar ");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                *uart_mode = g_uart_system_context.uart2_config.uart_mode;
                g_uart_system_context.uart2_number_of_char_to_be_tx = num_of_char_to_be_tx; /* Keep data in static global: */
                g_uart_system_context.p_uart2_transmit_buffer = p_data_char;                /* Bytes number and pointer on first byte */
                *uart_fifo_size = UART2_FIFO_SIZE;
                g_uart_system_context.uart2_ntx = 0;                    /* Initialize current byte number */
                *transfer_flags = g_uart_system_context.uart2_transfer_flags;
                break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }
    
    DBGEXIT0(UART_OK);
    return(UART_OK);
}


/****************************************************************************/
/* NAME         :   uart_TransmitItMode                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine is used to transmit in IT mode               */
/*                                                                          */
/*  PARAMETERS  :                                                           */
/*          IN  : uart_device_id       : UART Device                        */
/*              : num_of_char_to_be_tx : chars to be Tx                     */
/*              : p_data_char          : Location of Tx Buffer              */
/*              : lp_uart_register     : UART Register Set                  */
/*         OUT  : None                                                      */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE void uart_TransmitItMode(IN t_uart_register *lp_uart_register, IN t_uart_device_id uart_device_id,   
		                 IN t_uint8 *p_data_char, IN t_uint32 num_of_char_to_be_tx)
{
    t_uart_error error_status;
    t_uint32            first_transmit = 0;

    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_TXEN;               /* disable TX */

    /* Initiates transfer by a full fifo writing */
    error_status = uart_FillFifoTx(uart_device_id, p_data_char, num_of_char_to_be_tx, &first_transmit);

    /* Update bytes number to be tranmitted by UART_IT (processed by Process_It) */
    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        g_uart_system_context.uart0_comm_status.tx_comm_status = UART_TRANSMIT_ON_GOING;
        g_uart_system_context.uart0_ntx = g_uart_system_context.uart0_ntx + first_transmit;
        if (g_uart_system_context.uart0_number_of_char_to_be_tx == g_uart_system_context.uart0_ntx) /* If all writings covered by the fifo filling */
        {   /* ->transfer's end */
             g_uart_system_context.uart0_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
        }
        else
        {
            /* UART_CLEAR Transmit and UnderRun Error Int */
            lp_uart_register->uartx_icr |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);

            /* Enable Transmit Int and Underrun Error Int*/
            lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
        }
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        g_uart_system_context.uart1_comm_status.tx_comm_status = UART_TRANSMIT_ON_GOING;
        g_uart_system_context.uart1_ntx = g_uart_system_context.uart1_ntx + first_transmit;
        if (g_uart_system_context.uart1_number_of_char_to_be_tx == g_uart_system_context.uart1_ntx) /* If all writings covered by the fifo filling */
        {   /* ->transfer's end */
            g_uart_system_context.uart1_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
        }
        else
        {
            /* UART_CLEAR Transmit and underrun error Int */
            lp_uart_register->uartx_icr |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);

            /* Enable Transmit and underrun error Int*/
            lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
         }
    }

    else
    {
         g_uart_system_context.uart2_comm_status.tx_comm_status = UART_TRANSMIT_ON_GOING;
         g_uart_system_context.uart2_ntx = g_uart_system_context.uart2_ntx + first_transmit;
         if (g_uart_system_context.uart2_number_of_char_to_be_tx == g_uart_system_context.uart2_ntx) /* If all writings covered by the fifo filling */
         {   /* ->transfer's end */
             g_uart_system_context.uart2_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
         }
         else
         {
             /* UART_CLEAR Transmit and underrun error Int */
             lp_uart_register->uartx_icr |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);

             /* Enable Transmit and underrun error Int */
             lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
         }
    }
    
    /* Start transfer */
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_TXEN;   /* enable TX */

    /*coverity[self_assign]*/
    error_status = error_status; 
    return;
}


/****************************************************************************/
/* NAME         :   uart_enh_TransmitxCharacters                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to send x characters through UARTx  */
/*                  as a enhanced routine.                                  */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id      : Identify UART to configure        */
/*              :   num_of_char_to_be_tx: Number of characters to be        */
/*                                        transmitted                       */
/*              :   p_data_char         : Pointer on the first character to */
/*                                        transmit                          */
/*                                                                          */
/*      RETURN  :   t_uart_error        : UART error code                   */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_enh_TransmitxCharacters
(
    IN t_uart_device_id uart_device_id,
    IN t_uint32         num_of_char_to_be_tx,
    IN t_uint8          *p_data_char
)
{
    t_uart_register     *lp_uart_register = NULL;
    t_uart_error        error_status = UART_OK;
    t_uint32            ntx = 0;    /* Byte writing number already performed */
    t_uint32            num_of_char_to_be_written = 0;
    t_uint32            num_of_char_written = 0;
    t_uint32            transfer_flags =0x00;
    volatile t_uint8    uart_fifo_size = 0x00;
    volatile t_uart_mode uart_mode;
    t_uart_set_or_clear uart_dsr;
    t_uart_set_or_clear uart_dcd;
    t_uart_set_or_clear uart_ri;
    t_uart_set_or_clear uart_cts;
    t_uart_set_or_clear uart_dtr;

    DBGENTER3(" UART DEVICE %d, CHARS TX %x, p_data_char %x", uart_device_id, num_of_char_to_be_tx, (t_uint32) p_data_char);

    if (NULL == p_data_char)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    error_status = uart_GetTransmitInternalData(uart_device_id, &uart_mode, &uart_fifo_size, 
		                                 &transfer_flags, &lp_uart_register, num_of_char_to_be_tx, p_data_char);
    if(UART_OK != error_status)
    {
        DBGEXIT0(error_status);
        return(error_status);
    }

    /* IF DTE mode and both DSR and DCD are deasserted then return without transmission */
    if (UART_DTE_MODE == uart_mode)
    {
        error_status = UART_DTEGetInputSignals(uart_device_id, &uart_ri, &uart_dsr, &uart_dcd, &uart_cts);
        /*coverity[uninit_use]*/
        if (UART_CLEAR == uart_dsr && UART_CLEAR == uart_dcd)
        {
            /* Deassert DTR */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_DTR;
            DBGPRINT(DBGL_ERROR, " Line is not Active / DSR and DCD are down in DTE/ in TransmitxChar ");
            DBGEXIT0(UART_MODEM_ERROR);
            return(UART_MODEM_ERROR);
        }
    }

    /* If DCE mode and DTR is deasserted then return without transmission */
    if (UART_DCE_MODE == uart_mode)
    {
        error_status = UART_DCEGetInputSignals(uart_device_id, &uart_dtr, &uart_cts);
        /*coverity[uninit_use]*/
        if (UART_CLEAR == uart_dtr)
        {
            /* Deassert the DSR and quit */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_DTR;
            DBGPRINT(DBGL_ERROR, " Line is not Active / DTR is down in DCE/ in TransmitxChar ");
            DBGEXIT0(UART_MODEM_ERROR);
            return(UART_MODEM_ERROR);
        }
    }

    switch (transfer_flags & (UART_TX_IT_MODE | UART_TX_DMA_MODE | UART_TX_POLL_MODE))
    {
        case UART_TX_POLL_MODE:                                         /* All transfer managed by the function: performs suitable number of writing task */
            {
                do
                {
                    if (num_of_char_to_be_tx < uart_fifo_size)
                    {
                        num_of_char_to_be_written = num_of_char_to_be_tx;
                    }
                    else
                    {
                        if (ntx <= (num_of_char_to_be_tx - uart_fifo_size))
                        {
                            num_of_char_to_be_written = uart_fifo_size; /* Number of characters to transmit in the next writing */
                        }
                        else
                        {
                            num_of_char_to_be_written = (num_of_char_to_be_tx - ntx);
                        }
                    }

                    error_status = uart_WritexCharacters(uart_device_id, p_data_char + ntx, num_of_char_to_be_written, &num_of_char_written);
                    ntx = ntx + num_of_char_written;
                } while (ntx < num_of_char_to_be_tx);
                break;
            }

        case UART_TX_IT_MODE:
            uart_TransmitItMode(lp_uart_register, uart_device_id, p_data_char, num_of_char_to_be_tx);

            break;

        case UART_TX_DMA_MODE:
            {
                DBGPRINT(DBGL_ERROR, " call UART_transmit in UART_DMA mode Error");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

        /* Transfer should be managed by UART_DMA HCL */
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGEXIT0(error_status);
    return(error_status);
}


/****************************************************************************/
/* NAME         :   uart_CheckDCEDTEAssert                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine checks If DTE mode and both DSR and DCD are  */
/*                asserted and processes henceforth                         */
/*  PARAMETERS  :                                                           */
/*          IN  : uart_device_id     : UART Device                          */
/*              : transfer_flags     : Flags in Tx/Rx (IT/Poll/DMA)         */
/*              : uart_mode          : Mode of UART                         */
/*              : lp_uart_register   : UART Register Set                    */
/*         OUT  : p_comm_status      : Final Status of Tx/Rx                */
/*                                                                          */
/*      RETURN  : t_uart_error                                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/    
PRIVATE t_uart_error uart_CheckDCEDTEAssert(IN t_uart_mode uart_mode, IN t_uart_device_id uart_device_id, 
		                            OUT t_uart_it_communication_status  *p_comm_status, IN t_uint32 transfer_flags,
	                                    IN t_uart_register *lp_uart_register)
{
    t_uart_set_or_clear             uart_dsr;
    t_uart_set_or_clear             uart_dcd;
    t_uart_set_or_clear             uart_ri;
    t_uart_set_or_clear             uart_cts;
    t_uart_set_or_clear             uart_dtr;
    t_uart_error error_status=UART_OK;

    if (UART_DTE_MODE == uart_mode)
    {
        error_status = UART_DTEGetInputSignals(uart_device_id, &uart_ri, &uart_dsr, &uart_dcd, &uart_cts);
	if(UART_OK!=error_status)
	{
            DBGEXIT0(error_status);
            return(error_status);
	}
        if (UART_CLEAR == uart_dsr && UART_CLEAR == uart_dcd)
        {
            if (transfer_flags & UART_RX_IT_MODE)
            {
                p_comm_status->rx_comm_status = UART_RECEIVE_ENDED;
            }

            /* Deassert DTR and quit */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_DTR;
            DBGPRINT(DBGL_ERROR, " Line is not Active / DSR and DCD are down in DTE/ in ReceivexChar ");
            DBGEXIT0(UART_MODEM_ERROR);
            return(UART_MODEM_ERROR);
        }
    }

    /* If DCE mode and DTR is deasserted then return without reception */
    if (UART_DCE_MODE == uart_mode)
    {
        error_status = UART_DCEGetInputSignals(uart_device_id, &uart_dtr, &uart_cts);
	if(UART_OK!=error_status)
	{
            DBGEXIT0(error_status);
            return(error_status);
	}
	if (UART_CLEAR == uart_dtr)
        {
            if (transfer_flags & UART_RX_IT_MODE)
            {
                p_comm_status->rx_comm_status = UART_RECEIVE_ENDED;
            }

            /* Deassert the DSR and quit */
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_DTR;
            DBGPRINT(DBGL_ERROR, " Line is not Active / DTR is down in DCE/ in ReceivexChar ");
            DBGEXIT0(UART_MODEM_ERROR);
            return(UART_MODEM_ERROR);
        }
    }

    DBGEXIT0(UART_OK);
    return(UART_OK);
}


/****************************************************************************/
/* NAME         :   uart_DoRecieve                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine is used for Receiving characters on UART     */
/*                                                                          */
/*  PARAMETERS  :                                                           */
/*          IN  : uart_device_id         : UART Device                      */
/*              : transfer_flags         : Flags in Tx/Rx (IT/Poll/DMA)     */
/*              : uart_fifo_size         : size of UART FIFO                */
/*              : lp_uart_register       : UART Register Set                */
/*              : num_of_char_to_be_recd : No of chars to be read           */
/*         OUT  : p_num_of_char_recd     : Final Status of Tx/Rx            */
/*              : p_receive_error        : Errors during Recieve operation  */
/*              : p_data_char            : Chars Received                   */
/*                                                                          */
/*      RETURN  : t_uart_error                                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/  
PRIVATE t_uart_error uart_DoRecieve(IN t_uart_device_id uart_device_id, IN t_uart_register *lp_uart_register, 
		                    IN t_uint32 transfer_flags, IN t_uint8 uart_fifo_size, OUT t_uint8 *p_data_char, 
				    IN t_uint32 num_of_char_to_be_recd, OUT t_uart_receive_status   *p_receive_error,
				    OUT t_uint32 *p_num_of_char_recd)
{
    t_uint32 nrx=0x0, char_number_to_read=0x0, char_number_read=0x00; 
    t_uart_error error_status = UART_OK;

    switch (transfer_flags & (UART_RX_IT_MODE | UART_RX_DMA_MODE | UART_RX_POLL_MODE))
    {
        case UART_RX_POLL_MODE:
            {
                do
                {
                    if (num_of_char_to_be_recd >= nrx + uart_fifo_size)
                    {
                        char_number_to_read = uart_fifo_size;   /* Number of characters to receive in the next reading */
                    }
                    else
                    {
                        char_number_to_read = (num_of_char_to_be_recd - nrx);
                    }

                    error_status = uart_ReadxCharacters
                        (
                            uart_device_id,
                            char_number_to_read,
                            p_data_char + nrx,
                            &char_number_read,
                            p_receive_error
                        );
	            if(UART_OK!=error_status)
	            {
                         DBGEXIT0(error_status);
                         return(error_status);
                    } 
		    nrx = nrx + char_number_read;
                } while ((nrx < num_of_char_to_be_recd) && ((t_uart_receive_status) UART_RECEIVE_OK == *p_receive_error));

                /* Informs about number of bytes effectively read */
                *p_num_of_char_recd = nrx;
                break;
            }

        case UART_RX_IT_MODE:   /* Enable Receive and receive timeout UART_IT */
            lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
            break;              /* Transfer managed by Process_It */

        case UART_RX_DMA_MODE:
            {
                DBGPRINT(DBGL_ERROR, " call UART_transmit in UART_DMA mode Error");
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    /* If receive error then initiate setconfiguration again */
    if ((t_uart_receive_status) UART_RECEIVE_OK != *p_receive_error)
    {
        if (UART_DEVICE_ID_0 == uart_device_id)
        {
            g_uart_system_context.uart0_need_to_call_config = TRUE;
        }
        else if (UART_DEVICE_ID_1 == uart_device_id)
        {
            g_uart_system_context.uart1_need_to_call_config = TRUE;
        }

        else
        {
            g_uart_system_context.uart2_need_to_call_config = TRUE;
        }
        DBGPRINT(DBGL_ERROR, " Receive Error");
        DBGEXIT0(UART_RECEIVE_ERROR);
        return(UART_RECEIVE_ERROR);
    }
    DBGEXIT0(UART_OK);
    return(UART_OK);
}


/****************************************************************************/
/* NAME         :   uart_enh_ReceivexCharacters                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to receive x characters through     */
/*                  UARTx as a enhanced    routine.                         */
/*                                                                          */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id          : Identify UART to configure    */
/*              :   num_of_char_to_be_recd  : number of characters to be    */
/*                                            received                      */
/*         OUT  :   p_data_char             : pointer on the first buffer   */
/*                                            byte                          */
/*                                                                          */
/*              :   p_num_of_char_recd      : number of characters really   */
/*                                            received                      */
/*              :   p_receive_error         : error status related to       */
/*                                            receive data                  */
/*                                                                          */
/*      RETURN  :   t_uart_error            : UART error code               */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_enh_ReceivexCharacters
(
    IN t_uart_device_id         uart_device_id,
    IN t_uint32                 num_of_char_to_be_recd,
    OUT t_uint8                 *p_data_char,
    OUT t_uint32                *p_num_of_char_recd,
    OUT t_uart_receive_status   *p_receive_error
)
{
    t_uart_error                    error_status = UART_OK;
    t_uint8                         uart_fifo_size = 0;
    t_uart_flow_control             flow_control;
    t_uart_mode                     uart_mode;
    t_uart_it_communication_status  *p_comm_status;
    t_uart_register                 *lp_uart_register;
    t_uint32                        transfer_flags;

    DBGENTER3("UART DEVICE %d, NO CHARS To RX %x, p_data_char %x", uart_device_id, num_of_char_to_be_recd, (t_uint32) p_data_char);

    if (NULL == p_data_char || NULL == p_num_of_char_recd || NULL == p_receive_error)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    *p_receive_error = (t_uart_receive_status) UART_RECEIVE_OK;

    switch (uart_device_id) /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            {
                if (TRUE == g_uart_system_context.uart0_need_to_call_config)
                {
                    DBGPRINT(DBGL_ERROR, " Need to call UART_SetConfig Error");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                lp_uart_register = g_uart_system_context.p_uart0_register;
                flow_control = g_uart_system_context.uart0_config.flow_control;
                uart_mode = g_uart_system_context.uart0_config.uart_mode;
                if (UART_RECEIVE_ENDED != g_uart_system_context.uart0_comm_status.rx_comm_status)
                {
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                p_comm_status = &g_uart_system_context.uart0_comm_status;
                if (g_uart_system_context.uart0_transfer_flags & UART_RX_IT_MODE)
                {
                    g_uart_system_context.uart0_comm_status.rx_comm_status = UART_RECEIVE_ON_GOING;
                }

                g_uart_system_context.uart0_number_of_char_to_be_rx = num_of_char_to_be_recd;   
                g_uart_system_context.p_uart0_receive_buffer = p_data_char;     /* Bytes number and pointer on first byte */
                uart_fifo_size = UART0_FIFO_SIZE;
                g_uart_system_context.uart0_nrx = 0;
                transfer_flags = g_uart_system_context.uart0_transfer_flags;
                break;
            }

        case UART_DEVICE_ID_1:
            {
                if (TRUE == g_uart_system_context.uart1_need_to_call_config)
                {
                    DBGPRINT(DBGL_ERROR, " Need to call UART_SetConfig Error");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                lp_uart_register = g_uart_system_context.p_uart1_register;
                uart_mode = g_uart_system_context.uart1_config.uart_mode;
                flow_control = g_uart_system_context.uart1_config.flow_control;
                if (UART_RECEIVE_ENDED != g_uart_system_context.uart1_comm_status.rx_comm_status)
                {
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                p_comm_status = &g_uart_system_context.uart1_comm_status;
                if (g_uart_system_context.uart1_transfer_flags & UART_RX_IT_MODE)
                {
                    g_uart_system_context.uart1_comm_status.rx_comm_status = UART_RECEIVE_ON_GOING;
                }

                g_uart_system_context.uart1_number_of_char_to_be_rx = num_of_char_to_be_recd;   
                g_uart_system_context.p_uart1_receive_buffer = p_data_char;    /* Bytes number and pointer on first byte */
                uart_fifo_size = UART1_FIFO_SIZE;
                g_uart_system_context.uart1_nrx = 0;    /* Initialize current byte number */
                transfer_flags = g_uart_system_context.uart1_transfer_flags;
                break;
            }


        case UART_DEVICE_ID_2:
            {
                if (TRUE == g_uart_system_context.uart2_need_to_call_config)
                {
                    DBGPRINT(DBGL_ERROR, " Need to call UART_SetConfig Error");
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                lp_uart_register = g_uart_system_context.p_uart2_register;
                uart_mode = g_uart_system_context.uart2_config.uart_mode;
                flow_control = g_uart_system_context.uart2_config.flow_control;
                if (UART_RECEIVE_ENDED != g_uart_system_context.uart2_comm_status.rx_comm_status)
                {
                    DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                    return(UART_REQUEST_NOT_APPLICABLE);
                }

                p_comm_status = &g_uart_system_context.uart2_comm_status;
                if (g_uart_system_context.uart2_transfer_flags & UART_RX_IT_MODE)
                {
                    g_uart_system_context.uart2_comm_status.rx_comm_status = UART_RECEIVE_ON_GOING;
                }

                g_uart_system_context.uart2_number_of_char_to_be_rx = num_of_char_to_be_recd;   
                g_uart_system_context.p_uart2_receive_buffer = p_data_char;    /* Bytes number and pointer on first byte */
                uart_fifo_size = UART2_FIFO_SIZE;
                g_uart_system_context.uart2_nrx = 0;            /* Initialize current byte number */
                transfer_flags = g_uart_system_context.uart2_transfer_flags;
                break;
            }

        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    /* If DTE mode and both DSR and DCD are deasserted then return without reception */
    error_status = uart_CheckDCEDTEAssert(uart_mode, uart_device_id, p_comm_status, transfer_flags, lp_uart_register);
    if(UART_OK != error_status)
    {
         DBGEXIT0(error_status);
         return(error_status);         
    }

    if (UART_HW_FLOW_CONTROL == flow_control)                   /* Program Flow Control: HW */
    {
        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_RTS;
    }

    error_status = uart_DoRecieve(uart_device_id, lp_uart_register, transfer_flags, uart_fifo_size, p_data_char, 
		                   num_of_char_to_be_recd, p_receive_error, p_num_of_char_recd);

    DBGEXIT2(error_status, " No of Char Recieved %x, Receive Error %x", *p_num_of_char_recd, *p_receive_error);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   uart_enh_PowerOn                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This function is called by the upper layer to handle    */
/*                  the power on mode during power management by            */
/*                  reconfiguring the UARTs. UART_IT should be called after */
/*                  PowerOff. It is an enhanced routine.                    */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : The uart number                       */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   UART_REQUEST_NOT_APPLICABLE : When uart_elem_PowerOn is */
/*                                                called despite UART_IT is */
/*                                                in the power on mode.     */
/*              :   UART_OK                     : Otherwise                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Re-entrant                                         */
/* REENTRANCY ISSUES:    No Issues                                          */
/****************************************************************************/
PRIVATE t_uart_error uart_enh_PowerOn(IN t_uart_device_id uart_device_id)
{
    t_uart_error    error_status;
    DBGENTER1(" UART DEVICE (%d) ", uart_device_id);
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            if (TRUE == g_uart_system_context.uart0_need_to_call_config)
            {
                DBGEXIT0(UART_OK);
                return(UART_OK);
            }

            if (FALSE == g_uart_system_context.uart0_is_power_off)
            {
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart0_is_power_off = FALSE;
            error_status = uart_enh_SetConfiguration(uart_device_id, &g_uart_system_context.uart0_config);
            break;

        case UART_DEVICE_ID_1:
            if (TRUE == g_uart_system_context.uart1_need_to_call_config)
            {
                DBGEXIT0(UART_OK);
                return(UART_OK);
            }

            if (FALSE == g_uart_system_context.uart1_is_power_off)
            {
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart1_is_power_off = FALSE;
            error_status = uart_enh_SetConfiguration(uart_device_id, &g_uart_system_context.uart1_config);
            break;

        case UART_DEVICE_ID_2:
            if (TRUE == g_uart_system_context.uart2_need_to_call_config)
            {
                DBGEXIT0(UART_OK);
                return(UART_OK);
            }

            if (FALSE == g_uart_system_context.uart2_is_power_off)
            {
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart2_is_power_off = FALSE;
            error_status = uart_enh_SetConfiguration(uart_device_id, &g_uart_system_context.uart2_config);
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    DBGEXIT0(error_status);
    return(error_status);
}


/****************************************************************************/
/* NAME         :   uart_PowerDownSignals                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine is used to Turn Down UARt signals RTS/CTS and*/
/*                UART enable before Powering off UART                      */
/*  PARAMETERS  :                                                           */
/*          IN  : lp_uart_register       : UART Register Set                */
/*         OUT  : NONE                                                      */
/*                                                                          */
/*      RETURN  : void                                                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/  
PRIVATE void uart_PowerDownSignals(IN t_uart_register *lp_uart_register)
{
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_RTSFLOW;
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_CTSFLOW;
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_RTS;    /* Deassert the RTS signal */
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_UEN;    /* Disable Uart functionality  */
    
    return;
}


/****************************************************************************/
/* NAME         :   uart_PreparePowerOff                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine is used to prepare for UART Power-Off        */
/*                                                                          */
/*  PARAMETERS  :                                                           */
/*          IN  : uart_device_id   : UART Device                            */
/*              : lp_uart_register : UART Register Set                      */
/*         OUT  : NONE                                                      */
/*                                                                          */
/*      RETURN  : void                                                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/ 	    
PRIVATE void uart_PreparePowerOff(IN t_uart_device_id uart_device_id, IN t_uart_register *lp_uart_register)
{
    switch(uart_device_id)
    {
         case  UART_DEVICE_ID_0:   
	    if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart0_comm_status.tx_comm_status)
            {   /* Stop the transmission */
                g_uart_system_context.uart0_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
                lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
            }

            if (UART_RECEIVE_ON_GOING == g_uart_system_context.uart0_comm_status.rx_comm_status)
            {   /* Stop the reception */
                lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
                g_uart_system_context.uart0_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
            }

            if (FALSE == g_uart_system_context.uart0_is_autobaud_done)
            {   /* Stop the automatic format extraction process */
                lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
                lp_uart_register->uartx_abimsc &= ~(UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
                g_uart_system_context.uart0_is_autobaud_done = TRUE;
                g_uart_system_context.p_uart0_register->uartx_abcr = MASK_NULL16;
            }
	    return;

         case  UART_DEVICE_ID_1:   
            if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart1_comm_status.tx_comm_status)
            {   /* Stop the transmission */
                g_uart_system_context.uart1_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
                lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
            }

            if (UART_RECEIVE_ON_GOING == g_uart_system_context.uart1_comm_status.rx_comm_status)
            {   /* Stop the reception */
                lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
                g_uart_system_context.uart1_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
            }

            if (FALSE == g_uart_system_context.uart1_is_autobaud_done)
            {   /* Stop the automatic format extraction process */
                lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
                lp_uart_register->uartx_abimsc &= ~(UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
                g_uart_system_context.uart1_is_autobaud_done = TRUE;
                g_uart_system_context.p_uart1_register->uartx_abcr = MASK_NULL16;
            }
	    return;

         case  UART_DEVICE_ID_2:   
            if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart2_comm_status.tx_comm_status)
            {   /* Stop the transmission */
                g_uart_system_context.uart2_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
                lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
            }

            if (UART_RECEIVE_ON_GOING == g_uart_system_context.uart2_comm_status.rx_comm_status)
            {   /* Stop the reception */
                lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
                g_uart_system_context.uart2_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
            }

            if (FALSE == g_uart_system_context.uart2_is_autobaud_done)
            {   /* Stop the automatic format extraction process */
                lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
                lp_uart_register->uartx_abimsc &= ~(UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
                g_uart_system_context.uart2_is_autobaud_done = TRUE;
                g_uart_system_context.p_uart2_register->uartx_abcr = MASK_NULL16;
            }
	    return;

 	 case UART_DEVICE_ID_INVALID:    
         default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return;
    }
}

/****************************************************************************/
/* NAME         :   uart_enh_PowerOff                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This function is called by the upper layer to handle    */
/*                  the power off mode during power management. It is an    */
/*                  elementary routine context of uartx_imsc is saved. It   */
/*                  is an enhanced routine.                                 */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : The uart number                       */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   UART_REQUEST_NOT_APPLICABLE : When UART_PowerOff is     */
/*                                                called despite UART_IT is */
/*                                                in the power off mode.    */
/*              :   UART_OK                     : Otherwise                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       :    Non-Reentrant                                      */
/* REENTRANCY ISSUES:   This Function should be called in the CRITICAL      */
/*                      SECTION by the upper layer to maintain the          */
/*                      integrity of the context information stored.        */
/****************************************************************************/
PRIVATE t_uart_error uart_enh_PowerOff(IN t_uart_device_id uart_device_id)
{
    t_uart_register *lp_uart_register;
    DBGENTER1(" UART DEVICE (%d) ", uart_device_id);

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            if (TRUE == g_uart_system_context.uart0_need_to_call_config)
            {
                DBGEXIT0(UART_OK);
                return(UART_OK);
            }

            if (TRUE == g_uart_system_context.uart0_is_power_off)
            {
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart0_is_power_off = TRUE;

	    uart_PreparePowerOff(uart_device_id, lp_uart_register);
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            if (TRUE == g_uart_system_context.uart1_need_to_call_config)
            {
                DBGEXIT0(UART_OK);
                return(UART_OK);
            }

            if (TRUE == g_uart_system_context.uart1_is_power_off)
            {
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart1_is_power_off = TRUE;

	    uart_PreparePowerOff(uart_device_id, lp_uart_register);
            break;


        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            if (TRUE == g_uart_system_context.uart2_need_to_call_config)
            {
                DBGEXIT0(UART_OK);
                return(UART_OK);
            }

            if (TRUE == g_uart_system_context.uart2_is_power_off)
            {
                DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }

            g_uart_system_context.uart2_is_power_off = TRUE;

	    uart_PreparePowerOff(uart_device_id, lp_uart_register);
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    uart_PowerDownSignals(lp_uart_register);
    
    DBGEXIT0(UART_OK);
    return(UART_OK);
}


/****************************************************************************/
/* NAME         :   uart_enh_SetTxModeFlags_Tx                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine is used to Set the Transfer Mode flags during*/
/*                Tx                                                        */
/*  PARAMETERS  :                                                           */
/*          IN  : flags            : Flags indicating present transfer mode */
/*              : lp_uart_register : UART Register Set                      */
/*              : p_comm_status    : Present Tx/Rx Status                   */
/*         OUT  : p_transfer_flags : Transfer Flags to be set               */
/*                                                                          */
/*      RETURN  : t_uart_error                                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/ 	  
PRIVATE t_uart_error uart_enh_SetTxModeFlags_Tx(IN t_uint32 flags, IN t_uart_register *lp_uart_register, OUT t_uint32 *p_transfer_flags,  
		                                IN t_uart_it_communication_status *p_comm_status)
{
    if (flags & UART_TX_IT_MODE)
    {
        if (*p_transfer_flags & UART_TX_DMA_MODE)
        {
            lp_uart_register->uartx_dmacr &= ~(MASK_BIT1);
            *p_transfer_flags &= ~UART_TX_DMA_MODE;
        }
        else if (*p_transfer_flags & UART_TX_POLL_MODE)
        {
            *p_transfer_flags &= ~UART_TX_POLL_MODE;
        }

        *p_transfer_flags |= UART_TX_IT_MODE;
    }
    else if (flags & UART_TX_DMA_MODE)
    {
        if ((*p_transfer_flags & UART_TX_IT_MODE))
        {
            if (UART_TRANSMIT_ENDED == p_comm_status->tx_comm_status)
            {
                *p_transfer_flags &= ~UART_TX_IT_MODE;
                lp_uart_register->uartx_dmacr |= (MASK_BIT1 | MASK_BIT2);
            }
            else
            {
       	        DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }
        }
        else if (*p_transfer_flags & UART_TX_POLL_MODE)
        {
            *p_transfer_flags &= ~UART_TX_POLL_MODE;
            lp_uart_register->uartx_dmacr |= (MASK_BIT1 | MASK_BIT2);
        }

        *p_transfer_flags |= UART_TX_DMA_MODE;
    }
    else if (flags & UART_TX_POLL_MODE)
    {
        if (*p_transfer_flags & UART_TX_DMA_MODE)
        {
            lp_uart_register->uartx_dmacr &= ~(MASK_BIT1);
            *p_transfer_flags &= ~UART_TX_DMA_MODE;
        }
        else if ((*p_transfer_flags & UART_TX_IT_MODE))
        {
            if (UART_TRANSMIT_ENDED == p_comm_status->tx_comm_status)
            {
                *p_transfer_flags &= ~UART_TX_IT_MODE;
            }
            else
            {
       	        DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }
        }

        *p_transfer_flags |= UART_TX_POLL_MODE;
    }

    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   uart_enh_SetTxModeFlags_Rx                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine is used to Set the Transfer Mode flags during*/
/*                Rx                                                        */
/*  PARAMETERS  :                                                           */
/*          IN  : flags            : Flags indicating present transfer mode */
/*              : lp_uart_register : UART Register Set                      */
/*              : p_comm_status    : Present Tx/Rx Status                   */
/*         OUT  : p_transfer_flags : Transfer Flags to be set               */
/*                                                                          */
/*      RETURN  : t_uart_error                                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/ 	  
PRIVATE t_uart_error uart_enh_SetTxModeFlags_Rx(IN t_uint32 flags,  IN t_uart_register *lp_uart_register, OUT t_uint32 *p_transfer_flags,
		                                IN t_uart_it_communication_status  *p_comm_status)
{
    if (flags & UART_RX_IT_MODE)
    {
        if (*p_transfer_flags & UART_RX_DMA_MODE)
        {
            lp_uart_register->uartx_dmacr &= ~(MASK_BIT0);
            *p_transfer_flags &= ~UART_RX_DMA_MODE;
        }
        else if (*p_transfer_flags & UART_RX_POLL_MODE)
        {
            *p_transfer_flags &= ~UART_RX_POLL_MODE;
        }

        *p_transfer_flags |= UART_RX_IT_MODE;
    }
    else if (flags & UART_RX_DMA_MODE)
    {
        if ((*p_transfer_flags & UART_RX_IT_MODE))
        {
            if (UART_RECEIVE_ENDED == p_comm_status->rx_comm_status)
            {
                *p_transfer_flags &= ~UART_RX_IT_MODE;
                lp_uart_register->uartx_dmacr |= (MASK_BIT0 | MASK_BIT2);
            }
            else
            {
       	        DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }
        }
        else if (*p_transfer_flags & UART_RX_POLL_MODE)
        {
            *p_transfer_flags &= ~UART_RX_POLL_MODE;
            lp_uart_register->uartx_dmacr |= (MASK_BIT0 | MASK_BIT2);
        }

        *p_transfer_flags |= UART_RX_DMA_MODE;
    }
    else if (flags & UART_RX_POLL_MODE)
    {
        if (*p_transfer_flags & UART_RX_DMA_MODE)
        {
            lp_uart_register->uartx_dmacr &= ~(MASK_BIT0);
            *p_transfer_flags &= ~UART_RX_DMA_MODE;
        }
        else if ((*p_transfer_flags & UART_RX_IT_MODE))
        {
            if (UART_RECEIVE_ENDED == p_comm_status->rx_comm_status)
            {
                *p_transfer_flags &= ~UART_RX_IT_MODE;
            }
            else
            {
       	        DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
                return(UART_REQUEST_NOT_APPLICABLE);
            }
        }
        *p_transfer_flags |= UART_RX_POLL_MODE;
    }

    DBGEXIT0(UART_OK);
    return(UART_OK);
}
	
/****************************************************************************/
/* NAME         :   uart_enh_SetTransferMode                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine set the transfer mode for the UART. This   */
/*                  is an enhanced API.                                     */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   flags           : Flags                                 */
/*         OUT  :   none                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error: UART error code                           */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_enh_SetTransferMode(IN t_uart_device_id uart_device_id, IN t_uint32 flags)
{
    t_uart_register                 *lp_uart_register;
    t_uint32                        *p_transfer_flags;
    t_uart_it_communication_status  *p_comm_status;
    t_uart_error                    error_status=UART_OK;

    DBGENTER2("UART DEVICE %x, FLAGS %x", uart_device_id, flags);
    
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            p_transfer_flags = &g_uart_system_context.uart0_transfer_flags;
            p_comm_status = &g_uart_system_context.uart0_comm_status;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            p_transfer_flags = &g_uart_system_context.uart1_transfer_flags;
            p_comm_status = &g_uart_system_context.uart1_comm_status;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            p_transfer_flags = &g_uart_system_context.uart2_transfer_flags;
            p_comm_status = &g_uart_system_context.uart2_comm_status;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    error_status = uart_enh_SetTxModeFlags_Tx(flags, lp_uart_register, p_transfer_flags, p_comm_status);
    if(UART_OK !=error_status)
    {
         DBGEXIT0(error_status);
         return(error_status);
    }
    
    error_status = uart_enh_SetTxModeFlags_Rx(flags, lp_uart_register, p_transfer_flags, p_comm_status);

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   uart_ConfigRxTx                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  : This routine is used to configure the UART Rx/Tx sides    */
/*                                                                          */
/*  PARAMETERS  :                                                           */
/*          IN  : uart_device_id   : UART Device                            */
/*              : lp_uart_register : UART Register Set                      */
/*              : p_config         : configuration struct                   */
/*         OUT  : None                                                      */
/*                                                                          */
/*      RETURN  : t_uart_error                                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/ 	 
PRIVATE t_uart_error uart_ConfigRxTx(IN t_uart_device_id uart_device_id, IN t_uart_register *lp_uart_register,
	                             IN t_uart_config *p_config)
{
    volatile t_uint32    data_flushed = 0;                       /* Variable to flush Rx fifo */

    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_UEN;        /* Enable Uart functionality: Fix for Bug */

    /* Enable transmitter and/or receiver + Flush associated fifo */
    if (p_config->receive_enable)
    {
        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_RXEN;   /* enable RX */

        /* Fflush Rxfifo: Perform required Rx fifo reading to have Rx fifo empty */
        do
        {
            data_flushed = lp_uart_register->uartx_dr;
        } while (FALSE == UART_ISRXFIFOEMPTY());
    }

    if (p_config->transmit_enable)
    {
        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_TXEN;   /* Enable TX */

        if (FALSE == UART_ISTXFIFOEMPTY())
        {
            DBGPRINT(DBGL_ERROR, "Tx fifo was not empty");
            DBGEXIT0(UART_INTERNAL_ERROR);
            return(UART_INTERNAL_ERROR);
        }
    }

    /* All UART_IT except Tx  and Tx FIFO Empty INT are cleared */
    lp_uart_register->uartx_icr |= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);

    /* Enable the Error Interrupts */
    if (UART_DEVICE_ID_0 == uart_device_id)
    {
        lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_BE | UART_IRQ_SRC_FE | UART_IRQ_SRC_OE);

        /* Enable the Parity Interrupt */
        if (UART_PERR_ENDREAD == p_config->perr_condition)
        {
            lp_uart_register->uartx_imsc |= UART_IRQ_SRC_PE;
        }

        /* Enable Modem Related Interrupts */
        if (UART_NON_MODEM_MODE != p_config->uart_mode)
        {
            lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_DSR | UART_IRQ_SRC_DCD | UART_IRQ_SRC_RI);

            /* Enable HW FC Interrupt */
            if (UART_HW_FLOW_CONTROL == p_config->flow_control)
            {
                lp_uart_register->uartx_imsc |= UART_IRQ_SRC_CTS;
            }
        }
    }
    else if (UART_DEVICE_ID_1 == uart_device_id)
    {
        lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_BE | UART_IRQ_SRC_FE | UART_IRQ_SRC_OE);

        /* Enable the Parity Interrupt */
        if (UART_PERR_ENDREAD == p_config->perr_condition)
        {
            lp_uart_register->uartx_imsc |= UART_IRQ_SRC_PE;
        }

        /* Enable Modem Related Interrupts */
        if (UART_NON_MODEM_MODE != p_config->uart_mode)
        {
            g_uart_system_context.uart1_need_to_call_config = TRUE;
            DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
            return(UART_REQUEST_NOT_APPLICABLE);
	}
    }

    else
    {
        lp_uart_register->uartx_imsc |= (UART_IRQ_SRC_BE | UART_IRQ_SRC_FE | UART_IRQ_SRC_OE);

        /* Enable the Parity Interrupt */
        if (UART_PERR_ENDREAD == p_config->perr_condition)
        {
            lp_uart_register->uartx_imsc |= UART_IRQ_SRC_PE;
        }

        /* Enable Modem Related Interrupts */
        if (UART_NON_MODEM_MODE != p_config->uart_mode)
        {
            g_uart_system_context.uart2_need_to_call_config = TRUE;
            DBGEXIT0(UART_REQUEST_NOT_APPLICABLE);
            return(UART_REQUEST_NOT_APPLICABLE);
	}
    }
    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   uart_enh_SetConfiguration                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures UART registers                  */
/*                  It also flush Tx and Rx fifo to enable a new transfer.  */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   p_config        : Pointer on global configuration       */
/*                                    structure                             */
/*         OUT  :   None                                                    */
/*                                                                          */
/*      RETURN  :   t_uart_error: UART error code                           */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_enh_SetConfiguration(IN t_uart_device_id uart_device_id, IN t_uart_config *p_config)
{
    t_uart_error         error_status = UART_OK;
    t_uart_register      *lp_uart_register;

    DBGENTER2("UART DEVICE %d, CONFIG STRUCT POINTER %x", uart_device_id, (t_uint32) p_config);

    if (NULL == p_config)
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }
    
    /* Initialization phase */
    switch (uart_device_id)                                     /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            {
                lp_uart_register = g_uart_system_context.p_uart0_register;
                g_uart_system_context.uart0_config = *p_config;
                g_uart_system_context.uart0_need_to_call_config = FALSE;
                g_uart_system_context.uart0_comm_status.tx_comm_status = UART_TRANSMIT_ENDED;
                g_uart_system_context.uart0_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
                g_uart_system_context.uart0_rx_status = (t_uart_receive_status) UART_RECEIVE_OK;
                g_uart_system_context.uart0_is_autobaud_done = TRUE;
                g_uart_system_context.uart0_event = 0;
                g_uart_system_context.uart0_transfer_flags = UART_TX_IT_MODE | UART_RX_IT_MODE;
                UpdateTxTrig(g_uart_system_context.uart0, 1);
                UpdateRxTrig(g_uart_system_context.uart0, 1);
                break;
            }

        case UART_DEVICE_ID_1:
            {
                lp_uart_register = g_uart_system_context.p_uart1_register;
                g_uart_system_context.uart1_config = *p_config;
                g_uart_system_context.uart1_need_to_call_config = FALSE;
                g_uart_system_context.uart1_comm_status.tx_comm_status = UART_TRANSMIT_ENDED;
                g_uart_system_context.uart1_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
                g_uart_system_context.uart1_rx_status = (t_uart_receive_status) UART_RECEIVE_OK;
                g_uart_system_context.uart1_is_autobaud_done = TRUE;
                g_uart_system_context.uart1_event = 0;
                g_uart_system_context.uart1_transfer_flags = UART_TX_IT_MODE | UART_RX_IT_MODE;
                UpdateTxTrig(g_uart_system_context.uart1, 1);
                UpdateRxTrig(g_uart_system_context.uart1, 1);
                break;
            }


        case UART_DEVICE_ID_2:
            {
                lp_uart_register = g_uart_system_context.p_uart2_register;
                g_uart_system_context.uart2_config = *p_config;
                g_uart_system_context.uart2_need_to_call_config = FALSE;
                g_uart_system_context.uart2_comm_status.tx_comm_status = UART_TRANSMIT_ENDED;
                g_uart_system_context.uart2_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
                g_uart_system_context.uart2_rx_status = (t_uart_receive_status) UART_RECEIVE_OK;
                g_uart_system_context.uart2_is_autobaud_done = TRUE;
                g_uart_system_context.uart2_event = 0;
                g_uart_system_context.uart2_transfer_flags = UART_TX_IT_MODE | UART_RX_IT_MODE;
                UpdateTxTrig(g_uart_system_context.uart2, 1);
                UpdateRxTrig(g_uart_system_context.uart2, 1);
                break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    /* Reset main registers */
    UART_RESET_REG(lp_uart_register->uartx_lcrh_tx);
    UART_RESET_REG(lp_uart_register->uartx_lcrh_rx);
    UART_RESET_REG(lp_uart_register->uartx_imsc);
    UART_RESET_REG(lp_uart_register->uartx_cr);
    UART_RESET_REG(lp_uart_register->uartx_itcr);
    UART_RESET_REG(lp_uart_register->uartx_rsr_ecr);
    UART_RESET_REG(lp_uart_register->uartx_xfcr);
    UART_RESET_REG(lp_uart_register->uartx_xon1);
    UART_RESET_REG(lp_uart_register->uartx_xon2);
    UART_RESET_REG(lp_uart_register->uartx_xoff1);
    UART_RESET_REG(lp_uart_register->uartx_xoff2);
    UART_RESET_REG(lp_uart_register->uartx_dmacr);
    UART_RESET_REG(lp_uart_register->uartx_abimsc);
    lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);

    /* Set the BAUD RATE */
    error_status = uart_SetBaudRate(p_config, lp_uart_register);
    if(UART_OK != error_status)
    {
        DBGEXIT0(error_status);
        return(error_status);
    }

    /* Configure line control register */
    lp_uart_register->uartx_lcrh_tx = lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_SB;   /* Disable sendbreak */

    /* Enable FIFOs */
    uart_EnableFifo(p_config, lp_uart_register);


    /* Set data bits */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_WL) |
            ((t_uint32) ((t_uint32) p_config->tx_data_bits << UART_LINEC_SHIFT_WL) & (t_uint32) UART_LINEC_MASK_WL)
        );
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_WL) |
            ((t_uint32) ((t_uint32) p_config->rx_data_bits << UART_LINEC_SHIFT_WL) & (t_uint32) UART_LINEC_MASK_WL)
        );    /* program data_bits number */

    

    /* Set stop bits */
    lp_uart_register->uartx_lcrh_tx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_tx &~UART_LINEC_MASK_STOP) |
            ((t_uint32) ((t_uint32) p_config->tx_stop_bits << UART_LINEC_SHIFT_STOP) & (t_uint32) UART_LINEC_MASK_STOP)
        );              
    lp_uart_register->uartx_lcrh_rx =
        (
            (t_uint32) (lp_uart_register->uartx_lcrh_rx &~UART_LINEC_MASK_STOP) |
            ((t_uint32) ((t_uint32) p_config->rx_stop_bits << UART_LINEC_SHIFT_STOP) & (t_uint32) UART_LINEC_MASK_STOP)
        );                  /* program stop_bits number */


    /* Configure Parity */    
    error_status = uart_SetParity(p_config, lp_uart_register);
    if(UART_OK != error_status)
    {
         DBGEXIT0(error_status);
	 return(error_status);
    }

    /* Configure control register */
    if (UART_DCE_MODE == p_config->uart_mode)   /* UDCD and URI are then configured as output */
    {
        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_UDCD;
        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_URI;
    }


    /* Configure FIFO Trigger levels */
    error_status = uart_SetFifoTrigger(uart_device_id, p_config);
    if(UART_OK != error_status)
    {
         DBGEXIT0(error_status);
	 return(error_status);
    }    

    /* Configure Flow Controls */
    uart_SetFlowControl(p_config, lp_uart_register);


    if (TRUE == p_config->enable_autobaud)
    {

        /* Initiate the automatic format extraction */
        lp_uart_register->uartx_abcr |= (MASK_BIT0 | MASK_BIT1 | MASK_BIT2);

        do
        {
        } while
        (
            (0 == (lp_uart_register->uartx_absr & MASK_BIT12 /* AC Done */ ))
        &&  (0 == (lp_uart_register->uartx_absr & MASK_BIT8 /* Valid Format */ ))
        );

        UART_RESET_REG(lp_uart_register->uartx_abcr);

        /* UART_CLEAR & Disable all the Autobaud interrupts */
        UART_RESET_REG(lp_uart_register->uartx_abimsc);
        lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);

        if (UART_DEVICE_ID_0 == uart_device_id)
        {
            g_uart_system_context.uart0_is_autobaud_done = TRUE;
        }
        else if (UART_DEVICE_ID_1 == uart_device_id)
        {
            g_uart_system_context.uart1_is_autobaud_done = TRUE;
        }

        else
        {
            g_uart_system_context.uart2_is_autobaud_done = TRUE;
        }
    }

    error_status = uart_ConfigRxTx(uart_device_id, lp_uart_register, p_config);

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   uart_enh_ConfigureAutobaud                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine is used to set/UART_CLEAR the autobaud     */
/*                  configuration                                           */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   uart_action     : To set or clear autobaud configuration*/
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_enh_ConfigureAutoBaud(IN t_uart_device_id uart_device_id, IN t_uart_set_or_clear uart_action)
{
    t_uart_error    error_status = UART_OK;
    t_uart_register *lp_uart_register;
    DBGENTER2("UART DEVICE %d, CLEAR/SET=%d", uart_device_id, uart_action);

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    switch (uart_action)
    {
        case UART_SET:
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_UEN;

            /* UART_CLEAR & Enable all the Autobaud Interrupts */
            lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
            lp_uart_register->uartx_abimsc |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);

            if (UART_DEVICE_ID_0 == uart_device_id)
            {
                g_uart_system_context.uart0_is_autobaud_done = FALSE;
            }
            else if (UART_DEVICE_ID_1 == uart_device_id)
            {
                g_uart_system_context.uart1_is_autobaud_done = FALSE;
            }

            else
            {
                g_uart_system_context.uart2_is_autobaud_done = FALSE;
            }

            lp_uart_register->uartx_abcr |= (MASK_BIT0 | MASK_BIT1 | MASK_BIT2);
            break;

        case UART_CLEAR:
            /* UART_CLEAR & Disable all the Autobaud Interrupts */
            lp_uart_register->uartx_abicr |= (UART_IRQ_SRC_ABD | UART_IRQ_SRC_ABE);
            UART_RESET_REG(lp_uart_register->uartx_abimsc);

            if (UART_DEVICE_ID_0 == uart_device_id)
            {
                g_uart_system_context.uart0_is_autobaud_done = TRUE;
            }
            else if (UART_DEVICE_ID_1 == uart_device_id)
            {
                g_uart_system_context.uart1_is_autobaud_done = TRUE;
            }

            else
            {
                g_uart_system_context.uart2_is_autobaud_done = TRUE;
            }
            lp_uart_register->uartx_abcr = MASK_NULL16;
            lp_uart_register->uartx_cr = lp_uart_register->uartx_cr | UART_CONTROL_MASK_UEN;
            break;

        case UART_NO_CHANGE:
            error_status = UART_OK;
            break;

        default:
            error_status = UART_INVALID_PARAMETER;
            break;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   uart_enh_GetFormatDetails                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine gives status of stop_bits, data_bits,      */
/*                  parity bits, baud_rate and fifo enabled                 */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*         OUT  :   p_tx_stop_bits  : Tx Stop bits information              */
/*              :   p_tx_data_bits  : Tx Data bits information              */
/*              :   p_tx_parity_bit : Tx Parity bit information             */
/*              :   p_rx_stop_bits  : Rx Stop bits information              */
/*              :   p_rx_data_bits  : Rx Data bits information              */
/*              :   p_rx_parity_bit : Rx Parity bit information             */
/*              :   p_baudrate      : Baud_rate information                 */
/*              :   t_bool          : Fifo Enabled information(Tx/Rx)       */
/*              :   t_bool          : Autobaud Done information             */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY   :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_enh_GetFormatDetails
(
    IN t_uart_device_id     uart_device_id,
    OUT t_uart_stop_bits    *p_tx_stop_bits,
    OUT t_uart_data_bits    *p_tx_data_bits,
    OUT t_uart_parity_bit   *p_tx_parity_bit,
    OUT t_uart_stop_bits    *p_rx_stop_bits,
    OUT t_uart_data_bits    *p_rx_data_bits,
    OUT t_uart_parity_bit   *p_rx_parity_bit,
    OUT t_uart_baud_rate    *p_baud_rate,
    OUT t_bool              *p_tx_fifo_enabled,
    OUT t_bool              *p_rx_fifo_enabled,
    OUT t_bool              *p_is_autobaud_done
)
{
    t_uart_error    error_status = UART_OK;
    t_uart_register *lp_uart_register;

    DBGENTER1("UART DEVICE (%d)", uart_device_id);

    if
    (
        NULL == p_tx_stop_bits
    ||  NULL == p_tx_data_bits
    ||  NULL == p_tx_parity_bit
    ||  NULL == p_rx_stop_bits
    ||  NULL == p_rx_data_bits
    ||  NULL == p_rx_parity_bit    
    ||  NULL == p_baud_rate
    ||  NULL == p_tx_fifo_enabled
    ||  NULL == p_rx_fifo_enabled
    ||  NULL == p_is_autobaud_done
    )
    {
        DBGEXIT0(UART_INVALID_PARAMETER);
        return(UART_INVALID_PARAMETER);
    }

    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            *p_is_autobaud_done = g_uart_system_context.uart0_is_autobaud_done;
            *p_baud_rate = g_uart_system_context.uart0_config.baud_rate; 
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            *p_is_autobaud_done = g_uart_system_context.uart1_is_autobaud_done;
            *p_baud_rate = g_uart_system_context.uart1_config.baud_rate; 
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            *p_is_autobaud_done = g_uart_system_context.uart2_is_autobaud_done;
            *p_baud_rate = g_uart_system_context.uart2_config.baud_rate; 
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }


    error_status = uart_GetParity(lp_uart_register, p_tx_parity_bit, p_rx_parity_bit);   
    if(UART_OK != error_status)
    {
        DBGEXIT0(error_status);
        return(error_status);
    }
    *p_tx_data_bits = (t_uart_data_bits) ((lp_uart_register->uartx_lcrh_tx & UART_LINEC_MASK_WL) >> UART_LINEC_SHIFT_WL);
    *p_rx_data_bits = (t_uart_data_bits) ((lp_uart_register->uartx_lcrh_rx & UART_LINEC_MASK_WL) >> UART_LINEC_SHIFT_WL);

    *p_tx_stop_bits = (t_uart_stop_bits) ((lp_uart_register->uartx_lcrh_tx & UART_LINEC_MASK_STOP) >> UART_LINEC_SHIFT_STOP);
    *p_rx_stop_bits = (t_uart_stop_bits) ((lp_uart_register->uartx_lcrh_rx & UART_LINEC_MASK_STOP) >> UART_LINEC_SHIFT_STOP);

    *p_tx_fifo_enabled = (t_bool) ((lp_uart_register->uartx_lcrh_tx & UART_LINEC_MASK_ENFIFOS) >> UART_LINEC_SHIFT_ENFIFOS);
    *p_rx_fifo_enabled = (t_bool) ((lp_uart_register->uartx_lcrh_rx & UART_LINEC_MASK_ENFIFOS) >> UART_LINEC_SHIFT_ENFIFOS);

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME         :   uart_enh_Disable                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to disable UART                     */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to access               */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_enh_Disable(IN t_uart_device_id uart_device_id)
{
    t_uart_error    error_status = UART_OK;
    t_uart_mode     uart_mode;
    t_uart_register *lp_uart_register;
    DBGENTER1("UART DEVICE (%d)", uart_device_id);
    switch (uart_device_id)
    {
        case UART_DEVICE_ID_0:
            {
                lp_uart_register = g_uart_system_context.p_uart0_register;
                uart_mode = g_uart_system_context.uart0_config.uart_mode;
                if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart0_comm_status.tx_comm_status)
                {   /* Stop the transmission */
                    g_uart_system_context.uart0_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
                    lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
                }

                if (UART_RECEIVE_ON_GOING == g_uart_system_context.uart0_comm_status.rx_comm_status)
                {   /* Top the reception */
                    lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
                    g_uart_system_context.uart0_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
                }

                g_uart_system_context.uart0_is_xoff_recd = FALSE;
                g_uart_system_context.uart0_is_xon_recd = FALSE;
                g_uart_system_context.uart0_is_autobaud_done = FALSE;
                break;
            }

        case UART_DEVICE_ID_1:
            {
                lp_uart_register = g_uart_system_context.p_uart1_register;
                uart_mode = g_uart_system_context.uart1_config.uart_mode;
                if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart1_comm_status.tx_comm_status)
                {   /* Stop the transmission */
                    g_uart_system_context.uart1_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
                    lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
                }

                if (UART_RECEIVE_ON_GOING == g_uart_system_context.uart1_comm_status.rx_comm_status)
                {   /* Stop the reception */
                    lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
                    g_uart_system_context.uart1_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
                }

                g_uart_system_context.uart1_is_xoff_recd = FALSE;
                g_uart_system_context.uart1_is_xon_recd = FALSE;
                g_uart_system_context.uart1_is_autobaud_done = FALSE;
                break;
            }


        case UART_DEVICE_ID_2:
            {
                lp_uart_register = g_uart_system_context.p_uart2_register;
                uart_mode = g_uart_system_context.uart2_config.uart_mode;
                if (UART_TRANSMIT_ON_GOING == g_uart_system_context.uart2_comm_status.tx_comm_status)
                {   /* Stop the transmission */
                    g_uart_system_context.uart2_comm_status.tx_comm_status = UART_WRITTINGS_ENDED;
                    lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_TX | UART_IRQ_SRC_TXFE);
                }

                if (UART_RECEIVE_ON_GOING == g_uart_system_context.uart2_comm_status.rx_comm_status)
                {   /* Stop the reception */
                    lp_uart_register->uartx_imsc &= ~(UART_IRQ_SRC_RX | UART_IRQ_SRC_RXTO);
                    g_uart_system_context.uart2_comm_status.rx_comm_status = UART_RECEIVE_ENDED;
                }

                g_uart_system_context.uart2_is_xoff_recd = FALSE;
                g_uart_system_context.uart2_is_xon_recd = FALSE;
                g_uart_system_context.uart2_is_autobaud_done = FALSE;
                break;
            }

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_RTSFLOW;
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_CTSFLOW;
    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_RTS;        /* Deassert the RTS signal */

    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_DTR;        /* Deassert DTR */

    if (UART_DCE_MODE == uart_mode)
    {
        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_UDCD;   /* Deassert DCD */
        lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_URI;    /* Deassert RI */
    }

    lp_uart_register->uartx_cr = lp_uart_register->uartx_cr &~UART_CONTROL_MASK_UEN;        /* Disable Uart functionality */
    DBGEXIT0(error_status);
    return(error_status);
}
#endif /* __UART_ENHANCED */

/****************************************************************************/
/* NAME         :   uart_ChooseFifoTriggerLevel_Nomadik                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows user to choose  UART Int trigger    */
/*                  FIFO level for Nomadik UART                             */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id  : Identify UART to configure            */
/*              :   tx_trigger      : Trigger value for tx fifo             */
/*              :   rx_trigger      : Trigger value for rx fifo             */
/*         OUT  :     p_trigger     : Will be used by Uart_SetFifoTrigger   */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PRIVATE t_uart_error uart_ChooseFifoTriggerLevel_Nomadik
(
    IN t_uart_device_id     uart_device_id,
    IN t_uart_tx_trigger    tx_trigger,
    IN t_uart_rx_trigger    rx_trigger,
    OUT t_uart_fifo_trigger *p_trigger
)
{
    DBGENTER3("UART DEVICE %x, tx_trigger FIFO ptr %p, rx_trigger FIFO Ptr %p", uart_device_id, (t_uint32 *)(&tx_trigger), (t_uint32 *)(&rx_trigger) );
 	
    switch(uart_device_id)
    {
         case UART_DEVICE_ID_0:
                    p_trigger->trig0_tx = (t_uint32 )tx_trigger;
		    if(UART_TX_TRIG_THREE_FOURTH_EMPTY != tx_trigger)
		    {
                        UpdateTxTrig(g_uart_system_context.uart0, (((t_uint32 )tx_trigger)<<1) );
		    }
		    else
		    {
                        UpdateTxTrig(g_uart_system_context.uart0, UART_FIFO_THREE_FOURTH_TRIGGER_VAL);
		    }
        
		    p_trigger->trig0_rx = (t_uint32 )rx_trigger;
		    if(UART_RX_TRIG_THREE_FOURTH_FULL != rx_trigger)
		    {
                        UpdateRxTrig(g_uart_system_context.uart0, (((t_uint32 )rx_trigger)<<1) );
		    }
		    else
		    {
                        UpdateRxTrig(g_uart_system_context.uart0, UART_FIFO_THREE_FOURTH_TRIGGER_VAL);
		    }
	
	            break;
    
	 case UART_DEVICE_ID_1:
                   p_trigger->trig1_tx = (t_uint32 )tx_trigger;
		    if(UART_TX_TRIG_THREE_FOURTH_EMPTY != tx_trigger)
		    {
                        UpdateTxTrig(g_uart_system_context.uart1, (((t_uint32 )tx_trigger)<<1) );
		    }
		    else
		    {
                        UpdateTxTrig(g_uart_system_context.uart1, UART_FIFO_THREE_FOURTH_TRIGGER_VAL);
		    }            
                    p_trigger->trig1_rx = (t_uint32 )rx_trigger;
		    if(UART_RX_TRIG_THREE_FOURTH_FULL != rx_trigger)
		    {
                        UpdateRxTrig(g_uart_system_context.uart1, (((t_uint32 )rx_trigger)<<1) );
		    }
		    else
		    {
                        UpdateRxTrig(g_uart_system_context.uart1, UART_FIFO_THREE_FOURTH_TRIGGER_VAL);
		    } 
	
		    break;
	    
  	 case UART_DEVICE_ID_2:
                    p_trigger->trig2_tx = (t_uint32 )tx_trigger;
		    if(UART_TX_TRIG_THREE_FOURTH_EMPTY != tx_trigger)
		    {
                        UpdateTxTrig(g_uart_system_context.uart2, (((t_uint32 )tx_trigger)<<1) );
		    }
		    else
		    {
                        UpdateTxTrig(g_uart_system_context.uart2, UART_FIFO_THREE_FOURTH_TRIGGER_VAL);
		    }            
                    p_trigger->trig2_rx = (t_uint32 )rx_trigger;
		    if(UART_RX_TRIG_THREE_FOURTH_FULL != rx_trigger)
		    {
                        UpdateRxTrig(g_uart_system_context.uart2, (((t_uint32 )rx_trigger)<<1) );
		    }
		    else
		    {
                        UpdateRxTrig(g_uart_system_context.uart2, UART_FIFO_THREE_FOURTH_TRIGGER_VAL);
		    }
	
		    break;
       
  	 case UART_DEVICE_ID_INVALID:
         default:
                    DBGEXIT0(UART_INVALID_PARAMETER);
                    return(UART_INVALID_PARAMETER);
    }

    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/****************************************************************************/
/* NAME         :   UART_SetTimeout                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine configures the Rx Timeout duration. Formula*/
/*                  is period = timeout(ms)*Baud_Rate*(8 or 16 as per OVS)  */
/*  PARAMETERS  :                                                           */
/*          IN  :   uart_device_id      : Identify UART to configure        */
/*         OUT  :   uart_timeout_period : Timeout Period to configure       */
/*                                        Parameter's Least 22 bits used    */
/*                                                                          */
/*      RETURN  :   t_uart_error    : UART error code                       */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC t_uart_error UART_SetTimeout(IN t_uart_device_id uart_device_id, IN t_uint32 uart_timeout_period)
{
    t_uart_register                 *lp_uart_register;

    DBGENTER2(" UART DEVICE (%d), UART Timeout (%x)", uart_device_id, uart_timeout_period);

    switch (uart_device_id) /* Choose suitable UART base adress */
    {
        case UART_DEVICE_ID_0:
            lp_uart_register = g_uart_system_context.p_uart0_register;
            break;

        case UART_DEVICE_ID_1:
            lp_uart_register = g_uart_system_context.p_uart1_register;
            break;

        case UART_DEVICE_ID_2:
            lp_uart_register = g_uart_system_context.p_uart2_register;
            break;

	case UART_DEVICE_ID_INVALID:    
        default:
            DBGEXIT0(UART_INVALID_PARAMETER);
            return(UART_INVALID_PARAMETER);
    }

    lp_uart_register->uartx_timeout = uart_timeout_period & UART_MASK_TIMEOUT_22_BITS;
    
    DBGEXIT0(UART_OK);
    return(UART_OK);
}

/* End of file - uart.c */

