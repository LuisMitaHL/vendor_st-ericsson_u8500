/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public header file for the MSP
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _MSP_H_
#define _MSP_H_

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "debug.h"
#include "msp_irq.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Constants and new types													*
 * Values defined with an enum item have a real hardware signification.
 * They represent a real hardware register value. Be carefull when changing !
 * Except for the t_msp_error type.
 *--------------------------------------------------------------------------*/
/* Errors related values
-----------------------*/
typedef enum
{
    MSP_TRANSMIT_FIFO_TIMEOUT           = HCL_MAX_ERROR_VALUE - 3,
    MSP_NON_AUTHORIZED_MODE             = HCL_MAX_ERROR_VALUE - 2,
    MSP_NO_ACTIVE_IT_ERROR              = HCL_MAX_ERROR_VALUE - 1,
    MSP_TRANSMISSION_ON_GOING           = HCL_MAX_ERROR_VALUE,
    MSP_INTERNAL_ERROR                  = HCL_INTERNAL_ERROR,                   /*(-8)*/
    MSP_NOT_CONFIGURED                  = HCL_NOT_CONFIGURED,                   /*(-7)*/
    MSP_REQUEST_PENDING                 = HCL_REQUEST_PENDING,                  /*(-6)*/
    MSP_REQUEST_NOT_APPLICABLE          = HCL_REQUEST_NOT_APPLICABLE,           /*(-5)*/
    MSP_INVALID_PARAMETER               = HCL_INVALID_PARAMETER,                /*(-4)*/
    MSP_UNSUPPORTED_FEATURE             = HCL_UNSUPPORTED_FEATURE,              /*(-3)*/
    MSP_UNSUPPORTED_HW                  = HCL_UNSUPPORTED_HW,                   /*(-2)*/
    MSP_ERROR                           = HCL_ERROR,                            /*(-1)*/
    MSP_OK                              = HCL_OK,                               /*( 0)*/
    MSP_INTERNAL_EVENT                  = HCL_INTERNAL_EVENT,                   /*( 1)*/
    MSP_REMAINING_PENDING_EVENTS        = HCL_REMAINING_PENDING_EVENTS,         /*( 2)*/
    MSP_REMAINING_FILTER_PENDING_EVENTS = HCL_REMAINING_FILTER_PENDING_EVENTS,  /*( 3)*/
    MSP_NO_MORE_PENDING_EVENT           = HCL_NO_MORE_PENDING_EVENT,            /*( 4)*/
    MSP_NO_MORE_FILTER_PENDING_EVENT    = HCL_NO_MORE_FILTER_PENDING_EVENT,     /*( 5)*/
    MSP_NO_PENDING_EVENT_ERROR          = HCL_NO_PENDING_EVENT_ERROR            /*( 7)*/
} t_msp_error;

/* Interrupt sources & events
-----------------------*/
typedef t_uint32    t_msp_event;
typedef t_uint32    t_msp_filter_mode;


extern t_bool transfer_complete;

#define MSP_NO_FILTER_MODE  0

typedef enum
{
    MSP_IRQ_STATE_NEW                   = 0,
    MSP_IRQ_STATE_OLD                   = 1
} t_msp_irq_state;

/* Interrupt management global structure
-----------------------------------------*/
typedef struct
{
    t_msp_irq_state interrupt_state;
    /*t_uint8         padding[3];*/
    t_msp_irq_src   initial_irq;    /* To be done */
    t_msp_irq_src   pending_irq;
} t_msp_irq_status;

/* Transmit/Receive mode
-------------------------*/
typedef enum
{
    MSP_DIRECTION_TRANSMIT,
    MSP_DIRECTION_RECEIVE,
    MSP_DIRECTION_TRANSMIT_AND_RECEIVE
} t_msp_direction;

/* Work mode
------------*/
typedef enum
{
    MSP_MODE_POLLING,
    MSP_MODE_IT,
    MSP_MODE_DMA
} t_msp_mode;

