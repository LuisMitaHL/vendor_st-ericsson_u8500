/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_COMM_DEVICES_H_
#define _T_COMM_DEVICES_H_

/**
 * @addtogroup ldr_hw_abstraction
 * @{
 *   @addtogroup comm_devices
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_communication_abstraction.h"
#include "uart.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 * Parameter for external communication devices
 */
typedef struct {
    CommDevId_t         Device;  /**< UART Channel. */
    t_uart_baud_rate    Speed;   /**< UART Baud rate. */
    boolean             HiSpeed; /**< USB speed; HiSpead = true, LowSpeed = false. */
} DeviceParam_t;

/** @} */
/** @} */
#endif /*_T_COMM_DEVICES_H_*/

