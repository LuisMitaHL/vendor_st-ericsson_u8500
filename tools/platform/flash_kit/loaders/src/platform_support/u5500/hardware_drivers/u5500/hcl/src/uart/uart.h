/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This File contains public headers for HAL Routines for UART (U5500)
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "uart_irq.h"
/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
#define UART0_DMA_ON_ERROR  1
#define UART1_DMA_ON_ERROR  1
#define UART2_DMA_ON_ERROR  1
#define UART3_DMA_ON_ERROR  1

/* Defines for Version */
#define UART_HCL_VERSION_ID 1
#define UART_HCL_MAJOR_ID   0
#define UART_HCL_MINOR_ID   0

#define UART_TX_IT_MODE     MASK_BIT0
#define UART_TX_DMA_MODE    MASK_BIT1
#define UART_TX_POLL_MODE   MASK_BIT2

#define UART_RX_IT_MODE     MASK_BIT3
#define UART_RX_DMA_MODE    MASK_BIT4
#define UART_RX_POLL_MODE   MASK_BIT5


/* UART IP ID Definition */
typedef struct
{
    t_uint8 minor_rev_y;
    t_uint8 sw_complaincy_rev_x;
    t_uint8 hw_rev_h;
    t_uint8 product_id;
    t_uint16 provider_id;
}t_uart_ip_id;

/* Define baudrate */
typedef enum
{
    UART_BR_50_BAUD      =      50,
    UART_BR_75_BAUD      =      75,
    UART_BR_110_BAUD     =     110,
    UART_BR_134_BAUD     =     134,
    UART_BR_150_BAUD     =     150,
    UART_BR_200_BAUD     =     200,
    UART_BR_300_BAUD     =     300,
    UART_BR_600_BAUD     =     600,
    UART_BR_1200_BAUD    =    1200,
    UART_BR_1800_BAUD    =    1800,
    UART_BR_2000_BAUD    =    2000,
    UART_BR_2400_BAUD    =    2400,
    UART_BR_3600_BAUD    =    3600,
    UART_BR_4800_BAUD    =    4800,
    UART_BR_7200_BAUD    =    7200,
    UART_BR_9600_BAUD    =    9600,
    UART_BR_14400_BAUD   =   14400,
    UART_BR_19200_BAUD   =   19200,
    UART_BR_38400_BAUD   =   38400,
    UART_BR_56000_BAUD   =   56000,
    UART_BR_57600_BAUD   =   57600,
    UART_BR_115200_BAUD  =  115200,
    UART_BR_230400_BAUD  =  230400,
    UART_BR_460800_BAUD  =  460800,
    UART_BR_500000_BAUD  =  500000,
    UART_BR_576000_BAUD  =  576000,
    UART_BR_921600_BAUD  =  921600,
    UART_BR_1000000_BAUD = 1000000,
    UART_BR_1152000_BAUD = 1152000,
    UART_BR_1500000_BAUD = 1500000,
    UART_BR_1843200_BAUD = 1843200,
    UART_BR_2000000_BAUD = 2000000,
    UART_BR_2400000_BAUD = 2400000,
    UART_BR_2500000_BAUD = 2500000,
    UART_BR_3000000_BAUD = 3000000,
    UART_BR_3686400_BAUD = 3686400,
    UART_BR_4800000_BAUD = 4800000, 
    UART_BR_LAST_BAUD    = 0xFFFFFF
} t_uart_baud_rate;

/* Parity definition */
typedef enum
{
    UART_NO_PARITY_BIT,
    UART_EVEN_PARITY_BIT,
    UART_ODD_PARITY_BIT,
    UART_STICK_PARITY_0_BIT,                            /* Stick parity mode */
    UART_STICK_PARITY_1_BIT                             /* Stick parity mode */
} t_uart_parity_bit;

/* Data bits number */
typedef enum
{
    UART_DATA_BITS_5        = 0x0,
    UART_DATA_BITS_6        = 0x1,
    UART_DATA_BITS_7        = 0x2,
    UART_DATA_BITS_8        = 0x3
} t_uart_data_bits;

/* Stop bits number */
typedef enum
{
    UART_ONE_STOP_BIT       = 0x0,
    UART_TWO_STOP_BITS      = 0x1
} t_uart_stop_bits;


