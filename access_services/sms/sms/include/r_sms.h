#ifndef INCLUSION_GUARD_R_SMS_H
#define INCLUSION_GUARD_R_SMS_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
 *
 * DESCRIPTION:
 *
 * SMS Server include file for exported interface Requester Functions.
 *
 *************************************************************************/

#include "t_sms.h"
#include "t_sms_cb_session.h"

// Code is being run through a linux formatting tool (indent). The existance of
// these C/C++ extern declaration would cause all code within their braces to be
// indented. To ensure that that does not happen, we disable the indent tool
// when processing them using the *INDENT-OFF* and *INDENT-ON* commands.

// *INDENT-OFF*
// Ensure that "C++" compiled files can link with these "C" compiled functions.
#ifdef __cplusplus
extern "C" {
#endif
// *INDENT-ON*


// Unfortunately, at some point in the past, this declaration and #defines
// were moved here from r_smslinuxporting.h. This was to try and fix a
// problem clients had when using the SMS Server asynchronously.
// These are internal functions and macros and should not be used by
// clients. The correct solution was to add new API utilities which
// the clients should use:
//     Util_SMS_SignalReceiveOnSocket()
//     Util_SMS_SignalFree()
// When the clients have been updated to use these Util functions, this
// header file should be updated to remove this code. Updates should also
// be made in r_cbs.c and r_sms.c to complete the fix.
#define SMS_RECEIVE_ON_SOCKET(sig_no, socket) (union SMS_SIGNAL*)ose_to_ipc_sig_receive(sig_no, socket)

/*
 *
 *
 *  OSE --> IPC porting "utility" functions, implemented in r_sms.c 
 *
 *
 */
union SMS_SIGNAL *ose_to_ipc_sig_receive(
    const SMS_SIGSELECT * pSigNoToWaitFor,
    int socket);

#undef SMS_SIGNAL_FREE
#define SMS_SIGNAL_FREE(Signal_pp) ipc_sig_free_buf((void**)(Signal_pp))



/*
**========================================================================
** SMS Storage Interface Requester/Response function proto-types
**========================================================================
*/

//========================================================================

/**
 * Reads the SMS data from a given storage slot.
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the
 *                                    request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] Slot_p                 The position of the SMS to be read
 * @param [out] Status_p              The type of SMS
 * @param [out] SMSC_Address_TPDU_p   The SMSC address and SMS TPDU
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                    \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                    \li \c SMS_ERROR_INTERN_MESSAGE_NOT_FOUND
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_SHORTMESSAGEREAD
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ShortMessageRead(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    SMS_Status_t * const Status_p,
    SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    SMS_Error_t * const Error_p);


/**
 * Response function for Request_SMS_ShortMessageRead
 *
 */

SMS_RequestStatus_t Response_SMS_ShortMessageRead(
    const void *const SigStruct_p,
    SMS_Status_t * const Status_p,
    SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * Writes an SMS to the given storage slot. If the requested storage
 * slot is 0, the MTS will save into any convenient slot. If a specific slot is
 * specified, MTS will attempt to overwrite the contents of that slot.
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the
 *                                    request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] Status                 The type of SMS
 * @param [in] SMSC_Address_TPDU_p    The SMSC address and SMS TPDU
 * @param [in,out] Slot_p             The position the SMS is to be written to
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                    \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_SHORTMESSAGEWRITE
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ShortMessageWrite(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Status_t Status,
    const SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_ShortMessageWrite
 *
 */

SMS_RequestStatus_t Response_SMS_ShortMessageWrite(
    const void *const SigStruct_p,
    SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * Deletes the SMS in the given storage slot and any associated
 * Status Report.
 *
 * @param [in] RequestCtrl_p    Pointer to struct controlling whether the
 *                              request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p      Response signal to be unpacked.
 * @param [in] Slot_p           The memory position to delete
 * @param [out] Error_p         SMS Server internal error
 *                              cause returned by the server.
 *                              \li \c SMS_ERROR_INTERN_NO_ERROR
 *                              \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                              \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                              \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                              \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_SHORTMESSAGEDELETE
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ShortMessageDelete(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_ShortMessageDelete
 *
 */

SMS_RequestStatus_t Response_SMS_ShortMessageDelete(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * Reads the Status Report data associated with an SMS storage slot.
 *
 * @param [in] RequestCtrl_p             Pointer to struct controlling whether the
 *                                       request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p               Response signal to be unpacked.
 * @param [in] Slot_p                    The position of the SMS whose matching Status Report is to be read
 * @param [out] StatusReport_p           The Status Report TPDU data
 * @param [out] Error_p                  SMS Server internal error
 *                                       cause returned by the server.
 *                                       \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                       \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                       \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                       \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                       \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                       \li \c SMS_ERROR_INTERN_MESSAGE_NOT_FOUND
 *
 * @retval SMS_REQUEST_OK                    Request performed successfully
 * @retval SMS_REQUEST_PENDING               Used to indicate that the requester was
 *                                           called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION    Request failed by SMS Server,
 *                                           which could not perform what was
 *                                           asked for. Look in Error_p for
 *                                           details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER      Request failed due to some wrong
 *                                           parameters. Look in Error_p for
 *                                           details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE       Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE         Software back plane service not
 *                                           available
 *
 * @signalid RESPONSE_SMS_STATUSREPORTREAD
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_StatusReportRead(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    SMS_StatusReport_t * const StatusReport_p,
    SMS_Error_t * const Error_p);


/**
 * Response function for Request_SMS_StatusReportRead
 *
 */

SMS_RequestStatus_t Response_SMS_StatusReportRead(
    const void *const SigStruct_p,
    SMS_StatusReport_t * const StatusReport_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * This function is used to delete all messages in a certain storage media.
 * Please be careful when using this, as it can delete messages used by another
 * client as well
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether
 *                                    the request is called in wait mode or no
 *                                    wait mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] Storage                Storage that should be emptied.
 * @param [in] AllThisICCID_Messages  Set to TRUE if you want to delete all
 *                                    messages in the storage related to
 *                                    the current ICCID.
 * @param [in] AllOtherICCID_Messages Set to TRUE if you want to delete all
 *                                    messages in the storage related to
 *                                    other ICCID's.
 * @param [out] Error_p               SMS Server internal
 *                                    error cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                    \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by Message Transport
 *                                        Server, which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_STORAGECLEAR
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_StorageClear(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Storage_t Storage,
    const uint8_t AllThisICCID_Messages,
    const uint8_t AllOtherICCID_Messages,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_StorageClear
 *
 */

SMS_RequestStatus_t Response_SMS_StorageClear(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * Sets the status (READ, TO BE READ, SENT, etc.) of the SMS in a
 * given storage slot.
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the
 *                                    request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] Slot_p                 The position of the SMS to change the status of.
 * @param [in] Status                 The status to associate with the SM.
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                    \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                    \li \c SMS_ERROR_INTERN_INVALID_MESSAGE_ID
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_SHORTMESSAGESTATUSSET
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ShortMessageStatusSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    const SMS_Status_t Status,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_ShortMessageStatusSet
 *
 */

SMS_RequestStatus_t Response_SMS_ShortMessageStatusSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * This function is used to search for SMS fitting certain SMS search criteria.
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [in] SigStruct_p          Response signal to be unpacked.
 * @param [in] SearchInfo_p         Describes Search criteria's
 * @param [out] Slot_p              The location of the SMS found
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                  \li \c SMS_ERROR_INTERN_INVALID_BROWSE_OPTION
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_SHORTMESSAGEFIND
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ShortMessageFind(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_SearchInfo_t * const SearchInfo_p,
    SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p);


/**
 * Response function for Request_SMS_ShortMessageFind
 *
 */

SMS_RequestStatus_t Response_SMS_ShortMessageFind(
    const void *const SigStruct_p,
    SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * This function is used for analyzing the status of the SMS store
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the
 *                                    request is called in wait mode or no wait
 *                                    mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] Storage                The type of storage medium.
 * @param [out] SlotInformation_p     The slot status
 * @param [out] StorageStatus_p       The status of the SMS Server storage
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_STORAGECAPACITYGET
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_StorageCapacityGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Storage_t Storage,
    SMS_SlotInformation_t * const SlotInformation_p,
    SMS_StorageStatus_t * const StorageStatus_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_StorageCapacityGet
 *
 */

SMS_RequestStatus_t Response_SMS_StorageCapacityGet(
    const void *const SigStruct_p,
    SMS_SlotInformation_t * const SlotInformation_p,
    SMS_StorageStatus_t * const StorageStatus_p,
    SMS_Error_t * const Error_p);


//========================================================================

/**
 * This function is used to read the information of an SMS. The information
 * about the SMS that can be determined without having to parse the TPDUs
 * of that message is returned.
 *
 * @param [in] RequestCtrl_p               Pointer to struct controlling
 *                                         whether the request is called in
 *                                         wait mode or no wait mode.
 * @param [in] SigStruct_p                 Response signal to be unpacked.
 * @param [in] Slot_p                      The SMS slot identifier
 *                                         that you want to read the header of.
 * @param [out] ReadInformation_p          The decoded header information of the
 *                                         SMS.
 * @param [out] Error_p                    SMS Server internal
 *                                         error cause returned by the server.
 *                                         \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                         \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                         \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                         \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                         \li \c SMS_ERROR_INTERN_INVALID_MESSAGE_ID
 *
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_SHORTMESSAGEINFOREAD
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ShortMessageInfoRead(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    SMS_ReadInformation_t * const ReadInformation_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_ShortMessageInfoRead
 *
 */

SMS_RequestStatus_t Response_SMS_ShortMessageInfoRead(
    const void *SigStruct_p,
    SMS_ReadInformation_t * const ReadInformation_p,
    SMS_Error_t * const Error_p);


//========================================================================

/**
 * This function is used to report that the client SMS Memory Capacity is full.
* When the platform is configured to send Deliver Reports the next MT SMS received will be
* negatively acknowledged and the platform memory capacity exceeded flag will be set if
* appropriate.
* This function should be called at start-up if client SMS Memory Capacity is full since the
* setting is not persistently stored by the platform.
* This function has no effect if the platform is not configured to send Deliver Reports.
* Request_SMS_MemCapacityAvailableSend() should be used to report when client
* SMS Memory Capacity becomes available.
 *
 * @param [in] RequestCtrl_p    Pointer to struct controlling whether the
 *                              request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p      Response signal to be unpacked.
 * @param [out] Error_p         Error cause returned by the server, indicating
 *                              if Memory Capacity Full was set.
 *                              \li \c SMS_ERROR_INTERN_NO_ERROR
 *                              \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                              \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
*
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_MEMCAPACITYFULLREPORT
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_MemCapacityFullReport(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_Error_t * const Error_p);

/**
 * Response function for Request_SMS_MemCapacityFullReport
 *
 */
SMS_RequestStatus_t Response_SMS_MemCapacityFullReport(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);


/*
**========================================================================
** SMS Interface Requester/Response function proto-types
**========================================================================
*/

//========================================================================

/**
 * This function is used to subscribe to receive unsolicited events such
 * as MT SMS received events from the SMS Server.
 *
 * @param [in] RequestCtrl_p              Pointer to struct controlling whether the
 *                                        request is called in wait mode or no wait mode.
 * @param [out] Error_p                   SMS Server internal error
 *                                        cause returned by the server.
 *                                        \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                        \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                        \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                        \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                        \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                        \li \c SMS_ERROR_INTERN_APP_PORT_IN_USE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid None
 * @waitmode Wait mode
 * @clienttag No.
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_EventsSubscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_Error_t * const Error_p);

/**
 * Response function for Request_SMS_EventsSubscribe
 *
 */

SMS_RequestStatus_t Response_SMS_EventsSubscribe(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);

//========================================================================

/**
 * This function is used to unsubscribe from reception of unsolicited events
 * such as MT SMS received events from the SMS Server.
 *
 * @param [in] RequestCtrl_p              Pointer to struct controlling whether the
 *                                        request is called in wait mode or no wait mode.
 * @param [out] Error_p                   SMS Server internal error
 *                                        cause returned by the server.
 *                                        \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                        \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                        \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                        \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                        \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                        \li \c SMS_ERROR_INTERN_APP_PORT_IN_USE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid None
 * @waitmode Wait mode
 * @clienttag No.
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_EventsUnsubscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_Error_t * const Error_p);


/**
 * Response function for Request_SMS_EventsUnsubscribe
 *
 */

SMS_RequestStatus_t Response_SMS_EventsUnsubscribe(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);


//========================================================================

/**
 * This function is used to subscribe to received SMS that contain a
 * particular application port. If the given application port or range has
 * already been subscribed to then SMS_ERROR_INTERN_APP_PORT_IN_USE will be
 * returned. It is valid for a particular port to be subscribed to by
 * two different clients if one has subscribed to the port as a destination
 * and the other as an origination port.
 *
 * @param [in] RequestCtrl_p              Pointer to struct controlling whether the
 *                                        request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p                Response signal to be unpacked.
 * @param [in] ApplicationPortRange_p     The application port range to subscribe to.
 * @param [out] Error_p                   SMS Server internal error
 *                                        cause returned by the server.
 *                                        \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                        \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                        \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                        \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                        \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                        \li \c SMS_ERROR_INTERN_APP_PORT_IN_USE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_APPLICATIONPORTSUBSCRIBE
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes.
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ApplicationPortSubscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_ApplicationPortSubscribe
 *
 */

SMS_RequestStatus_t Response_SMS_ApplicationPortSubscribe(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * This function is used to unsubscribe to received SMS that contain a
 * particular application port. If the given application port range is
 * not matched against the current subscriptions an error will be returned.
 *
 * @param [in] RequestCtrl_p              Pointer to struct controlling whether the
 *                                        request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p                Response signal to be unpacked.
 * @param [in] ApplicationPortRange_p     The application port range to unsubscribe to.
 * @param [out] Error_p                   SMS Server internal error
 *                                        cause returned by the server.
 *                                        \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                        \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                        \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                        \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                        \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                        \li \c SMS_ERROR_INTERN_APP_PORT_IN_USE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_APPLICATIONPORTUNSUBSCRIBE
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes.
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ApplicationPortUnsubscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_ApplicationPortUnsubscribe
 *
 */

SMS_RequestStatus_t Response_SMS_ApplicationPortUnsubscribe(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * This is used to deliver an MT SMS TPDU into the SMS Server
 * from an external user (apart from the normal Network Signalling). An example
 * is an SMS message being received with MMS as a bearer.
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p          Response signal to be unpacked.
 * @param [in] SMSC_Address_TPDU_p  The SMSC address and SMS TPDU that should be delivered
 *                                  to the SMS Server for routing.
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_SHORTMESSAGEDELIVER
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ShortMessageDeliver(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_ShortMessageDeliver
 *
 */

SMS_RequestStatus_t Response_SMS_ShortMessageDeliver(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * Send an SMS to the network. If the SMS TPDU parameter is not NULL
 * the TPDU data is sent to the network without storing to an SMS slot and the
 * SMS slot parameter is ignored. If the SMS TPDU parameter is NULL the TPDU
 * data stored at the given SMS slot is sent to the network. The more to send
 * parameter is used to request that a series of non-concatenated SMS are to
 * be sent together on the same radio link.
 *
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p          Response signal to be unpacked.
 * @param [in] SMSC_Address_TPDU_p  The SMS TPDU to send and destination
 *                                  Service Center number.
 * @param [in] Slot_p               The slot position of the message to send
 * @param [in] MoreToSend           Indication that there are more SMS to send
 * @param [out] SM_Reference_p      The unique message reference number sent over
 *                                  the air with this message (0 - 255)
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                  \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                  \li \c SMS_ERROR_INTERN_INVALID_MESSAGE_ID
 *                                  One of the Network Signalling error Ref.
 *                                  [3.1.5] cause can also be returned by the
 *                                  server.
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_SHORTMESSAGESEND
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_ShortMessageSend(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    const SMS_Slot_t * const Slot_p,
    const uint8_t MoreToSend,
    uint8_t * const SM_Reference_p,
    SMS_Error_t * const Error_p);

/**
 * Used by SAT to send an SMS to the network.
 *
 * For SAT, there are slight procedural changes in sending a short message.
 * However the API syntax and semantics are the same. See Request_SMS_ShortMessageSend
 * for further information.
 *
 * @note This API function is intended to ONLY be used by SAT!
 *
 * @see  Request_SMS_ShortMessageSend
 */
SMS_RequestStatus_t Request_SMS_SatShortMessageSend(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    const SMS_Slot_t * const Slot_p,
    const uint8_t MoreToSend,
    uint8_t * const SM_Reference_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_ShortMessageSend
 * and Request_SMS_SatShortMessageSend
 */
SMS_RequestStatus_t Response_SMS_ShortMessageSend(
    const void *const SigStruct_p,
    uint8_t * const SM_Reference_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * This function is used to send a Deliver Report of type RP_ACK to acknowledge
 * or RP_ERROR to negative acknowledge the receipt of an SMS If the RP
 * Error Cause (1( (sec 9.3.2.4) passed to the function is zero the associated
 * TPDU will be treated as a Deliver Report of type RP_ACK and sent to the
 * network. If the RP Error Cause passed to the function is non-zero the
 * associated TPDU will be treated as a Deliver Report of type RP_ERROR and
 * sent to the network along with that RP Error Cause.
 *
 * The response message contains a confirmation received from NS. The confirmation
 * indicates if the Deliver Report was sent successfully.
 *
 * @param [in] RequestCtrl_p     Pointer to struct controlling whether the
 *                               request is called in wait mode or no wait
 *                               mode.
 * @param [in] SigStruct_p       Response signal to be unpacked.
 * @param [in] RP_ErrorCause     The RP Error Cause to be sent to the network
 * @param [in] TPDU_p            The Deliver Report TPDU to be sent to the
 *                               network
 * @param [out] Error_p          Error cause returned by the server,
 *                               indicating if the Deliver Report (Ack or Nack)
 *                               was sent by the SMS Server, or if the safety
 *                               mechanism has already sent an error Deliver
 *                               Report (Nack). SMS Server
 *                               internal error cause returned by the server.
 *                               \li \c SMS_ERROR_INTERN_NO_ERROR
 *                               \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                               \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                               \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                               \li \c SMS_ERROR_INTERN_INVALID_MESSAGE_ID
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_DELIVERREPORTSEND
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_DeliverReportSend(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_RP_ErrorCause_t RP_ErrorCause,
    const SMS_TPDU_t * const TPDU_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_DeliverReportSend
 *
 */

SMS_RequestStatus_t Response_SMS_DeliverReportSend(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * This function is used to send a Memory Capacity Available message to the
 * Network. This message will be sent regardless of the memory capacity of the
 * platform but will clear the platform memory capacity exceeded flag. If the
 * Network responds to the Memory Capacity Available message with a negative
 * acknowledge the RP Error Cause will be translated to a MSG Error Code and
 * returned by the requester.
 *
 * @param [in] RequestCtrl_p    Pointer to struct controlling whether the
 *                              request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p      Response signal to be unpacked.
 * @param [out] Error_p         Error cause returned by the server, indicating
 *                              if Memory Capacity Available was sent by the
 *                              MTS, or if a negative acknowledge was received
 *                              from the Network. SMS Server
 *                              internal error cause returned by the server.
 *                              \li \c SMS_ERROR_INTERN_NO_ERROR
 *                              \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                              \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                              One of the Network Signalling error Ref.
 *                              [3.1.5] cause can also be returned by the
 *                              server.
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_MEMCAPACITYAVAILABLESEND
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_MemCapacityAvailableSend(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_MemCapacityAvailableSend
 *
 */

SMS_RequestStatus_t Response_SMS_MemCapacityAvailableSend(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);

//========================================================================

/**
 * This function is used to get the Memory Capacity Full parameter message
 * from the SMS server.
 *
 * @param [in] RequestCtrl_p    Pointer to struct controlling whether the
 *                              request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p      Response signal to be unpacked.
 * @param [out] memFullState_p  Integer to tell if memory is full, 1 = FULL.
 * @param [out] Error_p         Error cause returned by the server, indicating
 *                              if Memory Capacity Available was sent by the
 *                              MTS, or if a negative acknowledge was received
 *                              from the Network. SMS Server
 *                              internal error cause returned by the server.
 *                              \li \c SMS_ERROR_INTERN_NO_ERROR
 *                              \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                              \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                              One of the Network Signalling error Ref.
 *                              [3.1.5] cause can also be returned by the
 *                              server.
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_MEMCAPACITYGETSTATE
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_MemCapacityGetState(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    int * const memFullState_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for Request_SMS_MemCapacityGetState
 *
 */

SMS_RequestStatus_t Response_SMS_MemCapacityGetState(
    const void *const SigStruct_p,
    int * const memFullState_p,
    SMS_Error_t * const Error_p);



/*
**========================================================================
** Restricted SMS Interface Requester/Response function proto-types
**========================================================================
*/

//========================================================================

/**
 * This function is used to set the control of the continuity of the SMS relay protocol link
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [in] SigStruct_p          Response signal to be unpacked.
 * @param [in] RelayLinkControl     The type that defines the control of the Relay Protocol link.
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid R_RESP_SMS_RELAYLINKCONTROLSET
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_RelayLinkControlSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_RelayControl_t RelayLinkControl,
    SMS_Error_t * const Error_p);



/**
 * Response function for R_Req_SMS_RelayLinkControlSet
 *
 */
SMS_RequestStatus_t R_Resp_SMS_RelayLinkControlSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * This function is used to get the current control state of the continuity of the SMS relay protocol link
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the
 *                                    request is called in wait mode or no wait
 *                                    mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [out] RelayLinkControl_p    The type that defines the control of the Relay Protocol link.
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c v_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid R_RESP_SMS_RELAYLINKCONTROLGET
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_RelayLinkControlGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_RelayControl_t * const RelayLinkControl_p,
    SMS_Error_t * const Error_p);



/**
 * Response function for R_Req_SMS_RelayLinkControlGet
 *
 */

SMS_RequestStatus_t R_Resp_SMS_RelayLinkControlGet(
    const void *const SigStruct_p,
    SMS_RelayControl_t * const RelayLinkControl_p,
    SMS_Error_t * const Error_p);



//========================================================================

/**
 * This function is used for setting the preferred SMS store
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [in] SigStruct_p          Response signal to be unpacked.
 * @param [in] StoragePreferred     The preferred SMS Server storage
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid R_RESP_SMS_PREFERREDSTORAGESET
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_PreferredStorageSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Storage_t StoragePreferred,
    SMS_Error_t * const Error_p);



/**
 * Response function for R_Req_SMS_PreferredStorageSet
 *
 */

SMS_RequestStatus_t R_Resp_SMS_PreferredStorageSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);


/**
 * This function is used for defining the responsibility for acknowledgement
 * of MT SMS
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [in] NetworkAcknowledge   Define whether the network or client will
 *                                  send the deliver report in acknowledgement
 *                                  of the MT SMS.
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_DeliverReportControlSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_NetworkAcknowledge_t NetworkAcknowledge,
    SMS_Error_t * const Error_p);

/**
 * Response function for R_Req_SMS_DeliverReportControlSet
 *
 */
SMS_RequestStatus_t R_Resp_SMS_DeliverReportControlSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);

/**
 * This function is used for obtaining the current responsibility for
 * acknowledgement of MT SMS
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the
 *                                    request is called in wait mode or no wait
 *                                    mode.
 * @param [out] NetworkAcknowledge_p  Whether the network or client will
 *                                    send the deliver report in acknowledgement
 *                                    of MT SMS.
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_DeliverReportControlGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_NetworkAcknowledge_t * const NetworkAcknowledge_p,
    SMS_Error_t * const Error_p);

/**
 * Response function for R_Req_SMS_DeliverReportControlGet
 *
 */
SMS_RequestStatus_t R_Resp_SMS_DeliverReportControlGet(
    const void *const SigStruct_p,
    SMS_NetworkAcknowledge_t * const NetworkAcknowledge_p,
    SMS_Error_t * const Error_p);

/**
 * This function is used for setting the SMS MO route
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [in] Route                The preferred SMS Server routing
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_MO_RouteSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_MO_Route_t Route,
    SMS_Error_t * const Error_p);


/**
 * Response function for R_Req_SMS_MO_RouteSet
 *
 */
SMS_RequestStatus_t R_Resp_SMS_MO_RouteSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);

/**
 * This function is used for retrieving the MO SMS route
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [out] Route_p             The SMS Server routing
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_MO_RouteGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_MO_Route_t * const Route_p,
    SMS_Error_t * const Error_p);

/**
 * Response function for R_Req_SMS_MO_RouteGet
 *
 */
SMS_RequestStatus_t R_Resp_SMS_MO_RouteGet(
    const void *const SigStruct_p,
    SMS_MO_Route_t * const Route_p,
    SMS_Error_t * const Error_p);

/*
**========================================================================
** SMS Storage Interface Unpack function proto-types
**========================================================================
*/

//========================================================================

/**
 * This function is used to unpack a StorageStatus event from the SMS Server.
 *
 * @param [in] SigStruct_p          Event signal to be unpacked.
 * @param [out] StorageStatus_p     The actual data about the storage status
 *
 * @retval SMS_EVENT_OK                 The Unpack went okay
 * @retval SMS_EVENT_FAILED_UNPACKING   Could not unpack the data, due to some
 *                                      corruption
 * @retval SMS_EVENT_FAILED_APPLICATION Something wrong in the unpack function
 * @retval SMS_EVENT_FAILED_PARAMETER   Something wrong in the parameters
 *
 * @signalid EVENT_SMS_STORAGESTATUSCHANGED
 */

SMS_EventStatus_t Event_SMS_StorageStatusChanged(
    const void *const SigStruct_p,
    SMS_StorageStatus_t * const StorageStatus_p);

/*
**========================================================================
** SMS Interface Unpack function proto-types
**========================================================================
*/

//========================================================================

/**
 * This function is used to unpack a ServerStatus event from the SMS Server.
 *
 * @param [in] SigStruct_p             The signal being unpacked.
 * @param [out] ServerStatus_p         The actual data about the storage status
 *
 * @retval SMS_EVENT_OK                 The Unpack went okay
 * @retval SMS_EVENT_FAILED_UNPACKING   Could not unpack the data, due to some
 *                                      corruption
 * @retval SMS_EVENT_FAILED_APPLICATION Something wrong in the unpack function
 * @retval SMS_EVENT_FAILED_PARAMETER   Something wrong in the parameters
 *
 * @signalid EVENT_SMS_SERVERSTATUSCHANGED
 */
SMS_EventStatus_t Event_SMS_ServerStatusChanged(
    const void *const SigStruct_p,
    SMS_ServerStatus_t * const ServerStatus_p);


//========================================================================

/**
 * This function is used to unpack an unsolicited error.
 *
 * @param [in] SigStruct_p      Event signal to be unpacked.
 * @param [out] Error_p         The error information
 *
 * @retval SMS_EVENT_OK                 The Unpack went okay
 * @retval SMS_EVENT_FAILED_UNPACKING   Could not unpack the data, due to some
 *                                      corruption
 * @retval SMS_EVENT_FAILED_APPLICATION Something wrong in the unpack function
 * @retval SMS_EVENT_FAILED_PARAMETER   Something wrong in the parameters
 *
 * @signalid EVENT_SMS_ERROROCCURRED
 */

SMS_EventStatus_t Event_SMS_ErrorOccurred(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);


//========================================================================

/**
 * This function is used to unpack the TPDU of an SMS which has
 * been processed by the platform.
 *
 * @param [in] SigStruct_p            Event signal to be unpacked.
 * @param [out] ShortMessageData_p    The information associated with the
 *                                    SMS. This contains the storage slot
 *                                    where the short message has been stored.
 *
 * @retval SMS_EVENT_OK                 The Unpack went okay
 * @retval SMS_EVENT_FAILED_UNPACKING   Could not unpack the data, due to some
 *                                      corruption
 * @retval SMS_EVENT_FAILED_APPLICATION Something wrong in the unpack function
 * @retval SMS_EVENT_FAILED_PARAMETER   Something wrong in the parameters
 *
 * @signalid EVENT_SMS_SHORTMESSAGERECEIVED
 */

SMS_EventStatus_t Event_SMS_ShortMessageReceived(
    const void *const SigStruct_p,
    SMS_ShortMessageReceived_t * const ShortMessageData_p);


//========================================================================

/**
 * This function is used to unpack information about an SMS which
 * matches an application port subscription range.
 *
 * @param [in] SigStruct_p                  Event signal to be unpacked.
 * @param [out] ApplicationPortRange_p      The matched application port range.
 * @param [out] ShortMessageReceived_p      The information associated with the
 *                                          SMS. This contains the storage slot
 *                                          where the short message has been stored.
 *
 * @retval SMS_EVENT_OK                      The Unpack went okay
 * @retval SMS_EVENT_FAILED_UNPACKING        Could not unpack the data, due to some
 *                                           corruption
 * @retval SMS_EVENT_FAILED_APPLICATION      Something wrong in the unpack function
 * @retval SMS_EVENT_FAILED_PARAMETER        Something wrong in the parameters
 *
 * @signalid EVENT_SMS_APPLICATIONPORTMATCHED
 */

SMS_EventStatus_t Event_SMS_ApplicationPortMatched(
    const void *const SigStruct_p,
    SMS_ApplicationPortRange_t * const ApplicationPortRange_p,
    SMS_ShortMessageReceived_t * const ShortMessageReceived_p);


//========================================================================

/**
 * This function is used to unpack a Status Report for an SMS.
 *
 * @param [in] SigStruct_p               Event signal to be unpacked.
 * @param [out] Slot_p                   The slot number of the SMS that the status report is associated with.
 * @param [out] StatusReport_p           The SMSC address and status report in 164 byte TPDU format.
 *
 * @retval SMS_EVENT_OK                   The Unpack went okay
 * @retval SMS_EVENT_FAILED_UNPACKING     Could not unpack the data, due to some
 *                                        corruption
 * @retval SMS_EVENT_FAILED_APPLICATION   Something wrong in the unpack function
 * @retval SMS_EVENT_FAILED_PARAMETER     Something wrong in the parameters
 *
 * @signalid EVENT_SMS_STATUSREPORTRECEIVED
 */

SMS_EventStatus_t Event_SMS_StatusReportReceived(
    const void *const SigStruct_p,
    SMS_Slot_t * const Slot_p,
    SMS_SMSC_Address_TPDU_t * const StatusReport_p);




/*
**========================================================================
** Utility functions for receiving asynchronous data on a socket.
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     Util_SMS_SignalReceiveOnSocket
 *
 * @description  Receives a signal on the supplied socket.
 *
 * @param [in] Socket           Socket to receive data on.  Will hang on this socket until / unless data is available.
 * @param [out] Primitive_p     Returns the Signal Primitive associated with the data returned.
 * @param [out] ClientTag_p     Returns the ClientTag associated with the data returned.
 *
 * @return       void* Pointer to signal data buffer. (Must be freed by calling Util_SMS_SignalFree().)
 */
/********************************************************************/
void *Util_SMS_SignalReceiveOnSocket(
    const int Socket,
    uint32_t * const Primitive_p,
    SMS_ClientTag_t * const ClientTag_p);


//========================================================================

/********************************************************************/
/**
 *
 * @function     Util_SMS_SignalFree
 *
 * @description  Frees a signal which has been received using Util_SMS_SignalReceiveOnSocket().
 *
 * @param [in] Signal_p     Signal to be freed.
 *
 * @return       void
 */
/********************************************************************/
void Util_SMS_SignalFree(
    void *Signal_p);



// Code is being run through a linux formatting tool (indent). The existance of
// these C/C++ extern declaration would cause all code within their braces to be
// indented. To ensure that that does not happen, we disable the indent tool
// when processing them using the *INDENT-OFF* and *INDENT-ON* commands.

// *INDENT-OFF*
#ifdef __cplusplus
}
#endif
// *INDENT-ON*
#endif                          // INCLUSION_GUARD_R_SMS_H
