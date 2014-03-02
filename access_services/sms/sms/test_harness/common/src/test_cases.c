/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
*
* DESCRIPTION:
* Simple SMS send/receive test harness
*
*
*************************************************************************/

#include <stdint.h>
#include <stdio.h>


#include "r_sms.h"
#include "r_cbs.h"
#include "r_sms_cb_session.h"
#define LOG_TAG "SMS TEST"
#include "r_smslinuxporting.h"  // Included to get some useful utilities. However we a SMS server client shouldn't really include this file!
#include "g_sms.h"              // A client of SMS should not need this file. However we use it to get Event_SMS_ApplicationPortMatched_t, etc., to improve debug output to log.
#include "test_cases.h"
#include "test_cases_utils.h"
#include "c_sms_config.h"

// Memory Capacity Exceeded tests also validate that the EFsmss file is being updated on the SIM card
// as expected. This means we have to include the SIM API.
#include "sim.h"

//
// Application Port Test Data Type
//
typedef struct {
    SMS_SMSC_Address_TPDU_t appPortTPDU;
    SMS_ApplicationPortRange_t appPortRange;
    uint8_t eventType;
} appPortTestData;



static int RequestResponseSocket = SMS_NO_PROCESS;
static int EventSocket = SMS_NO_PROCESS;
static SMS_RequestControlBlock_t RequestCtrl = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };


static uint8_t Get_SMS_ServerStatusChangeEvent(
    const char *TestCaseName_p,
    const size_t LineNumber,
    const size_t Timeout_Seconds,
    const SMS_ServerStatus_t ExpectedStatus,
    SMS_ServerStatus_t *const ReceivedStatus_p,
    uint32_t *const NumberEventsRxd_p);
#define SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS 20
#define SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS__EVENT_NOT_EXPECT_EXPECTED 5


/**
 * Used to calculate, at compile time, the number of elements in an array.
 *
 * @param [in] array   Must be an array variable.
 *
 * @sigbased No - Macro
 */
#define SMS_TEST_ELEMENTS_OF_ARRAY(array) ((sizeof (array)) / (sizeof (array)[0]))





#ifdef SMS_PRINT_A_
/*************************************************************************
**************************************************************************
*
* Debug functions to print SMS data to log.
*
**************************************************************************
**************************************************************************/
static const char *SmsCategory_EnumtoText(
    const SMS_Category_t Category)
{
    switch (Category) {
    case SMS_CATEGORY_CLASS_ZERO:
        return "SMS_CATEGORY_CLASS_ZERO";
    case SMS_CATEGORY_STANDARD:
        return "SMS_CATEGORY_STANDARD";
    case SMS_CATEGORY_SIM_SPECIFIC:
        return "SMS_CATEGORY_SIM_SPECIFIC";
    case SMS_CATEGORY_SIM_SPECIFIC_TO_BE_ERASED:
        return "SMS_CATEGORY_SIM_SPECIFIC_TO_BE_ERASED";
    case SMS_CATEGORY_TE_SPECIFIC:
        return "SMS_CATEGORY_TE_SPECIFIC";
    case SMS_CATEGORY_ANY_CATEGORY:
        return "SMS_CATEGORY_ANY_CATEGORY";
    case SMS_CATEGORY_ME_SPECIFIC:
        return "SMS_CATEGORY_ME_SPECIFIC";
    case SMS_CATEGORY_WAITING_IN_SERVICE_CENTER:
        return "SMS_CATEGORY_WAITING_IN_SERVICE_CENTER";
    default:
        return "Unknown Category Value";
    }
}

static const char *SmsStorage_EnumtoText(
    const SMS_Storage_t Storage)
{
    switch (Storage) {
    case SMS_STORAGE_ME:
        return "SMS_STORAGE_ME";
    case SMS_STORAGE_SM:
        return "SMS_STORAGE_SM";
    case SMS_STORAGE_MT:
        return "SMS_STORAGE_MT";
    default:
        return "Unknown Storage Value";
    }
}

static void Uint8Buf_PrintToLog(
    const uint8_t *Data_p,
    const unsigned int BufferLength)
{
    if (Data_p != NULL) {
        unsigned int Index;
        size_t data_pos = 0;
        char *data_dump_p = malloc(3 * 20);

        for (Index = 0; Index < BufferLength; Index++, Data_p++) {     // Using 1-based so that Index % 16 is not TRUE for Index==0.
            if ((Index + 1) % 16 == 0) {
                SMS_LOG_D("%s", data_dump_p);
                data_pos = 0;
            }

            data_pos += sprintf(data_dump_p + data_pos, " %02X", *Data_p);
        }

        // Ensure that the last line is output
        SMS_LOG_D("%s", data_dump_p);
        free(data_dump_p);
    } else {
        SMS_LOG_E("%s: INPUT NULL", __func__);
    }
}

static void SMSTest_ReceivedSM_PrintToLog(
    const SMS_ShortMessageReceived_t *const SMS_p)
{
    SMS_A_(SMS_LOG_I("Category                                     = %d (%s)", SMS_p->Category, SmsCategory_EnumtoText(SMS_p->Category)));
    SMS_A_(SMS_LOG_I("IsReplaceType                                = %s", SMS_p->IsReplaceType ? "TRUE" : "FALSE"));
    SMS_A_(SMS_LOG_I("Slot.Storage                                 = %d (%s)", SMS_p->Slot.Storage, SmsStorage_EnumtoText(SMS_p->Slot.Storage)));
    SMS_A_(SMS_LOG_I("Slot.Position                                = %d", SMS_p->Slot.Position));
    SMS_A_(SMS_LOG_I("SMSC_Addr.ServiceCentreAddress.Length        = 0x%02x", SMS_p->SMSC_Address_TPDU.ServiceCenterAddress.Length));
    SMS_A_(SMS_LOG_I("SMSC_Addr.ServiceCentreAddress.TypeOfAddress = 0x%02x", SMS_p->SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress));
    SMS_A_(SMS_LOG_I("SMSC_Addr.ServiceCentreAddress.Value (Hex):"));
    SMS_A_(Uint8Buf_PrintToLog(SMS_p->SMSC_Address_TPDU.ServiceCenterAddress.AddressValue, SMS_p->SMSC_Address_TPDU.ServiceCenterAddress.Length));
    SMS_A_(SMS_LOG_I("SMSC_Addr.TPDU.Length                        = 0x%02x", SMS_p->SMSC_Address_TPDU.TPDU.Length));
    SMS_A_(SMS_LOG_I("SMSC_Addr.TPDU.Data (Hex):"));
    SMS_A_(Uint8Buf_PrintToLog(SMS_p->SMSC_Address_TPDU.TPDU.Data, SMS_p->SMSC_Address_TPDU.TPDU.Length));
}

static void SMSTest_PrintFrameworkIdToLog(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    const char *FrameWorkText_p;

    switch (TestCaseFrameworkId) {
    case SMS_TEST_CASE_FRAMEWORK_ID_STAND_ALONE:
        FrameWorkText_p = "Stand Alone";
        break;

    case SMS_TEST_CASE_FRAMEWORK_ID_LTP:
        FrameWorkText_p = "LTP";
        break;

    case SMS_TEST_CASE_FRAMEWORK_ID_UNKNOWN:
        FrameWorkText_p = "Unknown";
        break;

    default:
        FrameWorkText_p = "INVALID";
        break;
    }

    SMS_LOG_I("%s Running in %s Framework.", TestCaseName_p, FrameWorkText_p);
}
#endif                          // SMS_PRINT_A_

static void SMSTest_ReceivedSM_AppPort_PrintToLog(
    const Event_SMS_ApplicationPortMatched_t *const SMS_p)
{
    SMS_A_(SMS_LOG_I("Appplication Port                                = %d", SMS_p->ApplicationPortRange.ApplicationPort));
    SMS_A_(SMS_LOG_I("Lower Port                                       = %d", SMS_p->ApplicationPortRange.LowerPort));
    SMS_A_(SMS_LOG_I("Upper Port                                       = %d", SMS_p->ApplicationPortRange.UpperPort));
    SMS_A_(SMS_LOG_I("Category                                         = %d (%s)", SMS_p->Data.Category, SmsCategory_EnumtoText(SMS_p->Data.Category)));
    SMS_A_(SMS_LOG_I("IsReplaceType                                    = %s", SMS_p->Data.IsReplaceType ? "TRUE" : "FALSE"));
    SMS_A_(SMS_LOG_I("Slot.Storage                                     = %d (%s)", SMS_p->Data.Slot.Storage, SmsStorage_EnumtoText(SMS_p->Data.Slot.Storage)));
    SMS_A_(SMS_LOG_I("Slot.Position                                    = %d", SMS_p->Data.Slot.Position));
    SMS_A_(SMS_LOG_I("SMSSMS_C_Addr.ServiceCentreAddress.Length        = 0x%02x", SMS_p->Data.SMSC_Address_TPDU.ServiceCenterAddress.Length));
    SMS_A_(SMS_LOG_I("SMSSMS_C_Addr.ServiceCentreAddress.TypeOfAddress = 0x%02x", SMS_p->Data.SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress));
    SMS_A_(SMS_LOG_I("SMSSMS_C_Addr.ServiceCentreAddress.Value (Hex):"));
    SMS_A_(Uint8Buf_PrintToLog(SMS_p->Data.SMSC_Address_TPDU.ServiceCenterAddress.AddressValue, SMS_p->Data.SMSC_Address_TPDU.ServiceCenterAddress.Length));
    SMS_A_(SMS_LOG_I("SMSSMS_C_Addr.TPDU.Length                        = 0x%02x", SMS_p->Data.SMSC_Address_TPDU.TPDU.Length));
    SMS_A_(SMS_LOG_I("SMSSMS_C_Addr.TPDU.Data (Hex):"));
    SMS_A_(Uint8Buf_PrintToLog(SMS_p->Data.SMSC_Address_TPDU.TPDU.Data, SMS_p->Data.SMSC_Address_TPDU.TPDU.Length));
}

static void CB_Message_HexPrint(
    const CBS_Message_t *Message_p)
{
    if (NULL != Message_p) {
        uint16_t loop;
        char *CharBuffer_p = NULL;
        char *PrintBuffer_p = NULL;

        SMS_LOG_I("CBS Test: CB_Page_HexPrint: SerialNumber     : 0x%04x", Message_p->Header.SerialNumber);
        SMS_LOG_I("CBS Test: CB_Page_HexPrint: MID              : 0x%04x", Message_p->Header.MessageIdentifier);
        SMS_LOG_I("CBS Test: CB_Page_HexPrint: DataCodingScheme : 0x%02x", Message_p->Header.DataCodingScheme);
        SMS_LOG_I("CBS Test: CB_Page_HexPrint: PageParameter    : 0x%02x", Message_p->Header.PageParameter);

        // To reduce the number of prints, we print to a buffer first - Allocate some memory :-
        // +1 for the Null terminator
        PrintBuffer_p = malloc(CBS_MESSAGE_MAX_PAGE_LENGTH + 1);

        if (PrintBuffer_p != NULL) {
            CharBuffer_p = PrintBuffer_p;

            for (loop = 0; loop < CBS_MESSAGE_MAX_PAGE_LENGTH; loop++) {
                sprintf(CharBuffer_p, " %02X", (uint8_t) Message_p->Data[loop]);
                CharBuffer_p += 3;
            }

            SMS_LOG_I("CBS Test: CB_Message_HexPrint: CB Message =%s", PrintBuffer_p);

            free(PrintBuffer_p);
        }
    }
}

#ifndef REMOVE_SMSTRG_LAYER
/* This test case is used to test the functionality of deleting the short message in the given
 *               storage slot.*/
static uint8_t SMSTest_Util_DeleteAllShortMessages(
    void)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint16_t SMS_Total = 0;
    SMS_SlotInformation_t SlotInfo = { 0, 0, 0, 0, 0 };
    SMS_StorageStatus_t StorageStatus = { FALSE, FALSE };
    SMS_Slot_t Slot = { SMS_STORAGE_MT, SMS_STORAGE_POSITION_INVALID };
    SMS_SearchInfo_t SearchInfo = { SMS_STATUS_ANY_STATUS, Slot, SMS_BROWSE_OPTION_FIRST };
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = Request_SMS_ShortMessageFind(&RequestCtrl_temp,  // Global RequestCtrl variable which contains socket to be used.
                    &SearchInfo, &Slot, &ErrorCode);

    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = Request_SMS_ShortMessageFind(&RequestCtrl_temp,  // Global RequestCtrl variable which contains socket to be used.
                    &SearchInfo, &Slot, &ErrorCode);

    RequestStatus = Request_SMS_ShortMessageFind(NULL,  // Global RequestCtrl variable which contains socket to be used.
                    &SearchInfo, &Slot, &ErrorCode);

    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_SMS_ShortMessageFind(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SearchInfo, &Slot, &ErrorCode);

    if (RequestStatus != SMS_REQUEST_OK || (ErrorCode != SMS_ERROR_INTERN_NO_ERROR && ErrorCode != SMS_ERROR_INTERN_MESSAGE_NOT_FOUND)) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
    }

    while (SMS_STORAGE_POSITION_INVALID != Slot.Position && ErrorCode != SMS_ERROR_INTERN_MESSAGE_NOT_FOUND) {
        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_SMS_ShortMessageDelete(&RequestCtrl_temp,    // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &ErrorCode);

        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = Request_SMS_ShortMessageDelete(&RequestCtrl_temp,    // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &ErrorCode);

        RequestStatus = Request_SMS_ShortMessageDelete(NULL,    // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &ErrorCode);
        RequestStatus = Request_SMS_ShortMessageDelete(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &ErrorCode);

        if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
            Result = FALSE;
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }

        RequestStatus = Request_SMS_ShortMessageFind(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used.
                        &SearchInfo, &Slot, &ErrorCode);

        if (RequestStatus != SMS_REQUEST_OK || (ErrorCode != SMS_ERROR_INTERN_NO_ERROR && ErrorCode != SMS_ERROR_INTERN_MESSAGE_NOT_FOUND)) {
            Result = FALSE;
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl_temp,        // Global RequestCtrl variable which contains socket to be used.
                    SMS_STORAGE_MT, &SlotInfo, &StorageStatus, &ErrorCode);
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl_temp,        // Global RequestCtrl variable which contains socket to be used.
                    SMS_STORAGE_MT, &SlotInfo, &StorageStatus, &ErrorCode);

    RequestStatus = Request_SMS_StorageCapacityGet(NULL,        // Global RequestCtrl variable which contains socket to be used.
                    SMS_STORAGE_MT, &SlotInfo, &StorageStatus, &ErrorCode);

    RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,        // Global RequestCtrl variable which contains socket to be used.
                    SMS_STORAGE_MT, &SlotInfo, &StorageStatus, &ErrorCode);

    SMS_Total = SlotInfo.ReadCount + SlotInfo.UnreadCount + SlotInfo.SentCount + SlotInfo.UnsentCount;

    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
    } else if (SMS_Total != 0) {
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        SMS_A_(SMS_LOG_I("Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", StorageStatus.StorageFullSIM, StorageStatus.StorageFullME));
        SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                         SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
        SMS_A_(SMS_LOG_E("ERROR: SMSTest_Util_DeleteAllShortMessages( %d SMS still exist !", SMS_Total));
        Result = FALSE;
    }

    return Result;
}
#endif

/*************************************************************************
**************************************************************************
*
* SMSTest_Initialise
*
* Initialise connection to SMS Server
*
**************************************************************************
**************************************************************************/
uint8_t SMSTest_Initialise(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Subscribe to SMS Server events and connect to sockets
    RequestStatus = Request_SMS_CB_SessionCreate(&RequestResponseSocket, &EventSocket, &ErrorCode);
    Response_SMS_CB_SessionCreate(NULL, NULL);

    SMS_A_(SMS_LOG_I("%s: Request_SMS_CB_SessionCreate, RequestStatus = 0x%08x, ErrorCode = 0x%08x, RequestResponseSocket = 0x%08x, EventSocket = 0x%08x",
                     TestCaseName_p, RequestStatus, ErrorCode, RequestResponseSocket, EventSocket));

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        // Initialised and update global RequestCtrl blocks used in this test harness.
        Do_SMS_RequestControlBlock_Init(&RequestCtrl);
        RequestCtrl.Socket = RequestResponseSocket;
    } else {
        Result = FALSE;
    }

    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;

    // Subscribe to SMS Server MT SMS and Status Report events
    RequestStatus = Request_SMS_EventsSubscribe(&RequestCtrl_temp, &ErrorCode);

    RequestCtrl_temp.WaitMode = 4;

    // Subscribe to SMS Server MT SMS and Status Report events
    RequestStatus = Request_SMS_EventsSubscribe(&RequestCtrl_temp, &ErrorCode);

    // Subscribe to SMS Server MT SMS and Status Report events
    RequestStatus = Request_SMS_EventsSubscribe(NULL, &ErrorCode);

    if (Result) {
        // Ensure global parameter configured for wait mode.
        RequestCtrl.WaitMode = SMS_WAIT_MODE;

        // Subscribe to SMS Server MT SMS and Status Report events
        RequestStatus = Request_SMS_EventsSubscribe(&RequestCtrl, &ErrorCode);
        Response_SMS_EventsSubscribe(NULL, NULL);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_EventsSubscribe, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
            Result = FALSE;
        }
    }

#ifndef REMOVE_SMSTRG_LAYER
    // Delete any SMS that may have been left from the previous test run
    (void) SMSTest_Util_DeleteAllShortMessages();
#endif

#ifdef SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED

    // If we have enabled memory macro tracking in the SMS Server we should force a dump of the memory
    // allocation status now; i.e. before all the tests are run.
    if (Result) {
        RequestStatus = Request_SMS_TestUtil_MemoryStatusGet(&RequestCtrl);

        if (RequestStatus != SMS_REQUEST_OK) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_TestUtil_MemoryStatusGet() returned %d", TestCaseName_p, RequestStatus));
            Result = FALSE;
        }
    } else {
        SMS_A_(SMS_LOG_I("%s: Request_SMS_TestUtil_MemoryStatusGet() not called as commuications to SMS Server not available", TestCaseName_p));
    }

#endif

    return Result;
}


/*************************************************************************
**************************************************************************
*
* SMSTest_Uninitialise
*
* Uninitialise connection to SMS Server
*
**************************************************************************
**************************************************************************/
uint8_t SMSTest_Uninitialise(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

#ifndef REMOVE_SMSTRG_LAYER
    // Delete any SMS that may have been left from the this test run
    (void) SMSTest_Util_DeleteAllShortMessages();
#endif

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

#ifdef SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED
    // If we have enabled memory macro tracking in the SMS Server we should force a dump of the memory
    // allocation status now; i.e. after all the tests are run, but before communications with the SMS
    // Server are torn down.
    {
        // As we are about to dump the SMS Memory macro data to the log, we need to be sure that the SMS Server
        // has finished everything and all its timeouts have expired.
        uint32_t Timeout = SMS_DELIVER_REPORT_TIMEOUT_VALUE + 5;        // +5 to ensure that timeout does expire and any mal callbacks also take place.

        SMS_A_(SMS_LOG_I("%s: Wait %d secs before calling Request_SMS_TestUtil_MemoryStatusGet()", TestCaseName_p, Timeout));
        sleep(Timeout);
        SMS_A_(SMS_LOG_I("%s: Timeout before calling Request_SMS_TestUtil_MemoryStatusGet() EXPIRED", TestCaseName_p));
    }
    RequestStatus = Request_SMS_TestUtil_MemoryStatusGet(&RequestCtrl);

    if (RequestStatus != SMS_REQUEST_OK) {
        SMS_A_(SMS_LOG_I("%s: Request_SMS_TestUtil_MemoryStatusGet() returned %d", TestCaseName_p, RequestStatus));
    }

#endif

    RequestStatus = Request_SMS_EventsUnsubscribe(NULL, &ErrorCode);
    // Subscribe to SMS Server MT SMS and Status Report events
    RequestStatus = Request_SMS_EventsUnsubscribe(&RequestCtrl, &ErrorCode);
    Response_SMS_EventsUnsubscribe(NULL, NULL);


    SMS_A_(SMS_LOG_I("%s: Request_SMS_EventsUnsubscribe, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        Result = FALSE;
    }

    RequestStatus = Request_SMS_CB_SessionDestroy(&RequestCtrl, EventSocket, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: Request_SMS_CB_SessionDestroy, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestResponseSocket = SMS_NO_PROCESS;
        EventSocket = SMS_NO_PROCESS;
        RequestCtrl.WaitMode = SMS_WAIT_MODE;
        RequestCtrl.ClientTag = SMS_CLIENT_TAG_NOT_USED;
        RequestCtrl.Socket = SMS_NO_PROCESS;
    } else {
        Result = FALSE;
    }

    return Result;
}

/*************************************************************************
**************************************************************************
*
* SMSTest_Shutdown
*
* Shutdown connection to SMS Server
*
**************************************************************************
**************************************************************************/
uint8_t SMSTest_Shutdown(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    // Unsubscribe to SMS Server MT SMS and Status Report events
    RequestStatus = Request_SMS_EventsUnsubscribe(&RequestCtrl, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: Request_SMS_EventsUnsubscribe, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        Result = FALSE;
    }

    RequestStatus = Request_SMS_CB_Shutdown(&RequestCtrl, EventSocket);

    SMS_A_(SMS_LOG_I("%s: Request_SMS_CB_Shutdown, RequestStatus = 0x%08x ", TestCaseName_p, RequestStatus));


    if (RequestStatus == SMS_REQUEST_OK) {
        RequestResponseSocket = SMS_NO_PROCESS;
        EventSocket = SMS_NO_PROCESS;
        RequestCtrl.WaitMode = SMS_WAIT_MODE;
        RequestCtrl.ClientTag = SMS_CLIENT_TAG_NOT_USED;
        RequestCtrl.Socket = SMS_NO_PROCESS;
    } else {
        Result = FALSE;
    }

    return Result;
}

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_Send_ZeroLengthTPDU
*
* Test case for Sending hardcoded Zero Length MO SMS. This test confirms that
* the SMS Server traps the condition but still sends an error response to the
* calling client.
*
 __________________________________________________________________________ Service Centre Address
|
07916407058099f9

 __________________________________________________ Deliver length
|  ________________________________________________ TP_MTI_MTS_VPF
| |  ______________________________________________ TP MR
| | |  ____________________________________________ Sender Address
| | | |          __________________________________ PID
| | | |         |  ________________________________ Data Coding Scheme
| | | |         | |  ______________________________ Validity Period (Absolute)
| | | |         | | |              ________________ User Data Length (septets)
| | | |         | | |             |  ______________ User Data Header
| | | |         | | |             | |        ______ Total SMs
| | | |         | | |             | |       |  ____ SM part number
| | | |         | | |             | |       | |  __ 7 bit user data
| | | |         | | |             | |       | | |
000000000000000000000000000000000000000000000000000000000000
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_Send_ZeroLengthTPDU(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_ZeroLength = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            0,                     // Indicates the length of the TPDU data.
            {0x00}                 // TPDU data
        }
    };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU_ZeroLength, NULL,   // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_UNSPECIFIED_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        Result = TRUE;
    }

    /*  testing condition SMSC_Address_TPDU_ZeroLength is NULL */

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,// Global RequestCtrl variable which contains socket to be used.
                    NULL, NULL,// Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_UNSPECIFIED_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        Result = TRUE;
    }

    return Result;
}


/**
 * This test case is to test the functionality of Sending an SMS to the network.
 * If the SMS TPDU parameter is not NULL
 * the TPDU data is sent to the network without storing to an SMS slot and the
 * SMS slot parameter is ignored. If the SMS TPDU parameter is NULL the TPDU
 * data stored at the given SMS slot is sent to the network. The more to send
 * parameter is used to request that a series of non-concatenated SMS are to
 * be sent together on the same radio link.
 */

uint8_t SMSTest_SMS_Send_RequestCtrl_P(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_ZeroLength = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            0,                     // Indicates the length of the TPDU data.
            {0x00}                 // TPDU data
        }
    };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Send RequestCtrl is wrong value.
    RequestCtrl.WaitMode = 3;

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU_ZeroLength, NULL,   // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_UNSPECIFIED_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        Result = TRUE;
    }

    /*  testing condition RequestCtrl is NULL */

    RequestStatus = Request_SMS_ShortMessageSend(NULL,// Global RequestCtrl variable which contains socket to be used.
                    NULL, NULL,// Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_UNSPECIFIED_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        Result = TRUE;
    }

    return Result;
}



/**
 * This test case is to test the functionality of Sending an SMS to the network.
 * Address type is having different value here
 */

uint8_t SMSTest_SMS_Send_RequestCtrl_P1(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_ZeroLength = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x57,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            0,                     // Indicates the length of the TPDU data.
            {0x00}                 // TPDU data
        }
    };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Send RequestCtrl is wrong value.
    RequestCtrl.WaitMode = 3;

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU_ZeroLength, NULL,   // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_UNSPECIFIED_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        Result = TRUE;
    }

    /*  testing condition RequestCtrl is NULL */

    RequestStatus = Request_SMS_ShortMessageSend(NULL,// Global RequestCtrl variable which contains socket to be used.
                    NULL, NULL,// Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_UNSPECIFIED_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        Result = TRUE;
    }

    return Result;
}

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_Send_PPDN
*
* Test case for Sending hardcoded MO SMS
*
 __________________________________________________________________________ Service Centre Address
|
07916407058099f9

 __________________________________________________ Deliver length
|  ________________________________________________ TP_MTI_MTS_VPF
| |  ______________________________________________ TP MR
| | |  ____________________________________________ Sender Address
| | | |          __________________________________ PID
| | | |         |  ________________________________ Data Coding Scheme
| | | |         | |  ______________________________ Validity Period (Absolute)
| | | |         | | |              ________________ User Data Length (septets)
| | | |         | | |             |  ______________ User Data Header
| | | |         | | |             | |        ______ Total SMs
| | | |         | | |             | |       |  ____ SM part number
| | | |         | | |             | |       | |  __ 7 bit user data
| | | |         | | |             | |       | | |
1D790106812143657FF602041A08271E040D050003010101A061391D1403
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_Send_PPDN(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            0x1D,                  // Indicates the length of the TPDU data.
            {0x79, 0x01, 0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1, 0x02, 0x04, 0x1A, 0x08, 0x27, 0x1E, 0x04, 0x0D, 0x05, 0x00, 0x03, 0x01, 0x01, 0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03} // TPDU data
        }
    };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU, NULL,      // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    sleep(5);                   // keep the test in synchronisation

    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        Result = TRUE;
    }

    return Result;
}

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_Send_OTADP
*
* Test case for Sending hardcoded MO SMS
*
 __________________________________________________________________________ Service Centre Address
|
07916407058099f9

 __________________________________________________ Deliver length
|  ________________________________________________ TP_MTI_MTS_VPF
| |  ______________________________________________ TP MR
| | |  ____________________________________________ Sender Address
| | | |          __________________________________ PID
| | | |         |  ________________________________ Data Coding Scheme
| | | |         | |  ______________________________ Validity Period (Absolute)
| | | |         | | |              ________________ User Data Length (septets)
| | | |         | | |             |  ______________ User Data Header
| | | |         | | |             | |        ______ Total SMs
| | | |         | | |             | |       |  ____ SM part number
| | | |         | | |             | |       | |  __ 7 bit user data
| | | |         | | |             | |       | | |
1D790106812143657e1102041A082714040D050003120101A061391D1403
**************************************************************************
**************************************************************************/
//
uint8_t SMSTest_SMS_Send_OTADP(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            0x1D,                  // Indicates the length of the TPDU data.
            {0x79, 0x01, 0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1, 0x02, 0x04, 0x1A, 0x08, 0x27, 0x1E, 0x04, 0x0D, 0x05, 0x00, 0x03, 0x01, 0x01, 0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03} // TPDU data
        }
    };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU, NULL,      // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    sleep(5);                   // keep the test in synchronisation

    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        Result = TRUE;
    }

    return Result;
}

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_Send_TPDU_WaitMode
*
* Test case for Sending MO SMS
*
* @param TPDU_p - if this is not null then this arg will over-ride the static version
*
 __________________________________________________________________________ Service Centre Address
|
07916407058099f9

 __________________________________________________ Deliver length
