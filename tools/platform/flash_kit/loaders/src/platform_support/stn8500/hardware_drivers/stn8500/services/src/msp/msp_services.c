/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the MSP
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "memory_mapping.h"
#include "services.h"
#include "gic.h"
/* Removing DMA & GPIO calls from PEPS */
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_HREFV2)||(defined ST_8500V2))
#include "gpio.h"
#endif


#include "msp.h"
#include "msp_irq.h"
#include "msp_services.h"

/* GLOBAL CONFIGURATION PARAMETER . TO SAVE CONFIGURATION PARAMETERS */
#define SER_MSP_DEFAULT_WORK_MODE   MSP_MODE_POLLING
#define SER_MSP_DEFAULT_DIRECTION   MSP_DIRECTION_TRANSMIT_AND_RECEIVE
#define SER_MSP_DEFAULT_FREQUENCY   HCL_FREQ_8KHZ
#define SER_MSP_DEFAULT_PROTOCOL    SER_MSP_I2S_PROTOCOL

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
#define MSP_PRINT_TRACE PRINT


#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
#define SER_MSP_INSTANCES	4
#else
#define SER_MSP_INSTANCES   3
#endif


#define GIC_MSP_0_LINE      GIC_MSP_0_LINE
#define GIC_MSP_1_LINE      GIC_MSP_1_LINE
#define GIC_MSP_2_LINE      GIC_MSP_2_LINE
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
#define GIC_MSP_3_LINE		GIC_MSP_1_LINE
#define BASE_ADDR_MSP3		MSP_3_REG_BASE_ADDR
#endif

#define BASE_ADDR_MSP0  MSP_0_REG_BASE_ADDR
#define BASE_ADDR_MSP1  MSP_1_REG_BASE_ADDR
#define BASE_ADDR_MSP2  MSP_2_REG_BASE_ADDR



typedef struct
{
	t_msp_direction             msp_direction;
    t_uint32                    msp_sample_freq;
    t_ser_msp_protocol          ser_msp_protocol;
    t_msp_mode                  tx_msp_mode;
    t_msp_mode                  rx_msp_mode;
    t_msp_protocol_descriptor   msp_protocol_descriptor;
    t_msp_protocol_descriptor   *p_msp_protocol_desc;
    t_bool                      tx_in_progress;
    t_bool                      rx_in_progress;
    t_bool                      is_msp_configured;
    t_bool                      is_msp_enabled;
    t_callback                  callback_msp;
} t_ser_msp_context;

#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
PRIVATE t_ser_msp_context			g_ser_msp_context[4];
#else
PRIVATE t_ser_msp_context           g_ser_msp_context[3];	/* equal to max possible SER_MSP_INSTANCES */
#endif

PRIVATE t_msp_protocol_descriptor   MSP_I2S_PROTOCOL =
{
    MSP_DATA_TRANSFER_WIDTH_WORD,       /*rx_data_transfer_width*/
    MSP_DATA_TRANSFER_WIDTH_WORD,       /*tx_data_transfer_width*/
    MSP_PHASE_MODE_SINGLE,              /*rx_phase_mode			*/
    MSP_PHASE_MODE_SINGLE,              /*tx_phase_mode			*/
    MSP_PHASE2_START_MODE_IMEDIATE,     /*rx_phase2_start_mode	*/
    MSP_PHASE2_START_MODE_IMEDIATE,     /*tx_phase2_start_mode	*/
    MSP_BTF_MS_BIT_FIRST,               /*rx_endianess			*/
    MSP_BTF_MS_BIT_FIRST,               /*tx_endianess			*/
    MSP_FRAME_LENGTH_1,                 /*rx_frame_length_1		*/
    MSP_FRAME_LENGTH_1,                 /*rx_frame_length_2		*/
    MSP_FRAME_LENGTH_1,                 /*tx_frame_length_1		*/
    MSP_FRAME_LENGTH_1,                 /*tx_frame_length_2		*/
    MSP_ELEMENT_LENGTH_32,              /*rx_element_length_1	*/
    MSP_ELEMENT_LENGTH_32,              /*rx_element_length_2	*/
    MSP_ELEMENT_LENGTH_32,              /*tx_element_length_1	*/
    MSP_ELEMENT_LENGTH_32,              /*tx_element_length_2	*/
    MSP_DATA_DELAY_1,                   /*rx_data_delay			*/
    MSP_DATA_DELAY_1,                   /*tx_data_delay			*/
    MSP_CLOCK_EDGE_RISING,              /*rx_clock_pol			*/
    MSP_CLOCK_EDGE_FALLING,             /*tx_clock_pol			*/
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH,     /*rx_msp_frame_pol		*/
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH,     /*tx_msp_frame_pol		*/
    MSP_HWS_NO_SWAP,                    /*rx_half_word_swap		*/
    MSP_HWS_NO_SWAP,                    /*tx_half_word_swap		*/
    MSP_COMPRESS_MODE_LINEAR,           /*compression_mode		*/
    MSP_EXPAND_MODE_LINEAR,             /*expansion_mode		*/
    MSP_SPI_CLOCK_MODE_NON_SPI,         /*spi_clk_mode			*/
    MSP_SPI_BURST_MODE_DISABLE,         /*spi_burst_mode		*/
    31,                                 /*frame_period			*/
    15,                                 /*frame_width			*/
    32,                                 /*total_clocks_for_one_frame		*/
};

PRIVATE t_msp_protocol_descriptor   MSP_PCM_PROTOCOL =
{
    MSP_DATA_TRANSFER_WIDTH_HALFWORD,   /*rx_data_transfer_width*/
    MSP_DATA_TRANSFER_WIDTH_HALFWORD,   /*tx_data_transfer_width*/
    MSP_PHASE_MODE_SINGLE,              /*rx_phase_mode         */
    MSP_PHASE_MODE_SINGLE,              /*tx_phase_mode         */
    MSP_PHASE2_START_MODE_IMEDIATE,     /*rx_phase2_start_mode  */
    MSP_PHASE2_START_MODE_IMEDIATE,     /*tx_phase2_start_mode  */
    MSP_BTF_MS_BIT_FIRST,               /*rx_endianess          */
    MSP_BTF_MS_BIT_FIRST,               /*tx_endianess          */
    MSP_FRAME_LENGTH_1,                 /*rx_frame_length_1     */
    MSP_FRAME_LENGTH_1,                 /*rx_frame_length_2     */
    MSP_FRAME_LENGTH_1,                 /*tx_frame_length_1     */
    MSP_FRAME_LENGTH_1,                 /*tx_frame_length_2     */
    MSP_ELEMENT_LENGTH_16,              /*rx_element_length_1   */
    MSP_ELEMENT_LENGTH_16,              /*rx_element_length_2   */
    MSP_ELEMENT_LENGTH_16,              /*tx_element_length_1   */
    MSP_ELEMENT_LENGTH_16,              /*tx_element_length_2   */
    MSP_DATA_DELAY_1,                   /*rx_data_delay         */
    MSP_DATA_DELAY_1,                   /*tx_data_delay         */
    MSP_CLOCK_EDGE_FALLING,              /*rx_clock_pol          */
    MSP_CLOCK_EDGE_RISING,              /*tx_clock_pol          */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH,     /*rx_msp_frame_pol      */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH,     /*tx_msp_frame_pol      */
    MSP_HWS_NO_SWAP,                    /*rx_half_word_swap		*/
    MSP_HWS_NO_SWAP,                    /*tx_half_word_swap		*/
    MSP_COMPRESS_MODE_LINEAR,           /*compression_mode      */
    MSP_EXPAND_MODE_LINEAR,             /*expansion_mode        */
    MSP_SPI_CLOCK_MODE_NON_SPI,         /*spi_clk_mode          */
    MSP_SPI_BURST_MODE_DISABLE,         /*spi_burst_mode        */
    255,                                /*frame_period          */
    0,      /*frame_width           */
    256,    /*total_clocks_for_one_frame		*/
};