/* UART FLOW CONTROL DEFINITION */
typedef enum
{
    UART_HW_FLOW_CONTROL,                               /* Bytes flow managed by RTS and CTS signals */
    UART_SW_FLOW_CONTROL,                               /* Bytes flow managed by special characters (Xon/Xoff) */

    /* included in the data stream  */
    UART_NO_FLOW_CONTROL                                /* No bytes flow management */
} t_uart_flow_control;

/* Rx and Tx Trigger Levels */
typedef enum
{
    UART_RX_TRIG_ONE_BYTE_FULL = 0,
    UART_RX_TRIG_ONE_THIRTYSECOND_FULL,
    UART_RX_TRIG_ONE_SIXTEENTH_FULL,
    UART_RX_TRIG_ONE_EIGHT_FULL,
    UART_RX_TRIG_ONE_FOURTH_FULL,
    UART_RX_TRIG_HALF_FULL,  
    UART_RX_TRIG_THREE_FOURTH_FULL
} t_uart_rx_trigger;

typedef enum
{
    UART_TX_TRIG_ONE_BYTE_EMPTY = 0,
    UART_TX_TRIG_ONE_THIRTYSECOND_EMPTY,
    UART_TX_TRIG_ONE_SIXTEENTH_EMPTY,	
    UART_TX_TRIG_ONE_EIGHTH_EMPTY,
    UART_TX_TRIG_ONE_FOURTH_EMPTY,
    UART_TX_TRIG_HALF_EMPTY,
    UART_TX_TRIG_THREE_FOURTH_EMPTY
} t_uart_tx_trigger;

typedef struct
{
    t_uart_tx_trigger uart_tx_trigger;
    t_uart_rx_trigger uart_rx_trigger;
}t_uart_dmawm_trigger;

typedef struct
{
    t_uart_data_bits   uart_data_bits;
    t_uart_parity_bit  uart_parity_bits;
    t_uart_stop_bits   uart_stop_bits;
    t_bool             uart_enable_fifo;
}t_uart_txrx_config;


/* SET OR CLEAR COMMAND */
typedef enum
{
    UART_CLEAR,
    UART_SET,
    UART_NO_CHANGE
} t_uart_set_or_clear;

/* UART RECEIVE STATUS */
typedef enum
{
    UART_RECEIVE_OK                     = MASK_NULL8,
    UART_OVERRUN_ERROR                  = MASK_BIT3,    /* Asserted if data is received when receive FIFO already full */

    /* Deasserted once there is an empty space in the fifo */
    UART_BREAK_ERROR                    = MASK_BIT2,    /* Correponds to a break condition detection */
    UART_PARITY_ERROR                   = MASK_BIT1,    /* Correponds to an error in the parity of received data */
    UART_FRAMING_ERROR                  = MASK_BIT0     /* Correponds to an error in the stop bits received */
} t_uart_rec_status;

#define t_uart_receive_status   t_uint32

/* UART TEST MODE */
typedef enum                    
{
    UART_FUNC_MODE,             /* UART is then set in functional mode */
    UART_LOOP_BACK_MODE         /* Internal connection for data path (UTXD and URXD) */
} t_uart_test_mode;

/* UART MODEM MODE */
typedef enum
{
    UART_NON_MODEM_MODE,        /* Default mode */
    UART_DTE_MODE,              /* Default MODEM mode : Data Terminal Equipment mode     */
    UART_DCE_MODE               /* Second MODEM mode : Data Communication Equipment mode */
} t_uart_mode;

/* Parity Error Behaviour */
typedef enum
{
    UART_PERR_IGNORE,       /* Error ignored and bad parity bytes put apart             */
    UART_PERR_REPLACECHAR,  /* Replace bad parity bytes by a specific byte              */
    UART_PERR_ENDREAD       /* End immediately reading operation and return an error    */
} t_uart_perr_condition;

/*------------------------------------------*
 * Possible Errors returned by UART APIs    *
 *------------------------------------------*/