/* Transfer width:
1. Used in NON-DMA mode
2. 	MSP_DATA_TRANSFER_WIDTH_BYTE specifies that one BYTE from given buffer will be transfer 
	to/from MSP_DR (Msp Data Register) at once.
	MSP_DATA_TRANSFER_WIDTH_HALFWORD specifies that 16-bit from given buffer will be transfer 
	to/from MSP_DR (Msp Data Register) at once.
	MSP_DATA_TRANSFER_WIDTH_WORD specifies that one WORD (32-bit) from given buffer will be
	transfer to/from MSP_DR (Msp Data Register) at once.
	
------------------------------------------------------------------------------------*/
typedef enum
{
    MSP_DATA_TRANSFER_WIDTH_BYTE,
    MSP_DATA_TRANSFER_WIDTH_HALFWORD,
    MSP_DATA_TRANSFER_WIDTH_WORD
} t_msp_data_transfer_width;

/* Single or dual phase mode
------------------------------*/
typedef enum
{
    MSP_PHASE_MODE_SINGLE               = 0,
    MSP_PHASE_MODE_DUAL                 = 1
} t_msp_phase_mode;

/* Frame length
------------------*/
typedef enum
{
    MSP_FRAME_LENGTH_1                  = 0,
    MSP_FRAME_LENGTH_2                  = 1,
    MSP_FRAME_LENGTH_3                  = 2,
    MSP_FRAME_LENGTH_4                  = 3,
    MSP_FRAME_LENGTH_5                  = 4,
    MSP_FRAME_LENGTH_6                  = 5,
    MSP_FRAME_LENGTH_7                  = 6,
    MSP_FRAME_LENGTH_8                  = 7,
    MSP_FRAME_LENGTH_9                  = 8,
    MSP_FRAME_LENGTH_10                 = 9,
    MSP_FRAME_LENGTH_11                 = 10,
    MSP_FRAME_LENGTH_12                 = 11,
    MSP_FRAME_LENGTH_13                 = 12,
    MSP_FRAME_LENGTH_14                 = 13,
    MSP_FRAME_LENGTH_15                 = 14,
    MSP_FRAME_LENGTH_16                 = 15,
    MSP_FRAME_LENGTH_17                 = 16,
    MSP_FRAME_LENGTH_18                 = 17,
    MSP_FRAME_LENGTH_19                 = 18,
    MSP_FRAME_LENGTH_20                 = 19,
    MSP_FRAME_LENGTH_21                 = 20,
    MSP_FRAME_LENGTH_22                 = 21,
    MSP_FRAME_LENGTH_23                 = 22,
    MSP_FRAME_LENGTH_24                 = 23,
    MSP_FRAME_LENGTH_25                 = 24,
    MSP_FRAME_LENGTH_26                 = 25,
    MSP_FRAME_LENGTH_27                 = 26,
    MSP_FRAME_LENGTH_28                 = 27,
    MSP_FRAME_LENGTH_29                 = 28,
    MSP_FRAME_LENGTH_30                 = 29,
    MSP_FRAME_LENGTH_31                 = 30,
    MSP_FRAME_LENGTH_32                 = 31,
    MSP_FRAME_LENGTH_33                 = 32,
    MSP_FRAME_LENGTH_34                 = 33,
    MSP_FRAME_LENGTH_35                 = 34,
    MSP_FRAME_LENGTH_36                 = 35,
    MSP_FRAME_LENGTH_37                 = 36,
    MSP_FRAME_LENGTH_38                 = 37,
    MSP_FRAME_LENGTH_39                 = 38,
    MSP_FRAME_LENGTH_40                 = 39,
    MSP_FRAME_LENGTH_41                 = 40,
    MSP_FRAME_LENGTH_42                 = 41,
    MSP_FRAME_LENGTH_43                 = 42,
    MSP_FRAME_LENGTH_44                 = 43,
    MSP_FRAME_LENGTH_45                 = 44,
    MSP_FRAME_LENGTH_46                 = 45,
    MSP_FRAME_LENGTH_47                 = 46,
    MSP_FRAME_LENGTH_48                 = 47,
    MSP_FRAME_LENGTH_49                 = 48,
    MSP_FRAME_LENGTH_50                 = 49,
    MSP_FRAME_LENGTH_51                 = 50,
    MSP_FRAME_LENGTH_52                 = 51,
    MSP_FRAME_LENGTH_53                 = 52,
    MSP_FRAME_LENGTH_54                 = 53,
    MSP_FRAME_LENGTH_55                 = 54,
    MSP_FRAME_LENGTH_56                 = 55,
    MSP_FRAME_LENGTH_57                 = 56,
    MSP_FRAME_LENGTH_58                 = 57,
    MSP_FRAME_LENGTH_59                 = 58,
    MSP_FRAME_LENGTH_60                 = 59,
    MSP_FRAME_LENGTH_61                 = 60,
    MSP_FRAME_LENGTH_62                 = 61,
    MSP_FRAME_LENGTH_63                 = 62,
    MSP_FRAME_LENGTH_64                 = 63,
    MSP_FRAME_LENGTH_65                 = 64,
    MSP_FRAME_LENGTH_66                 = 65,
    MSP_FRAME_LENGTH_67                 = 66,
    MSP_FRAME_LENGTH_68                 = 67,
    MSP_FRAME_LENGTH_69                 = 68,
    MSP_FRAME_LENGTH_70                 = 69,
    MSP_FRAME_LENGTH_71                 = 70,
    MSP_FRAME_LENGTH_72                 = 71,
    MSP_FRAME_LENGTH_73                 = 72,
    MSP_FRAME_LENGTH_74                 = 73,
    MSP_FRAME_LENGTH_75                 = 74,
    MSP_FRAME_LENGTH_76                 = 75,
    MSP_FRAME_LENGTH_77                 = 76,
    MSP_FRAME_LENGTH_78                 = 77,
    MSP_FRAME_LENGTH_79                 = 78,
    MSP_FRAME_LENGTH_80                 = 79,
    MSP_FRAME_LENGTH_81                 = 80,
    MSP_FRAME_LENGTH_82                 = 81,
    MSP_FRAME_LENGTH_83                 = 82,
    MSP_FRAME_LENGTH_84                 = 83,
    MSP_FRAME_LENGTH_85                 = 84,
    MSP_FRAME_LENGTH_86                 = 85,
    MSP_FRAME_LENGTH_87                 = 86,
    MSP_FRAME_LENGTH_88                 = 87,
    MSP_FRAME_LENGTH_89                 = 88,
    MSP_FRAME_LENGTH_90                 = 89,
    MSP_FRAME_LENGTH_91                 = 90,
    MSP_FRAME_LENGTH_92                 = 91,
    MSP_FRAME_LENGTH_93                 = 92,
    MSP_FRAME_LENGTH_94                 = 93,
    MSP_FRAME_LENGTH_95                 = 94,
    MSP_FRAME_LENGTH_96                 = 95,
    MSP_FRAME_LENGTH_97                 = 96,
    MSP_FRAME_LENGTH_98                 = 97,
    MSP_FRAME_LENGTH_99                 = 98,
    MSP_FRAME_LENGTH_100                = 99,
    MSP_FRAME_LENGTH_101                = 100,
    MSP_FRAME_LENGTH_102                = 101,
    MSP_FRAME_LENGTH_103                = 102,
    MSP_FRAME_LENGTH_104                = 103,
    MSP_FRAME_LENGTH_105                = 104,
    MSP_FRAME_LENGTH_106                = 105,
    MSP_FRAME_LENGTH_107                = 106,
    MSP_FRAME_LENGTH_108                = 107,
    MSP_FRAME_LENGTH_109                = 108,
    MSP_FRAME_LENGTH_110                = 109,
    MSP_FRAME_LENGTH_111                = 110,
    MSP_FRAME_LENGTH_112                = 111,
    MSP_FRAME_LENGTH_113                = 112,
    MSP_FRAME_LENGTH_114                = 113,
    MSP_FRAME_LENGTH_115                = 114,
    MSP_FRAME_LENGTH_116                = 115,
    MSP_FRAME_LENGTH_117                = 116,
    MSP_FRAME_LENGTH_118                = 117,
    MSP_FRAME_LENGTH_119                = 118,
    MSP_FRAME_LENGTH_120                = 119,
    MSP_FRAME_LENGTH_121                = 120,
    MSP_FRAME_LENGTH_122                = 121,
    MSP_FRAME_LENGTH_123                = 122,
    MSP_FRAME_LENGTH_124                = 123,
    MSP_FRAME_LENGTH_125                = 124,
    MSP_FRAME_LENGTH_126                = 125,
    MSP_FRAME_LENGTH_127                = 126,
    MSP_FRAME_LENGTH_128                = 127
} t_msp_frame_length;