|  ________________________________________________ TP_MTI_MTS_VPF
| |  ______________________________________________ TP MR
| | |  ____________________________________________ Sender Address
| | | |          __________________________________ PID
| | | |         |  ________________________________ Data Coding Scheme
| | | |         | |  ______________________________ Validity Period (Absolute)
| | | |         | | |              ________________ User Data Length (septets)
| | | |         | | |             |  ______________ User Data Header
| | | |         | | |             | |        ______ Total SMs
| | | |         | | |             | |       |  ____ SM part number
| | | |         | | |             | |       | |  __ 7 bit user data
| | | |         | | |             | |       | | |
1D7901068121436500F102041A08271E040D050003010101A061391D1403
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_Send_TPDU_WaitMode(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId,
    SMS_SMSC_Address_TPDU_t *TPDU_p)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;

    SMS_SMSC_Address_TPDU_t *MO_TPDU_p = NULL;

    SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x55,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            29,                    // Indicates the length of the TPDU data.
            {
                0x79, 0x01, 0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1, 0x02, 0x04,
                0x1A, 0x08, 0x27, 0x1E, 0x04, 0x0D, 0x05, 0x00, 0x03, 0x01, 0x01,
                0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03
            }     // TPDU data
        }
    };

    if (TPDU_p != NULL) {
        MO_TPDU_p = TPDU_p;
    } else {
        MO_TPDU_p = &SMSC_Address_TPDU;
    }

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    MO_TPDU_p, NULL,       // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        Result = TRUE;
    }

    return Result;
}

/*
 * This test case is used to test the fucnationality of setting the prefered storage
 * for short message store.
/********************************************************************/


uint8_t SMSTest_SMS_PreferredStorageSet(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t i, Result = FALSE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMS_Storage_t sms_storage = SMS_STORAGE_ME;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));

    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;

    RequestStatus = R_Req_SMS_PreferredStorageSet(&RequestCtrl_temp, sms_storage, &ErrorCode);
    RequestCtrl_temp.WaitMode = 4;

    RequestStatus = R_Req_SMS_PreferredStorageSet(&RequestCtrl_temp, sms_storage, &ErrorCode);
    RequestStatus = R_Req_SMS_PreferredStorageSet(NULL, sms_storage, &ErrorCode);

    RequestStatus = R_Req_SMS_PreferredStorageSet(&RequestCtrl, sms_storage, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_PreferredStorageSet PASSED", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: R_Req_SMS_PreferredStorageSet FAILED", TestCaseName_p));
        Result = FALSE;
    }

    //}

    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));
    return Result;
}

/* This test case is used to test the functionality of controlling of the continuity
  *of the SMS relay protocol link.*/

uint8_t SMSTest_SMS_RelayLinkControlSet(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = FALSE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMS_RelayControl_t RelayLinkControl = SMS_RELAY_CONTROL_DISABLED;

    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));

    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = R_Req_SMS_RelayLinkControlSet(&RequestCtrl_temp, RelayLinkControl, &ErrorCode);

    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = R_Req_SMS_RelayLinkControlSet(&RequestCtrl_temp, RelayLinkControl, &ErrorCode);
    RequestStatus = R_Req_SMS_RelayLinkControlSet(NULL, RelayLinkControl, &ErrorCode);


    RequestStatus = R_Req_SMS_RelayLinkControlSet(&RequestCtrl, RelayLinkControl, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlSet PASSED", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: R_Req_SMS_RelayLinkControlSet FAILED", TestCaseName_p));
        Result = FALSE;
    }


    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));
    return Result;
}

/* This test case is used to test the functionality of reading the information of a Short Message. The information
 *               about the message that can be determined without having to parse the TPDUs
 *               of that message is returned.*/

uint8_t SMSTest_SMS_ShortMessageInfoRead(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{

    uint8_t Result = FALSE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Slot_t Slot = { SMS_STORAGE_MT, SMS_STORAGE_POSITION_INVALID };
    SMS_ReadInformation_t ReadInformation_p = {SMS_STATUS_READ, 5, 2, 8,
                          5, 2, 8,
                          0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0
                                              };
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);
    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus =  Request_SMS_ShortMessageInfoRead(&RequestCtrl, &Slot, &ReadInformation_p, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlSet PASSED", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: R_Req_SMS_RelayLinkControlSet FAILED", TestCaseName_p));
        Result = FALSE;
    }

    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));
    return Result;


}

/*This test case is used to test the functionality of reading the status report associated
 *               with the given short message storage slot.*/

uint8_t SMSTest_SMS_StatusReportRead(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{


    uint8_t Result = FALSE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Slot_t Slot = { SMS_STORAGE_MT, SMS_STORAGE_POSITION_INVALID };
    SMS_StatusReport_t StatusReport_p = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus =  Request_SMS_StatusReportRead(&RequestCtrl, &Slot, &StatusReport_p, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlSet PASSED", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: R_Req_SMS_RelayLinkControlSet FAILED", TestCaseName_p));
        Result = FALSE;
    }

    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));
    return Result;


}

/* This test case is used to test the functionality of setting the status of the short message in
 *a given slot. i.e. read,to be read or sent,etc.*/

uint8_t SMSTest_SMS_ShortMessageStatusSet(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{


    uint8_t Result = FALSE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Slot_t Slot = { SMS_STORAGE_MT, SMS_STORAGE_POSITION_INVALID };
    SMS_Status_t Status = SMS_STATUS_READ;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus =  Request_SMS_ShortMessageStatusSet(&RequestCtrl, &Slot, Status, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlSet PASSED", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: R_Req_SMS_RelayLinkControlSet FAILED", TestCaseName_p));
        Result = FALSE;
    }

    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));
    return Result;


}

/* This test case is used to test the functionality of shutdowning the sms_server. */

uint8_t SMSTest_SMS_CB_Shutdown(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{


    uint8_t Result = FALSE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus =  Request_SMS_CB_Shutdown(&RequestCtrl, 1);

    if (RequestStatus == SMS_REQUEST_OK) {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlSet PASSED", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: R_Req_SMS_RelayLinkControlSet FAILED", TestCaseName_p));
        Result = FALSE;
    }

    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));
    return Result;


}



/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_Send_WaitMode
*
* Test case for Sending hardcoded MO SMS
*
* @param TPDU_p - if this is not null then this arg will over-ride the static version
*
 __________________________________________________________________________ Service Centre Address
|
07916407058099f9

 __________________________________________________ Deliver length
|  ________________________________________________ TP_MTI_MTS_VPF
| |  ______________________________________________ TP MR
| | |  ____________________________________________ Sender Address
| | | |          __________________________________ PID
| | | |         |  ________________________________ Data Coding Scheme
| | | |         | |  ______________________________ Validity Period (Absolute)
| | | |         | | |              ________________ User Data Length (septets)
| | | |         | | |             |  ______________ User Data Header
| | | |         | | |             | |        ______ Total SMs
| | | |         | | |             | |       |  ____ SM part number
| | | |         | | |             | |       | |  __ 7 bit user data
| | | |         | | |             | |       | | |
1D7901068121436500F102041A08271E040D050003010101A061391D1403
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_Send_WaitMode(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    return SMSTest_SMS_Send_TPDU_WaitMode(TestCaseName_p, TestCaseFrameworkId, NULL);
}

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_Send_NoWaitMode
*
* Test case for Sending hardcoded MO SMS
*
 __________________________________________________________________________ Service Centre Address
|
07916407058099f9

 __________________________________________________ Deliver length
|  ________________________________________________ TP_MTI_MTS_VPF
| |  ______________________________________________ TP MR
| | |  ____________________________________________ Sender Address
| | | |          __________________________________ PID
| | | |         |  ________________________________ Data Coding Scheme
| | | |         | |  ______________________________ Validity Period (Absolute)
| | | |         | | |              ________________ User Data Length (septets)
| | | |         | | |             |  ______________ User Data Header
| | | |         | | |             | |        ______ Total SMs
| | | |         | | |             | |       |  ____ SM part number
| | | |         | | |             | |       | |  __ 7 bit user data
| | | |         | | |             | |       | | |
1D7901068121436500F102041A08271E040D050003010101A061391D1403
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_Send_NoWaitMode(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    const SMS_ClientTag_t RequestClientTag = 42;
    uint8_t Result = FALSE;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            29,                    // Indicates the length of the TPDU data.
            {0x79, 0x01, 0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1, 0x02, 0x04, 0x1A, 0x08, 0x27, 0x1E, 0x04, 0x0D, 0x05, 0x00, 0x03, 0x01, 0x01, 0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03} // TPDU data
        }
    };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Ensure global parameter configured for no wait mode.
    RequestCtrl.WaitMode = SMS_NO_WAIT_MODE;
    RequestCtrl.ClientTag = RequestClientTag;

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU, NULL,      // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_PENDING && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: Wait for Response Signal from SMS Server", TestCaseName_p));

        while (TRUE) {
            // Now wait for asynchronous response.
            static const SMS_SIGSELECT SignalSelector[] = { 1, RESPONSE_SMS_SHORTMESSAGESEND };
            void *Rec_p;

            // Wait for response from the SMS Server
            Rec_p = SMS_RECEIVE_ON_SOCKET(SignalSelector, RequestResponseSocket);

            if (Rec_p != NULL) {
                if (((union SMS_SIGNAL *) Rec_p)->Primitive == RESPONSE_SMS_SHORTMESSAGESEND) {
                    // Get the Client Tag
                    SMS_ClientTag_t ClientTag;

                    Do_SMS_ClientTag_Get(Rec_p, &ClientTag);

                    // Unpack the returned signal into the output parameters
                    RequestStatus = Response_SMS_ShortMessageSend(Rec_p, &SM_Reference, &ErrorCode);

                    SMS_A_(SMS_LOG_I("%s: RESPONSE_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x, ClientTag = %d", TestCaseName_p, RequestStatus, ErrorCode, ClientTag));

                    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && ClientTag == RequestClientTag) {
                        SMS_A_(SMS_LOG_I("%s: RESPONSE_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
                        Result = TRUE;
                    }

                    SMS_SIGNAL_FREE(&Rec_p);
                    // Exit while loop.
                    break;
                } else {
                    SMS_A_(SMS_LOG_E("%s: SMS_RECEIVE_ON_SOCKET returned unexpected primitive 0x%08X", TestCaseName_p, (unsigned int)((union SMS_SIGNAL *) Rec_p)->Primitive));
                    SMS_SIGNAL_FREE(&Rec_p);
                }
            } else {
                SMS_A_(SMS_LOG_E("%s: SMS_RECEIVE_ON_SOCKET returned NULL", TestCaseName_p));
            }
        }
    }

    return Result;
}


/* Deliver Report Data:
#____ RP Error Cause
#    ____ Data Length (bytes)
#   |  ____ Message Type Indicator
#   | |  ____ Parameter Indicator 00000111 indicates User Data, DCS & PID present
#   | | |  ____ PID
#   | | | |  ____ Data Coding Scheme
#   | | | | |
000004000300F1
*/
static const SMS_TPDU_t DeliverReportTPDU = {
    0x04,                       // Length
    {0x00, 0x03, 0x00, 0xF1}    // Data
};

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_Receive
*
* Receive asynchronous MT SMS
*
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_Receive_Ack(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId,
    const uint8_t Acknowledge)
{
    uint8_t DeliverReportResult = FALSE;
    uint8_t UnpackResult = FALSE;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: Wait for SMS Event Signal from SMS Server", TestCaseName_p));

    while (TRUE) {
        /**************************************************************************************************/
        /* Use the low level Linux system commands to Listen out for an event                             */
        /**************************************************************************************************/

        fd_set rfds;
        int retval = 0;
        union SMS_SIGNAL *ReceivedSignal_p = NULL;
        uint32_t primitive = 0;

        FD_ZERO(&rfds);
        FD_SET(EventSocket, &rfds);

        retval = select(EventSocket + 1, &rfds, NULL, NULL, NULL);

        SMS_A_(SMS_LOG_D("select returned = %d", retval));

        if (retval > 0) {       // got something to read
            ReceivedSignal_p = Util_SMS_SignalReceiveOnSocket(EventSocket, &primitive, NULL);
        }                       // end got something to read

        if (ReceivedSignal_p != NULL) {
            if (primitive == EVENT_SMS_SHORTMESSAGERECEIVED) {
                SMS_ShortMessageReceived_t *RxdSMS_p;

                SMS_A_(SMS_LOG_I("%s: select() returned EVENT_SMS_SHORTMESSAGERECEIVED", TestCaseName_p));

                RxdSMS_p = malloc(sizeof(SMS_ShortMessageReceived_t));

                if (RxdSMS_p != NULL) {
                    SMS_EventStatus_t EventStatus;

                    memset(RxdSMS_p, '\0', sizeof(SMS_ShortMessageReceived_t));

                    // Unpack event
                    EventStatus = Event_SMS_ShortMessageReceived(ReceivedSignal_p, RxdSMS_p);

                    if (EventStatus == SMS_EVENT_OK) {
                        UnpackResult = TRUE;
                        SMS_A_(SMSTest_ReceivedSM_PrintToLog(RxdSMS_p));
                    } else {
                        SMS_A_(SMS_LOG_E("%s: EVENT_SMS_SHORTMESSAGERECEIVED failed %d", TestCaseName_p, EventStatus));
                    }

                    free(RxdSMS_p);
                } else {
                    SMS_A_(SMS_LOG_E("%s: Malloc failed", TestCaseName_p));
                }

                // Respond to event by sending Deliver Report
                if (Acknowledge) {
                    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
                    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

                    // Ensure global parameter configured for wait mode.
                    RequestCtrl.WaitMode = SMS_WAIT_MODE;

                    RequestStatus = Request_SMS_DeliverReportSend(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                                    0x0000,       //RP_ErrorCause
                                    &DeliverReportTPDU, &ErrorCode);

                    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_DELIVERREPORTSEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

                    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                        DeliverReportResult = TRUE;
                    }
                } else {
                    DeliverReportResult = TRUE;
                }

                Util_SMS_SignalFree(ReceivedSignal_p);  // free sig buffer
                // Exit while loop.
                break;
            } else {
                SMS_A_(SMS_LOG_E("%s: select() returned unexpected primitive 0x%08X", TestCaseName_p, (unsigned int) ReceivedSignal_p->Primitive));
            }

            Util_SMS_SignalFree(ReceivedSignal_p);      // free sig buffer
        } else {
            SMS_A_(SMS_LOG_E("%s: select() returned NULL", TestCaseName_p));
        }
    }

    return DeliverReportResult && UnpackResult;
}

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_Receive
*
* Receive asynchronous MT SMS
*
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_Receive(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    const uint8_t ACK = TRUE;
    return SMSTest_SMS_Receive_Ack(TestCaseName_p, TestCaseFrameworkId, ACK);
}


static const SMS_SMSC_Address_TPDU_t Concat_SMS_Part1 = {
    {
        // Address
        0x05,                      // The number of bytes in the AddressValue.
        0x91,                      // Type of address as defined in the standards.
        {0x44, 0x52, 0x78, 0x63, 0xF5}     // The address value in packed format.
    },
    {
        // TPDU
        0x1A,                      // Indicates the length of the TPDU data.
        {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x99, 0x03, 0x10, 0x89, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x01, 0xa0, 0x61, 0x39, 0x1d, 0x14, 0x03}       // TPDU data
        //     {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x89, 0x01, 0x31, 0x58, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x01, 0xa0, 0x61, 0x39, 0x1d, 0x14, 0x03} // TPDU data
    }
};

static const SMS_SMSC_Address_TPDU_t Concat_SMS_Part2 = {
    {
        // Address
        0x05,                      // The number of bytes in the AddressValue.
        0x91,                      // Type of address as defined in the standards.
        {0x44, 0x52, 0x78, 0x63, 0xF5}     // The address value in packed format.
    },
    {
        // TPDU
        0x1A,                      // Indicates the length of the TPDU data.
        {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x99, 0x03, 0x10, 0x89, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x02, 0xa0, 0x61, 0x39, 0x1d, 0x24, 0x03}       // TPDU data
        //     {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x89, 0x01, 0x31, 0x58, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x02, 0xa0, 0x61, 0x39, 0x1d, 0x24, 0x03} // TPDU data
    }
};

static const SMS_SMSC_Address_TPDU_t Concat_SMS_Part3 = {
    {
        // Address
        0x05,                      // The number of bytes in the AddressValue.
        0x91,                      // Type of address as defined in the standards.
        {0x44, 0x52, 0x78, 0x63, 0xF5}     // The address value in packed format.
    },
    {
        // TPDU
        0x1A,                      // Indicates the length of the TPDU data.
        {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x99, 0x03, 0x10, 0x89, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x03, 0xa0, 0x61, 0x39, 0x1d, 0x34, 0x03}       // TPDU data
        //     {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x89, 0x01, 0x31, 0x58, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x03, 0xa0, 0x61, 0x39, 0x1d, 0x34, 0x03} // TPDU data
    }
};

static const SMS_SMSC_Address_TPDU_t Concat_SMS_Part4 = {
    {
        // Address
        0x05,                      // The number of bytes in the AddressValue.
        0x91,                      // Type of address as defined in the standards.
        {0x44, 0x52, 0x78, 0x63, 0xF5}     // The address value in packed format.
    },
    {
        // TPDU
        0x1A,                      // Indicates the length of the TPDU data.
        {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x99, 0x03, 0x10, 0x89, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x04, 0xa0, 0x61, 0x39, 0x1d, 0x44, 0x03}       // TPDU data
        //     {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x89, 0x01, 0x31, 0x58, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x04, 0xa0, 0x61, 0x39, 0x1d, 0x44, 0x03} // TPDU data
    }
};

static const SMS_SMSC_Address_TPDU_t Concat_SMS_Part5 = {
    {
        // Address
        0x05,                      // The number of bytes in the AddressValue.
        0x91,                      // Type of address as defined in the standards.
        {0x44, 0x52, 0x78, 0x63, 0xF5}     // The address value in packed format.
    },
    {
        // TPDU
        0x1A,                      // Indicates the length of the TPDU data.
        {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x99, 0x03, 0x10, 0x89, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x05, 0xa0, 0x61, 0x39, 0x1d, 0x54, 0x03}       // TPDU data
        //     {0x41, 0x00, 0x0D, 0x91, 0x44, 0x52, 0x89, 0x01, 0x31, 0x58, 0xF6, 0x00, 0x00, 0x0d, 0x05, 0x00, 0x03, 0x01, 0x05, 0x05, 0xa0, 0x61, 0x39, 0x1d, 0x54, 0x03} // TPDU data
    }
};

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_Receive
*
* Send and receive a 5 part concat SMS
*
* To determine if this test has passed the log must be checked to
* verify that all 5 SMS have been sent on the same radio connection.
*
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_SendConcat(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    uint8_t loop;
    const SMS_SMSC_Address_TPDU_t *SMSC_TPDU_Array[] = {
        &Concat_SMS_Part1,
        &Concat_SMS_Part2,
        &Concat_SMS_Part3,
        &Concat_SMS_Part4,
        &Concat_SMS_Part5
    };

    for (loop = 0; loop < sizeof(SMSC_TPDU_Array) / sizeof(SMSC_TPDU_Array[0]); loop++) {

        SMS_A_(SMS_LOG_I("%s: commence MO SMS loop %d", TestCaseName_p, loop));

        if (SMSTest_SMS_Send_TPDU_WaitMode(TestCaseName_p, TestCaseFrameworkId, (SMS_SMSC_Address_TPDU_t *) SMSC_TPDU_Array[loop]) == FALSE) {
            Result = FALSE;
        }

        // Disable this MT SMS section if not sending to self or not using the modem stub
        SMS_A_(SMS_LOG_I("%s: commence MT SMS loop %d", TestCaseName_p, loop));

        // Set ACK to FALSE if Android is to handle the MT SMS acknowledge
        if (SMSTest_SMS_Receive_Ack(TestCaseName_p, TestCaseFrameworkId, TRUE) == FALSE) {
            Result = FALSE;
        }
    }

    return Result;
}

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_MO_MT_SoakTest
*
* Continiously send MO SMS and receive MT SMS. This test may be run on the
* S98 network or using the MAL stub.
*
* When using the MAL stub or S98 SIM with destination address as specified
* by S98_Destination_TPDU the test will send MO SMS and receive MT SMS (ie
* send to self).
* When testing on S98 with a SIM that does not match S98_Destination_TPDU
* the test will only send MO SMS and the SMSTest_SMS_Receive_Ack part
* of the test should be disabled.
*
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_MO_MT_SoakTest(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    uint32_t LoopCount;

    SMS_SMSC_Address_TPDU_t S98_Destination_TPDU = {
        {
            // NULL Address, Modem will use SMSC address on SIM
            0x00,                  // The number of useful semi-octets in the AddressValue.
            0x00,                  // Type of address as defined in the standards.
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}   // The address value in packed format.
        },
        {
            // TPDU
            84,
            {
                0x21, 0x00, 0x0c, 0x91, 0x44, 0x77, 0x89, 0x59, 0x10, 0x32,   // contains destination address +447798950123 use matching SIM for send to self
                0x00, 0x00, 0x50, 0x49, 0xa2, 0x0e, 0x14, 0xc3, 0xd9,
                //{0x21, 0x00, 0x0d, 0x91, 0x44, 0x52, 0x89, 0x01, 0x31, 0x58, // contains destination address +4425981013856 use matching SIM for send to self
                //{0x21, 0x00, 0x0d, 0x91, 0x44, 0x52, 0x99, 0x03, 0x10, 0x89, // contains destination address +4425993001986 use matching SIM for send to self
                // 0xf6, 0x00, 0x00, 0x50, 0x49, 0xa2, 0x0e, 0x14, 0xc3, 0xd9,
                0x58, 0xa0, 0x66, 0x79, 0x3e, 0x0f, 0x9f, 0xcb, 0x3a, 0x10,
                0x2c, 0x26, 0x9b, 0xd1, 0x6a, 0xb6, 0x1b, 0x2e, 0x17, 0x14,
                0x0e, 0x89, 0x45, 0xe3, 0x11, 0x99, 0x54, 0x2e, 0x99, 0x4d,
                0xe7, 0x13, 0x1a, 0x95, 0x4e, 0xa9, 0x55, 0xeb, 0x15, 0x9b,
                0xd5, 0x86, 0xc5, 0x63, 0x72, 0xd9, 0x7c, 0x46, 0xa7, 0xd5,
                0x6b, 0x76, 0xdb, 0xfd, 0x86, 0xc7, 0xe5, 0x73, 0x7a, 0xdd,
                0x7e, 0xc7, 0xe7, 0xf5
            }       // TPDU data
        }
    };

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    for (LoopCount = 0; LoopCount < 3000; LoopCount++) {
        SMS_A_(SMS_LOG_I("%s: MO & MT SMS, Loop %d", TestCaseName_p, LoopCount));

        if (SMSTest_SMS_Send_TPDU_WaitMode(TestCaseName_p, TestCaseFrameworkId, &S98_Destination_TPDU) == FALSE) {
            Result = FALSE;
        }

        // Disable when NOT sending to self on S98 or NOT using the MAL stub
        // Set Acknowledge flag to TRUE when using MAL stub
        if (SMSTest_SMS_Receive_Ack(TestCaseName_p, TestCaseFrameworkId, TRUE) == FALSE) {
            Result = FALSE;
        }

#ifdef SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED

        if (LoopCount % 100 == 0) {
            SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;

            // If we have enabled memory macro tracking in the SMS Server we should force a dump of the memory
            // allocation status now; i.e. after all the tests are run, but before communications with the SMS
            // Server are torn down.
            RequestStatus = Request_SMS_TestUtil_MemoryStatusGet(&RequestCtrl);

            if (RequestStatus != SMS_REQUEST_OK) {
                SMS_A_(SMS_LOG_I("%s: Request_SMS_TestUtil_MemoryStatusGet() returned %d", TestCaseName_p, RequestStatus));
                Result = FALSE;
            }
        }

#endif

    }

    return Result;
}

/*************************************************************************
**************************************************************************
*
* SMSTest_SMS_MO_MT_Loopback
*
* This test sends an MO SMS, waits for the aysnch reponse to the send,
* then waits for an MT SMS (simulating sending SMS to self), sends a
* Deliver Report and handles the aysnch send Deliver Report response.
*
**************************************************************************
**************************************************************************/
uint8_t SMSTest_SMS_MO_MT_Loopback(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    const SMS_ClientTag_t RequestSendClientTag = 20;
    const SMS_ClientTag_t RequestSendAckClientTag = 30;
    uint8_t Result = TRUE;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;
    uint8_t NumberOfLoops = 2;
    int Socket = SMS_NO_PROCESS;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            29,                    // Indicates the length of the TPDU data.
            {0x79, 0x01, 0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1, 0x02, 0x04, 0x1A, 0x08, 0x27, 0x1E, 0x04, 0x0D, 0x05, 0x00, 0x03, 0x01, 0x01, 0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03} // TPDU data
        }
    };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Ensure global parameter configured for no wait mode.
    RequestCtrl.WaitMode = SMS_NO_WAIT_MODE;
    RequestCtrl.ClientTag = RequestSendClientTag + NumberOfLoops;

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU, NULL,      // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: RESPONSE_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_PENDING && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: Wait for Signal from SMS Server", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: ERROR in response from RESPONSE_SMS_SHORTMESSAGESEND", TestCaseName_p));
    }

    while (NumberOfLoops) {
        /**************************************************************************************************/
        /* Use the low level Linux system commands to Listen out for an event                             */
        /**************************************************************************************************/

        fd_set rfds;
        int retval = 0;
        union SMS_SIGNAL *ReceivedSignal_p = NULL;
        uint32_t primitive = 0;

        FD_ZERO(&rfds);
        FD_SET(RequestResponseSocket, &rfds);
        FD_SET(EventSocket, &rfds);

        retval = select(EventSocket + 1, &rfds, NULL, NULL, NULL);

        SMS_A_(SMS_LOG_D("select returned = %d", retval));

        if (retval > 0) {       // got something to read
            if (FD_ISSET(RequestResponseSocket, &rfds)) {
                Socket = RequestResponseSocket;
                SMS_A_(SMS_LOG_D("select on RequestResponseSocket"));

            } else if (FD_ISSET(EventSocket, &rfds)) {
                Socket = EventSocket;
                SMS_A_(SMS_LOG_D("select on EventSocket "));
            } else {
                SMS_A_(SMS_LOG_E("ERROR: select on Unknown Socket "));
                break;          //break the loop since something is wrong
            }

            ReceivedSignal_p = Util_SMS_SignalReceiveOnSocket(Socket, &primitive, NULL);
        }                       // end got something to read

        if (ReceivedSignal_p != NULL) {
            switch (primitive) {
            case RESPONSE_SMS_SHORTMESSAGESEND: {
                // Get the Client Tag
                SMS_ClientTag_t ClientTag;

                Do_SMS_ClientTag_Get(ReceivedSignal_p, &ClientTag);

                // Unpack the returned signal into the output parameters
                RequestStatus = Response_SMS_ShortMessageSend(ReceivedSignal_p, &SM_Reference, &ErrorCode);

                SMS_A_(SMS_LOG_I("%s: RESPONSE_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x, ClientTag = %d", TestCaseName_p, RequestStatus, ErrorCode, ClientTag));

                if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && (RequestSendClientTag + NumberOfLoops) == ClientTag) {
                    SMS_A_(SMS_LOG_I("%s: RESPONSE_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
                } else {
                    Result = FALSE;
                }
            }
            break;

            case EVENT_SMS_SHORTMESSAGERECEIVED: {
                SMS_ShortMessageReceived_t *RxdSMS_p;

                SMS_A_(SMS_LOG_I("%s: select() returned EVENT_SMS_SHORTMESSAGERECEIVED", TestCaseName_p));

                RxdSMS_p = malloc(sizeof(SMS_ShortMessageReceived_t));

                if (RxdSMS_p != NULL) {
                    SMS_EventStatus_t EventStatus;

                    memset(RxdSMS_p, '\0', sizeof(SMS_ShortMessageReceived_t));

                    // Unpack event
                    EventStatus = Event_SMS_ShortMessageReceived(ReceivedSignal_p, RxdSMS_p);

                    if (EventStatus == SMS_EVENT_OK) {
                        SMS_A_(SMSTest_ReceivedSM_PrintToLog(RxdSMS_p));
                    } else {
                        Result = FALSE;
                        SMS_A_(SMS_LOG_E("%s: EVENT_SMS_SHORTMESSAGERECEIVED failed %d", TestCaseName_p, EventStatus));
                    }

                    free(RxdSMS_p);
                } else {
                    SMS_A_(SMS_LOG_E("%s: Malloc failed", TestCaseName_p));
                }

                // Ensure global parameter configured for no wait mode.
                RequestCtrl.WaitMode = SMS_NO_WAIT_MODE;
                RequestCtrl.ClientTag = RequestSendAckClientTag + NumberOfLoops;

                RequestStatus = Request_SMS_DeliverReportSend(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                                0x0000,       //RP_ErrorCause
                                &DeliverReportTPDU, &ErrorCode);

                SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_DELIVERREPORTSEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));
                SMS_A_(SMS_LOG_I("%s: Will now select for a RESPONSE_SMS_DELIVERREPORTSEND", TestCaseName_p));
            }
            break;

            case RESPONSE_SMS_DELIVERREPORTSEND: {
                // Get the Client Tag
                SMS_ClientTag_t ClientTag;

                Do_SMS_ClientTag_Get(ReceivedSignal_p, &ClientTag);

                // Unpack the returned signal into the output parameters
                RequestStatus = Response_SMS_DeliverReportSend(ReceivedSignal_p, &ErrorCode);

                SMS_A_(SMS_LOG_I("%s: RESPONSE_SMS_DELIVERREPORTSEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x, ClientTag = %d", TestCaseName_p, RequestStatus, ErrorCode, ClientTag));

                if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && (RequestSendAckClientTag + NumberOfLoops) == ClientTag) {
                } else {
                    Result = FALSE;
                }

                // break from the loop
                NumberOfLoops--;

                if (NumberOfLoops) {
                    // Ensure global parameter configured for no wait mode.
                    RequestCtrl.WaitMode = SMS_NO_WAIT_MODE;
                    RequestCtrl.ClientTag = RequestSendClientTag + NumberOfLoops;

                    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used.
                                    &SMSC_Address_TPDU, NULL,  // Slot
                                    MoreToSend, &SM_Reference, &ErrorCode);

                    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

                    if (RequestStatus == SMS_REQUEST_PENDING && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                        SMS_A_(SMS_LOG_I("%s: Wait for Signal from SMS Server", TestCaseName_p));
                    } else {
                        SMS_A_(SMS_LOG_E("%s: ERROR in response from REQUEST_SMS_SHORTMESSAGESEND", TestCaseName_p));
                    }
                }
            }
            break;

            default:
                SMS_A_(SMS_LOG_E("%s: select returned unexpected primitive 0x%08X", TestCaseName_p, (unsigned int) ReceivedSignal_p->Primitive));
                break;
            }

            Util_SMS_SignalFree(ReceivedSignal_p);      // free sig buffer
        } else {
            SMS_A_(SMS_LOG_E("%s: select returned NULL", TestCaseName_p));
        }
    }

    return Result;
}



