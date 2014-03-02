/*****************************************************************************/
/**
*  � ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief This file holds the private functions and data structures used by HCL.
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _USB_P_H_
#define _USB_P_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define USB_DEV_DEFAULT_THRESHOLD_SIZE  256
#define USB_DEV_EP0_DEFAULT_MAXPCKSIZE  64
#define USB_MAX_TRANSFER_SIZE           1024

#define USB_EP0_DEFAULT_FIFOSIZE        64
#define USB_INVALID_FIFO_SIZE           0xFF

#ifdef USB_FIFO_MGMT
#define USB_COUNT1                      0x1
#define USB_COUNT2                      0x3
#define USB_COUNT3                      0x7
#define USB_COUNT4                      0xF
#define USB_COUNT5                      0x1F
#define USB_COUNT6                      0x3F
#define USB_COUNT7                      0x7F
#define USB_COUNT8                      0xFF
#define USB_COUNT9                      0x1FF
#define USB_COUNT10                     0x3FF
#define USB_COUNT11                     0x7FF
#define USB_COUNT12                     0xFFF
#define USB_COUNT13                     0x1FFF
#define USB_COUNT14                     0x3FFF
#define USB_COUNT15                     0x7FFF
#define USB_COUNT16                     0xFFFF
#define USB_COUNT17                     0x1FFFF
#define USB_COUNT18                     0x3FFFF
#define USB_COUNT19                     0x7FFFF
#define USB_COUNT20                     0xFFFFF
#define USB_COUNT21                     0x1FFFFF
#define USB_COUNT22                     0x3FFFFF
#define USB_COUNT23                     0x7FFFFF
#define USB_COUNT24                     0xFFFFFF
#define USB_COUNT25                     0x1FFFFFF
#define USB_COUNT26                     0x3FFFFFF
#define USB_COUNT27                     0x7FFFFFF
#define USB_COUNT28                     0xFFFFFFF
#define USB_COUNT29                     0x1FFFFFFF
#define USB_COUNT30                     0x3FFFFFFF
#define USB_COUNT31                     0x7FFFFFFF
#define USB_COUNT32                     0xFFFFFFFF
#endif

typedef enum
{
    USBOTG_HOST_MODE,
    USBOTG_DEVICE_MODE,
    USBOTG_OTG_MODE
} t_usb_otg_mode;
 

typedef struct
{
    t_uint8             *p_xfer_buffer;

#ifdef __HCL_USB_FB
    t_uint8             extra_xfer_buffer[512];
    t_uint32            overflow_bytes;
#endif

    t_uint32            xfer_length;
    t_uint32            len_transferred;
    t_usb_xfer_status   xfer_status;
    t_usb_endp_type     endp_type;
    t_uint16            maxpacketsize;
    t_bool              is_endpoint_configured;
    t_usb_mode          xfer_mode;
    t_usb_dma_req_mode  dma_req_mode;
    t_bool              send_zero_byte;
    t_bool              enable_double_buffer;
    t_bool              enable_multipacket;
    t_usb_fifo_size     fifosize;
    t_uint32            remaining_bytes;
    t_uint32            multiplier;
    t_bool              auto_set_or_clear;
    t_bool		enable_nyet;
} t_usb_dev_endpoint;

typedef struct
{
    t_uint8                 *p_rx_buffer;
    t_bool                  enable_double_buffer;
    t_size                  rx_length;
    t_usb_mode              rx_mode;
    t_usb_host_xfer_status  xfer_status;
} t_usb_host_endpoint;

typedef struct
{
    t_usb_dev_endpoint      dev_endpoint[USB_MAX_NUM_ENDPOINTS];
    t_usb_host_endpoint     host_endpoint[USB_MAX_NUM_ENDPOINTS];
    t_logical_address       usb_base_address;   /* Base Addresses. */
    t_usb_0_register        *p_usb_0_register;
    t_usb_host_token_type   last_transferred_token;
    t_usb_otg_mode          usb_otg_mode;
    t_usb_event             usb_event;
    t_bool                  set_config_intr_received;
    t_bool                  set_interface_intr_received;
    t_usb_setup_request     setup_data;
    t_bool                  is_device_connected;