/* Element length
---------------------*/
typedef enum
{
    MSP_ELEMENT_LENGTH_8                = 0,
    MSP_ELEMENT_LENGTH_10               = 1,
    MSP_ELEMENT_LENGTH_12               = 2,
    MSP_ELEMENT_LENGTH_14               = 3,
    MSP_ELEMENT_LENGTH_16               = 4,
    MSP_ELEMENT_LENGTH_20               = 5,
    MSP_ELEMENT_LENGTH_24               = 6,
    MSP_ELEMENT_LENGTH_32               = 7
} t_msp_element_length;

/* Data delay (in bit clock cycles)
---------------------------------------*/
typedef enum
{
    MSP_DATA_DELAY_0                    = 0,
    MSP_DATA_DELAY_1                    = 1,
    MSP_DATA_DELAY_2                    = 2,
    MSP_DATA_DELAY_3                    = 3
} t_msp_data_delay;

/* Configurations of clocks (transmit, receive or sample rate generator)
-------------------------------------------------------------------------*/
typedef enum
{
    MSP_CLOCK_EDGE_RISING               = 0,
    MSP_CLOCK_EDGE_FALLING              = 1
} t_msp_clock_edge;

/*Defines when Phase 2 starts.*/
typedef enum
{
    MSP_PHASE2_START_MODE_IMEDIATE,
    MSP_PHASE2_START_MODE_FRAME_SYNC
} t_msp_phase2_start_mode;

