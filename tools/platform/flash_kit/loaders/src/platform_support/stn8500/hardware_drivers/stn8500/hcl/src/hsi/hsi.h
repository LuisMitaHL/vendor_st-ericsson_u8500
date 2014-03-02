/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of High Speed Serial Interface (HSI) module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HSI_H_
#define _HSI_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*-----------------------------------------------------------------------------
 * Includes							                                   
 *---------------------------------------------------------------------------*/
#include "debug.h"
#include "hsi_irq.h"

/*------------------------------------------------------------------------------
	typedefs and exported defines.
------------------------------------------------------------------------------*/
#define HSI_MAX_CHANNEL_NUM  0x08
#define HSI_MAX_RX_TIME_OUT  0xFFFFFF            /*Receiver time out*/
#define HSI_MAX_RX_THRESHOLD 0x3F                /*Reciever threshold*/
#define HSI_MAX_TX_BRD       0xFFFFFF            /*Transmitter baud rate*/
#define HSI_MAX_TX_BURSTLEN  0xFFFFFF            /*Transmitter burst length*/
#define HSI_MAX_FRAMELEN     0x1F                /*Channel burstlen*/

#define HSI_EXCEP_ALL    0x0F

#define HSI_DEFAULT_PREAMBLE 0x00FFFFFF
#define HSI_MAX_FLUSHBITS    0x3F
/*------------------------------------------------------------------------------
	Interrupt related typedefs.
------------------------------------------------------------------------------*/
/*Type defs for the channel number*/
typedef t_uint32    t_hsi_channel_num;
typedef t_uint32    t_hsi_filter_mode;

#define HSI_NO_FILTER_MODE  0

/*------------------------------------------------------------------------------
	Enumerations 
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
	HSI Error Codes
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_OK                          = HCL_OK,
    HSI_UNSUPPORTED_HW              = HCL_UNSUPPORTED_HW,
    HSI_INVALID_PARAMETER           = HCL_INVALID_PARAMETER,
    HSI_INTERNAL_EVENT              = HCL_INTERNAL_EVENT,
    HSI_INTERNAL_ERROR              = HCL_INTERNAL_ERROR,
    HSI_NO_PENDING_EVENT_ERROR      = HCL_NO_PENDING_EVENT_ERROR,
    HSI_REMAINING_PENDING_EVENTS    = HCL_REMAINING_PENDING_EVENTS,
    HSI_NO_MORE_PENDING_EVENT       = HCL_NO_MORE_PENDING_EVENT,
    HSI_NO_MORE_FILTER_PENDING_EVENT= HCL_NO_MORE_FILTER_PENDING_EVENT,
    HSI_REQUEST_PENDING             = HCL_REQUEST_PENDING,
    HSI_REQUEST_NOT_APPLICABLE      = HCL_REQUEST_NOT_APPLICABLE,
    HSI_OPERATION_FAILED            = HCL_MAX_ERROR_VALUE - 1,
    HSI_INVALID_MODE                = HCL_MAX_ERROR_VALUE - 2
} t_hsi_error;

/*------------------------------------------------------------------------------
	HSI Fifo event enum
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_FIFO_EVENT_NONE             = 0x00,
    HSI_FIFO_EVENT_ID_TX0           = MASK_BIT0,
    HSI_FIFO_EVENT_ID_TX1           = MASK_BIT1,
    HSI_FIFO_EVENT_ID_TX2           = MASK_BIT2,
    HSI_FIFO_EVENT_ID_TX3           = MASK_BIT3,
    HSI_FIFO_EVENT_ID_TX4           = MASK_BIT4,
    HSI_FIFO_EVENT_ID_TX5           = MASK_BIT5,
    HSI_FIFO_EVENT_ID_TX6           = MASK_BIT6,
    HSI_FIFO_EVENT_ID_TX7           = MASK_BIT7,
    HSI_FIFO_EVENT_ID_RX0           = MASK_BIT8,
    HSI_FIFO_EVENT_ID_RX1           = MASK_BIT9,
    HSI_FIFO_EVENT_ID_RX2           = MASK_BIT10,
    HSI_FIFO_EVENT_ID_RX3           = MASK_BIT11,
    HSI_FIFO_EVENT_ID_RX4           = MASK_BIT12,
    HSI_FIFO_EVENT_ID_RX5           = MASK_BIT13,
    HSI_FIFO_EVENT_ID_RX6           = MASK_BIT14,
    HSI_FIFO_EVENT_ID_RX7           = MASK_BIT15
} t_hsi_fifo_event_id;

/*------------------------------------------------------------------------------
	HSI Exception events
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_EXCEP_EVENT_NONE           = 0x00,
    HSI_EXCEP_EVENT_RX_OVR0        = MASK_BIT0,
    HSI_EXCEP_EVENT_RX_OVR1        = MASK_BIT1,
    HSI_EXCEP_EVENT_RX_OVR2        = MASK_BIT2,
    HSI_EXCEP_EVENT_RX_OVR3        = MASK_BIT3,
    HSI_EXCEP_EVENT_RX_OVR4        = MASK_BIT4,
    HSI_EXCEP_EVENT_RX_OVR5        = MASK_BIT5,
    HSI_EXCEP_EVENT_RX_OVR6        = MASK_BIT6,
    HSI_EXCEP_EVENT_RX_OVR7        = MASK_BIT7,
    HSI_EXCEP_EVENT_RX_TIMEOUT     = MASK_BIT8,
    HSI_EXCEP_EVENT_RX_OVERRUN     = MASK_BIT9,
    HSI_EXCEP_EVENT_RX_BREAK       = MASK_BIT10,
    HSI_EXCEP_EVENT_RX_PARITY      = MASK_BIT11
} t_hsi_excep_event_id;


/*------------------------------------------------------------------------------
	HSI mode
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_MODE_POLLING,
    HSI_MODE_IT_CPU,
    HSI_MODE_DMA
} t_hsi_mode;

/*------------------------------------------------------------------------------
	HSI Number of channels
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_NOC_1                       = 0x01,
    HSI_NOC_2                       = 0x02,
    HSI_NOC_4                       = 0x04,
    HSI_NOC_8                       = 0x08
} t_hsi_noc;

/*------------------------------------------------------------------------------
	HSI TX or RX function mode
------------------------------------------------------------------------------*/

