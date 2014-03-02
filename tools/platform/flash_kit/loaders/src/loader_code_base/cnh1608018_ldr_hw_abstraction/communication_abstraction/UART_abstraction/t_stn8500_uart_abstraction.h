/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef _T_UART_ABSTRACTION_H_
#define _T_UART_ABSTRACTION_H_

/**
 * @addtogroup ldr_hw_abstraction
 * @{
 *   @addtogroup UART_abstraction
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "uart_services.h"
#include "uart_irqp.h"
#include "t_communication_abstraction.h"
#include "t_r15_network_layer.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/**
 *  This is the driver function to be called to start receiving data.
 */
typedef boolean(*DriverRxNoWait_t)(uint8 *const Data_p,
                                   const uint32 NrBytes,
                                   const t_uart_device_id UDev);

/**
 *  This is the driver function to be called to check if the data has
 *  been received.
 */
typedef uint32(*DriverCheckRxLeft_t)(const t_uart_device_id UDev);

/**
 *  This is the driver function to be called to start sending data.
 */
typedef boolean(*DriverTxNoWait_t)(uint8 const *const Data_p,
                                   const uint32 NrBytes,
                                   const t_uart_device_id UDev);

/**
 *  This is the driver function to be called to check if the data has been sent.
 */
typedef uint32(*DriverCheckTxLeft_t)(const t_uart_device_id UDev);

/**
 * Declaration of UART driver data type
 */
typedef struct {
    DriverRxNoWait_t    RxNoWait_p;
    DriverCheckRxLeft_t CheckRxLeft_p;
    DriverTxNoWait_t    TxNoWait_p;
    DriverCheckTxLeft_t CheckTxLeft_p;
} UART_Driver_Data_t;


/**
 * Declarations of UART internal data type
 */
typedef struct {
    PacketMeta_t     *Buffer_p;
    uint32            Length;
    uint32            Received;
} UART_RxData_t;

typedef struct {
    PacketMeta_t    *Buffer_p;
    uint32           Sending;
} UART_TxData_t;

typedef struct {
    t_uart_device_id     Device;        /**< The UART device used. */
    t_uart_baud_rate     Speed;         /**< The requested baud rate. */
    UART_Driver_Data_t   UARTDriver;    /**< The driver data. */
    UART_RxData_t        Rx;
    UART_TxData_t        Tx;
    void                *IntData_p;     /**< Internal data used by the driver. */
    boolean              Debug_Port;    /**< Set to TRUE if UART is used for debug printouts. */
} UART_InternalData_t;

typedef struct {
    /**< Is a pointer to a function that performs finalization on the device. */
    t_callback_fct UART_Callback_Fn_p;
} UARTDevCallBack_t;

/** @} */
/** @} */
#endif /*_T_UART_ABSTRACTION_H_*/
