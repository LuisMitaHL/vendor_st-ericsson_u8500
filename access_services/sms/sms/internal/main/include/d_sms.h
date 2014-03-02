#ifndef INCLUSION_GUARD_D_SMS_H
#define INCLUSION_GUARD_D_SMS_H
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
 * SMS Server include file for exported interface dispatch functions.
 *
 *************************************************************************/

#include "t_sms.h"


/*
**========================================================================
** SMS Storage Interface Dispatch function proto-types
**========================================================================
*/


//========================================================================

/**
 * This function is called periodically to indicate the status of the SMS
 * store if the status has changed from the last time it was checked.
 *
 * @param [in] StorageStatus The data associated with the SMS store.
 *
 * @eventid EVENT_ID_SMS_STORAGESTATUSCHANGED
 * @signalid EVENT_SMS_STORAGESTATUSCHANGED
 * @datatype None - see unpack function.
 */

void Dispatch_SMS_StorageStatusChanged(
    const SMS_StorageStatus_t StorageStatus);


/*
**========================================================================
** SMS Interface Dispatch function proto-types
**========================================================================
*/


//========================================================================

/**
 * This function is called when the message storage initialization is complete,
 * to indicate the SMS Server is ready to accept all signals.
 *
 * @param [in] ServerStatus The status of the SMS Server, ie BUSY or READY
 *
 * @eventid EVENT_ID_SMS_SERVERSTATUSCHANGED
 * @signalid EVENT_SMS_SERVERSTATUSCHANGED
 * @datatype None - see unpack function.
 */

void Dispatch_SMS_ServerStatusChanged(
    const SMS_ServerStatus_t ServerStatus);


//========================================================================

/**
 * This function is called when an unsolicited Error cause is received from
 * Network signalling.
 *
 * @param [in] Error The actual data associated with the Error indication.
 *
 * @eventid EVENT_ID_SMS_ERROROCCURRED
 * @signalid EVENT_SMS_ERROROCCURRED
 * @datatype None - see unpack function.
 */

void Dispatch_SMS_ErrorOccurred(
    const SMS_Error_t Error);


//========================================================================

/**
 * This function is called when an SMS is received which has not been
 * processed by the platform. It indicates the TPDU so that it can be inspected
 * and then a Deliver Report acknowledge or negative acknowledge can be sent in
 * response depending upon the data.
 *
 * @param [in] ShortMessageData_p The SMS category, notification type,
 *                                SMSC address, TPDU and Slot Number where it is stored if applicable.
 *
 * @eventid EVENT_ID_SMS_SHORTMESSAGERECEIVED
 * @signalid EVENT_SMS_SHORTMESSAGERECEIVED
 * @datatype None - see unpack function.
 */

void Dispatch_SMS_ShortMessageReceived(
    const SMS_ShortMessageReceived_t * const ShortMessageData_p);


//========================================================================
/**
 * This function is called when an SMS Status Report is received.
 *
 * @param [in] Slot            The slot number of the SMS that the status report relates to
 * @param [in] StatusReport_p  The SMSC address and data the status report in 164 byte TPDU format.
 *
 * @eventid EVENT_ID_SMS_STATUSREPORTRECEIVED
 * @signalid EVENT_SMS_STATUSREPORTRECEIVED
 * @datatype None - see unpack function.
 */
void Dispatch_SMS_StatusReportReceived(
    const SMS_Slot_t Slot,
    const SMS_SMSC_Address_TPDU_t * const StatusReport_p);

#endif                          // INCLUSION_GUARD_D_SMS_H
