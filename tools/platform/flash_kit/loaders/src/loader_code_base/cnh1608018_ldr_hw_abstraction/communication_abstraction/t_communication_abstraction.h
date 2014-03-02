/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_COMMUNICATION_ABSTRACTION_H_
#define _T_COMMUNICATION_ABSTRACTION_H_

/**
 * @addtogroup ldr_hw_abstraction
 * @{
 *   @addtogroup communication_abstraction Communication Abstraction
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "error_codes.h"
#include "t_communication_service.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** Declaration for the CommDev structure. */
struct CommDev_s;

/** Number of UART devices */
#define UART_DEVICES    4

/** Number of USB devices */
#define USB_DEVICES     1

/** Number of callback per device */
#define NO_CALLBACK_PER_DEVICE 2

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

#define CALLBACKS_PER_DEVICE 2

/** Device state. */
typedef enum {
    DEV_DOWN  = 0x0000001,  /**< The device is not started. */
    DEV_UP    = 0x0000002,  /**< The device is running. */
    DEV_ERROR = 0x0000004   /**< The device is in error state. */
} CommDevState_t;

/** Device type. */
typedef enum {
    DEV_UART0   = 0x0000000,  /**< The device is UART 0. */
    DEV_UART1   = 0x0000001,  /**< The device is UART 1. */
    DEV_UART2   = 0x0000002,  /**< The device is UART 2. */
    DEV_USB     = 0x0000004,  /**< The device is USB. */
    DEV_AAIF    = 0x0000008,  /**< The device is AAIF. */
    DEV_HSI     = 0x0000010,  /**< The device is HSI. */
    DEV_SDIO    = 0x0000020,  /**< The device is SDIO. */
    DEV_UNKNOWN = 0xFFFFFFF   /**< Unknown device. */
} CommDevId_t;

/** Structure for the device context type. */
typedef struct {
    void           *Data_p;    /**< Internal data used by the driver. */
    CommDevState_t  DevState;  /**< The device state. */
} CommDevContext_t;

/** Structure for the communication device callback function. */
#if defined(MACH_TYPE_DB5700)
typedef struct {
    /**< Is a pointer to a function that performs finalization on the device. */
    HL1UART_Callback_t RxCallback_Fn_p;
    /**< Is a pointer to a function that performs finalization on the device. */
    HL1UART_Callback_t TxCallback_Fn_p;
} UARTDevCallBack_t;

/** Structure for the communication device callback function. */
typedef struct {
    /**< Is a pointer to a function that performs finalization on the device. */
    HL1USB_Callback_t RxCallback_Fn_p;
    /**< Is a pointer to a function that performs finalization on the device. */
    HL1USB_Callback_t TxCallback_Fn_p;
} USBDevCallBack_t;

/** Structure for the HSI communication device callback functions. */
typedef struct {
    /**< Is a pointer to a function that performs reading finalization on the device. */
    HSI_BSC_TransferCompleteCallback_t RxCallback_Fn_p;
    /**< Is a pointer to a function that performs writing finalization on the device. */
    HSI_BSC_TransferCompleteCallback_t TxCallback_Fn_p;
} HSIDevCallback_t;

/** Structure for the HSI communication device error callback functions. */
typedef struct {
    /**< Is a pointer to a function that performs handling of error while reading on the device. */
    HSI_BSC_ErrorCallback_t RxErrorCallback_Fn_p;
    /**< Is a pointer to a function that performs handling of error while writing on the device. */
    HSI_BSC_ErrorCallback_t TxErrorCallback_Fn_p;
} HSIDevErrorCallback_t;

/** Structure for the SDIO communication device callback functions. */
typedef struct {
    /**< Is a pointer to a function that performs reading finalization on the device. */
    SDIO_BSC_TransferCompleteCallback_t RxCallback_Fn_p;
    /**< Is a pointer to a function that performs writing finalization on the device. */
    SDIO_BSC_TransferCompleteCallback_t TxCallback_Fn_p;
} SDIODevCallback_t;

/** Structure for the SDIO communication device error callback functions. */
typedef struct {
    /**< Is a pointer to a function that performs handling of error while reading on the device. */
    SDIO_BSC_ErrorCallback_t RxErrorCallback_Fn_p;
    /**< Is a pointer to a function that performs handling of error while writing on the device. */
    SDIO_BSC_ErrorCallback_t TxErrorCallback_Fn_p;
} SDIODevErrorCallback_t;
#endif

/** The initialization function for a device. */
typedef ErrorCode_e(*CommDevInit_Fn)(CommDevContext_t *Ctx_p,
                                     void *CallBack_p);

/** The shutdown function for a device. */
typedef void (*CommDevShutDown_Fn)(struct CommDev_s *Device_p,
                                   void *CommunicationDevice_p);

/** Structure for the device descriptor type. */
typedef struct {
    /**< Member contains value indicating the device type. */
    CommDevId_t        DevType;
    /**< Is a pointer to a function that initializes the device for use. */
    CommDevInit_Fn     Init_Fn_p;
    /**< Is a pointer to a function that performs finalization on the device. */
    CommDevShutDown_Fn ShutDown_Fn_p;
    /**< Is a pointer to a function that performs finalization on the device. */
    void              *RxCallback_Fn_p;
    /**< Is a pointer to a function that performs finalization on the device. */
    void              *TxCallback_Fn_p;
    /**< Is a pointer to the device internal data structure holding the device parameters. */
    void              *Parameters;
} CommDevDescr_t;

/** Device structure. Contains the instance, state and peer address of
    a device. */
typedef struct CommDev_s {
    CommDevDescr_t   *Descriptor_p; /** The device descriptor. */
    CommDevContext_t Context;       /** The device context. */
} CommDev_t;

typedef struct {
    CommDev_t *Device_p;                  /**< Initialized device. */
    CommunicationCallback_t TxCallback_fn;/**< Callback function for this device
                                             and this read/write instance. */
    CommunicationCallback_t RxCallback_fn;/**< Callback function for this device
                                             and this read/write instance. */
    void *Param_p;                        /**< Context field. */
    // Might be needed for the moment in FAIRBANKS
    uint32 TxBytes;                       /**< How many bytes are to be sent
                                             //in next writes */
    uint32 RxBytes;                       /**< How many bytes are to be read */
    uint32 BytesToSend;                   /**< How many bytes are to be sent
                                             in next writes */
} CallbackEntry;

/** @} */
/** @} */
#endif /*_T_COMMUNICATION_ABSTRACTION_H_*/
