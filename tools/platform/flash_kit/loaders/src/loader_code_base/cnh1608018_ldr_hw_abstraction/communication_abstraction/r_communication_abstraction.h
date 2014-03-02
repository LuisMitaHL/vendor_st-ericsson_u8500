/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_COMMUNICATION_ABSTRACTION_H_
#define _R_COMMUNICATION_ABSTRACTION_H_

/**
 *  @addtogroup ldr_hw_abstraction
 *  @{
 *    @addtogroup communication_abstraction
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_communication_abstraction.h"
// Removed in fairbanks for some reason(?)
//#include "t_communication_service.h"


/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/


/**
 * Initialization of currently connected communication device.
 *
 * Used communication devices USB or UART (UART0/UART1/UART2).
 *
 * @retval Pointer to the initialized communication device.
 * @retval NULL if initialization of device fails.
 */
CommunicationDevice_t *Do_CommDeviceInit(void);


/**
 * Initialization of Debug communication device.
 *
 * Used communication devices UART (specified with UART_DEBUG_PORT).
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @return none.
 */
void Do_CommDeviceDebugoutInit(Communication_t *const Communication_p);


/**
 * Shutdown of currently used communication device.
 *
 * @param [in,out] CommunicationDevice_p  Pointer to a communication device.
 *
 * @return none.
 */
ErrorCode_e Do_CommDeviceShutDown(const CommunicationDevice_t *CommunicationDevice_p);

/** @} */
/** @} */
#endif /*_R_COMMUNICATION_ABSTRACTION_H_*/