#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
typedef enum
{
    HSI_TX_MODE_SLEEP               = 0x00,
    HSI_TX_MODE_STREAM              = 0x01,
    HSI_TX_MODE_FRAME               = 0x02,
} t_hsi_tx_mode;

#else
typedef enum
{
    HSI_TX_MODE_INIT                = 0x00,
    HSI_TX_MODE_STREAM              = 0x01,
    HSI_TX_MODE_FRAME               = 0x02,
} t_hsi_tx_mode;

#endif

typedef enum
{
    HSI_RX_MODE_SLEEP               = 0x00,
    HSI_RX_MODE_STREAM              = 0x01,
    HSI_RX_MODE_FRAME               = 0x02,
    HSI_RX_MODE_PIPELINED           = 0x03,
    HSI_RX_MODE_FAILSAFE            = 0x04,
} t_hsi_rx_mode;

/*------------------------------------------------------------------------------
	HSI TX or RX Arbitation policy
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_TX_ARB_CHANNEL_PRIORITY_0           = 0x00,
    HSI_TX_ARB_CHANNEL_PRIORITY_1           = 0x01,
} t_hsi_tx_arb_policy;

/*------------------------------------------------------------------------------
	HSI TX or RX number
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_TX_NUM_0                    = 0,
    HSI_TX_NUM_1
} t_hsi_tx_num;

typedef enum
{
    HSI_RX_NUM_0                    = 0,
    HSI_RX_NUM_1
} t_hsi_rx_num;


typedef enum
{
	HSI_RX_DETECTOR_0               = 0x00,
	HSI_RX_DETECTOR_1               = 0x01
}t_hsi_rx_detector;

/*------------------------------------------------------------------------------
	HSI TX frame size in number of bytes
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_FRAME_SIZE_BYTE             = 0x07,
    HSI_FRAME_SIZE_HALF_WORD        = 0x0F,
    HSI_FRAME_SIZE_WORD             = 0x1F
} t_hsi_frame_size;


/*------------------------------------------------------------------------------
	HSI RX state machine status
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_RX_STATE_IDLE              = 0x00,
    HSI_RX_STATE_RECEIVING         = 0x01,
    HSI_RX_STATE_FLUSH             = 0x02,
    HSI_RX_STATE_SYNC              = 0x03,
} t_hsi_rx_state;

/*------------------------------------------------------------------------------
	HSI TX state machine status
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_TX_STATE_IDLE              = 0x00,
    HSI_TX_STATE_START             = 0x01,
    HSI_TX_STATE_TRANSMIT          = 0x02,
    HSI_TX_STATE_BREAK             = 0x03,
    HSI_TX_STATE_FLUSH             = 0x04,
    HSI_TX_STATE_HALT              = 0x05
} t_hsi_tx_state;


/*------------------------------------------------------------------------------
	HSI IRQ state
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_IRQ_STATE_NEW,
    HSI_IRQ_STATE_OLD
} t_hsi_irq_state;

/*------------------------------------------------------------------------------
	HSI FIFO status
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_FIFO_STATUS_EMPTY           = 0,
    HSI_FIFO_STATUS_PARTIAL,
    HSI_FIFO_STATUS_FULL
} t_hsi_fifo_status;

/*------------------------------------------------------------------------------
	HSI channel transfer status
------------------------------------------------------------------------------*/
typedef enum
{
    HSI_CHANNEL_STATUS_NO_TRANSFER      = 0x0,  /*Channel is free to start transfer through interrupts*/
    HSI_CHANNEL_STATUS_TRANSFERRING,            /*Channel is busy in transfer*/
    HSI_CHANNEL_STATUS_TRANSFER_ENDED           /*Channel transfer has completed*/
} t_hsi_channel_status;


