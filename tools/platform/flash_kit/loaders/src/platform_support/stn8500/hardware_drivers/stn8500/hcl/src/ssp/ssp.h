 /************************************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of Synchronous Serial Port
* \author  ST-Ericsson
*/
/************************************************************************************************/

#ifndef __INC_SSP_H
#define __INC_SSP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "debug.h"

/*Defines for Version */
#define SSP_HCL_VERSION_ID  1
#define SSP_HCL_MAJOR_ID    0
#define SSP_HCL_MINOR_ID    8

/*------------------------------------------------------------------------
 * Structures and enums
 *----------------------------------------------------------------------*/

/* Enum to identify SSP device blocks */
typedef enum
{
    SSP_DEVICE_ID_0,
    SSP_DEVICE_ID_1,
    SSP_DEVICE_ID_INVALID       = 0x2
} t_ssp_device_id;

/*	Define SSP errors	*/
typedef enum
{
    SSP_OK                      = HCL_OK,
    SSP_UNSUPPORTED_HW          = HCL_UNSUPPORTED_HW,
    SSP_INVALID_PARAMETER       = HCL_INVALID_PARAMETER,
    SSP_UNSUPPORTED_FEATURE     = HCL_UNSUPPORTED_FEATURE,
    SSP_REQUEST_NOT_APPLICABLE  = HCL_REQUEST_NOT_APPLICABLE,
    SSP_INTERNAL_ERROR          = HCL_INTERNAL_ERROR,
    SSP_RECEIVE_ERROR           = -9,
    SSP_NON_AUTHORIZED_MODE     = -10
} t_ssp_error;

/* enabling Rx or Tx of SSP */
typedef enum
{
    SSP_ENABLE_RX_TX,                   /* Both RX and TX are enabled. */
    SSP_ENABLE_RX_DISABLE_TX,           /* RX enabled, and TX disabled. */
    SSP_DISABLE_RX_ENABLE_TX,           /* RX disabled, and TX enabled */
    SSP_DISABLE_RX_TX                   /* both RX and TX disabled. */
} t_ssp_enable;

/* Interface Type */
typedef enum
{
    SSP_INTERFACE_MOTOROLA_SPI,         /* Motorola Interface */
    SSP_INTERFACE_TI_SYNC_SERIAL,       /* Texas Instrument Synchronous Serial interface */
    SSP_INTERFACE_NATIONAL_MICROWIRE,   /* National Semiconductor Microwire interface */
    SSP_INTERFACE_UNIDIRECTIONAL        /* Unidirectional interface */
} t_ssp_interface;

/* master or slave configuration */
typedef enum
{
    SSP_MASTER,                         /* MuPoC SSP is master (provides the clock) */
    SSP_SLAVE                           /* MuPoC SSP is slave (receives the clock) */
} t_ssp_hierarchy;

/* Clock parameters */
typedef struct
{
    t_uint8 cpsdvsr;                    /* value from 2 to 254 (even only!) */
    t_uint8 scr;                        /* value from 0 to 255 */
} t_ssp_clock_params;

/* Endianness */
typedef enum
{
    SSP_RX_MSB_TX_MSB,                  /* receive: MSBit first & transmit: MSBit first */
    SSP_RX_MSB_TX_LSB,                  /* receive: MSBit first & transmit: LSBit first */
    SSP_RX_LSB_TX_MSB,                  /* receive: LSBit first & transmit: MSBit first */
    SSP_RX_LSB_TX_LSB                   /* receive: LSBit first & transmit: LSBit first */
} t_ssp_endian;

/*Number of bits in one data element */
typedef enum
{
    SSP_DATA_RESERVED_0 = 0x00,
    SSP_DATA_RESERVED_1 = 0x01,
    SSP_DATA_RESERVED_2 = 0x02,
    SSP_DATA_BITS_4     = 0x03,
    SSP_DATA_BITS_5     = 0x04,
    SSP_DATA_BITS_6     = 0x05,
    SSP_DATA_BITS_7     = 0x06,
    SSP_DATA_BITS_8     = 0x07,
    SSP_DATA_BITS_9     = 0x08,
    SSP_DATA_BITS_10    = 0x09,
    SSP_DATA_BITS_11    = 0x0A,
    SSP_DATA_BITS_12    = 0x0B,
    SSP_DATA_BITS_13    = 0x0C,
    SSP_DATA_BITS_14    = 0x0D,
    SSP_DATA_BITS_15    = 0x0E,
    SSP_DATA_BITS_16    = 0x0F,
    SSP_DATA_BITS_17    = 0x10,
    SSP_DATA_BITS_18    = 0x11,
    SSP_DATA_BITS_19    = 0x12,
    SSP_DATA_BITS_20    = 0x13,
    SSP_DATA_BITS_21    = 0x14,
    SSP_DATA_BITS_22    = 0x15,
    SSP_DATA_BITS_23    = 0x16,
    SSP_DATA_BITS_24    = 0x17,
    SSP_DATA_BITS_25    = 0x18,
    SSP_DATA_BITS_26    = 0x19,
    SSP_DATA_BITS_27    = 0x1A,
    SSP_DATA_BITS_28    = 0x1B,
    SSP_DATA_BITS_29    = 0x1C,
    SSP_DATA_BITS_30    = 0x1D,
    SSP_DATA_BITS_31    = 0x1E,
    SSP_DATA_BITS_32    = 0x1F
} t_ssp_data_size;