#ifndef REMOVE_SMSTRG_LAYER

static uint8_t SMSTest_CompareTPDUs(
    const SMS_SMSC_Address_TPDU_t *const TPDU1_p,
    const SMS_SMSC_Address_TPDU_t *const TPDU2_p)
{
    uint8_t DataMatches = FALSE;

    if (TPDU1_p != NULL && TPDU2_p != NULL) {
        // Compare integer parameters.
        if (TPDU1_p->ServiceCenterAddress.Length == TPDU2_p->ServiceCenterAddress.Length &&
                TPDU1_p->ServiceCenterAddress.TypeOfAddress == TPDU2_p->ServiceCenterAddress.TypeOfAddress && TPDU1_p->TPDU.Length == TPDU2_p->TPDU.Length) {
            // Compare TPDU Data buffers.
            if (memcmp(TPDU1_p->TPDU.Data, TPDU2_p->TPDU.Data, TPDU1_p->TPDU.Length) == 0) {
                // Compare Service Centre Address Value buffers.
                if (memcmp(TPDU1_p->ServiceCenterAddress.AddressValue, TPDU2_p->ServiceCenterAddress.AddressValue, TPDU1_p->ServiceCenterAddress.Length - sizeof TPDU1_p->ServiceCenterAddress.TypeOfAddress) == 0) {   // The Length includes the TypeOfAddress parameter so we have to subtract it before doing the memcmp.
                    DataMatches = TRUE;
                }
            }
        }
    }

    return DataMatches;
}

/*This function is used to test the storeage of SMS in specified location*/
static uint8_t SMSTest_SMS_Storage_StorageSpecified(
    const char *TestCaseName_p,
    const SMS_Storage_t Storage)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t TmpResult = FALSE;
    SMS_Status_t Status = SMS_STATUS_READ;
    SMS_SlotInformation_t SlotInfo = { 0, 0, 0, 0, 0 };
    SMS_StorageStatus_t StorageStatus = { FALSE, FALSE };
    uint32_t TotalSlots = 0;
    uint32_t TotalSlots_SIM = 0;
    uint32_t LoopCount = 0;
    SMS_SMSC_Address_TPDU_t *TPDU_p = NULL;
    SMS_SMSC_Address_TPDU_t *ReadTPDU_p = NULL;
    static uint8_t DeleteSlots[6];

    if (Storage == SMS_STORAGE_SM) {
        DeleteSlots[0] = 2;
        DeleteSlots[1] = 3;
        DeleteSlots[2] = 4;
        DeleteSlots[3] = 7;
        DeleteSlots[4] = 8;
        DeleteSlots[5] = 10;
    } else {
        DeleteSlots[0] = 2;
        DeleteSlots[1] = 3;
        DeleteSlots[2] = 4;
        DeleteSlots[3] = 16;
        DeleteSlots[4] = 29;
        DeleteSlots[5] = 30;
    }

#if 0
    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            0x1C,                  // Indicates the length of the TPDU data.
            {
                0x44,
                0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1,
                0x00, 0x04,
                0x1A, 0x08, 0x27, 0x14, 0x04,
                0x0D, 0x05, 0x00, 0x03, 0x12, 0x01, 0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03
            }       // TPDU data
        }
    };
#endif
    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        0x07 ,  // The number of bytes in the AddressValue.
        0x91 , // Type of address of SMSC
        0x72 , 0x83 , 0x01 , 0x00 , 0x10 , 0xF5 , //address
        0x04 , //First octet of this SMS-DELIVER message.
        0x0B , //Address-Length. Length of the sender number (0B hex = 11 dec)
        0xC8 , //Type-of-address of the sender number
        0x72 , 0x38 , 0x88 , 0x09 , 0x00 , 0xF1 , //Sender number (decimal semi-octets), with a trailing F
        0x00 , // TP-PID. Protocol identifier.
        0x00 , // TP-DCS Data coding scheme
        0x99 , 0x30 , 0x92 , 0x51 , 0x61 , 0x95 , 0x80 , //TP-SCTS. Time stamp
        0x0A , // TP-UDL. User data length, length of message.
        0xE8 , 0x32 , 0x9B , 0xFD , 0x46 , 0x97 , 0xD9 , 0xEC , 0x37 //TP-UD. Message "hellohello" , 8-bit octet representing 7-bit data.
    };
    // Create TPDU buffer which is used throughout this test case.
    TPDU_p = malloc(sizeof(SMS_SMSC_Address_TPDU_t));

    if (TPDU_p != NULL) {
        memset(TPDU_p, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));

        TPDU_p->ServiceCenterAddress.Length = SMSC_Address_TPDU.ServiceCenterAddress.Length;
        TPDU_p->ServiceCenterAddress.TypeOfAddress = SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress;
        memcpy(TPDU_p->ServiceCenterAddress.AddressValue,
               SMSC_Address_TPDU.ServiceCenterAddress.AddressValue,
               SMS_MIN(SMSC_Address_TPDU.ServiceCenterAddress.Length, SMS_MAX_PACKED_ADDRESS_LENGTH));

        TPDU_p->TPDU.Length = SMSC_Address_TPDU.TPDU.Length;
        memcpy(TPDU_p->TPDU.Data, SMSC_Address_TPDU.TPDU.Data, SMS_MIN(SMSC_Address_TPDU.TPDU.Length, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH));

        TmpResult = TRUE;
    } else {
        SMS_A_(SMS_LOG_E("%s: Malloc failed.", TestCaseName_p));
        TmpResult = FALSE;
    }

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestCtrl.ClientTag = 0;

    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        Storage, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TotalSlots = SlotInfo.ReadCount + SlotInfo.UnreadCount + SlotInfo.SentCount + SlotInfo.UnsentCount + SlotInfo.FreeCount;
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    if (TmpResult && Storage == SMS_STORAGE_MT) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_SM, &SlotInfo, &StorageStatus, &ErrorCode);

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            TotalSlots_SIM = SlotInfo.ReadCount + SlotInfo.UnreadCount + SlotInfo.SentCount + SlotInfo.UnsentCount + SlotInfo.FreeCount;
            TmpResult = TRUE;
            SMS_A_(SMS_LOG_I("TotalSlot_SIM = %d", TotalSlots_SIM));
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    if (TmpResult) {
        TmpResult = FALSE;
        SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_SMS_StorageClear(&RequestCtrl_temp,  // Global RequestCtrl variable which contains socket to be used.
                        Storage, TRUE, FALSE, &ErrorCode);
        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = Request_SMS_StorageClear(&RequestCtrl_temp,  // Global RequestCtrl variable which contains socket to be used.
                        Storage, TRUE, FALSE, &ErrorCode);

        RequestStatus = Request_SMS_StorageClear(NULL,  // Global RequestCtrl variable which contains socket to be used.
                        Storage, TRUE, FALSE, &ErrorCode);

        // Delete any messages which may already be stored as a result of previous tests.
        RequestStatus = Request_SMS_StorageClear(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                        Storage, TRUE, FALSE, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageClear, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            TmpResult = TRUE;
        } else {
            SMS_A_(printf("Test Fails Here: %d", __LINE__));
        }
    }

    // Wait for the server status events that are dispatched by SMS Server. This ensures that we are never waiting more than necessary
    // and that the test harness is acting like a good client of SMS Server.
    {
        uint8_t StatusChangeOk;
        SMS_ServerStatus_t ReceivedStatus;
        uint32_t NumberEventsRxd;

        SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_BUSY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
        StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_BUSY, &ReceivedStatus, &NumberEventsRxd);

        if (StatusChangeOk) {
            SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_READY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
            StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_READY, &ReceivedStatus, &NumberEventsRxd);
        }

        if (!StatusChangeOk) {
            TmpResult = FALSE;
        }
    }

    if (TmpResult) {
        TmpResult = FALSE;

        SlotInfo.ReadCount = 0;
        SlotInfo.UnreadCount = 0;
        SlotInfo.SentCount = 0;
        SlotInfo.UnsentCount = 0;
        SlotInfo.FreeCount = 0;
        StorageStatus.StorageFullSIM = FALSE;
        StorageStatus.StorageFullME = FALSE;

        // Confirm that any messages which previously existed have been deleted.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        Storage, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TotalSlots = SlotInfo.ReadCount + SlotInfo.UnreadCount + SlotInfo.SentCount + SlotInfo.UnsentCount + SlotInfo.FreeCount;
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Validate the number of free slots is as expected and is not zero.
    if (TmpResult) {
        TmpResult = FALSE;

        if (SlotInfo.FreeCount == TotalSlots && TotalSlots > 0) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("%s: SlotInfo.FreeCount != TotalSlots, SlotInfo.FreeCount = %d TotalSlots = %d", TestCaseName_p, SlotInfo.FreeCount, TotalSlots));
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }


    while (TmpResult) {
        SMS_Slot_t Slot = { Storage, SMS_STORAGE_POSITION_INVALID };

        TmpResult = FALSE;

        // Increment LoopCounter
        LoopCount++;
        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, LoopCount = %d", TestCaseName_p, LoopCount));
        // Set a byte, in the TPDU, to a known value which can be used for checking the data later.
        // Byte 16 chosen at random.
        TPDU_p->TPDU.Data[16] = (uint8_t) LoopCount;

        RequestStatus = Request_SMS_ShortMessageWrite(&RequestCtrl,     // Global RequestCtrl variable which contains socket to be used.
                        Status, TPDU_p, &Slot, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, Slot.Storage = %d Slot.Position = %d", TestCaseName_p, Slot.Storage, Slot.Position));
            TmpResult = TRUE;
        } else if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_STORAGE_FULL) {
            if (LoopCount != 1) {
                SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, STORAGE FULL", TestCaseName_p));
                // This is a successful test outcome.
                TmpResult = TRUE;
            } else {
                // First iteration around the loop. Should not fail to write at least 1 SMS
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }

            // Force exit of while loop
            break;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }


    if (TmpResult) {
        TmpResult = FALSE;

        SlotInfo.ReadCount = 0;
        SlotInfo.UnreadCount = 0;
        SlotInfo.SentCount = 0;
        SlotInfo.UnsentCount = 0;
        SlotInfo.FreeCount = 0;
        StorageStatus.StorageFullSIM = FALSE;
        StorageStatus.StorageFullME = FALSE;

        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        Storage, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Validate the number of used slots is as expected and is not zero.
    if (TmpResult) {
        TmpResult = FALSE;

        if (SlotInfo.FreeCount == 0 && SlotInfo.ReadCount == TotalSlots) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("%s: After Writing: SlotInfo.FreeCount = %d SlotInfo.ReadCount = %d", TestCaseName_p, SlotInfo.FreeCount, SlotInfo.ReadCount));
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    LoopCount = 1;              // Use as counter to iterate through the TPDUs stored in ME storage.
    ReadTPDU_p = malloc(sizeof(SMS_SMSC_Address_TPDU_t));

    if (ReadTPDU_p == NULL) {
        TmpResult = FALSE;
    }

    // Read TPDUs and check that they are as expected.
    while (TmpResult) {
        SMS_Slot_t Slot;        // use ME or SM when reading data. MT is rejected.

        Slot.Position = LoopCount;
        Slot.Storage = SMS_STORAGE_SM;

        if (Storage == SMS_STORAGE_ME) {
            Slot.Storage = SMS_STORAGE_ME;
        } else if (Storage == SMS_STORAGE_MT && Slot.Position > TotalSlots_SIM) {
            Slot.Storage = SMS_STORAGE_ME;
            Slot.Position -= TotalSlots_SIM;
        }

        TmpResult = FALSE;

        Status = SMS_STATUS_ANY_STATUS;
        memset(ReadTPDU_p, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));

        SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };
        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_SMS_ShortMessageRead(&RequestCtrl_temp,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);
        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = Request_SMS_ShortMessageRead(&RequestCtrl_temp,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);
        RequestStatus = Request_SMS_ShortMessageRead(NULL,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);
        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, LoopCount = %d", TestCaseName_p, LoopCount));
        RequestStatus = Request_SMS_ShortMessageRead(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, Slot.Storage = %d Slot.Position = %d Status = %d", TestCaseName_p, Slot.Storage, Slot.Position, Status));
            // Setup expected data
            // Set a byte, in the TPDU, to a known value which can be used for checking the data later.
            // Byte 16 chosen at random.
            TPDU_p->TPDU.Data[16] = (uint8_t) LoopCount;

            if (SMSTest_CompareTPDUs(TPDU_p, ReadTPDU_p) && Status == SMS_STATUS_READ) {
                TmpResult = TRUE;
            } else {
                SMS_A_(SMS_LOG_I("Expected Data"));
                SMS_A_(Uint8Buf_PrintToLog((uint8_t *) TPDU_p, sizeof(SMS_SMSC_Address_TPDU_t)));

                SMS_A_(SMS_LOG_I("Received Data"));
                SMS_A_(Uint8Buf_PrintToLog((uint8_t *) ReadTPDU_p, sizeof(SMS_SMSC_Address_TPDU_t)));
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }
        } else if (LoopCount > TotalSlots && RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_INVALID_MESSAGE_ID) {
            // We have tried to read an invalid record number and got the expected error value.
            TmpResult = TRUE;

            // Force exit of while loop
            break;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }

        LoopCount++;
    }

    // Delete some SMs and then read back, and validate, TPDUs.
    if (TmpResult) {
        TmpResult = FALSE;

        for (LoopCount = 0; LoopCount < sizeof DeleteSlots / sizeof DeleteSlots[0]; LoopCount++) {
            SMS_Slot_t Slot;    // use ME or SM when reading data. MT is rejected.

            Slot.Position = DeleteSlots[LoopCount];

            if (Storage == SMS_STORAGE_SM) {
                Slot.Storage = SMS_STORAGE_SM;
            } else {
                Slot.Storage = SMS_STORAGE_ME;
            }

            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageDelete, LoopCount = %d SlotNumber = %d", TestCaseName_p, LoopCount, DeleteSlots[LoopCount]));
            RequestStatus = Request_SMS_ShortMessageDelete(&RequestCtrl,        // Global RequestCtrl variable which contains socket to be used.
                            &Slot, &ErrorCode);

            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageDelete, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

            if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                TmpResult = TRUE;
            } else {
                TmpResult = FALSE;
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
                break;
            }
        }
    }

    // Get storage status and check it has fewer messages in it.
    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        Storage, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            uint32_t NumberOfSlotsDeleted = sizeof DeleteSlots / sizeof DeleteSlots[0];
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));

            if (SlotInfo.FreeCount == NumberOfSlotsDeleted && SlotInfo.ReadCount == TotalSlots - NumberOfSlotsDeleted) {
                TmpResult = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }
        }
    }

    // Read back all records and check data
    LoopCount = 1;

    while (TmpResult) {
        SMS_Slot_t Slot;        // use ME or SM when reading data. MT is rejected.

        Slot.Position = LoopCount;
        Slot.Storage = SMS_STORAGE_SM;

        if (Storage == SMS_STORAGE_ME) {
            Slot.Storage = SMS_STORAGE_ME;
        } else if (Storage == SMS_STORAGE_MT && Slot.Position > TotalSlots_SIM) {
            Slot.Storage = SMS_STORAGE_ME;
            Slot.Position -= TotalSlots_SIM;
        }

        TmpResult = FALSE;

        Status = SMS_STATUS_ANY_STATUS;
        memset(ReadTPDU_p, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, LoopCount = %d", TestCaseName_p, LoopCount));
        RequestStatus = Request_SMS_ShortMessageRead(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            uint32_t n;

            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, Slot.Storage = %d Slot.Position = %d Status = %d", TestCaseName_p, Slot.Storage, Slot.Position, Status));
            // Setup expected data
            // Set a byte, in the TPDU, to a known value which can be used for checking the data later.
            // Byte 16 chosen at random.
            TPDU_p->TPDU.Data[16] = (uint8_t) LoopCount;

            if (SMSTest_CompareTPDUs(TPDU_p, ReadTPDU_p) && Status == SMS_STATUS_READ) {
                TmpResult = TRUE;
            } else {
                SMS_A_(SMS_LOG_I("Expected Data"));
                SMS_A_(Uint8Buf_PrintToLog((uint8_t *) TPDU_p, sizeof(SMS_SMSC_Address_TPDU_t)));
                SMS_A_(SMS_LOG_I("Received Data"));
                SMS_A_(Uint8Buf_PrintToLog((uint8_t *) ReadTPDU_p, sizeof(SMS_SMSC_Address_TPDU_t)));
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }

            // Check that this record is not one which we previously tried to delete!
            for (n = 0; n < sizeof DeleteSlots / sizeof DeleteSlots[0]; n++) {
                if (DeleteSlots[n] == Slot.Position) {
                    if (Storage != SMS_STORAGE_MT || Slot.Storage == SMS_STORAGE_ME) {
                        // Successfully read from a "deleted" slot!
                        SMS_A_(SMS_LOG_E("%s: Request_SMS_ShortMessageRead, Record should not exist!", TestCaseName_p));
                        TmpResult = FALSE;
                        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
                        break;
                    }
                }
            }
        } else if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_INVALID_MESSAGE_ID) {
            uint32_t n;

            TmpResult = FALSE;

            if (LoopCount > TotalSlots) {
                // We have tried to read an invalid record number and got the expected error value.
                TmpResult = TRUE;

                // Force exit of while loop
                break;
            }

            // Check if we are trying to read a record which is once that has been deleted.
            for (n = 0; n < sizeof DeleteSlots / sizeof DeleteSlots[0]; n++) {
                if (DeleteSlots[n] == Slot.Position) {
                    if (Storage != SMS_STORAGE_MT || Slot.Storage == SMS_STORAGE_ME) {
                        // We have failed to read a record which we have previously deleted.
                        // This is what we want.
                        TmpResult = TRUE;
                        break;
                    }
                }
            }
        } else {
            TmpResult = FALSE;
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }

        LoopCount++;
    }

    // Get storage status.
    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        Storage, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            uint32_t NumberOfSlotsDeleted = sizeof DeleteSlots / sizeof DeleteSlots[0];
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));

            if (SlotInfo.FreeCount == NumberOfSlotsDeleted && SlotInfo.ReadCount == TotalSlots - NumberOfSlotsDeleted) {
                TmpResult = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Try deleting an invalid slot number.
    if (TmpResult) {
        SMS_Slot_t Slot;        // use ME or SM when reading data. MT is rejected.

        Slot.Position = TotalSlots + 1;

        if (Storage == SMS_STORAGE_SM) {
            Slot.Storage = SMS_STORAGE_SM;
        } else {
            Slot.Storage = SMS_STORAGE_ME;
        }

        TmpResult = FALSE;

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageDelete Invalid Slot, SlotNumber = %d", TestCaseName_p, Slot.Position));
        RequestStatus = Request_SMS_ShortMessageDelete(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageDelete Invalid Slot, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_INVALID_MESSAGE_ID) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Get storage status.
    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        Storage, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            uint32_t NumberOfSlotsDeleted = sizeof DeleteSlots / sizeof DeleteSlots[0];
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));

            if (SlotInfo.FreeCount == NumberOfSlotsDeleted && SlotInfo.ReadCount == TotalSlots - NumberOfSlotsDeleted) {
                TmpResult = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    free(TPDU_p);
    free(ReadTPDU_p);

    if (TmpResult) {
        Result = TRUE;
    }

    // Delete all SMS stored as a result of this test
    (void) SMSTest_Util_DeleteAllShortMessages();

    return Result;
}

/* This test case is used to test the functionality of stroing an SMS in SIM*/
uint8_t SMSTest_SMS_Storage_SM(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    return SMSTest_SMS_Storage_StorageSpecified(TestCaseName_p, SMS_STORAGE_SM);
}

/* This test case is used to test the functionality of stroing an SMS in ME*/

uint8_t SMSTest_SMS_Storage_ME(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    return SMSTest_SMS_Storage_StorageSpecified(TestCaseName_p, SMS_STORAGE_ME);
}

/* This test case is used to test the functionality of stroing an SMS in MT*/

uint8_t SMSTest_SMS_Storage_MT(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    return SMSTest_SMS_Storage_StorageSpecified(TestCaseName_p, SMS_STORAGE_MT);

}

#define SMS_TEST_MAX_SLOT_POSITION_NOT_SET 0

