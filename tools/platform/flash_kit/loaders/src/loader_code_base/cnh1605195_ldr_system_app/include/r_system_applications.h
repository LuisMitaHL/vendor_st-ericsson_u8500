/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_SYSTEM_APPLICATIONS_H_
#define _R_SYSTEM_APPLICATIONS_H_

/**
 *  @addtogroup ldr_system_app Loader system applications
 *  @{
 *  This module implements functionalities of system applications.
 *
 *  It comprises functionalities: @n
 *  - ME power down, @n
 *  - Execute software, @n
 *  - Reboot, @n
 *  - Authenticate, @n
 *  - Loader startup status, @n
 *  - Get control keys, @n
 *  - Authentication challenge, @n
 *
 *  Parameters for commands are packed in
 *  the payload of incoming packet. @n
 *  All command responses are packed
 *  in the payload of general response packet. @n
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_system_applications.h"
#include "t_communication_service.h"


/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * Used for sending the start message.
 *
 * This start message 'Loader Started' is to indicate that
 * the loader is ready to receive commands.
 * If any errors come up then sends a command to PA to indicate that
 * the loader is not ready.
 *
 * @param [in]  ErrorCode       Current error state. This is the error from the
 *                              previous function.
 * @retval  E_SUCCESS After successful execution.
 */
ErrorCode_e Do_System_R15LoaderStarted(ErrorCode_e ErrorCode);

/** @} */
#endif /*_R_SYSTEM_APPLICATIONS_H_*/
