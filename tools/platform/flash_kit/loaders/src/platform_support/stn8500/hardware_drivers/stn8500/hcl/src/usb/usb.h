/*****************************************************************************/
/**
*  � ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This file holds the register data structures used by HCL.
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _USB_H_
#define _USB_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "debug.h"

/* Total number of endpoints 16-IN and 16-OUT */
#define USB_MAX_NUM_ENDPOINTS   32



typedef enum
{
    USB_ENDPNUM_0_IN,
    USB_ENDPNUM_1_IN,
    USB_ENDPNUM_2_IN,
    USB_ENDPNUM_3_IN,
    USB_ENDPNUM_4_IN,
    USB_ENDPNUM_5_IN,
    USB_ENDPNUM_6_IN,
    USB_ENDPNUM_7_IN,
    USB_ENDPNUM_8_IN,
    USB_ENDPNUM_9_IN,
    USB_ENDPNUM_10_IN,
    USB_ENDPNUM_11_IN,
    USB_ENDPNUM_12_IN,
    USB_ENDPNUM_13_IN,
    USB_ENDPNUM_14_IN,
    USB_ENDPNUM_15_IN,

    USB_ENDPNUM_0_OUT,
    USB_ENDPNUM_1_OUT,
    USB_ENDPNUM_2_OUT,
    USB_ENDPNUM_3_OUT,
    USB_ENDPNUM_4_OUT,
    USB_ENDPNUM_5_OUT,
    USB_ENDPNUM_6_OUT,
    USB_ENDPNUM_7_OUT,
    USB_ENDPNUM_8_OUT,
    USB_ENDPNUM_9_OUT,
    USB_ENDPNUM_10_OUT,
    USB_ENDPNUM_11_OUT,
    USB_ENDPNUM_12_OUT,
    USB_ENDPNUM_13_OUT,
    USB_ENDPNUM_14_OUT,
    USB_ENDPNUM_15_OUT
} t_usb_endp_num;

/* USB Errors */
typedef enum
{
    USB_INTERNAL_EVENT              = HCL_INTERNAL_EVENT,
    USB_REMAINING_PENDING_EVENTS    = HCL_REMAINING_PENDING_EVENTS,
    USB_NO_MORE_PENDING_EVENT       = HCL_NO_MORE_PENDING_EVENT,
    USB_NO_MORE_FILTER_PENDING_EVENT= HCL_NO_MORE_FILTER_PENDING_EVENT,
    USB_NO_PENDING_EVENT_ERROR      = HCL_NO_PENDING_EVENT_ERROR,
    USB_OK                          = HCL_OK,
    USB_ERROR                       = HCL_ERROR,
    USB_INVALID_PARAMETER           = HCL_INVALID_PARAMETER,
    USB_REQUEST_NOT_APPLICABLE      = HCL_REQUEST_NOT_APPLICABLE,
    USB_INTERNAL_ERROR              = HCL_INTERNAL_ERROR,
    USB_UNSUPPORTED_HW              = HCL_UNSUPPORTED_HW,
    USB_UNSUPPORTED_FEATURE         = HCL_UNSUPPORTED_FEATURE,
    USB_REQUEST_PENDING             = HCL_REQUEST_PENDING,
    USB_NOT_CONFIGURED              = HCL_NOT_CONFIGURED
} t_usb_error;

typedef struct
{
    t_bool      high_speed_mode_enabled;
    t_uint16    frame_number;
    t_bool      rx_multipacket_fifo_enabled;
    t_bool      tx_multipacket_fifo_enabled;
    t_bool      is_big_endian;
    t_bool      high_bandwidth_rx_fifo_enabled;
    t_bool      high_bandwidth_tx_fifo_enabled;
    t_bool      dynamic_fifo_sizing_enabled;
    t_bool      soft_conn_disconn_enabled;
    t_uint8     utmi_data_width;
} t_usb_status;

#define USB_HCL_VERSION_ID  5
#define USB_HCL_MAJOR_ID    1
#define USB_HCL_MINOR_ID    4