uint8_t SMSTest_SMS_StatusReport_Receive(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId,
    SMS_SMSC_Address_TPDU_t *RxdSR_p,
    SMS_Slot_t *Slot_p)
{
    uint8_t DeliverReportResult = FALSE;
    uint8_t UnpackResult = FALSE;
    uint8_t SR_Wait = TRUE;
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: Wait for SMS Event Signal from SMS Server", TestCaseName_p));

    while (SR_Wait) {
        /**************************************************************************************************/
        /* Use the low level Linux system commands to Listen out for an event                             */
        /**************************************************************************************************/
        fd_set rfds;
        int retval = 0;
        union SMS_SIGNAL *ReceivedSignal_p = NULL;
        uint32_t primitive = 0;

        FD_ZERO(&rfds);
        FD_SET(EventSocket, &rfds);

        SR_Wait = FALSE;

        retval = select(EventSocket + 1, &rfds, NULL, NULL, NULL);

        SMS_A_(SMS_LOG_D("select returned = %d", retval));

        if (retval > 0) {       // got something to read
            ReceivedSignal_p = Util_SMS_SignalReceiveOnSocket(EventSocket, &primitive, NULL);
        }                       // end got something to read

        if (ReceivedSignal_p != NULL) {
            switch (primitive) {
                printf("%s: select() returned EVENT_SMS_STATUSREPORTRECEIVED%d", primitive);
            case EVENT_SMS_STATUSREPORTRECEIVED: {
                SMS_A_(printf("%s: select() returned EVENT_SMS_STATUSREPORTRECEIVED %d\n", TestCaseName_p, primitive));
                SMS_EventStatus_t EventStatus;

                memset(RxdSR_p, '\0', sizeof(SMS_StatusReport_t));

                // Unpack event
                EventStatus = Event_SMS_StatusReportReceived(ReceivedSignal_p, Slot_p, RxdSR_p);

                if (EventStatus == SMS_EVENT_OK) {
                    UnpackResult = TRUE;
                } else {
                    SMS_A_(SMS_LOG_E("%s: EVENT_SMS_STATUSREPORTRECEIVED failed %d", TestCaseName_p, EventStatus));
                }

                // Respond to event by sending Deliver Report
                {
                    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
                    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

                    // Ensure global parameter configured for wait mode.
                    RequestCtrl.WaitMode = SMS_WAIT_MODE;

                    RequestStatus = Request_SMS_DeliverReportSend(&RequestCtrl,     // Global RequestCtrl variable which contains socket to be used.
                                    0x0000,   // RP_ErrorCause
                                    &DeliverReportTPDU, &ErrorCode);

                    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_DELIVERREPORTSEND", TestCaseName_p));
                        DeliverReportResult = TRUE;
                    } else {
                        SMS_A_(SMS_LOG_E("%s: REQUEST_SMS_DELIVERREPORTSEND ERROR, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));
                    }
                }
                break;
            }
            case EVENT_SMS_SERVERSTATUSCHANGED:
            case EVENT_SMS_STORAGESTATUSCHANGED:
                SR_Wait = TRUE;
                break;
            case EVENT_SMS_SHORTMESSAGERECEIVED:
                SMS_A_(SMS_LOG_E("%s: select() returned unexpected EVENT_SMS_SHORTMESSAGERECEIVED", TestCaseName_p));
                break;
            case EVENT_SMS_ERROROCCURRED: {
                SMS_A_(SMS_LOG_I("%s: select() returned EVENT_SMS_ERROROCCURRED", TestCaseName_p));
                SMS_EventStatus_t EventStatus;
                SMS_Error_t Error;

                //memset(RxdSR_p, '\0', sizeof(SMS_StatusReport_t));

                // Unpack event
                EventStatus = Event_SMS_ErrorOccurred(ReceivedSignal_p, &Error);

                if (EventStatus == SMS_EVENT_OK) {
                    UnpackResult = TRUE;
                } else {
                    SMS_A_(SMS_LOG_E("%s: EVENT_SMS_ErrorOccurred failed %d", TestCaseName_p, EventStatus));
                }

                SMS_A_(SMS_LOG_E("%s: select() returned unexpected EVENT_SMS_ERROROCCURED", TestCaseName_p));
                break;
            }
            default:
                SMS_A_(SMS_LOG_E("%s: select() returned unexpected primitive 0x%08X", TestCaseName_p, (unsigned int) ReceivedSignal_p->Primitive));
                break;
            }

            Util_SMS_SignalFree(ReceivedSignal_p);      // free sig buffer
        } else {
            SMS_A_(SMS_LOG_E("%s: select() returned NULL", TestCaseName_p));
        }
    }

    return DeliverReportResult && UnpackResult;
}

/*This test case is used to test the INIT storage part of SMS*/
static uint8_t SMSTest_SMS_Init_Storage(
    const char *TestCaseName_p,
    const SMS_Storage_t Storage,
    uint8_t max_storage_position)
{
    uint8_t Result = FALSE;
    uint8_t TmpResult = FALSE;
    SMS_SlotInformation_t SlotInfo = { 0, 0, 0, 0, 0 };
    SMS_StorageStatus_t StorageStatus = { FALSE, FALSE };

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint32_t TotalSlots = 0;
    uint32_t LoopCount = 0;
    uint8_t number_of_slots = 0;

    const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x00,                  // The number of bytes in the AddressValue.
            0x00,                  // Type of address as defined in the standards.
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}   // The address value in packed format.
        },

        {
            26,                    // Indicates the length of the TPDU data.
            {0x21, 0x00, 0x0B, 0x81, 0x70, 0x28, 0x96, 0x30, 0x68, 0xF6, 0x00, 0x00, 0x0E, 0xD3, 0xE6, 0x14, 0x44, 0x2F, 0xCF, 0xE9, 0x69, 0xF7, 0x19, 0x34, 0x95, 0x02}
        }
    };

    // Delete any messages which may already be stored as a result of previous tests.
    RequestStatus = Request_SMS_StorageClear(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used.
                    Storage, TRUE, FALSE, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        TmpResult = TRUE;
    } else {
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
    }

    // Wait for the server status events that are dispatched by SMS Server. This ensures that we are never waiting more than necessary
    // and that the test harness is acting like a good client of SMS Server.
    {
        uint8_t StatusChangeOk;
        SMS_ServerStatus_t ReceivedStatus;
        uint32_t NumberEventsRxd;

        SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_BUSY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
        StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_BUSY, &ReceivedStatus, &NumberEventsRxd);

        if (StatusChangeOk) {
            SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_READY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
            StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_READY, &ReceivedStatus, &NumberEventsRxd);
        }

        if (!StatusChangeOk) {
            TmpResult = FALSE;
        }
    }

    if (TmpResult) {
        TmpResult = FALSE;

        SlotInfo.ReadCount = 0;
        SlotInfo.UnreadCount = 0;
        SlotInfo.SentCount = 0;
        SlotInfo.UnsentCount = 0;
        SlotInfo.FreeCount = 0;
        StorageStatus.StorageFullSIM = FALSE;
        StorageStatus.StorageFullME = FALSE;

        // Confirm that any messages which previously existed have been deleted.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        Storage, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", StorageStatus.StorageFullSIM, StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TotalSlots = SlotInfo.ReadCount + SlotInfo.UnreadCount + SlotInfo.SentCount + SlotInfo.UnsentCount + SlotInfo.FreeCount;
            TmpResult = TRUE;
            number_of_slots = (max_storage_position == SMS_TEST_MAX_SLOT_POSITION_NOT_SET) ? TotalSlots : max_storage_position;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Validate the number of free slots is as expected and is not zero.
    if (TmpResult) {
        TmpResult = FALSE;

        if (SlotInfo.FreeCount) {       // == TotalSlots && TotalSlots > 0) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("SlotInfo.FreeCount != TotalSlots, SlotInfo.FreeCount = %d TotalSlots = %d", SlotInfo.FreeCount, TotalSlots));
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    while (TmpResult) {
        SMS_Slot_t Slot = { Storage, 0 };
        SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };
        SMS_Status_t Status = SMS_STATUS_UNSENT;

        TmpResult = FALSE;

        // Increment LoopCounter
        LoopCount++;
        SMS_A_(SMS_LOG_I("Request_SMS_ShortMessageWrite, LoopCount = %d", LoopCount));
        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_SMS_ShortMessageWrite(&RequestCtrl_temp,     // Global RequestCtrl variable which contains socket to be used.
                        Status, &SMSC_Address_TPDU, &Slot, &ErrorCode);

        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = Request_SMS_ShortMessageWrite(&RequestCtrl_temp,     // Global RequestCtrl variable which contains socket to be used.
                        Status, &SMSC_Address_TPDU, &Slot, &ErrorCode);

        RequestStatus = Request_SMS_ShortMessageWrite(NULL,     // Global RequestCtrl variable which contains socket to be used.
                        Status, &SMSC_Address_TPDU, &Slot, &ErrorCode);

        RequestStatus = Request_SMS_ShortMessageWrite(&RequestCtrl,     // Global RequestCtrl variable which contains socket to be used.
                        Status, &SMSC_Address_TPDU, &Slot, &ErrorCode);

        SMS_A_(SMS_LOG_I("Request_SMS_ShortMessageWrite, RequestStatus = 0x%08x, ErrorCode = 0x%08x", RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("Request_SMS_ShortMessageWrite, Slot.Storage = %d Slot.Position = %d", Slot.Storage, Slot.Position));
            TmpResult = TRUE;

            if (LoopCount == number_of_slots) {
                break;
            }
        } else if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_STORAGE_FULL) {
            if (LoopCount != 1) {
                SMS_A_(SMS_LOG_I("Request_SMS_ShortMessageWrite, STORAGE FULL"));
                // This is a successful test outcome.
                TmpResult = TRUE;
            } else {
                // First iteration around the loop. Should not fail to write at least 1 SMS
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }

            // Force exit of while loop
            break;
        } else if (LoopCount == number_of_slots) {
            TmpResult = TRUE;
            break;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    if (TmpResult) {
        TmpResult = FALSE;

        SlotInfo.ReadCount = 0;
        SlotInfo.UnreadCount = 0;
        SlotInfo.SentCount = 0;
        SlotInfo.UnsentCount = 0;
        SlotInfo.FreeCount = 0;
        StorageStatus.StorageFullSIM = FALSE;
        StorageStatus.StorageFullME = FALSE;

        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        Storage, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", StorageStatus.StorageFullSIM, StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            Result = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    return Result;
}

static uint8_t SMSTest_SMS_StatusReport_StorageSpecified(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId,
    const SMS_Storage_t Storage,
    const uint8_t max_storage_position)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    SMS_SlotInformation_t SlotInfo;
    SMS_StorageStatus_t StorageStatus = { FALSE, FALSE };

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    SMS_A_(printf("%s: Initialise Storage", TestCaseName_p));

    if (SMSTest_SMS_Init_Storage(TestCaseName_p, Storage, max_storage_position) == FALSE) {
        SMS_A_(printf("Test Fails Here: %d", __LINE__));
        return Result;
    }

    RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl, Storage, &SlotInfo, &StorageStatus, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        uint8_t SendSms = TRUE;
        SMS_Slot_t Slot = { Storage, 0 };
        const uint8_t MAX_STORAGE_POSITION = (max_storage_position == SMS_TEST_MAX_SLOT_POSITION_NOT_SET) ? SlotInfo.UnsentCount : max_storage_position;

        if (SlotInfo.UnsentCount != MAX_STORAGE_POSITION) {
            SendSms = FALSE;
        }

        while (SendSms) {
            uint8_t MO_TPMR = 0;

            Slot.Position++;
            SendSms = FALSE;
            Result = FALSE;

            RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl, NULL, &Slot, FALSE,      // MoreToSend
                            &MO_TPMR, &ErrorCode);

            if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                SMS_SMSC_Address_TPDU_t *RxdSR_p = malloc(sizeof(SMS_SMSC_Address_TPDU_t));

                SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, MO_TPMR));

                if (RxdSR_p != NULL) {
                    SMS_Slot_t SR_Slot;
                    memset(RxdSR_p, '\0', sizeof(SMS_StatusReport_t));

                    if (SMSTest_SMS_StatusReport_Receive(TestCaseName_p, TestCaseFrameworkId, RxdSR_p, &SR_Slot)) {
#define TP_MR 1
                        SMS_StatusReport_t Stored_SR;

                        if (MO_TPMR == RxdSR_p->TPDU.Data[TP_MR]) {
                            if (SMS_STATUS_REPORTS_MATCH_STORE) {
                                RequestStatus = Request_SMS_StatusReportRead(&RequestCtrl, &SR_Slot, &Stored_SR, &ErrorCode);

                                if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                                    if (memcmp(RxdSR_p->TPDU.Data, Stored_SR.StatusReport, SMS_STATUS_REPORT_TPDU_MAX_LENGTH) == 0) {
                                        SendSms = TRUE; // success, try again
                                    } else {
                                        int i;
                                        char *data_dump_p = malloc(3 * SMS_STATUS_REPORT_TPDU_MAX_LENGTH + 1);
                                        size_t data_pos = 0;
                                        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));

                                        for (i = 0; i < SMS_STATUS_REPORT_TPDU_MAX_LENGTH; i++) {
                                            data_pos += sprintf(data_dump_p + data_pos, " %02x", (uint8_t)RxdSR_p->TPDU.Data[i]);
                                        }

                                        SMS_A_(printf("SMS SR Rcvd =%s", data_dump_p));

                                        data_pos = 0;

                                        for (i = 0; i < SMS_STATUS_REPORT_TPDU_MAX_LENGTH; i++) {
                                            data_pos += sprintf(data_dump_p + data_pos, " %02x", (uint8_t)Stored_SR.StatusReport[i]);
                                        }

                                        SMS_A_(printf("SMS SR Stored =%s", data_dump_p));
                                        free(data_dump_p);
                                    }
                                } else {
                                    SMS_A_(printf("Test Fails Here: %d", __LINE__));
                                }
                            } else {
                                SendSms = TRUE; // success, try again
                            }

                            SMS_A_(printf("%s: MO_TP-MR = 0x%02x SR_TP-MR = 0x%02x", TestCaseName_p, MO_TPMR, RxdSR_p->TPDU.Data[TP_MR]));
                        } else {
                            SMS_A_(printf("%s: ERROR Not Matching MO_TP-MR = x%02x SR(TP-MR) = 0x%02x", TestCaseName_p, MO_TPMR, RxdSR_p->TPDU.Data[TP_MR]));
                        }

#undef TP_MR
                    }

                    free(RxdSR_p);
                } else {
                    SMS_A_(SMS_LOG_E("%s: Malloc failed", TestCaseName_p));
                }
            } else if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_MESSAGE_NOT_FOUND) {
                SMS_A_(printf("SMS_ERROR_INTERN_MESSAGE_NOT_FOUND - Sent all SM's"));
                Result = TRUE;  // now exit - success
            } else {
                SMS_A_(SMS_LOG_E("REQUEST_SMS_SHORTMESSAGESEND ERROR RequestStatus = 0x%02x ErrorCode = 0x%02x", RequestStatus, ErrorCode));
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }
        }
    } else {
        SMS_A_(printf("Test Fails Here: %d", __LINE__));
    }

    // Tidy up the storage used
    RequestStatus = Request_SMS_StorageClear(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used.
                    Storage, TRUE, FALSE, &ErrorCode);

    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(printf("Failed to clear storage after testing"));
    } else {
        // Wait for the server status events that are dispatched by SMS Server. This ensures that we are never waiting more than necessary
        // and that the test harness is acting like a good client of SMS Server.
        uint8_t StatusChangeOk;
        SMS_ServerStatus_t ReceivedStatus;
        uint32_t NumberEventsRxd;


        SMS_A_(printf("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_BUSY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
        StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_BUSY, &ReceivedStatus, &NumberEventsRxd);

        if (StatusChangeOk) {
            SMS_A_(printf("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_READY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
            StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_READY, &ReceivedStatus, &NumberEventsRxd);
        }

        if (!StatusChangeOk) {
            SMS_A_(printf("Result value change here"));
            Result = FALSE;
        }
    }

    return Result;
}

uint8_t SMSTest_SMS_StatusReport_SM(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    Result = SMSTest_SMS_StatusReport_StorageSpecified(TestCaseName_p, TestCaseFrameworkId, SMS_STORAGE_SM, SMS_TEST_MAX_SLOT_POSITION_NOT_SET);

    if (!Result) {
        SMS_A_(SMS_LOG_I("%s: ****** This test can only pass if using a live network. If stubs are being used it will fail! ******\n", TestCaseName_p));
    }

    return Result;
}


uint8_t SMSTest_DeliverReportControl_Set(
    const char *TestCaseName_p,
    const SMS_NetworkAcknowledge_t NetworkAcknowledge)
{
    uint8_t Success = TRUE;
    SMS_NetworkAcknowledge_t LocalNetworkAcknowledge;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    SMS_Error_t Error = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    SMS_A_(SMS_LOG_I("%s: DeliverReport_Client_Set", TestCaseName_p));

    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = R_Req_SMS_DeliverReportControlSet(&RequestCtrl_temp, NetworkAcknowledge, &Error);

    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = R_Req_SMS_DeliverReportControlSet(&RequestCtrl_temp, NetworkAcknowledge, &Error);

    RequestStatus = R_Req_SMS_DeliverReportControlSet(NULL, NetworkAcknowledge, &Error);
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    RequestStatus = R_Req_SMS_DeliverReportControlSet(&RequestCtrl, NetworkAcknowledge, &Error);

    if (RequestStatus != SMS_REQUEST_OK || Error != SMS_ERROR_INTERN_NO_ERROR) {
        Success = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
    }

    R_Resp_SMS_DeliverReportControlSet(NULL, NULL);
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = R_Req_SMS_DeliverReportControlGet(&RequestCtrl_temp, &LocalNetworkAcknowledge, &Error);

    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = R_Req_SMS_DeliverReportControlGet(&RequestCtrl_temp, &LocalNetworkAcknowledge, &Error);
    RequestStatus = R_Req_SMS_DeliverReportControlGet(NULL, &LocalNetworkAcknowledge, &Error);

    RequestStatus = R_Req_SMS_DeliverReportControlGet(&RequestCtrl, &LocalNetworkAcknowledge, &Error);

    if (RequestStatus != SMS_REQUEST_OK || Error != SMS_ERROR_INTERN_NO_ERROR || NetworkAcknowledge != LocalNetworkAcknowledge) {
        Success = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
    }

    R_Resp_SMS_DeliverReportControlGet(NULL, NULL, NULL);

    return Success;
}

uint8_t SMSTest_SMS_DeliverReport_Control(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = FALSE;
    const uint8_t ACKNOWLEDGE = TRUE;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    if (SMSTest_DeliverReportControl_Set(TestCaseName_p, SMS_NETWORK_ACKNOWLEDGE_CLIENT)) {
        if (SMSTest_SMS_Send_WaitMode(TestCaseName_p, TestCaseFrameworkId)) {
            if (!SMSTest_SMS_Receive_Ack(TestCaseName_p, TestCaseFrameworkId, ACKNOWLEDGE)) {
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            } else {
                Result = TRUE;
            }
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    } else {
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
    }

    if (Result == TRUE) {
        Result = FALSE;

        if (SMSTest_DeliverReportControl_Set(TestCaseName_p, SMS_NETWORK_ACKNOWLEDGE_NORMAL)) {
            int i;
            const int NUMBER_OF_MSGS = 2;

            for (i = 0; i < NUMBER_OF_MSGS; i++) {
                if (SMSTest_SMS_Send_WaitMode(TestCaseName_p, TestCaseFrameworkId)) {
                    if (!SMSTest_SMS_Receive_Ack(TestCaseName_p, TestCaseFrameworkId, !ACKNOWLEDGE)) {
                        SMS_A_(SMS_LOG_E("Test Fails Here: %d Message : %d", __LINE__, i + 1));
                        break;
                    } else if (i == NUMBER_OF_MSGS - 1) {
                        Result = TRUE;
                    }
                } else {
                    SMS_A_(SMS_LOG_E("Test Fails Here: %d Message : %d", __LINE__, i + 1));
                }
            }
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    if (!SMSTest_DeliverReportControl_Set(TestCaseName_p, SMS_NETWORK_ACKNOWLEDGE_CLIENT)) {
        SMS_A_(SMS_LOG_E("Error resetting DeliverReportControl"));
    }

    return Result;
}

uint8_t SMSTest_SMS_MemCapacityFullReport(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);
    /*   RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
       RequestStatus = Request_SMS_MemCapacityFullReport(&RequestCtrl_temp, &ErrorCode);

        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = Request_SMS_MemCapacityFullReport(&RequestCtrl_temp, &ErrorCode);

        RequestStatus = Request_SMS_MemCapacityFullReport(NULL, &ErrorCode);*/
    //RequestCtrl_temp.WaitMode = SMS_WAIT_MODE;

    RequestStatus = Request_SMS_MemCapacityFullReport(&RequestCtrl, &ErrorCode);

    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
    }

    if (Result) {
        int i;
        const int NUMBER_OF_MSGS = 2;

        for (i = 0; i < NUMBER_OF_MSGS; i++) {
            if (!SMSTest_SMS_Send_WaitMode(TestCaseName_p, TestCaseFrameworkId)) {
                Result = FALSE;
                SMS_A_(SMS_LOG_E("Test Fails Here: %d Message : %d", __LINE__, i + 1));
            }
        }
    }

#define MEM_CAP_FULL_REPORT_TEST_DELAY_SEC   10
    SMS_A_(SMS_LOG_I("%s: Wait %ds for events / callbacks to complete", TestCaseName_p, MEM_CAP_FULL_REPORT_TEST_DELAY_SEC));
    sleep(MEM_CAP_FULL_REPORT_TEST_DELAY_SEC);
    /* RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
     RequestStatus = Request_SMS_MemCapacityAvailableSend(&RequestCtrl_temp, &ErrorCode);
     RequestCtrl_temp.WaitMode = 4;
     RequestStatus = Request_SMS_MemCapacityAvailableSend(&RequestCtrl_temp, &ErrorCode);
     RequestStatus = Request_SMS_MemCapacityAvailableSend(NULL, &ErrorCode);
     RequestCtrl_temp.WaitMode = SMS_WAIT_MODE;*/

    RequestStatus = Request_SMS_MemCapacityAvailableSend(&RequestCtrl, &ErrorCode);

    //Response_SMS_MemCapacityAvailableSend(NULL, NULL);
    // RequestStatus = Request_SMS_MemCapacityAvailableSend(&RequestCtrl, &ErrorCode);
    //Response_SMS_MemCapacityAvailableSend(NULL, NULL);
    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
    }

    return Result;
}

uint8_t SMSTest_SMS_MemCapacityFullGetState(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    int memFullState = 2; //Dummy value, cause 0 is default value and means not FULL, 1 == FULL.
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    //run in wait_mode
    RequestStatus = Request_SMS_MemCapacityGetState(&RequestCtrl, &memFullState, &ErrorCode);

    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR
            || memFullState > 0) { //Here we check to see so we got the initial Full state which is 0.
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        goto Error;
    }

    //Set the Cap flag to 1
    RequestStatus = Request_SMS_MemCapacityFullReport(&RequestCtrl, &ErrorCode);
    Response_SMS_MemCapacityFullReport(NULL, NULL);

    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        goto Error;
    }

    //Get the cap flag state
    RequestStatus = Request_SMS_MemCapacityGetState(&RequestCtrl, &memFullState, &ErrorCode);

    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR
            || !memFullState) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        goto Error;
    }

    //reset parameter for next test
    memFullState = -1;

    //run in no_wait_mode
    RequestCtrl.WaitMode = SMS_NO_WAIT_MODE;

    RequestStatus = Request_SMS_MemCapacityGetState(&RequestCtrl, &memFullState, &ErrorCode);

    if (RequestStatus != SMS_REQUEST_PENDING || ErrorCode != SMS_ERROR_INTERN_NO_ERROR
            || !memFullState) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        goto Error;
    }

    void *Rec_p = NULL;
    uint32_t primitive = 0;

    Rec_p = Util_SMS_SignalReceiveOnSocket(RequestCtrl.Socket, &primitive, NULL);

    // Unpack the returned signal into the output parameters, check if memFullState still are TRUE in SMS server.
    RequestStatus = Response_SMS_MemCapacityGetState(Rec_p, &memFullState, &ErrorCode);

    if (RequestStatus != SMS_REQUEST_OK || ErrorCode != SMS_ERROR_INTERN_NO_ERROR
            || !memFullState) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        goto Error;
    }

    //For negative tests
    RequestStatus = Request_SMS_MemCapacityGetState(NULL, &memFullState, &ErrorCode);

    if (RequestStatus != SMS_REQUEST_FAILED_PARAMETER) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        goto Error;
    }

    RequestCtrl.WaitMode = 2; // Dummy value for dummy negative test.

    RequestStatus = Request_SMS_MemCapacityGetState(&RequestCtrl, &memFullState, &ErrorCode);

    if (RequestStatus != SMS_REQUEST_FAILED_PARAMETER || ErrorCode != SMS_ERROR_INTERN_INVALID_WAIT_MODE) {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
    }

Error:
    // Must make sure that its Wait_mode, so all states are reset for the next test.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    return Result;
}

#undef SMS_TEST_MAX_SLOT_POSITION_NOT_SET


uint8_t SMSTest_SMS_PowerCycle_Storage_Write_ME(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t TmpResult = FALSE;
    SMS_Status_t Status = SMS_STATUS_READ;
    SMS_SlotInformation_t SlotInfo = { 0, 0, 0, 0, 0 };
    SMS_StorageStatus_t StorageStatus = { FALSE, FALSE };
    uint32_t TotalSlots = 0;
    uint32_t LoopCount = 0;
    SMS_SMSC_Address_TPDU_t *TPDU_p = NULL;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            0x1C,                  // Indicates the length of the TPDU data.
            {0x44, 0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1, 0x02, 0x04, 0x1A, 0x08, 0x27, 0x14, 0x04, 0x0D, 0x05, 0x00, 0x03, 0x12, 0x01, 0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03}       // TPDU data
        }
    };

    // Create TPDU buffer which is used throughout this test case.
    TPDU_p = malloc(sizeof(SMS_SMSC_Address_TPDU_t));

    if (TPDU_p != NULL) {
        memset(TPDU_p, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));

        TPDU_p->ServiceCenterAddress.Length = SMSC_Address_TPDU.ServiceCenterAddress.Length;
        TPDU_p->ServiceCenterAddress.TypeOfAddress = SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress;
        memcpy(TPDU_p->ServiceCenterAddress.AddressValue,
               SMSC_Address_TPDU.ServiceCenterAddress.AddressValue,
               SMS_MIN(SMSC_Address_TPDU.ServiceCenterAddress.Length - sizeof SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress, SMS_MAX_PACKED_ADDRESS_LENGTH));

        TPDU_p->TPDU.Length = SMSC_Address_TPDU.TPDU.Length;
        memcpy(TPDU_p->TPDU.Data, SMSC_Address_TPDU.TPDU.Data, SMS_MIN(SMSC_Address_TPDU.TPDU.Length, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH));

        TmpResult = TRUE;
    } else {
        SMS_A_(SMS_LOG_E("%s: Malloc failed.", TestCaseName_p));
        TmpResult = FALSE;
    }

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestCtrl.ClientTag = 0;

    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TotalSlots = SlotInfo.ReadCount + SlotInfo.UnreadCount + SlotInfo.SentCount + SlotInfo.UnsentCount + SlotInfo.FreeCount;
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    if (TmpResult) {
        TmpResult = FALSE;

        // Delete any messages which may already be stored as a result of previous tests.
        RequestStatus = Request_SMS_StorageClear(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, TRUE, TRUE, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageClear, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Wait for the server status events that are dispatched by SMS Server. This ensures that we are never waiting more than necessary
    // and that the test harness is acting like a good client of SMS Server.
    {
        uint8_t StatusChangeOk;
        SMS_ServerStatus_t ReceivedStatus;
        uint32_t NumberEventsRxd;

        SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_BUSY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
        StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_BUSY, &ReceivedStatus, &NumberEventsRxd);

        if (StatusChangeOk) {
            SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_READY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
            StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_READY, &ReceivedStatus, &NumberEventsRxd);
        }

        if (!StatusChangeOk) {
            TmpResult = FALSE;
        }
    }

    if (TmpResult) {
        TmpResult = FALSE;

        SlotInfo.ReadCount = 0;
        SlotInfo.UnreadCount = 0;
        SlotInfo.SentCount = 0;
        SlotInfo.UnsentCount = 0;
        SlotInfo.FreeCount = 0;
        StorageStatus.StorageFullSIM = FALSE;
        StorageStatus.StorageFullME = FALSE;

        // Confirm that any messages which previously existed have been deleted.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Validate the number of free slots is as expected and is not zero.
    if (TmpResult) {
        TmpResult = FALSE;

        if (SlotInfo.FreeCount == TotalSlots && TotalSlots > 0) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("%s: SlotInfo.FreeCount != TotalSlots, SlotInfo.FreeCount = %d TotalSlots = %d", TestCaseName_p, SlotInfo.FreeCount, TotalSlots));
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }


    while (TmpResult) {
        SMS_Slot_t Slot = { SMS_STORAGE_ME, SMS_STORAGE_POSITION_INVALID };

        TmpResult = FALSE;

        // Increment LoopCounter
        LoopCount++;
        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, LoopCount = %d", TestCaseName_p, LoopCount));
        // Set a byte, in the TPDU, to a known value which can be used for checking the data later.
        // Byte 16 chosen at random.
        TPDU_p->TPDU.Data[16] = (uint8_t) LoopCount;

        RequestStatus = Request_SMS_ShortMessageWrite(&RequestCtrl,     // Global RequestCtrl variable which contains socket to be used.
                        Status, TPDU_p, &Slot, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, Slot.Storage = %d Slot.Position = %d", TestCaseName_p, Slot.Storage, Slot.Position));
            TmpResult = TRUE;
        } else if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_STORAGE_FULL) {
            if (LoopCount != 1) {
                SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, STORAGE FULL", TestCaseName_p));
                // This is a successful test outcome.
                TmpResult = TRUE;
            } else {
                // First iteration around the loop. Should not fail to write at least 1 SMS
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }

            // Force exit of while loop
            break;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }


    if (TmpResult) {
        TmpResult = FALSE;

        SlotInfo.ReadCount = 0;
        SlotInfo.UnreadCount = 0;
        SlotInfo.SentCount = 0;
        SlotInfo.UnsentCount = 0;
        SlotInfo.FreeCount = 0;
        StorageStatus.StorageFullSIM = FALSE;
        StorageStatus.StorageFullME = FALSE;

        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Validate the number of used slots is as expected and is not zero.
    if (TmpResult) {
        TmpResult = FALSE;

        if (SlotInfo.FreeCount == 0 && SlotInfo.ReadCount == TotalSlots) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("%s: After Writing: SlotInfo.FreeCount = %d SlotInfo.ReadCount = %d", TestCaseName_p, SlotInfo.FreeCount, SlotInfo.ReadCount));
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    free(TPDU_p);

    if (TmpResult) {
        Result = TRUE;
    }

    // Delete all SMS stored as a result of this test
    (void) SMSTest_Util_DeleteAllShortMessages();

    return Result;
}




