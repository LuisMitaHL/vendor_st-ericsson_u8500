/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_A2_PROTOCOL_H_
#define _R_A2_PROTOCOL_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup a2_protocol A2 Protocol
 *      Functionalities for handling A2 commands.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_a2_protocol.h"
#include "t_a2_network.h"
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
     * Sends command packet with A2 protocol.
     *
     * @param [in] Communication_p Communication module context.
     * @param [in] CmdData_p       Pointer to the command data.
     *
     * @retval  E_SUCCESS                  After successful execution.
     * @retval  E_INVALID_INPUT_PARAMETERS If CmdData_p is NULL.
     */
    LCM_API ErrorCode_e Do_A2_Command_Send(Communication_t *Communication_p, A2_CommandData_t *CmdData_p);
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
ErrorCode_e A2_Command_Process(Communication_t *Communication_p, A2_PacketMeta_t *Packet_p);

/** @} */
/** @} */
/** @} */
#endif /* _R_A2_PROTOCOL_H_*/