typedef enum
{
    UART_DTR_RECEIVED_UART_OK           = (HCL_MAX_ERROR_VALUE - 2),        /* Value : (-67) */
    UART_MODEM_ERROR                    = (HCL_MAX_ERROR_VALUE - 1),        /* Value : (-66) */
    UART_RECEIVE_ERROR                  = HCL_MAX_ERROR_VALUE,              /* Value : (-65) */
    UART_INTERNAL_ERROR                 = HCL_INTERNAL_ERROR,               /* Value :  (-8) */
    UART_NOT_CONFIGURED                 = HCL_NOT_CONFIGURED,               /* Value :  (-7) */
    UART_REQUEST_PENDING                = HCL_REQUEST_PENDING,              /* Value :  (-6) */
    UART_REQUEST_NOT_APPLICABLE         = HCL_REQUEST_NOT_APPLICABLE,       /* Value :  (-5) */
    UART_INVALID_PARAMETER              = HCL_INVALID_PARAMETER,            /* Value :  (-4) */
    UART_UNSUPPORTED_FEATURE            = HCL_UNSUPPORTED_FEATURE,          /* Value :  (-3) */
    UART_UNSUPPORTED_HW                 = HCL_UNSUPPORTED_HW,               /* Value :  (-2) */
    UART_ERROR                          = HCL_ERROR,                        /* Value :  (-1) */
    UART_OK                             = HCL_OK,                           /* Value :  ( 0) */
    UART_INTERNAL_EVENT                 = HCL_INTERNAL_EVENT,               /* Value :  ( 1) */
    UART_REMAINING_PENDING_EVENTS       = HCL_REMAINING_PENDING_EVENTS,     /* Value :  ( 2) */
    UART_NO_MORE_PENDING_EVENT          = HCL_NO_MORE_PENDING_EVENT,        /* Value :  ( 4) */
    UART_NO_MORE_FILTER_PENDING_EVENT   = HCL_NO_MORE_FILTER_PENDING_EVENT, /* Value :  ( 5) */
    UART_NO_PENDING_EVENT_ERROR         = HCL_NO_PENDING_EVENT_ERROR        /* Value :  ( 7) */
} t_uart_error;

/* Software Flow Control Mode */
typedef enum
{
    UART_RX_SW_FC1                      = MASK_BIT1,
    UART_RX_SW_FC2                      = MASK_BIT2,
    UART_TX_SW_FC1                      = MASK_BIT3,
    UART_TX_SW_FC2                      = MASK_BIT4,
    UART_XON_ANY                        = MASK_BIT5,
    UART_SPEC_CHAR                      = MASK_BIT6
} t_uart_sw_fc_mode;

typedef t_uint32    t_uart_sw_fcm;


/* UART GLOBAL CONFIGURATION */
typedef struct
{
    t_uart_parity_bit       tx_parity_bit;      /* Choice of uart tx parity bits number */
    t_uart_parity_bit       rx_parity_bit;      /* Choice of uart rx parity bits number */
    t_uart_data_bits        tx_data_bits;       /* Choice of uart tx data bits number */
    t_uart_data_bits        rx_data_bits;       /* Choice of uart rx data bits number */
    t_uart_stop_bits        tx_stop_bits;       /* Choice of uart tx stop bits number */
    t_uart_stop_bits        rx_stop_bits;       /* Choice of uart rx stop bits number */
    t_uart_baud_rate        baud_rate;          /* Choice of UART baudrate */
    t_uart_mode             uart_mode;          /* Choice between modem (DTE or DCE) and non modem operation */
    t_uart_flow_control     flow_control;       /* Choice between HW,SW or no flow control */
    t_uart_sw_fcm           sw_fc_mode;         /* Software flow control Mode */
    t_bool                  receive_enable;     /* Enable UART in reception */
    t_bool                  transmit_enable;    /* Enable UART in transmission */
    t_bool                  enable_autobaud;    /* Enable Autobaud Functionality */
    t_bool                  tx_enable_fifo;     /* Enable/disable Tx FIFOs */
    t_bool                  rx_enable_fifo;     /* Enable/disable Rx FIFOs */
    t_uart_perr_condition   perr_condition;     /* Parity Error Behaviour */
    t_uint8                 xoff1_char;         /* Xoff1 Character */
    t_uint8                 xon1_char;          /* Xon1 Character */
    t_uint8                 xoff2_char;         /* Xoff2 Character */
    t_uint8                 xon2_char;          /* Xon2 Character */
    t_uart_tx_trigger       tx_trigger;         /* Int Trigger Level in Tx FIFO */
    t_uart_rx_trigger       rx_trigger;         /* Int Trigger Level in Rx FIFO */
    t_uart_tx_trigger       tx_dmawm_trigger;   /* DMA Watermark Trigger Level in Tx FIFO */
    t_uart_rx_trigger       rx_dmawm_trigger;   /* DMA Watermark Trigger Level in Rx FIFO */
} t_uart_config;