/* Different USB IRQ Sorces */
typedef enum
{
    USB_IRQ_SRC_VBUSERROR           = MASK_BIT0,
    USB_IRQ_SRC_SESSIONREQ          = MASK_BIT1,
    USB_IRQ_SRC_DISCONNECT          = MASK_BIT2,
    USB_IRQ_SRC_CONNECT             = MASK_BIT3,
    USB_IRQ_SRC_SOF                 = MASK_BIT4,
    USB_IRQ_SRC_RESET               = MASK_BIT5,
    USB_IRQ_SRC_BABBLE              = MASK_BIT6,
    USB_IRQ_SRC_RESUME              = MASK_BIT7,
    USB_IRQ_SRC_SUSPEND             = MASK_BIT8,
    USB_IRQ_SRC_I2C                 = MASK_BIT9,
    USB_IRQ_SRC_OTG                 = MASK_BIT10,
    USB_IRQ_SRC_PORTSTAT            = MASK_BIT11,
    USB_IRQ_SRC_EP0                 = MASK_BIT12,
    USB_IRQ_SRC_DEVRESPONSE         = MASK_BIT13,
    USB_IRQ_SRC_SYS_ALL             = 0x3FFF
} t_usb_irq_src_id;

/* IRQ Source ID's */
typedef enum
{
    USB_IRQ_SRC_IN_EP1              = MASK_BIT0,
    USB_IRQ_SRC_IN_EP2              = MASK_BIT1,
    USB_IRQ_SRC_IN_EP3              = MASK_BIT2,
    USB_IRQ_SRC_IN_EP4              = MASK_BIT3,
    USB_IRQ_SRC_IN_EP5              = MASK_BIT4,
    USB_IRQ_SRC_IN_EP6              = MASK_BIT5,
    USB_IRQ_SRC_IN_EP7              = MASK_BIT6,
    USB_IRQ_SRC_IN_EP8              = MASK_BIT7,
    USB_IRQ_SRC_IN_EP9              = MASK_BIT8,
    USB_IRQ_SRC_IN_EP10             = MASK_BIT9,
    USB_IRQ_SRC_IN_EP11             = MASK_BIT10,
    USB_IRQ_SRC_IN_EP12             = MASK_BIT11,
    USB_IRQ_SRC_IN_EP13             = MASK_BIT12,
    USB_IRQ_SRC_IN_EP14             = MASK_BIT13,
    USB_IRQ_SRC_IN_EP15             = MASK_BIT14,

    USB_IRQ_SRC_OUT_EP1             = MASK_BIT15,
    USB_IRQ_SRC_OUT_EP2             = MASK_BIT16,
    USB_IRQ_SRC_OUT_EP3             = MASK_BIT17,
    USB_IRQ_SRC_OUT_EP4             = MASK_BIT18,
    USB_IRQ_SRC_OUT_EP5             = MASK_BIT19,
    USB_IRQ_SRC_OUT_EP6             = MASK_BIT20,
    USB_IRQ_SRC_OUT_EP7             = MASK_BIT21,
    USB_IRQ_SRC_OUT_EP8             = MASK_BIT22,
    USB_IRQ_SRC_OUT_EP9             = MASK_BIT23,
    USB_IRQ_SRC_OUT_EP10            = MASK_BIT24,
    USB_IRQ_SRC_OUT_EP11            = MASK_BIT25,
    USB_IRQ_SRC_OUT_EP12            = MASK_BIT26,
    USB_IRQ_SRC_OUT_EP13            = MASK_BIT27,
    USB_IRQ_SRC_OUT_EP14            = MASK_BIT28,
    USB_IRQ_SRC_OUT_EP15            = MASK_BIT29,

    USB_IRQ_SRC_EP_ALL              = 0x3FFFFFFF
} t_usb_endp_irq_src_id;

typedef struct
{
    t_uint32        system;
    t_uint32        endp;
} t_usb_irq_src;

typedef t_usb_irq_src   t_usb_filter_mode;
typedef t_usb_irq_src   t_usb_event;

#define USB_NO_FILTER_MODE  0

typedef enum
{
    USB_IRQ_STATE_NEW               = 0,
    USB_IRQ_STATE_OLD               = 1
} t_usb_irq_state;

typedef struct
{
    t_usb_irq_state irq_state;
    t_usb_irq_src   initial_irq;
    t_usb_irq_src   pending_irq;
} t_usb_irq_status;

/* USB Device Power Modes */
typedef enum
{
    USB_BUS_POWERED,
    USB_SELF_POWERED
} t_usb_dev_power_mode;