uint8_t SMSTest_SMS_PowerCycle_Storage_Write_ME1(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t TmpResult = FALSE;
    SMS_Status_t Status = SMS_STATUS_READ;
    SMS_SlotInformation_t SlotInfo = { 0, 0, 0, 0, 0 };
    SMS_StorageStatus_t StorageStatus = { FALSE, FALSE };
    uint32_t TotalSlots = 0;
    uint32_t LoopCount = 0;
    SMS_SMSC_Address_TPDU_t *TPDU_p = NULL;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        0x07 ,  // The number of bytes in the AddressValue.
        0x77 , // Type of address of SMSC
        0x72 , 0x83 , 0x01 , 0x00 , 0x10 , 0xF5 , //address
        0x11 , //First octet of this SMS-SUBMIT message.
        0x0B , //Address-Length. Length of the sender number (0B hex = 11 dec)
        0xC8 , //Type-of-address of the sender number
        0x72 , 0x38 , 0x88 , 0x09 , 0x00 , 0xF1 , //Sender number (decimal semi-octets), with a trailing F
        0xFF , // TP-PID. Protocol identifier.
        0xFF , // TP-DCS Data coding scheme
        0x99 , 0x30 , 0x92 , 0x51 , 0x61 , 0x95 , 0x80 , //TP-SCTS. Time stamp
        0x0D , // TP-UDL. User data length, length of message.
        0xE8 , 0x32 , 0x9B , 0xFD , 0x46 , 0x97 , 0xD9 , 0xEC , 0x37 //TP-UD. Message "hellohello" , 8-bit octets representing 7-bit data.
    };
    // Create TPDU buffer which is used throughout this test case.
    TPDU_p = malloc(sizeof(SMS_SMSC_Address_TPDU_t));

    if (TPDU_p != NULL) {
        memset(TPDU_p, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));

        TPDU_p->ServiceCenterAddress.Length = SMSC_Address_TPDU.ServiceCenterAddress.Length;
        TPDU_p->ServiceCenterAddress.TypeOfAddress = SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress;
        memcpy(TPDU_p->ServiceCenterAddress.AddressValue,
               SMSC_Address_TPDU.ServiceCenterAddress.AddressValue,
               SMS_MIN(SMSC_Address_TPDU.ServiceCenterAddress.Length - sizeof SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress, SMS_MAX_PACKED_ADDRESS_LENGTH));

        TPDU_p->TPDU.Length = SMSC_Address_TPDU.TPDU.Length;
        memcpy(TPDU_p->TPDU.Data, SMSC_Address_TPDU.TPDU.Data, SMS_MIN(SMSC_Address_TPDU.TPDU.Length, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH));

        TmpResult = TRUE;
    } else {
        SMS_A_(SMS_LOG_E("%s: Malloc failed.", TestCaseName_p));
        TmpResult = FALSE;
    }

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestCtrl.ClientTag = 0;

    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TotalSlots = SlotInfo.ReadCount + SlotInfo.UnreadCount + SlotInfo.SentCount + SlotInfo.UnsentCount + SlotInfo.FreeCount;
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    if (TmpResult) {
        TmpResult = FALSE;

        // Delete any messages which may already be stored as a result of previous tests.
        RequestStatus = Request_SMS_StorageClear(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, TRUE, TRUE, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageClear, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Wait for the server status events that are dispatched by SMS Server. This ensures that we are never waiting more than necessary
    // and that the test harness is acting like a good client of SMS Server.
    {
        uint8_t StatusChangeOk;
        SMS_ServerStatus_t ReceivedStatus;
        uint32_t NumberEventsRxd;

        SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_BUSY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
        StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_BUSY, &ReceivedStatus, &NumberEventsRxd);

        if (StatusChangeOk) {
            SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_READY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
            StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_READY, &ReceivedStatus, &NumberEventsRxd);
        }

        if (!StatusChangeOk) {
            TmpResult = FALSE;
        }
    }

    if (TmpResult) {
        TmpResult = FALSE;

        SlotInfo.ReadCount = 0;
        SlotInfo.UnreadCount = 0;
        SlotInfo.SentCount = 0;
        SlotInfo.UnsentCount = 0;
        SlotInfo.FreeCount = 0;
        StorageStatus.StorageFullSIM = FALSE;
        StorageStatus.StorageFullME = FALSE;

        // Confirm that any messages which previously existed have been deleted.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Validate the number of free slots is as expected and is not zero.
    if (TmpResult) {
        TmpResult = FALSE;

        if (SlotInfo.FreeCount == TotalSlots && TotalSlots > 0) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("%s: SlotInfo.FreeCount != TotalSlots, SlotInfo.FreeCount = %d TotalSlots = %d", TestCaseName_p, SlotInfo.FreeCount, TotalSlots));
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }


    while (TmpResult) {
        SMS_Slot_t Slot = { SMS_STORAGE_ME, SMS_STORAGE_POSITION_INVALID };

        TmpResult = FALSE;

        // Increment LoopCounter
        LoopCount++;
        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, LoopCount = %d", TestCaseName_p, LoopCount));
        // Set a byte, in the TPDU, to a known value which can be used for checking the data later.
        // Byte 16 chosen at random.
        TPDU_p->TPDU.Data[16] = (uint8_t) LoopCount;

        RequestStatus = Request_SMS_ShortMessageWrite(&RequestCtrl,     // Global RequestCtrl variable which contains socket to be used.
                        Status, TPDU_p, &Slot, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, Slot.Storage = %d Slot.Position = %d", TestCaseName_p, Slot.Storage, Slot.Position));
            TmpResult = TRUE;
        } else if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_STORAGE_FULL) {
            if (LoopCount != 1) {
                SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageWrite, STORAGE FULL", TestCaseName_p));
                // This is a successful test outcome.
                TmpResult = TRUE;
            } else {
                // First iteration around the loop. Should not fail to write at least 1 SMS
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }

            // Force exit of while loop
            break;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }


    if (TmpResult) {
        TmpResult = FALSE;

        SlotInfo.ReadCount = 0;
        SlotInfo.UnreadCount = 0;
        SlotInfo.SentCount = 0;
        SlotInfo.UnsentCount = 0;
        SlotInfo.FreeCount = 0;
        StorageStatus.StorageFullSIM = FALSE;
        StorageStatus.StorageFullME = FALSE;

        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Validate the number of used slots is as expected and is not zero.
    if (TmpResult) {
        TmpResult = FALSE;

        if (SlotInfo.FreeCount == 0 && SlotInfo.ReadCount == TotalSlots) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("%s: After Writing: SlotInfo.FreeCount = %d SlotInfo.ReadCount = %d", TestCaseName_p, SlotInfo.FreeCount, SlotInfo.ReadCount));
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    free(TPDU_p);

    if (TmpResult) {
        Result = TRUE;
    }

    // Delete all SMS stored as a result of this test
    (void) SMSTest_Util_DeleteAllShortMessages();

    return Result;
}


uint8_t SMSTest_SMS_PowerCycle_Storage_Read_ME(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    uint8_t TmpResult = FALSE;
    uint8_t TestAborted = FALSE;
    SMS_Status_t Status = SMS_STATUS_READ;
    SMS_SlotInformation_t SlotInfo = { 0, 0, 0, 0, 0 };
    SMS_StorageStatus_t StorageStatus = { FALSE, FALSE };
    uint32_t TotalSlots = 0;
    uint32_t LoopCount = 0;
    SMS_SMSC_Address_TPDU_t *TPDU_p = NULL;
    SMS_SMSC_Address_TPDU_t *ReadTPDU_p = NULL;
    static const uint8_t DeleteSlots[] = { 2, 3, 4, 16, 29, 30 };

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x06,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            0x1C,                  // Indicates the length of the TPDU data.
            {0x44, 0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1, 0x02, 0x04, 0x1A, 0x08, 0x27, 0x14, 0x04, 0x0D, 0x05, 0x00, 0x03, 0x12, 0x01, 0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03}       // TPDU data
        }
    };

    // Create TPDU buffer which is used throughout this test case.
    TPDU_p = malloc(sizeof(SMS_SMSC_Address_TPDU_t));

    if (TPDU_p != NULL) {
        memset(TPDU_p, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));

        TPDU_p->ServiceCenterAddress.Length = SMSC_Address_TPDU.ServiceCenterAddress.Length;
        TPDU_p->ServiceCenterAddress.TypeOfAddress = SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress;
        memcpy(TPDU_p->ServiceCenterAddress.AddressValue,
               SMSC_Address_TPDU.ServiceCenterAddress.AddressValue,
               SMS_MIN(SMSC_Address_TPDU.ServiceCenterAddress.Length - sizeof SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress, SMS_MAX_PACKED_ADDRESS_LENGTH));

        TPDU_p->TPDU.Length = SMSC_Address_TPDU.TPDU.Length;
        memcpy(TPDU_p->TPDU.Data, SMSC_Address_TPDU.TPDU.Data, SMS_MIN(SMSC_Address_TPDU.TPDU.Length, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH));

        TmpResult = TRUE;
    } else {
        SMS_A_(SMS_LOG_E("%s: Malloc failed.", TestCaseName_p));
        TmpResult = FALSE;
    }

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestCtrl.ClientTag = 0;

    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TotalSlots = SlotInfo.ReadCount + SlotInfo.UnreadCount + SlotInfo.SentCount + SlotInfo.UnsentCount + SlotInfo.FreeCount;
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    if (TmpResult) {
        // This function tests pre-existing SMS data which has been written and the end of a previous test harness run.
        // If there is no data then the test can aborted.
        if (SlotInfo.ReadCount == 0) {
            SMS_A_(SMS_LOG_I("%s: **** NO PRE-EXISTING DATA **** Aborting Test\n", TestCaseName_p));
            TestAborted = TRUE;
            TmpResult = FALSE;  // Stop further tests running as we have aborted.
        }
        // If there is data written we expect all slots to be written and "Read"
        else if (SlotInfo.UnreadCount != 0 || SlotInfo.SentCount != 0 || SlotInfo.UnsentCount != 0 || SlotInfo.FreeCount != 0) {
            SMS_A_(SMS_LOG_E("%s: Unexpected data found", TestCaseName_p));
            TmpResult = FALSE;
        }
    }

    LoopCount = 1;              // Use as counter to iterate through the TPDUs stored in ME storage.
    ReadTPDU_p = malloc(sizeof(SMS_SMSC_Address_TPDU_t));

    if (ReadTPDU_p == NULL) {
        TmpResult = FALSE;
    }

    // Read TPDUs and check that they are as expected.
    while (TmpResult) {
        SMS_Slot_t Slot = { SMS_STORAGE_ME, LoopCount };
        SMS_WaitMode_t dummy_waitmode = RequestCtrl.WaitMode;
        SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

        TmpResult = FALSE;

        Status = SMS_STATUS_ANY_STATUS;
        memset(ReadTPDU_p, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));
        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = Request_SMS_ShortMessageRead(&RequestCtrl_temp,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);
        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_SMS_ShortMessageRead(&RequestCtrl_temp,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);

        RequestStatus = Request_SMS_ShortMessageRead(NULL,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);
        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, LoopCount = %d", TestCaseName_p, LoopCount));
        RequestStatus = Request_SMS_ShortMessageRead(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, Slot.Storage = %d Slot.Position = %d Status = %d", TestCaseName_p, Slot.Storage, Slot.Position, Status));
            // Setup expected data
            // Set a byte, in the TPDU, to a known value which can be used for checking the data later.
            // Byte 16 chosen at random.
            TPDU_p->TPDU.Data[16] = (uint8_t) LoopCount;

            if (SMSTest_CompareTPDUs(TPDU_p, ReadTPDU_p) && Status == SMS_STATUS_READ) {
                TmpResult = TRUE;
            } else {
                SMS_A_(SMS_LOG_I("Expected Data"));
                SMS_A_(Uint8Buf_PrintToLog((uint8_t *) TPDU_p, sizeof(SMS_SMSC_Address_TPDU_t)));

                SMS_A_(SMS_LOG_I("Received Data"));
                SMS_A_(Uint8Buf_PrintToLog((uint8_t *) ReadTPDU_p, sizeof(SMS_SMSC_Address_TPDU_t)));
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }
        } else if (LoopCount > TotalSlots && RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_INVALID_MESSAGE_ID) {
            // We have tried to read an invalid record number and got the expected error value.
            TmpResult = TRUE;

            // Force exit of while loop
            break;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }

        LoopCount++;
    }

    // Delete some SMs and then read back, and validate, TPDUs.
    if (TmpResult) {
        TmpResult = FALSE;
        SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

        for (LoopCount = 0; LoopCount < sizeof DeleteSlots / sizeof DeleteSlots[0]; LoopCount++) {
            SMS_Slot_t Slot = { SMS_STORAGE_ME, DeleteSlots[LoopCount] };       // Always use ME when reading data. MT is rejected.

            RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
            RequestStatus = Request_SMS_ShortMessageDelete(&RequestCtrl_temp,        // Global RequestCtrl variable which contains socket to be used.
                            &Slot, &ErrorCode);

            RequestCtrl_temp.WaitMode = 4;
            RequestStatus = Request_SMS_ShortMessageDelete(&RequestCtrl_temp,        // Global RequestCtrl variable which contains socket to be used.
                            &Slot, &ErrorCode);

            RequestStatus = Request_SMS_ShortMessageDelete(NULL,        // Global RequestCtrl variable which contains socket to be used.
                            &Slot, &ErrorCode);
            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageDelete, LoopCount = %d SlotNumber = %d", TestCaseName_p, LoopCount, DeleteSlots[LoopCount]));
            RequestStatus = Request_SMS_ShortMessageDelete(&RequestCtrl,        // Global RequestCtrl variable which contains socket to be used.
                            &Slot, &ErrorCode);
            Response_SMS_ShortMessageDelete(NULL, NULL);

            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageDelete, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

            if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                TmpResult = TRUE;
            } else {
                TmpResult = FALSE;
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
                break;
            }
        }
    }

    // Get storage status and check it has fewer messages in it.
    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            uint32_t NumberOfSlotsDeleted = sizeof DeleteSlots / sizeof DeleteSlots[0];
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));

            if (SlotInfo.FreeCount == NumberOfSlotsDeleted && SlotInfo.ReadCount == TotalSlots - NumberOfSlotsDeleted) {
                TmpResult = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }
        }
    }

    // Read back all records and check data
    LoopCount = 1;

    while (TmpResult) {
        SMS_Slot_t Slot = { SMS_STORAGE_ME, LoopCount };        // Always use ME when reading data. MT is rejected.

        TmpResult = FALSE;

        Status = SMS_STATUS_ANY_STATUS;
        memset(ReadTPDU_p, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, LoopCount = %d", TestCaseName_p, LoopCount));
        RequestStatus = Request_SMS_ShortMessageRead(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used.
                        &Slot, &Status, ReadTPDU_p, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            uint32_t n;

            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageRead, Slot.Storage = %d Slot.Position = %d Status = %d", TestCaseName_p, Slot.Storage, Slot.Position, Status));
            // Setup expected data
            // Set a byte, in the TPDU, to a known value which can be used for checking the data later.
            // Byte 16 chosen at random.
            TPDU_p->TPDU.Data[16] = (uint8_t) LoopCount;

            if (SMSTest_CompareTPDUs(TPDU_p, ReadTPDU_p) && Status == SMS_STATUS_READ) {
                TmpResult = TRUE;
            } else {
                SMS_A_(SMS_LOG_I("Expected Data"));
                SMS_A_(Uint8Buf_PrintToLog((uint8_t *) TPDU_p, sizeof(SMS_SMSC_Address_TPDU_t)));

                SMS_A_(SMS_LOG_I("Received Data"));
                SMS_A_(Uint8Buf_PrintToLog((uint8_t *) ReadTPDU_p, sizeof(SMS_SMSC_Address_TPDU_t)));
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }

            // Check that this record is not one which we previously tried to delete!
            for (n = 0; n < sizeof DeleteSlots / sizeof DeleteSlots[0]; n++) {
                if (DeleteSlots[n] == LoopCount) {
                    // Successfully read from a "deleted" slot!
                    SMS_A_(SMS_LOG_E("%s: Request_SMS_ShortMessageRead, Record should not exist!", TestCaseName_p));
                    TmpResult = FALSE;
                    SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
                    break;
                }
            }
        } else if (RequestStatus == SMS_REQUEST_FAILED_APPLICATION && ErrorCode == SMS_ERROR_INTERN_INVALID_MESSAGE_ID) {
            uint32_t n;

            TmpResult = FALSE;

            if (LoopCount > TotalSlots) {
                // We have tried to read an invalid record number and got the expected error value.
                TmpResult = TRUE;

                // Force exit of while loop
                break;
            }

            // Check if we are trying to read a record which is once that has been deleted.
            for (n = 0; n < sizeof DeleteSlots / sizeof DeleteSlots[0]; n++) {
                if (DeleteSlots[n] == LoopCount) {
                    // We have failed to read a record which we have previously deleted.
                    // This is what we want.
                    TmpResult = TRUE;
                    break;
                }
            }
        } else {
            TmpResult = FALSE;
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }

        LoopCount++;
    }

    // Get storage status.
    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            uint32_t NumberOfSlotsDeleted = sizeof DeleteSlots / sizeof DeleteSlots[0];
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));

            if (SlotInfo.FreeCount == NumberOfSlotsDeleted && SlotInfo.ReadCount == TotalSlots - NumberOfSlotsDeleted) {
                TmpResult = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
            }
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Delete all SMSs from ME storage.
    if (TmpResult) {
        TmpResult = FALSE;

        // Delete any messages which may already be stored as a result of previous tests.
        RequestStatus = Request_SMS_StorageClear(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, TRUE, TRUE, &ErrorCode);
        Response_SMS_StorageClear(NULL, NULL);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageClear, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            uint8_t StatusChangeOk;
            SMS_ServerStatus_t ReceivedStatus;
            uint32_t NumberEventsRxd;

            TmpResult = TRUE;

            // Wait for the server status events that are dispatched by SMS Server. This ensures that we are never waiting more than necessary
            // and that the test harness is acting like a good client of SMS Server.
            SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_BUSY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
            StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_BUSY, &ReceivedStatus, &NumberEventsRxd);

            if (StatusChangeOk) {
                SMS_A_(SMS_LOG_I("%s: Wait upto %ds for SMS_SERVER_STATUS_MTS_READY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
                StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_READY, &ReceivedStatus, &NumberEventsRxd);
            }

            if (!StatusChangeOk) {
                TmpResult = FALSE;
            }
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Get storage status.
    if (TmpResult) {
        TmpResult = FALSE;

        // Get initial storage status.
        RequestStatus = Request_SMS_StorageCapacityGet(&RequestCtrl,    // Global RequestCtrl variable which contains socket to be used.
                        SMS_STORAGE_ME, &SlotInfo, &StorageStatus, &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_StorageCapacityGet, StorageStatus.StorageFullSIM = %d StorageStatus.StorageFullME = %d", TestCaseName_p, StorageStatus.StorageFullSIM,
                             StorageStatus.StorageFullME));
            SMS_A_(SMS_LOG_I("SlotInfo.ReadCount = %d SlotInfo.UnreadCount = %d SlotInfo.SentCount = %d SlotInfo.UnsentCount = %d SlotInfo.FreeCount = %d", SlotInfo.ReadCount,
                             SlotInfo.UnreadCount, SlotInfo.SentCount, SlotInfo.UnsentCount, SlotInfo.FreeCount));
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Validate the number of free slots is as expected and is not zero.
    if (TmpResult) {
        TmpResult = FALSE;

        if (SlotInfo.FreeCount == TotalSlots && TotalSlots > 0) {
            TmpResult = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("%s: SlotInfo.FreeCount != TotalSlots, SlotInfo.FreeCount = %d TotalSlots = %d", TestCaseName_p, SlotInfo.FreeCount, TotalSlots));
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }


    free(TPDU_p);
    free(ReadTPDU_p);

    if (TmpResult || TestAborted) {
        Result = TRUE;
    }

    return Result;
}


#endif                          // #ifndef REMOVE_SMSTRG_LAYER


/***************************************************************
 * readEvent() - read and respond to event from server.
 ***************************************************************/
static uint8_t readEvent(
    const char *TestCaseName_p,
    uint8_t eventType)
{
    uint8_t Result = TRUE;
    uint8_t DeliverReportResult = FALSE;
    uint8_t UnpackResult = FALSE;
    uint8_t HandlingSmsEvent = TRUE;
    uint32_t ReceivedPrimitive = 0;


    // Use the low level Linux system commands to Listen out for the event port subscribe event
    fd_set rfds;
    int retval = 0;
    struct timeval tv;
    void *incoming_signal_p = NULL;

    FD_ZERO(&rfds);
    FD_SET(EventSocket, &rfds);
    /* Wait up to five seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(EventSocket + 1, &rfds, NULL, NULL, &tv);

    SMS_A_(SMS_LOG_I("%s: readEvent: select returned = %d", TestCaseName_p, retval));

    if (retval > 0) {           // got something to read
        incoming_signal_p = Util_SMS_SignalReceiveOnSocket(EventSocket, &ReceivedPrimitive, NULL);      // ClientTag not wanted
    } else {
        SMS_A_(SMS_LOG_E("%s readEvent: Timeout: No data to read", TestCaseName_p));
        Result = FALSE;
    }

    if (incoming_signal_p != NULL) {
        switch (eventType) {
        case EVENT_SMS_APPLICATIONPORTMATCHED: {
            if (ReceivedPrimitive == EVENT_SMS_APPLICATIONPORTMATCHED) {
                Event_SMS_ApplicationPortMatched_t *RxdSMS_p;

                SMS_A_(SMS_LOG_I("%s readEvent: decodeEvent: EVENT_SMS_APPLICATIONPORTMATCHED", TestCaseName_p));

                RxdSMS_p = malloc(sizeof(Event_SMS_ApplicationPortMatched_t));

                if (RxdSMS_p != NULL) {
                    SMS_EventStatus_t EventStatus;

                    memset(RxdSMS_p, '\0', sizeof(Event_SMS_ApplicationPortMatched_t));

                    // Unpack event
                    EventStatus = Event_SMS_ApplicationPortMatched(incoming_signal_p, &(RxdSMS_p->ApplicationPortRange), &(RxdSMS_p->Data));

                    if (EventStatus == SMS_EVENT_OK) {
                        UnpackResult = TRUE;
                        SMSTest_ReceivedSM_AppPort_PrintToLog(RxdSMS_p);
                    } else {
                        SMS_A_(SMS_LOG_E("%s readEvent: EVENT_SMS_SHORTMESSAGERECEIVED failed %d", TestCaseName_p, EventStatus));
                        Result = FALSE;
                        break;
                    }

                    free(RxdSMS_p);
                } else {
                    SMS_A_(SMS_LOG_E("%s readEvent: Malloc failed", TestCaseName_p));
                    Result = FALSE;
                    break;
                }

            } else {
                SMS_A_(SMS_LOG_E("%s readEvent: select() returned unexpected primitive 0x%08X", TestCaseName_p, ReceivedPrimitive));
                Result = FALSE;
                break;
            }

            break;
        }

        case EVENT_SMS_SHORTMESSAGERECEIVED: {
            if (ReceivedPrimitive == EVENT_SMS_SHORTMESSAGERECEIVED) {
                SMS_ShortMessageReceived_t *RxdSMS_p;

                SMS_A_(SMS_LOG_I("%s readEvent: decodeEvent: EVENT_SMS_SHORTMESSAGERECEIVED", TestCaseName_p));

                RxdSMS_p = malloc(sizeof(SMS_ShortMessageReceived_t));

                if (RxdSMS_p != NULL) {
                    SMS_EventStatus_t EventStatus;

                    memset(RxdSMS_p, '\0', sizeof(SMS_ShortMessageReceived_t));

                    // Unpack event
                    EventStatus = Event_SMS_ShortMessageReceived(incoming_signal_p, RxdSMS_p);

                    if (EventStatus == SMS_EVENT_OK) {
                        UnpackResult = TRUE;
                        SMSTest_ReceivedSM_PrintToLog(RxdSMS_p);
                    } else {
                        SMS_A_(SMS_LOG_E("%s readEvent: EVENT_SMS_SHORTMESSAGERECEIVED failed %d", TestCaseName_p, EventStatus));
                        Result = FALSE;
                    }

                    free(RxdSMS_p);
                } else {
                    SMS_A_(SMS_LOG_E("%s readEvent: Malloc failed", TestCaseName_p));
                    Result = FALSE;
                }
            }

            break;
        }

        case EVENT_CBS_CELLBROADCASTMESSAGERECEIVED: {
            if (ReceivedPrimitive == EVENT_CBS_CELLBROADCASTMESSAGERECEIVED) {
                SMS_ClientTag_t ClientTag;
                SMS_EventStatus_t EventStatus;
                CB_Pdu_t Pdu;

                SMS_A_(SMS_LOG_E("%s readEvent: decodeEvent: EVENT_CBS_CELLBROADCASTMESSAGERECEIVED", TestCaseName_p));

                (void) memset(&Pdu, '\0', sizeof(CB_Pdu_t));

                // Unpack event
                EventStatus = Event_CBS_CellBroadcastMessageReceived(incoming_signal_p, &ClientTag, &Pdu);

                if (EventStatus == SMS_EVENT_OK) {
                    UnpackResult = TRUE;
                } else {
                    SMS_A_(SMS_LOG_E("%s readEvent: EVENT_CBS_CELLBROADCASTPAGERECEIVED failed %d", TestCaseName_p, EventStatus));
                    Result = FALSE;
                }
            }

            HandlingSmsEvent = FALSE;
            break;
        }

        default: {
            SMS_A_(SMS_LOG_E("%s readEvent: unhandled event type", TestCaseName_p));
            Result = FALSE;
            break;
        }
        }

        // Respond to event by sending Deliver Report, if appropriate
        if (HandlingSmsEvent) {
            SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
            SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

            /* Deliver Report Data:
               #____ RP Error Cause
               #    ____ Data Length (bytes)
               #   |  ____ Message Type Indicator
               #   | |  ____ Parameter Indicator 00000111 indicates User Data, DCS & PID present
               #   | | |  ____ PID
               #   | | | |  ____ Data Coding Scheme
               #   | | | | |
               #   | | | | |
               #   | | | | |
               #   | | | | |
               000004000300F1
             */
            static const SMS_TPDU_t LclDeliverReportTPDU = {
                0x04,           // Length
                {0x00, 0x03, 0x00, 0xF1}        // Data
            };

            // Ensure global parameter configured for wait mode.
            RequestCtrl.WaitMode = SMS_WAIT_MODE;

            SMS_A_(SMS_LOG_I("Calling Request_SMS_DeliverReportSend"));
            RequestStatus = Request_SMS_DeliverReportSend(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                            0x0000,       //RP_ErrorCause
                            &LclDeliverReportTPDU, &ErrorCode);

            SMS_A_(SMS_LOG_I("%s readEvent: REQUEST_SMS_DELIVERREPORTSEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

            if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                DeliverReportResult = TRUE;
            } else {
                Result = FALSE;
            }
        }

        // Free signal
        Util_SMS_SignalFree(incoming_signal_p);
    } else {
        SMS_A_(SMS_LOG_E("%s readEvent: decodeEvent: Null signal", TestCaseName_p));
    }

    return Result;
}