/* Endianess
--------------*/
typedef enum
{
    MSP_BTF_MS_BIT_FIRST                = 0,
    MSP_BTF_LS_BIT_FIRST                = 1
} t_msp_bit_transfer_format;

/* Half word swapping
---------------------*/
typedef enum
{
    MSP_HWS_NO_SWAP                     = 0,
    MSP_HWS_BYTE_SWAP_IN_WORD           = 1,
    MSP_HWS_BYTE_SWAP_IN_EACH_HALF_WORD = 2,
    MSP_HWS_HALF_WORD_SWAP_IN_WORD      = 3
} t_msp_half_word_swap;

/* Frame sync polarity
-----------------------*/
typedef enum
{
    MSP_FRAME_SYNC_POL_ACTIVE_HIGH      = 0,
    MSP_FRAME_SYNC_POL_ACTIVE_LOW       = 1
} t_msp_frame_sync_pol;

/* Expanding capabilities
----------------------------*/
typedef enum
{
    MSP_EXPAND_MODE_LINEAR              = 0,
    MSP_EXPAND_MODE_LINEAR_SIGNED       = 1,
    MSP_EXPAND_MODE_MU_LAW              = 2,
    MSP_EXPAND_MODE_A_LAW               = 3
} t_msp_expand_mode;

/* Companding capabilities
-----------------------------*/
typedef enum
{
    MSP_COMPRESS_MODE_LINEAR            = 0,
    MSP_COMPRESS_MODE_MU_LAW            = 2,
    MSP_COMPRESS_MODE_A_LAW             = 3
} t_msp_compress_mode;

