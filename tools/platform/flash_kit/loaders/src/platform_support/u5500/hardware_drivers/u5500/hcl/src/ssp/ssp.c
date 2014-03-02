/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the PL022 (SSP)
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "ssp_p.h"

/*--------------------------------------------------------------------------*
 * debug stuff																*
 *--------------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_SSP
#define MY_DEBUG_ID             myDebugID_SSP
PRIVATE t_dbg_level     MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id        MY_DEBUG_ID = SSP_HCL_DBG_ID;
#endif

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
PUBLIC t_ssp_system_context    g_ssp_system_context[NUM_SSP_INSTANCES];

/****************************************************************************/
/* NAME:	t_ssp_error SSP_Init ();										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the SSP registers.					*/
/* PARAMETERS:																*/
/* IN : t_logical_address ssp_base_address:	SSP registers base address 	    */
/* OUT: none 																*/
/* RETURN: t_ssp_error 														*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/
/****************************************************************************/
PUBLIC t_ssp_error SSP_Init(IN t_ssp_device_id ssp_device_id, IN t_logical_address ssp_base_address)
{
    t_ssp_error ssp_error;
    DBGENTER0();
    if (ssp_base_address == NULL)
    {
        ssp_error = SSP_UNSUPPORTED_HW;
        DBGEXIT0(ssp_error);
        return(ssp_error);
    }

    /* Initialize the SSP registers structure */
    g_ssp_system_context[ssp_device_id].p_ssp_register = (t_ssp_register *) ssp_base_address;

    /* Check Peripheral and Pcell Id Register for SSP */
    if
    (
        (SSP_PERIPHID0 == g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_periphid0)
    &&  (SSP_PERIPHID1 == g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_periphid1)
    &&  (SSP_PERIPHID2 == g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_periphid2)
    &&  (SSP_PERIPHID3 == g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_periphid3)
    &&  (SSP_PCELLID0 == g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_pcellid0)
    &&  (SSP_PCELLID1 == g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_pcellid1)
    &&  (SSP_PCELLID2 == g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_pcellid2)
    &&  (SSP_PCELLID3 == g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_pcellid3)
    )
    {
        ssp_error = SSP_OK;
    }
    else
    {
        ssp_error = SSP_INVALID_PARAMETER;
    }

    DBGEXIT0(ssp_error);
    return(ssp_error);
}

/**********************************************************************************************/
/* NAME:	t_ssp_error SSP_GetVersion()									     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns version for SSP HCL.							          */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		NONE																		  */
/* OUT : 		t_version*   : p_version	Version for SSP HCL								  */
/* 																			                  */
/* RETURN:		t_ssp_error :          SSP error code							          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant																	  */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_GetVersion(OUT t_version *p_version)
{
    DBGENTER0();
    if (p_version == NULL)
    {
        DBGEXIT0(SSP_INVALID_PARAMETER);
        return(SSP_INVALID_PARAMETER);
    }

    p_version->minor = SSP_HCL_MINOR_ID;
    p_version->major = SSP_HCL_MAJOR_ID;
    p_version->version = SSP_HCL_VERSION_ID;

    DBGEXIT0(SSP_OK);
    return(SSP_OK);
}

/**********************************************************************************************/
/* NAME:	t_ssp_error SSP_SetDbgLevel()									     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different debug comments levels        */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_dbg_level :		    DebugLevel from DEBUG_LEVEL0 to DEBUG_LEVEL4.      	  */
/* OUT: 		none																		  */
/* 																			                  */
/* RETURN:		t_ssp_error :			SSP error code							          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non-reentrant												                  */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_SetDbgLevel(IN t_dbg_level ssp_dbg_level)
{
    DBGENTER1(" (%x)", ssp_dbg_level);
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = ssp_dbg_level;
#endif
    DBGEXIT0(SSP_OK);
    return(SSP_OK);
}

