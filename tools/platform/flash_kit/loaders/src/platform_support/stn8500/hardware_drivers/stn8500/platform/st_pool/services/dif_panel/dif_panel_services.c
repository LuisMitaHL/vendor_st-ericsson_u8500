/****************************************************************************
 *                            C STMicroelectronics
 *    Reproduction and Communication of this document is strictly prohibited 
 *      unless specifically authorized in writing by STMicroelectronics.
 *------------------------------------------------------------------------
 * dif_services.c
 * This file provides initialization services for DIF
 *------------------------------------------------------------------------
 *
 ****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "hcl_defs.h"
#include "mupoc_mapping.h"
#include "dma.h"
#include "dif.h"

#if defined(__DIF_PANEL_C2_M1)
#include "dif_panel_c2_m1.h"
#endif
#if defined(__DIF_PANEL_C1_L2F50011)
#include "dif_panel_c1_l2f50011.h "
#endif
#include "vic.h"
#include "tmr.h"

#include "services.h"
#include "dif_panel_services.h"
#include "tmr_services.h"

#if defined(__DIF_PANEL_C1_L2F50011)
#define PANEL(var)      DIF_PANEL_C1_L2F50011_##var
#define VARDECL(var)    t_dif_panel_c1_l2f50011_##var
#endif
#if defined(__DIF_PANEL_C2_M1)
#define PANEL(var)      DIF_PANEL_C2_M1_##var
#define VARDECL(var)    t_dif_panel_c2_m1_##var
#endif

/*--------------------------------------------------------------------------*
 * Global variables  						            					*	
 *--------------------------------------------------------------------------*/
t_idif_internal idif_internal;

/*--------------------------------------------------------------------------*
 * Private functions						            					*	
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_DIF_Panel_DoTransmit                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION :This routine is called by timer interrupt to update     	*/
/*               dif internal buffer  										*/
/*																			*/
/* PARAMETERS :                                              		        */
/*        IN  :None    														*/
/*     InOut  :None                                                         */
/* 		OUT   :None                                                	        */
/*                                                                          */
/* RETURN	  :None                                 				   	    */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PRIVATE void SER_DIF_Panel_DoTransmit(void)
{


#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
    t_dma_error                 dma_error;
    t_dma_ctrl_id               dma_id;
    t_dma_exchange_status       status;
#endif
#if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
    t_dif_frame_transfer_status status;
    t_dif_error                 dif_error;    
#endif
    if (SER_DIF_STATE_RUNNING == idif_internal.status)
    {
#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
        dma_error = DMA_GetExchangeStatus(idif_internal.dma_id, idif_internal.exchangeid, &status);
           /*Initiate Panel Transfer */
        if (DMA_OK != dma_error)
        {
               PRINT("Unable to initiate panel transfert\n");
               return;
        }
        
        if ((DMA_EXCHANGE_ON_GOING != status) && (DMA_EXCHANGE_QUEUED != status))
        {
 
            dma_error = DMA_Transmit
                (
                    idif_internal.dma_id,
                    idif_internal.frame_buffer_addr,
                    idif_internal.tsize,
                    FALSE,
                    TRUE,
                    &dma_id,
                    &idif_internal.exchangeid
                );
            /* Added to remove the warning */
            dma_error = dma_error;                
        }
 
#endif
#if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
        dif_error = DIF_GetFrameTransferStatus(&status);
        if (DIF_OK != dif_error)
        {
            PRINT("Error in DIF_GetFrameTransferStatus\n");
            return;
        }

        if (DIF_FRAME_TRANSFER_ENDED == status)
        {
            dif_error = DIF_ConstructFrameDesc(idif_internal.frame_desc);
            if (DIF_OK != dif_error)
            {
                PRINT("Unable to construct frame descriptor\n");
                return;
            }

            dif_error = DIF_StartFrameTransfer();
            if (DIF_OK != dif_error)
            {
                PRINT("Unable to start frame transfer\n");
                return;
            }
        }
#endif
        else
        {
            PRINT("Drop frame\n");
        }
    }
}