/* Receive FIFO watermark level which triggers IT */
typedef enum
{
    SSP_RX_1_OR_MORE_ELEM,              /* IT fires when 1 or more elements in RX FIFO */
    SSP_RX_4_OR_MORE_ELEM,              /* IT fires when 4 or more elements in RX FIFO */
    SSP_RX_8_OR_MORE_ELEM,              /* IT fires when 8 or more elements in RX FIFO */
    SSP_RX_16_OR_MORE_ELEM,             /* IT fires when 16 or more elements in RX FIFO */
    SSP_RX_32_OR_MORE_ELEM              /* IT fires when 32 or more elements in RX FIFO */
} t_ssp_rx_level_trig;

/* Transmit FIFO watermark level which triggers IT */
typedef enum
{
    SSP_TX_1_OR_MORE_EMPTY_LOC,         /* IT fires when 1 or more empty locations in TX FIFO */
    SSP_TX_4_OR_MORE_EMPTY_LOC,         /* IT fires when 4 or more empty locations in TX FIFO */
    SSP_TX_8_OR_MORE_EMPTY_LOC,         /* IT fires when 8 or more empty locations in TX FIFO */
    SSP_TX_16_OR_MORE_EMPTY_LOC,        /* IT fires when 16 or more empty locations in TX FIFO */
    SSP_TX_32_OR_MORE_EMPTY_LOC         /* IT fires when 32 or more empty locations in TX FIFO */
} t_ssp_tx_level_trig;

/* clock phase (Motorola SPI interface only) */
typedef enum
{
    SSP_CLK_FALLING_EDGE,               /* Receive data on falling edge. */
    SSP_CLK_RISING_EDGE,                /* Receive data on rising edge. */
} t_ssp_spi_clk_phase;

/* clock polarity (Motorola SPI interface only) */
typedef enum
{
    SSP_CLK_POL_IDLE_LOW,               /* Low inactive level */
    SSP_CLK_POL_IDLE_HIGH               /* High inactive level */
} t_ssp_spi_clk_pol;

/* Command size in microwire format */
typedef enum
{
    SSP_RESERVED_0          = 0x00,
    SSP_RESERVED_1          = 0x01,
    SSP_RESERVED_2          = 0x02,
    SSP_BITS_4              = 0x03,
    SSP_BITS_5              = 0x04, 
    SSP_BITS_6              = 0x05,
    SSP_BITS_7              = 0x06,
    SSP_BITS_8              = 0x07,
    SSP_BITS_9              = 0x08,
    SSP_BITS_10             = 0x09,
    SSP_BITS_11             = 0x0A,
    SSP_BITS_12             = 0x0B,
    SSP_BITS_13             = 0x0C,
    SSP_BITS_14             = 0x0D,
    SSP_BITS_15             = 0x0E,
    SSP_BITS_16             = 0x0F,
    SSP_BITS_17             = 0x10,
    SSP_BITS_18             = 0x11,
    SSP_BITS_19             = 0x12,
    SSP_BITS_20             = 0x13,
    SSP_BITS_21             = 0x14,
    SSP_BITS_22             = 0x15,
    SSP_BITS_23             = 0x16,
    SSP_BITS_24             = 0x17,
    SSP_BITS_25             = 0x18,
    SSP_BITS_26             = 0x19,
    SSP_BITS_27             = 0x1A,
    SSP_BITS_28             = 0x1B,
    SSP_BITS_29             = 0x1C,
    SSP_BITS_30             = 0x1D,
    SSP_BITS_31             = 0x1E,
    SSP_BITS_32             = 0x1F
} t_ssp_microwire_ctrl_len;

/* wait state */
typedef enum
{
    SSP_MICROWIRE_WAIT_ZERO,            /* No wait state inserted after last command bit */
    SSP_MICROWIRE_WAIT_ONE              /* One wait state inserted after last command bit */
} t_ssp_microwire_wait_state;

