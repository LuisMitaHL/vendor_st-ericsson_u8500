/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_USB_ABSTRACTION_H_
#define _R_USB_ABSTRACTION_H_

/**
 *  @addtogroup ldr_hw_abstraction
 *  @{
 *    @addtogroup USB_abstraction
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_communication_abstraction.h"
// Removed for Fairbanks for some reason(?)
//#include "t_USB_abstraction.h"
#include "error_codes.h"
// Removed for Fairbanks
//#include "r_hl1usb.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * @brief Creates a device descriptor.
 *
 * This descriptor is for setting up communication over the specified USB
 * interface using the specified speed.
 *
 * @param [in]  HighSpeed  TRUE if the device should be configured for
 *                         HighSpeed.
 * @return      Pointer to the device descriptor.
 * @retval NULL on failure.
 */
CommDev_t *Do_GetUSBDescriptor(const boolean HighSpeed);

/**
 * @brief The function prepares all data structures that are going to be used by
 *        the device.
 *
 * The function also registers interrupt callback functions.
 *
 * @param [in] Ctx_p      Pointer to the device descriptor.
 *                        The device context is initialized by the function.
 * @param [in] Callback_p Pointer to Callback functions that will be called when
 *                        data is received or transmitted.
 *
 * @return     E_SUCCESS                      After successful execution.
 *
 * @return     E_ALLOCATE_FAILED              Failed to allocate memory space.
 * @return     E_FAILED_TO_INIT_COM_DEVICE    Failed to initialize the
 *                                            communication device.
 * @return     E_FAILED_TO_FLUSH_RXFIFO       Failed to flush the communication
 *                                            device FIFO.
 */
ErrorCode_e Do_DevUSBInit(CommDevContext_t *const Ctx_p,
                          void *const Callback_p);

/**
 * @brief The function frees all memory that has been allocated for the
 *        specified UART device.
 *
 * The function also flushes transmission data buffers, releases all buffers
 * under the device control and shuts down the device.
 *
 * @param [in] Device_p               Pointer to the device structure.
 * @param [in] CommunicationDevice_p  Pointer to the communication device.
 * @return     None.
 */
void Do_DevUSBShutDown(CommDev_t *Device_p, void *CommunicationDevice_p);

/**
 * Initialization of USB communication device.
 *
 * @return Pointer to the initialized USB communication device.
 *
 * @retval NULL if initialization of device fails.
 * @retval Pointer to the initialized communication device.
 *
 */
CommunicationDevice_t *Do_CommDeviceInitUSB(void);

/**
 * @brief USB driver CallBack function
 *
 * The function checks if it is RX or TX callback
 *
 * @param [in] Params_p      Not used.
 * @param [in] IRQ_Status_p  USB IRQ status.
 *
 * @retval     None.
 */
void CallbackUSB(void *Params_p, void *IRQ_Status_p);


/** @} */
/** @} */
#endif /*_R_USB_ABSTRACTION_H_*/