/****************************************************************************/
/* NAME:    SER_DIF_Panel_Configure                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configure dma/timer/dif according to given 	*/
/*			connected pannel ID and color_depth and frame rate              */
/*																			*/
/* PARAMETERS:                                              		        */
/*         IN:     															*/
/*		        - panel_id     : Panel ID						            */
/*              - color_depth : color depth of frame buffer                 */
/*              - refresh_rate : refresh rate of display                    */
/*              - frame_buffer_addr : frame buffer address                  */
/*     InOut None                                                           */
/* 		OUT  None                                                	        */
/*                                                                          */
/* RETURN	 :t_ser_dif_error                                 			    */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_ser_dif_error SER_DIF_Panel_Configure
(
    IN t_ser_dif_color_depth    color_depth,
    IN t_uint8                  refresh_rate,
    IN t_physical_address       frame_buffer_addr
)
{
    VARDECL(disp_settings) panel;

    t_dif_error         dif_error;
    t_sint32            tmr_ser_error;
#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
    t_dma_access_width  dma_width;
    t_dma_error         dma_error;
    t_uint32            tsize;
#endif
#if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
    t_dif_frame_desc    frame_desc;
#endif
    t_tmr_error         tmr_error;

#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
    /* Open DMA Pipe */
    /*    idif_internal.dma_id=DMA_OpenPipe(MEMORY_DMA_DEVICE ,DIF_TX_DMA_DEVICE,FALSE,HIGH_PRIORITY);*/
    dma_error = DMA_OpenPipe
        (
            DMA_MEMORY_DEVICE,
            DMA_DIF_TX_DEVICE,
            DMA_CTRL_NO_PREFERENCE,
            FALSE,
            FALSE,
            DMA_EXCH_PRIORITY_HIGH,
            &idif_internal.dma_id
        );
    if (DMA_OK != dma_error)
    {
        PRINT("Unable to open dma pipe\n");
        return(SER_DIF_ERROR);
    }
#endif

    /* Allocate Available timer and connect callback */
    tmr_error = TMR_AllocTimer(&idif_internal.tmr_id);
    if (TMR_OK != tmr_error)
    {
        PRINT("tmr error\n");
        return(SER_DIF_ERROR);
    }

    tmr_ser_error = SER_TMR_ConnectonTimer(idif_internal.tmr_id, (t_callback_fct) SER_DIF_Panel_DoTransmit, 0);
    if (HCL_OK != tmr_ser_error)
    {
        return(SER_DIF_ERROR);
    }

    /*test parameters validity*/
    if (refresh_rate < MIN_REFRESH_RATE || refresh_rate > MAX_REFRESH_RATE)
    {
        PRINT("Bad frame rate\n");
        return(SER_DIF_ERROR);
    }

    if (NULL == frame_buffer_addr)
    {
        PRINT("Bad frame buffer address\n");
        return(SER_DIF_ERROR);
    }

    switch (color_depth)
    {
        case SER_DIF_COLOR_8BITS:
            panel.mode = PANEL(COLOR_RGB);
            panel.order = PANEL(LITTLE_ENDIAN);
            panel.bpp = PANEL(8_BPP);
    #if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
            dma_width = DMA_HALFWORD_WIDTH;
            tsize = DIF_LCD_HEIGHT * DIF_LCD_WIDTH;
    #endif
    #if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
            frame_desc.frame_start_addr = (t_physical_address) frame_buffer_addr;
            frame_desc.elem_desc.elem_incr = 2;
            frame_desc.elem_desc.num_elem = DIF_LCD_WIDTH / 2;
            frame_desc.elem_desc.elem_size = 2;
            frame_desc.line_desc.line_incr = frame_desc.elem_desc.num_elem * frame_desc.elem_desc.elem_size;
            frame_desc.line_desc.num_line = DIF_LCD_HEIGHT;
            frame_desc.line_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.frame_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.type = DIF_TRANSACTION_DATA;
            frame_desc.is_eof_intr_enabled = TRUE;
            frame_desc.is_frame_self_chained = FALSE;
    #endif
            break;

        case SER_DIF_COLOR_12BITS:
            panel.mode = PANEL(COLOR_RGB);
            panel.order = PANEL(LITTLE_ENDIAN);
            panel.bpp = PANEL(12_BPP_UNPACKED);
    #if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
            dma_width = DMA_WORD_WIDTH;
            tsize = DIF_LCD_HEIGHT * DIF_LCD_WIDTH * 2;
    #endif
    #if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
            frame_desc.frame_start_addr = (t_physical_address) frame_buffer_addr;
            frame_desc.elem_desc.elem_incr = 4;
            frame_desc.elem_desc.num_elem = DIF_LCD_WIDTH / 2;
            frame_desc.elem_desc.elem_size = 4;
            frame_desc.line_desc.line_incr = frame_desc.elem_desc.num_elem * frame_desc.elem_desc.elem_size;
            frame_desc.line_desc.num_line = DIF_LCD_HEIGHT;
            frame_desc.line_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.frame_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.type = DIF_TRANSACTION_DATA;
            frame_desc.is_eof_intr_enabled = TRUE;
            frame_desc.is_frame_self_chained = FALSE;
    #endif
            break;

        case SER_DIF_COLOR_16BITS:
            panel.mode = PANEL(COLOR_RGB);
            panel.order = PANEL(LITTLE_ENDIAN);
            panel.bpp = PANEL(16_BPP);
    #if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
            dma_width = DMA_WORD_WIDTH;
            tsize = DIF_LCD_HEIGHT * DIF_LCD_WIDTH * 2;
    #endif
    #if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
            frame_desc.frame_start_addr = (t_physical_address) frame_buffer_addr;
    #if defined(__DIF_PANEL_C1_L2F50011)
            frame_desc.elem_desc.elem_incr = 2;
            frame_desc.elem_desc.num_elem = DIF_LCD_WIDTH;
            frame_desc.elem_desc.elem_size = 2;
    #endif
    #if defined(__DIF_PANEL_C2_M1)
            frame_desc.elem_desc.elem_incr = 4;
            frame_desc.elem_desc.num_elem = DIF_LCD_WIDTH / 2;
            frame_desc.elem_desc.elem_size = 4;
    #endif
            frame_desc.line_desc.line_incr = frame_desc.elem_desc.num_elem * frame_desc.elem_desc.elem_size;
            frame_desc.line_desc.num_line = DIF_LCD_HEIGHT;
            frame_desc.line_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.frame_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.type = DIF_TRANSACTION_DATA;
            frame_desc.is_eof_intr_enabled = TRUE;
            frame_desc.is_frame_self_chained = FALSE;
    #endif
            break;

        case SER_DIF_COLOR_24BITS:
            panel.mode = PANEL(COLOR_RGB);
            panel.order = PANEL(LITTLE_ENDIAN);
            panel.bpp = PANEL(24_BPP_PACKED);
    #if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
            dma_width = DMA_WORD_WIDTH;
            tsize = DIF_LCD_HEIGHT * DIF_LCD_WIDTH * 3;
    #endif
    #if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
            frame_desc.frame_start_addr = (t_physical_address) frame_buffer_addr;
            frame_desc.elem_desc.elem_incr = 4;
            frame_desc.elem_desc.num_elem = (DIF_LCD_WIDTH * 3) / 4;
            frame_desc.elem_desc.elem_size = 4;
            frame_desc.line_desc.line_incr = frame_desc.elem_desc.num_elem * frame_desc.elem_desc.elem_size;
            frame_desc.line_desc.num_line = DIF_LCD_HEIGHT;
            frame_desc.line_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.frame_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.type = DIF_TRANSACTION_DATA;
            frame_desc.is_eof_intr_enabled = TRUE;
            frame_desc.is_frame_self_chained = FALSE;
    #endif
            break;

        case SER_DIF_COLOR_32BITS:
            panel.mode = PANEL(COLOR_BGR);
            panel.order = PANEL(LITTLE_ENDIAN);
            panel.bpp = PANEL(24_BPP_UNPACKED);
    #if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
            dma_width = DMA_WORD_WIDTH;
            tsize = DIF_LCD_HEIGHT * DIF_LCD_WIDTH * 4;
    #endif
    #if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
            frame_desc.frame_start_addr = (t_physical_address) frame_buffer_addr;
            frame_desc.elem_desc.elem_incr = 4;
            frame_desc.elem_desc.num_elem = DIF_LCD_WIDTH;
            frame_desc.elem_desc.elem_size = 4;
            frame_desc.line_desc.line_incr = frame_desc.elem_desc.num_elem * frame_desc.elem_desc.elem_size;
            frame_desc.line_desc.num_line = DIF_LCD_HEIGHT;
            frame_desc.line_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.frame_start_event = DIF_SYNCHRO_NO_EVENT;
            frame_desc.type = DIF_TRANSACTION_DATA;
            frame_desc.is_eof_intr_enabled = TRUE;
            frame_desc.is_frame_self_chained = FALSE;
    #endif
            break;

        default:
            PRINT("Bad color depth\n");
            return(SER_DIF_ERROR);
    }

    /*dif configure*/
    dif_error = PANEL(InitializePanel());
    if (DIF_OK != dif_error)
    {
        PRINT("Unable to initialize panel\n");
        return(SER_DIF_ERROR);
    }

    dif_error = PANEL(TurnPanelON());
    if (DIF_OK != dif_error)
    {
        PRINT("Unable to turn ON panel\n");
        return(SER_DIF_ERROR);
    }

    dif_error = PANEL(PanelSettings(panel));
    if (DIF_OK != dif_error)
    {
        PRINT("Unable to configure panel\n");
        return(SER_DIF_ERROR);
    }

#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
    /*dma configure*/
    dma_error = DMA_ConfigureDmaDevice
        (
            idif_internal.dma_id,
            DMA_DIF_TX_DEVICE,
            0,
            DMA_SINGLE_BUFFERED,
            dma_width,
            idif_internal.tsize,
            DMA_BURST_SIZE_4
        );
    if (DMA_OK != dma_error)
    {
        PRINT("Unable to configure dif device\n");
        return(SER_DIF_ERROR);
    }
#endif
#if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
    dif_error = DIF_ConstructFrameDesc(frame_desc);
    if (DIF_OK != dif_error)
    {
        printf("Unable to construct frame descriptor\n");
        return(SER_DIF_ERROR);
    }
#endif

    /*timer configure*/
    tmr_error = TMR_Configure(idif_internal.tmr_id, TMR_MODE_PERIODIC, 32768/refresh_rate, TMR_CLK_PRESCALER_1);

    /*    tmr_error=TMR_InitTimerInTimeUnit(idif_internal.tmr_id,TMR_MODE_PERIODIC,1000000/refresh_rate,32768,1);*/
    if (TMR_OK != tmr_error)
    {
        PRINT("Timer error\n");
        return((t_ser_dif_error) tmr_error);
    }

    /*struct update*/
    idif_internal.status = SER_DIF_STATE_CONFIGURED;
    idif_internal.color_depth = color_depth;
    idif_internal.frame_rate = refresh_rate;
#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
    idif_internal.frame_buffer_addr = frame_buffer_addr;
    idif_internal.tsize = tsize;
#endif
#if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
    idif_internal.frame_desc = frame_desc;
#endif
    return(SER_DIF_OK);
}