/**********************************************************************************************/
/* NAME:	t_ssp_error SSP_Enable()										     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables Tx-Rx of SSP.							      */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_ssp_enable rx_tx_enable:		    Enable/Disable SSP Rx, Tx  	          	  */
/* OUT:         none																		  */
/* 																			                  */
/* RETURN:		t_ssp_error :						SSP error code				          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_Enable(IN t_ssp_device_id ssp_device_id, IN t_ssp_enable rx_tx_enable)
{
    t_bool  rx_enable = FALSE;
    t_bool  tx_enable = FALSE;

    DBGENTER1(" (%x)", rx_tx_enable);

    if (SSP_DISABLE_RX_TX != rx_tx_enable)
    {
        /* Set SSE bit in SSP_CR1 */
        SSP_SET_SSE(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 1);

        if (SSP_ENABLE_RX_TX == rx_tx_enable)
        {
            rx_enable = TRUE;
            tx_enable = TRUE;
        }
        else
        {
            if (SSP_ENABLE_RX_DISABLE_TX == rx_tx_enable)
            {
                rx_enable = TRUE;
            }

            if (SSP_DISABLE_RX_ENABLE_TX == rx_tx_enable)
            {
                tx_enable = TRUE;
            }
        }
    }
    else
    {   /* Clear SSE bit in SSP_CR1 */
        SSP_SET_SSE(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 0);
    }

    /* Set DMACR register values */
    if (SSP_DMA_MODE == g_ssp_system_context[ssp_device_id].tx_com_mode)
    {
        if (tx_enable)
        {
            g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dmacr |= SSP_DMACR_MASK_TXDMAE;     /* tx is through dma  and tx is enabled.*/
        }
        else
        {
            g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dmacr &= ~SSP_DMACR_MASK_TXDMAE;    /* tx is through dma  and tx is disabled.*/
        }
    }
    else
    {
        g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dmacr &= ~SSP_DMACR_MASK_TXDMAE;        /*  tx is not through dma mode.*/
    }

    if (SSP_DMA_MODE == g_ssp_system_context[ssp_device_id].rx_com_mode)
    {
        if (rx_enable)
        {
            g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dmacr |= SSP_DMACR_MASK_RXDMAE;     /* rx is through dma  and rx is enabled.*/
        }
        else
        {
            g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dmacr &= ~SSP_DMACR_MASK_RXDMAE;    /* rx is through dma  and rx is disabled.*/
        }
    }
    else
    {
        g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dmacr &= ~SSP_DMACR_MASK_RXDMAE;        /* rx is not through dma mode.*/
    }

    DBGEXIT0(SSP_OK);
    return(SSP_OK);
}

/**********************************************************************************************/
/* NAME:	t_ssp_error SSP_EnableLoopBackMode()							     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns TRUE if SSP is master o.w. it returns FALSE	  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		lbm_enable													  	          	  */
/* OUT:			NONE																		  */
/* 																			                  */
/* RETURN:		t_ssp_error :	SSP_OK, if SSP Loop Back Mode is enabled		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_EnableLoopBackMode(IN t_ssp_device_id ssp_device_id, IN t_bool lbm_enable)
{
    t_bool  status = TRUE;

    DBGENTER0();

    if (status == lbm_enable)
    {
        /* Set LBM bit in SSP_CR1 */
        SSP_SET_LBM(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1,SSP_MASKBIT0);
    }
    else
    {   /* Clear LBM bit in SSP_CR1 */
        SSP_SET_LBM(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1,NULL);
    }

    DBGEXIT0(SSP_OK);
    return(SSP_OK);
}

/**********************************************************************************************/
/* NAME:	void SSP_Reset()													     		  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns TRUE if SSP is Reset.o.W.it returns FALSE	 	  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		NONE														  	          	  */
/* OUT:			NONE																		  */
/* 																			                  */
/* RETURN:		void :															          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC void SSP_Reset(IN t_ssp_device_id ssp_device_id)
{
    t_uint32    rx_element;
    t_uint32    rx_fifo_not_empty = SSP_RIS_MASK_RXRIS;
    t_uint32    fifo_status;
    t_ssp_error ssp_error = SSP_OK;
    
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_imsc &= ~(SSP_RIS_MASK_RORRIS | SSP_RIS_MASK_RTRIS | SSP_RIS_MASK_RXRIS | SSP_RIS_MASK_TXRIS);

    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_icr |= (SSP_ICR_MASK_RORIC | SSP_ICR_MASK_RTIC);

    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dmacr = SSP_ALLZERO;
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0 = SSP_ALLZERO;
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1 = SSP_ALLZERO;

    DBGENTER0();

    if (SSP_GET_LBM(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1))
    {
        ssp_error = SSP_INTERNAL_ERROR;
        DBGEXIT0(ssp_error);
        return;
    }

    /* flush receive fifo */
    do
    {
    	ssp_error = SSP_GetFIFOStatus(ssp_device_id, &fifo_status);
    	
        if ((ssp_error) != SSP_OK)
        {
            DBGEXIT0(ssp_error);
            return;
        }

        if (fifo_status & rx_fifo_not_empty)
        {
        	ssp_error = SSP_GetData(ssp_device_id, &rx_element);
        	
            if ((ssp_error) != SSP_OK)
            {
                DBGEXIT0(ssp_error);
                return;
            }
        }
    } while (fifo_status & rx_fifo_not_empty);


        DBGEXIT0(ssp_error);
        return;
}