/*************************************************************************
SMSTest_SMS_Application_Port
**************************************************************************/
uint8_t SMSTest_SMS_Application_Port(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMS_ApplicationPortRange_t ApplicationPortRangeTest1, ApplicationPortRangeTest2, ApplicationPortRangeTest3, ApplicationPortRangeTest4, ApplicationPortRangeTest5, ApplicationPortRangeTest6;
    uint8_t numTests = 6;
    uint8_t i = 0;

    appPortTestData appPortTestDataArray[numTests];

    /*
       # __________________________________________________________________________________________________ Service Centre Address (ASCII)
       #|                                              ____________________________________________________ TPDU Length
       #|                                             |  __________________________________________________ TP MTI, UDHI
       #|                                             | |  ________________________________________________ Sender Address
       #|                                             | | |                ________________________________ PID
       #|                                             | | |               |  ______________________________ Data Coding Scheme
       #|                                             | | |               | |  ____________________________ Timestamp yy mm dd hh mm ss timezone
       #|                                             | | |               | | |              ______________ User Data Length (septets)
       #|                                             | | |               | | |             |  ____________ User Data Header
       #|                                             | | |               | | |             | |  __________ IEI Application Port 8bit
       #|                                             | | |               | | |             | | |  ________ IEI Length
       #|                                             | | |               | | |             | | | |  ______ Application Port 8bit Destination (100, 0x64)
       #|                                             | | |               | | |             | | | | |  ____ Application Port 8bit Origination (100, 0x64)
       #|                                             | | |               | | |             | | | | | |  __ 7 bit user data
       #|                                             | | |               | | |             | | | | | | |
       3739373331303039373300AAAAAAAAAAAAAAAAAAAA010135440C914477152447790015200103000000002204040264644170706C69636174696F6E20706F72742074657374206D657373616765000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
       3739373331303039373300AAAAAAAAAAAAAAAAAAAA01013051010C914477152447790015002204040264644170706C69636174696F6E20706F72742074657374206D657373616765000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
     */
    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_AppPort_Test1 = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x79, 0x73, 0x10, 0x09, 0x73} // The address value in packed format.
        },
        {
            // TPDU
            0x35,                  // Indicates the length of the TPDU data.
            {0x44, 0x0c, 0x91, 0x44, 0x77, 0x15, 0x24, 0x47, 0x79, 0x00, 0x15, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04, 0x02, 0x64, 0x64, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x74, 0x65, 0x73, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65}     // TPDU data
        }
    };

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_AppPort_Test2 = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x79, 0x73, 0x10, 0x09, 0x73} // The address value in packed format.
        },
        {
            // TPDU
            0x35,                  // Indicates the length of the TPDU data.
            {0x44, 0x0c, 0x91, 0x44, 0x77, 0x15, 0x24, 0x47, 0x79, 0x00, 0x15, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04, 0x02, 0xfa, 0xfa, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x74, 0x65, 0x73, 0x6d, 0x65, 0x73, 0x74, 0x20, 0x6D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65}       // TPDU data
        }
    };

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_AppPort_Test3 = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x79, 0x73, 0x10, 0x09, 0x73} // The address value in packed format.
        },
        {
            // TPDU
            0x37,                  // Indicates the length of the TPDU data.
            {0x44, 0x0c, 0x91, 0x44, 0x77, 0x15, 0x24, 0x47, 0x79, 0x00, 0x15, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x24, 0x06, 0x05, 0x04, 0x3e, 0x80, 0x3e, 0x80, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65}     // TPDU data
        }
    };

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_AppPort_Test4 = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x79, 0x73, 0x10, 0x09, 0x73} // The address value in packed format.
        },
        {
            // TPDU
            0x35,                  // Indicates the length of the TPDU data.
            {0x44, 0x0c, 0x91, 0x44, 0x77, 0x15, 0x24, 0x47, 0x79, 0x00, 0x15, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x24, 0x06, 0x05, 0x04, 0x42, 0x68, 0x42, 0x68, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65}     // TPDU data
        }
    };

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_AppPort_Test5 = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x79, 0x73, 0x10, 0x09, 0x73} // The address value in packed format.
        },
        {
            // TPDU
            0x35,                  // Indicates the length of the TPDU data.
            {0x44, 0x0c, 0x91, 0x44, 0x77, 0x15, 0x24, 0x47, 0x79, 0x00, 0x15, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04, 0x02, 0x64, 0x64, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x74, 0x65, 0x73, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65}     // TPDU data
        }
    };

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_AppPort_Test6 = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x79, 0x73, 0x10, 0x09, 0x73} // The address value in packed format.
        },
        {
            // TPDU
            0x35,                  // Indicates the length of the TPDU data.
            {0x44, 0x0c, 0x91, 0x44, 0x77, 0x15, 0x24, 0x47, 0x79, 0x00, 0x15, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04, 0x02, 0xfa, 0xfa, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x74, 0x65, 0x73, 0x6d, 0x65, 0x73, 0x74, 0x20, 0x6D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65}       // TPDU data
        }
    };

    /*
       static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_AppPort_101 =
       {
       { // Address
       0x05,  // The number of bytes in the AddressValue.
       0x91, // Type of address as defined in the standards.
       {0x79, 0x73, 0x10, 0x09, 0x73} // The address value in packed format.
       },
       { // TPDU
       0x35, // Indicates the length of the TPDU data.
       {0x44, 0x0c, 0x91, 0x44, 0x77, 0x15, 0x24, 0x47, 0x79, 0x00, 0x15, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x22, 0x04, 0x04, 0x02, 0x65, 0x65, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x74, 0x65, 0x73, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65} // TPDU data
       }
       };
     */
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("TEST BEGIN"));
    //
    // Setup test data
    //
    ApplicationPortRangeTest1.ApplicationPort = 1;
    ApplicationPortRangeTest1.LowerPort = 100;
    ApplicationPortRangeTest1.UpperPort = 100;

    ApplicationPortRangeTest2.ApplicationPort = 0;
    ApplicationPortRangeTest2.LowerPort = 250;
    ApplicationPortRangeTest2.UpperPort = 250;

    ApplicationPortRangeTest3.ApplicationPort = 1;
    ApplicationPortRangeTest3.LowerPort = 16000;
    ApplicationPortRangeTest3.UpperPort = 16000;

    ApplicationPortRangeTest4.ApplicationPort = 0;
    ApplicationPortRangeTest4.LowerPort = 17000;
    ApplicationPortRangeTest4.UpperPort = 17000;

    ApplicationPortRangeTest5.ApplicationPort = 1;
    ApplicationPortRangeTest5.LowerPort = 101;
    ApplicationPortRangeTest5.UpperPort = 101;

    ApplicationPortRangeTest6.ApplicationPort = 0;
    ApplicationPortRangeTest6.LowerPort = 16001;
    ApplicationPortRangeTest6.UpperPort = 16001;



    appPortTestDataArray[0].appPortTPDU = SMSC_Address_TPDU_AppPort_Test1;
    appPortTestDataArray[0].appPortRange = ApplicationPortRangeTest1;
    appPortTestDataArray[0].eventType = EVENT_SMS_APPLICATIONPORTMATCHED;

    appPortTestDataArray[1].appPortTPDU = SMSC_Address_TPDU_AppPort_Test2;
    appPortTestDataArray[1].appPortRange = ApplicationPortRangeTest2;
    appPortTestDataArray[1].eventType = EVENT_SMS_APPLICATIONPORTMATCHED;

    appPortTestDataArray[2].appPortTPDU = SMSC_Address_TPDU_AppPort_Test3;
    appPortTestDataArray[2].appPortRange = ApplicationPortRangeTest3;
    appPortTestDataArray[2].eventType = EVENT_SMS_APPLICATIONPORTMATCHED;

    appPortTestDataArray[3].appPortTPDU = SMSC_Address_TPDU_AppPort_Test4;
    appPortTestDataArray[3].appPortRange = ApplicationPortRangeTest4;
    appPortTestDataArray[3].eventType = EVENT_SMS_APPLICATIONPORTMATCHED;

    appPortTestDataArray[4].appPortTPDU = SMSC_Address_TPDU_AppPort_Test5;
    appPortTestDataArray[4].appPortRange = ApplicationPortRangeTest5;
    appPortTestDataArray[4].eventType = EVENT_SMS_SHORTMESSAGERECEIVED;

    appPortTestDataArray[5].appPortTPDU = SMSC_Address_TPDU_AppPort_Test6;
    appPortTestDataArray[5].appPortRange = ApplicationPortRangeTest6;
    appPortTestDataArray[5].eventType = EVENT_SMS_SHORTMESSAGERECEIVED;

    /*
     * For each app port test:
     * 1, subscribe to port events for port range
     * 2, send tpdu in wait mode
     * 3, read and respond to event from sms server
     * 4, send tpdu in no wait mode
     * 5, wait for asynch response
     * 6, read and respond to event from sms server
     * 7, unsubscribe to port events
     */
    for (i = 0; i < numTests; i++) {
        SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };
        SMS_A_(SMS_LOG_I("%s: Sub-Test Number: %d", TestCaseName_p, i));
        //
        // make port subscription
        //
        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_SMS_ApplicationPortSubscribe(&RequestCtrl_temp, &appPortTestDataArray[i].appPortRange, &ErrorCode);

        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = Request_SMS_ApplicationPortSubscribe(&RequestCtrl_temp, &appPortTestDataArray[i].appPortRange, &ErrorCode);

        RequestStatus = Request_SMS_ApplicationPortSubscribe(NULL, &appPortTestDataArray[i].appPortRange, &ErrorCode);
        RequestCtrl.WaitMode = SMS_WAIT_MODE;
        RequestStatus = Request_SMS_ApplicationPortSubscribe(&RequestCtrl, &appPortTestDataArray[i].appPortRange, &ErrorCode);
        Response_SMS_ApplicationPortSubscribe(NULL, NULL);

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_ApplicationPortSubscribe PASSED", TestCaseName_p));
        } else {
            SMS_A_(SMS_LOG_E("%s: Request_SMS_ApplicationPortSubscribe FAILED", TestCaseName_p));
            Result = FALSE;
        }

        //
        // send the PDU in wait mode
        //
        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_SMS_ShortMessageDeliver(&RequestCtrl_temp,   // Global RequestCtrl variable which contains socket to be used.
                        &appPortTestDataArray[i].appPortTPDU, &ErrorCode);
        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = Request_SMS_ShortMessageDeliver(&RequestCtrl_temp,   // Global RequestCtrl variable which contains socket to be used.
                        &appPortTestDataArray[i].appPortTPDU, &ErrorCode);

        RequestStatus = Request_SMS_ShortMessageDeliver(NULL,   // Global RequestCtrl variable which contains socket to be used.
                        &appPortTestDataArray[i].appPortTPDU, &ErrorCode);
        RequestCtrl.WaitMode = SMS_WAIT_MODE;
        RequestStatus = Request_SMS_ShortMessageDeliver(&RequestCtrl, &appPortTestDataArray[i].appPortTPDU, &ErrorCode);

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s: Request_SMS_ShortMessageDeliver PASSED", TestCaseName_p));
        } else {
            SMS_A_(SMS_LOG_E("%s: Request_SMS_ShortMessageDeliver FAILED", TestCaseName_p));
            Result = FALSE;
        }

        //
        // read event from sms server
        //
        if (readEvent(TestCaseName_p, appPortTestDataArray[i].eventType) == FALSE) {
            SMS_A_(SMS_LOG_I("%s Send SM in Port Range EVENT NOT RECEIVED", TestCaseName_p));
            Result = FALSE;
        } else {
            SMS_A_(SMS_LOG_I("%s Send SM in Port Range EVENT RECEIVED", TestCaseName_p));
        }


        //
        // send the PDU in no wait mode
        //
        RequestCtrl.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_SMS_ShortMessageDeliver(&RequestCtrl,   // Global RequestCtrl variable which contains socket to be used.
                        &appPortTestDataArray[i].appPortTPDU, &ErrorCode);

        //
        // wait for asynch response
        //
        if (RequestStatus == SMS_REQUEST_PENDING && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s Send SM in Port Range No Wait Mode PASSED", TestCaseName_p));

            while (TRUE) {
                // Now wait for asynchronous response.
                static const SMS_SIGSELECT SignalSelector[] = { 1, RESPONSE_SMS_SHORTMESSAGEDELIVER };
                void *Rec_p;

                // Wait for response from the SMS Server
                Rec_p = SMS_RECEIVE_ON_SOCKET(SignalSelector, RequestResponseSocket);

                if (Rec_p != NULL) {
                    if (((union SMS_SIGNAL *) Rec_p)->Primitive == RESPONSE_SMS_SHORTMESSAGEDELIVER) {
                        // Unpack the returned signal into the output parameters
                        RequestStatus = Response_SMS_ShortMessageDeliver(Rec_p, &ErrorCode);

                        //SMS_A_(SMS_LOG_I("LTP_SMS_TEST: RESPONSE_SMS_SHORTMESSAGEDELIVER, RequestStatus = 0x%08x, ErrorCode = 0x%08x", RequestStatus, ErrorCode);

                        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                            SMS_A_(SMS_LOG_I("%s Send SM in Port Range No Wait Mode RESPONSE RECEIVED", TestCaseName_p));
                            //Result = TRUE;
                        } else {
                            SMS_A_(SMS_LOG_E("%s Send SM in Port Range No Wait Mode ERROR.  RequestStatus:%d, ErrorCode:%d", TestCaseName_p, RequestStatus, ErrorCode));
                            Result = FALSE;
                        }

                        SMS_SIGNAL_FREE(&Rec_p);
                        // Exit while loop.
                        break;
                    } else {
                        SMS_A_(SMS_LOG_E("%s: SMS_RECEIVE_ON_SOCKET returned unexpected primitive 0x%08X", TestCaseName_p, (unsigned int)((union SMS_SIGNAL *) Rec_p)->Primitive));
                        SMS_SIGNAL_FREE(&Rec_p);
                    }
                } else {
                    SMS_A_(SMS_LOG_E("%s: SMS_RECEIVE_ON_SOCKET returned NULL", TestCaseName_p));
                }
            }
        } else {
            SMS_A_(SMS_LOG_E("%s Send SM in Port Range No Wait Mode FAILED.  RequestStatus: %d, ErrorCode: %d", TestCaseName_p, RequestStatus, ErrorCode));
            Result = FALSE;
        }

        //
        // read event from sms server
        //
        if (readEvent(TestCaseName_p, appPortTestDataArray[i].eventType) == FALSE) {
            SMS_A_(SMS_LOG_I("%s Send SM in Port Range No Wait Mode EVENT NOT RECEIVED", TestCaseName_p));
            Result = FALSE;
        } else {
            SMS_A_(SMS_LOG_I("%s Send SM in Port Range No Wait Mode EVENT RECEIVED", TestCaseName_p));
        }

        SMS_A_(SMS_LOG_I("%s: Unsubscribe to Port Events", TestCaseName_p));
        //
        // port unsubscribe
        //
        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_SMS_ApplicationPortUnsubscribe(&RequestCtrl_temp, &appPortTestDataArray[i].appPortRange, &ErrorCode);

        RequestCtrl_temp.WaitMode = 6;
        RequestStatus = Request_SMS_ApplicationPortUnsubscribe(&RequestCtrl_temp, &appPortTestDataArray[i].appPortRange, &ErrorCode);

        RequestStatus = Request_SMS_ApplicationPortUnsubscribe(NULL, &appPortTestDataArray[i].appPortRange, &ErrorCode);
        RequestStatus = Request_SMS_ApplicationPortUnsubscribe(&RequestCtrl, &appPortTestDataArray[i].appPortRange, &ErrorCode);
        Response_SMS_ApplicationPortUnsubscribe(NULL, NULL);

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_I("%s Request_SMS_ApplicationPortUnsubscribe PASSED", TestCaseName_p));
        } else {
            SMS_A_(SMS_LOG_E("%s Request_SMS_ApplicationPortUnsubscribe FAILED", TestCaseName_p));
            Result = FALSE;
        }

        SMS_A_(SMS_LOG_I("%s: End of test %d: Result %d", TestCaseName_p, i, Result));
    }

    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));

    return Result;
}

/*************************************************************************
SMSTest_SMS_MoreToSend
**************************************************************************/
uint8_t SMSTest_SMS_MoreToSend(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;
    SMS_RelayControl_t RelayLinkControl = SMS_RELAY_CONTROL_DISABLED;
    SMS_RelayControl_t RelayLinkControlset = SMS_RELAY_CONTROL_ENABLED;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_MoreToSend = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x79, 0x73, 0x10, 0x09, 0x73} // The address value in packed format.
        },
        {
            // TPDU
            0x30,                  // Indicates the length of the TPDU data.
            {0x51, 0x51, 0x0c, 0x91, 0x44, 0x77, 0x15, 0x24, 0x47, 0x79, 0x00, 0x15, 0x00, 0x22, 0x04, 0x04, 0x02, 0x64, 0x64, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65}       // TPDU data
        }
    };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    //
    // Check relay setting
    //
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = R_Req_SMS_RelayLinkControlGet(&RequestCtrl_temp, // Global RequestCtrl variable which contains socket to be used.
                    &RelayLinkControl, &ErrorCode);

    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = R_Req_SMS_RelayLinkControlGet(&RequestCtrl_temp, // Global RequestCtrl variable which contains socket to be used.
                    &RelayLinkControl, &ErrorCode);

    RequestStatus = R_Req_SMS_RelayLinkControlGet(NULL, // Global RequestCtrl variable which contains socket to be used.
                    &RelayLinkControl, &ErrorCode);
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = R_Req_SMS_RelayLinkControlGet(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                    &RelayLinkControl, &ErrorCode);
    R_Resp_SMS_RelayLinkControlGet(NULL, NULL, NULL);


    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        if (RelayLinkControl != SMS_RELAY_CONTROL_DISABLED) {
            SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlGet FAILED.  RelayLinkControl:%d", TestCaseName_p, RelayLinkControl));
            Result = FALSE;
        }
    } else {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlGet FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, ErrorCode));
        Result = FALSE;
    }


    //
    // Send sms, no link preservation
    //
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU_MoreToSend, NULL,   // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
        //Result = TRUE;
    } else {
        SMS_A_(SMS_LOG_E(" %s: Send SM in Port Range FAILED", TestCaseName_p));
        Result = FALSE;
    }

    if (readEvent(TestCaseName_p, EVENT_SMS_SHORTMESSAGERECEIVED) == FALSE) {
        SMS_A_(SMS_LOG_I("%s: Send SM in Port Range EVENT NOT RECEIVED", TestCaseName_p));
        Result = FALSE;
    } else {
        SMS_A_(SMS_LOG_I("%s: Send SM in Port Range EVENT RECEIVED", TestCaseName_p));
    }


    //
    // Check relay setting
    //
    RequestStatus = R_Req_SMS_RelayLinkControlGet(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                    &RelayLinkControl, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && RelayLinkControl == SMS_RELAY_CONTROL_DISABLED) {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlGet PASSED", TestCaseName_p));
        //Result = TRUE;
    } else {
        SMS_A_(SMS_LOG_E("%s: R_Req_SMS_RelayLinkControlGet FAILED", TestCaseName_p));
        Result = FALSE;
    }


    //
    // Send sms, with link preservation
    //
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    MoreToSend = TRUE;
    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU_MoreToSend, NULL,   // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x", TestCaseName_p, SM_Reference));
    } else {
        SMS_A_(SMS_LOG_E("%s: Send SM in Port Range FAILED", TestCaseName_p));
        Result = FALSE;
    }

    if (readEvent(TestCaseName_p, EVENT_SMS_SHORTMESSAGERECEIVED) == FALSE) {
        SMS_A_(SMS_LOG_I("%s: Send SM in Port Range EVENT NOT RECEIVED", TestCaseName_p));
        Result = FALSE;
    } else {
        SMS_A_(SMS_LOG_I("%s: Send SM in Port Range EVENT RECEIVED", TestCaseName_p));
    }

    //
    // Check relay setting
    //
    RequestStatus = R_Req_SMS_RelayLinkControlGet(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                    &RelayLinkControl, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && RelayLinkControl == SMS_RELAY_CONTROL_ENABLED) {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlGet PASSED", TestCaseName_p));
        //Result = TRUE;
    } else {
        SMS_A_(SMS_LOG_E("%s: R_Req_SMS_RelayLinkControlGet FAILED", TestCaseName_p));
        Result = FALSE;
    }


    //
    // let timer expire
    //
    // Do_TimerDelay_1_MS(7000);
    sleep(7);

    //
    // Check relay setting
    //
    RequestStatus = R_Req_SMS_RelayLinkControlGet(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                    &RelayLinkControl, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && RelayLinkControl == SMS_RELAY_CONTROL_DISABLED) {
        SMS_A_(SMS_LOG_I("%s: R_Req_SMS_RelayLinkControlGet PASSED", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: R_Req_SMS_RelayLinkControlGet FAILED", TestCaseName_p));
        Result = FALSE;
    }


    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));

    return Result;
}

/*This test case is used to test the functionality of setting the MO SMS route */
uint8_t SMSTest_SMS_MO_Route(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = FALSE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    int r;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };
    const int NUM_ROUTES = 4;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));

    RequestCtrl.WaitMode = SMS_WAIT_MODE;

    for (r = SMS_MO_ROUTE_PS; r < NUM_ROUTES; r++) {
        SMS_MO_Route_t RouteSet = r;
        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = R_Req_SMS_MO_RouteSet(&RequestCtrl_temp, RouteSet, &ErrorCode);
        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = R_Req_SMS_MO_RouteSet(&RequestCtrl_temp, RouteSet, &ErrorCode);
        RequestStatus = R_Req_SMS_MO_RouteSet(NULL, RouteSet, &ErrorCode);
        RequestStatus = R_Req_SMS_MO_RouteSet(&RequestCtrl, RouteSet, &ErrorCode);
        R_Resp_SMS_MO_RouteSet(NULL, NULL);

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            SMS_MO_Route_t RouteGet = 0;
            RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
            RequestStatus = R_Req_SMS_MO_RouteGet(&RequestCtrl_temp, &RouteGet, &ErrorCode);

            RequestCtrl_temp.WaitMode = 4;
            RequestStatus = R_Req_SMS_MO_RouteGet(&RequestCtrl_temp, &RouteGet, &ErrorCode);
            RequestStatus = R_Req_SMS_MO_RouteGet(NULL, &RouteGet, &ErrorCode);
            RequestStatus = R_Req_SMS_MO_RouteGet(&RequestCtrl, &RouteGet, &ErrorCode);
            R_Resp_SMS_MO_RouteGet(NULL, NULL, NULL);

            if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && RouteGet == (SMS_MO_Route_t) r) {
                if (r == NUM_ROUTES - 1) {
                    Result = TRUE;
                }
            } else {
                SMS_A_(SMS_LOG_E("Test Fails Here: %d SetRoute: %d RouteGet: %d", __LINE__, r, RouteGet));
                break;
            }
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d SetRoute: %d", __LINE__, r));
        }
    }

    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));
    return Result;
}

// Data object used with synchronous calls to SIM API.
static ste_sim_t *sim_sync = NULL;

// Call back function used by SIM API.
static void sim_callback_funcs(
    int cause,
    uintptr_t client_tag,
    void *data,
    void *user_data)
{
    // We have to have a callback function for the SIM API. However, for the purposes of this
    // test harness we don't care about any callbacks that we might receive!
    SMS_LOG_I("sim_callback_funcs: cause=%d Received", cause);
}

// Utility function to disconnect from the SIM module.
static void DisconnectFromSim(
    void)
{
    if (sim_sync != NULL) {
        uintptr_t client_tag = (uintptr_t) sim_sync;
        // According to sim.h, this will do a disconnect and then a delete of the ste_sim_t SIM object.
        ste_sim_delete(sim_sync, client_tag);
        sim_sync = NULL;
        SMS_LOG_I("DisconnectFromSim: Disconnected.");
    } else {
        SMS_LOG_E("DisconnectFromSim: Already disconnected.");
    }
}

// Utility function to connect to the SIM module.
static uint8_t ConnectToSim(
    void)
{
    SMS_LOG_I("ConnectToSim Called");

    if (sim_sync == NULL) {
        ste_sim_closure_t sim_closure = { NULL, NULL };
        uintptr_t client_tag;

        sim_closure.func = sim_callback_funcs;
        sim_sync = ste_sim_new_st(&sim_closure);
        sim_closure.user_data = sim_sync;

        SMS_LOG_I("ConnectToSim: ste_sim_new_st() %sSuccessful.", sim_sync == NULL ? "** NOT ** " : "");

        // Now connect to SIM
        if (sim_sync != NULL) {
            client_tag = (uintptr_t) sim_sync;

            if (ste_sim_connect(sim_sync, client_tag) == 0) {
                SMS_LOG_I("ConnectToSim: ste_sim_connect Successful.");
            } else {
                SMS_LOG_E("ConnectToSim: ste_sim_connect ** NOT ** Successful.");
                // Kill off our connection so that everything is removed and sim_sync set to NULL.
                DisconnectFromSim();
            }
        }
    } else {
        SMS_LOG_E("ConnectToSim: Already connected.");
    }

    return sim_sync != NULL ? TRUE : FALSE;
}


/*
 * EfSmssFile: EfSmssFile defines
 */
#define EFSMSS_FILE_READ_OFFSET               0
#define EFSMSS_FILE_MEM_CAPACITY_EXC_OFFSET   1
#define EFSMSS_FILE_MIN_LENGTH                2 // Memory capacity available is in 2nd octet. Only first 2 octets are mandatory.
#define EFSMSS_FILE_MEM_CAPACITY_EXC_MASK     0x01
#define EFSMSS_FILE_NAME                      0x6F43
#define EFSMSS_FILE_PATH                      NULL

// Utility function to read the value of the Memory Capacity Exceeded Notification Flag in the EFsmss file.
static uint8_t EFsmss_ReadMemCapacityExceededFlag(
    const char *TestCaseName_p,
    uint8_t *MemCapExceeded_p)
{
    uintptr_t client_tag;
    int read_status;
    sim_ef_t file_name = EFSMSS_FILE_NAME;
    uint8_t Success = FALSE;
    ste_uicc_sim_file_read_binary_response_t read_result = {
        STE_UICC_STATUS_CODE_FAIL,
        STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
        {0x00, 0x00},
        NULL,
        0
    };

    SMS_LOG_I("EFsmss_ReadMemCapacityExceededFlag: Called from %s", TestCaseName_p);

    if (sim_sync != NULL) {
        client_tag = (uintptr_t) sim_sync;

        read_status = ste_uicc_sim_file_read_binary_sync(sim_sync, client_tag, file_name, EFSMSS_FILE_READ_OFFSET, EFSMSS_FILE_MIN_LENGTH, EFSMSS_FILE_PATH, &read_result);

        if (read_status == 0 && read_result.uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            SMS_LOG_I("EFsmss_ReadMemCapacityExceededFlag: ste_uicc_sim_file_read_binary_sync: Len = %d, data %sNULL", read_result.length, read_result.data != NULL ? "NOT " : "");

            if (read_result.length >= EFSMSS_FILE_MIN_LENGTH && read_result.data != NULL) {
                Success = TRUE;
                SMS_LOG_I("EFsmss_ReadMemCapacityExceededFlag: ste_uicc_sim_file_read_binary_sync: MemCapExc Octet 0x%02X", read_result.data[EFSMSS_FILE_MEM_CAPACITY_EXC_OFFSET]);

                if (read_result.data[EFSMSS_FILE_MEM_CAPACITY_EXC_OFFSET] & EFSMSS_FILE_MEM_CAPACITY_EXC_MASK) {
                    // Bit 1, as defined by 3GPP, set; therefore Memory capacity available
                    *MemCapExceeded_p = FALSE;
                } else {
                    // Bit 1, as defined by 3GPP, unset; therefore Memory capacity not available
                    *MemCapExceeded_p = TRUE;
                }
            } else {
                SMS_LOG_E("EFsmss_ReadMemCapacityExceededFlag: ste_uicc_sim_file_read_binary_sync: ERROR: Len = %d, data %sNULL", read_result.length, read_result.data != NULL ? "NOT " : "");
            }
        } else {
            SMS_LOG_E("EFsmss_ReadMemCapacityExceededFlag: ste_uicc_sim_file_read_binary_sync: ERROR: read_status %d, uicc_status_code %d.", read_status, read_result.uicc_status_code);
        }
    } else {
        SMS_LOG_E("EFsmss_ReadMemCapacityExceededFlag: sim_sync is NULL !!!!");
    }

    SMS_LOG_I("EFsmss_ReadMemCapacityExceededFlag: Success %d, MemCapEx %d", Success, *MemCapExceeded_p);
    return Success;
}