/* SPI Clock Modes enumertion
SPI clock modes of MSP provides compatibility with 
the SPI protocol.MSP supports 2 SPI transfer formats.
MSP_SPI_CLOCK_MODE_ZERO_DELAY:MSP transmits data over Tx/Rx 
Lines immediately after MSPTCK/MSPRCK rising/falling edge.
MSP_SPI_CLOCK_MODE_HALF_CYCLE_DELAY:MSP transmits data  one-half cycle 
ahead of the rising/falling edge of the MSPTCK
-------------------------------------------------------------*/
typedef enum
{
    MSP_SPI_CLOCK_MODE_NON_SPI          = 0,
    MSP_SPI_CLOCK_MODE_ZERO_DELAY       = 2,
    MSP_SPI_CLOCK_MODE_HALF_CYCLE_DELAY = 3
} t_msp_spi_clock_modes;

/*Selection of SPI burst mode.*/
typedef enum
{
    MSP_SPI_BURST_MODE_DISABLE          = 0,
    MSP_SPI_BURST_MODE_ENABLE           = 1
} t_msp_spi_burst_mode;

/* Protocol Descriptor
* Contains parameters for configuring various protocol
------------------------------------------------------------------*/
typedef struct
{
    t_msp_data_transfer_width   rx_data_transfer_width;
    t_msp_data_transfer_width   tx_data_transfer_width;
    t_msp_phase_mode            rx_phase_mode;
    t_msp_phase_mode            tx_phase_mode;
    t_msp_phase2_start_mode     rx_phase2_start_mode;
    t_msp_phase2_start_mode     tx_phase2_start_mode;
    t_msp_bit_transfer_format   rx_bit_transfer_format;
    t_msp_bit_transfer_format   tx_bit_transfer_format;
    t_msp_frame_length          rx_frame_length_1;
    t_msp_frame_length          rx_frame_length_2;
    t_msp_frame_length          tx_frame_length_1;
    t_msp_frame_length          tx_frame_length_2;
    t_msp_element_length        rx_element_length_1;
    t_msp_element_length        rx_element_length_2;
    t_msp_element_length        tx_element_length_1;
    t_msp_element_length        tx_element_length_2;
    t_msp_data_delay            rx_data_delay;
    t_msp_data_delay            tx_data_delay;
    t_msp_clock_edge            rx_clock_pol;
    t_msp_clock_edge            tx_clock_pol;
    t_msp_frame_sync_pol        rx_frame_sync_pol;
    t_msp_frame_sync_pol        tx_frame_sync_pol;
    t_msp_half_word_swap        rx_half_word_swap;
    t_msp_half_word_swap        tx_half_word_swap;
    t_msp_compress_mode         compression_mode;
    t_msp_expand_mode           expansion_mode;
    t_msp_spi_clock_modes       spi_clk_mode;
    t_msp_spi_burst_mode        spi_burst_mode;
    t_uint16                    frame_period;
    t_uint16                    frame_width;
    t_uint16                    total_clocks_for_one_frame;
} t_msp_protocol_descriptor;

/* Clock selection
* Clock may be an input (external) or an ouput (sample rate generator).
* Loopback mode on the receiver brings some specificities.
-------------------------------------------------------------------------*/
typedef enum
{
    MSP_CLOCK_SEL_EXT                   = 0,
    MSP_CLOCK_SEL_SRG                   = 1
} t_msp_clock_sel;

/*Unexpected frame sync treatment
-----------------------------------*/
typedef enum
{
    MSP_UNEXPEC_FRAME_SYNC_ABORT        = 0,
    MSP_UNEXPEC_FRAME_SYNC_IGNORED      = 1
} t_msp_unexpect_frame_sync;

/* Transmit frame sync selection
* Three possibilities: from an external signal, created by the sample rate generator
* or synchronised on an internal copy.
------------------------------------------------------------------------------------*/
typedef enum
{
    MSP_TX_FRAME_SYNC_EXT               = 0,
    MSP_TX_FRAME_SYNC_SRG_AUTO          = 2,
    MSP_TX_FRAME_SYNC_SRG_PROG          = 3
} t_msp_tx_frame_sync_sel;

