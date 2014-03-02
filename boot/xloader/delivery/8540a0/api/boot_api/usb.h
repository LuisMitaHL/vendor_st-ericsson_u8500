/*-----------------------------------------------------------------------------
 * ST Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST Ericsson.
 *---------------------------------------------------------------------------*/
/** \addtogroup USB
 * @{
 */ 
/*---------------------------------------------------------------------------*/
/** \file usb.h
 *  \author ST Ericsson
 *  \brief This file holds the public functions and data structures used by ROM code
 */
/*---------------------------------------------------------------------------*/
#ifndef _USB_H_
#define _USB_H_

#include "boot_types.h"
#include "dma.h"

#define USB_CUST_PUB_ROM_RET_OK     0x00000001   /**< (0x00000001)  */
#define USB_CUST_PUB_ROM_RET_FAIL   0x80000000   /**< (0x80000000)  */

/** This type stores the SUB callback function address */
typedef void (*t_pUsbCallBack) (t_uint32);

/** USB event given as parameter to event callback */
typedef enum{
USB_EVENT_SUSPEND=1, /**< (1), suspend interrupt received*/
USB_EVENT_RESUME=2,  /**< (2), resume interrupt received*/
USB_EVENT_RESET=3    /**< (3), reset interrupt received*/
}t_usb_event;

/** This type stores the status given as input to callback enum */
typedef enum{
USB_CONNECT_OK,   /**< (0), enum done (get configuration descriptor received)*/
USB_CONNECT_TIMEOUT /**< (1), timeout (not used) */
}t_usb_cust_enum_status;

/** Mode of the USB transfer */
typedef enum{
USB_POLLING=0,   /**< (0), polling */
USB_DMA=1   /**< (1), DMA */
}t_usb_mode;

/** Direction of the USB transfer */ 
typedef enum{
USB_TX=0,    /**< (0), transmit */
USB_RX=1     /**< (1), receive */
}t_usb_dir;

/** \internal
 This type stores the different vendor descriptor available for Nomadik */
typedef enum{
ST_DESCRIPTOR,   /**< \internal (0), ST */
NOKIA_DESCRIPTOR /**< \internal (1), Nokia */
}t_vendor_desc;

/** Structure that contains descriptor information */
typedef struct{
    t_uint32  descAddr;   /**< Address of the descriptor. If null: not used. */
    t_uint32  descSize; /**< Size of the descriptor. */
}t_usb_descriptor;

/** Structure that list all the descriptor that can be changed in USB_init(). */
typedef struct{
    t_usb_descriptor*  p_device;   /**< Pointer on device descriptor info. */
    t_usb_descriptor*  p_devQualifier;   /**< Pointer on device qualifier descriptor info. */
    t_usb_descriptor*  p_config;   /**< Pointer on configuration descriptor info 
                                        (contain also interface and endpoint descriptor). */
    t_usb_descriptor*  p_otherSpeed;   /**< Pointer on other speed configuration descriptor info. */
    t_usb_descriptor*  p_prodString;   /**< Pointer on product string descriptor info. */
}t_usb_descriptor_list;

/** Structure filled by USB_GetStatus(). */
typedef struct{
    t_bool isHighSpeedSession; /**< TRUE if HighSpeed has been successfully negociated,
                                    FALSE if FullSpeed*/
    t_bool isConfiguredState; /**< TRUE if state is configured */
    t_uint8 FctAddress; /**< Fonction address given by the Host when in address state */
    t_uint8 padding;   /**< unused */
} t_usb_status;

/** Number of endpoint   */
typedef enum {
    USB_EP0_NB,    /**< 0 */
    USB_EP1_NB,    /**< 1 */
    USB_EP2_NB,    /**< 2 */
    USB_EP3_NB,    /**< 3 */
    USB_EP4_NB,    /**< 4 */
    USB_EP5_NB,    /**< 5 */
    USB_EP6_NB,    /**< 6 */
    USB_EP7_NB,    /**< 7 */
    USB_EP8_NB,    /**< 8 */
    USB_EP9_NB,    /**< 9 */
    USB_EP10_NB,   /**< 10 */
    USB_EP11_NB,   /**< 11 */
    USB_EP12_NB,   /**< 12 */
    USB_EP13_NB,   /**< 13 */
    USB_EP14_NB,   /**< 14 */
    USB_EP15_NB    /**< 15 */
}t_endp_num;

/** EndPoint direction */
typedef enum {
    TX_DIR, /**< 0, TX direction */
    RX_DIR  /**< 1 RX direction */
}t_endp_dir;

/** Endpoint configuration */
typedef struct{
    t_endp_num      EpNb;           /**< End point number (0-15)  */
    t_endp_dir      Direction;      /**< End point direction: TX or RX. */
    t_uint16        MaxPacketSize;  /**< Maximum packet size      */
    t_uint16        FifoSize;       /**< Size of the RX/TX Fifo (must be at least 1 packet size) */
}t_endpoint_config;

