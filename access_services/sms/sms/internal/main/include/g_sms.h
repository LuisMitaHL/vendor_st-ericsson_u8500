#ifndef G_SMS_H
#define G_SMS_H
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

#include "r_sms.h"
#include "t_sms.h"
#include "t_smslinuxporting.h"

/*************************************************************************
* SMS Server thread id
*************************************************************************/
#define SMS_SERVERTHREAD_    (999)
#ifndef HAVE_ANDROID_OS
#define SMS_SERVER_SOCK_PATH "/tmp/socket_sms_server"
#else
#define SMS_SERVER_SOCK_PATH "/dev/socket/sms_server"
#endif


/*************************************************************************
* SMS Server Internal Client Tags
*************************************************************************/
#define SMS_TIMER_DEFAULT_CLIENT_TAG  (0)

/*************************************************************************
* Numbering of primitives sent from MSG to another process YYY
*************************************************************************/


// Type for basic reception of signals.- temporary!
typedef struct SMS_Create_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_Create_Req_t;

/*
**========================================================================
** Requester/Response SWBP transport data types
**========================================================================
*/


//========================================================================
typedef struct SMS_ShortMessageDeliver_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU;
} SMS_ShortMessageDeliver_Req_t;

typedef struct SMS_ShortMessageDeliver_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_ShortMessageDeliver_Resp_t;


//========================================================================
typedef struct SMS_Events_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_Events_Req_t;

typedef struct SMS_Events_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_Events_Resp_t;


//========================================================================
typedef struct SMS_CB_SessionCreate_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_CB_SessionCreate_Req_t;

typedef struct SMS_CB_SessionCreate_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_CB_SessionCreate_Resp_t;


//========================================================================
typedef struct SMS_CB_Shutdown_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_CB_Shutdown_Req_t;

//========================================================================
typedef struct SMS_CB_SessionDestroy_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_CB_SessionDestroy_Req_t;

typedef struct SMS_CB_SessionDestroy_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_CB_SessionDestroy_Resp_t;


//========================================================================
typedef struct SMS_ApplicationPort_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_ApplicationPortRange_t ApplicationPortRange;
} SMS_ApplicationPort_Req_t;

typedef struct SMS_ApplicationPort_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_ApplicationPort_Resp_t;


//========================================================================
typedef struct SMS_ShortMessageInfoRead_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t Slot;
} SMS_ShortMessageInfoRead_Req_t;

typedef struct SMS_ShortMessageInfoRead_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_ReadInformation_t ReadInformation;
    SMS_Error_t ErrorCode;
} SMS_ShortMessageInfoRead_Resp_t;

//========================================================================
typedef struct SMS_MemCapacityFullReport_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_MemCapacityFullReport_Req_t;

typedef struct SMS_MemCapacityFullReport_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_MemCapacityFullReport_Resp_t;

//========================================================================
typedef struct SMS_StorageClear_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Storage_t Storage;
    uint8_t AllThisICCID_Messages;
    uint8_t AllOtherICCID_Messages;
} SMS_StorageClear_Req_t;

typedef struct SMS_StorageClear_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_StorageClear_Resp_t;


//========================================================================
typedef struct SMS_ShortMessageRead_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t Slot;
    SMS_Status_t Status;
    SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU;
} SMS_ShortMessageRead_Req_t;

typedef struct SMS_ShortMessageRead_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Status_t Status;
    SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU;
    SMS_Error_t ErrorCode;
} SMS_ShortMessageRead_Resp_t;


//========================================================================
typedef struct SMS_ShortMessageWrite_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t Slot;
    SMS_Status_t Status;
    SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU;
} SMS_ShortMessageWrite_Req_t;

typedef struct SMS_ShortMessageWrite_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t Slot;
    SMS_Error_t ErrorCode;
} SMS_ShortMessageWrite_Resp_t;


//========================================================================
typedef struct SMS_ShortMessageDelete_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t Slot;
} SMS_ShortMessageDelete_Req_t;

typedef struct SMS_ShortMessageDelete_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_ShortMessageDelete_Resp_t;


//========================================================================
typedef struct SMS_StatusReportRead_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t Slot;
} SMS_StatusReportRead_Req_t;

