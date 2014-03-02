/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_BULK_PROTOCOL_H_
#define _R_BULK_PROTOCOL_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup r15_family
 *    @{
 *      @addtogroup bulk_protocol Bulk Protocol
 *      Functionalities for handling bulk protocol. Receiving/Sending bulk
 *      commands and state machines for receiving and sending bulk data chunks.
 *
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system_v2.h"
#include "t_bulk_protocol.h"
#include "t_r15_transport_layer.h"
#include "t_r15_network_layer.h"
#include "error_codes.h"
#include "r_communication_service.h"
#ifdef  CFG_ENABLE_LOADER_TYPE
#include "t_security_algorithms.h"
#endif

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

#endif //#ifdef  CFG_ENABLE_LOADER_TYPE

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * State machine for bulk transfer from ME to PC.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] BulkVector_p    Pointer to the received buffer.
 *
 * @retval  E_SUCCESS                  After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS If Packet_p is NULL.
 */
ErrorCode_e R15_Bulk_Process_Write(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p);
/**
 * Decoding received bulk command.
 *
 * @param [in] Communication_p Communication module context.
 * @param [in] Packet_p        Pointer to the received buffer.
 *
 * @retval  E_SUCCESS                  After successful execution.
 * @retval  E_INVALID_INPUT_PARAMETERS If Packet_p is NULL.
 */
ErrorCode_e R15_Bulk_Process(Communication_t *Communication_p, PacketMeta_t *Packet_p);

#ifdef  CFG_ENABLE_LOADER_TYPE
/**
 * Generate bulk Session ID. Allowed bulk session ID is from 1 to 65535.
 *
 * @param [in] Communication_p Communication module context.
 *
 * @retval  Session ID Next free session ID.
 */
uint16 Do_R15_Bulk_GenerateBulkSessionID(Communication_t *Communication_p);
#endif  //#ifdef  CFG_ENABLE_LOADER_TYPE

#ifdef __cplusplus
extern "C"
{
#endif // #ifdef __cplusplus
    /**
     * Opens Bulk session.
     *
     * @param [in] Communication_p Communication module context.
     * @param [in] SessionId       Session ID.
     * @param [in] Mode            Type of operation(Read or Write).
     * (send or receive) over the bulk protocol.
     *
     * @return Vector ID  Vector ID of the newly opened session.
     * @retval BULK_ERROR If the session can not be opened.
     */
    LCM_API uint32 Do_R15_Bulk_OpenSession(const Communication_t *const Communication_p, const uint16 SessionId, const TL_SessionMode_t Mode);

    /**
     * Creates Bulk Vector.
     *
     * @param [in] Communication_p     Communication module context.
     * @param [in] BulkVector          Index of Bulk Vector.
     * @param [in] Length              Data Length in bytes.
     * @param [in] BuffSize            Requested size of each Buffer.
     * @param [in] CreatedBulkVector_p Already created bulk vector which
     *                                 need to be updated with correct
     *                                 information.
     *
     * @return Vector ID  The ID of the Vector that has been created.
     * @retval BULK_ERROR If creation of vector failed.
     */
    LCM_API TL_BulkVectorList_t *Do_R15_Bulk_CreateVector(const Communication_t *const Communication_p, const uint32 BulkVector, uint32 Length, const uint32 BuffSize, TL_BulkVectorList_t *CreatedBulkVector_p);

    /**
     * Frees Bulk Vector.
     *
     * @param [in] Communication_p  Communication module context.
     * @param [in] BulkVector_p     Pointer to the Bulk Vector.
     * @param [in] ReqReleaseBuffer Defines if complete buffers with metadata will
     *                              be released or only only the metadata will be cleared.
     *
     * @retval  E_SUCCESS   After successful execution.
     * @retval  BULK_ERROR  Error while destroying vector.
     */
    LCM_API uint32 Do_R15_Bulk_DestroyVector(const Communication_t *const Communication_p, TL_BulkVectorList_t *BulkVector_p, boolean ReqReleaseBuffer);

    /**
     * Start specified bulk session.
     *
     * @param [in] Communication_p Communication module context.
     * @param [in] BulkVector_p    Pointer to the Bulk Vector.
     * @param [in] Offset          Sets the offset from where data
     *                             read/write should be performed.
     *
     * @retval  E_INVALID_BULK_MODE            Invalid bulk mode.
     * @retval  E_FAILED_TO_START_BULK_SESSION Either the session is
     *                                         invalid or the BulkVector is invalid.
     * @retval  E_SUCCESS                      After successful execution.
     */
    LCM_API ErrorCode_e Do_R15_Bulk_StartSession(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p, const uint64 Offset);

    /**
     *  Get status of bulk transfer.
     *
     * @param [in] BulkVector_p Pointer to the Bulk Vector.
     *
     * @retval BULK_SESSION_IDLE       Idle state of bulk protocol.
     * @retval BULK_SESSION_OPEN       Opened bulk protocol and ready for
     *                                 transmitting.
     * @retval BULK_SESSION_PROCESSING Processing bulk transfer.
     * @retval BULK_SESSION_FINISHED   Bulk transfer finished.
     */
    LCM_API TL_BulkSessionState_t Do_R15_Bulk_GetStatusSession(const TL_BulkVectorList_t *BulkVector_p);

    /**
     * Close specified Bulk session.
     *
     * @param [in] Communication_p Communication module context.
     * @param [in] BulkVector_p    Pointer to the Bulk Vector.
     *                             for the session you want to close.
     *
     * @retval  E_SUCCESS                      After successful execution.
     * @retval  E_FAILED_TO_CLOSE_BULK_SESSION If the BulkVector is not valid.
     */
    LCM_API ErrorCode_e Do_R15_Bulk_CloseSession(Communication_t *Communication_p, TL_BulkVectorList_t *BulkVector_p);

#ifndef  CFG_ENABLE_LOADER_TYPE
    /**
     * Sets pointers to callback functions regarding the bulk transfer.
     *
     * @param [in] Communication_p       Communication module context.
     * @param [in] BulkCommandCallback_p Pointer to the callback function
                                         for handling a received command.
     * @param [in] BulkDataCallback_p    Pointer to the callback function
                                         for handling a received data.
     * @param [in] EndOfDump_p           Pointer to the callback function
                                         for handling a finished bulk transfer.
     */

    LCM_API void Do_R15_Bulk_SetCallbacks(Communication_t *Communication_p, void *BulkCommandCallback_p, void *BulkDataCallback_p, void *EndOfDump_p);

    LCM_API void Do_R15_Bulk_SetBuffersRelease(Communication_t *Communication_p, void *BulkBufferRelease_p);
#endif

#ifdef __cplusplus
};
#endif // #ifdef __cplusplus

/** @} */
/** @} */
/** @} */
#endif /* _R_BULK_PROTOCOL_H_*/