PRIVATE t_msp_protocol_descriptor   MSP_PCM_COMPAND_A_LAW_PROTOCOL =
{
    MSP_DATA_TRANSFER_WIDTH_HALFWORD,   /*rx_data_transfer_width*/
    MSP_DATA_TRANSFER_WIDTH_HALFWORD,   /*tx_data_transfer_width*/
    MSP_PHASE_MODE_SINGLE,              /*rx_phase_mode			*/
    MSP_PHASE_MODE_SINGLE,              /*tx_phase_mode			*/
    MSP_PHASE2_START_MODE_IMEDIATE,     /*rx_phase2_start_mode	*/
    MSP_PHASE2_START_MODE_IMEDIATE,     /*tx_phase2_start_mode	*/
    MSP_BTF_MS_BIT_FIRST,               /*rx_endianess			*/
    MSP_BTF_MS_BIT_FIRST,               /*tx_endianess			*/
    MSP_FRAME_LENGTH_1,                 /*rx_frame_length_1		*/
    MSP_FRAME_LENGTH_1,                 /*rx_frame_length_2		*/
    MSP_FRAME_LENGTH_1,                 /*tx_frame_length_1		*/
    MSP_FRAME_LENGTH_1,                 /*tx_frame_length_2		*/
    MSP_ELEMENT_LENGTH_8,               /*rx_element_length_1	*/
    MSP_ELEMENT_LENGTH_8,               /*rx_element_length_2	*/
    MSP_ELEMENT_LENGTH_8,               /*tx_element_length_1	*/
    MSP_ELEMENT_LENGTH_8,               /*tx_element_length_2	*/
    MSP_DATA_DELAY_1,                   /*rx_data_delay			*/
    MSP_DATA_DELAY_1,                   /*tx_data_delay			*/
    MSP_CLOCK_EDGE_RISING,              /*rx_clock_pol			*/
    MSP_CLOCK_EDGE_RISING,              /*tx_clock_pol			*/
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH,     /*rx_msp_frame_pol		*/
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH,     /*tx_msp_frame_pol		*/
    MSP_HWS_NO_SWAP,                    /*rx_half_word_swap		*/
    MSP_HWS_NO_SWAP,                    /*tx_half_word_swap		*/
    MSP_COMPRESS_MODE_A_LAW,            /*compression_mode		*/
    MSP_EXPAND_MODE_A_LAW,              /*expansion_mode		*/
    MSP_SPI_CLOCK_MODE_NON_SPI,         /*spi_clk_mode			*/
    MSP_SPI_BURST_MODE_DISABLE,         /*spi_burst_mode		*/
    255,                                /*frame_period			*/
    0,      /*frame_width			*/
    256,    /*total_clocks_for_one_frame	*/
};

PRIVATE t_msp_protocol_descriptor   MSP_PCM_COMPAND_MU_LAW_PROTOCOL =
{
    MSP_DATA_TRANSFER_WIDTH_HALFWORD,   /*rx_data_transfer_width*/
    MSP_DATA_TRANSFER_WIDTH_HALFWORD,   /*tx_data_transfer_width*/
    MSP_PHASE_MODE_SINGLE,              /*rx_phase_mode         */
    MSP_PHASE_MODE_SINGLE,              /*tx_phase_mode         */
    MSP_PHASE2_START_MODE_IMEDIATE,     /*rx_phase2_start_mode  */
    MSP_PHASE2_START_MODE_IMEDIATE,     /*tx_phase2_start_mode  */
    MSP_BTF_MS_BIT_FIRST,               /*rx_endianess          */
    MSP_BTF_MS_BIT_FIRST,               /*tx_endianess          */
    MSP_FRAME_LENGTH_1,                 /*rx_frame_length_1     */
    MSP_FRAME_LENGTH_1,                 /*rx_frame_length_2     */
    MSP_FRAME_LENGTH_1,                 /*tx_frame_length_1     */
    MSP_FRAME_LENGTH_1,                 /*tx_frame_length_2     */
    MSP_ELEMENT_LENGTH_8,               /*rx_element_length_1   */
    MSP_ELEMENT_LENGTH_8,               /*rx_element_length_2   */
    MSP_ELEMENT_LENGTH_8,               /*tx_element_length_1   */
    MSP_ELEMENT_LENGTH_8,               /*tx_element_length_2   */
    MSP_DATA_DELAY_1,                   /*rx_data_delay         */
    MSP_DATA_DELAY_1,                   /*tx_data_delay         */
    MSP_CLOCK_EDGE_RISING,              /*rx_clock_pol          */
    MSP_CLOCK_EDGE_RISING,              /*tx_clock_pol          */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH,     /*rx_msp_frame_pol      */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH,     /*tx_msp_frame_pol      */
    MSP_HWS_NO_SWAP,                    /*rx_half_word_swap     */
    MSP_HWS_NO_SWAP,                    /*tx_half_word_swap     */
    MSP_COMPRESS_MODE_MU_LAW,           /*compression_mode      */
    MSP_EXPAND_MODE_MU_LAW,             /*expansion_mode        */
    MSP_SPI_CLOCK_MODE_NON_SPI,         /*spi_clk_mode          */
    MSP_SPI_BURST_MODE_DISABLE,         /*spi_burst_mode        */
    255,                                /*frame_period          */
    0,                              /*frame_width           */
    256,                            /*total_clocks_for_one_frame    */
};

PRIVATE t_msp_protocol_descriptor   MSP_AC97_PROTOCOL =
{
    MSP_DATA_TRANSFER_WIDTH_WORD,   /*rx_data_transfer_width*/
    MSP_DATA_TRANSFER_WIDTH_WORD,   /*tx_data_transfer_width*/
    MSP_PHASE_MODE_DUAL,            /*rx_phase_mode         */
    MSP_PHASE_MODE_DUAL,            /*tx_phase_mode         */
    MSP_PHASE2_START_MODE_IMEDIATE, /*rx_phase2_start_mode  */
    MSP_PHASE2_START_MODE_IMEDIATE, /*tx_phase2_start_mode  */
    MSP_BTF_MS_BIT_FIRST,           /*rx_endianess          */
    MSP_BTF_MS_BIT_FIRST,           /*tx_endianess          */
    MSP_FRAME_LENGTH_1,             /*rx_frame_length_1     */
    MSP_FRAME_LENGTH_12,            /*rx_frame_length_2     */
    MSP_FRAME_LENGTH_1,             /*tx_frame_length_1     */
    MSP_FRAME_LENGTH_12,            /*tx_frame_length_2     */
    MSP_ELEMENT_LENGTH_16,          /*rx_element_length_1   */
    MSP_ELEMENT_LENGTH_20,          /*rx_element_length_2   */
    MSP_ELEMENT_LENGTH_16,          /*tx_element_length_1   */
    MSP_ELEMENT_LENGTH_20,          /*tx_element_length_2   */
    MSP_DATA_DELAY_1,               /*rx_data_delay         */
    MSP_DATA_DELAY_1,               /*tx_data_delay         */
    MSP_CLOCK_EDGE_RISING,          /*rx_clock_pol          */
    MSP_CLOCK_EDGE_RISING,          /*tx_clock_pol          */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH, /*rx_msp_frame_pol      */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH, /*tx_msp_frame_pol      */
    MSP_HWS_NO_SWAP,                /*rx_half_word_swap     */
    MSP_HWS_NO_SWAP,                /*tx_half_word_swap     */
    MSP_COMPRESS_MODE_LINEAR,       /*compression_mode      */
    MSP_EXPAND_MODE_LINEAR,         /*expansion_mode        */
    MSP_SPI_CLOCK_MODE_NON_SPI,     /*spi_clk_mode          */
    MSP_SPI_BURST_MODE_DISABLE,     /*spi_burst_mode        */
    255,                            /*frame_period          */
    15,                             /*frame_width           */
    256,                            /*total_clocks_for_one_frame    */
};

PRIVATE t_msp_protocol_descriptor   MSP_MASTER_SPI_PROTOCOL =
{
    MSP_DATA_TRANSFER_WIDTH_BYTE,   /*rx_data_transfer_width*/
    MSP_DATA_TRANSFER_WIDTH_BYTE,   /*tx_data_transfer_width*/
    MSP_PHASE_MODE_SINGLE,          /*rx_phase_mode         */
    MSP_PHASE_MODE_SINGLE,          /*tx_phase_mode         */
    MSP_PHASE2_START_MODE_IMEDIATE, /*rx_phase2_start_mode  */
    MSP_PHASE2_START_MODE_IMEDIATE, /*tx_phase2_start_mode  */
    MSP_BTF_MS_BIT_FIRST,           /*rx_endianess          */
    MSP_BTF_MS_BIT_FIRST,           /*tx_endianess          */
    MSP_FRAME_LENGTH_1,             /*rx_frame_length_1     */
    MSP_FRAME_LENGTH_1,             /*rx_frame_length_2     */
    MSP_FRAME_LENGTH_1,             /*tx_frame_length_1     */
    MSP_FRAME_LENGTH_1,             /*tx_frame_length_2     */
    MSP_ELEMENT_LENGTH_8,           /*rx_element_length_1   */
    MSP_ELEMENT_LENGTH_8,           /*rx_element_length_2   */
    MSP_ELEMENT_LENGTH_8,           /*tx_element_length_1   */
    MSP_ELEMENT_LENGTH_8,           /*tx_element_length_2   */
    MSP_DATA_DELAY_1,               /*rx_data_delay         */
    MSP_DATA_DELAY_1,               /*tx_data_delay         */
    MSP_CLOCK_EDGE_RISING,          /*rx_clock_pol          */
    MSP_CLOCK_EDGE_RISING,          /*tx_clock_pol          */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH, /*rx_msp_frame_pol      */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH, /*tx_msp_frame_pol      */
    MSP_HWS_NO_SWAP,                /*rx_half_word_swap     */
    MSP_HWS_NO_SWAP,                /*tx_half_word_swap     */
    MSP_COMPRESS_MODE_LINEAR,       /*compression_mode      */
    MSP_EXPAND_MODE_LINEAR,         /*expansion_mode        */
    MSP_SPI_CLOCK_MODE_ZERO_DELAY,    /*spi_clk_mode          */
    MSP_SPI_BURST_MODE_ENABLE,             /*spi_burst_mode        */
    255,                            /*frame_period          */
    0,                              /*frame_width           */
    256,                            /*total_clocks_for_one_frame    */
};