/** Defines INTUSB register possible values (received usb system interrupt).*/
typedef union{
    struct {                 
            t_bitfield Susp:1;  /**< Suspend it received*/
            t_bitfield Res:1;   /**< Resume it received*/                
            t_bitfield Rst:1;   /**< Reset it received*/
            t_bitfield Sof:1;   /**< Start of Frame it received*/
            t_bitfield Conn:1;  /**< Connect it received*/
            t_bitfield Discon:1;/**< Disconnect it received*/
            t_bitfield SReq:1;  /**< Session request it received*/
            t_bitfield VbE:1;   /**< Vbus error it received*/
            }Bit;
    t_uint8 Reg;    
}t_usb_sys_it;

/** EndPoint direction */
typedef enum {
    USB_EP_0=0, /**< 0, EP0 */
    USB_EP_TX =1, /**< 1 TX EP */
    USB_EP_TX_MAX =2 /**< number of EP used in TX*/
}t_usb_ep_tx_index;

/** \internal EndPoint direction */
typedef enum {
    USB_DEVICE_DESC_SIZE_INDEX  = 0,        /**< \internal 0, size of the descriptor in bytes */
    USB_DEVICE_DESC_TYPE_INDEX  = 1,        /**< \internal 1 Descriptor type for DEVICE */
    USB_DEVICE_DESC_SPEC_REL_LSB_INDEX  = 2, /**< \internal 2 USB specification release : version 2.0 */
    USB_DEVICE_DESC_SPEC_REL_MSB_INDEX  = 3, /**< \internal 3 USB specification release : version 2.0 */
    USB_DEVICE_DESC_CLASS_INDEX  = 4,        /**< \internal 4, Device class    */
    USB_DEVICE_DESC_SUBCLASSE_INDEX  = 5,    /**< \internal 5 Device sub class */
    USB_DEVICE_DESC_PROTOCOL_INDEX  = 6,     /**< \internal 6 Device protocol  */
    USB_DEVICE_DESC_EP0_MAX_PACKET_INDEX  = 7, /**< \internal 7, Max packet size for endpoint 0, 64 bytes */
    USB_DEVICE_DESC_VID_LSB_INDEX  = 8,      /**< \internal 8 vendor ID lsb  */
    USB_DEVICE_DESC_VID_MSB_INDEX  = 9,      /**< \internal 9 vendor ID msb  */
    USB_DEVICE_DESC_PID_LSB_INDEX  = 10,     /**< \internal 10 PRODUCT ID lsb  */
    USB_DEVICE_DESC_PID_MSB_INDEX  = 11,     /**< \internal 11 PRODUCT ID msb  */
    USB_DEVICE_DESC_DEVICEID_LSB_INDEX  = 12,/**< \internal 12 Device release number lsb  */
    USB_DEVICE_DESC_DEVICEID_MSB_INDEX  = 13,/**< \internal 13 Device release number msb  */
    USB_DEVICE_DESC_MANU_STRING_INDEX  = 14,     /**< \internal 10 Index of manufacturer string descriptor   */
    USB_DEVICE_DESC_PROD_STRING_INDEX  = 15,     /**< \internal 11 Index of product string descriptor  */
    USB_DEVICE_DESC_SERIALNB_STRING_INDEX  = 16,/**< \internal 12 Index of serial number string descriptor  */
    USB_DEVICE_DESC_CONFIG_NB_INDEX  = 17,/**< \internal 13 Number of possible configuration  */
    USB_DEVICE_DESC_LENGTH  =18      /**< \internal size of the desriptor in byte */
}t_usb_device_desc_index;

typedef struct
{
    t_uint8 field[USB_DEVICE_DESC_LENGTH] ;
}t_usb_device_desc;