#ifdef ST_8500ED
    t_uint32                usb_save_device_0_context[31];
#else
    t_uint32                usb_save_device_0_context[29];
#endif

#ifdef __HCL_USB_FB
    t_usb_extra_func_ptr extra_xfer_callback;
#endif

#ifdef USB_FIFO_MGMT
t_uint32 fifo_pool[6];
t_uint8 bit_number[USB_MAX_NUM_ENDPOINTS];
t_uint32 chunk_count[USB_MAX_NUM_ENDPOINTS];
#endif

} t_usb_system_context;

/*
  Process the USB Interrupt
*/
PRIVATE void        usb_ProcessIt(IN t_usb_irq_src *p_status);

/*
  Handler for IN Endpoint Interrupt -> Device Mode
*/
PRIVATE void        usb_dev_handle_in_endp_intr(IN t_usb_endp_num endp_num);

/*
  Handler OUT IN Endpoint Interrupt -> Device Mode
*/
PRIVATE void        usb_dev_handle_out_endp_intr(IN t_usb_endp_num endp_num);

/*
  Handler for Endpoint0 Interrupt
*/
PRIVATE void        usb_dev_handle_endp0_intr(void);

/*
  Read from FIFO correponding to the given Endpoint
*/
PRIVATE void        usb_ReadFromFifo(t_uint8 endp_num, void *p_dest, t_size size);

/*
  Write into FIFO correponding to the given Endpoint
*/
PRIVATE void        usb_WriteToFifo(t_uint8 endp_num, void *p_src, t_size size, t_usb_type usb_type);

/*
  Handler for Status Interrupt -> Host Mode
*/
PRIVATE void        usb_host_handle_status_intr(IN t_usb_endp_num endp_num);

/*
  Get the USB FIFO Size
*/
PRIVATE t_uint8    usb_GetFifoSize(t_usb_fifo_size fifosize);

/*
  Update the function details -> Host Mode
*/
PRIVATE void        usb_host_UpdateFunctionDetails(t_uint8 token_addr, t_uint8 hub_addr, t_uint8 hub_port, t_usb_endp_num endp_num);


/*
  Get FIFO address corresponding to the given Endpoint
*/
PRIVATE t_uint32    *usb_getFifoAddress(t_uint8 endp_num_short);

PRIVATE void        USB_dev_OutEndp_TransferInitiate
                    (
                        IN t_usb_endp_num           endp_num,
                        INOUT t_usb_dev_xfer_config xfer_config
                    );
PRIVATE void        USB_dev_InEndp_TransferInitiate
                    (
                        IN t_usb_endp_num           endp_num,
                        INOUT t_usb_dev_xfer_config xfer_config
                    );

PRIVATE void        USB_dev_OutEndp_IsoTransferInitiate
                    (
                        IN t_usb_endp_num           endp_num,
                        INOUT t_usb_dev_xfer_config xfer_config
                    );

PRIVATE void        USB_dev_OutEndp_BulkIntrTransferInitiate
                    (
                        IN t_usb_endp_num           endp_num,
                        INOUT t_usb_dev_xfer_config xfer_config
                    );

PRIVATE void        USB_dev_OutEndp_BulkDMATransferInitiate
                    (
                        IN t_usb_endp_num           endp_num,
                        INOUT t_usb_dev_xfer_config xfer_config
                    );

PRIVATE void        USB_dev_InEndp_IsoTransferInitiate
                    (
                        IN t_usb_endp_num           endp_num,
                        INOUT t_usb_dev_xfer_config xfer_config
                    );

PRIVATE void        USB_dev_InEndp_BulkIntrTransferInitiate
                    (
                        IN t_usb_endp_num           endp_num,
                        INOUT t_usb_dev_xfer_config xfer_config
                    );

PRIVATE void        USB_dev_InEndp_BulkDMATransferInitiate
                    (
                        IN t_usb_endp_num           endp_num,
                        INOUT t_usb_dev_xfer_config xfer_config
                    );

PRIVATE void        USB_GetUSBSysIRQSrc(INOUT t_usb_irq_src *p_status, IN t_usb_event *p_event);

