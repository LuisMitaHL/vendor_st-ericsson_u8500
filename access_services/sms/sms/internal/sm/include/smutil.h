#ifndef SMUTIL_H
#define SMUTIL_H
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
 *  The Short Message Bearer Object Utility.
 *
 *************************************************************************/
#include "r_smslinuxporting.h"

#include "r_sms.h"

#include "sm.h"

#ifdef SMS_USE_MFL_MODEM
#include "t_rms.h"
#else
#include "mal_sms.h"
#endif                          // SMS_USE_MFL_MODEM

#include "sim.h"


#define MTI_MASK                          0x03  // Mask for SMS Message Type Indicator
#define VPF_MASK                          0x18  /* Validity period format */
#define TP_VPF_MISSING                    0x00  /* bit 3 and 4 */
#define TP_VPF_INTEGER_REPRESENTATION     0x10  /* bit 3 and 4 */
#define DCS_MC_MASK                       0xF4
#define GENERAL_DCS_MASK                  0xCC  // bits 7&6, bits 3&2
#define SMS_DELIVER_MTI                   0x00
#define SMS_SUBMIT_MTI                    0x01
#define SMS_STATUS_REPORT_MTI             0x02
#define SMS_RESERVED_MTI                  0x03
#define TP_PID_MASK                       0x01
#define TP_DCS_MASK                       0x02
#define TP_UDL_MASK                       0x04


typedef struct MSG_MemoryCapacityExceededFlag_t {
    uint8_t SMS_MemoryCapacityExceededFlag;
} MSG_MemoryCapacityExceededFlag_t;


//-------------------------------------------------------------------------------
// SMS Persistent Data constants and interface
//-------------------------------------------------------------------------------
// Global constants for top-level FS directories.
#ifndef SMS_FS_DEFAULT_TOP_MOUNT_POINT
// The top-level mount point has not been overridden with a compile-switch
// so use the default value of /dev.

#ifndef HAVE_ANDROID_OS
#ifdef SMS_SIM_TEST_PLATFORM
#define SMS_FS_DEFAULT_TOP_MOUNT_POINT "."
#else
#define SMS_FS_DEFAULT_TOP_MOUNT_POINT "/tmp"
#endif
#else
#define SMS_FS_DEFAULT_TOP_MOUNT_POINT "/dev"
#endif
#endif

extern const char SMS_FS_DIR_TOP_MOUNT_POINT[];
extern const char SMS_FS_DIR_MESSAGING[];

typedef enum {
    SMS_PERSISTENT_STORAGE_DATA_TYPE_MEMORY_CAPACITY_EXCEEDED_FLAG,
    SMS_PERSISTENT_STORAGE_DATA_TYPE_MESSAGE_REFERENCE,
    SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT,   // Count of short messages stored in ME.
    SMS_PERSISTENT_STORAGE_DATA_TYPE_ICCID
} SMS_PersistentStorageDataType_t;

SMS_Error_t SmUtil_PersistentStorageData_Read(
    const SMS_PersistentStorageDataType_t DataType,
    const size_t ReadSize,
    void *const Data_p);

SMS_Error_t SmUtil_PersistentStorageData_Write(
    const SMS_PersistentStorageDataType_t DataType,
    const size_t WriteSize,
    const void *const Data_p);


//-------------------------------------------------------------------------------
// Messaging heap malloc interface
//-------------------------------------------------------------------------------
#ifdef SMS_HEAP_CHECK
void *SmUtil_HeapUntypedAlloc(
    const uint32_t sizeToMalloc,
    const char *filename,
    const size_t line);
void SmUtil_HeapFree(
    void *memoryToFree,
    const char *filename,
    const size_t line);
uint32_t SmUtil_HeapStatus(
    void);

#ifdef SMS_HEAP_ALLOC
#undef SMS_HEAP_ALLOC
#endif
#define SMS_HEAP_ALLOC(t)     (t*) SmUtil_HeapUntypedAlloc(sizeof(t), __FILE__, __LINE__)

#ifdef SMS_HEAP_UNTYPED_ALLOC
#undef SMS_HEAP_UNTYPED_ALLOC
#endif
#define SMS_HEAP_UNTYPED_ALLOC(x)  SmUtil_HeapUntypedAlloc(x, __FILE__, __LINE__)

#ifdef SMS_HEAP_REALLOC
#undef SMS_HEAP_REALLOC
#endif
//#define SMS_HEAP_REALLOC(x,y) realloc(x, y)

#ifdef SMS_HEAP_FREE
#undef SMS_HEAP_FREE
#endif
#define SMS_HEAP_FREE(Pointer_pp) do {SmUtil_HeapFree(*Pointer_pp, __FILE__, __LINE__); (*Pointer_pp) = NULL; } while (0)

#ifdef SMS_HEAP_FREEP
#undef SMS_HEAP_FREEP
#endif
#define SMS_HEAP_FREEP(Pointer_pp) SMS_HEAP_FREE(Pointer_pp)

#ifdef SMS_HEAP_FREE_BUF
#undef SMS_HEAP_FREE_BUF
#endif
#define SMS_HEAP_FREE_BUF(x) SmUtil_HeapFree(x, __FILE__, __LINE__)

#endif                          //SMS_HEAP_CHECK


/**
 * Used to calculate, at compile time, the number of elements in an array.
 *
 * @param [in] array   Must be an array variable.
 *
 * @sigbased No - Macro
 */