PRIVATE t_msp_protocol_descriptor   MSP_SLAVE_SPI_PROTOCOL =
{
    MSP_DATA_TRANSFER_WIDTH_BYTE,   /*rx_data_transfer_width*/
    MSP_DATA_TRANSFER_WIDTH_BYTE,   /*tx_data_transfer_width*/
    MSP_PHASE_MODE_SINGLE,          /*rx_phase_mode         */
    MSP_PHASE_MODE_SINGLE,          /*tx_phase_mode         */
    MSP_PHASE2_START_MODE_IMEDIATE, /*rx_phase2_start_mode  */
    MSP_PHASE2_START_MODE_IMEDIATE, /*tx_phase2_start_mode  */
    MSP_BTF_MS_BIT_FIRST,           /*rx_endianess          */
    MSP_BTF_MS_BIT_FIRST,           /*tx_endianess          */
    MSP_FRAME_LENGTH_1,             /*rx_frame_length_1     */
    MSP_FRAME_LENGTH_1,             /*rx_frame_length_2     */
    MSP_FRAME_LENGTH_1,             /*tx_frame_length_1     */
    MSP_FRAME_LENGTH_1,             /*tx_frame_length_2     */
    MSP_ELEMENT_LENGTH_8,           /*rx_element_length_1   */
    MSP_ELEMENT_LENGTH_8,           /*rx_element_length_2   */
    MSP_ELEMENT_LENGTH_8,           /*tx_element_length_1   */
    MSP_ELEMENT_LENGTH_8,           /*tx_element_length_2   */
    MSP_DATA_DELAY_0,               /*rx_data_delay         */
    MSP_DATA_DELAY_0,               /*tx_data_delay         */
    MSP_CLOCK_EDGE_RISING,          /*rx_clock_pol          */
    MSP_CLOCK_EDGE_RISING,          /*tx_clock_pol          */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH, /*rx_msp_frame_pol      */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH, /*tx_msp_frame_pol      */
    MSP_HWS_NO_SWAP,                /*rx_half_word_swap     */
    MSP_HWS_NO_SWAP,                /*tx_half_word_swap     */
    MSP_COMPRESS_MODE_LINEAR,       /*compression_mode      */
    MSP_EXPAND_MODE_LINEAR,         /*expansion_mode        */
    MSP_SPI_CLOCK_MODE_HALF_CYCLE_DELAY,    /*spi_clk_mode          */
    MSP_SPI_BURST_MODE_DISABLE,             /*spi_burst_mode        */
    255,                            /*frame_period          */
    0,                              /*frame_width           */
    256,                            /*total_clocks_for_one_frame    */
};

PRIVATE t_msp_protocol_descriptor   MSP_8BIT_PCM_PROTOCOL =
{
    MSP_DATA_TRANSFER_WIDTH_BYTE,   /*rx_data_transfer_width*/
    MSP_DATA_TRANSFER_WIDTH_BYTE,   /*tx_data_transfer_width*/
    MSP_PHASE_MODE_SINGLE,          /*rx_phase_mode         */
    MSP_PHASE_MODE_SINGLE,          /*tx_phase_mode         */
    MSP_PHASE2_START_MODE_IMEDIATE, /*rx_phase2_start_mode  */
    MSP_PHASE2_START_MODE_IMEDIATE, /*tx_phase2_start_mode  */
    MSP_BTF_MS_BIT_FIRST,           /*rx_endianess          */
    MSP_BTF_MS_BIT_FIRST,           /*tx_endianess          */
    MSP_FRAME_LENGTH_1,             /*rx_frame_length_1     */
    MSP_FRAME_LENGTH_1,             /*rx_frame_length_2     */
    MSP_FRAME_LENGTH_1,             /*tx_frame_length_1     */
    MSP_FRAME_LENGTH_1,             /*tx_frame_length_2     */
    MSP_ELEMENT_LENGTH_8,           /*rx_element_length_1   */
    MSP_ELEMENT_LENGTH_8,           /*rx_element_length_2   */
    MSP_ELEMENT_LENGTH_8,           /*tx_element_length_1   */
    MSP_ELEMENT_LENGTH_8,           /*tx_element_length_2   */
    MSP_DATA_DELAY_1,               /*rx_data_delay         */
    MSP_DATA_DELAY_1,               /*tx_data_delay         */
    MSP_CLOCK_EDGE_RISING,          /*rx_clock_pol          */
    MSP_CLOCK_EDGE_RISING,          /*tx_clock_pol          */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH, /*rx_msp_frame_pol      */
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH, /*tx_msp_frame_pol      */
    MSP_HWS_NO_SWAP,                /*rx_half_word_swap     */
    MSP_HWS_NO_SWAP,                /*tx_half_word_swap     */
    MSP_COMPRESS_MODE_LINEAR,       /*compression_mode      */
    MSP_EXPAND_MODE_LINEAR,         /*expansion_mode        */
    MSP_SPI_CLOCK_MODE_NON_SPI,     /*spi_clk_mode          */
    MSP_SPI_BURST_MODE_DISABLE,     /*spi_burst_mode        */
    255,                            /*frame_period          */
    0,                              /*frame_width           */
    256,                            /*total_clocks_for_one_frame    */
};




/**********************************************************************
 *
 * Interrupt handler
 *
 **********************************************************************/
/****************************************************************************/
/* NAME:    SER_MSP_InterruptHandler				                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: interrupt handler implementation							*/
/*                                                                          */
/* PARAMETERS: 																*/
/*			t_msp_device_id msp_device_id: 									*/
/*		MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2/MSP_DEVICE_ID_3		*/
/*                                                                          */
/* RETURN:	void															*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
#ifdef MSP_USE_FILTER_PROCESS_IRQ
PRIVATE void ser_msp_interrupt_handler(IN t_msp_device_id msp_device_id)
{
    t_gic_error         gic_error = GIC_OK;
    t_msp_irq_src       irq_src;
    t_msp_irq_status    irq_status;
    t_msp_event         current_event;
    t_msp_error         msp_error;
    t_ser_msp_param     ser_msp_param;
    t_bool              done = FALSE;
    t_gic_line          gic_line_for_msp;
    volatile t_callback *p_callback_msp;    /*ptr to call back stucture*/

    switch (msp_device_id)
    {
        case MSP_DEVICE_ID_0:
        default:
            irq_src = MSP0_IRQ_SRC_ALL;
            gic_line_for_msp = GIC_MSP_0_LINE;
            p_callback_msp = &g_ser_msp_context[0].callback_msp;
            break;

        case MSP_DEVICE_ID_1:
            irq_src = MSP1_IRQ_SRC_ALL;
            gic_line_for_msp = GIC_MSP_1_LINE;
            p_callback_msp = &g_ser_msp_context[1].callback_msp;
            break;
            
        case MSP_DEVICE_ID_2:
            irq_src = MSP2_IRQ_SRC_ALL;
            gic_line_for_msp = GIC_MSP_2_LINE;
            p_callback_msp = &g_ser_msp_context[2].callback_msp;
            break;
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))		
		case MSP_DEVICE_ID_3:
			irq_src = MSP3_IRQ_SRC_ALL;
			gic_line_for_msp = GIC_MSP_3_LINE;
			p_callback_msp = &g_ser_msp_context[3].callback_msp;
			break;