/****************************************************************************/
/* NAME:    SER_DIF_Panel_Run		                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the periodic refresh of the previously	*/
/*		 configured frameBuffer												*/
/*																			*/
/* PARAMETERS:                                              		        */
/*       IN  : None															*/
/*     InOut : None                                                         */
/* 		OUT  : None                                                	        */
/*                                                                          */
/* RETURN	 :t_ser_dif_error                                 			    */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC t_ser_dif_error SER_DIF_Panel_Run(void)
{
    t_dif_error                 dif_error;
    t_tmr_error                 tmr_error;
#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
    t_dif_dma                   dif_dma;
    t_dma_exchange_status       status;
    t_dma_error                 dma_error;
    t_dma_ctrl_id               dma_ctrl_id;
#endif
#if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
    t_dif_frame_transfer_status status;
#endif
    VARDECL(transfer_mode) mode;

    mode = PANEL(WRITE);
    dif_error = PANEL(InitiatePanelTransfer(mode));
    if (DIF_OK != dif_error)
    {
        return(SER_DIF_ERROR);
    }

#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
    /*Turn on dma mode of dif*/
    dif_dma.tx_dma = DIF_ENABLE;
    dif_dma.rx_dma = DIF_DISABLE;
    dif_error = DIF_HandleDMA(dif_dma);
    if (DIF_OK != dif_error)
    {
        PRINT("Unable to enable dma for dif\n");
        return(SER_DIF_ERROR);
    }

    dma_error = DMA_CreateMemoryBuffer(&idif_internal.dma_buff);
    if (DMA_OK != dma_error)
    {
        PRINT("Unable to create Memory Buffer\n");
        return(SER_DIF_ERROR);
    }

    dma_error = DMA_UpdateMemoryBuffer
        (
            idif_internal.dma_buff,
            ARM_TO_AHB_ADDR((t_uint32) idif_internal.frame_buffer_addr),
            idif_internal.tsize
        );
    if (DMA_OK != dma_error)
    {
        PRINT("Unable to update Memory buffer\n");
        return(SER_DIF_ERROR);
    }

    dma_error = DMA_Transmit
        (
            idif_internal.dma_id,
            idif_internal.dma_buff,
            idif_internal.tsize,
            FALSE,
            TRUE,
            &dma_ctrl_id,
            &idif_internal.exchangeid
        );

    if (DMA_OK != dma_error)
    {
        PRINT("Unable to start dma transfert\n");
        return(SER_DIF_ERROR);
    }

    do
    {
       dma_error= DMA_GetExchangeStatus(idif_internal.dma_id, idif_internal.exchangeid, &status);
    } while (DMA_EXCHANGE_ON_GOING == status);
#endif
#if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
    DIF_EnableIRQSrc(((t_uint32) DIF_IRQ_SRC_END_OF_FRAME_TRANSFER) | ((t_uint32) DIF_IRQ_SRC_MASTER_BUS_ERROR));

    dif_error = DIF_StartFrameTransfer();
    if (DIF_OK != dif_error)
    {
        PRINT("Unable to start frame transfer\n");
        return(SER_DIF_ERROR);
    }

    do
    {
       dif_error =  DIF_GetFrameTransferStatus(&status);
    } while (DIF_FRAME_TRANSFER_ONGOING == status);
#endif

    /*Start timer*/
    tmr_error = TMR_StartTimer(idif_internal.tmr_id);

    if (TMR_OK != tmr_error)
    {
        PRINT("tmr error\n");
        return(SER_DIF_ERROR);
    }

    /*Update struct*/
    idif_internal.status = SER_DIF_STATE_RUNNING;
    return(SER_DIF_OK);
}