/*------------------------------------------------------------------------------
    HSI Parity Status  
--------------------------------------------------------------------------------*/

typedef enum
{
    HSI_NO_PARITY                       = 0x00,
    HSI_ODD_PARITY                      = 0x01,
    HSI_EVEN_PARITY                     = 0x02,
}t_hsi_parity;

/*------------------------------------------------------------------------------
     HSI Flush Bits 
--------------------------------------------------------------------------------*/
typedef enum
{
    HSI_FLUSH_BIT_01                     = 0x01,
    HSI_FLUSH_BIT_02                     = 0x02,
    HSI_FLUSH_BIT_03                     = 0x03,
    HSI_FLUSH_BIT_04                     = 0x04,
    HSI_FLUSH_BIT_05                     = 0x05,
    HSI_FLUSH_BIT_06                     = 0x06,
    HSI_FLUSH_BIT_07                     = 0x07,
    HSI_FLUSH_BIT_08                     = 0x08,
    HSI_FLUSH_BIT_09                     = 0x09,
    HSI_FLUSH_BIT_10                     = 0x0A,
    HSI_FLUSH_BIT_11                     = 0x0B,
    HSI_FLUSH_BIT_12                     = 0x0C,
    HSI_FLUSH_BIT_13                     = 0x0D,
    HSI_FLUSH_BIT_14                     = 0x0E,
    HSI_FLUSH_BIT_15                     = 0x0F,
    HSI_FLUSH_BIT_16                     = 0x10,
    HSI_FLUSH_BIT_17                     = 0x11,
    HSI_FLUSH_BIT_18                     = 0x12,
    HSI_FLUSH_BIT_19                     = 0x13,
    HSI_FLUSH_BIT_20                     = 0x14,
    HSI_FLUSH_BIT_21                     = 0x15,
    HSI_FLUSH_BIT_22                     = 0x16,
    HSI_FLUSH_BIT_23                     = 0x17,
    HSI_FLUSH_BIT_24                     = 0x18,
    HSI_FLUSH_BIT_25                     = 0x19,
    HSI_FLUSH_BIT_26                     = 0x1A,
    HSI_FLUSH_BIT_27                     = 0x1B,
    HSI_FLUSH_BIT_28                     = 0x1C,
    HSI_FLUSH_BIT_29                     = 0x1D,
    HSI_FLUSH_BIT_30                     = 0x1E,
    HSI_FLUSH_BIT_31                     = 0x1F,
    HSI_FLUSH_BIT_32                     = 0x20,
    HSI_FLUSH_BIT_33                     = 0x21,
    HSI_FLUSH_BIT_34                     = 0x22,
    HSI_FLUSH_BIT_35                     = 0x23,
    HSI_FLUSH_BIT_36                     = 0x24,
    HSI_FLUSH_BIT_37                     = 0x25,
    HSI_FLUSH_BIT_38                     = 0x26,
    HSI_FLUSH_BIT_39                     = 0x27,
    HSI_FLUSH_BIT_40                     = 0x28,
    HSI_FLUSH_BIT_41                     = 0x29,
    HSI_FLUSH_BIT_42                     = 0x2A,
    HSI_FLUSH_BIT_43                     = 0x2B,
    HSI_FLUSH_BIT_44                     = 0x2C,
    HSI_FLUSH_BIT_45                     = 0x2D,
    HSI_FLUSH_BIT_46                     = 0x2E,
    HSI_FLUSH_BIT_47                     = 0x2F,
    HSI_FLUSH_BIT_48                     = 0x30,
    HSI_FLUSH_BIT_49                     = 0x31,
    HSI_FLUSH_BIT_50                     = 0x32,
    HSI_FLUSH_BIT_51                     = 0x33,
    HSI_FLUSH_BIT_52                     = 0x34,
    HSI_FLUSH_BIT_53                     = 0x35,
    HSI_FLUSH_BIT_54                     = 0x36,
    HSI_FLUSH_BIT_55                     = 0x37,
    HSI_FLUSH_BIT_56                     = 0x38,
    HSI_FLUSH_BIT_57                     = 0x39,
    HSI_FLUSH_BIT_58                     = 0x3A,
    HSI_FLUSH_BIT_59                     = 0x3B,
    HSI_FLUSH_BIT_60                     = 0x3C,
    HSI_FLUSH_BIT_61                     = 0x3D,
    HSI_FLUSH_BIT_62                     = 0x3E,
    HSI_FLUSH_BIT_63                     = 0x3F
    
}t_hsi_flushbits;