/* USB Bus Speed */
typedef enum
{
    USB_LOW_SPEED                   = 2,
    USB_FULL_SPEED,
    USB_HIGH_SPEED
} t_usb_speed;

/* USB FIFO Sizes */
typedef enum
{
    USB_FIFO_SIZE_8                 = 8,
    USB_FIFO_SIZE_16                = 16,
    USB_FIFO_SIZE_32                = 32,
    USB_FIFO_SIZE_64                = 64,
    USB_FIFO_SIZE_128               = 128,
    USB_FIFO_SIZE_256               = 256,
    USB_FIFO_SIZE_512               = 512,
    USB_FIFO_SIZE_1024              = 1024,
    USB_FIFO_SIZE_2048              = 2048,
    USB_FIFO_SIZE_4096              = 4096
} t_usb_fifo_size;

typedef struct
{
    t_usb_fifo_size         fifosize[USB_MAX_NUM_ENDPOINTS];
    t_usb_dev_power_mode    power_mode;
    t_usb_speed             speed;
    t_bool                  remote_wakeup_supported;
} t_usb_dev_config;

#ifdef ST_8500ED
#define USB_0_XCEIVER_ULPI_ENABLE       MASK_BIT0
#define USB_0_XCEIVER_ULPI_DISABLE      MASK_BIT1

#define USB_0_XCEIVER_ULPI_DDR_ENABLE   MASK_BIT2
#define USB_0_XCEIVER_ULPI_DDR_DISABLE  MASK_BIT3

#define USB_0_XCEIVER_XCLK_ENABLE       MASK_BIT5
#define USB_0_XCEIVER_XCLK_DISABLE      MASK_BIT6
#define USB_0_XCEIVER_I2C_OFF           MASK_BIT7
#endif

#define USB_0_XCEIVER_SRST              MASK_BIT4

typedef enum
{
    USB_SET,
    USB_CLEAR,
    USB_NO_CHANGE
} t_usb_set_or_clear;

typedef enum
{
    USB_HOST_TYPE,
    USB_DEVICE_TYPE
} t_usb_type;

typedef enum
{
    USB_A_DEVICE_MODE,
    USB_B_DEVICE_MODE
} t_usb_con_mode;

typedef enum
{
    USB_ABOVE_VBUSVALID,
    USB_ABOVE_AVALID,
    USB_ABOVE_SESSIONEND,
    USB_BELOW_SESSIONEND
} t_usb_vbus_level;


typedef enum
{
    USB_TEST_J                      = 0,
    USB_TEST_K,
    USB_TEST_SE0_NAK,
    USB_TEST_PACKET,
    USB_TEST_FORCE_ENABLE_HOST
} t_usb_dev_test_mode;

typedef struct
{
    t_bool  is_suspend_enabled;
    t_bool  is_reset_enabled;
    t_bool  is_overcurrent_enabled;
} t_usb_dev_port_status;

typedef struct
{
    t_bool      is_resume_enabled;
    t_bool      is_reset_enabled;
    t_bool      is_port_enabled;
    t_bool      is_device_connected;
    t_usb_speed connected_device_speed;
} t_usb_host_port_status;

typedef enum
{
    USB_DATA0                       = 0,
    USB_DATA1
} t_usb_host_data_toggle;

typedef struct
{
    t_usb_set_or_clear  resume_request;
    t_usb_set_or_clear  soft_connect_request;
} t_usb_dev_port_config;

typedef struct
{
    t_usb_set_or_clear  suspend_request;
    t_usb_set_or_clear  reset_request;
    t_usb_set_or_clear  port_enable;
    t_usb_set_or_clear  port_power;
} t_usb_host_port_config;

typedef struct
{
    t_usb_set_or_clear  srp_capable;
    t_usb_set_or_clear  hnp_capable;
    t_usb_set_or_clear  hnp_enable;
    t_usb_set_or_clear  session_request;
} t_usb_host_otg_config;

typedef struct
{
    t_usb_set_or_clear  srp_capable;
    t_usb_set_or_clear  hnp_capable;
    t_usb_set_or_clear  hnp_enable;
    t_usb_set_or_clear  hnp_request;
    t_usb_set_or_clear  session_request;
} t_usb_dev_otg_config;