#define SMS_ELEMENTS_OF_ARRAY(array) ((sizeof (array)) / (sizeof (array)[0]))



//-------------------------------------------------------------------------------
// 8-bit pack/unpack utilities
//-------------------------------------------------------------------------------
uint8_t SmUtil_UnpackTo8Bits(
    uint8_t * Out_p,
    uint8_t * In_p,
    uint8_t NoOfPackedUnits,
    DataCoding_t PackedFormat);

uint8_t SmUtil_PackFrom8Bits(
    uint8_t * Out_p,
    uint8_t * In_p,
    uint8_t NoOfOctetsToPack,
    DataCoding_t FormatToPackTo);

//-------------------------------------------------------------------------------
// Short Message size and type utilities
//-------------------------------------------------------------------------------
uint16_t SmUtil_CalculateSizeOf_SM(
    uint8_t * Data_p);

ShortMessageType_t SmUtil_ConvertMtiToSmMessageType(
    uint8_t TpMti,
    SmMessageDirection_t Direction);

//-------------------------------------------------------------------------------
// Reading and Writing the last used Message Reference Number for MO
//-------------------------------------------------------------------------------
uint8_t SmUtil_TP_MessageReferenceReadAndIncrement(
    void);
uint8_t SmUtil_ConcatMessageReferenceReadAndIncrement(
    SMS_ConcatMessageRefNumber_t * const MSG_ConcatenatedRefNumber_p);

//-------------------------------------------------------------------------------
// IMSI GD Requester Interface
//-------------------------------------------------------------------------------
uint8_t SmUtil_Get_GD_MSG_ICCID(
    uint8_t * const MSG_ICCID_p);
uint8_t SmUtil_Put_GD_MSG_ICCID(
    uint8_t * const MSG_ICCID_p);

//-------------------------------------------------------------------------------
// MT and MO counter GD Requester Interface
//-------------------------------------------------------------------------------
uint8_t SmUtil_Get_GD_MSG_MT_Counter(
    uint16_t * const MT_Counter_p);
uint8_t SmUtil_Put_GD_MSG_MT_Counter(
    uint16_t * const MT_Counter_p);
void SmUtil_Increment_MSG_MT_Counter(
    void);

uint8_t SmUtil_Get_GD_MSG_MO_Counter(
    uint16_t * const MO_Counter_p);
uint8_t SmUtil_Put_GD_MSG_MO_Counter(
    uint16_t * const MO_Counter_p);
void SmUtil_Increment_MSG_MO_Counter(
    void);

//-------------------------------------------------------------------------------
// Flow Control Requester Interface
//-------------------------------------------------------------------------------
uint8_t SmUtil_Increment_MSG_FlowControlReferenceCounter(
    void);
uint8_t SmUtil_Decrement_MSG_FlowControlReferenceCounter(
    void);
void SmUtil_Reset_MSG_FlowControlReferenceCounter(
    void);
uint8_t SmUtil_Check_MSG_FlowControlReferenceCounter(
    void);

//-------------------------------------------------------------------------------
// Reading and Writing Memory Capacity Status
//-------------------------------------------------------------------------------
uint8_t SmUtil_Get_MSG_MemoryCapacityExceededFlag(
    uint8_t * const MemoryCapacityExceededNotificationPtr);
uint8_t SmUtil_Put_MSG_MemoryCapacityExceededFlag(
    const uint8_t MemoryCapacityExceededNotification);

void SmUtil_PrintHexBytes(
    uint8_t NumberOfHex,
    uint8_t * Hex_p,
    uint8_t Separated);

//-------------------------------------------------------------------------------
// Home PLMN
//-------------------------------------------------------------------------------
uint8_t SmUtil_Get_MSG_Home_PLMN(
    MSG_PLMN_t * HPLMN);
uint8_t SmUtil_Same_PLMN(
    const MSG_PLMN_t * PLMN1_p,
    const MSG_PLMN_t * PLMN2_p);

//-------------------------------------------------------------------------------
// Messaging USIM
//-------------------------------------------------------------------------------
uint8_t SmUtil_USIM_Initialise(
    uint8_t * eventData_p);
void SmUtil_ICCID_Get(
    char *const ICCID_p);
uint8_t SmUtil_ICCID_Changed(
    void);


#ifdef SMS_USE_MFL_UICC
SMS_UICC_ServiceHandle_t SmUtil_UICC_ServiceHandleGet(
    void);                      // Linux porting

#endif                          // SMS_USE_MFL_UICC

#ifdef SMS_USE_MFL_MODEM
//-------------------------------------------------------------------------------
// NS Error code translation
//-------------------------------------------------------------------------------
SMS_Error_t SmUtil_TranslateErrorCodes(
    const RMS_V2_ErrorCode_t RMS_V2_ErrorCode);
#endif                          // SMS_USE_MFL_MODEM

#ifndef SMS_USE_MFL_MODEM
MAL_SMS_RP_ErrorCause_t SmUtil_TranslateErrorCodesToMal(
    const SMS_RP_ErrorCause_t in);
#endif

void SmUtil_HexPrint_Raw(
    const uint8_t * data_p,
    const size_t length,
    const char *debugtag_p);

uint8_t SmUtil_UICC_ServiceEnabled(
    const sim_service_type_t ServiceType);

uint8_t SmUtil_FixedDiallingNumbersMatched(
    const ShortMessagePackedAddress_t * const SMSC_Address_p,
    const ShortMessagePackedAddress_t * const DestinationAddress_p);

#endif                          // SMUTIL_H