typedef struct SMS_StatusReportRead_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t Slot;
    SMS_StatusReport_t TPDU_StatusReport;
    SMS_Error_t ErrorCode;
} SMS_StatusReportRead_Resp_t;


//========================================================================
typedef struct SMS_ShortMessageStatusSet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t Slot;
    SMS_Status_t Status;
} SMS_ShortMessageStatusSet_Req_t;

typedef struct SMS_ShortMessageStatusSet_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_ShortMessageStatusSet_Resp_t;


//========================================================================
typedef struct SMS_ShortMessageSend_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU;
    SMS_Slot_t Slot;
    uint8_t MoreToSend;
    uint8_t SM_Reference;
    uint8_t SatInitiated;
} SMS_ShortMessageSend_Req_t;

typedef struct SMS_ShortMessageSend_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    uint8_t SM_Reference;
    SMS_Error_t ErrorCode;
} SMS_ShortMessageSend_Resp_t;


//========================================================================
typedef struct SMS_ShortMessageFind_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_SearchInfo_t SearchInfo;
} SMS_ShortMessageFind_Req_t;

typedef struct SMS_ShortMessageFind_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t Slot;
    SMS_Error_t ErrorCode;
} SMS_ShortMessageFind_Resp_t;


//========================================================================
typedef struct SMS_StorageCapacityGet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Storage_t Storage;
} SMS_StorageCapacityGet_Req_t;

typedef struct SMS_StorageCapacityGet_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_SlotInformation_t SlotInfo;
    SMS_StorageStatus_t StorageStatus;
    SMS_Error_t ErrorCode;
} SMS_StorageCapacityGet_Resp_t;


//========================================================================
typedef struct SMS_PreferredStorageSet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Storage_t PreferredStorage;
} SMS_PreferredStorageSet_Req_t;

typedef struct SMS_PreferredStorageSet_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_PreferredStorageSet_Resp_t;

//========================================================================
typedef struct SMS_DeliverReportControlSet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_NetworkAcknowledge_t NetworkAcknowledge;
} SMS_DeliverReportControlSet_Req_t;

typedef struct SMS_DeliverReportControlSet_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_DeliverReportControlSet_Resp_t;

//========================================================================
typedef struct SMS_DeliverReportControlGet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_DeliverReportControlGet_Req_t;

typedef struct SMS_DeliverReportControlGet_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_NetworkAcknowledge_t NetworkAcknowledge;
    SMS_Error_t ErrorCode;
} SMS_DeliverReportControlGet_Resp_t;

//========================================================================
typedef struct SMS_MO_RouteSet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_MO_Route_t Route;
} SMS_MO_RouteSet_Req_t;

typedef struct SMS_MO_RouteSet_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_MO_RouteSet_Resp_t;

//========================================================================
typedef struct SMS_MO_RouteGet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_MO_RouteGet_Req_t;

typedef struct SMS_MO_RouteGet_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_MO_Route_t Route;
    SMS_Error_t ErrorCode;
} SMS_MO_RouteGet_Resp_t;

//========================================================================
typedef struct SMS_RelayLinkControlSet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_RelayControl_t RelayLinkControl;
} SMS_RelayLinkControlSet_Req_t;

typedef struct SMS_RelayLinkControlSet_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_RelayLinkControlSet_Resp_t;


//========================================================================
typedef struct SMS_RelayLinkControlGet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_RelayLinkControlGet_Req_t;

typedef struct SMS_RelayLinkControlGet_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_RelayControl_t RelayLinkControl;
    SMS_Error_t ErrorCode;
} SMS_RelayLinkControlGet_Resp_t;


//========================================================================
typedef struct SMS_DeliverReportSend_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_RP_ErrorCause_t RP_ErrorCause;
    SMS_TPDU_t TPDU;
} SMS_DeliverReportSend_Req_t;

typedef struct SMS_DeliverReportSend_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_DeliverReportSend_Resp_t;


//========================================================================
typedef struct SMS_MemCapacityAvailableSend_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_MemCapacityAvailableSend_Req_t;

typedef struct SMS_MemCapacityAvailableSend_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} SMS_MemCapacityAvailableSend_Resp_t;

//========================================================================
typedef struct SMS_MemCapacityGetState_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_MemCapacityGetState_Req_t;