/** USB context */
typedef struct
{
    t_bool      SetAddress;    /* set to TRUE when the request SET_ADDRESS is received */
    t_uint8     FctAddress;    /* USB device address affected by the host           */
    t_bool      ConfigReceived; /* Set when config is received (GET/SET_CONFIGURATION)*/
    t_endp_num   ReceiveEp;         /* EndPoint use to receive Data */
    t_endp_num   SendEp;            /* EndPoint use to Send Data */
    t_uint32*   p_DataReceived;     /* point where the received data should be copied */
    t_uint32    NbBytesReceived;    /* Number of received data                      */
    t_uint32    NbBytesRequired;    /* Number of data that must be received         */
    t_uint32    OffsetFifo;         /* use to set the fifo address                  */
    t_uint16    ConfigValue;        /* Device configured state : by default set to 0 */
    t_bool      isUserConfDesc;     /* Set when the configuration descriptor to use
                                     is the user one not the rom default */
    t_bool      isUserOtherConfDesc;/* Set when the other speed configuration
                                         descriptor to use is the user one */
    t_uint32*   p_DataToSend[USB_EP_TX_MAX];       /* point to the data to send for EP0 [0] and TX EP [1]  */
    t_uint32    NbByteToSend[USB_EP_TX_MAX];       /* Nb of bytes to send  for EP0 [0] and TX EP [1]   */
    t_uint32    NbByteSent[USB_EP_TX_MAX];         /* Nb of bytes sent for EP0 [0] and TX EP [1]   */
    t_bool      isEpHalted[USB_EP15_NB+1]; /* Indicate if current EP is halted or not  */
    t_pUsbCallBack usbRxCallback;     /* Callback function used at end of receive transfer */
    t_pUsbCallBack usbTxCallback;    /* Callback function used at end of transmit transfer */
    t_pUsbCallBack usbEnumCallback;    /* Callback function used at end of enumeration */
    t_pUsbCallBack usbExtEventCallback; /* Callback function used for resume, suspend end reset interrupt*/    
    t_uint32    dmaChannel;         /* DMA channel use for DMA transfer */
    t_dma_std_channel_desc*   p_dmaLli;  /* pointer to the LLI to be used by the DMA */
    t_uint32    lliMaxSize;         /* Max size for LLI with USB  */
    t_uint32    RemainBytesAfterDma;  /* Bytes te get by hand after DMA transfer */ 
    t_address   LastAddrAfterDma;    /* Addr to write remain bytes afte DMA transfer */
    t_uint32    NbLliProg;   /* Nb of Lli programmed for the DMA transfer */
    t_usb_dma_status isUsbDmaTransferFinished;  /* status of DMA transfer */
    t_usb_mode  CustRxMode;   /* Define polling or dma mode for customer receive function*/
    t_usb_mode  CustTxMode;   /* Define polling or dma mode for customer send function*/
    t_usb_device_desc DeviceDesc; /* default ROM Device descriptor used during enumeration */
}t_usb_ctxt;


/** USB enum aligned with UsbStringSelect fuse value 
used to define the string descriptor and default device descriptor to be used  */
typedef enum {
    USB_STRING_DESC_DEFAULT1 = 0,   /**< Select default1 string (generic) and device desc */
    USB_STRING_DESC_DEFAULT2 = 1,   /**< Select default2 string (7400) and device desc */   
    USB_STRING_DESC_CUST = 2,       /**< Select cust reserved string and device desc, unused on 7400 */
    USB_STRING_DESC_EMPTY =3        /**< Select empty string and device desc */
}t_usb_string_desc;

/** USB structure that contain usb init conf  */
typedef struct{
    volatile t_usb_ctxt * p_UsbCtxt; /**< if not null, pointer on the USB context global */
    t_uint16  VID;   /**< if =0: unused -> use UsbStringSelect default conf
                          or use boolean to know if valid or not */
    t_uint16  PID;   /**< PID to use if VID is valid */
    t_usb_string_desc  UsbStringSelect;   /**< string descr to use (+default VID/PID if VID not valid)  */
    t_uint8  unused;   /**< ??? */
}t_usb_type;


PUBLIC void USB_Init(t_usb_type * , t_usb_descriptor_list*  );
PUBLIC t_uint32 USB_CustInitialize(void );
PUBLIC t_uint32 USB_CustConnect(t_usb_descriptor_list* , t_pUsbCallBack );
PUBLIC t_usb_sys_it USB_ItProcess(void);
PUBLIC void USB_ReceiveData( t_size    nbBytes,
                             t_address dataAddr,
                             t_dma_use dmaUse,
                             t_pUsbCallBack usbCallback
                                );
PUBLIC void USB_IsConfigReceived(t_bool *);
PUBLIC void USB_SendAsicId(t_boot_asic_id_desc  *);
PUBLIC void USB_TransmitData(t_endp_num, t_address, t_size);
PUBLIC void USB_GetNbBytesReceived( t_uint32*);
PUBLIC void USB_SetConfiguration( void );
PUBLIC t_bool USB_IsSessionValid(void);
PUBLIC void USB_Disconnect( void );
PUBLIC t_uint32 USB_CustDisconnect( void );
PUBLIC void USB_Close( void );
PUBLIC t_uint32 USB_CustClose( void );
PUBLIC void USB_GetStatus( t_usb_status * p_usb_status );
PUBLIC void USB_InitEndpoint(t_endpoint_config* p_endpConfig);
PUBLIC void USB_HandleDmaEnd (t_boot_error error, t_uint32 bytesRec);
PUBLIC t_uint32 USB_CustWriteData(t_size nbBytes, t_address dataAddr, t_pUsbCallBack p_usbCallback);
PUBLIC void USB_GetNbBytesSent( t_uint32* p_nbBytesWritten);
PUBLIC void USB_ReadLliConf( t_address *p_lliAreaAddr, t_uint32 *p_lliMaxSize);
PUBLIC t_boot_error USB_ChangeLliConf( t_address lliAreaAddr, t_uint32 lliMaxSize);
PUBLIC t_uint32 USB_CustSetExtEventHandler(t_pUsbCallBack p_extEventHandler);
PUBLIC t_uint32 USB_CustReadData(t_size nbBytes, t_address dataAddr, t_pUsbCallBack p_usbCallback);
PUBLIC t_uint32 USB_CustSetTransferMode(t_usb_mode mode, t_usb_dir direction);

#endif /* _USB_H_ */
/* end of usb.h */
/** @} */