#endif
                       
	};

    MSP_GetIRQSrcStatus(irq_src, &irq_status);


    gic_error = GIC_DisableItLine(gic_line_for_msp);
    if (GIC_OK != gic_error)
    {
        PRINT("\nGIC error after GIC_AcknowledgeItLine  :: %d", gic_error);
        exit(-1);
    }
/*
    gic_error = GIC_AcknowledgeItLine(gic_line_for_msp);
    if (GIC_OK != gic_error)
    {
        PRINT("\nGIC error after GIC_AcknowledgeItLine  :: %d", gic_error);
        return;
    }
*/
    do
    {
        msp_error = MSP_FilterProcessIRQSrc(&irq_status, &current_event, MSP_NO_FILTER_MODE);
        switch (msp_error)
        {
            case MSP_NO_PENDING_EVENT_ERROR:
            case MSP_NO_MORE_PENDING_EVENT:
            case MSP_NO_MORE_FILTER_PENDING_EVENT:
                done = TRUE;
                break;

            case MSP_INTERNAL_EVENT:
                break;

            case MSP_REMAINING_PENDING_EVENTS:
                MSP_AcknowledgeEvent(&current_event);
                if (p_callback_msp->fct != NULL)
                {
                    ser_msp_param.irq_status = irq_status;
                    ser_msp_param.current_event = current_event;
                    ser_msp_param.msp_error = msp_error;
                    p_callback_msp->fct(p_callback_msp->param, &ser_msp_param);
                }
                break;

            case MSP_TRANSMISSION_ON_GOING:
                if (p_callback_msp->fct != NULL)
                {
                    ser_msp_param.irq_status = irq_status;
                    ser_msp_param.current_event = current_event;
                    ser_msp_param.msp_error = msp_error;
                    p_callback_msp->fct(p_callback_msp->param, &ser_msp_param);
                }
                break;
        }
    } while (done != TRUE);


    gic_error = GIC_EnableItLine(gic_line_for_msp);
    if (GIC_OK != gic_error)
    {
        PRINT("\nGIC error after GIC_AcknowledgeItLine  :: %d", gic_error);
        return;
    }

}
#endif /*#ifdef MSP_USE_FILTER_PROCESS_IRQ*/

#ifndef MSP_USE_FILTER_PROCESS_IRQ
PRIVATE void ser_msp_interrupt_handler(IN t_msp_device_id msp_device_id)
{
    t_msp_error         msp_error;
    t_msp_irq_src       msp_irq_src;
    t_msp_irq_status    msp_irq_status;
    t_gic_error         gic_error = GIC_OK;
    t_ser_msp_param     ser_msp_param;
    t_gic_line          gic_line_for_msp = GIC_MSP_0_LINE;
    volatile t_callback *p_callback_msp;    /*ptr to call back stucture*/

    switch (msp_device_id)
    {
        case MSP_DEVICE_ID_0:
        default:
            msp_irq_src = (t_uint32)MSP0_IRQ_SRC_ALL;
            gic_line_for_msp = GIC_MSP_0_LINE;
            p_callback_msp = &g_ser_msp_context[0].callback_msp;
            break;

        case MSP_DEVICE_ID_1:
            msp_irq_src = (t_uint32)MSP1_IRQ_SRC_ALL;
            gic_line_for_msp = GIC_MSP_1_LINE;
            p_callback_msp = &g_ser_msp_context[1].callback_msp;
            break;

        case MSP_DEVICE_ID_2:
            msp_irq_src = (t_uint32)MSP2_IRQ_SRC_ALL;
            gic_line_for_msp = GIC_MSP_2_LINE;
            p_callback_msp = &g_ser_msp_context[2].callback_msp;
            break;
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
		case MSP_DEVICE_ID_3:
			msp_irq_src = (t_uint32)MSP3_IRQ_SRC_ALL;
			gic_line_for_msp = GIC_MSP_3_LINE;
			p_callback_msp = &g_ser_msp_context[3].callback_msp;
			break;
#endif
        		
    };

    MSP_GetIRQSrcStatus(msp_irq_src, &msp_irq_status);


    gic_error = GIC_DisableItLine(gic_line_for_msp);
    if (GIC_OK != gic_error)
    {
        PRINT("\nGIC error after GIC_AcknowledgeItLine  :: %d", gic_error);
        return;
    }
/*
    gic_error = GIC_AcknowledgeItLine(gic_line_for_msp);
    if (GIC_OK != gic_error)
    {
        PRINT("\nGIC error after GIC_AcknowledgeItLine  :: %d", gic_error);
        return;
    }
*/

    
    /* Service The IRQ */
    msp_error = MSP_ProcessIRQSrc(&msp_irq_status);
    if (p_callback_msp->fct != NULL)
    {
        ser_msp_param.irq_status = msp_irq_status;
        ser_msp_param.msp_error = msp_error;
        p_callback_msp->fct(p_callback_msp->param, &ser_msp_param);
    }


    gic_error = GIC_EnableItLine(gic_line_for_msp);
    if (GIC_OK != gic_error)
    {
        PRINT("\nGIC error after GIC_AcknowledgeItLine  :: %d", gic_error);
        return;
    }

}
#endif /*#ifndef MSP_USE_FILTER_PROCESS_IRQ*/

PUBLIC void SER_MSP0_InterruptHandler(IN t_uint32 not_used)
{
    not_used = not_used + 0xFF; /*Added to remove the coverity Error*/	
    ser_msp_interrupt_handler(MSP_DEVICE_ID_0);
}

PUBLIC void SER_MSP1_InterruptHandler(IN t_uint32 not_used)
{
    not_used = not_used + 0xFF; /*Added to remove the coverity Error*/
    ser_msp_interrupt_handler(MSP_DEVICE_ID_1);
}

PUBLIC void SER_MSP2_InterruptHandler(IN t_uint32 not_used)
{
    not_used = not_used + 0xFF; /*Added to remove the coverity Error*/
    ser_msp_interrupt_handler(MSP_DEVICE_ID_2);
}

#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
PUBLIC void SER_MSP3_InterruptHandler(IN t_uint32 not_used)
{
	not_used = not_used + 0xFF; /*Added to remove the coverity Error*/
	ser_msp_interrupt_handler(MSP_DEVICE_ID_3);
}
#endif

/*--------------------------------------------------------------------------*
 * Public functions														    *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_MSP_ConfigureCustom				                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This allows to configure msp								*/