/* Receive frame sync selection
* Two possibilities: from an external signal or created by the sample rate generator.
------------------------------------------------------------------------------------*/
typedef enum
{
    MSP_RX_FRAME_SYNC_EXT               = 0,
    MSP_RX_FRAME_SYNC_SRG               = 1
} t_msp_rx_frame_sync_sel;

/* Sample rate generator clock configuration
* Can be derived from the APB clock, derived from the SCK clock or derived from the
* SCK clock and synchronised on RFS transitions.
-----------------------------------------------------------------------------------*/
typedef enum
{
    MSP_SRG_IN_CLOCK_APB                = 0,
    MSP_SRG_IN_CLOCK_SCK                = 2,
    MSP_SRG_IN_CLOCK_SCK_SYNC           = 3
} t_msp_srg_in_clock_sel;

/* FIFO configuration
------------------------*/
typedef enum
{
    MSP_FIFO_DISABLE                    = 0,
    MSP_FIFO_ENABLE                     = 1
} t_msp_fifo_config;

/*Sample Rate Generator Clock Polarity*/
typedef enum
{
    MSP_SCK_POL_AT_RISING_EDGE          = 0,
    MSP_SCK_POL_AT_FALLING_EDGE         = 1
} t_msp_sck_pol;

/* Co-processor modes
-------------------------*/
typedef enum
{
    MSP_DIRECT_COMPANDING_MODE_DISABLE  = 0,
    MSP_DIRECT_COMPANDING_MODE_ENABLE   = 1
} t_msp_direct_companding_mode;

/* Loop back mode
-------------------------*/
typedef enum
{
    MSP_LOOPBACK_MODE_DISABLE           = 0,
    MSP_LOOPBACK_MODE_ENABLE            = 1
} t_msp_loopback_mode;

/* The extra delay on MSPTXD output enable
-------------------------*/
typedef enum
{
    MSP_TX_EXTRA_DELAY_OFF              = 0,
    MSP_TX_EXTRA_DELAY_ON               = 1
} t_msp_tx_extra_delay;

/* General  configuration data struct
---------------------------------------------------------------*/
typedef struct
{
    t_msp_srg_in_clock_sel          srg_clock_sel;
    t_msp_sck_pol                   sck_pol;
    t_msp_loopback_mode             msp_loopback_mode;
    t_msp_direct_companding_mode    msp_direct_companding_mode;
    t_msp_clock_sel                 rx_clock_sel;
    t_msp_clock_sel                 tx_clock_sel;
    t_msp_mode                      rx_msp_mode;
    t_msp_mode                      tx_msp_mode;
    t_msp_rx_frame_sync_sel         rx_frame_sync_sel;
    t_msp_tx_frame_sync_sel         tx_frame_sync_sel;
    t_msp_unexpect_frame_sync       rx_unexpect_frame_sync;
    t_msp_unexpect_frame_sync       tx_unexpect_frame_sync;
    t_msp_fifo_config               rx_fifo_config;
    t_msp_fifo_config               tx_fifo_config;
    t_msp_tx_extra_delay            tx_extra_delay;
} t_msp_general_config;

/* Multichannel mode
---------------------------*/
typedef enum
{
    MSP_MULTICHANNEL_DISABLED           = 0,
    MSP_MULTICHANNEL_ENABLED            = 1
} t_msp_multichannel_enable;

/* Multichannel comparison mode description
* The data compared can be accepted if the
* bit-to-bit comparison is true or false.
* Works for receive mode only.
------------------------------------------*/
typedef enum
{
    MSP_COMPARISON_DISABLED             = 0,
    MSP_COMPARISON_NONEQUAL_ENABLED     = 2,
    MSP_COMPARISON_EQUAL_ENABLED        = 3
} t_msp_rx_comparison_enable_mode;

/* Transmit/Receive FIFO status
---------------------------------*/
typedef enum
{
    MSP_FIFO_FULL,
    MSP_FIFO_PARTIALLY_FILLED,
    MSP_FIFO_EMPTY
} t_msp_fifo_status;

