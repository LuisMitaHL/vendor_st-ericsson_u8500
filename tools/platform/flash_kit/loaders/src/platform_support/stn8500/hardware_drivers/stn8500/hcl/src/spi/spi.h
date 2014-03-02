/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of Serial Periphiral Interface
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __INC_SPI_H
#define __INC_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "debug.h"
#include "spi_irq.h"

/*Defines for Version */
#define SPI_HCL_VERSION_ID  2
#define SPI_HCL_MAJOR_ID    3
#define SPI_HCL_MINOR_ID    2

#define SPI_ENABLE          0x01
#define SPI_DISABLE         0x00

#define SPI_CLOCK_INCREMENT 0x01
#define SPI_LOW             0x00
#define SPI_INCREMENT       0x01
#define SPI_NO_FILTER_MODE  0x00
#define SPI_INTERRUPT_ALL   0x0F

#define SPI_ERROR_OVERRUN   0x01
#define SPI_ERROR_TIMEOUT   0x02

#ifndef ST_8500_ED
#define SPI_MAX_NUMBER_SLAVES 0x03
#endif

typedef t_uint32  t_spi_filter_mode;
typedef t_uint8   t_watermark_level;
/*------------------------------------------------------------------------
 * Structures and enums
 *----------------------------------------------------------------------*/
/*	Define SPI errors	*/
typedef enum
{
    SPI_NON_AUTHORIZED_MODE          = HCL_MAX_ERROR_VALUE - 2,
    SPI_TRANSMISSION_ON_GOING        = HCL_MAX_ERROR_VALUE,
    SPI_TRANSMIT_ERROR               = -10,
    SPI_RECEIVE_ERROR                = -9,
    SPI_INTERNAL_ERROR               = HCL_INTERNAL_ERROR,
    SPI_INVALID_PARAMETER            = HCL_INVALID_PARAMETER,
    SPI_REQUEST_NOT_APPLICABLE       = HCL_REQUEST_NOT_APPLICABLE,
    SPI_UNSUPPORTED_FEATURE          = HCL_UNSUPPORTED_FEATURE,
    SPI_UNSUPPORTED_HW               = HCL_UNSUPPORTED_HW,
    SPI_ERROR                        = HCL_ERROR,
    SPI_OK                           = HCL_OK,   
    SPI_REMAINING_PENDING_EVENTS     = HCL_REMAINING_PENDING_EVENTS,
    SPI_INTERNAL_EVENT               = HCL_INTERNAL_EVENT,                   
    SPI_NO_MORE_PENDING_EVENT        = HCL_NO_MORE_PENDING_EVENT,
    SPI_NO_MORE_FILTER_PENDING_EVENT = HCL_NO_MORE_FILTER_PENDING_EVENT,
    SPI_NO_PENDING_EVENT_ERROR       = HCL_NO_PENDING_EVENT_ERROR,
    
} t_spi_error;

/* enabling Rx or Tx of SPI */
typedef enum
{
    SPI_ENABLE_RX_TX            = 0x00, /* Both RX and TX are enabled. */
    SPI_ENABLE_RX_DISABLE_TX    = 0x01, /* RX enabled, and TX disabled. */
    SPI_DISABLE_RX_ENABLE_TX    = 0x02, /* RX disabled, and TX enabled */
    SPI_DISABLE_RX_TX           = 0x03, /* both RX and TX disabled. */
} t_spi_enable;


/* master or slave configuration */
typedef enum
{
    SPI_MASTER,                         /* MuPoC SPI is master (provides the clock) */
    SPI_SLAVE                           /* MuPoC SPI is slave (receives the clock) */
} t_spi_hierarchy;

/* Clock parameters */
typedef struct
{
    t_uint8 cpsdvsr;                    /* value from 2 to 254 (even only!) */
    t_uint8 scr;                        /* value from 0 to 255 */
} t_spi_clock_params;

/* Endianness */
typedef enum
{
    SPI_RX_MSB_TX_MSB,                  /* receive: MSBit first & transmit: MSBit first */
    SPI_RX_MSB_TX_LSB,                  /* receive: MSBit first & transmit: LSBit first */
    SPI_RX_LSB_TX_MSB,                  /* receive: LSBit first & transmit: MSBit first */
    SPI_RX_LSB_TX_LSB                   /* receive: LSBit first & transmit: LSBit first */
} t_spi_endian;

#ifndef ST_8500_ED
typedef enum
{
	SPI_SLAVE_DEVICE_1     = MASK_BIT0,                 /* Select the Slave Device 1 */
	SPI_SLAVE_DEVICE_2     = MASK_BIT1,                 /* Select the Slave Device 2 */
	SPI_SLAVE_DEVICE_3     = MASK_BIT2,                 /* Select the Slave Device 3 */
}t_spi_slave_device;