/*                                                                          */
/* PARAMETERS: 																*/
/*	msp_device_id: MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2			*/
/*	p_msp_protocol_desc_user_defined: user-defined protcol : if not NULL, 	*/
/*	 protocol descriptor pointed by the param is used to configure the MSP.	*/
/*	 for users this parameter must always be non-NULL						*/
/*			is only aplicable if SER_MSP_USER_DEFINED_PROTOCOL is used		*/
/*			NULL can be passed if using standard protocols like I2S,PCM etc.*/
/*	msp_mode			: DMA/Polling/Interrupt								*/
/*	msp_sample_freq			: msp sample freq								*/
/*	msp_general_config		: general configuartion							*/
/*	ser_msp_compand_mode	: A/Mu/Linear mode								*/
/*	msp_coprocessor_mode	: Loop back/direct/normal						*/
/*																			*/
/* RETURN:	TRUE if successful												*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_bool SER_MSP_ConfigureCustom
(
    IN t_msp_device_id              msp_device_id,
    IN t_msp_protocol_descriptor    *p_msp_protocol_desc_user_defined,
    IN t_uint32                     msp_sample_freq,
    IN t_uint32                     msp_in_clock_freq,
    IN t_msp_general_config         *p_msp_general_config
)
{
    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(t_bool) FALSE;
    }

    g_ser_msp_context[msp_device_id].msp_sample_freq = msp_sample_freq;

    /*g_ser_msp_context[msp_device_id].ser_msp_protocol=ser_msp_protocol;*/
    if (p_msp_protocol_desc_user_defined != NULL)
    {
        g_ser_msp_context[msp_device_id].ser_msp_protocol = SER_MSP_USER_DEFINED_PROTOCOL;
        g_ser_msp_context[msp_device_id].msp_protocol_descriptor = *p_msp_protocol_desc_user_defined;
    }

    switch (g_ser_msp_context[msp_device_id].ser_msp_protocol)
    {
        case SER_MSP_I2S_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &MSP_I2S_PROTOCOL;
            break;

        case SER_MSP_AC97_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &MSP_AC97_PROTOCOL;
            break;

        case SER_MSP_16BIT_PCM_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &MSP_PCM_PROTOCOL;
            break;

        case SER_MSP_PCM_COMPAND_A_LAW_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &MSP_PCM_COMPAND_A_LAW_PROTOCOL;
            break;

        case SER_MSP_PCM_COMPAND_MU_LAW_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &MSP_PCM_COMPAND_MU_LAW_PROTOCOL;
            break;

        case SER_MSP_8BIT_PCM_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &MSP_8BIT_PCM_PROTOCOL;
            break;

        case SER_MSP_MASTER_SPI_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &MSP_MASTER_SPI_PROTOCOL;
            break;

        case SER_MSP_SLAVE_SPI_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &MSP_SLAVE_SPI_PROTOCOL;
            break;

        case SER_MSP_SPI_BURST_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &MSP_MASTER_SPI_PROTOCOL;
            break;

        case SER_MSP_USER_DEFINED_PROTOCOL:
            g_ser_msp_context[msp_device_id].p_msp_protocol_desc = &g_ser_msp_context[msp_device_id].msp_protocol_descriptor;
            break;
    };

    if (MSP_OK != MSP_Disable(msp_device_id, MSP_DIRECTION_TRANSMIT_AND_RECEIVE))
    {
        PRINT("<E> SER_MSP: SER_MSP_ConfigureCustom: from MSP_Disable\n");
        return(t_bool) FALSE;
    }

    MSP_Delay(msp_device_id, 8);

    if (MSP_OK != MSP_Reset(msp_device_id)) /* kind of device reset*/
    {
        PRINT("<E> SER_MSP: SER_MSP_ConfigureCustom: from MSP_Reset\n");
        return(t_bool) FALSE;
    }

    MSP_Delay(msp_device_id, 8);

    /* Configure MSP*/
    if (MSP_OK != MSP_Configure(msp_device_id, p_msp_general_config, g_ser_msp_context[msp_device_id].p_msp_protocol_desc))
    {
        PRINT("<E> SER_MSP: SER_MSP_ConfigureCustom: from MSP_Configure\n");
        return(t_bool) FALSE;
    }

    MSP_Delay(msp_device_id, 8);

    if (MSP_OK != MSP_SetSampleFrequency(msp_device_id, msp_in_clock_freq, msp_sample_freq, NULL))
    {
        PRINT("<E> SER_MSP: SER_MSP_ConfigureCustom: from MSP_SetSampleFrequency\n");
        return(t_bool) FALSE;
    }

    g_ser_msp_context[msp_device_id].rx_msp_mode = p_msp_general_config->rx_msp_mode;
    g_ser_msp_context[msp_device_id].tx_msp_mode = p_msp_general_config->tx_msp_mode;

    g_ser_msp_context[msp_device_id].is_msp_configured = (t_bool) TRUE;

    return(t_bool) TRUE;
}

/****************************************************************************/
/* NAME:    SER_MSP_Configure    				                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: this routine allows to configue msp for normal operation 	*/
/*	(play / record through audio codec)										*/
/*                                                                          */
/* PARAMETERS: 																*/
/*	msp_device_id		: MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2	*/
/*	ser_msp_protocol		: from t_ser_msp_protocol 						*/
/*								(pre-defined standard protocol)				*/
/*	msp_mode			: DMA/Polling/Interrupt								*/
/*	msp_sample_freq			: msp sample freq								*/
/*	msp_in_clock_freq		: msp input freq								*/
/*																			*/
/* RETURN:	TRUE if successful												*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_bool SER_MSP_Configure(IN t_msp_device_id msp_device_id, IN t_ser_msp_protocol ser_msp_protocol, IN t_msp_mode msp_mode, IN t_uint32 msp_sample_freq, IN t_uint32 msp_in_clock_freq)
{
    t_msp_general_config    msp_configuration;

    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(t_bool) FALSE;
    }

    if (SER_MSP_USER_DEFINED_PROTOCOL == ser_msp_protocol)
    {
        PRINT("<E> SER_MSP:SER_MSP_Configure:SER_MSP_USER_DEFINED_PROTOCOL can not be used in SER_MSP_Configure, use SER_MSP_ConfigureCustom.\n");
        return(t_bool) FALSE;
    }

    g_ser_msp_context[msp_device_id].ser_msp_protocol = ser_msp_protocol;

    /* Fill MSP Configuration register */

    msp_configuration.srg_clock_sel = MSP_SRG_IN_CLOCK_APB;

    msp_configuration.rx_clock_sel = MSP_CLOCK_SEL_SRG;             /* TP */
    msp_configuration.rx_frame_sync_sel = MSP_RX_FRAME_SYNC_SRG;    /* TP */
    msp_configuration.sck_pol = MSP_SCK_POL_AT_RISING_EDGE;
    msp_configuration.msp_loopback_mode = MSP_LOOPBACK_MODE_DISABLE;
    msp_configuration.msp_direct_companding_mode = MSP_DIRECT_COMPANDING_MODE_DISABLE;
    msp_configuration.tx_clock_sel = MSP_CLOCK_SEL_SRG;
    msp_configuration.rx_msp_mode = msp_mode;
    msp_configuration.tx_msp_mode = msp_mode;
    msp_configuration.rx_frame_sync_sel = MSP_RX_FRAME_SYNC_SRG;
    msp_configuration.tx_frame_sync_sel = MSP_TX_FRAME_SYNC_SRG_PROG;
    msp_configuration.rx_unexpect_frame_sync = MSP_UNEXPEC_FRAME_SYNC_IGNORED;
    msp_configuration.tx_unexpect_frame_sync = MSP_UNEXPEC_FRAME_SYNC_IGNORED;
    msp_configuration.rx_fifo_config = MSP_FIFO_ENABLE;
    msp_configuration.tx_fifo_config = MSP_FIFO_ENABLE;

    return(SER_MSP_ConfigureCustom(msp_device_id, NULL, msp_sample_freq, msp_in_clock_freq, &msp_configuration));
}

/****************************************************************************/
/* NAME:    SER_MSP_Transmit					                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: this routine allows to transmit data.						*/
/*              Blocking for Polling mode				                    */
/* PARAMETERS: 																*/
/*	msp_device_id			: MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2								*/
/*	p_transmit_buffer	: pointer to buffer 								*/
/*	buffer_size		: size of buffer										*/
/*																			*/
/* RETURN:	TRUE if successful												*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_bool SER_MSP_Transmit(IN t_msp_device_id msp_device_id, IN t_uint8 *p_transmit_buffer, IN t_uint32 buffer_size)
{	

    t_msp_error msp_error;

    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(t_bool) FALSE;
    }

    if ((t_bool)FALSE == g_ser_msp_context[msp_device_id].is_msp_configured)
    {
        PRINT("<E> SER_MSP: SER_MSP_Transmit : MSP not configured\n");
    }

    if ((t_bool)FALSE == g_ser_msp_context[msp_device_id].is_msp_enabled)
    {
        PRINT("<E> SER_MSP: SER_MSP_Transmit : MSP not enabled \n");
    }

    if (MSP_DIRECTION_RECEIVE == g_ser_msp_context[msp_device_id].msp_direction)    /* if MSP_DIRECTION_RECEIVE_MODE only*/
    {
        PRINT("<E> SER_MSP: SER_MSP_Transmit : Cannot Transmit in MSP_DIRECTION_RECEIVE_MODE\n");
    }

    /*
	switch(g_ser_msp_context[msp_device_id].tx_msp_mode)
	{
		case MSP_MODE_POLLING	:PRINT("<I> SER_MSP:TX:MSP_MODE_POLLING");break;
		case MSP_MODE_IT		:PRINT("<I> SER_MSP:TX:MSP_MODE_IT     ");break;	
		case MSP_MODE_DMA     	:PRINT("<I> SER_MSP:TX:MSP_MODE_DMA    ");break;
	}
*/
	
    
	
        msp_error = MSP_TransmitData(msp_device_id, (t_logical_address) p_transmit_buffer, buffer_size);
        if (msp_error != MSP_OK)
        {
            PRINT("<E> MSP_SER:SER_MSP_Transmit:from MSP_TransmitData\n");
            return(t_bool) FALSE;
        }

        g_ser_msp_context[msp_device_id].tx_in_progress = (t_bool) TRUE;
        if (MSP_MODE_POLLING == g_ser_msp_context[msp_device_id].tx_msp_mode)
        {
            g_ser_msp_context[msp_device_id].tx_in_progress = (t_bool) FALSE;
        }
    

    return(t_bool) TRUE;
}