#ifdef __UART_ELEMENTARY
PUBLIC t_bool       UART_isTxfifoFull(IN t_uart_device_id uart_device_id);
PUBLIC t_bool       UART_isTxfifoEmpty(IN t_uart_device_id uart_device_id);
PUBLIC t_bool       UART_isRxfifoEmpty(IN t_uart_device_id uart_device_id);
PUBLIC t_uart_error UART_GetRxChar
                    (
                        IN t_uart_device_id         uart_device_id,
                        OUT t_uint8                 *p_data_char,
                        OUT t_uart_receive_status   *p_receive_error
                    );
PUBLIC t_uart_error UART_SetTxChar(IN t_uart_device_id uart_device_id, IN t_uint8 char_to_send);
#endif /* __UART_ELEMENTARY */

#ifdef __UART_ENHANCED

/* Defines the events associated with the interrupts *
 *---------------------------------------------------*/
typedef t_uint32    t_uart_event;

#define UART_PARITY_ERROR_CHAR  ((t_uint8) 0xFF)

/* Defines the filter mode *
 *-------------------------*/
typedef t_uint32    t_uart_filter_mode;
#define UART_NO_FILTER_MODE 0

/* Enumeration used to indicate the state (Old/New) of t_uart_irq_status structure *
 *---------------------------------------------------------------------------------*/
typedef enum
{
    UART_IRQ_STATE_NEW                  = 0,
    UART_IRQ_STATE_OLD                  = 1
} t_uart_irq_state;

/* Structure used to store information associated with a hardware interrupt source *
 *---------------------------------------------------------------------------------*/
typedef struct
{
    t_uart_irq_state    irq_state;
    t_uint8             padding[3];
    t_uart_irq_src      initial_irq;
    t_uart_irq_src      pending_irq;
} t_uart_irq_status;

/* UART TX COMMUNICATION STATUS */
typedef enum
{
    UART_TRANSMIT_ON_GOING,
    UART_WRITTINGS_ENDED,   /* Means that all suitable fifo writtings were performed */
    UART_TRANSMIT_ENDED     /* Next step of WRITTINGS_ENDED: Message was not only */
    /* written in fifo but also transmitted by UART */
} t_uart_tx_comm_status;

/* UART RX COMMUNICATION STATUS */
typedef enum
{
    UART_RECEIVE_ON_GOING,
    UART_RECEIVE_ENDED
} t_uart_rx_comm_status;

/* UART IT COMMUNICATION STATUS */
typedef struct
{
    t_uart_tx_comm_status   tx_comm_status;
    t_uart_rx_comm_status   rx_comm_status;
} t_uart_it_communication_status;

PUBLIC void         UART_GetIRQSrcStatus(IN t_uart_irq_src irq_src, OUT t_uart_irq_status *p_status);
PUBLIC t_bool       UART_IsIRQSrcActive(IN t_uart_irq_src irq_src, OUT t_uart_irq_status *p_status);
PUBLIC t_uart_error UART_ProcessIRQSrc(INOUT t_uart_irq_status *p_status);
PUBLIC t_uart_error UART_FilterProcessIRQSrc
                    (
                        INOUT t_uart_irq_status *p_status,
                        INOUT t_uart_event      *p_event,
                        IN t_uart_filter_mode   filter_mode
                    );
PUBLIC t_bool       UART_IsEventActive(IN t_uart_event *p_event);
PUBLIC void         UART_AcknowledgeEvent(IN t_uart_event *p_event);
PUBLIC t_uart_error UART_SendXoff(IN t_uart_device_id uart_device_id);
PUBLIC t_uart_error UART_SendXon(IN t_uart_device_id uart_device_id);
PUBLIC t_uart_error UART_GetFlowControlStatus
                    (
                        IN t_uart_device_id uart_device_id,
                        OUT t_bool          *p_xoff_char,
                        OUT t_bool          *p_xon_char
                    );
PUBLIC t_uart_error UART_BytesReceived(IN t_uart_device_id uart_device_id, OUT t_uint32 *p_bytes);
PUBLIC t_uart_error UART_BytesTransmitted(IN t_uart_device_id uart_device_id, OUT t_uint32 *p_bytes);
PUBLIC t_uart_error UART_ReceiveCancel(IN t_uart_device_id uart_device_id);
PUBLIC t_uart_error UART_TransmitCancel(IN t_uart_device_id uart_device_id);
PUBLIC t_uart_error UART_GetReceiveStatus(IN t_uart_device_id uart_device_id, OUT t_uart_receive_status *p_rx_status);
PUBLIC t_uart_error UART_GetComStatus
                    (
                        IN t_uart_device_id                 uart_device_id,
                        OUT t_uart_it_communication_status  *p_comm_status
                    );