typedef enum
{
	SPI_FBCLKDEL_NO_DELAY            = 0x00,
	SPI_FBCLKDEL_01_DELAY            = 0x01,
	SPI_FBCLKDEL_02_DELAY            = 0x02,
	SPI_FBCLKDEL_03_DELAY            = 0x03,
	SPI_FBCLKDEL_04_DELAY            = 0x04,
	SPI_FBCLKDEL_05_DELAY            = 0x05,
	SPI_FBCLKDEL_06_DELAY            = 0x06,
	SPI_FBCLKDEL_07_DELAY            = 0x07
}t_spi_fbclkdel;

#endif
/*Number of bits in one data element */
typedef enum
{
    SPI_DATA_BITS_4                 = 0x03,
    SPI_DATA_BITS_5,
    SPI_DATA_BITS_6,
    SPI_DATA_BITS_7,
    SPI_DATA_BITS_8,
    SPI_DATA_BITS_9,
    SPI_DATA_BITS_10,
    SPI_DATA_BITS_11,
    SPI_DATA_BITS_12,
    SPI_DATA_BITS_13,
    SPI_DATA_BITS_14,
    SPI_DATA_BITS_15,
    SPI_DATA_BITS_16,
    SPI_DATA_BITS_17,
    SPI_DATA_BITS_18,
    SPI_DATA_BITS_19,
    SPI_DATA_BITS_20,
    SPI_DATA_BITS_21,
    SPI_DATA_BITS_22,
    SPI_DATA_BITS_23,
    SPI_DATA_BITS_24,
    SPI_DATA_BITS_25,
    SPI_DATA_BITS_26,
    SPI_DATA_BITS_27,
    SPI_DATA_BITS_28,
    SPI_DATA_BITS_29,
    SPI_DATA_BITS_30,
    SPI_DATA_BITS_31,
    SPI_DATA_BITS_32
} t_spi_data_size;

/* Receive FIFO watermark level which triggers IT */
typedef enum
{
    SPI_RX_1_OR_MORE_ELEM,              /* IT fires when 1 or more elements in RX FIFO */
    SPI_RX_4_OR_MORE_ELEM,              /* IT fires when 4 or more elements in RX FIFO */
    SPI_RX_8_OR_MORE_ELEM,              /* IT fires when 8 or more elements in RX FIFO */
    SPI_RX_16_OR_MORE_ELEM,             /* IT fires when 16 or more elements in RX FIFO */
    SPI_RX_32_OR_MORE_ELEM              /* IT fires when 32 or more elements in RX FIFO */
} t_spi_rx_level_trig;

/* Transmit FIFO watermark level which triggers IT */
typedef enum
{
    SPI_TX_1_OR_MORE_EMPTY_LOC,         /* IT fires when 1 or more empty locations in TX FIFO */
    SPI_TX_4_OR_MORE_EMPTY_LOC,         /* IT fires when 4 or more empty locations in TX FIFO */
    SPI_TX_8_OR_MORE_EMPTY_LOC,         /* IT fires when 8 or more empty locations in TX FIFO */
    SPI_TX_16_OR_MORE_EMPTY_LOC,        /* IT fires when 16 or more empty locations in TX FIFO */
    SPI_TX_32_OR_MORE_EMPTY_LOC         /* IT fires when 32 or more empty locations in TX FIFO */
} t_spi_tx_level_trig;

/* clock phase (Motorola SPI interface only) */
typedef enum
{
    SPI_CLK_FALLING_EDGE,               /* Receive data on falling edge. */
    SPI_CLK_RISING_EDGE,                /* Receive data on rising edge. */
} t_spi_spi_clk_phase;

/* clock polarity (Motorola SPI interface only) */
typedef enum
{
    SPI_CLK_POL_IDLE_LOW,               /* Low inactive level */
    SPI_CLK_POL_IDLE_HIGH               /* High inactive level */
} t_spi_spi_clk_pol;


/* communication mode */
typedef enum
{
    SPI_POLLING_MODE,                   /* Polling mode */
    SPI_IT_MODE,                        /* Interrupt mode */
    SPI_DMA_MODE                        /* DMA mode */
} t_spi_mode;

/* SPI FIFO status */
typedef enum
{
    SPI_TRANSMIT_FIFO_EMPTY             = 0x01,
    SPI_TRANSMIT_FIFO_NOT_FULL          = 0x02,
    SPI_RECEIVE_FIFO_NOT_EMPTY          = 0x04,
    SPI_RECEIVE_FIFO_FULL               = 0x08,
    SPI_BUSY                            = 0x10
} t_spi_fifo_status;

/* The Interrupt State */
typedef enum
{
    SPI_IRQ_STATE_NEW                   = 0x0,
    SPI_IRQ_STATE_OLD                   = 0x1,
} t_spi_irq_state;

typedef enum
{
	SPI_DATA_SIZE_BYTE                  = 0x01,
	SPI_DATA_SIZE_HALFWORD              = 0x02,
	SPI_DATA_SIZE_WORD                  = 0x04 
}t_spi_transfer_data_size;