typedef struct
{
    t_usb_con_mode      con_mode;
    t_usb_type          device_type;
    t_usb_vbus_level    vbus_level;
} t_usb_otg_status;

typedef struct
{
    t_bool              hnp_detected;
    t_bool              srp_detected;
    t_bool              is_connectorid_changed;
    t_usb_otg_status    otg_status;
} t_usb_host_otg_status;

typedef struct
{
    t_bool              hnp_success;
    t_bool              srp_success;
    t_usb_otg_status    otg_status;
} t_usb_dev_otg_status;

typedef t_uint32    t_usb_dev_endp_status;

typedef enum
{
    USB_DEV_IN_STATUS_ERROR         = MASK_BIT0,
    USB_DEV_IN_DATASENT             = MASK_BIT1,
    USB_DEV_IN_BELOWTHRESHOLD       = MASK_BIT2,
    USB_DEV_IN_NAKSENT              = MASK_BIT3,
    USB_DEV_IN_UNDERRUN             = MASK_BIT4,
    USB_DEV_IN_ISOTXDN              = MASK_BIT5,
    USB_DEV_IN_INCOMPTX             = MASK_BIT6,
    USB_DEV_IN_FIFONOTEMPTY         = MASK_BIT7,
    USB_DEV_IN_TXPCKRDY             = MASK_BIT8
} t_usb_dev_in_endp_status;

typedef enum
{
    USB_DEV_OUT_STATUS_ERROR        = MASK_BIT0,
    USB_DEV_OUT_LASTDATA            = MASK_BIT1,
    USB_DEV_OUT_ABOVETHRESHOLD      = MASK_BIT2,
    USB_DEV_OUT_OVERRUN             = MASK_BIT4,
    USB_DEV_OUT_INCOMPRX            = MASK_BIT5,
    USB_DEV_OUT_RXFIFOFULL          = MASK_BIT6,
    USB_DEV_OUT_RXPCKRDY            = MASK_BIT7
} t_usb_dev_out_endp_status;

typedef enum
{
    USB_CONTROL_XFER                = 0,
    USB_ISO_XFER,
    USB_BULK_XFER,
    USB_INTERRUPT_XFER
} t_usb_xfer_type;

typedef t_usb_xfer_type t_usb_endp_type;

typedef struct
{
    t_usb_endp_num  endp_num;
    t_usb_endp_type endp_type;
    t_uint16        maxpacketsize;
    t_bool          enable_double_buffer;
    t_bool          enable_multipacket;
    t_uint32        multiplier;
    t_uint32        fifo_size;
    t_bool          enable_nyet;
} t_usb_endp_config;


#define USB_SETUP_DATA_HOST_TO_DEVICE       0x00
#define USB_SETUP_DATA_DEVICE_TO_HOST       0x80
#define USB_SETUP_DATA_STANDARD_TYPE        0x00
#define USB_SETUP_DATA_CLASS_TYPE           0x20
#define USB_SETUP_DATA_VENDOR_TYPE          0x40
#define USB_SETUP_DATA_DEVICE_RECIPIENT     0x00
#define USB_SETUP_DATA_INTERFACE_RECIPIENT  0x01
#define USB_SETUP_DATA_ENDPOINT_RECIPIENT   0x02
#define USB_SETUP_DATA_OTHER_RECIPIENT      0x03

#define USB_GET_STATUS                      0
#define USB_CLEAR_FEATURE                   1
#define USB_SET_FEATURE                     3
#define USB_SET_ADDRESS                     5
#define USB_GET_DESCRIPTOR                  6
#define USB_SET_DESCRIPTOR                  7
#define USB_GET_CONFIGURATION               8
#define USB_SET_CONFIGURATION               9
#define USB_GET_INTERFACE                   10
#define USB_SET_INTERFACE                   11
#define USB_SYNCH_FRAME                     12

#define USB_DEVICE_DESC                     1
#define USB_CONF_DESC                       2
#define USB_STRING_DESC                     3
#define USB_INTERFACE_DESC                  4
#define USB_ENDPOINT_DESC                   5
#define USB_DEVICE_QUALIFIER_DESC           6
#define USB_OTHER_SPEED_CONF_DESC           7
#define USB_INTERFACE_POWER_DESC            8

