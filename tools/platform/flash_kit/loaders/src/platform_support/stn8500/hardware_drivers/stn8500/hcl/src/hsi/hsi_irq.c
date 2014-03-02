/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public file of High Speed Serial Interface (HSI) module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "hsi_p.h"

/*--------------------------------------------
	Global variables
----------------------------------------------*/
PRIVATE t_hsi_system_context    g_hsi_system_context;

/****************************************************************************/
/* NAME:	 HSI_SetBaseAddress()											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine initializes the HSI registers 				*/
/* PARAMETERS    :															*/
/* IN            : id_device -- Transmitter/Receiver device.				*/
/*				   base_address -- Base Address of the device				*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : void														*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA														    */

/****************************************************************************/
PUBLIC void HSI_SetBaseAddress(IN t_hsi_device_id id_device, IN t_logical_address base_address)
{
    if (HSI_DEVICE_ID_RX == id_device)
    {
        g_hsi_system_context.p_hsi_rx = (t_hsi_rx_register *) base_address;
    }
    else if(HSI_DEVICE_ID_TX == id_device)
    {
        g_hsi_system_context.p_hsi_tx = (t_hsi_tx_register *) base_address;
    }
}

/*****************************************************************************/
/* NAME:	 HSI_GetDeviceID()												 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION   : This function provides the Device ID number associated	 */
/*             	  with the IRQ Source ID number.							 */
/* PARAMETERS    :															 */
/* IN            : irq_src :-ORed combination of the interrupt value		 */
/* INOUT         : None														 */
/* OUT           : None														 */
/* RETURN VALUE  : t_hsi_device 											 */
/* TYPE          : Public													 */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													 */

/*****************************************************************************/
PUBLIC t_hsi_device_id HSI_GetDeviceID(IN t_hsi_irq_src irq_src)
{
    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_TX_IRQ0 | (t_uint32) HSI_IRQ_SRC_TX_IRQ1 | (t_uint32) HSI_IRQ_SRC_TX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_TX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ7 
                )
        ) != NULL
    )
    {
        return(HSI_DEVICE_ID_TX);
    }
    else if
        (
            (
                irq_src &
                    (
                        (t_uint32) HSI_IRQ_SRC_RX_IRQ0 | (t_uint32) HSI_IRQ_SRC_RX_IRQ1 | (t_uint32) HSI_IRQ_SRC_RX_IRQ2 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ3 |
                                (t_uint32) HSI_IRQ_SRC_RX_IRQ4 |
                                (t_uint32) HSI_IRQ_SRC_RX_IRQ5 |
                                (t_uint32) HSI_IRQ_SRC_RX_IRQ6 |
                                (t_uint32) HSI_IRQ_SRC_RX_IRQ7 |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_TIMEOUT |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_OVERRUN |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_BREAK |
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
                                (t_uint32) HSI_IRQ_SRC_RX_EX_PARITY |
#endif
                                (t_uint32) HSI_IRQ_SRC_RX_EX_OV0 |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_OV1 |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_OV2 |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_OV3 |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_OV4 |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_OV5 |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_OV6 |
                                (t_uint32) HSI_IRQ_SRC_RX_EX_OV7 

                    )
            ) != NULL
        )
    {
        return(HSI_DEVICE_ID_RX);
    }
    else
    {
        return(HSI_DEVICE_ID_INVALID);
    }
}

/****************************************************************************/
/* NAME:	 HSI_EnableIRQSrc() 											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine enables single/ multiple interrupt sources	*/
/* PARAMETERS    :															*/
/* IN            : irq_src -- irqsource identification						*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : void 												    */
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC void HSI_EnableIRQSrc(IN t_hsi_irq_src irq_src)
{
    t_uint32    index;

    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_TX_IRQ0 | (t_uint32) HSI_IRQ_SRC_TX_IRQ1 | (t_uint32) HSI_IRQ_SRC_TX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_TX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ7
                )
        ) != NULL
    )
    {
        for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
        {
            if ((t_uint32) (MASK_BIT0 << index) & irq_src)
            {
                HSI_SET_WMARKIMx(g_hsi_system_context.p_hsi_tx->hsit_wmarkim, index);
            }
        }
    }

    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_RX_IRQ0 | (t_uint32) HSI_IRQ_SRC_RX_IRQ1 | (t_uint32) HSI_IRQ_SRC_RX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_RX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ7
                )
        ) != NULL
    )
    {
        for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
        {
            if ((t_uint32) (MASK_BIT0 << (index + HSI_MAX_CHANNEL_NUM)) & irq_src)
            {
                HSI_SET_WMARKIMx(g_hsi_system_context.p_hsi_rx->hsir_wmarkim, index);
            }
        }
    }

    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_TIMEOUT)
    {
        HSI_SET_RX_EX_TIMEOUT(g_hsi_system_context.p_hsi_rx->hsir_excepim);
    }

    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_OVERRUN)
    {
        HSI_SET_RX_EX_OVR(g_hsi_system_context.p_hsi_rx->hsir_excepim);
    }

    if(irq_src & ((t_uint32) HSI_IRQ_SRC_RX_EX_OV0 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV1 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV2 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV3 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV4 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV5 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV6 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV7 
       ))
    {
        for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
        {
            if ((t_uint32) (MASK_BIT0 << (index + SHIFT_QUARTET5)) & irq_src)
            {
                HSI_SET_RX_OVERRUN_CHx(g_hsi_system_context.p_hsi_rx->hsir_overrunim, index);
            }
        }

    }

    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_BREAK)
    {
        HSI_SET_RX_EX_BREAK(g_hsi_system_context.p_hsi_rx->hsir_excepim);

    }
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_PARITY)
    {
        HSI_SET_RX_EX_PARITY(g_hsi_system_context.p_hsi_rx->hsir_excepim);

    }