typedef struct SMS_MemCapacityGetState_Resp {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    int memFullState;
    SMS_Error_t ErrorCode;
} SMS_MemCapacityGetState_Resp_t;

/*
**========================================================================
** Event unpack Data types
**========================================================================
*/

//========================================================================
typedef struct Event_SMS_ServerStatusChanged {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_ServerStatus_t ServerStatus;
} Event_SMS_ServerStatusChanged_t;


//========================================================================
typedef struct Event_SMS_ErrorOccurred {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t ErrorCode;
} Event_SMS_ErrorOccurred_t;


//========================================================================
typedef struct Event_SMS_StorageStatusChanged {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_StorageStatus_t StorageStatus;
} Event_SMS_StorageStatusChanged_t;


//========================================================================
typedef struct Event_SMS_ShortMessageReceived {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_ShortMessageReceived_t Data;
} Event_SMS_ShortMessageReceived_t;


//========================================================================
typedef struct Event_SMS_ApplicationPortMatched {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_ApplicationPortRange_t ApplicationPortRange;
    SMS_ShortMessageReceived_t Data;
} Event_SMS_ApplicationPortMatched_t;


//========================================================================
typedef struct Event_SMS_StatusReportReceived {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Slot_t SlotNumber;
    SMS_SMSC_Address_TPDU_t TPDU_StatusReport;
} Event_SMS_StatusReportReceived_t;



/*
**========================================================================
** Test Harness Data types
**========================================================================
*/

//========================================================================
#ifdef SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED
typedef struct SMS_TestUtil_MemoryStatusGet_Req {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_TestUtil_MemoryStatusGet_Req_t;
#endif

//========================================================================
#ifdef SMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED
// SMS Test Harness Code is enabled to simulate CAT Refresh events.

// We need to have copies of the data structures used on the UICC CAT Server API
// defined here, in this code used by the test harness, because this header file
// cannot have dependencies on sim.h. SMS_COMPILE_TIME_ASSERT_STATEMENT macros
// will be used to check that these structures and enums remain valid when we
// have enabled the compile switch SMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED.
typedef enum {
    SMS_TEST_COPY_STE_SIM_PC_REFRESH_INIT_AND_FULL_FILE_CHANGE = 0x00,
    SMS_TEST_COPY_STE_SIM_PC_REFRESH_FILE_CHANGE,
    SMS_TEST_COPY_STE_SIM_PC_REFRESH_INIT_AND_FILE_CHANGE,
    SMS_TEST_COPY_STE_SIM_PC_REFRESH_INIT,
    SMS_TEST_COPY_STE_SIM_PC_REFRESH_UICC_RESET,
    SMS_TEST_COPY_STE_SIM_PC_REFRESH_3G_APP_RESET,
    SMS_TEST_COPY_STE_SIM_PC_REFRESH_3G_SESSION_RESET,
    SMS_TEST_COPY_STE_SIM_PC_REFRESH_RESERVED
} SMS_Test_copy_ste_sim_pc_refresh_type_t;

typedef struct {
    uintptr_t simd_tag;
    SMS_Test_copy_ste_sim_pc_refresh_type_t type;
} SMS_Test_copy_ste_cat_pc_refresh_ind_t;

#define SMS_TEST_COPY_SIM_MAX_PATH_LENGTH  10
typedef struct {
    uint8_t pathlen;
    uint8_t path[SMS_TEST_COPY_SIM_MAX_PATH_LENGTH];
} SMS_Test_copy_sim_path_t;

typedef struct {
    uintptr_t simd_tag;
    SMS_Test_copy_sim_path_t path;
} SMS_Test_copy_ste_cat_pc_refresh_file_ind_t;


typedef struct SMS_Test_CAT_Cause_PC_Refresh_Ind {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Test_copy_ste_cat_pc_refresh_ind_t refresh_ind_data;
} SMS_Test_CAT_Cause_PC_Refresh_Ind_t;

typedef struct SMS_Test_CAT_Cause_PC_Refresh_File_Ind {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Test_copy_ste_cat_pc_refresh_file_ind_t refresh_file_ind_data;
} SMS_Test_CAT_Cause_PC_Refresh_File_Ind_t;
#endif

#endif                          // G_SMS_H