#endif /* __UART_ENHANCED */

/* Common Functions */
#ifdef __DEBUG
PUBLIC t_uart_error UART_SetDbgLevel(IN t_dbg_level debug_level);
PUBLIC t_uart_error UART_GetDbgLevel(OUT t_dbg_level *p_debug_level);
#endif
PUBLIC t_uart_error UART_Init(IN t_uart_device_id uart_device_id, IN t_logical_address uart_base_address);
PUBLIC t_uart_error UART_GetProductId(IN t_uart_device_id  uart_device_id, OUT t_uart_ip_id *uart_ip_id);
PUBLIC t_uart_error UART_SetDmaWatermarkLevel(IN t_uart_device_id uart_device_id, IN t_uart_dmawm_trigger uart_dmawm_trigger);
PUBLIC t_uart_error UART_ReConfigureRx(IN t_uart_device_id uart_device_id, IN t_uart_txrx_config uart_rx_config);
PUBLIC t_uart_error UART_ReConfigureTx(IN t_uart_device_id uart_device_id, IN t_uart_txrx_config uart_tx_config);
PUBLIC t_uart_error UART_ManageSendBreak(IN t_uart_device_id uart_device_id, IN t_uart_set_or_clear command);
PUBLIC t_uart_error UART_SetTestMode(IN t_uart_device_id uart_device_id, IN t_uart_test_mode test_mode);
PUBLIC t_uart_error UART_DTEGetInputSignals
                    (
                        IN t_uart_device_id     uart_device_id,
                        OUT t_uart_set_or_clear *p_ri,
                        OUT t_uart_set_or_clear *p_dsr,
                        OUT t_uart_set_or_clear *p_dcd,
                        OUT t_uart_set_or_clear *p_cts
                    );
PUBLIC t_uart_error UART_DTESetOutputSignals
                    (
                        IN t_uart_device_id     uart_device_id,
                        IN t_uart_set_or_clear  uart_dtr,
                        IN t_uart_set_or_clear  uart_rts
                    );
PUBLIC t_uart_error UART_DCEGetInputSignals
                    (
                        IN t_uart_device_id     uart_device_id,
                        OUT t_uart_set_or_clear *uart_dtr,
                        OUT t_uart_set_or_clear *uart_cts
                    );
PUBLIC t_uart_error UART_DCESetOutputSignals
                    (
                        IN t_uart_device_id     uart_device_id,
                        IN t_uart_set_or_clear  uart_ri,
                        IN t_uart_set_or_clear  uart_dsr,
                        IN t_uart_set_or_clear  uart_dcd,
                        IN t_uart_set_or_clear  uart_rts
                    );
PUBLIC t_uart_error UART_GetVersion(OUT t_version *p_version);
PUBLIC t_uart_error UART_TransmitxCharacters
                    (
                        IN t_uart_device_id uart_device_id,
                        IN t_uint32         num_of_char_to_be_tx,
                        IN t_uint8          *p_data_char
                    );
PUBLIC t_uart_error UART_ReceivexCharacters
                    (
                        IN t_uart_device_id         uart_device_id,
                        IN t_uint32                 num_of_char_to_be_recd,
                        OUT t_uint8                 *p_data_char,
                        OUT t_uint32                *p_num_of_char_received,
                        OUT t_uart_receive_status   *p_receive_status
                    );
PUBLIC t_uart_error UART_PowerOn(IN t_uart_device_id uart_device_id);
PUBLIC t_uart_error UART_PowerOff(IN t_uart_device_id uart_device_id);
PUBLIC t_uart_error UART_SetConfiguration(IN t_uart_device_id uart_device_id, IN t_uart_config *p_config);
PUBLIC t_uart_error UART_SetTransferMode(IN t_uart_device_id uart_device_id, IN t_uint32 flags);
PUBLIC t_uart_error UART_ConfigureAutoBaud(IN t_uart_device_id uart_device_id, IN t_uart_set_or_clear uart_status);
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
                    );
PUBLIC t_uart_error UART_Disable(IN t_uart_device_id uart_device_id);
PUBLIC t_uart_error UART_SetTimeout(IN t_uart_device_id , IN t_uint32 );