/****************************************************************************/
/* NAME:    SER_MSP_Receive						                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: this routine allows to receive data.						*/
/*              Blocking for Polling mode				                    */
/* PARAMETERS: 																*/
/*	msp_device_id		: MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2	*/
/*	p_receive_buffer	: pointer to buffer 								*/
/*	buffer_size		: size of buffer										*/
/*																			*/
/* RETURN:	TRUE if successful												*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_bool SER_MSP_Receive(IN t_msp_device_id msp_device_id, IN t_uint8 *p_receive_buffer, IN t_uint32 buffer_size)
{	
	
    t_msp_error msp_error;

    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(t_bool) FALSE;
    }

    if ((t_bool)FALSE == g_ser_msp_context[msp_device_id].is_msp_configured)
    {
        PRINT("<E> SER_MSP: SER_MSP_Receive : MSP not configured\n");
    }

    if ((t_bool)FALSE == g_ser_msp_context[msp_device_id].is_msp_enabled)
    {
        PRINT("<E> SER_MSP: SER_MSP_Receive : MSP not enabled \n");
    }

    if (MSP_DIRECTION_TRANSMIT == g_ser_msp_context[msp_device_id].msp_direction)   /* if transmit only*/
    {
        PRINT("<E> SER_MSP: SER_MSP_Receive : Cannot Receive in MSP_DIRECTION_TRANSMIT_MODE\n");
    }

    /*
	switch(g_ser_msp_context[msp_device_id].rx_msp_mode)
	{
		case MSP_MODE_POLLING	:PRINT("<I> SER_MSP:RX:MSP_MODE_POLLING");break;
		case MSP_MODE_IT		:PRINT("<I> SER_MSP:RX:MSP_MODE_IT     ");break;	
		case MSP_MODE_DMA     	:PRINT("<I> SER_MSP:RX:MSP_MODE_DMA    ");break;
	}
*/
	
	
    
    
        msp_error = MSP_ReceiveData(msp_device_id, (t_logical_address) p_receive_buffer, buffer_size);
        if (msp_error != MSP_OK)
        {
            PRINT("<E> MSP_SER:SER_MSP_Receive:from MSP_ReceiveData\n");
            return(t_bool) FALSE;
        }

        g_ser_msp_context[msp_device_id].rx_in_progress = (t_bool) TRUE;
    

    return(t_bool) TRUE;
}

/****************************************************************************/
/* NAME:    SER_MSP_TransmitWaitEnd				                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: this routine allows to wait for transmition to complete if.	*/
/*              Ineffective in Polling mode.			                    */
/* PARAMETERS: 																*/
/*	msp_device_id	: MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2		*/
/*																			*/
/* RETURN:	TRUE if successful												*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_bool SER_MSP_TransmitWaitEnd(IN t_msp_device_id msp_device_id)
{

	   
    
    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(t_bool) FALSE;
    }

    if ((t_bool)FALSE == g_ser_msp_context[msp_device_id].is_msp_configured)
    {
        PRINT("<E> SER_MSP: SER_MSP_TransmitWaitEnd : MSP not configured\n");
    }

    if ((t_bool)FALSE == g_ser_msp_context[msp_device_id].is_msp_enabled)
    {
        PRINT("<E> SER_MSP: SER_MSP_TransmitWaitEnd : MSP not enabled \n");
    }

    if (MSP_DIRECTION_RECEIVE == g_ser_msp_context[msp_device_id].msp_direction)    /* if MSP_DIRECTION_RECEIVE_MODE only*/
    {
        PRINT("<E> MSP_SER:SER_MSP_TransmitWaitEnd:cannot wait for Tx-over in MSP_DIRECTION_RECEIVE_MODE\n");
    }

    if (MSP_MODE_POLLING == g_ser_msp_context[msp_device_id].tx_msp_mode)
    {
        return(t_bool) TRUE;
    }

    if (g_ser_msp_context[msp_device_id].tx_in_progress != (t_bool)TRUE)
    {
        PRINT("<E> MSP_SER:SER_MSP_TransmitWaitEnd:cannot wait for Tx-over. no transmition in progress.\n");
    }
    
    
        if (MSP_MODE_IT == g_ser_msp_context[msp_device_id].tx_msp_mode)
        {
            t_bool  is_transmition_completed = (t_bool) FALSE;
            do
            {
                if (MSP_OK != MSP_GetTransmissionStatus(msp_device_id, NULL, NULL, &is_transmition_completed))
                {
                    PRINT("<E> SER_MSP: SER_MSP_TransmitWaitEnd: MSP_GetTransmissionStatus");
                }
            } while ((t_bool)FALSE == is_transmition_completed);
        }
    

    return(t_bool) TRUE;
}

/****************************************************************************/
/* NAME:    SER_MSP_ReceiveWaitEnd				                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: this routine allows to wait for reception to complete if.	*/
/*              Ineffective in Polling mode.				                */
/* PARAMETERS: 																*/
/*	msp_device_id	: MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2		*/
/*																			*/
/* RETURN:	TRUE if successful												*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_bool SER_MSP_ReceiveWaitEnd(IN t_msp_device_id msp_device_id)
{
	    
    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(t_bool) FALSE;
    }

    if ((t_bool)FALSE == g_ser_msp_context[msp_device_id].is_msp_configured)
    {
        PRINT("<E> SER_MSP: SER_MSP_ReceiveWaitEnd : MSP not configured\n");
    }

    if ((t_bool)FALSE == g_ser_msp_context[msp_device_id].is_msp_enabled)
    {
        PRINT("<E> SER_MSP: SER_MSP_ReceiveWaitEnd : MSP not enabled \n");
    }

    if (MSP_DIRECTION_TRANSMIT == g_ser_msp_context[msp_device_id].msp_direction)   /* if transmit only*/
    {
        PRINT("<E> MSP_SER:SER_MSP_ReceiveWaitEnd:cannot wait for Rx-over in MSP_DIRECTION_TRANSMIT_MODE\n");
    }

    if (MSP_MODE_POLLING == g_ser_msp_context[msp_device_id].rx_msp_mode)
    {
        return(t_bool) TRUE;
    }

    if (g_ser_msp_context[msp_device_id].rx_in_progress != (t_bool)TRUE)
    {
        PRINT("<E> MSP_SER:SER_MSP_ReceiveWaitEnd:cannot wait for Rx-over. no reception in progress.\n");
    }
    
    
    
        if (MSP_MODE_IT == g_ser_msp_context[msp_device_id].rx_msp_mode)
        {
            t_bool  is_reception_completed = (t_bool) FALSE;
            do
            {
                if (MSP_OK != MSP_GetReceptionStatus(msp_device_id, NULL, NULL, &is_reception_completed))
                {
                    PRINT("<E> SER_MSP: SER_MSP_TransmitWaitEnd: MSP_GetReceptionStatus");
                }
            } while ((t_bool)FALSE == is_reception_completed);
        }
    

    return(t_bool) TRUE;
}