/* Full/Half Duplex */
typedef enum
{
    SSP_MICROWIRE_CHANNEL_FULL_DUPLEX,  /* SSPTXD becomes bi-directional, SSPRXD not used*/
    SSP_MICROWIRE_CHANNEL_HALF_DUPLEX   /* SSPTXD is an output, SSPRXD is an input. */
} t_ssp_duplex;

/* communication mode */
typedef enum
{
    SSP_POLLING_MODE,                   /* Polling mode */
    SSP_IT_MODE,                        /* Interrupt mode */
    SSP_DMA_MODE,                       /* DMA mode */
    SSP_UNINITIALZED_MODE 
} t_ssp_mode;

/* SSP FIFO status */
typedef enum
{
    SSP_TRANSMIT_FIFO_EMPTY             = 0x01,
    SSP_TRANSMIT_FIFO_NOT_FULL          = 0x02,
    SSP_RECEIVE_FIFO_NOT_EMPTY          = 0x04,
    SSP_RECEIVE_FIFO_FULL               = 0x08,
    SSP_BUSY                            = 0x10
} t_ssp_fifo_status;


/* Configuration parameters */
typedef struct
{
    t_ssp_interface             iface;              /*Interface type*/
    t_ssp_hierarchy             hierarchy;          /*sets whether interface is master or slave*/
    t_bool                      slave_tx_disable;   /*SSPTXD is disconnected (in slave mode only)*/
    t_ssp_clock_params          clk_freq;           /*Freq. of SSP interface (when master)*/
    t_ssp_endian                endian;             /*sets whether MSBit or LSBit is first*/
    t_ssp_data_size             data_size;          /*size of data elements (4 to 32 bits)*/
    t_ssp_mode                  txcom_mode;         /*tx communication mode : polling, IT or DMA*/
    t_ssp_mode                  rxcom_mode;         /*rx communication mode : polling, IT or DMA*/
    t_ssp_rx_level_trig         rx_lev_trig;        /*Rx FIFO watermark level (for IT & DMA mode)*/
    t_ssp_tx_level_trig         tx_lev_trig;        /*Tx FIFO watermark level (for IT & DMA mode)*/
    t_ssp_spi_clk_phase         clk_phase;          /*Motorola SPI interface Clock phase*/
    t_ssp_spi_clk_pol           clk_pol;            /*Motorola SPI interface Clock polarity*/
    t_ssp_microwire_ctrl_len    ctrl_len;           /*Microwire interface: Control length*/
    t_ssp_microwire_wait_state  wait_state;         /*Microwire interface: Wait state*/
    t_ssp_duplex                duplex;             /*Microwire interface: Full/Half duplex*/
} t_ssp_config;

/*------------------------------------------------------------------------
 * Functions declaration
 *----------------------------------------------------------------------*/
PUBLIC void         SSP_SetBaseAddress(t_ssp_device_id, t_logical_address);
PUBLIC t_ssp_error  SSP_Init(t_ssp_device_id, t_logical_address ssp_base_address);
PUBLIC t_ssp_error  SSP_SetDbgLevel(t_dbg_level);
PUBLIC t_ssp_error  SSP_GetVersion(t_version *);
PUBLIC t_ssp_error  SSP_Enable(t_ssp_device_id, t_ssp_enable);
PUBLIC t_ssp_error  SSP_EnableLoopBackMode(t_ssp_device_id, t_bool);
PUBLIC void         SSP_Reset(t_ssp_device_id);
PUBLIC t_ssp_error  SSP_ResolveClockFrequency(t_uint32, t_uint32, t_uint32 *, t_ssp_clock_params *);
PUBLIC t_ssp_error  SSP_SetConfiguration(t_ssp_device_id, t_ssp_config *);
PUBLIC t_ssp_error  SSP_CheckConfiguration(t_ssp_config *);

PUBLIC t_ssp_error  SSP_SetData(t_ssp_device_id, t_uint32);
PUBLIC t_ssp_error  SSP_GetData(t_ssp_device_id, t_uint32 *);

PUBLIC t_ssp_error  SSP_GetFIFOStatus(t_ssp_device_id, t_uint32 *);

PUBLIC t_ssp_error SSP_GetTxMode(t_ssp_device_id,t_ssp_mode *);
PUBLIC t_ssp_error SSP_GetRxMode(t_ssp_device_id,t_ssp_mode *);

PUBLIC void         SSP_SaveDeviceContext(t_ssp_device_id);
PUBLIC void         SSP_RestoreDeviceContext(t_ssp_device_id);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_SSP_H */

// End of file - ssp.h