#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
/* Transmit/Receive FIFO Watermark Level
------------------------------------------*/
typedef enum
{
	MSP_TWO_ELEMENTS  		= 0,
	MSP_FOUR_ELEMENTS 		= 1,
	MSP_EIGHT_ELEMENTS 		= 2,
	MSP_SIXTEEN_ELEMENTS	= 3
} t_msp_wmrk_level;
#endif

/* Data struct for multichannel comparison      */

/* Each bit within the t_uint32 corresponds to  */

/* an element from a frame.						*/

/*----------------------------------------------*/
typedef struct
{
    t_msp_multichannel_enable       rx_multichannel_enable;
    t_msp_multichannel_enable       tx_multichannel_enable;
    t_msp_rx_comparison_enable_mode rx_comparison_enable_mode;
    t_uint8                         padding;
    t_uint32                        comparison_value;
    t_uint32                        comparison_mask;
    t_uint32                        rx_channel_0_enable;
    t_uint32                        rx_channel_1_enable;
    t_uint32                        rx_channel_2_enable;
    t_uint32                        rx_channel_3_enable;
    t_uint32                        tx_channel_0_enable;
    t_uint32                        tx_channel_1_enable;
    t_uint32                        tx_channel_2_enable;
    t_uint32                        tx_channel_3_enable;
} t_msp_multichannel_config;

/* Multi-channel subframe id*/
typedef enum
{
    MSP_MULTICHANNEL_SUBFRAME_ID_0      = 0,
    MSP_MULTICHANNEL_SUBFRAME_ID_1      = 1,
    MSP_MULTICHANNEL_SUBFRAME_ID_2      = 2,
    MSP_MULTICHANNEL_SUBFRAME_ID_3      = 3
} t_msp_multichannel_subframe_id;

/*--------------------------------------------------------------------------*/
/* Functions declaration													*/
/*--------------------------------------------------------------------------*/
PUBLIC t_msp_error  MSP_Init(IN t_msp_device_id msp_device_id, IN t_logical_address msp_base_address);
PUBLIC t_msp_error  MSP_Configure(IN t_msp_device_id msp_device_id, IN t_msp_general_config const *const p_msp_general_config, IN t_msp_protocol_descriptor const *const p_msp_protocol_descriptor);
PUBLIC t_msp_error  MSP_ConfigureMultiChannel(IN t_msp_device_id msp_device_id, IN t_msp_multichannel_config const *const p_multichannel_config);
PUBLIC t_msp_error  MSP_SetSampleFrequency(IN t_msp_device_id msp_device_id, IN t_uint32 msp_in_clock_frequency, IN t_uint32 sample_frequency, OUT t_uint32 *p_effective_sample_frequency);
PUBLIC t_msp_error  MSP_SetSerialClockdevisor(IN t_msp_device_id msp_device_id, IN t_uint32 msp_in_clock_frequency, IN t_uint32 serial_clock_devisor);

PUBLIC t_msp_error  MSP_Enable(IN t_msp_device_id msp_device_id, IN t_msp_direction msp_direction);
PUBLIC t_msp_error  MSP_Disable(IN t_msp_device_id msp_device_id, IN t_msp_direction direction);

PUBLIC t_msp_error  MSP_SetDbgLevel(IN t_dbg_level dbg_level);
PUBLIC t_msp_error  MSP_GetDbgLevel(OUT t_dbg_level *p_dbg_level);

/* Subframe id query in case of multichannel */
PUBLIC t_msp_error  MSP_GetTransmitSubframeId(IN t_msp_device_id msp_device_id, OUT t_msp_multichannel_subframe_id *p_msp_multichannel_subframe_id);
PUBLIC t_msp_error  MSP_GetReceiveSubframeId(IN t_msp_device_id msp_device_id, OUT t_msp_multichannel_subframe_id *p_msp_multichannel_subframe_id);