PRIVATE void        USB_GetUSBInEndpIRQSrc(INOUT t_usb_irq_src *p_status, IN t_usb_event *p_event);

PRIVATE void        USB_GetUSBOutEndpIRQSrc(INOUT t_usb_irq_src *p_status, IN t_usb_event *p_event);

PRIVATE t_usb_error USB_FilterProcessIRQErrorCheck
                    (
                        IN t_usb_irq_status     *p_status,
                        IN t_usb_filter_mode    *p_filter_mode,
                        INOUT t_usb_event       *p_event
                    );

PRIVATE void        USB_GetSysIRQSrcStatus(IN t_uint32 misc, OUT t_usb_irq_status *p_status, t_bool is_host);
PRIVATE void        USB_GetInEndpIRQSrcStatus(IN t_uint32 misc, OUT t_usb_irq_status *p_status, t_bool is_host);
PRIVATE void        USB_GetOutEndpIRQSrcStatus(IN t_uint32 misc, OUT t_usb_irq_status *p_status, t_bool is_host);

PRIVATE void        USB_GetInEndpIRQStatus(IN t_uint32 misc, OUT t_usb_irq_status *p_status);
PRIVATE void        USB_GetOutEndpIRQStatus(IN t_uint32 misc, OUT t_usb_irq_status *p_status);

PRIVATE void        USB_host_Endp0_TransferInitiate
                    (
                        INOUT t_uint8               *p_buffer,
                        IN t_size                   size,
                        IN t_usb_host_xfer_config   *p_xfer_config,
                        IN t_uint32                 speed_mask,
                        IN t_uint8                  endp_num_short
                    );

PRIVATE void        USB_host_Out_TransferInitiate
                    (
                        INOUT t_uint8               *p_buffer,
                        IN t_size                   size,
                        IN t_usb_host_xfer_config   *p_xfer_config,
                        IN t_uint32                 speed_mask,
                        IN t_uint8                  endp_num_short
                    );

PRIVATE void        USB_host_In_TransferInitiate
                    (
                        INOUT t_uint8               *p_buffer,
                        IN t_size                   size,
                        IN t_usb_host_xfer_config   *p_xfer_config,
                        IN t_uint32                 speed_mask,
                        IN t_uint8                  endp_num_short
                    );

PRIVATE void        usb_host_Out_Endp_UpdateFunctionDetails
                    (
                        IN t_uint8          token_addr,
                        IN t_uint8          hub_addr,
                        IN t_uint8          hub_port,
                        IN t_usb_endp_num   endp_num
                    );

PRIVATE void        usb_host_In_Endp_UpdateFunctionDetails
                    (
                        IN t_uint8          token_addr,
                        IN t_uint8          hub_addr,
                        IN t_uint8          hub_port,
                        IN t_usb_endp_num   endp_num
                    );

PRIVATE void        usb_host_handle_status_in_endp0_intr(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short);
PRIVATE void        usb_host_handle_status_out_endp0_intr(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short);
PRIVATE void        usb_host_handle_status_in_endp_intr(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short);
PRIVATE void        usb_host_handle_status_out_endp_intr(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short);

void                usb_ProcessEndp0It(t_bool is_host);

PRIVATE void        usb_dev_handle_in_endp_intr_itmode(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short);

#ifdef USB_FIFO_MGMT
PRIVATE t_uint32 usb_GetMask(t_uint32 chunk_count);
PRIVATE t_uint8  usb_GetOffsetCount(t_uint32 no_of_chunks);
PRIVATE void usb_ClearBufferBits(t_uint8 bit_number,t_uint8 no_of_bits);
PRIVATE t_usb_error usb_DeallocateFifoBuffer(t_usb_endp_num endp_num);
PRIVATE t_usb_error usb_AllocateFifoBuffer(t_usb_endp_num endp_num);
PRIVATE t_usb_error usb_UpdateFifosEps(void);
#else
/*
  Update all the Endpoint FIFOs
*/
PRIVATE t_usb_error  usb_UpdateFifos(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _USB_P_H_ */