/* UART_DEVICE_ID_3 Interrupts */
#define UART3_DEVICE_ALL_IT                 0x00037FFF
#define UART3_AUTOBAUD_DONE_IT              0x00034000
#define UART3_AUTOBAUD_ERROR_IT             0x00032000
#define UART3_TXFE_IT                       0x00031000
#define UART3_XOFF_IT                       0x00030800
#define UART3_OVERRUN_ERROR_IT              0x00030400
#define UART3_BREAK_ERROR_IT                0x00030200
#define UART3_PARITY_ERROR_IT               0x00030100
#define UART3_FRAMING_ERROR_IT              0x00030080
#define UART3_RECEIVE_TIMEOUT_IT            0x00030040
#define UART3_TRANSMIT_IT                   0x00030020
#define UART3_RECEIVE_IT                    0x00030010
#define UART3_DATA_SET_READY_MODEM_IT       0x00030008
#define UART3_DATA_CARRIER_DETECT_MODEM_IT  0x00030004
#define UART3_CLEAR_TO_SEND_MODEM_IT        0x00030002
#define UART3_RING_INDICATOR_MODEM_IT       0x00030001

/* UART_DEVICE_ID_2 Interrupts */
#define UART2_DEVICE_ALL_IT                 0x00027FFF
#define UART2_AUTOBAUD_DONE_IT              0x00024000
#define UART2_AUTOBAUD_ERROR_IT             0x00022000
#define UART2_TXFE_IT                       0x00021000
#define UART2_XOFF_IT                       0x00020800
#define UART2_OVERRUN_ERROR_IT              0x00020400
#define UART2_BREAK_ERROR_IT                0x00020200
#define UART2_PARITY_ERROR_IT               0x00020100
#define UART2_FRAMING_ERROR_IT              0x00020080
#define UART2_RECEIVE_TIMEOUT_IT            0x00020040
#define UART2_TRANSMIT_IT                   0x00020020
#define UART2_RECEIVE_IT                    0x00020010
#define UART2_DATA_SET_READY_MODEM_IT       0x00020008
#define UART2_DATA_CARRIER_DETECT_MODEM_IT  0x00020004
#define UART2_CLEAR_TO_SEND_MODEM_IT        0x00020002
#define UART2_RING_INDICATOR_MODEM_IT       0x00020001

/* UART_DEVICE_ID_1 Interrupts */
#define UART1_DEVICE_ALL_IT                 0x00017FFF
#define UART1_AUTOBAUD_DONE_IT              0x00014000
#define UART1_AUTOBAUD_ERROR_IT             0x00012000
#define UART1_TXFE_IT                       0x00011000
#define UART1_XOFF_IT                       0x00010800
#define UART1_OVERRUN_ERROR_IT              0x00010400
#define UART1_BREAK_ERROR_IT                0x00010200
#define UART1_PARITY_ERROR_IT               0x00010100
#define UART1_FRAMING_ERROR_IT              0x00010080
#define UART1_RECEIVE_TIMEOUT_IT            0x00010040
#define UART1_TRANSMIT_IT                   0x00010020
#define UART1_RECEIVE_IT                    0x00010010
#define UART1_DATA_SET_READY_MODEM_IT       0x00010008
#define UART1_DATA_CARRIER_DETECT_MODEM_IT  0x00010004
#define UART1_CLEAR_TO_SEND_MODEM_IT        0x00010002
#define UART1_RING_INDICATOR_MODEM_IT       0x00010001

/* UART IT */
/* UART_DEVICE_ID_0 Interrupts */
#define UART0_DEVICE_ALL_IT                 0x00007FFF
#define UART0_AUTOBAUD_DONE_IT              0x00004000
#define UART0_AUTOBAUD_ERROR_IT             0x00002000
#define UART0_TXFE_IT                       0x00001000
#define UART0_XOFF_IT                       0x00000800
#define UART0_OVERRUN_ERROR_IT              0x00000400
#define UART0_BREAK_ERROR_IT                0x00000200
#define UART0_PARITY_ERROR_IT               0x00000100
#define UART0_FRAMING_ERROR_IT              0x00000080
#define UART0_RECEIVE_TIMEOUT_IT            0x00000040
#define UART0_TRANSMIT_IT                   0x00000020
#define UART0_RECEIVE_IT                    0x00000010
#define UART0_DATA_SET_READY_MODEM_IT       0x00000008
#define UART0_DATA_CARRIER_DETECT_MODEM_IT  0x00000004
#define UART0_CLEAR_TO_SEND_MODEM_IT        0x00000002
#define UART0_RING_INDICATOR_MODEM_IT       0x00000001

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _UART_H_ */

/* End of file - uart.h */