/****************************************************************************/
/* NAME:    SER_MSP_Start						                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine must be called after Configuration.			*/
/*				This routine allows to enable MSP with specified	 		*/
/*				direction. 													*/
/*              															*/
/* PARAMETERS: 																*/
/*	msp_device_id	: MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2		*/
/*																			*/
/* RETURN:	TRUE if successful												*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_bool SER_MSP_Start(IN t_msp_device_id msp_device_id, t_msp_direction msp_direction)
{
    t_msp_error msp_error;

    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(t_bool) FALSE;
    }

    g_ser_msp_context[msp_device_id].msp_direction = msp_direction;

    if (g_ser_msp_context[msp_device_id].is_msp_configured != (t_bool)TRUE)
    {
        PRINT("<E> SER_MSP: SER_MSP_Start: MSP not configured\n");
    }

    SER_MSP_Stop(msp_device_id);
    msp_error = MSP_Enable(msp_device_id, g_ser_msp_context[msp_device_id].msp_direction);
    if (msp_error != MSP_OK)
    {
        PRINT("<E> MSP_SER:SER_MSP_Start: MSP_Enable returned %x\n", msp_error);
        return(t_bool) FALSE;
    }

    g_ser_msp_context[msp_device_id].is_msp_enabled = (t_bool) TRUE;

    return(t_bool) TRUE;
}

/****************************************************************************/
/* NAME:    SER_MSP_Stop						                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: this routine disables the msp and maintains internal SM.	*/
/*              															*/
/* PARAMETERS: 																*/
/*	msp_device_id : MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2			*/
/*																			*/
/* RETURN:	TRUE if successful												*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_bool SER_MSP_Stop(IN t_msp_device_id msp_device_id)
{
    t_msp_error msp_error;

    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(t_bool) FALSE;
    }

    g_ser_msp_context[msp_device_id].is_msp_enabled = (t_bool) FALSE;

    msp_error = MSP_Disable(msp_device_id, g_ser_msp_context[msp_device_id].msp_direction);
    if (msp_error != MSP_OK)
    {
        PRINT("<E> SER_MSP:SER_MSP_Stop:MSP_Enable returned %x\n", msp_error);
        return(t_bool) FALSE;
    }

    return(t_bool) TRUE;
}

/****************************************************************************/
/* NAME:    SER_MSP_Reset						                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clears all the settings, global variables.		*/
/*																			*/
/*				Call this routine to reset everything.						*/
/*              															*/
/* PARAMETERS: 																*/
/*	msp_device_id : MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2			*/
/*																			*/
/* RETURN:	TRUE if successful												*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_bool SER_MSP_Reset(IN t_msp_device_id msp_device_id)
{
    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(t_bool) FALSE;
    }

    /*PRINT("\n SER_MSP_Reset");*/
    g_ser_msp_context[msp_device_id].tx_msp_mode = SER_MSP_DEFAULT_WORK_MODE;
    g_ser_msp_context[msp_device_id].rx_msp_mode = SER_MSP_DEFAULT_WORK_MODE;
    g_ser_msp_context[msp_device_id].msp_direction = SER_MSP_DEFAULT_DIRECTION;
    g_ser_msp_context[msp_device_id].msp_sample_freq = 8000;
    g_ser_msp_context[msp_device_id].ser_msp_protocol = SER_MSP_DEFAULT_PROTOCOL;

    g_ser_msp_context[msp_device_id].is_msp_enabled = (t_bool) FALSE;
    g_ser_msp_context[msp_device_id].is_msp_configured = (t_bool) FALSE;
    g_ser_msp_context[msp_device_id].tx_in_progress = (t_bool) FALSE;
    g_ser_msp_context[msp_device_id].rx_in_progress = (t_bool) FALSE;

    /*PRINT("<I> SER_MSP: Reset called \n");*/
    if (MSP_Disable(msp_device_id, g_ser_msp_context[msp_device_id].msp_direction) != MSP_OK)
    {
        return(t_bool) FALSE;
    }

    if (MSP_Reset(msp_device_id) != MSP_OK)
    {
        return(t_bool) FALSE;
    }

    /*PRINT("<I> SER_MSP: Reset Exit\n");*/
    return(t_bool) TRUE;
}

/****************************************************************************/
/* NAME:    SER_MSP_InstallDefaultHandler		                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine installs default interrupt handler 			*/
/*				Call this routine to restore MSP interrupt handler.			*/
/*              															*/
/* PARAMETERS: 																*/
/*	msp_device_id : MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2			*/
/*																			*/
/* RETURN:	none															*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_ser_error SER_MSP_InstallDefaultHandler(IN t_msp_device_id msp_device_id)
{
    t_gic_error gic_error;
    t_gic_func_ptr    old_datum;
    t_gic_line  gic_line_for_msp;

    switch (msp_device_id)
    {
        case MSP_DEVICE_ID_0:
        default:
            gic_line_for_msp = GIC_MSP_0_LINE;
            break;

        case MSP_DEVICE_ID_1:
            gic_line_for_msp = GIC_MSP_1_LINE;
            break;

        case MSP_DEVICE_ID_2:
            gic_line_for_msp = GIC_MSP_2_LINE;
            break;
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
        case MSP_DEVICE_ID_3:
			gic_line_for_msp = GIC_MSP_3_LINE;
			break;
#endif
	};

    gic_error = GIC_DisableItLine(gic_line_for_msp);
    if (0 > (t_sint32) gic_error)
    {
        PRINT("<E> SER_MSP:SER_MSP_InstallDefaultHandler:GIC_DisableItLine::Error in Disabling Interrupt for MSP_LINE");

        return(SERVICE_FAILED);
    }

    switch (msp_device_id)
    {
        case MSP_DEVICE_ID_0:
            gic_error = GIC_ChangeDatum(gic_line_for_msp, SER_MSP0_InterruptHandler, &old_datum);
            break;

        case MSP_DEVICE_ID_1:
            gic_error = GIC_ChangeDatum(gic_line_for_msp, SER_MSP1_InterruptHandler, &old_datum);
            break;

        case MSP_DEVICE_ID_2:
            gic_error = GIC_ChangeDatum(gic_line_for_msp, SER_MSP2_InterruptHandler, &old_datum);
            break;
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
		case MSP_DEVICE_ID_3:
			gic_error = GIC_ChangeDatum(gic_line_for_msp, SER_MSP3_InterruptHandler, &old_datum);
			break;
#endif
        
        default:
            return(SERVICE_FAILED);
	};

    if (0 > (t_sint32) gic_error)
    {
        PRINT("<E> SER_MSP:SER_MSP_InstallDefaultHandler:GIC_ChangeDatum::Error in Binding Interrupt for MSP_LINE");
        return(SERVICE_FAILED);
    }

    gic_error = GIC_EnableItLine(gic_line_for_msp);
    if (0 > (t_sint32) gic_error)
    {
        PRINT("<E> SER_MSP:SER_MSP_InstallDefaultHandler:GIC_EnableItLine::Error in Enabling Interrupt for MSP_LINE");
        return(SERVICE_FAILED);
    }


    return(SERVICE_OK);
}

/****************************************************************************/
/* NAME:    SER_MSP_RegisterCallback			                       		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to register users call back functions.	*/
/*				The function will be called when following values returned	*/
/*				from MSP_FilterProcessIRQSrc:								*/
/*				1. MSP_REMAINING_PENDING_EVENTS								*/
/*				2. MSP_TRANSMISSION_ON_GOING								*/
/*																			*/
/*				When call back function will be invoked due to any of the   */
/*				events mention above, user will receive two parameters.		*/
/*				1. param : same as passed during registartion				*/
/*				2. &t_ser_map_param : the stucture contains information     */
/*							about events, errors etc.						*/
/*																			*/
/*				Call this function register your call back function.		*/
/*              															*/
/* PARAMETERS: 																*/
/*	msp_device_id : MSP_DEVICE_ID_0/MSP_DEVICE_ID_1/MSP_DEVICE_ID_2 		*/
/*		callback_fct : pointer to function of type t_callback_fct			*/
/*		param		 : value which will be passed back to the function 		*/
/*						pointed by "callback_fct" as first parameter		*/
/*              															*/
/* RETURN:	HCL_OK always													*/
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_ser_error SER_MSP_RegisterCallback(IN t_msp_device_id msp_device_id, IN t_callback_fct callback_fct, IN void *param)
{
    if ((t_uint32)msp_device_id >= SER_MSP_INSTANCES)
    {
        return(SERVICE_FAILED);
    }

    g_ser_msp_context[msp_device_id].callback_msp.fct = callback_fct;
    g_ser_msp_context[msp_device_id].callback_msp.param = param;
    return(SERVICE_OK);
}

/*--------------------------------------------------------------------------*
 * Public functions: Called by services.c									*
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_MSP_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the msp	                            */
/*                                                                          */
/* PARAMETERS: 		                                                        */
/* IN:																		*/
/* mask: initialization mask corresonding to MSP Device id to               */
/* initialize perticular instance of MSP                                    */
/* OUT:                                                                     */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_MSP_Init(IN t_uint8 mask)
{
    t_version           version;
    t_gic_func_ptr      interrupt_handler = NULL;
    t_gic_func_ptr      old_datum;
    t_gic_line          gic_line = GIC_MSP_0_LINE;
    t_msp_device_id     msp_device_id;
    t_logical_address   msp_base_address;
    t_uint8 temp_mask = mask, device_count = 0;
    
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined ST_8500V2)||(defined ST_HREFV2)||(defined __PEPS_8500V2))   
    
    /* Enabling MSP clock by writing at PRCM_MSPCLK_MGT_REG = PRCMU_BASE_ADDRESS + 0x01C */
    *((t_uint32 *)(PRCMU_REG_BASE_ADDR + PRCMU_MSP_CLK_EN_OFFSET)) = MSP_CLK_EN_VAL;
    
    /* AMBA & Kernel clock enable for Peripheral1-MSP0/1 */
	/* For V2.0 MSP 3 will also get Enabled */
	
    (*(volatile t_uint32 *)(PRCC_1_CTRL_REG_BASE_ADDR))                             |= MSP0_AMBA_CLK_EN_VAL;
    (*(volatile t_uint32 *)(PRCC_1_CTRL_REG_BASE_ADDR + PRCC_KERNEL_CLK_EN_OFFSET)) |= MSP0_KERNEL_CLK_EN_VAL;
    
    /* AMBA & Kernel clock enable for Peripheral2-MSP2 */
    (*(volatile t_uint32 *)(PRCC_2_CTRL_REG_BASE_ADDR))                             |= MSP2_AMBA_CLK_EN_VAL;
    (*(volatile t_uint32 *)(PRCC_2_CTRL_REG_BASE_ADDR + PRCC_KERNEL_CLK_EN_OFFSET)) |= MSP2_KERNEL_CLK_EN_VAL;
    