/*-----------------------------------------------------------------------------
   Data Swap Register
-------------------------------------------------------------------------------*/

typedef enum
{
    HSI_NO_SWAPPING                      = 0x00,
    HSI_BYTE_SWAPPING                    = 0x01,
    HSI_HALFWORD_SWAPPING                = 0x02,
    HSI_HALFWORD_BYTE_SWAPPING           = 0x03,
}t_hsi_dataswap;

/*------------------------------------------------------------------------------
	Structures  
------------------------------------------------------------------------------*/
typedef struct
{
    t_uint32    fifo_event;     /*Transmitter and receiver fifo events*/
    t_uint32    rx_ex_event;    /*Receiver exceptions events*/
} t_hsi_event;

typedef struct
{
    t_hsi_irq_state irq_state;
    t_hsi_irq_src   initial_irq;
    t_hsi_irq_src   pending_irq;
} t_hsi_irq_status;

typedef struct
{
    t_hsi_noc           noc;
    t_hsi_rx_state      rxstate;
    t_hsi_rx_mode       mode;
    t_uint32            threshold;
    t_hsi_parity        parity;
    t_uint32            preamble;
    t_hsi_rx_detector   detector;
} t_hsi_rx_config;

typedef struct
{
    t_uint32            divisor;
    t_hsi_tx_state      txstate;
    t_uint32            burstlen;
    t_uint32            preamble;
    t_hsi_noc           noc;
    t_hsi_tx_mode       mode;
    t_hsi_parity        parity;

} t_hsi_tx_config;

typedef struct
{
    t_hsi_channel_num    channel_no;
    t_uint32             base;
    t_uint32             span;
    t_hsi_mode           mode;
    t_uint32             frame_len;
    t_hsi_tx_arb_policy  chhanel_priority;
    t_uint32             watermark;
}t_hsi_tx_channel_config;

typedef struct
{
    t_hsi_channel_num    channel_no;
    t_uint32             base;
    t_uint32             span;
    t_hsi_mode           mode;
    t_uint32             frame_len;
    t_uint32             watermark;
}t_hsi_rx_channel_config;

             
typedef struct
{

    t_bool  ex_parity;
    t_bool  ex_time_out;
    t_bool  ex_brk;
    t_bool  ex_overrun;
    t_bool  overrun[8];
} t_hsi_rx_exception;

/*-----------------------------------------------------------------------------
 * Public M0 functions						                                   
 *---------------------------------------------------------------------------*/
/* Initialization functions*/
PUBLIC t_hsi_error  HSI_Init(t_hsi_device_id device, t_logical_address base_address);
PUBLIC t_hsi_error  HSI_GetVersion(t_version *p_version);
PUBLIC t_hsi_error  HSI_SetDbgLevel(t_dbg_level debug_level);
PUBLIC t_hsi_error  HSI_Reset(t_hsi_device_id device);

/* Configuration Related */
PUBLIC t_hsi_error  HSI_SetTxConfiguration(t_hsi_tx_config *p_tx_config);
PUBLIC t_hsi_error  HSI_SetRxConfiguration(t_hsi_rx_config *p_rx_config);
PUBLIC t_hsi_error  HSI_SetTxChannelConfig(t_hsi_tx_channel_config tx_channel_config);
PUBLIC t_hsi_error  HSI_SetRxChannelConfig(t_hsi_rx_channel_config rx_channel_config);

