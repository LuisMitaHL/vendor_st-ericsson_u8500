/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_UART_ABSTRACTION_H_
#define _R_UART_ABSTRACTION_H_

/**
 *  @addtogroup ldr_hw_abstraction
 *  @{
 *    @addtogroup UART_abstraction
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
// Removed from fairbanks for some reason (?)
//#include "t_UART_abstraction.h"
#include "t_communication_abstraction.h"
#include "error_codes.h"
#include "uart.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * @brief Creates a device descriptor.
 *
 * Creates a device descriptor for setting up communication over the
 * specified UART interface using the specified speed. Note that this
 * function can be called several times with different device IDs
 * to enable communication over more than one serial port.
 *
 * @param [in] Device   The device ordinal value. 0 for the first UART port,
 *                      1 for the second and so on.
 * @param [in] Speed    The speed to configure the device.
 * @return              Pointer to the device descriptor.
 * @retval  NULL        If creation of descriptor fails.
 */
CommDev_t *Do_GetUARTDescriptor(const CommDevId_t Device,
                                const t_uart_baud_rate Speed);

/**
 * @brief The function prepares all data structures that are going to be used by
 *        the device.
 *
 * The function also registers interrupt callback functions.
 *
 * @param [in] Ctx_p            Pointer to the device descriptor.
 *                              The device context is initialized by the func.
 * @param [in] Callback_p       Pointer to Callback functions that will be
 *                              called when data is received or transmitted.
 * @return     E_SUCCESS   After successful execution.
 *
 *
 * @return     E_ALLOCATE_FAILED             Failed to allocate memory space.
 * @return     E_FAILED_TO_INIT_COM_DEVICE   Failed to initialize the
 *                                           communication device.
 * @return     E_FAILED_TO_FLUSH_RXFIFO      Failed to flush the
 *                                                communication device fifo.
 */
ErrorCode_e Do_DevUARTInit(CommDevContext_t *const Ctx_p,
                           void *const Callback_p);

/**
 * @brief The function frees all memory that has been allocated for the
 *        specified UART device.
 *
 * The function also flushes transmission data buffers, releases all buffers
 * under the device control and shuts down the device.
 *
 * @param [in] Device_p  Pointer to the device structure.
 * @param [in] CommunicationDevice_p  Pointer to the communication device.
 * @return     None.
 */
void Do_DevUARTShutDown(CommDev_t *Device_p, void *CommunicationDevice_p);

/**
 * Initialization of UART communication device.
 *
 * @param [in] Device     Specified UART communication device.
 * @param [in] For_Debug  Set to TRUE if UART is used for debug information. FALSE for communication.
 *
 * @retval NULL if initialization of UART device fails.
 * @retval Pointer to the initialized UART communication device.
 */
CommunicationDevice_t *Do_CommDeviceInitUART(CommDevId_t Device, boolean For_Debug);

/**
 * @brief UART driver CallBack function
 *
 * The function checks if it is RX or TX callback
 *
 * @param [in] Params_p      Not used.
 * @param [in] IRQ_Status_p  UART IRQ status.
 *
 * @retval     None.
 */
void CallbackUART(void *Params_p, void *IRQ_Status_p);

/* This is for the crash handler  */
#ifdef ENABLE_DEBUG
/**
 * @brief Print on UART using direct hw writes
 *
 * Writes to UART by direct writes in hw registers
 * and is used by the crashhandler
 *
 * @note assumes UART is setup by regular debug-subsystem
 *
 * @param [in] buffer        buffer with data to write.
 * @param [in] size          number of bytes in buffer.
 *
 * @retval     None.
 */
void DirectPrint(char *Buffer, uint32 Size);
#endif
/** @} */
/** @} */
#endif /*_R_UART_ABSTRACTION_H_*/