#endif


    while (temp_mask && device_count <= MSP_LAST_DEVICE_ID)
    {
        temp_mask >>= 1;
        device_count++;
    }

    if (SER_MSP_INSTANCES < device_count)
    {
        return;
    }

    if (1 == SER_MSP_INSTANCES)
    {
        if (INIT_MSP0 != mask)
        {
            /* PRINT("\nError <E> SER_MSP:SER_MSP_Init: Invalid mask %X", mask);*/
            return;
        }
    }
    
    
	
    if (INIT_MSP2 == mask)
    {
        if (3 != SER_MSP_INSTANCES)
        {
            /* PRINT("\nError <E> SER_MSP:SER_MSP_Init: Invalid mask %X", mask);*/
            return;
        }
    }
#if ((defined ST_8500V2)||(defined ST_HREFV2)||(defined __PEPS_8500V2))
	if (INIT_MSP3 == mask)
	{
		if(4 != SER_MSP_INSTANCES)
		{
			return; 
		}
	}
#endif

    if (MSP_OK == MSP_GetVersion(&version))
    {
        PRINT(" Version %d.%d.%d ", version.version, version.major, version.minor);
    }
    else
    {
        PRINT("\nError <E> SER_MSP:SER_MSP_Init:from MSP_GetVersion\n");
        return;
    }
            

    switch (mask)
    {
        case INIT_MSP0:
            msp_device_id = MSP_DEVICE_ID_0;
            msp_base_address = BASE_ADDR_MSP0;
            gic_line = GIC_MSP_0_LINE;
            interrupt_handler = SER_MSP0_InterruptHandler;
            
    #if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V2)||(defined ST_HREFV2))
            if (GPIO_EnableAltFunction(GPIO_ALT_MSP0) != GPIO_OK)
            {
                PRINT("<E> MSP_SER:from GPIO_EnableAltFunction\n");
                return;
            }
    #endif
            break;

        case INIT_MSP1:
            msp_device_id = MSP_DEVICE_ID_1;
            msp_base_address = BASE_ADDR_MSP1;
            gic_line = GIC_MSP_1_LINE;
            interrupt_handler = SER_MSP1_InterruptHandler;
            
   #if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V2)||(defined ST_HREFV2))
            if (GPIO_EnableAltFunction(GPIO_ALT_MSP1) != GPIO_OK)
            {
                PRINT("<E> MSP_SER:from GPIO_EnableAltFunction\n");
                return;
            }
    #endif
            break;
    
    
    
        case INIT_MSP2:
            msp_device_id = MSP_DEVICE_ID_2;
            msp_base_address = BASE_ADDR_MSP2;
            gic_line = GIC_MSP_2_LINE;
            interrupt_handler = SER_MSP2_InterruptHandler; 
			
    #if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V2)||(defined ST_HREFV2))
            if (GPIO_EnableAltFunction(GPIO_ALT_MSP2) != GPIO_OK)
            {
                PRINT("<E> MSP_SER:from GPIO_EnableAltFunction\n");
                return;
            }
    
            break;
    #endif
		
	#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
        case INIT_MSP3:
            msp_device_id = MSP_DEVICE_ID_3;
            msp_base_address = BASE_ADDR_MSP3;
            gic_line = GIC_MSP_3_LINE;
            interrupt_handler = SER_MSP3_InterruptHandler;            
    #if ((defined ST_8500V2)||(defined ST_HREFV2))
            if (GPIO_EnableAltFunction(GPIO_ALT_MSP1) != GPIO_OK)
            {
                PRINT("<E> MSP_SER:from GPIO_EnableAltFunction\n");
                return;
            }
    #endif /*only for V2 Hardware*/
            break;
    #endif /*Only for V2 Hardware and PEPS V2.0*/
		default:
            msp_device_id = MSP_DEVICE_ID_0;
            msp_base_address = BASE_ADDR_MSP0;
            gic_line = GIC_MSP_0_LINE;
            interrupt_handler = SER_MSP0_InterruptHandler;
            PRINT("<E> SER_MSP:SER_MSP_Init:Invalid Mask Value = %X", mask);
            break;
    }




    if (GIC_DisableItLine(gic_line) != GIC_OK)
    {
        PRINT("<E> SER_MSP:SER_MSP_Init:from GIC_DisableItLine\n");
        return;
    }

    if (GIC_ChangeDatum(gic_line, interrupt_handler, &old_datum) != GIC_OK)
    {
        PRINT("<E> SER_MSP:SER_MSP_Init:from GIC_ChangeDatum\n");
        return;
    }

    if (MSP_Init(msp_device_id, msp_base_address) != MSP_OK)
    {
        PRINT("<E> SER_MSP: from MSP_SetBaseAddress\n");
        return;
    }

    MSP_SetBaseAddress(msp_device_id, msp_base_address);


    if (GIC_EnableItLine(gic_line) != GIC_OK)
    {
        PRINT("<E> SER_MSP:SER_MSP_Init:from GIC_EnableItLine\n");
        return;
    }

    /* Initialise g_ser_msp_context */
    {
        g_ser_msp_context[msp_device_id].tx_msp_mode = SER_MSP_DEFAULT_WORK_MODE;
        g_ser_msp_context[msp_device_id].rx_msp_mode = SER_MSP_DEFAULT_WORK_MODE;
        g_ser_msp_context[msp_device_id].msp_direction = SER_MSP_DEFAULT_DIRECTION;
        g_ser_msp_context[msp_device_id].msp_sample_freq = 8000;
        g_ser_msp_context[msp_device_id].ser_msp_protocol = SER_MSP_DEFAULT_PROTOCOL;

        /*g_ser_msp_context[msp_device_id].msp_protocol_descriptor; */
        /* Not initialised */
        g_ser_msp_context[msp_device_id].p_msp_protocol_desc = NULL;

        g_ser_msp_context[msp_device_id].tx_in_progress = (t_bool) FALSE;
        g_ser_msp_context[msp_device_id].rx_in_progress = (t_bool) FALSE;
        g_ser_msp_context[msp_device_id].is_msp_configured = (t_bool) FALSE;
        g_ser_msp_context[msp_device_id].is_msp_enabled = (t_bool) FALSE;

        /*g_ser_msp_context[msp_device_id].codec_exch_id;*/
        /* Not initialised */
        g_ser_msp_context[msp_device_id].callback_msp.fct = NULL;
        g_ser_msp_context[msp_device_id].callback_msp.param = 0;
    }
}

/****************************************************************************/
/* NAME:    SER_MSP_Close               	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine close services for ssp			                */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_MSP_Close(void)
{
    SER_MSP_Reset(MSP_DEVICE_ID_0); /*is similar to close*/

    SER_MSP_Reset(MSP_DEVICE_ID_1); /*is similar to close*/
    SER_MSP_Reset(MSP_DEVICE_ID_2); /*is similar to close*/
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
	SER_MSP_Reset(MSP_DEVICE_ID_3);
#endif
    
}

/****************************************************************************/
/* NAME:    MSP_Delay               	                                	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: 															*/
/* This API is implemented out side HCL. MSP HCL APIs will call this 		*/
/* function with appropriate value of delay to provide required delay.      */
/*                                                                          */
/* PARAMETERS: 		                                                        */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2       */
/* delay: required delay in microseconds.                                   */
/* OUT:                                                                     */
/* None                                                                     */
/*              															*/
/* RETURN:                                                                  */
/* always MSP_OK                                                            */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
#ifndef SER_MSP_DONOT_IMPLEMENT_MSP_HCL_API
PUBLIC t_msp_error MSP_Delay(IN t_msp_device_id msp_device_id, IN t_uint32 delay)
{
    t_msp_error msp_error = MSP_OK;

    /* For accurate delay this API may be implemented using Timer HCL*/
    while (delay--)
    {
        volatile t_uint32   loop_count;
        for (loop_count = 0; loop_count < 2; loop_count++)
            ;

        /* assuming this delay equal to 1us */
    }

    return(msp_error);
}
#endif

