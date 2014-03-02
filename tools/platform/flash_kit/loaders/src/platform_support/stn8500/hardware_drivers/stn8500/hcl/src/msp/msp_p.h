/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file for MSP
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _MSPP_H_
#define _MSPP_H_

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "msp.h"

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
#define MSP_HCL_VERSION_ID  2
#define MSP_HCL_MAJOR_ID    2
#define MSP_HCL_MINOR_ID    0

/*--------------------------------------------------------------------------*
 * Peripheral & Cell Ids   														*
 *--------------------------------------------------------------------------*/

#define MSP_P_ID0   0x21
#define MSP_P_ID1   0x00
#define MSP_P_ID2   0x28
#define MSP_P_ID3   0x00

#define MSP_CELL_ID0   0x0D
#define MSP_CELL_ID1   0xF0
#define MSP_CELL_ID2   0x05
#define MSP_CELL_ID3   0xB1

/*--------------------------------------------------------------------------*
 * INTERNAL DEFINES															*
 *--------------------------------------------------------------------------*/
#define MSP_SERIAL_CLOCK_DIV            8
#define MSP_FIFO_DEPTH                  8
#define MSP_REGISTER_RESET_VALUE        0x00000000
#define MSP_CLEAR_ALL_INTERRUPT         0x000000EE
#define MSP_RFE_FLAG_BIT                0x02
#define MSP_TFE_FLAG_BIT                0x10
#define MSP_TX_RX_FIFO_ENABLE           0x00000202
#define MSP_INTEGRATION_TEST_MODE       0x00000003
#define MSP_EIGHT_BYTES                 8
#define MSP_SIXTEEN_BYTES               16
#define MSP_FRAME_PERIOD_IN_MONO_MODE   256
#define MSP_FRAME_WIDTH_IN_MONO_MODE    1

#define MSP_FRAME_PERIOD_IN_STEREO_MODE 32
#define MSP_FRAME_WIDTH_IN_STEREO_MODE  16
#define MSP_SHIFT_BY_EIGHT              8
#define MSP_SHIFT_BY_FIVE               5

#if ((defined ST_8500ED)||(defined ST_HREFED)||(defined ST_8500V1)||(defined ST_HREFV1)||(__PEPS_8500)||(__PEPS_8500_V1))
#define MSP_FIFO_WATERMARK_LEVEL        4
#endif

typedef struct
{
    t_bool                      tx_enable;
    t_bool                      rx_enable;
    t_uint32                    serial_clock_divisor;
    t_uint32                    msp_in_clock_frequency;
    t_bool                      msp_general_configuration_done;
    t_bool                      msp_protocol_descriptor_set;
    t_bool                      serial_clock_divisor_set;
    t_bool                      multichannel_configured;
    t_uint16                    padding;
    volatile t_uint32           imsc_reg;
    t_msp_general_config        general_config;
    t_msp_multichannel_config   multichannel_config;
    t_msp_protocol_descriptor   protocol_descriptor;
} t_msp_device_context;

/* Transmit/Receive shifter status
-----------------------------------*/
typedef enum
{
    MSP_SHIFTER_IDLE    = 0,
    MSP_SHIFTER_WORKING = 1
} t_msp_shifter_status;

/* Current status (used for receive and transmit modes)
* WorkMode:		IT, polling or DMA
* PhaseMode:		Single or dual phases
* StereoMode:		Mono or stereo
* Padding:			Not used. To have an aligned structure
* pItDataFlow:		Pointer on the current data transfered in IT mode
* ItHalfwordsNb:	Data number to be still transfered with the current call to MSP_Transmit(Receive)Data
* FlowErrorNb:		Flow errors number (underflow in transmit mode, overflow for receive operations)
----------------------------------------------------------------------------------------------------------*/
typedef struct
{
    t_msp_phase_mode    phase_mode;
    t_uint8             padding;
    volatile t_uint8    *p_it_data_flow;
    volatile t_uint32   it_bytes_nb;
    volatile t_uint32   flow_error_nb;
} t_msp_mode_status;

typedef struct
{
    t_msp_register          *p_msp_register;            /* Pointer to MSP registers structure */
    t_msp_mode_status       rx_status;                  /* Receive mode description*/
    t_msp_mode_status       tx_status;                  /* Transmit mode description*/
    t_msp_device_context    msp_device_context;         /* keep updated copy of parameters passed */
    t_msp_device_context    msp_device_context_main;    /* used for storing device context*/
    volatile t_msp_event    msp_event_status;           /* Each event corresponds to a bit set to TRUE when the event is active.*/
    volatile t_uint32       flag_for_transmit_interrupt_mode;
    volatile t_uint32       flag_for_receive_interrupt_mode;
} t_msp_system_context;

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
/* Macros especially dedicated for bits operations
* Casts to t_uint32 are made mandatory because of warnings at compilation stage
* Variables signification
* reg:		register to be written (read)
* val:		value to be written (read)
* bit_nb:	number of bits where the value, a "0" or a "1" will be written (read)
* pos:		position of the value to be written (read).
------------------------------------------------------------------------------*/

/* With shift operators !
-----------------------------*/
#define BYTE_TO_WORD(x)     (x >> 2)
#define WORD_TO_BYTE(x)     (x << 2)
#define HALFWORD_TO_BYTE(x) (x << 1)
#define BYTE_TO_HALFWORD(x) (x >> 1)
#endif /* _MSPP_H_*/

/* End of file - mspp.h*/