#endif
}

/****************************************************************************/
/* NAME:	 HSI_DisableIRQSrc() 											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine enables single/ multiple interrupt sources	*/
/* PARAMETERS    :															*/
/* IN            : id -- irqsource identification							*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : void 													*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC void HSI_DisableIRQSrc(IN t_hsi_irq_src irq_src)
{
    t_uint32    index;
    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_TX_IRQ0 | (t_uint32) HSI_IRQ_SRC_TX_IRQ1 | (t_uint32) HSI_IRQ_SRC_TX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_TX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ7
                )
        ) != NULL
    )
    {
        for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
        {
            if ((t_uint32) (MASK_BIT0 << index) & irq_src)
            {
                HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_tx->hsit_wmarkim, index);
            }
        }
    }

    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_RX_IRQ0 | (t_uint32) HSI_IRQ_SRC_RX_IRQ1 | (t_uint32) HSI_IRQ_SRC_RX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_RX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ7
                )
        ) != NULL
    )
    {
        for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
        {
            if ((t_uint32) (MASK_BIT0 << (index + HSI_MAX_CHANNEL_NUM)) & irq_src)
            {
                HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_rx->hsir_wmarkim, index);
            }
        }
    }

    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_TIMEOUT)
    {
        HSI_RX_CLR_EX_TIMEOUT(g_hsi_system_context.p_hsi_rx->hsir_excepim);
    }

    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_OVERRUN)
    {
        HSI_CLR_RX_EX_OVR(g_hsi_system_context.p_hsi_rx->hsir_excepim);
    }

    if(irq_src & ((t_uint32) HSI_IRQ_SRC_RX_EX_OV0 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV1 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV2 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV3 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV4 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV5 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV6 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV7 
       ))
    {
        for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
        {
            if ((t_uint32) (MASK_BIT0 << (index + SHIFT_QUARTET5)) & irq_src)
            {
                HSI_CLEAR_RX_OVERRUN_CHx(g_hsi_system_context.p_hsi_rx->hsir_overrunim, index);
            }
        }

    }

    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_BREAK)
    {
        HSI_CLR_RX_EX_BREAK(g_hsi_system_context.p_hsi_rx->hsir_excepim);

    }
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_PARITY)
    {
        HSI_CLR_RX_EX_PARITY(g_hsi_system_context.p_hsi_rx->hsir_excepim);

    }
#endif
}

/****************************************************************************/
/* NAME:	 HSI_GetIRQSrc() 												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This function returns the IRQ source numbers of the 		*/
/*					available interrupts.									*/
/* PARAMETERS    :															*/
/* IN            : idDevice -- Tx or Rx										*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : t_hsi_idIRQSrc											*/
/* TYPE          : Public													*/
/*  20     19     16    15                   8       7 				0		*/
/* ------ ------ -----  ----- ----- ----- -----  ----- ------ ---- ------	*/
/*   RX_ | RX_ | | RX_  | RX_  |RX_  |     |RX_  | TX_ |     |     | TX_ |  */
/*   OVR | REX | |IRQEX | IRQ7 |IRQ6 |     |IRQ0 | IRQ7|     |     | IRQ0|  */
/* ------ ------ -----  ----- ----- ----- -----  ----- ------ ---- ------   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/

PUBLIC t_hsi_irq_src HSI_GetIRQSrc(IN t_hsi_device_id device)
{
    t_hsi_irq_src   interrupt_status = NULL;

    switch (device)
    {
        case HSI_DEVICE_ID_TX:
            interrupt_status |= ((g_hsi_system_context.p_hsi_tx->hsit_wmarkmis) & MASK_ALL8);
            break;

        case HSI_DEVICE_ID_RX:
            interrupt_status |= ((g_hsi_system_context.p_hsi_rx->hsir_wmarkmis) & MASK_ALL8)  << SHIFT_BYTE1;
            interrupt_status |= ((g_hsi_system_context.p_hsi_rx->hsir_excepmis & HSI_RX_MASK_EXP_ALL) << SHIFT_BYTE2);
            interrupt_status |= ((g_hsi_system_context.p_hsi_rx->hsir_overrunmis & MASK_ALL8) << SHIFT_QUARTET5);
            break;

        case HSI_DEVICE_ID_INVALID:
        default:
            return(NULL);
    }

    return(interrupt_status);
}

/****************************************************************************/
/* NAME:	 HSI_ClearIRQSrc() 												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine is used to clear the interrupt status bit	*/
/* PARAMETERS    :															*/
/* IN            : irq_src:Interrupt surce 									*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : void														*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/
PUBLIC void HSI_ClearIRQSrc(IN t_hsi_irq_src irq_src)
{

    t_uint32    index;
    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_TX_IRQ0 | (t_uint32) HSI_IRQ_SRC_TX_IRQ1 | (t_uint32) HSI_IRQ_SRC_TX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_TX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ7
                )
        ) != NULL
    )
    {
        for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
        {
            if ((t_uint32) (MASK_BIT0 << index) & irq_src)
            {
                HSI_SET_WMARKICx(g_hsi_system_context.p_hsi_tx->hsit_wmarkic, index);
            }
        }
    }

    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_RX_IRQ0 | (t_uint32) HSI_IRQ_SRC_RX_IRQ1 | (t_uint32) HSI_IRQ_SRC_RX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_RX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ7
                )
        ) != NULL
    )
    {
        for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
        {
            if ((t_uint32) (MASK_BIT0 << (index + HSI_MAX_CHANNEL_NUM)) & irq_src)
            {
                HSI_SET_WMARKICx(g_hsi_system_context.p_hsi_rx->hsir_wmarkic, index);
            }
        }
    }

}



/****************************************************************************/
/* NAME:	 HSI_IsPendingIRQSrc() 											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This function allows to know if the IRQ Source is 		*/
/*				  active or not.t_hsi_idIRQSrc is a single interrupt.		*/
/* PARAMETERS    :															*/
/* IN            : irq_src -- irqsource identification						*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : t_bool												    */
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_bool HSI_IsPendingIRQSrc(IN t_hsi_irq_src irq_src)
{
    t_uint32    it_status = NULL;
    
    if (irq_src > (t_uint32) HSI_IRQ_SRC_ALL_IT)
    {
        return(FALSE);
    }

    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_TX_IRQ0 | (t_uint32) HSI_IRQ_SRC_TX_IRQ1 | (t_uint32) HSI_IRQ_SRC_TX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_TX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ7
                )
        ) != NULL
    )
    {
        it_status |= ((HSI_GET_WMARKMISx_ALL(g_hsi_system_context.p_hsi_tx->hsit_wmarkmis) & irq_src));
    }

    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_RX_IRQ0 | (t_uint32) HSI_IRQ_SRC_RX_IRQ1 | (t_uint32) HSI_IRQ_SRC_RX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_RX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ7
                )
        ) != NULL
    )
    {
        it_status |= ((HSI_GET_WMARKMISx_ALL(g_hsi_system_context.p_hsi_rx->hsir_wmarkmis) << SHIFT_BYTE1) & irq_src);
    }

    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_TIMEOUT)
    {
        it_status |= ((HSI_RX_GET_EX_TIMEOUT(g_hsi_system_context.p_hsi_rx->hsir_excepmis) << (SHIFT_BYTE2 + HSI_RX_ESR_SHIFT_TIMEOUT)) & irq_src);
    }

    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_OVERRUN)
    {
        it_status |= ((HSI_GET_RX_EX_OVR(g_hsi_system_context.p_hsi_rx->hsir_excepmis) << (SHIFT_BYTE2 + HSI_RX_ESR_SHIFT_OVR)) & irq_src);
    }
    
    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_BREAK)
    {
        it_status |= ((HSI_GET_RX_EX_BREAK(g_hsi_system_context.p_hsi_rx->hsir_excepmis) << (SHIFT_BYTE2 + HSI_RX_ESR_SHIFT_BREAK)) & irq_src);

    }
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    if (irq_src & (t_uint32) HSI_IRQ_SRC_RX_EX_PARITY)
    {
        it_status |= ((HSI_GET_RX_EX_PARITY(g_hsi_system_context.p_hsi_rx->hsir_excepmis) << (SHIFT_BYTE2 + HSI_RX_ESR_SHIFT_PARITY)) & irq_src);

    }
#endif

    if(irq_src & ((t_uint32) HSI_IRQ_SRC_RX_EX_OV0 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV1 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV2 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV3 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV4 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV5 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV6 |
                 (t_uint32) HSI_IRQ_SRC_RX_EX_OV7 
       ))
    {
         it_status |= ((HSI_GET_RX_OVERRUNMISx_ALL(g_hsi_system_context.p_hsi_rx->hsir_overrunmis) << SHIFT_QUARTET5) & irq_src);
    }

    
    if (it_status != NULL)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