/**********************************************************************************************/
/* NAME:	t_bool ssp_IsSspMaster()										     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns TRUE if SSP is master o.w. it returns FALSE	  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		NONE														  	          	  */
/* OUT:			NONE																		  */
/* 																			                  */
/* RETURN:		t_bool :	TRUE, if SSP is master otherwise FALSE				          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PRIVATE t_bool ssp_IsSspMaster(IN t_ssp_device_id ssp_device_id)
{
    DBGENTER0();
    if (g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1 & SSP_CR1_MASK_MS)
    {
        DBGEXIT0(SSP_OK);
        return(FALSE);
    }
    else
    {
        DBGEXIT0(SSP_OK);
        return(TRUE);
    }
}

/**********************************************************************************************/
/* NAME:	t_ssp_error SSP_ResolveClockFrequency()							     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine calculates the value of effective frequency.				      */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_uint32 ssp_freq:		    Frequency of the clock that feeds SSP HW IP    	  */
/*				t_uint32 target_freq:		Target frequency when SSP is master				  */
/* OUT:			t_uint32 *p_effective_freq	Closest frequency that SSP can effectively achieve*/
/*				t_ssp_clock_params *p_clock_parameters									      */
/*											Clock parameters corresponding to effectiveFreq   */
/* 																			                  */
/* RETURN:		t_ssp_error :						SSP error code				          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_ResolveClockFrequency
(
    IN  t_uint32             ssp_freq,
    IN  t_uint32             target_freq,
    OUT t_uint32             *p_effective_freq,
    OUT t_ssp_clock_params   *p_clock_parameters
)
{
    t_ssp_error status = SSP_OK;
    t_uint32    cpsdvsr = MIN_CPSDVR;
    t_uint32    scr = NULL;
    t_bool      freq_found = FALSE;
    t_uint32    max_tclk;
    t_uint32    min_tclk;

    DBGENTER2(" (%ld %ld) ", ssp_freq, target_freq);

    /* Check for null pointers */
    if ((p_effective_freq == NULL) || (p_clock_parameters == NULL))
    {
        status = SSP_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    /* Calculate effective frequency only in master mode, o.w. return error */
    max_tclk = (ssp_freq / (MIN_CPSDVR * (SSP_MASKBIT0 + MIN_SCR)));   /* cpsdvscr = 2 & scr 0 */
    min_tclk = (ssp_freq / (MAX_CPSDVR * (SSP_MASKBIT0 + MAX_SCR)));   /* cpsdvsr = 254 & scr = 255 */

    if ((target_freq <= max_tclk) && (target_freq >= min_tclk))
    {
        while (cpsdvsr <= MAX_CPSDVR && !freq_found)
        {
            while (scr <= MAX_SCR && !freq_found)
            {
                if ((ssp_freq / (cpsdvsr * (SSP_MASKBIT0 + scr))) > target_freq)
                {
                    scr += SSP_MASKBIT0;
                }
                else
                {
                    /* This bool is made TRUE when effective frequency >= target frequency is found*/
                    freq_found = TRUE;
                    if ((ssp_freq / (cpsdvsr * (SSP_MASKBIT0 + scr))) != target_freq)
                    {
                        if (scr == MIN_SCR)
                        {
                            cpsdvsr -= MIN_CPSDVR;
                            scr = MAX_SCR;
                        }
                        else
                        {
                            scr -= SSP_MASKBIT0;
                        }
                    }
                }   /* end of else */
            }       /* end of inner while */

            if (!freq_found)
            {
                cpsdvsr += MIN_CPSDVR;
                scr = MIN_SCR;
            }
        }           /* end of outer while */

        if (cpsdvsr != NULL)
        {
            *p_effective_freq = ssp_freq / (cpsdvsr * (SSP_MASKBIT0 + scr));
            p_clock_parameters->cpsdvsr = (t_uint8) (cpsdvsr & SSP_LAST8BITS);
            p_clock_parameters->scr = (t_uint8) (scr & SSP_LAST8BITS);
        }
    }
    else
    {
        status = SSP_INVALID_PARAMETER;
        *p_effective_freq = NULL;
        DBGEXIT0(status);
        return(status);
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	 t_ssp_error SSP_CheckConfiguration()									          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine checks the configuration parameters.							  */
/* PARAMETERS:																                  */
/* IN : 		t_ssp_config  *p_Config:   structure holding the config parameters			  */
/* OUT: 		none														                  */
/* 																			                  */
/* RETURN:		t_ssp_error :            SSP error code										  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_CheckConfiguration(IN t_ssp_config *p_config)
{
    t_ssp_error status = SSP_OK;
    DBGENTER0();

    /*Check for null pointer. */
    if (p_config == NULL)
    {
        status = SSP_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	 t_ssp_error SSP_SetConfiguration()									              */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures SSP registers.										  */
/* PARAMETERS:																                  */
/* IN : 		t_ssp_config  *p_config  pointer to the structure							  */
/*										 holding the config parameters.						  */
/* OUT : 		NONE														                  */
/* 																			                  */
/* RETURN:		t_ssp_error :            SSP error code										  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant														          */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_SetConfiguration(IN t_ssp_device_id ssp_device_id, IN t_ssp_config *p_config)
{
    t_ssp_error status = SSP_OK;
    t_uint32    prescale_dvsr;

    DBGENTER0();

    /*Check for null pointer. */
    if (p_config == NULL)
    {
        status = SSP_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    /* Set FRF bit to interface type */
    SSP_SET_FRF(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0, (t_uint32) p_config->iface);

    /* Set MS bit to master/slave hierarchy */
    SSP_SET_MS(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, (t_uint32) p_config->hierarchy);

    /* Set SCR and CPSDVSR bits to clock parameters */
    if (ssp_IsSspMaster(ssp_device_id))
    {
        prescale_dvsr = (t_uint32) p_config->clk_freq.cpsdvsr;
        if ((prescale_dvsr % MIN_CPSDVR) != NULL)
        {
            prescale_dvsr = prescale_dvsr - SSP_MASKBIT0;  /* make it even */
        }

        SSP_SET_CPSDVSR(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cpsr, prescale_dvsr);
        SSP_SET_SCR(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0, (t_uint32) p_config->clk_freq.scr);
    }

    /* Set Endianness */
    switch (p_config->endian)
    {
        case SSP_RX_MSB_TX_MSB:
            SSP_SET_RENDN(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 0);
            SSP_SET_TENDN(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 0);
            break;

        case SSP_RX_MSB_TX_LSB:
            SSP_SET_RENDN(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 0);
            SSP_SET_TENDN(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 1);
            break;

        case SSP_RX_LSB_TX_MSB:
            SSP_SET_RENDN(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 1);
            SSP_SET_TENDN(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 0);
            break;

        case SSP_RX_LSB_TX_LSB:
            SSP_SET_RENDN(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 1);
            SSP_SET_TENDN(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, 1);
            break;

        default:
            status = SSP_INVALID_PARAMETER;
            DBGEXIT0(status);
            return(status);
    }

    SSP_SET_DSS(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0, (t_uint32) (p_config->data_size));

    /* Set g_ssp_system_context.tx_com_mode and g_ssp_system_context.rx_com_mode to the communication mode */
    g_ssp_system_context[ssp_device_id].tx_com_mode = p_config->txcom_mode;
    g_ssp_system_context[ssp_device_id].rx_com_mode = p_config->rxcom_mode;

    SSP_SET_RXIFLSEL(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, (t_uint32) p_config->rx_lev_trig);
    SSP_SET_TXIFLSEL(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, (t_uint32) p_config->tx_lev_trig);

    /* Set clock phase and polarity */
    if (p_config->iface == SSP_INTERFACE_MOTOROLA_SPI)
    {
        SSP_SET_SPO(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0, (t_uint32) p_config->clk_pol);
        SSP_SET_SPH(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0, (t_uint32) p_config->clk_phase);
    }

    if (SSP_INTERFACE_NATIONAL_MICROWIRE == p_config->iface)
    {
        SSP_SET_CSS(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0, (t_uint32) p_config->ctrl_len);
        SSP_SET_MWAIT(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, (t_uint32) p_config->wait_state);
    }

    /* Set Half or Full duplex */
    if (SSP_INTERFACE_NATIONAL_MICROWIRE == p_config->iface || SSP_INTERFACE_UNIDIRECTIONAL == p_config->iface)
    {
        SSP_SET_HALFDUP(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0, (t_uint32) p_config->duplex);
    }
    else
    {
        if
        (
            (
            p_config->iface != SSP_INTERFACE_NATIONAL_MICROWIRE
        &&  p_config->iface != SSP_INTERFACE_UNIDIRECTIONAL
            )
            &&  (SSP_MICROWIRE_CHANNEL_FULL_DUPLEX == p_config->duplex)
        )
        {
            SSP_SET_HALFDUP(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0, (t_uint32) p_config->duplex);
        }
        else
        {
            status = SSP_REQUEST_NOT_APPLICABLE;
            DBGEXIT0(status);
            return(status);
        }
    }

    /* Set SOD bit in slave mode*/
    if (!ssp_IsSspMaster(ssp_device_id))
    {
        SSP_SET_SOD(g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1, (t_uint32) p_config->slave_tx_disable);
    }

    /* Clear Receive timeout and overrun Interrupts */
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_icr = SSP_ICR_MASK_RORIC | SSP_ICR_MASK_RTIC;

    /* Disable all Interrupts */
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_imsc = SSP_DISABLE_ALL_INTERRUPT;

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	t_ssp_error		SSP_GetData()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets data from the receive FIFO								  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		none 																		  */
/* OUT : 		t_uint32    *p_dataelement: represents the data element in the Rx FIFO		  */
/*              																			  */
/* 																			                  */
/* RETURN:		t_ssp_error :              SSP error code									  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_GetData(IN t_ssp_device_id ssp_device_id, OUT t_uint32 *p_dataelement)
{
    t_ssp_error status = SSP_OK;
    DBGENTER0();

    /*Check for null pointer. */
    if (p_dataelement == NULL)
    {
        status = SSP_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    if (g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_sr & SSP_SR_MASK_RNE)
    {
        *p_dataelement = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dr & SSP_DR_MASK_DATA;
    }
    else
    {
        status = SSP_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	t_ssp_error		SSP_SetData()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine puts data in the transmit FIFO									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_uint32    data				: the data to be put in TX fifo				  */
/* OUT: 		none																		  */
/* 																			                  */
/* RETURN:		t_ssp_error :            SSP error code										  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_SetData(IN t_ssp_device_id ssp_device_id, IN t_uint32 data)
{
    t_ssp_error status = SSP_OK;
    DBGENTER0();
    if (g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_sr & SSP_SR_MASK_TNF)
    {
        g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dr = data & SSP_DR_MASK_DATA;
    }
    else
    {
        status = SSP_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	t_ssp_error		SSP_GetFIFOstatus()												  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to get the value of status register					  */
/* PARAMETERS:																                  */
/* IN :			none 																		  */
/* OUT : 		t_uint32			*p_dataelement: value of status register				  */
/* RETURN:		t_ssp_error :       SSP error code											  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_GetFIFOStatus(IN t_ssp_device_id ssp_device_id, OUT t_uint32 *p_dataelement)
{
    t_ssp_error status;
    DBGENTER0();

    /*Check for null pointer. */
    if (p_dataelement == NULL)
    {
        status = SSP_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    *p_dataelement = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_sr;
    if (g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_sr & SSP_SR_MASK_RNE)
    {
        status = SSP_OK;
    }
    else
    {
        status = SSP_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	t_ssp_error		SSP_GETTxMode()				    								  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to get the mode of the SSP Transmitter					  */
/* PARAMETERS:																                  */
/* IN :			t_ssp_device_id     ssp_device_id:SSP Device ID								  */
/* OUT : 		t_ssp_mode			ssp_mode: SSP Tx mode                   				  */
/* RETURN:		t_ssp_error :       SSP error code											  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_GetTxMode(IN t_ssp_device_id ssp_device_id,OUT t_ssp_mode  *ssp_mode)
{
    t_ssp_error error_ssp;
    DBGENTER0();

    switch(ssp_device_id)
    {
    case SSP_DEVICE_ID_0:
        *ssp_mode = g_ssp_system_context[ssp_device_id].tx_com_mode;
        error_ssp = SSP_OK;
        break;

    case SSP_DEVICE_ID_1:
        *ssp_mode = g_ssp_system_context[ssp_device_id].tx_com_mode;
        error_ssp = SSP_OK;
        break;

    default:
        error_ssp = SSP_INVALID_PARAMETER;
        break;

    }

    DBGEXIT0(error_ssp);
    return(error_ssp);

}

/**********************************************************************************************/
/* NAME:	t_ssp_error		SSP_GETRxMode()				    								  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to get the mode of the SSP Reciever					  */
/* PARAMETERS:																                  */
/* IN :			t_ssp_device_id     ssp_device_id:SSP Device ID								  */
/* OUT : 		t_ssp_mode			ssp_mode: SSP Tx mode                   				  */
/* RETURN:		t_ssp_error :       SSP error code											  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */
/**********************************************************************************************/
PUBLIC t_ssp_error SSP_GetRxMode(IN t_ssp_device_id ssp_device_id,OUT t_ssp_mode *ssp_mode)
{
    t_ssp_error error_ssp;
    DBGENTER0();

    switch(ssp_device_id)
    {
    case SSP_DEVICE_ID_0:
        *ssp_mode = g_ssp_system_context[ssp_device_id].rx_com_mode;
        error_ssp = SSP_OK;
        break;

    case SSP_DEVICE_ID_1:
        *ssp_mode = g_ssp_system_context[ssp_device_id].rx_com_mode;
        error_ssp = SSP_OK;
        break;

    default:
        error_ssp = SSP_INVALID_PARAMETER;
        break;

    }

    DBGEXIT0(error_ssp);
    return(error_ssp);

}

/**********************************************************************************************/
/* NAME:	void SSP_SaveDeviceContext()										     	 	  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine saves current of SSP hardware for power management.			  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN			NONE											 	  					  	  */
/* OUT			NONE																		  */
/* RETURN:		void																		  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												                  */
/* Global array g_ssp_system_context.device_context[9] is being modified					  */
/**********************************************************************************************/
PUBLIC void SSP_SaveDeviceContext(IN t_ssp_device_id ssp_device_id)
{
    DBGENTER0();
    g_ssp_system_context[ssp_device_id].device_context[0] = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0;
    g_ssp_system_context[ssp_device_id].device_context[1] = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1;
    g_ssp_system_context[ssp_device_id].device_context[2] = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cpsr;
    g_ssp_system_context[ssp_device_id].device_context[3] = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_imsc;
    g_ssp_system_context[ssp_device_id].device_context[4] = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dmacr;
    g_ssp_system_context[ssp_device_id].device_context[5] = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_itcr;
    g_ssp_system_context[ssp_device_id].device_context[6] = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_itip;
    g_ssp_system_context[ssp_device_id].device_context[7] = g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_itop;
    DBGEXIT0(SSP_OK);
}

/**********************************************************************************************/
/* NAME:	void SSP_RestoreDeviceContext()										     	 	  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine restore current of SSP hardware.								  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN			NONE											 	  					  	  */
/* OUT			NONE																		  */
/* RETURN:		void																		  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY:Reentrant														                  */
/**********************************************************************************************/
PUBLIC void SSP_RestoreDeviceContext(IN t_ssp_device_id ssp_device_id)
{
    DBGENTER0();
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr0 = g_ssp_system_context[ssp_device_id].device_context[0];
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cpsr = g_ssp_system_context[ssp_device_id].device_context[2];
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_itcr = g_ssp_system_context[ssp_device_id].device_context[5];
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_itip = g_ssp_system_context[ssp_device_id].device_context[6];
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_itop = g_ssp_system_context[ssp_device_id].device_context[7];
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_cr1 = g_ssp_system_context[ssp_device_id].device_context[1];
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_dmacr = g_ssp_system_context[ssp_device_id].device_context[4];
    g_ssp_system_context[ssp_device_id].p_ssp_register->ssp_imsc = g_ssp_system_context[ssp_device_id].device_context[3];
    DBGEXIT0(SSP_OK);
}