#define USB_DEVICE_REMOTE_WAKEUP_FEATURE    1
#define USB_ENDPOINT_HALT_FEATURE           0
#define USB_TEST_MODE_FEATURE               2

#ifdef __HCL_USB_FB
typedef void (*t_usb_extra_func_ptr) (t_uint8 *p_usb_buffer, t_uint32 no_of_bytes, t_usb_endp_num endp); 
#endif

typedef struct
{
    t_bool          is_setup_good;
    t_bool          is_setup_received;
    t_bool          is_setup_after_out;
    t_bool          is_setup_end;
    t_usb_endp_num  endp_num;
} t_usb_setup_status;

typedef struct
{
    t_uint8     bRequestType;
    t_uint8     bRequest;
    t_uint8     wValueIndex;
    t_uint8     wValue;
    t_uint16    wIndex;
    t_uint16    wLength;
} t_usb_setup_request;

typedef enum
{
    USB_DMA_MODE,
    USB_IT_MODE,
    USB_POLLING_MODE
} t_usb_mode;

typedef enum
{
    USB_DMA_REQ_MODE_0,
    USB_DMA_REQ_MODE_1
} t_usb_dma_req_mode;

typedef enum
{
    USB_TRANSFER_ENDED,
    USB_WRITING_ENDED,
    USB_TRANSFER_ON_GOING,
    USB_TRANSFER_UNDERRUN_ERROR,
    USB_TRANSFER_OVERRUN_ERROR,
    USB_TRANSFER_OTHER_ERROR
} t_usb_xfer_status;

typedef struct
{
    t_uint8             *p_xfer_buffer;
    t_size              xfer_size;
    t_bool              send_zero_byte;
    t_usb_mode          xfer_mode;
    t_bool              auto_set_or_clear;
    t_usb_dma_req_mode  dma_req_mode;
} t_usb_dev_xfer_config;

#ifdef __HCL_USB_FB
/* 
  Register the Callback for extra data sent by host
*/
PUBLIC void USB_RegisterCallbackForExtraData(t_usb_extra_func_ptr usb_extra_fct);
#endif

/* 
  Get the Device Transfer Status 
*/
PUBLIC t_usb_error  USB_dev_GetTransferStatus(IN t_usb_endp_num endp_num, OUT t_usb_xfer_status * p_xfer_status);

/* 
  Flush the Tx FIFO 
*/
PUBLIC t_usb_error  USB_dev_FlushTransferFifo(IN t_usb_endp_num);

/* 
  Get the Number of Transfered Bytes 
*/
PUBLIC t_usb_error  USB_dev_GetTransferBytes(IN t_usb_endp_num endp_num, OUT t_size * p_bytes);

/* 
  Initiate Data Transfer -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_TransferInitiate(IN t_usb_endp_num endp_num, INOUT t_usb_dev_xfer_config xfer_config);

/* 
  Cancel Data Transfer -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_TransferCancel(IN t_usb_endp_num endp_num);

/* 
  Setup Iso Transfer -> Device Mode 
*/
PUBLIC void         USB_dev_SetupIsoTransfer(IN t_usb_endp_num endp_num);

/* 
  Complete Control Transfer (status stage) -> Device Mode 
*/
PUBLIC void         USB_dev_CompleteControlStatus(void);

/* 
  Complete Control Transfer (data stage) -> Device Mode 
*/
PUBLIC void         USB_dev_CompleteControlData(IN t_usb_endp_num);

/* 
  Set TxRx Ready for DMA -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_SetTxRxRdyForDma(IN t_usb_endp_num endp_num);

/* 
  To Enter into Test Mode -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_EnterTestMode(IN t_usb_dev_test_mode test_mode_selector);

/* 
  Check whether Rx is Ready  or not -> Device Mode 
*/
PUBLIC void         USB_dev_IsRxReadySet(t_bool *bRxReady);

/* 
  Set the Multiplier and FIFO Size 
*/
PUBLIC void         USB_SetMultiplierAndFifoSize(IN t_usb_dev_xfer_config xfer_config, INOUT t_usb_endp_config * endp_config);

/* 
  Get the Setup Status -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_GetSetupStatus(OUT t_usb_setup_status *);

/* 
  Get the Setup Data -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_GetSetupData(OUT t_usb_setup_request *);

/* 
  Set the Device Address 
*/
PUBLIC t_usb_error  USB_dev_SetAddress(IN t_uint8 address_value);