PUBLIC t_msp_error  MSP_GetTransmissionStatus
                    (
                        IN t_msp_device_id  msp_device_id,
                        OUT t_uint32        *p_underrun_error_count,
                        OUT t_uint32        *p_bytes_pending_to_transmitted,
                        OUT t_bool          *p_is_transmition_completed
                    );
PUBLIC t_msp_error  MSP_GetReceptionStatus(IN t_msp_device_id msp_device_id, OUT t_uint32 *p_overrun_error_count, OUT t_uint32 *p_bytes_pending_to_be_received, OUT t_bool *p_is_reception_completed);
PUBLIC t_msp_error  MSP_GetRxFIFOStatus(IN t_msp_device_id msp_device_id, OUT t_msp_fifo_status *p_msp_fifo_status, OUT t_bool *p_rx_busy_flag);
PUBLIC t_msp_error  MSP_GetTxFIFOStatus(IN t_msp_device_id msp_device_id, OUT t_msp_fifo_status *p_msp_fifo_status, OUT t_bool *p_tx_busy_flag);

PUBLIC t_msp_error  MSP_TransmitData(IN t_msp_device_id msp_device_id, IN t_logical_address data_origin_address, IN t_uint32 bytes_to_be_transfered);
PUBLIC t_msp_error  MSP_ReceiveData(IN t_msp_device_id msp_device_id, IN t_logical_address data_dest_address, IN t_uint32 bytes_to_be_retrieved);

PUBLIC t_msp_error  MSP_WriteBuffer(IN t_msp_device_id msp_device_id, IN t_uint8 *p_data_src_address, INOUT t_uint32 *p_bytes_count);
PUBLIC t_msp_error  MSP_ReadBuffer(IN t_msp_device_id msp_device_id, IN t_uint8 *p_data_dest_address, INOUT t_uint32 *p_bytes_count);

PUBLIC void         MSP_GetIRQSrcStatus(IN t_msp_irq_src msp_irq_src, OUT t_msp_irq_status *p_status_structure);
PUBLIC t_bool       MSP_IsIRQSrcActive(IN t_msp_irq_src msp_irq_src, OUT t_msp_irq_status *p_status_structure);

PUBLIC t_msp_error  MSP_FilterProcessIRQSrc(IN t_msp_irq_status *p_status_structure, OUT t_msp_event *p_event_id, IN t_msp_filter_mode filter_mode);
PUBLIC t_msp_error  MSP_ProcessIRQSrc(IN t_msp_irq_status *p_status_structure);
PUBLIC t_bool       MSP_IsEventActive(IN const t_msp_event *p_event_id);
PUBLIC void         MSP_AcknowledgeEvent(IN const t_msp_event *p_event_id);

/*FIFO management
-----------------------*/
PUBLIC t_msp_error  MSP_Reset(IN t_msp_device_id msp_device_id);
PUBLIC t_msp_error  MSP_EmptyRxFIFO(IN t_msp_device_id msp_device_id);
PUBLIC t_msp_error  MSP_EmptyTxFIFO(IN t_msp_device_id msp_device_id);
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
PUBLIC t_msp_error MSP_SetTxFifoWmrkLevel(IN t_msp_device_id msp_device_id, IN t_msp_wmrk_level msp_wmrk_level);
PUBLIC t_msp_error MSP_SetRxFifoWmrkLevel(IN t_msp_device_id msp_device_id, IN t_msp_wmrk_level msp_wmrk_level);
#endif

/*Context Save Restore Management*/
PUBLIC t_msp_error  MSP_SaveDeviceContext(IN t_msp_device_id msp_device_id);
PUBLIC t_msp_error  MSP_RestoreDeviceContext(IN t_msp_device_id msp_device_id);

/*Version Management*/
PUBLIC t_msp_error  MSP_GetVersion(OUT t_version *p_version);

/*APIs to be implemented by user*/
PUBLIC t_msp_error  MSP_Delay(IN t_msp_device_id msp_device_id, IN t_uint32 delay);

#ifdef __cplusplus
}   /* allow C++ to use these headers*/
#endif /* __cplusplus*/
#endif /* _MSP_H_*/

/*End of file - msp.h*/