/* Stores the interrupt status */
typedef struct
{
    t_spi_irq_state irq_state;
    t_spi_irq_src   initial_irq;
    t_spi_irq_src   pending_irq;
} t_spi_irq_status;

typedef t_uint32 t_spi_event;

typedef struct
{
    volatile t_uint8   *p_it_data_flow;  /* Pointer to the address of the current data flow in interrupt mode */
    volatile t_uint32   it_element_num;   /* Number of elements to be transfered */  
    volatile t_uint32   flow_error_nb;    /* Time Out Interrupt and Reciever Over run interrupt */
}t_spi_status;

/* Configuration parameters */
typedef struct
{
    t_spi_hierarchy             hierarchy;          /* sets whether interface is master or slave   */
    t_bool                      slave_tx_disable;   /* SPITXD is disconnected (in slave mode only) */
    t_spi_clock_params          clk_freq;           /* Freq. of SPI interface (when master)        */
    t_spi_endian                endian;             /* sets whether MSBit or LSBit is first        */
    t_spi_data_size             data_size;          /* size of data elements (4 to 32 bits)        */
    t_spi_mode                  txcom_mode;         /* tx communication mode : polling, IT or DMA  */
    t_spi_mode                  rxcom_mode;         /* rx communication mode : polling, IT or DMA  */
    t_spi_rx_level_trig         rx_lev_trig;        /* Rx FIFO watermark level (for IT & DMA mode) */
    t_spi_tx_level_trig         tx_lev_trig;        /* Tx FIFO watermark level (for IT & DMA mode) */
    t_spi_spi_clk_phase         clk_phase;          /* SPI interface Clock phase                   */
    t_spi_spi_clk_pol           clk_pol;            /* SPI interface Clock polarity                */
 } t_spi_config;

/*------------------------------------------------------------------------
 * Functions declaration
 *----------------------------------------------------------------------*/
PUBLIC void         SPI_SetBaseAddress(t_spi_device_id, t_logical_address);
PUBLIC t_spi_error  SPI_Init(t_spi_device_id, t_logical_address spi_base_address);
PUBLIC t_spi_error  SPI_SetDbgLevel(t_dbg_level);
PUBLIC t_spi_error  SPI_GetVersion(t_version *);
PUBLIC t_spi_error  SPI_Enable(t_spi_device_id, t_spi_enable);
PUBLIC void         SPI_Reset(t_spi_device_id);
PUBLIC t_spi_error  SPI_ResolveClockFrequency(t_uint32, t_uint32, t_uint32 *, t_spi_clock_params *);
PUBLIC t_spi_error  SPI_SetConfiguration(t_spi_device_id, t_spi_config *);
PUBLIC t_spi_error  SPI_CheckConfiguration(t_spi_config *);
PUBLIC t_spi_error SPI_SetWatermarkLevel(t_spi_device_id,t_spi_tx_level_trig,t_spi_rx_level_trig);
PUBLIC t_spi_error  SPI_SetData(t_spi_device_id, t_uint8 *);
PUBLIC t_spi_error  SPI_GetData(t_spi_device_id, t_uint8 *);
PUBLIC t_spi_error  SPI_GetIRQSrcStatus(IN t_spi_device_id,IN t_spi_irq_src,OUT t_spi_irq_status *);
PUBLIC t_spi_error  SPI_GetFIFOStatus(t_spi_device_id, t_uint32 *);
PUBLIC t_spi_error  SPI_FlushFifo(t_spi_device_id);
PUBLIC t_spi_error  SPI_TransmitData(IN t_spi_device_id,IN t_uint8 *,IN t_uint32,IN t_spi_config *);
PUBLIC t_spi_error  SPI_ReceiveData(IN t_spi_device_id,IN t_uint8 *,IN t_uint32,IN t_spi_config *);
PUBLIC t_spi_error  SPI_FilterProcessIRQSrc(IN t_spi_device_id,IN t_spi_irq_status *,OUT t_spi_event *,IN t_spi_filter_mode);
PUBLIC void         SPI_SaveDeviceContext(t_spi_device_id);
PUBLIC void         SPI_RestoreDeviceContext(t_spi_device_id);
PUBLIC t_bool SPI_IsIRQSrcActive(IN t_spi_device_id,IN t_spi_irq_src,OUT t_spi_irq_status *);
PUBLIC void SPI_AcknowledgeEvent(IN t_spi_device_id,IN const t_spi_event *);
#ifndef ST_8500_ED
PUBLIC t_spi_error SPI_SlaveSelect(IN t_spi_device_id,IN t_uint8);
PUBLIC t_spi_error SPI_DelayFeedbackClock(IN t_spi_device_id,IN t_spi_fbclkdel);
#endif
#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_SPI_H */

// End of file - spi.h