/****************************************************************************/
/* NAME:    SER_DIF_Panel_Stop	                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stops the display refresh service.	          	*/
/*                                                                          */
/* PARAMETERS:                                                              */
/*       IN  : None                                                         */
/*     InOut : None                                                         */
/* 		OUT  : None                                                	        */
/*                                                                          */
/* RETURN	 :None                                      			  	    */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :	    												*/

/****************************************************************************/
PUBLIC void SER_DIF_Panel_Stop(void)
{
#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
    t_dma_error dma_error;

     dma_error = DMA_FreeMemoryBuffer(idif_internal.dma_buff);
    if (DMA_OK != dma_error)
    {
        PRINT("Unable to close DMA Pipe \n");
    }

    dma_error = DMA_ClosePipe(idif_internal.dma_id);
    if (DMA_OK != dma_error)
    {
        PRINT("Unable to close DMA Pipe \n");
    }


    /*Added to remove the Warning */
    dma_error = dma_error;
#endif /* defined(__PLATFORM_MEK3) || defined(__PLATFORM_MEVKLITE) */

#if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
    t_dif_error dif_error;
    dif_error = DIF_StopFrameTransfer(DIF_STOP_AFTER_CURRENT_FRAME_TRANSFER);
    if (DIF_OK != dif_error)
    {
        PRINT("Unable to stop frame transfer\n");
    }

    /*Added to remove the Warning  */
    dif_error = dif_error;
#endif /* defined(__PLATFORM_MEK4) || defined(__PLATFORM_MEVKFULL) */
}

