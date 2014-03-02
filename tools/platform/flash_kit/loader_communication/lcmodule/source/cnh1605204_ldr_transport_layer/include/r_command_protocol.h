/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_COMMAND_PROTOCOL_H_
#define _R_COMMAND_PROTOCOL_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup command_protocol Command Protocol
 *      Functionalities for handling command protocol. Receiving/Sending
 *      commands with command protocol.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_command_protocol.h"
#include "t_r15_network_layer.h"
#include "error_codes.h"
#include "t_communication_service.h"

#if (defined(WIN32) || defined(__CYGWIN__))

#ifdef LCM_EXPORTS
#define LCM_API __declspec(dllexport) // export DLL information
#else //LCM_EXPORTS
#define LCM_API __declspec(dllimport) // import DLL information
#endif // LCM_EXPORTS

#elif (defined(__linux__) || defined(__APPLE__))

#ifdef LCM_EXPORTS
#define LCM_API __attribute__((visibility("default")))
#else //LCM_EXPORTS
#define LCM_API
#endif // LCM_EXPORTS

#elif defined(CFG_ENABLE_LOADER_TYPE)

#define LCM_API

#endif  // WIN32

/***********************************************************************
 * Declaration of functions
 **********************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif // #ifdef __cplusplus
    /**
     * Reset the Session counters.
     *
     * @param [in]  Communication_p Communication module context.
     *
     * @retval  E_SUCCESS After successful execution.
     */
    LCM_API ErrorCode_e Do_R15_Command_ResetSessionCounters(const Communication_t *const Communication_p);

    /**
     * Sends command packet with command protocol.
     *
     * @param [in] Communication_p Communication module context.
     * @param [in] CmdData_p       Pointer to the command data.
     *
     * @retval  E_SUCCESS                  After successful execution.
     * @retval  E_INVALID_INPUT_PARAMETERS If CmdData_p is NULL.
     */
    LCM_API ErrorCode_e Do_R15_Command_Send(Communication_t *Communication_p, CommandData_t *CmdData_p);
#ifdef __cplusplus
};
#endif // #ifdef __cplusplus

/**
 * Decode received command.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] Packet_p        Pointer to the received buffer.
 *
 * @retval E_SUCCESS                  After successful execution.
 * @retval E_INVALID_INPUT_PARAMETERS if Packet_p is NULL.
 */
ErrorCode_e R15_Command_Process(Communication_t *Communication_p, PacketMeta_t *Packet_p);

/** @} */
/** @} */
/** @} */
#endif /* _R_COMMAND_PROTOCOL_H_*/
