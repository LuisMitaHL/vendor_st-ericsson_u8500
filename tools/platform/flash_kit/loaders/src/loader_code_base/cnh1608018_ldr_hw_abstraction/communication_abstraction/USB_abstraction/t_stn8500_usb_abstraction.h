/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_USB_ABSTRACTION_H_
#define _T_USB_ABSTRACTION_H_

/**
 * @addtogroup ldr_hw_abstraction
 * @{
 *   @addtogroup USB_abstraction
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_communication_abstraction.h"
#include "usb.h"
#include "usb_services.h"
#include "t_r15_network_layer.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/**
 *  This is the driver function to be called to start receiving data.
 */
typedef boolean(*DriverUSBRxNoWait_t)(uint8 *const Data_p,
                                      const uint32 NrBytes);

/**
 *  This is the driver function to be called to check if the data has been
 *  received.
 */
typedef uint32(*DriverUSBCheckRxLeft_t)(void);

/**
 *  This is the driver function to be called to start sending data.
 */
typedef boolean(*DriverUSBTxNoWait_t)(uint8 *const Data_p,
                                      const uint32 NrBytes);

/**
 *  This is the driver function to be called to check if the data has been sent.
 */
typedef uint32(*DriverUSBCheckTxLeft_t)(void);


/**
 *  Declaration of USB driver data type.
 */
typedef struct {
    DriverUSBRxNoWait_t    RxNoWait_p;
    DriverUSBCheckRxLeft_t CheckRxLeft_p;
    DriverUSBTxNoWait_t    TxNoWait_p;
    DriverUSBCheckTxLeft_t CheckTxLeft_p;
} USB_Driver_Data_t;

/**
 *  Declarations of USB internal data type.
 */
typedef struct {
    PacketMeta_t     *Buffer_p;
    uint32            Length;
    uint32            Received;
} USB_RxData_t;

typedef struct {
    PacketMeta_t    *Buffer_p;
    uint32           Sending;
} USB_TxData_t;

typedef struct {
    boolean             HighSpeed;  /**<Is USB high speed used. */
    USB_Driver_Data_t   USBDriver;  /**< The driver data. */
    USB_RxData_t        Rx;
    USB_TxData_t        Tx;
    void               *IntData_p;  /**< Internal data used by the driver. */
} USB_InternalData_t;

typedef struct {
    /**< Is a pointer to a function that performs finalization on the device. */
    t_callback_fct USB_Callback_Fn_p;
} USBDevCallBack_t;

/** @} */
/** @} */
#endif /*_T_USB_ABSTRACTION_H_*/