/* 
  Stall the Endpoint -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_SetEndpointStallState(IN t_usb_endp_num endp_num);

/* 
  Check Whether the Endpoint is stalled or not -> Device Mode 
*/
PUBLIC t_bool       USB_dev_IsEndpointStalled(IN t_usb_endp_num endp_num);

/* 
  Clear the Endpoint Stall State -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_ClearEndpointStallState(IN t_usb_endp_num endp_num);

/* 
  Configure the Endpoint -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_ConfigureEndpoint(IN t_usb_endp_config endp_config);

/* 
  Reset the Endpoint Configuration -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_ResetEndpointConfig(IN t_usb_endp_num endp_num);

/* 
  Set the Port Configuration -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_SetPortConfig(IN t_usb_dev_port_config);

/* 
  Get the Port Status -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_GetPortStatus(OUT t_usb_dev_port_status *p_dev_port_status);

/* 
  Reset the Data Toggling to default -> Device Mode 
*/
PUBLIC t_usb_error USB_dev_ResetToggleData(IN t_usb_endp_num endp_num);

/* 
  Set the OTG Configuration 
*/
PUBLIC t_usb_error  USB_dev_SetOtgConfig(IN t_usb_dev_otg_config);

/* 
  Get the OTG status 
*/
PUBLIC t_usb_error  USB_dev_GetOtgStatus(OUT t_usb_dev_otg_status *p_dev_otg_status);

/* 
  Get the Endpoint Status -> Device Mode 
*/
PUBLIC t_usb_error  USB_dev_GetEndpStatus(IN t_usb_endp_num endp_num, OUT t_usb_dev_endp_status * p_endp_status);

/* 
  Set Device Configuration 
*/
PUBLIC t_usb_error  USB_dev_SetConfiguration(IN t_usb_dev_config *usb_dev_config);

/* 
  Complete the Setup and Status Stage 
*/
PUBLIC t_usb_error USB_dev_CompleteSetupAndStatusStage(void);

/* 
  Complete the Setup Stage 
*/
PUBLIC t_usb_error USB_dev_CompleteSetupStage(void);


/*******************************************************************************************************************/
/* Host related structures, enums and APIs */
/*	Host Configuration.	*/
typedef struct
{
    t_usb_fifo_size fifosize;
    t_bool          enable_double_buffer;
}t_usb_fifo_config;

typedef struct
{
    t_usb_fifo_config fifoconfig[USB_MAX_NUM_ENDPOINTS];
    t_usb_speed     speed;
} t_usb_host_config;

typedef enum
{
    USB_HOST_IN                     = 9,
    USB_HOST_OUT                    = 1,
    USB_HOST_SETUP                  = 13
} t_usb_host_token_type;

typedef enum
{
    USB_HOST_ACK                    = 0,
    USB_HOST_CRCERR,
    USB_HOST_BITSTUFFERR,
    USB_HOST_DATATOGGLEERR,
    USB_HOST_STALL,
    USB_HOST_TIMEOUT,
    USB_HOST_PIDCHECKFAIL,
    USB_HOST_UNEXPECTEDPID,
    USB_HOST_DATAOVERRUN,
    USB_HOST_DATAUNDERRUN,
    USB_HOST_NAK,
    USB_HOST_FRAMEOVERRUN,
    USB_HOST_FRAMEUNDERRUN,
    USB_HOST_BUFFEROVERRUN,
    USB_HOST_BUFFERUNDERRUN
} t_usb_host_xfer_status;

typedef struct  t_xfer_config
{
    t_usb_host_token_type   token_type;
    t_usb_speed             speed;
    t_usb_xfer_type         xfer_type;
    t_usb_host_data_toggle  data_toggle;
    t_usb_endp_num          endp_num;
    t_uint8          target_ep_number;
    t_uint8                 token_addr;
    t_uint8                 hub_addr;
    t_uint8                 hub_port;
    t_uint16                maxpacketsize;
    t_usb_mode              xfer_mode;
    t_usb_dma_req_mode  dma_req_mode;
    t_bool		auto_set_clear;
    t_uint8                 dma_request_signal;
    t_uint8                 iso_int_xfer_interval;
    t_bool      ping_disable;
} t_usb_host_xfer_config;