/*************************************************************************
SMSTest_SMS_MemoryCapacityExceeded
**************************************************************************/
uint8_t SMSTest_SMS_MemoryCapacityExceeded(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    /*
     * 1 Read EFsmss file from SIM and get initial status of Mem Cap Exc Notification flag.
     * 2 Trigger MO SMS. (This is done by sending an SMS when using stubbed MAL.)
     * 3 On receipt of MO SMS, send delivery response with "memory capacity exceeded" RP Error cause.
     * 4 This should trigger some callbacks from MAL to to SMS-Server which can be checked in the logs.
     * 5 Wait short while for all callbacks to occur.
     * 6 Read the EFsmss file from the SIM and check it has been updated correctly.
     * 7 Call Request_SMS_MemCapacityAvailableSend() to indicate that memory is now available again.
     * 8 Again, the logs should indicate that SMS-Server gets callbacks from MAL.
     * 9 Read EFsmss file from SIM and check it has been updated correctly.
     */

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    const SMS_ClientTag_t RequestSendClientTag = 20;
    const SMS_ClientTag_t RequestSendAckClientTag = 30;
    uint8_t Result = FALSE;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;
    int Socket = SMS_NO_PROCESS;
    uint8_t MaxLoopCount = 10;  // variable used to try and make sure we don't trapped in infinite loop
    uint8_t SimFlagOK = TRUE;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU = {
        {
            // Address
            0x07,                  // The number of useful semi-octets in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x64, 0x07, 0x05, 0x80, 0x99, 0xf9}   // The address value in packed format.
        },
        {
            // TPDU
            29,                    // Indicates the length of the TPDU data.
            {0x79, 0x01, 0x06, 0x81, 0x21, 0x43, 0x65, 0x00, 0xF1, 0x02, 0x04, 0x1A, 0x08, 0x27, 0x1E, 0x04, 0x0D, 0x05, 0x00, 0x03, 0x01, 0x01, 0x01, 0xA0, 0x61, 0x39, 0x1D, 0x14, 0x03} // TPDU data
        }
    };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);
    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));

    // Connect to SIM module.
    if (ConnectToSim()) {
        // Get initial status of EFsmss file on SIM so we can determine initial value of its Mem Cap Exc Notification Flag.
        uint8_t MemCapExceededOnSim = TRUE;

        if (EFsmss_ReadMemCapacityExceededFlag(TestCaseName_p, &MemCapExceededOnSim)) {
            if (MemCapExceededOnSim) {
                SMS_A_(SMS_LOG_W("%s: WARNING: Unexpected initial EFsmss value on SIM. Memory Capacity Exceeded=%d", TestCaseName_p, MemCapExceededOnSim));
                SimFlagOK = FALSE;
            } else {
                SMS_A_(SMS_LOG_I("%s: Expected initial EFsmss value on SIM. Memory Capacity Exceeded=%d", TestCaseName_p, MemCapExceededOnSim));
            }
        }
    }

    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestCtrl.ClientTag = RequestSendClientTag;

    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU, NULL,      // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    SMS_A_(SMS_LOG_I("%s: RESPONSE_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("%s: SMS Sent OK", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: ERROR in response from RESPONSE_SMS_SHORTMESSAGESEND", TestCaseName_p));
    }

    while (MaxLoopCount--) {
        fd_set rfds;
        int retval = 0;
        union SMS_SIGNAL *ReceivedSignal_p = NULL;
        uint32_t primitive = 0;

        FD_ZERO(&rfds);
        FD_SET(RequestResponseSocket, &rfds);
        FD_SET(EventSocket, &rfds);

        retval = select(EventSocket + 1, &rfds, NULL, NULL, NULL);

        SMS_A_(SMS_LOG_I("%s: select returned = %d", TestCaseName_p, retval));

        if (retval > 0) {       // got something to read
            if (FD_ISSET(RequestResponseSocket, &rfds)) {
                Socket = RequestResponseSocket;
                SMS_A_(SMS_LOG_I("%s: select on RequestResponseSocket", TestCaseName_p));

            } else if (FD_ISSET(EventSocket, &rfds)) {
                Socket = EventSocket;
                SMS_A_(SMS_LOG_I("%s: select on EventSocket", TestCaseName_p));
            } else {
                SMS_A_(SMS_LOG_E("%s: ERROR: select on Unknown Socket", TestCaseName_p));
                break;          //break the loop since something is wrong
            }

            ReceivedSignal_p = Util_SMS_SignalReceiveOnSocket(Socket, &primitive, NULL);
        }                       // end got something to read

        if (ReceivedSignal_p != NULL) {
            if (primitive == EVENT_SMS_SHORTMESSAGERECEIVED) {
                SMS_ShortMessageReceived_t *RxdSMS_p;

                SMS_A_(SMS_LOG_I("%s: select() returned EVENT_SMS_SHORTMESSAGERECEIVED", TestCaseName_p));

                RxdSMS_p = malloc(sizeof(SMS_ShortMessageReceived_t));

                if (RxdSMS_p != NULL) {
                    SMS_EventStatus_t EventStatus;

                    memset(RxdSMS_p, '\0', sizeof(SMS_ShortMessageReceived_t));

                    // Unpack event
                    EventStatus = Event_SMS_ShortMessageReceived(ReceivedSignal_p, RxdSMS_p);

                    if (EventStatus == SMS_EVENT_OK) {
                        SMS_A_(SMSTest_ReceivedSM_PrintToLog(RxdSMS_p));
                    } else {
                        SMS_A_(SMS_LOG_I("%s: EVENT_SMS_SHORTMESSAGERECEIVED failed %d", TestCaseName_p, EventStatus));
                        free(RxdSMS_p);
                        Util_SMS_SignalFree(ReceivedSignal_p);  // free sig buffer
                        break;
                    }

                    free(RxdSMS_p);
                } else {
                    SMS_A_(SMS_LOG_E("%s: Malloc failed", TestCaseName_p));
                }

                SMS_A_(SMS_LOG_I("%s: Send Memory Capacity Exceeded Delivery Report", TestCaseName_p));
                RequestCtrl.WaitMode = SMS_WAIT_MODE;
                RequestCtrl.ClientTag = RequestSendAckClientTag;

                RequestStatus = Request_SMS_DeliverReportSend(&RequestCtrl,     // Global RequestCtrl variable which contains socket to be used.
                                SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED,    //RP_ErrorCause
                                &DeliverReportTPDU, &ErrorCode);

                SMS_A_(SMS_LOG_I("%s: REQUEST_SMS_DELIVERREPORTSEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

                if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                    Result = TRUE;
                    Util_SMS_SignalFree(ReceivedSignal_p);      // free sig buffer
                    break;
                }
            } else {
                SMS_A_(SMS_LOG_E("%s: Unexpected primitve received 0x%08X", TestCaseName_p, primitive));
            }

            Util_SMS_SignalFree(ReceivedSignal_p);      // free sig buffer
        } else {
            SMS_A_(SMS_LOG_E("%s: Util_SMS_SignalReceiveOnSocket() returned NULL", TestCaseName_p));
        }

        if (MaxLoopCount == 0) {
            SMS_A_(SMS_LOG_E("%s: MaxLoopCount == 0. Abort while loop", TestCaseName_p));
        }
    }

#define MEM_CAP_EXC_TEST_DELAY_SEC   5
    SMS_A_(SMS_LOG_I("%s: Wait %ds for events / callbacks to complete", TestCaseName_p, MEM_CAP_EXC_TEST_DELAY_SEC));
    sleep(MEM_CAP_EXC_TEST_DELAY_SEC);

    // Now try and read the EFsmss file from the SIM and double-check that the SMS Memory Capacity Exceeded Notification
    // Flag has been set.
    if (Result) {
        // Get EFsmss file on SIM so we can determine value of its Mem Cap Exc Notification Flag.
        uint8_t MemCapExceededOnSim = FALSE;

        if (EFsmss_ReadMemCapacityExceededFlag(TestCaseName_p, &MemCapExceededOnSim)) {
            if (!MemCapExceededOnSim) {
                SMS_A_(SMS_LOG_W("%s: WARNING: Unexpected EFsmss value on SIM. Memory Capacity Exceeded=%d", TestCaseName_p, MemCapExceededOnSim));
                SimFlagOK = FALSE;
            } else {
                SMS_A_(SMS_LOG_I("%s: Expected EFsmss value on SIM. Memory Capacity Exceeded=%d", TestCaseName_p, MemCapExceededOnSim));
            }
        }
    }

    if (Result) {
        // We have successfully sent a Memory Capacity Exceeded deliver report.
        // Now try and send Memory Capacity Available.
        Result = FALSE;

        SMS_A_(SMS_LOG_I("%s: Send Memory Capacity Exceeded Delivery Report", TestCaseName_p));
        RequestCtrl.WaitMode = SMS_WAIT_MODE;
        RequestCtrl.ClientTag = RequestSendAckClientTag;
        RequestStatus = Request_SMS_MemCapacityAvailableSend(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used.
                        &ErrorCode);

        SMS_A_(SMS_LOG_I("%s: Request_SMS_MemCapacityAvailableSend, RequestStatus = 0x%08x, ErrorCode = 0x%08x", TestCaseName_p, RequestStatus, ErrorCode));

        if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            Result = TRUE;
        }

        SMS_A_(SMS_LOG_I("%s: Wait another %ds for events / callbacks to complete", TestCaseName_p, MEM_CAP_EXC_TEST_DELAY_SEC));
        sleep(MEM_CAP_EXC_TEST_DELAY_SEC);
    }

    if (Result) {
        // Get EFsmss file on SIM so we can determine value of its Mem Cap Exc Notification Flag.
        uint8_t MemCapExceededOnSim = TRUE;

        if (EFsmss_ReadMemCapacityExceededFlag(TestCaseName_p, &MemCapExceededOnSim)) {
            if (MemCapExceededOnSim) {
                SMS_A_(SMS_LOG_W("%s: WARNING: Unexpected EFsmss value on SIM. Memory Capacity Exceeded=%d", TestCaseName_p, MemCapExceededOnSim));
                SimFlagOK = FALSE;
            } else {
                SMS_A_(SMS_LOG_I("%s: Expected EFsmss value on SIM. Memory Capacity Exceeded=%d", TestCaseName_p, MemCapExceededOnSim));
            }
        }
    }

    // Close SIM connection. Do this whatever the outcome of the test has been.
    DisconnectFromSim();

    if (!SimFlagOK) {
        // If we are using MAL Stubs the EFsmss file will not be updated. Therefore checking that it has been could lead to
        // errors which are inevitable. Therefore we print to the log but we do not fail the test case.
        SMS_A_(SMS_LOG_E("%s: SimFlagOK FAILED. If we are using MAL STUBS this is to be expected.", TestCaseName_p));
    }

    SMS_A_(SMS_LOG_I("%s: TEST END", TestCaseName_p));

    return Result;
}






/*
 * CBS Test Data
 */
#define MAX_CB_SUBSCRIPTIONS    10
#define N_CB_SUBSCRIPTIONS      10

static uint8_t CurrNoOfSubscriptions = 1;
static uint8_t NoOfSubscriptions = 0;

static const CBS_Subscription_t CBS_Subscriptions[] = {
    // First Message ID, Last Message ID, First DCS, Last DCS
    {0x0001, 0x0001, 0x01, 0x01},
    {0x0101, 0x0101, 0x01, 0x01},
    {0x0201, 0x0201, 0x01, 0x01},
    {0x0301, 0x0301, 0x01, 0x01},
    {0x0401, 0x0401, 0x01, 0x01},
    {0x0501, 0x0501, 0x01, 0x01},
    {0x0601, 0x0601, 0x01, 0x01},
    {0x0701, 0x0701, 0x01, 0x01},
    {0x0801, 0x0801, 0x01, 0x01},
    {0x0901, 0x0901, 0x01, 0x01}
};