PUBLIC t_hsi_error  HSI_TxInSleep(void);
PUBLIC t_hsi_error  HSI_TxOutSleep(void);
PUBLIC t_hsi_error  HSI_RxInSleep(void);
PUBLIC t_hsi_error  HSI_RxOutSleep(void);
PUBLIC t_hsi_error  HSI_ChangeRxTimeout(t_uint32 value);
PUBLIC t_hsi_error  HSI_ChangeTxBitrateDivisor(t_uint32 value);

/* FIFO Related */
PUBLIC t_hsi_error  HSI_WriteBreak(void);
PUBLIC t_hsi_error  HSI_WriteSingleData(t_hsi_channel_num channel_num, t_uint32 data,t_hsi_dataswap dataswap);
PUBLIC t_hsi_error  HSI_ReadSingleData(t_hsi_channel_num channel_num, t_uint32 *p_data);

PUBLIC t_hsi_error  HSI_WriteTxBuffer(t_hsi_channel_num channelno, t_uint32 *p_buffer, t_uint32 size,t_hsi_dataswap datatswap);
PUBLIC t_hsi_error  HSI_ReadRxBuffer(t_hsi_channel_num channelno, t_uint32 *p_buffer, t_uint32 size);
PUBLIC t_hsi_error  HSI_GetTxChannelStatus(t_hsi_channel_num channel_num, t_hsi_channel_status *p_tx_ch_status);
PUBLIC t_hsi_error  HSI_GetRxChannelStatus(t_hsi_channel_num channel_num, t_hsi_channel_status *p_rx_ch_status);

PUBLIC void         HSI_GetIRQSrcStatus(t_hsi_irq_src irq_id, t_hsi_irq_status *p_status);

PUBLIC t_hsi_error  HSI_FilterProcessIRQSrc
                    (
                        t_hsi_irq_status    *p_status,
                        t_hsi_event         *p_event,
                        t_hsi_filter_mode   filter_mode
                    );
PUBLIC t_bool       HSI_IsIRQSrcActive(t_hsi_irq_src irqSrc, t_hsi_irq_status *p_status);

PUBLIC t_bool       HSI_IsEventActive(t_hsi_event *p_event);
PUBLIC void         HSI_AcknowledgeEvent(t_hsi_event *p_event);

/* Device Status */
/* Device FIFO Status */
PUBLIC t_hsi_error  HSI_GetRxState(t_hsi_rx_state *p_status);
PUBLIC t_hsi_error  HSI_GetTxState(t_hsi_tx_state *p_status);
PUBLIC t_hsi_error  HSI_TxChangeFlushBits(t_hsi_flushbits flushbits);
PUBLIC t_hsi_error  HSI_GetTxFIFOStatus(t_hsi_channel_num channelno, t_hsi_fifo_status *p_status);
PUBLIC t_hsi_error  HSI_TxFIFOReset(IN t_hsi_channel_num channel_no);
PUBLIC t_hsi_error  HSI_GetRxFIFOStatus(t_hsi_channel_num channelno, t_hsi_fifo_status *p_status);
PUBLIC t_hsi_error  HSI_RxFIFOReset(IN t_hsi_channel_num channel_no);
PUBLIC t_hsi_error  HSI_GetRxExceptionstatus(t_hsi_rx_exception *p_exception);
PUBLIC t_hsi_error  HSI_SetRxExceptionstatus(t_hsi_rx_exception *p_exception);
PUBLIC t_hsi_error  HSI_AcknowledgeException(IN t_hsi_rx_exception *p_exception);
PUBLIC t_hsi_error  HSI_SetRxRealTime(IN t_hsi_channel_num channel_no);
PUBLIC t_hsi_error  HSI_GetRxRealTime(IN t_hsi_channel_num channel_no,OUT t_bool *realtime_status);
PUBLIC t_hsi_error  HSI_ClearRxRealTime(IN t_hsi_channel_num channel_no);
PUBLIC t_hsi_error  HSI_SetTxWaterMarkLevel(IN t_uint32 watermark,IN t_hsi_channel_num channel_no);
PUBLIC t_hsi_error  HSI_SetRxWaterMarkLevel(IN t_uint32 watermark,IN t_hsi_channel_num channel_no);
#if ((defined ST_8500V1) || (defined ST_HREFV1) || (defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
PUBLIC t_hsi_error  HSI_GetRxFrameBurstCnt(OUT t_uint32 *framebrstcnt);
PUBLIC t_hsi_error  HSI_SetRxFrameBurstCnt(IN t_uint32 framebrstcnt);
#endif
/*Power Management*/
PUBLIC void         HSI_SaveDeviceContext(void);
PUBLIC void         HSI_RestoreDeviceContext(void);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _INC_HSI_H */