/* 
  Flush the Transfer FIFO -> Host Mode 
*/
PUBLIC t_usb_error  USB_host_FlushTransferFifo(IN t_usb_endp_num endp_num);

/* 
  Inintiate the Transfer -> Host Mode 
*/
PUBLIC t_usb_error  USB_host_TransferInitiate(INOUT t_uint8 * p_buffer, IN t_size size, IN t_usb_host_xfer_config xfer_config);

/* 
  Set the Host Mode Configuration 
*/
PUBLIC t_usb_error  USB_host_SetConfiguration(IN t_usb_host_config *host_config);

/* 
  Get the last Transfer Status -> Host Mode 
*/
PUBLIC t_usb_error  USB_host_GetLastTransferStatus(IN t_usb_endp_num endp_num, OUT t_usb_host_xfer_status * p_xfer_status, OUT t_size * p_size);

/* 
  Set the OTG configuration 
*/
PUBLIC t_usb_error  USB_host_SetOtgConfig(IN t_usb_host_otg_config);

/* 
  Get the OTG status 
*/
PUBLIC t_usb_error  USB_host_GetOtgStatus(OUT t_usb_host_otg_status *p_host_otg_status);

/* 
  Cancel the Transfer -> Host Mode 
*/
PUBLIC t_usb_error USB_host_TransferCancel(IN t_usb_endp_num endp_num);

/* 
  Toggle the Data -> Host Mode 
*/
PUBLIC t_usb_host_data_toggle USB_host_GetToggleData(IN t_usb_endp_num endp_num);


/*
  Reset the Data Toggle
*/
PUBLIC t_usb_error USB_host_ResetToggleData(IN t_usb_endp_num endp_num);

/* 
  Set the Port Configuration -> Host Mode 
*/
PUBLIC t_usb_error  USB_host_SetPortConfig(IN t_usb_host_port_config);

/* 
  Get the Port Status -> Host Mode 
*/
PUBLIC t_usb_error  USB_host_GetPortStatus(OUT t_usb_host_port_status * p_host_port_status);

/* 
   Common APIs for both Host and device mode 
*/
/* 
  Initialize the USB HCL 
*/
PUBLIC t_usb_error  USB_Init(IN t_logical_address usb_dev_base_address);

/* 
  Get the HCL Version 
*/
PUBLIC t_usb_error  USB_GetVersion(t_version *p_version);

/* 
  Get the IRQ Status 
*/
PUBLIC void         USB_GetIRQSrcStatus(IN t_usb_irq_src irq_status, OUT t_usb_irq_status *p_status);

/* 
  Ackowledge the USB Event 
*/
PUBLIC void         USB_AcknowledgeEvent(IN t_usb_event dev_event);

/* 
  Check whether the given USB event is active or not 
*/
PUBLIC t_bool       USB_IsEventActive(IN t_usb_event dev_event);

/* 
  Filter the IRQ Source 
*/
PUBLIC t_usb_error  USB_FilterProcessIRQSrc(INOUT t_usb_irq_status * p_status, OUT t_usb_event * p_event, IN t_usb_filter_mode filter_mode);

/* 
  Save the System Context 
*/
PUBLIC void         USB_SaveSystemContext(void);

/* 
  Restore the System Context 
*/
PUBLIC void         USB_RestoreSystemContext(void);

/* 
  Update the ULPI Transceiver 
*/
PUBLIC void         USB_UpdateTransceiver(t_uint8 reg, t_uint8 value);

/* 
  Get the USB Device Status 
*/
PUBLIC t_usb_error  USB_GetDeviceStatus(OUT t_usb_status *p_dev_status);

/*
  Get number number of bytes in Rx USB FIFO
*/
PUBLIC t_uint32 usb_NrOfBytesInRxFIFO(IN t_usb_endp_num endp_num);

/*
  Public interface for reading data from the Rx FIFO
*/
PUBLIC void usb_ExternalReadFromFifo(IN t_usb_endp_num endp_num, OUT void *p_dest, IN t_size size);

#ifdef __DEBUG
PUBLIC t_usb_error  USB_SetDbgLevel(IN t_dbg_level debug_level);
PUBLIC t_usb_error  USB_GetDbgLevel(IN t_dbg_level *p_debug_level);
#endif /* __DEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _USB_H_ */