/*************************************************************************
CBSTest_Activate
**************************************************************************/
uint8_t CBSTest_Activate(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    CBS_Error_t Error = CBS_ERROR_LAST_ERROR_TYPE;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));

    /*
     * The CBS tests rely on the Modem stub dispatching CB pages to the CB Server.
     * The Modem stub does this whenever it has its CB service activated; i.e. for the
     * MAL modem this is when mal_cbs_request_routing_set() is called.
     *
     * If we have a SIM which has valid EFcbmid values, then the CB Server will activate
     * the modem's CB service immediately and it will remain activated irrespective of
     * the clients calls to Request_CBS_Activate(). This means that the test harness
     * will not function correctly because the necessary CB pages will not be dispatched
     * during the running of the tests.
     *
     * Therefore, the CB tests must be run using:
     * 1)  the modem stub
     * 2)  a SIM which has either
     * 2a) no EFcbmid file
     * 2b) a EFcbmid file which only contains "unused" values; i.e. MIDs of 0xFFFF
     */
    SMS_A_(SMS_LOG_I("\n\n%s: ***************************IMPORTANT WARNING************************", TestCaseName_p));
    SMS_A_(SMS_LOG_I("%s: * CB tests will not work if EFcbmid file contains valid MID values *", TestCaseName_p));
    SMS_A_(SMS_LOG_I("%s: ********************************************************************\n\n\n", TestCaseName_p));
    RequestCtrl_temp.WaitMode = 6;
    RequestStatus = Request_CBS_Activate(&RequestCtrl_temp,  // Global RequestCtrl variable which contains socket to be used.
                                         &Error);
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = Request_CBS_Activate(&RequestCtrl_temp,  // Global RequestCtrl variable which contains socket to be used.
                                         &Error);
    RequestStatus = Request_CBS_Activate(NULL,  // Global RequestCtrl variable which contains socket to be used.
                                         &Error);

    // Activate the CBS service
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_CBS_Activate(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                                         &Error);

    Response_CBS_Activate(NULL, NULL);


    if (RequestStatus == SMS_REQUEST_OK && Error == CBS_ERROR_NONE) {
        SMS_A_(SMS_LOG_I("%s: Request_CBS_Activate PASSED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
    } else {
        SMS_A_(SMS_LOG_E("%s: Request_CBS_Activate FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        Result = FALSE;
    }

    return (Result);
}

/*************************************************************************
CBSTest_Deactivate
**************************************************************************/
uint8_t CBSTest_Deactivate(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    CBS_Error_t Error = CBS_ERROR_LAST_ERROR_TYPE;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = Request_CBS_Deactivate(&RequestCtrl_temp, &Error);

    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = Request_CBS_Deactivate(&RequestCtrl_temp, &Error);

    RequestStatus = Request_CBS_Deactivate(NULL, &Error);
    // Deactivate the CBS service
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_CBS_Deactivate(&RequestCtrl, &Error);

    Response_CBS_Deactivate(NULL, NULL);

    if (RequestStatus == SMS_REQUEST_OK && Error == CBS_ERROR_NONE) {
        SMS_A_(SMS_LOG_I("%s: Request_CBS_Deactivate PASSED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
    } else {
        SMS_A_(SMS_LOG_E("%s: Request_CBS_Deactivate FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        Result = FALSE;
    }

    return (Result);
}

/*************************************************************************
CBSTest_SubscribeToAll
**************************************************************************/
uint8_t CBSTest_SubscribeToAll(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    CBS_Error_t Error = CBS_ERROR_LAST_ERROR_TYPE;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    // Subscribe to receive all cell broadcast pages
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = Request_CBS_Subscribe(&RequestCtrl_temp, // Global RequestCtrl variable which contains socket to be used.
                                          FALSE,        // Subscribe to (n) number of cell broadcast pages
                                          N_CB_SUBSCRIPTIONS,   // Number of subscriptions
                                          CBS_Subscriptions,    // Subscriptions
                                          &Error);
    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = Request_CBS_Subscribe(&RequestCtrl_temp, // Global RequestCtrl variable which contains socket to be used.
                                          FALSE,        // Subscribe to (n) number of cell broadcast pages
                                          N_CB_SUBSCRIPTIONS,   // Number of subscriptions
                                          CBS_Subscriptions,    // Subscriptions
                                          &Error);

    RequestStatus = Request_CBS_Subscribe(NULL, // Global RequestCtrl variable which contains socket to be used.
                                          FALSE,        // Subscribe to (n) number of cell broadcast pages
                                          N_CB_SUBSCRIPTIONS,   // Number of subscriptions
                                          CBS_Subscriptions,    // Subscriptions
                                          &Error);
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_CBS_Subscribe(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                                          TRUE, // Subscribe to all
                                          0,    // Number of subscriptions
                                          NULL, // Subscriptions
                                          &Error);

    if (RequestStatus == SMS_REQUEST_OK && Error == CBS_ERROR_NONE) {
        uint8_t Result;

        SMS_A_(SMS_LOG_I("%s: Request_CBS_Subscribe Success.", TestCaseName_p));

        SMS_A_(SMS_LOG_I("%s: readEvent", TestCaseName_p));
        Result = readEvent(TestCaseName_p, EVENT_CBS_CELLBROADCASTMESSAGERECEIVED);

        if (Result) {
            SMS_A_(SMS_LOG_I("%s: Test Success.", TestCaseName_p));
        } else {
            SMS_A_(SMS_LOG_I("%s: No CB Page received before timeout.", TestCaseName_p));
        }

        SMS_A_(SMS_LOG_I("%s: Cached CB Messages received", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: Request_CBS_Subscribe FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        Result = FALSE;
    }

    return (Result);
}

/*************************************************************************
CBSTest_UnSubscribeToAll
**************************************************************************/
uint8_t CBSTest_UnSubscribeToAll(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    CBS_Error_t Error = CBS_ERROR_LAST_ERROR_TYPE;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    // Unsubscribe to receive all cell broadcast pages
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = Request_CBS_Unsubscribe(&RequestCtrl_temp,       // Global RequestCtrl variable which contains socket to be used.
                                            FALSE,      // Unsubscribe to all
                                            N_CB_SUBSCRIPTIONS, // Number of subscriptions
                                            CBS_Subscriptions,  // Subscriptions
                                            &Error);
    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = Request_CBS_Unsubscribe(&RequestCtrl_temp,       // Global RequestCtrl variable which contains socket to be used.
                                            FALSE,      // Unsubscribe to all
                                            N_CB_SUBSCRIPTIONS, // Number of subscriptions
                                            CBS_Subscriptions,  // Subscriptions
                                            &Error);

    RequestStatus = Request_CBS_Unsubscribe(NULL,       // Global RequestCtrl variable which contains socket to be used.
                                            FALSE,      // Unsubscribe to all
                                            N_CB_SUBSCRIPTIONS, // Number of subscriptions
                                            CBS_Subscriptions,  // Subscriptions
                                            &Error);
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_CBS_Unsubscribe(&RequestCtrl,       // Global RequestCtrl variable which contains socket to be used.
                                            TRUE,       // Unsubscribe to all
                                            0,  // Number of subscriptions
                                            NULL,       // Subscriptions
                                            &Error);

    if (RequestStatus == SMS_REQUEST_OK && Error == CBS_ERROR_NONE) {
        SMS_A_(SMS_LOG_I("%s: Request_CBS_Unsubscribe PASSED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
    } else {
        SMS_A_(SMS_LOG_E("%s: Request_CBS_Unsubscribe FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        Result = FALSE;
    }

    return (Result);
}


/*************************************************************************
CBSTest_SubscribeToN
**************************************************************************/
uint8_t CBSTest_SubscribeToN(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    CBS_Error_t Error = CBS_ERROR_LAST_ERROR_TYPE;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    // Subscribe to receive (n) number of cell broadcast pages
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_CBS_Subscribe(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                                          FALSE,        // Subscribe to (n) number of cell broadcast pages
                                          N_CB_SUBSCRIPTIONS,   // Number of subscriptions
                                          CBS_Subscriptions,    // Subscriptions
                                          &Error);
    Response_CBS_Subscribe(NULL, NULL);

    if (RequestStatus == SMS_REQUEST_OK && Error == CBS_ERROR_NONE) {
        SMS_A_(SMS_LOG_I("%s: Request_CBS_Subscribe Success.", TestCaseName_p));
        CurrNoOfSubscriptions = N_CB_SUBSCRIPTIONS;
        SMS_A_(SMS_LOG_I("%s: readEvent", TestCaseName_p));
        Result = readEvent(TestCaseName_p, EVENT_CBS_CELLBROADCASTMESSAGERECEIVED);

        if (Result) {
            SMS_A_(SMS_LOG_I("%s: Test Success.", TestCaseName_p));
        } else {
            SMS_A_(SMS_LOG_I("%s: No CB Page received before timeout.", TestCaseName_p));
        }

        SMS_A_(SMS_LOG_I("%s: CB Pages Received", TestCaseName_p));
    } else {
        SMS_A_(SMS_LOG_E("%s: Request_CBS_Subscribe FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        Result = FALSE;
    }

    //SIM_type CBMID subscription
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_CBS_Subscribe(&RequestCtrl, // Global RequestCtrl variable which contains socket to be used.
                                          FALSE,    // Subscribe to (n) number of cell broadcast pages
                                          1,    // Number of subscriptions
                                          CBS_Subscriptions,    // Subscriptions
                                          &Error);


    return (Result);
}

/*************************************************************************
CBSTest_UnSubscribeToN
**************************************************************************/
uint8_t CBSTest_UnSubscribeToN(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    CBS_Error_t Error = CBS_ERROR_LAST_ERROR_TYPE;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    // Unsubscribe to receive (n) cell broadcast pages
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_CBS_Unsubscribe(&RequestCtrl,       // Global RequestCtrl variable which contains socket to be used.
                                            FALSE,      // Unsubscribe to all
                                            N_CB_SUBSCRIPTIONS, // Number of subscriptions
                                            CBS_Subscriptions,  // Subscriptions
                                            &Error);
    Response_CBS_Unsubscribe(NULL, NULL);


    if (RequestStatus == SMS_REQUEST_OK && Error == CBS_ERROR_NONE) {
        SMS_A_(SMS_LOG_I("%s: Request_CBS_Unsubscribe PASSED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
    } else {
        SMS_A_(SMS_LOG_E("%s: Request_CBS_Unsubscribe FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        Result = FALSE;
    }

    return (Result);
}

/*************************************************************************
CBSTest_GetNoOfSubscriptions
**************************************************************************/
uint8_t CBSTest_GetNoOfSubscriptions(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    CBS_Error_t Error = CBS_ERROR_LAST_ERROR_TYPE;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));

    NoOfSubscriptions = CurrNoOfSubscriptions;

    // Get all number of subscriptions
    /*
     * NOTE - RETRIEVE THIS CLIENTS SUBSCRIOTONS IS YET IMPLEMENTATED
     */
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = Request_CBS_GetNumberOfSubscriptions(&RequestCtrl_temp,  // Global RequestCtrl variable which contains socket to be used,
                    TRUE,  // Retreive all client subscriptons
                    &NoOfSubscriptions,    // Number of subscriptions
                    &Error);

    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = Request_CBS_GetNumberOfSubscriptions(&RequestCtrl_temp,  // Global RequestCtrl variable which contains socket to be used,
                    TRUE,  // Retreive all client subscriptons
                    &NoOfSubscriptions,    // Number of subscriptions
                    &Error);

    RequestStatus = Request_CBS_GetNumberOfSubscriptions(NULL,  // Global RequestCtrl variable which contains socket to be used,
                    TRUE,  // Retreive all client subscriptons
                    &NoOfSubscriptions,    // Number of subscriptions
                    &Error);
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_CBS_GetNumberOfSubscriptions(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used,
                    TRUE,  // Retreive all client subscriptons
                    &NoOfSubscriptions,    // Number of subscriptions
                    &Error);
    Response_CBS_GetNumberOfSubscriptions(NULL, NULL, NULL);


    if (RequestStatus == SMS_REQUEST_OK && Error == CBS_ERROR_NONE) {
        SMS_A_(SMS_LOG_I("CBS: CBSTest_GetNoOfSubscriptions: Number Of Subscriptions = %d.", NoOfSubscriptions));

        if (NoOfSubscriptions == CurrNoOfSubscriptions) {
            SMS_A_(SMS_LOG_I("%s: Request_CBS_GetNumberOfSubscriptions PASSED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        } else {
            SMS_A_(SMS_LOG_I("%s: Request_CBS_GetNumberOfSubscriptions Unexpected number. NoOfSubscriptions=%d, CurrNoOfSubscriptions=%d", TestCaseName_p, NoOfSubscriptions, CurrNoOfSubscriptions));
            Result = FALSE;
        }
    } else {
        SMS_A_(SMS_LOG_E("%s: Request_CBS_GetNumberOfSubscriptions FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        Result = FALSE;
    }

    return (Result);
}

/*************************************************************************
CBSTest_GetSubscriptions
**************************************************************************/
uint8_t CBSTest_GetSubscriptions(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    CBS_Error_t Error = CBS_ERROR_LAST_ERROR_TYPE;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    uint8_t NoOfSubscriptionsToGet = NoOfSubscriptions;
    CBS_Subscription_t *CB_Subscriptions_p = NULL;

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));

    CB_Subscriptions_p = malloc(NoOfSubscriptionsToGet * (sizeof(CBS_Subscription_t)));

    if (CB_Subscriptions_p != NULL) {
        memset(CB_Subscriptions_p, '\0', NoOfSubscriptionsToGet *(sizeof(CBS_Subscription_t)));

        // Get all subscriptions
        /*
         * NOTE - RETRIEVE THIS CLIENTS SUBSCRIPTIONS IS YET IMPLEMENTATED
         */
        RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
        RequestStatus = Request_CBS_GetSubscriptions(&RequestCtrl_temp,      // Global RequestCtrl variable which contains socket to be used,
                        TRUE,      // Retreive all client subscriptons
                        &NoOfSubscriptionsToGet,   // Number of subscriptions
                        CB_Subscriptions_p,        // Subscriptions
                        &Error);
        RequestCtrl_temp.WaitMode = 4;
        RequestStatus = Request_CBS_GetSubscriptions(&RequestCtrl_temp,      // Global RequestCtrl variable which contains socket to be used,
                        TRUE,      // Retreive all client subscriptons
                        &NoOfSubscriptionsToGet,   // Number of subscriptions
                        CB_Subscriptions_p,        // Subscriptions
                        &Error);

        RequestStatus = Request_CBS_GetSubscriptions(NULL,      // Global RequestCtrl variable which contains socket to be used,
                        TRUE,      // Retreive all client subscriptons
                        &NoOfSubscriptionsToGet,   // Number of subscriptions
                        CB_Subscriptions_p,        // Subscriptions
                        &Error);
        RequestCtrl.WaitMode = SMS_WAIT_MODE;
        RequestStatus = Request_CBS_GetSubscriptions(&RequestCtrl,      // Global RequestCtrl variable which contains socket to be used,
                        TRUE,      // Retreive all client subscriptons
                        &NoOfSubscriptionsToGet,   // Number of subscriptions
                        CB_Subscriptions_p,        // Subscriptions
                        &Error);

        if (RequestStatus == SMS_REQUEST_OK && Error == CBS_ERROR_NONE) {
            SMS_A_(SMS_LOG_I("CBS: CBSTest_GetSubscriptions: Number Of Subscriptions = %d.", NoOfSubscriptionsToGet));

            if (NoOfSubscriptionsToGet == NoOfSubscriptions) {
                SMS_A_(SMS_LOG_I("%s: Request_CBS_GetSubscriptions PASSED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));

                for (NoOfSubscriptionsToGet = 0; NoOfSubscriptionsToGet < NoOfSubscriptions; NoOfSubscriptionsToGet++) {
                    SMS_A_(SMS_LOG_I("CBS Subscription No:       %d", NoOfSubscriptionsToGet + 1));
                    SMS_A_(SMS_LOG_I("CBS_Subscription FID     = %04x", CB_Subscriptions_p[NoOfSubscriptionsToGet].FirstMessageIdentifier));
                    SMS_A_(SMS_LOG_I("CBS_Subscription LID     = %04x", CB_Subscriptions_p[NoOfSubscriptionsToGet].LastMessageIdentifier));
                    SMS_A_(SMS_LOG_I("CBS_Subscription Fst DCS = %02x", CB_Subscriptions_p[NoOfSubscriptionsToGet].FirstDataCodingScheme));
                    SMS_A_(SMS_LOG_I("CBS_Subscription Lst DCS = %02x\n\n", CB_Subscriptions_p[NoOfSubscriptionsToGet].LastDataCodingScheme));
                }
            } else {
                SMS_A_(SMS_LOG_E("%s: Request_CBS_GetSubscriptions FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
                Result = FALSE;
            }
        } else {
            SMS_A_(SMS_LOG_E("%s: Request_CBS_GetSubscriptions FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
            Result = FALSE;
        }

        free(CB_Subscriptions_p);
    } else {
        SMS_A_(SMS_LOG_E("%s: Alloc Subscription list FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        Result = FALSE;
    }

    return (Result);
}

/*************************************************************************
CBSTest_CellBroadcastMessageRead
**************************************************************************/
uint8_t CBSTest_CellBroadcastMessageRead(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    CBS_Error_t Error = CBS_ERROR_LAST_ERROR_TYPE;
    SMS_RequestControlBlock_t RequestCtrl_temp = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, SMS_NO_PROCESS };

    CB_Pdu_t Pdu = { 0, { 0 } };
    CBS_MessageHeader_t MessageHeader = { 0x00B2,  /* Serial Number */
                                          0x0001,  /* Message ID */
                                          0x01,    /* DCS */
                                          0x11     /* Page Parameter */
                                        };

    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));
    // Read a previously received Cell Broadcast page
    RequestCtrl_temp.WaitMode = SMS_NO_WAIT_MODE;
    RequestStatus = Request_CBS_CellBroadcastMessageRead(&RequestCtrl_temp, &MessageHeader, &Pdu, &Error);

    RequestCtrl_temp.WaitMode = 4;
    RequestStatus = Request_CBS_CellBroadcastMessageRead(&RequestCtrl_temp, &MessageHeader, &Pdu, &Error);

    RequestStatus = Request_CBS_CellBroadcastMessageRead(NULL, &MessageHeader, &Pdu, &Error);
    RequestCtrl.WaitMode = SMS_WAIT_MODE;
    RequestStatus = Request_CBS_CellBroadcastMessageRead(&RequestCtrl, &MessageHeader, &Pdu, &Error);

    if (RequestStatus == SMS_REQUEST_OK && Error == CBS_ERROR_NONE) {
        uint16_t i;
        SMS_A_(SMS_LOG_I("%s: Request_CBS_CellBroadcastMessageRead PASSED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        SMS_A_(SMS_LOG_I("%s, PDU length=%d PDU=", TestCaseName_p, Pdu.PduLength));

        for (i = 0; i < Pdu.PduLength; i++) {
            SMS_A_(SMS_LOG_I("%x", Pdu.Pdu[i]));
        }
    } else {
        SMS_A_(SMS_LOG_E("%s: Request_CBS_CellBroadcastMessageRead FAILED.  Request Status %d, Error Code %d", TestCaseName_p, RequestStatus, Error));
        Result = FALSE;
    }

    return (Result);
}


/*************************************************************************
* Get_SMS_ServerStatusChangeEvent
* Utility function to receive SMS Server Status change events. If the expected
* status is received, it returns TRUE, else FALSE.
*
* N.B. If any events, other than EVENT_SMS_SERVERSTATUSCHANGED, are received
* before the timeout expires, they are thrown away.
**************************************************************************/
static uint8_t Get_SMS_ServerStatusChangeEvent(
    const char *TestCaseName_p,
    const size_t LineNumber,
    const size_t Timeout_Seconds,
    const SMS_ServerStatus_t ExpectedStatus,
    SMS_ServerStatus_t *const ReceivedStatus_p,
    uint32_t *const NumberEventsRxd_p)
{
    uint8_t ExpectedStatusReceived = FALSE;
    uint8_t ContinueLoop;
    uint32_t ReceivedPrimitive = 0;
    fd_set rfds;
    int retval;
    struct timeval tv;
    void *incoming_signal_p = NULL;

    if (ReceivedStatus_p == NULL || NumberEventsRxd_p == NULL) {
        return FALSE;
    }

    *NumberEventsRxd_p = 0;

    do {
        FD_ZERO(&rfds);
        FD_SET(EventSocket, &rfds);
        tv.tv_sec = Timeout_Seconds;
        tv.tv_usec = 0;

        ContinueLoop = FALSE;

        SMS_A_(SMS_LOG_I("%s Line %d: Get_SMS_ServerStatusChangeEvent(): select timeout = %d secs", TestCaseName_p, LineNumber, Timeout_Seconds));
        retval = select(EventSocket + 1, &rfds, NULL, NULL, &tv);

        SMS_A_(SMS_LOG_I("%s Line %d: Get_SMS_ServerStatusChangeEvent(): select returned = %d", TestCaseName_p, LineNumber, retval));

        if (retval > 0) {       // got something to read
            incoming_signal_p = Util_SMS_SignalReceiveOnSocket(EventSocket, &ReceivedPrimitive, NULL);  // ClientTag not wanted
        } else {
            SMS_A_(printf("%s Line %d: Get_SMS_ServerStatusChangeEvent: Timeout: No data to read", TestCaseName_p, LineNumber));
            incoming_signal_p = NULL;
        }

        if (incoming_signal_p != NULL) {
            (*NumberEventsRxd_p)++;

            if (ReceivedPrimitive == EVENT_SMS_SERVERSTATUSCHANGED) {
                SMS_EventStatus_t EventStatus;

                EventStatus = Event_SMS_ServerStatusChanged(incoming_signal_p, ReceivedStatus_p);

                if (EventStatus == SMS_EVENT_OK) {
                    SMS_A_(SMS_LOG_I("%s Line %d: Get_SMS_ServerStatusChangeEvent: Server Status = %d. (Expected Status %d)", TestCaseName_p, LineNumber, *ReceivedStatus_p, ExpectedStatus));

                    if (*ReceivedStatus_p == ExpectedStatus) {
                        ExpectedStatusReceived = TRUE;
                    }
                } else {
                    SMS_A_(SMS_LOG_I("%s Line %d: Get_SMS_ServerStatusChangeEvent: Event_SMS_ServerStatusChanged returned %d", TestCaseName_p, LineNumber, EventStatus));
                }
            } else {
                ContinueLoop = TRUE;
                SMS_A_(SMS_LOG_I("%s Line %d: Get_SMS_ServerStatusChangeEvent: Unwanted Primitive %d (0x%08X). Free it and select again.", TestCaseName_p, LineNumber, ReceivedPrimitive,
                                 ReceivedPrimitive));
            }

            // Free received signal
            Util_SMS_SignalFree(incoming_signal_p);
        }
    } while (ContinueLoop);

    return ExpectedStatusReceived;
}

static const uint8_t CAT_Refresh_EFsms_Path[] = { 0x3F, 0x00, 0x7F, 0x10, 0x6F, 0x3C }; // Test long path
static const uint8_t CAT_Refresh_EFcbmid_Path[] = { 0x6F, 0x48 };       // Test short path
static const uint8_t CAT_Refresh_EFpbr_Path[] = { 0x4F, 0x30 }; // Test path which is irrelevant to SMS and CB Server
static const uint8_t CAT_Refresh_EFtoo_short_Path[] = { 0x4F }; // Test path which is too short to be valid

typedef struct {
    const uint8_t *Path_p;
    uint8_t PathLen;
} CAT_Refresh_paths_data_t;
static const CAT_Refresh_paths_data_t CAT_Refresh_Paths_SMS_CBS[] = {
    {CAT_Refresh_EFsms_Path, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_EFsms_Path)}
    ,
    {CAT_Refresh_EFcbmid_Path, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_EFcbmid_Path)}
    ,
    {CAT_Refresh_EFpbr_Path, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_EFpbr_Path)}
    ,
    {CAT_Refresh_EFtoo_short_Path, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_EFtoo_short_Path)}
};

static const CAT_Refresh_paths_data_t CAT_Refresh_Paths_No_SMS_CBS[] = {
    {CAT_Refresh_EFpbr_Path, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_EFpbr_Path)}
};

static const CAT_Refresh_paths_data_t CAT_Refresh_Paths_SMS[] = {
    {CAT_Refresh_EFsms_Path, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_EFsms_Path)}
};

static const CAT_Refresh_paths_data_t CAT_Refresh_Paths_CBS[] = {
    {CAT_Refresh_EFcbmid_Path, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_EFcbmid_Path)}
};


/*************************************************************************
SMSTest_SMS_CAT_Refresh_SMS_CBS_Handler
**************************************************************************/
uint8_t SMSTest_SMS_CAT_Refresh_SMS_CBS_Handler(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId,
    const CAT_Refresh_paths_data_t RefreshPathData[],
    const size_t RefreshPathDataTableSize)
{
    uint8_t Result = FALSE;

    /*
     * Test case needs to simulate sending CAT Refresh events to the SMS Server to check that it handles them ok.
     * In normal operation, these would be sent from the UICC CAT Server code and would result in callbacks with
     * the appropriate "cause" status settings. However we don't have a stub for the SIM code as we test reading
     * and writing to the SIM EF files which are used by the SMS and CB Servers.
     * Therefore we send signals to the SMS Server process which, when the test harness is enabled, it interprets
     * as CAT Refresh events and calls the same functions as though they came from the UICC CAT Server.
     */

    // Simulate a CAT refresh which means that an SMS and CB EF file may have changed.
    {
        SMS_RequestStatus_t ReqStat;
        SMS_Test_copy_ste_cat_pc_refresh_ind_t refresh_ind_data;

        refresh_ind_data.simd_tag = 0;
        refresh_ind_data.type = SMS_TEST_COPY_STE_SIM_PC_REFRESH_FILE_CHANGE;

        RequestCtrl.WaitMode = SMS_WAIT_MODE;
        ReqStat = Request_SMS_Test_CAT_Cause_PC_RefreshInd(&RequestCtrl, &refresh_ind_data);

        if (ReqStat != SMS_REQUEST_OK) {
            SMS_A_(SMS_LOG_E("%s: Test Fails Here: %d", TestCaseName_p, __LINE__));
        }

        if (ReqStat == SMS_REQUEST_OK) {
            uint8_t Index;
            SMS_Test_copy_ste_cat_pc_refresh_file_ind_t refresh_file_ind_data;
            memset(&refresh_file_ind_data, 0, sizeof(SMS_Test_copy_ste_cat_pc_refresh_file_ind_t));

            for (Index = 0; Index < RefreshPathDataTableSize; Index++) {
                refresh_file_ind_data.simd_tag = 0;
                refresh_file_ind_data.path.pathlen = RefreshPathData[Index].PathLen;
                memcpy(refresh_file_ind_data.path.path, RefreshPathData[Index].Path_p, refresh_file_ind_data.path.pathlen);
                SMS_A_(SMS_LOG_I("%s: Send CAT Refresh FILE IND. Index = %d", TestCaseName_p, Index));
                ReqStat = Request_SMS_Test_CAT_Cause_PC_RefreshFileInd(&RequestCtrl, &refresh_file_ind_data);

                if (ReqStat != SMS_REQUEST_OK) {
                    SMS_A_(SMS_LOG_E("%s: Test Fails Here: %d", TestCaseName_p, __LINE__));
                    break;
                }
            }
        }

        if (ReqStat == SMS_REQUEST_OK) {
            Result = TRUE;
        }
    }

    // Wait for the server status events that are dispatched by SMS Server. This ensures that we are never waiting more than necessary
    // and that the test harness is acting like a good client of SMS Server.
    {
        uint8_t StatusChangeOk;
        SMS_ServerStatus_t ReceivedStatus;
        uint32_t NumberEventsRxd;

        SMS_A_(SMS_LOG_I("%s: Wait up to %ds for SMS_SERVER_STATUS_MTS_BUSY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
        StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_BUSY, &ReceivedStatus, &NumberEventsRxd);

        if (StatusChangeOk) {
            SMS_A_(SMS_LOG_I("%s: Wait up to %ds for SMS_SERVER_STATUS_MTS_READY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
            StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_READY, &ReceivedStatus, &NumberEventsRxd);
        }

        if (!StatusChangeOk) {
            SMS_A_(SMS_LOG_E("%s: Test Fails Here: %d", TestCaseName_p, __LINE__));
            Result = FALSE;
        }
    }

    return Result;
}


/*************************************************************************
SMSTest_SMS_CAT_Refresh_SMS_CBS
**************************************************************************/
uint8_t SMSTest_SMS_CAT_Refresh_SMS_CBS(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    return SMSTest_SMS_CAT_Refresh_SMS_CBS_Handler(TestCaseName_p, TestCaseFrameworkId, CAT_Refresh_Paths_SMS_CBS, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_Paths_SMS_CBS));
}


/*************************************************************************
SMSTest_SMS_CAT_Refresh_SMS
**************************************************************************/
uint8_t SMSTest_SMS_CAT_Refresh_SMS(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    return SMSTest_SMS_CAT_Refresh_SMS_CBS_Handler(TestCaseName_p, TestCaseFrameworkId, CAT_Refresh_Paths_SMS, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_Paths_SMS));
}


/*************************************************************************
SMSTest_SMS_CAT_Refresh_CBS
**************************************************************************/
uint8_t SMSTest_SMS_CAT_Refresh_CBS(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    return SMSTest_SMS_CAT_Refresh_SMS_CBS_Handler(TestCaseName_p, TestCaseFrameworkId, CAT_Refresh_Paths_CBS, SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_Paths_CBS));
}


/*************************************************************************
SMSTest_SMS_CAT_Refresh_No_SMS_CBS
**************************************************************************/
uint8_t SMSTest_SMS_CAT_Refresh_No_SMS_CBS(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = FALSE;

    /*
     * Test case needs to simulate sending CAT Refresh events to the SMS Server to check that it handles them ok.
     * In normal operation, these would be sent from the UICC CAT Server code and would result in callbacks with
     * the appropriate "cause" status settings. However we don't have a stub for the SIM code as we test reading
     * and writing to the SIM EF files which are used by the SMS and CB Servers.
     * Therefore we send signals to the SMS Server process which, when the test harness is enabled, it interprets
     * as CAT Refresh events and calls the same functions as though they came from the UICC CAT Server.
     */

    // Simulate a CAT refresh which means that no SMS or CB EF files will have changed.
    {
        SMS_RequestStatus_t ReqStat;
        SMS_Test_copy_ste_cat_pc_refresh_ind_t refresh_ind_data;

        refresh_ind_data.simd_tag = 0;
        refresh_ind_data.type = SMS_TEST_COPY_STE_SIM_PC_REFRESH_FILE_CHANGE;

        RequestCtrl.WaitMode = SMS_WAIT_MODE;
        ReqStat = Request_SMS_Test_CAT_Cause_PC_RefreshInd(&RequestCtrl, &refresh_ind_data);

        if (ReqStat != SMS_REQUEST_OK) {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }

        if (ReqStat == SMS_REQUEST_OK) {
            uint8_t Index;
            SMS_Test_copy_ste_cat_pc_refresh_file_ind_t refresh_file_ind_data;
            memset(&refresh_file_ind_data, 0, sizeof(SMS_Test_copy_ste_cat_pc_refresh_file_ind_t));

            for (Index = 0; Index < SMS_TEST_ELEMENTS_OF_ARRAY(CAT_Refresh_Paths_No_SMS_CBS); Index++) {
                refresh_file_ind_data.simd_tag = 0;
                refresh_file_ind_data.path.pathlen = CAT_Refresh_Paths_No_SMS_CBS[Index].PathLen;
                memcpy(refresh_file_ind_data.path.path, CAT_Refresh_Paths_No_SMS_CBS[Index].Path_p, refresh_file_ind_data.path.pathlen);
                SMS_A_(SMS_LOG_I("%s: Send CAT Refresh FILE IND. Index = %d", TestCaseName_p, Index));
                ReqStat = Request_SMS_Test_CAT_Cause_PC_RefreshFileInd(&RequestCtrl, &refresh_file_ind_data);

                if (ReqStat != SMS_REQUEST_OK) {
                    SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
                    break;
                }
            }
        }

        if (ReqStat == SMS_REQUEST_OK) {
            Result = TRUE;
        }
    }

    // As the CAT Refresh is not updating EF files used by the SMS / CBS code we do a check to ensure that no events are received.
    {
        SMS_ServerStatus_t ReceivedStatus;
        uint32_t NumberEventsRxd = 0;

        SMS_A_(SMS_LOG_I("%s: Wait up to %ds for any events (Not expecting any to be found!)", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS__EVENT_NOT_EXPECT_EXPECTED));
        (void) Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS__EVENT_NOT_EXPECT_EXPECTED, SMS_SERVER_STATUS_MTS_BUSY, &ReceivedStatus,
                                               &NumberEventsRxd);

        if (NumberEventsRxd > 0) {
            SMS_A_(SMS_LOG_E("%s: %d Unexpected events received.", TestCaseName_p, NumberEventsRxd));
            SMS_A_(SMS_LOG_E("Test Fails Here: %d.", __LINE__));
            Result = FALSE;
        }
    }

    return Result;
}


/*************************************************************************
SMSTest_SMS_CAT_Refresh_All
**************************************************************************/
uint8_t SMSTest_SMS_CAT_Refresh_All(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = FALSE;

    /*
     * Test case needs to simulate sending CAT Refresh events to the SMS Server to check that it handles them ok.
     * In normal operation, these would be sent from the UICC CAT Server code and would result in callbacks with
     * the appropriate "cause" status settings. However we don't have a stub for the SIM code as we test reading
     * and writing to the SIM EF files which are used by the SMS and CB Servers.
     * Therefore we send signals to the SMS Server process which, when the test harness is enabled, it interprets
     * as CAT Refresh events and calls the same functions as though they came from the UICC CAT Server.
     */

    // Simulate a CAT refresh which means that all EF files may have changed.
    {
        SMS_RequestStatus_t ReqStat;
        SMS_Test_copy_ste_cat_pc_refresh_ind_t refresh_ind_data;

        refresh_ind_data.simd_tag = 0;
        refresh_ind_data.type = SMS_TEST_COPY_STE_SIM_PC_REFRESH_INIT_AND_FULL_FILE_CHANGE;

        RequestCtrl.WaitMode = SMS_WAIT_MODE;
        ReqStat = Request_SMS_Test_CAT_Cause_PC_RefreshInd(&RequestCtrl, &refresh_ind_data);

        if (ReqStat == SMS_REQUEST_OK) {
            Result = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d", __LINE__));
        }
    }

    // Wait for the server status events that are dispatched by SMS Server. This ensures that we are never waiting more than necessary
    // and that the test harness is acting like a good client of SMS Server.
    {
        uint8_t StatusChangeOk;
        SMS_ServerStatus_t ReceivedStatus;
        uint32_t NumberEventsRxd;

        SMS_A_(SMS_LOG_I("%s: Wait up to %ds for SMS_SERVER_STATUS_MTS_BUSY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
        StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_BUSY, &ReceivedStatus, &NumberEventsRxd);

        if (StatusChangeOk) {
            SMS_A_(SMS_LOG_I("%s: Wait up to %ds for SMS_SERVER_STATUS_MTS_READY", TestCaseName_p, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS));
            StatusChangeOk = Get_SMS_ServerStatusChangeEvent(TestCaseName_p, __LINE__, SMS_SERVER_STATUS_EVENT_CHANGE_TIMEOUT_SECS, SMS_SERVER_STATUS_MTS_READY, &ReceivedStatus, &NumberEventsRxd);
        }

        if (!StatusChangeOk) {
            SMS_A_(SMS_LOG_E("Test Fails Here: %d.", __LINE__));
            Result = FALSE;
        }
    }

    return Result;
}


/*************************************************************************
* SMSTest_Util_EmptySignalQueue
*
* Utility "test" used to ensure that signal queue is empty before moving
* to next test case.
**************************************************************************/
#ifndef SMS_TEST_CASE_EMPTY_SIGNAL_QUEUE_TIMEOUT_SECS
#define SMS_TEST_CASE_EMPTY_SIGNAL_QUEUE_TIMEOUT_SECS 2
#endif
uint8_t SMSTest_Util_EmptySignalQueue(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    SMS_A_(SMS_LOG_I("%s: TEST BEGIN", TestCaseName_p));

    while (TRUE) {
        fd_set rfds;
        int retval = 0;
        int Socket = -1;
        union SMS_SIGNAL *ReceivedSignal_p = NULL;
        uint32_t primitive = 0;
        struct timeval tv;

        FD_ZERO(&rfds);
        FD_SET(RequestResponseSocket, &rfds);
        FD_SET(EventSocket, &rfds);
        /* Wait a few seconds in select before timing out. */
        tv.tv_sec = SMS_TEST_CASE_EMPTY_SIGNAL_QUEUE_TIMEOUT_SECS;
        tv.tv_usec = 0;

        retval = select(EventSocket + 1, &rfds, NULL, NULL, &tv);

        SMS_A_(SMS_LOG_D("select returned = %d.", retval));

        if (retval > 0) {
            if (FD_ISSET(RequestResponseSocket, &rfds)) {
                Socket = RequestResponseSocket;
                SMS_A_(SMS_LOG_D("select on RequestResponseSocket"));
            } else if (FD_ISSET(EventSocket, &rfds)) {
                Socket = EventSocket;
                SMS_A_(SMS_LOG_D("select on EventSocket"));
            } else {
                SMS_A_(SMS_LOG_E("ERROR: select on Unknown Socket"));
                break;
            }

            ReceivedSignal_p = Util_SMS_SignalReceiveOnSocket(Socket, &primitive, NULL);
        } else {
            break;
        }

        if (ReceivedSignal_p != NULL) {
            SMS_A_(SMS_LOG_I("%s: Signal Primitive 0x%08X", TestCaseName_p, primitive));
            Util_SMS_SignalFree(ReceivedSignal_p);      // free sig buffer
        } else {
            SMS_A_(SMS_LOG_E("%s: select returned NULL", TestCaseName_p));
        }
    }

    return TRUE;
}

/* This test case is used to test the functionality of SMS Control on Sending an SMS to the network*/
uint8_t SMSTest_SMS_Control(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t Result = FALSE;
    const SMS_ClientTag_t RequestClientTag = 42;
    uint8_t MoreToSend = FALSE;
    uint8_t SM_Reference = 0;

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_allowed = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x44, 0x52, 0x78, 0x63, 0xF5}   // The address value in packed format.
        },
        {
            // TPDU
            0x10,                  // Indicates the length of the TPDU data.
            {0x01, 0x04, 0x0D, 0x91, 0x11, 0x11, 0x99, 0x03, 0x10, 0x57, 0xF9, 0x00, 0x00, 0x03, 0x47, 0xB3, 0x19} // TPDU data
        }
    };

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_disallowed = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x44, 0x52, 0x78, 0x63, 0xF5}   // The address value in packed format.
        },
        {
            // TPDU
            0x10,                  // Indicates the length of the TPDU data.
            {0x01, 0x04, 0x0D, 0x91, 0x22, 0x22, 0x99, 0x03, 0x10, 0x57, 0xF9, 0x00, 0x00, 0x03, 0x47, 0xB3, 0x19} // TPDU data
        }
    };

    static const SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU_allowed_with_mod = {
        {
            // Address
            0x05,                  // The number of bytes in the AddressValue.
            0x91,                  // Type of address as defined in the standards.
            {0x44, 0x52, 0x78, 0x63, 0xF5}   // The address value in packed format.
        },
        {
            // TPDU
            0x10,                  // Indicates the length of the TPDU data.
            {0x01, 0x04, 0x0D, 0x91, 0x33, 0x33, 0x99, 0x03, 0x10, 0x57, 0xF9, 0x00, 0x00, 0x03, 0x47, 0xB3, 0x19} // TPDU data
        }
    };

    /* Test SMS control allowed */
    SMSTest_PrintFrameworkIdToLog(TestCaseName_p, TestCaseFrameworkId);

    // Ensure global parameter configured for wait mode.
    RequestCtrl.WaitMode = SMS_NO_WAIT_MODE;
    RequestCtrl.ClientTag = RequestClientTag;

    /* Test sms allowed */
    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU_allowed, NULL,      // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_PENDING && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(printf("\nSMS TEST: %s: Wait for Response Signal from SMS Server\n", TestCaseName_p));

        while (TRUE) {
            // Now wait for async signal
            static const SMS_SIGSELECT SignalSelector[] = { 1, RESPONSE_SMS_SHORTMESSAGESEND };
            void *Rec_p;
            uint8_t SM_Ref;

            // Wait for response from the SMS Server
            Rec_p = SMS_RECEIVE_ON_SOCKET(SignalSelector, RequestResponseSocket);

            if (Rec_p != NULL) {
                if (((union SMS_SIGNAL *) Rec_p)->Primitive == RESPONSE_SMS_SHORTMESSAGESEND) {
                    //Get the Client Tag
                    SMS_ClientTag_t ClientTag;

                    Do_SMS_ClientTag_Get(Rec_p, &ClientTag);

                    // Unpack the returned signal into the output parameters
                    RequestStatus = Response_SMS_ShortMessageSend(Rec_p, &SM_Ref, &ErrorCode);

                    SMS_A_(printf("\nSMS TEST: %s: RESPONSE_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x, ClientTag = %d\n", TestCaseName_p, RequestStatus, ErrorCode, ClientTag));

                    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && ClientTag == RequestClientTag) {
                        SMS_A_(printf("\nSMS TEST: %s: RESPONSE_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x\n", TestCaseName_p, SM_Reference));
                        Result = TRUE;
                    } else {
                        SMS_A_(printf("\nSMS TEST fails: %s: RESPONSE_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x\n", TestCaseName_p, SM_Reference));
                        Result = FALSE;
                    }

                    SMS_SIGNAL_FREE(&Rec_p);
                    // Exit while loop.
                    break;

                } else {
                    SMS_A_(printf("\nSMS TEST: %s: SMS_RECEIVE_ON_SOCKET returned unexpected primitive 0x%08X\n", TestCaseName_p, (unsigned int)((union SMS_SIGNAL *) Rec_p)->Primitive));
                    SMS_SIGNAL_FREE(&Rec_p);
                }
            } else {
                SMS_A_(printf("\nSMS TEST: %s: SMS_RECEIVE_ON_SOCKET returned NULL\n", TestCaseName_p));
            }
        }
    }

    if (Result != TRUE) {
        goto Error;
    }

    /* Next test, send sms disallowed */
    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU_disallowed, NULL,      // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_PENDING && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(printf("\nSMS TEST: %s: Wait for Response Signal from SMS Server\n", TestCaseName_p));

        while (TRUE) {
            // Now wait for async signal
            static const SMS_SIGSELECT SignalSelector[] = { 1, RESPONSE_SMS_SHORTMESSAGESEND };
            void *Rec_p;
            uint8_t SM_ref;

            // Wait for response from the SMS Server
            Rec_p = SMS_RECEIVE_ON_SOCKET(SignalSelector, RequestResponseSocket);

            if (Rec_p != NULL) {
                if (((union SMS_SIGNAL *) Rec_p)->Primitive == RESPONSE_SMS_SHORTMESSAGESEND) {
                    //Get the Client Tag
                    SMS_ClientTag_t ClientTag;

                    Do_SMS_ClientTag_Get(Rec_p, &ClientTag);

                    // Unpack the returned signal into the output parameters
                    RequestStatus = Response_SMS_ShortMessageSend(Rec_p, &SM_ref, &ErrorCode);

                    SMS_A_(printf("\nSMS TEST: %s: RESPONSE_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x, ClientTag = %d\n", TestCaseName_p, RequestStatus, ErrorCode, ClientTag));

                    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && ClientTag == RequestClientTag) {
                        SMS_A_(printf("\nSMS TEST: %s: RESPONSE_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x\n", TestCaseName_p, SM_Reference));
                        Result = TRUE;
                    } else {
                        SMS_A_(printf("\nSMS TEST Fails: %s: RESPONSE_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x\n", TestCaseName_p, SM_Reference));
                        Result = FALSE;
                    }

                    SMS_SIGNAL_FREE(&Rec_p);
                    // Exit while loop.
                    break;

                } else {
                    SMS_A_(printf("\nSMS TEST: %s: SMS_RECEIVE_ON_SOCKET returned unexpected primitive 0x%08X\n", TestCaseName_p, (unsigned int)((union SMS_SIGNAL *) Rec_p)->Primitive));
                    SMS_SIGNAL_FREE(&Rec_p);
                }
            } else {
                SMS_A_(printf("\nSMS TEST: %s: SMS_RECEIVE_ON_SOCKET returned NULL\n", TestCaseName_p));
            }
        }
    }

    if (Result != TRUE) {
        goto Error;
    }

    /* Test sms allowed with modifications */
    RequestStatus = Request_SMS_ShortMessageSend(&RequestCtrl,  // Global RequestCtrl variable which contains socket to be used.
                    &SMSC_Address_TPDU_allowed_with_mod, NULL,      // Slot
                    MoreToSend, &SM_Reference, &ErrorCode);

    if (RequestStatus == SMS_REQUEST_PENDING && ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(printf("\nSMS TEST: %s: Wait for Response Signal from SMS Server\n", TestCaseName_p));

        while (TRUE) {
            // Now wait for async signal
            static const SMS_SIGSELECT SignalSelector[] = { 1, RESPONSE_SMS_SHORTMESSAGESEND };
            void *Rec_p;
            uint8_t SM_Ref;

            // Wait for response from the SMS Server
            Rec_p = SMS_RECEIVE_ON_SOCKET(SignalSelector, RequestResponseSocket);

            if (Rec_p != NULL) {
                if (((union SMS_SIGNAL *) Rec_p)->Primitive == RESPONSE_SMS_SHORTMESSAGESEND) {
                    //Get the Client Tag
                    SMS_ClientTag_t ClientTag;

                    Do_SMS_ClientTag_Get(Rec_p, &ClientTag);

                    // Unpack the returned signal into the output parameters
                    RequestStatus = Response_SMS_ShortMessageSend(Rec_p, &SM_Ref, &ErrorCode);

                    SMS_A_(printf("\nSMS TEST: %s: RESPONSE_SMS_SHORTMESSAGESEND, RequestStatus = 0x%08x, ErrorCode = 0x%08x, ClientTag = %d\n", TestCaseName_p, RequestStatus, ErrorCode, ClientTag));

                    if (RequestStatus == SMS_REQUEST_OK && ErrorCode == SMS_ERROR_INTERN_NO_ERROR && ClientTag == RequestClientTag) {
                        SMS_A_(printf("\nSMS TEST: %s: RESPONSE_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x\n", TestCaseName_p, SM_Reference));
                        Result = TRUE;
                    } else {
                        SMS_A_(printf("\nSMS TEST fails: %s: RESPONSE_SMS_SHORTMESSAGESEND, TP-MR = 0x%02x\n", TestCaseName_p, SM_Reference));
                        Result = FALSE;
                    }

                    SMS_SIGNAL_FREE(&Rec_p);
                    // Exit while loop.
                    break;

                } else {
                    SMS_A_(printf("\nSMS TEST: %s: SMS_RECEIVE_ON_SOCKET returned unexpected primitive 0x%08X\n", TestCaseName_p, (unsigned int)((union SMS_SIGNAL *) Rec_p)->Primitive));
                    SMS_SIGNAL_FREE(&Rec_p);
                }
            } else {
                SMS_A_(printf("\nSMS TEST: %s: SMS_RECEIVE_ON_SOCKET returned NULL\n", TestCaseName_p));
            }
        }
    }

Error:
    return Result;
}

/*   This test case is used to test the functionality of unpacking an unsolicited error
  * event to a subscriber.*/
uint8_t SMSTest_SMS_errorOccured(
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId)
{
    uint8_t Result = TRUE;
    //uint32_t ReceivedSignal_p =110;

    SMS_A_(SMS_LOG_I("%s: select() returned EVENT_SMS_ERROROCCURRED", TestCaseName_p));

    SMS_EventStatus_t EventStatus;
    SMS_Error_t Error;

    //memset(RxdSR_p, '\0', sizeof(SMS_StatusReport_t));

    // Unpack event
    EventStatus = Event_SMS_ErrorOccurred(NULL, &Error);

    if (EventStatus == SMS_EVENT_OK) {
        Result = TRUE;
    } else {
        SMS_A_(printf("%s: EVENT_SMS_ErrorOccurred failed %d", TestCaseName_p, EventStatus));
        Result = FALSE;
    }

    return Result;

}
