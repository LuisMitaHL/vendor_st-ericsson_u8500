#ifndef INCLUSION_GUARD_T_SMS_H
#define INCLUSION_GUARD_T_SMS_H
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
 * SMS Server include file for exported interface types.
 *
 *************************************************************************/
#include <stdint.h>


/**
 * This defines the maximum number of SMS parts in a concatenated message.
 */
#define SMS_MAX_PARTS  255

/**
 * This is the Concatenated Message Reference Number contained in a
 * concatenation IEI with a TPDU. If the MTS is configured for 8 bit
 * operation, this value will be within the range 0-255.
 */
typedef uint16_t SMS_ConcatMessageRefNumber_t;


/**
 * This is the Message Reference (TP-MR) of an SMS-SUBMIT
 * or an SMS-COMMAND submitted to the SC by the MS.
 */
typedef uint8_t SMS_TP_MessageReference_t;


/**
 * MSG Error Type contains a number of different types of errors.
 * There are a number of different error classes depending on the
 * origination of the error. The first byte of the error cause
 * determines the error class according to the following list.
 * * 01  RP Layer errors
 * * 02  CP Layer errors
 * * 03  STE Internal Network Signalling errors
 * * 10  TP Failure Causes [1]
 * * 20  Messaging Transport Server internal errors
 * * 30  U8500 Modem Error Causes
 *
 * @param SMS_ERROR_RP_UNALLOCATED_NUMBER                             RP Unallocated Number
 * @param SMS_ERROR_RP_OPERATOR_DETERMINED_BARRING                    RR Operator Determined Barring
 * @param SMS_ERROR_RP_CALL_BARRED                                    RP Call Barring
 * @param SMS_ERROR_RP_RESERVED                                       RP Reserved
 * @param SMS_ERROR_RP_SMS_TRANSFER_REJECTED                          RR SMS Transfer Rejected
 * @param SMS_ERROR_RP_DESTINATION_OUT_OF_ORDER                       RP Destination Out Of Order
 * @param SMS_ERROR_RP_UNIDENTIFIED_SUBSCRIBER                        RP Unidentified Subscriber
 * @param SMS_ERROR_RP_FACILITY_REJECTED                              RP Facility Rejected
 * @param SMS_ERROR_RP_UNKNOWN_SUBSCRIBER                             RP Unknown Subscriber
 * @param SMS_ERROR_RP_NETWORK_OUT_OF_ORDER                           RP Network Out Of Order
 * @param SMS_ERROR_RP_TEMPORARY_FAILURE                              RP Temporary Failure
 * @param SMS_ERROR_RP_CONGESTION                                     RP Congestion
 * @param SMS_ERROR_RP_RESOURCES_UNAVAILABLE_UNSPECIFIED              RP Resources Unavailable Unspecified
 * @param SMS_ERROR_RP_REQUESTED_FACILITY_NOT_SUBSCRIBED              RP Requested Facility Not Subscribed
 * @param SMS_ERROR_RP_REQUESTED_FACILITY_NOT_IMPLEMENTED             RP Requested Facility Not Implemented
 * @param SMS_ERROR_RP_INVALID_SMS_TRANSFER_REFERENCE_VALUE           RP Invalid SMS Transfer Reference Value
 * @param SMS_ERROR_RP_SEMANTICALLY_INCORRECT_MESSAGE                 RP Semantically Incorrect Message
 * @param SMS_ERROR_RP_INVALID_MANDATORY_INFORMATION                  RP Invalid Mandatory Information
 * @param SMS_ERROR_RP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED   RP Message Type Non-Existent or Not Implemented
 * @param SMS_ERROR_RP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE RP Message Not Compatible With SMS Protocol State
 * @param SMS_ERROR_RP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED             RP IE Not Existent Or Not Implemented
 * @param SMS_ERROR_RP_INTERWORKING_UNSPECIFIED_PROBLEM               RP Interworking Unspecified Problem
 * @param SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED                     RP Protocol Error Unspecified
 * @param SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED                       RP Memory Capacity Exceeded
 *
 * @param SMS_ERROR_CP_NETWORK_FAILURE                                CP Network failure
 * @param SMS_ERROR_CP_CONGESTION                                     CP congestion
 * @param SMS_ERROR_CP_INVALID_TI_VALUE                               CP invalid TI value
 * @param SMS_ERROR_CP_SEMANTICALLY_INCORRECT_MESSAGE                 CP semantically incorrect message
 * @param SMS_ERROR_CP_INVALID_MANDATORY_INFORMATION                  CP invalid mandatory information
 * @param SMS_ERROR_CP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED   CP message type non-existent or not implemented
 * @param SMS_ERROR_CP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE CP message not compatible with SMS protocol state
 * @param SMS_ERROR_CP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED             CP IE non-existent or not implemented
 * @param SMS_ERROR_CP_PROTOCOL_ERROR_UNSPECIFIED                     CP protocol error unspecified

 * @param SMS_ERROR_RMS_INTERN_CONNECTION_ESTABLISHMENT_FAILED        RMS internal connection establishment failed
 * @param SMS_ERROR_RMS_INTERN_CONCATENATION_TIMER_EXPIRY             RMS internal concatenation timer expiry
 * @param SMS_ERROR_RMS_INTERN_RELAY_LAYER_TIMER_EXPIRY               RMS internal relay layer timer expiry
 * @param SMS_ERROR_RMS_INTERN_CONTROL_LAYER_TIMER_EXPIRY             RMS internal control layer timer expiry
 * @param SMS_ERROR_RMS_INTERN_CONNECTION_LOST                        RMS internal connection lost
 * @param SMS_ERROR_RMS_INTERN_TIMEOUT                                RMS internal timeout
 * @param SMS_ERROR_RMS_INTERN_RELEASE_FAILURE                        RMS internal release failure
 * @param SMS_ERROR_RMS_INTERN_ACCESS_CLASS_BARRED                    RMS internal access class barred
 * @param SMS_ERROR_RMS_INTERN_RADIO_PATH_NOT_AVAILABLE               RMS internal radio path not available
 * @param SMS_ERROR_RMS_INTERN_NO_ERROR_NO_ACK                        RMS internal no error no acknowledgment

 * @param SMS_ERROR_TP_FCS_PID_TELE_INT_NOT_SUPPORTED                 TP FCS PID tele-int not supported
 * @param SMS_ERROR_TP_FCS_PID_SM_TYPE_0_NOT_SUPPORTED                TP FCS PID SM type 0 not supported
 * @param SMS_ERROR_TP_FCS_PID_CANNOT_REPLACE_SM                      TP FCS PID cannot replace SM
 * @param SMS_ERROR_TP_FCS_PID_UNSPECIFIED_ERROR                      TP FCS PID unspecified error
 * @param SMS_ERROR_TP_FCS_DCS_CODING_NOT_SUPPORTED                   TP FCS DCS unspecified error
 * @param SMS_ERROR_TP_FCS_DCS_MESSAGE_CLASS_NOT_SUPPORTED            TP FCS DCS message class not supported
 * @param SMS_ERROR_TP_FCS_DCS_UNSPECIFIED_ERROR                      TP FCS DCS unspecified error
 * @param SMS_ERROR_TP_FCS_COMMAND_CANNOT_BE_ACTIONED                 TP FCS command cannot be acted upon
 * @param SMS_ERROR_TP_FCS_COMMAND_UNSUPPORTED                        TP FCS command unsupported
 * @param SMS_ERROR_TP_FCS_COMMAND_UNSPECIFIED_ERROR                  TP FCS command unspecified error
 * @param SMS_ERROR_TP_FCS_TPDU_NOT_SUPPORTED                         TP FCS TPDU not supported
 * @param SMS_ERROR_TP_FCS_SC_BUSY                                    TP FCS Service Center busy
 * @param SMS_ERROR_TP_FCS_NO_SC_SUBSCRIPTION                         TP FCS no Service Center subscription
 * @param SMS_ERROR_TP_FCS_SC_SYSTEM_FAILURE                          TP FCS Service Center system failure
 * @param SMS_ERROR_TP_FCS_INVALID_SME_ADDRESS                        TP FCS invalid SME address
 * @param SMS_ERROR_TP_FCS_DESTINATION_SME_BARRED                     TP FCS destination SME barred
 * @param SMS_ERROR_TP_FCS_SM_REJECTED_DUPLICATE_SM                   TP FCS SM rejected duplicate SM
 * @param SMS_ERROR_TP_FCS_TP_VPF_NOT_SUPPORTED                       TP FCS TP Validity Period Format not supported
 * @param SMS_ERROR_TP_FCS_TP_VP_NOT_SUPPORTED                        TP FCS TP Validity Period not supported
 * @param SMS_ERROR_TP_FCS_SIM_SMS_STORAGE_FULL                       TP FCS SIM SMS storage full
 * @param SMS_ERROR_TP_FCS_NO_SMS_STORAGE_CAPABILITY_IN_SIM           TP FCS no SMS storage capability in SIM
 * @param SMS_ERROR_TP_FCS_ERROR_IN_MS                                TP FCS error in MS
 * @param SMS_ERROR_TP_FCS_MEMORY_CAPACITY_EXCEEDED                   TP FCS memory capacity exceeded
 * @param SMS_ERROR_TP_FCS_SIM_APPLICATION_TOOLKIT_BUSY               TP FCS SIM application toolkit busy
 * @param SMS_ERROR_TP_FCS_SIM_DATA_DOWNLOAD_ERROR                    TP FCS SIM data download error
 * @param SMS_ERROR_TP_FCS_APPLICATION_SPECIFIC_FIRST                 TP FCS application specific first
 * @param SMS_ERROR_TP_FCS_APPLICATION_SPECIFIC_LAST                  TP FCS application specific last
 * @param SMS_ERROR_TP_FCS_UNSPECIFIED_ERROR_CAUSE                    TP FCS unspecified error cause
 *
 * @param SMS_ERROR_INTERN_NO_ERROR                                   Messaging internal no error
 * @param SMS_ERROR_INTERN_INVALID_MESSAGE_ID                         Messaging internal invalid message identifier
 * @param SMS_ERROR_INTERN_STORAGE_FULL                               Messaging internal storage full
 * @param SMS_ERROR_INTERN_MESSAGE_PARTIALLY_STORED                   Messaging internal message partially stored
 * @param SMS_ERROR_INTERN_MESSAGE_NOT_FOUND                          Messaging internal message not found
 * @param SMS_ERROR_INTERN_MESSAGE_ALREADY_IN_SIM                     Messaging internal message already in SIM
 * @param SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT                      Messaging internal invalid parameters for MT
 * @param SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MO                      Messaging internal invalid parameters for MO
 * @param SMS_ERROR_INTERN_NO_AIR_INTERFACE                           Messaging internal no air interface
 * @param SMS_ERROR_INTERN_COMMAND_NOT_UNDERSTOOD                     Messaging internal command not understood
 * @param SMS_ERROR_INTERN_UNSPECIFIED_ERROR                          Messaging internal unspecified error
 * @param SMS_ERROR_INTERN_ERROR_MESSAGE_SYSTEM_TIMEOUT               Messaging internal error message system timeout
 * @param SMS_ERROR_INTERN_CAN_NOT_CODE_MSG_AS_EMS_RELEASE4           Messaging internal cannot code message as EMS Release 4
 * @param SMS_ERROR_INTERN_CALLING_PROCESS_NOT_RECOGNISED             Messaging internal calling process not recognized
 * @param SMS_ERROR_INTERN_CLIENT_CONTROL_DATA_MISSING_OR_WRONG       Messaging internal client control data missing or wrong
 * @param SMS_ERROR_INTERN_INVALID_PID_VALUE                          Messaging internal invalid PID value
 * @param SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED                   Messaging internal memory allocation failed
 * @param SMS_ERROR_INTERN_INVALID_WAIT_MODE                          Messaging internal invalid wait mode
 * @param SMS_ERROR_INTERN_MESSAGE_NOT_USC2                           Messaging internal message not UCS2
 * @param SMS_ERROR_INTERN_FILE_SYSTEM_ACCESS_FAILED                  Messaging internal file system access failed
 * @param SMS_ERROR_INTERN_INVALID_BROWSE_OPTION                      Messaging internal invalid browse option
 * @param SMS_ERROR_INTERN_INVALID_MESSAGE_TYPE                       Messaging internal invalid message type
 * @param SMS_ERROR_INTERN_NO_STATUSREPORT_FOR_SMS_PART               Messaging internal no Status Report for SMS part
 * @param SMS_ERROR_INTERN_NO_SMS_FOR_PART_NUMBER                     Messaging internal no SMS for part number
 * @param SMS_ERROR_INTERN_SERVER_BUSY                                Messaging internal MTS server busy
 * @param SMS_ERROR_INTERN_INVALID_VOLUME                             Messaging internal FileSystem invalid volume
 * @param SMS_ERROR_INTERN_APP_PORT_IN_USE                            Messaging internal application port in use
 * @param SMS_ERROR_INTERN_OPERATION_NOT_SUPPORTED_BY_STORAGE         Messaging internal operation not supported by storage
 * @param SMS_ERROR_INTERN_INVALID_SIM_SLOT_NUMBER                    Messaging internal invalid SIM slot number
 * @param SMS_ERROR_INTERN_FIXED_DIALLING_NUMBER_RESTRICTED           Messaging internal Fixed Dialling Number check failed
 * @param SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED            Messaging internal MO SMS rejected by USIM
 * @param SMS_ERROR_INTERN_NO_ERROR_NO_ACTION                         Messaging internal No error since no action was taken.
 */
typedef enum {
    SMS_ERROR_RP_UNALLOCATED_NUMBER = 0x0101,
    SMS_ERROR_RP_OPERATOR_DETERMINED_BARRING = 0x0108,
    SMS_ERROR_RP_CALL_BARRED = 0x010a,
    SMS_ERROR_RP_RESERVED = 0x010b,
    SMS_ERROR_RP_SMS_TRANSFER_REJECTED = 0x0115,
    SMS_ERROR_RP_DESTINATION_OUT_OF_ORDER = 0x011b,
    SMS_ERROR_RP_UNIDENTIFIED_SUBSCRIBER = 0x011c,
    SMS_ERROR_RP_FACILITY_REJECTED = 0x011d,
    SMS_ERROR_RP_UNKNOWN_SUBSCRIBER = 0x011e,
    SMS_ERROR_RP_NETWORK_OUT_OF_ORDER = 0x0126,
    SMS_ERROR_RP_TEMPORARY_FAILURE = 0x0129,
    SMS_ERROR_RP_CONGESTION = 0x012a,
    SMS_ERROR_RP_RESOURCES_UNAVAILABLE_UNSPECIFIED = 0x012f,
    SMS_ERROR_RP_REQUESTED_FACILITY_NOT_SUBSCRIBED = 0x0132,
    SMS_ERROR_RP_REQUESTED_FACILITY_NOT_IMPLEMENTED = 0x0145,
    SMS_ERROR_RP_INVALID_SMS_TRANSFER_REFERENCE_VALUE = 0x0151,
    SMS_ERROR_RP_SEMANTICALLY_INCORRECT_MESSAGE = 0x015f,
    SMS_ERROR_RP_INVALID_MANDATORY_INFORMATION = 0x0160,
    SMS_ERROR_RP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 0x0161,
    SMS_ERROR_RP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE = 0x0162,
    SMS_ERROR_RP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED = 0x0163,
    SMS_ERROR_RP_INTERWORKING_UNSPECIFIED_PROBLEM = 0x017f,
    SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED = 0x016f,
    SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED = 0x0116,

    SMS_ERROR_CP_NETWORK_FAILURE = 0x0211,
    SMS_ERROR_CP_CONGESTION = 0x0216,
    SMS_ERROR_CP_INVALID_TI_VALUE = 0x0251,
    SMS_ERROR_CP_SEMANTICALLY_INCORRECT_MESSAGE = 0x025f,
    SMS_ERROR_CP_INVALID_MANDATORY_INFORMATION = 0x0260,
    SMS_ERROR_CP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 0x0261,
    SMS_ERROR_CP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE = 0x0262,
    SMS_ERROR_CP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED = 0x0263,
    SMS_ERROR_CP_PROTOCOL_ERROR_UNSPECIFIED = 0x026f,

    SMS_ERROR_RMS_INTERN_CONNECTION_ESTABLISHMENT_FAILED = 0x0301,
    SMS_ERROR_RMS_INTERN_CONCATENATION_TIMER_EXPIRY = 0x0302,
    SMS_ERROR_RMS_INTERN_RELAY_LAYER_TIMER_EXPIRY = 0x0303,
    SMS_ERROR_RMS_INTERN_CONTROL_LAYER_TIMER_EXPIRY = 0x0304,
    SMS_ERROR_RMS_INTERN_CONNECTION_LOST = 0x0305,
    SMS_ERROR_RMS_INTERN_TIMEOUT = 0x0306,
    SMS_ERROR_RMS_INTERN_RELEASE_FAILURE = 0x0307,
    SMS_ERROR_RMS_INTERN_ACCESS_CLASS_BARRED = 0x0308,
    SMS_ERROR_RMS_INTERN_RADIO_PATH_NOT_AVAILABLE = 0x0396,
    SMS_ERROR_RMS_INTERN_NO_ERROR_NO_ACK = 0x03FE,

    SMS_ERROR_TP_FCS_PID_TELE_INT_NOT_SUPPORTED = 0x1080,
    SMS_ERROR_TP_FCS_PID_SM_TYPE_0_NOT_SUPPORTED = 0x1081,
    SMS_ERROR_TP_FCS_PID_CANNOT_REPLACE_SM = 0x1082,
    SMS_ERROR_TP_FCS_PID_UNSPECIFIED_ERROR = 0x108F,
    SMS_ERROR_TP_FCS_DCS_CODING_NOT_SUPPORTED = 0x1090,
    SMS_ERROR_TP_FCS_DCS_MESSAGE_CLASS_NOT_SUPPORTED = 0x1091,
    SMS_ERROR_TP_FCS_DCS_UNSPECIFIED_ERROR = 0x109F,
    SMS_ERROR_TP_FCS_COMMAND_CANNOT_BE_ACTIONED = 0x10A0,
    SMS_ERROR_TP_FCS_COMMAND_UNSUPPORTED = 0x10A1,
    SMS_ERROR_TP_FCS_COMMAND_UNSPECIFIED_ERROR = 0x10AF,
    SMS_ERROR_TP_FCS_TPDU_NOT_SUPPORTED = 0x10B0,
    SMS_ERROR_TP_FCS_SC_BUSY = 0x10C0,
    SMS_ERROR_TP_FCS_NO_SC_SUBSCRIPTION = 0x10C1,
    SMS_ERROR_TP_FCS_SC_SYSTEM_FAILURE = 0x10C2,
    SMS_ERROR_TP_FCS_INVALID_SME_ADDRESS = 0x10C3,
    SMS_ERROR_TP_FCS_DESTINATION_SME_BARRED = 0x10C4,
    SMS_ERROR_TP_FCS_SM_REJECTED_DUPLICATE_SM = 0x10C5,
    SMS_ERROR_TP_FCS_TP_VPF_NOT_SUPPORTED = 0x10C6,
    SMS_ERROR_TP_FCS_TP_VP_NOT_SUPPORTED = 0x10C7,
    SMS_ERROR_TP_FCS_SIM_SMS_STORAGE_FULL = 0x10D0,
    SMS_ERROR_TP_FCS_NO_SMS_STORAGE_CAPABILITY_IN_SIM = 0x10D1,
    SMS_ERROR_TP_FCS_ERROR_IN_MS = 0x10D2,
    SMS_ERROR_TP_FCS_MEMORY_CAPACITY_EXCEEDED = 0x10D3,
    SMS_ERROR_TP_FCS_SIM_APPLICATION_TOOLKIT_BUSY = 0x10D4,
    SMS_ERROR_TP_FCS_SIM_DATA_DOWNLOAD_ERROR = 0x10D5,
    SMS_ERROR_TP_FCS_APPLICATION_SPECIFIC_FIRST = 0x10E0,
    SMS_ERROR_TP_FCS_APPLICATION_SPECIFIC_LAST = 0x10FE,
    SMS_ERROR_TP_FCS_UNSPECIFIED_ERROR_CAUSE = 0x10FF,

    SMS_ERROR_INTERN_NO_ERROR = 0x2000,
    SMS_ERROR_INTERN_INVALID_MESSAGE_ID = 0x2001,
    SMS_ERROR_INTERN_STORAGE_FULL = 0x2002,
    SMS_ERROR_INTERN_MESSAGE_PARTIALLY_STORED = 0x2003,
    SMS_ERROR_INTERN_MESSAGE_NOT_FOUND = 0x2004,
    SMS_ERROR_INTERN_MESSAGE_ALREADY_IN_SIM = 0x2005,
    SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT = 0x2006,
    SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MO = 0x2007,
    SMS_ERROR_INTERN_NO_AIR_INTERFACE = 0x2008,
    SMS_ERROR_INTERN_COMMAND_NOT_UNDERSTOOD = 0x2009,
    SMS_ERROR_INTERN_UNSPECIFIED_ERROR = 0x2010,
    SMS_ERROR_INTERN_ERROR_MESSAGE_SYSTEM_TIMEOUT = 0x2011,
    SMS_ERROR_INTERN_CAN_NOT_CODE_MSG_AS_EMS_RELEASE4 = 0x2012,
    SMS_ERROR_INTERN_CALLING_PROCESS_NOT_RECOGNISED = 0x2013,
    SMS_ERROR_INTERN_CLIENT_CONTROL_DATA_MISSING_OR_WRONG = 0x2014,
    SMS_ERROR_INTERN_INVALID_PID_VALUE = 0x2015,
    SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED = 0x2016,
    SMS_ERROR_INTERN_INVALID_WAIT_MODE = 0x2017,
    SMS_ERROR_INTERN_MESSAGE_NOT_USC2 = 0x2018,
    SMS_ERROR_INTERN_FILE_SYSTEM_ACCESS_FAILED = 0x2019,
    SMS_ERROR_INTERN_INVALID_BROWSE_OPTION = 0x2020,
    SMS_ERROR_INTERN_INVALID_MESSAGE_TYPE = 0x2021,
    SMS_ERROR_INTERN_NO_STATUSREPORT_FOR_SMS_PART = 0x2022,
    SMS_ERROR_INTERN_NO_SMS_FOR_PART_NUMBER = 0x2023,
    SMS_ERROR_INTERN_SERVER_BUSY = 0x2024,
    SMS_ERROR_INTERN_INVALID_VOLUME = 0x2025,
    SMS_ERROR_INTERN_APP_PORT_IN_USE = 0x2026,
    SMS_ERROR_INTERN_OPERATION_NOT_SUPPORTED_BY_STORAGE = 0x2027,
    SMS_ERROR_INTERN_INVALID_SIM_SLOT_NUMBER = 0x2028,
    SMS_ERROR_INTERN_FIXED_DIALLING_NUMBER_RESTRICTED = 0x2029,
    SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED = 0x2030,
    SMS_ERROR_INTERN_NO_ERROR_NO_ACTION = 0x2031,

    /* SMS_CAUSE   */
    SMS_ERR_OK = 0x3000,        /* A and B */
    SMS_ERR_ROUTING_RELEASED = 0x3001,  /* A and B */
    SMS_ERR_INVALID_PARAMETER = 0x3002, /* A and B */
    SMS_ERR_DEVICE_FAILURE = 0x3003,    /* A and B */
    SMS_ERR_PP_RESERVED = 0x3004,       /* A and B */
    SMS_ERR_ROUTE_NOT_AVAILABLE = 0x3005,       /* A and B */
    SMS_ERR_ROUTE_NOT_ALLOWED = 0x3006, /* A and B */
    SMS_ERR_SERVICE_RESERVED = 0x3007,  /* A */
    SMS_ERR_INVALID_LOCATION = 0x3008,  /* A */
    SMS_ERR_NO_NETW_RESPONSE = 0x300B,  /* B */
    SMS_ERR_DEST_ADDR_FDN_RESTRICTED = 0x300C,  /* B */
    SMS_ERR_SMSC_ADDR_FDN_RESTRICTED = 0x300D,  /* B */
    SMS_ERR_RESEND_ALREADY_DONE = 0x300E,       /* A */
    SMS_ERR_SMSC_ADDR_NOT_AVAILABLE = 0x300F,   /* B */
    SMS_ERR_ROUTING_FAILED = 0x3010,    /* A */
    SMS_ERR_CS_INACTIVE = 0x3011,       /* A and B */
    SMS_ERR_SAT_MO_CONTROL_MODIFIED = 0x3012,   /* B */
    SMS_ERR_SAT_MO_CONTROL_REJECT = 0x3013,     /* B */
    SMS_ERR_TRACFONE_FAILED = 0x3014,   /* B */
    SMS_ERR_SENDING_ONGOING = 0x3015,   /* A and B */
    SMS_ERR_SERVER_NOT_READY = 0x3016,  /* A and B */
    SMS_ERR_NO_TRANSACTION = 0x3017,    /* A and B */
    SMS_ERR_REJECTED = 0x3018,  /* B */
    SMS_ERR_INVALID_SUBSCRIPTION_NR = 0x3019,   /* A */
    SMS_ERR_RECEPTION_FAILED = 0x301A,  /* B */
    SMS_ERR_RC_REJECTED = 0x301B,       /* B */
    SMS_ERR_ALL_SUBSCRIPTIONS_ALLOCATED = 0x301C,       /* A */
    SMS_ERR_SUBJECT_COUNT_OVERFLOW = 0x301D,    /* A */
    SMS_ERR_DCS_COUNT_OVERFLOW = 0x301E,        /* A */

    /* SMS_EXT_CAUSE */
    SMS_EXT_ERR_UNASSIGNED_NUMBER = 0x4001,     /* Direction: MO */
    SMS_EXT_ERR_OPER_DETERMINED_BARR = 0x4008,  /* Direction: MO */
    SMS_EXT_ERR_CALL_BARRED = 0x400A,   /* Direction: MO */
    SMS_EXT_ERR_RESERVED = 0x400B,      /* Direction: MO */
    SMS_EXT_ERR_MSG_TRANSFER_REJ = 0x4015,      /* Direction: MO */
    SMS_EXT_ERR_MEMORY_CAPACITY_EXC = 0x4016,   /* Direction: MT */
    SMS_EXT_ERR_DEST_OUT_OF_ORDER = 0x401B,     /* Direction: MO */
    SMS_EXT_ERR_UNDEFINED_SUBSCRIBER = 0x401C,  /* Direction: MO */
    SMS_EXT_ERR_FACILITY_REJECTED = 0x401D,     /* Direction: MO */
    SMS_EXT_ERR_UNKNOWN_SUBSCRIBER = 0x401E,    /* Direction: MO */
    SMS_EXT_ERR_NETW_OUT_OF_ORDER = 0x4026,     /* Direction: MO */
    SMS_EXT_ERR_TEMPORARY_FAILURE = 0x4029,     /* Direction: MO */
    SMS_EXT_ERR_CONGESTION = 0x402A,    /* Direction: MO */
    SMS_EXT_ERR_RESOURCE_UNAVAILABLE = 0x402F,  /* Direction: MO */
    SMS_EXT_ERR_REQ_FACILITY_NOT_SUB = 0x4032,  /* Direction: MO */
    SMS_EXT_ERR_REQ_FACILITY_NOT_IMP = 0x4045,  /* Direction: MO */
    SMS_EXT_ERR_INVALID_REFERENCE = 0x4051,     /* Direction: MO & MT */
    SMS_EXT_ERR_INCORRECT_MESSAGE = 0x405F,     /* Direction: MO & MT */
    SMS_EXT_ERR_INVALID_MAND_INFO = 0x4060,     /* Direction: MO & MT */
    SMS_EXT_ERR_INVALID_MSG_TYPE = 0x4061,      /* Direction: MO & MT */
    SMS_EXT_ERR_MSG_NOT_COMP_WITH_ST = 0x4062,  /* Direction: MO & MT */
    SMS_EXT_ERR_INVALID_INFO_ELEMENT = 0x4063,  /* Direction: MO & MT */
    SMS_EXT_ERR_PROTOCOL_ERROR = 0x406F,        /* Direction: MO & MT */
    SMS_EXT_ERR_INTERWORKING = 0x407F,  /* Direction: MO */
    SMS_EXT_ERR_NO_CAUSE = 0x4080,      /* Direction: MO */
    SMS_EXT_ERR_IMSI_UNKNOWN_HLR = 0x4082,      /* Direction: MO */
    SMS_EXT_ERR_ILLEGAL_MS = 0x4083,    /* Direction: MO */
    SMS_EXT_ERR_IMSI_UNKNOWN_VLR = 0x4084,      /* Direction: MO */
    SMS_EXT_ERR_IMEI_NOT_ACCEPTED = 0x4085,     /* Direction: MO */
    SMS_EXT_ERR_ILLEGAL_ME = 0x4086,    /* Direction: MO */
    SMS_EXT_ERR_PLMN_NOT_ALLOWED = 0x408B,      /* Direction: MO */
    SMS_EXT_ERR_LA_NOT_ALLOWED = 0x408C,        /* Direction: MO */
    SMS_EXT_ERR_ROAM_NOT_ALLOWED_LA = 0x408D,   /* Direction: MO */
    SMS_EXT_ERR_NO_SUITABLE_CELLS_LA = 0x408F,  /* Direction: MO */
    SMS_EXT_ERR_NETWORK_FAILURE = 0x4091,       /* Direction: MO */
    SMS_EXT_ERR_MAC_FAILURE = 0x4094,   /* Direction: MO */
    SMS_EXT_ERR_SYNC_FAILURE = 0x4095,  /* Direction: MO */
    SMS_EXT_ERR_LOW_LAYER_CONGESTION = 0x4096,  /* Direction: MO */
    SMS_EXT_ERR_AUTH_UNACCEPTABLE = 0x4097,     /* Direction: MO */
    SMS_EXT_ERR_SERV_OPT_NOT_SUPPORTED = 0x40A0,        /* Direction: MO */
    SMS_EXT_ERR_SERV_OPT_NOT_SUBSCRIBED = 0x40A1,       /* Direction: MO */
    SMS_EXT_ERR_SERV_OPT_TEMP_OUT_OF_ORDER = 0x40A2,    /* Direction: MO */
    SMS_EXT_ERR_CALL_CANNOT_BE_IDENTIFIED = 0x40A6,     /* Direction: MO */
    SMS_EXT_ERR_SEMANTICALLY_INCORR_MSG = 0x40DF,       /* Direction: MO */
    SMS_EXT_ERR_LOW_LAYER_INVALID_MAND_INFO = 0x40E0,   /* Direction: MO */
    SMS_EXT_ERR_LOW_LAYER_INVALID_MSG_TYPE = 0x40E1,    /* Direction: MO */
    SMS_EXT_ERR_LOW_LAYER_MSG_TYPE_NOT_COMP_WITH_ST = 0x40E2,   /* Direction: MO */
    SMS_EXT_ERR_LOW_LAYER_INVALID_INFO_ELEMENT = 0x40E3,        /* Direction: MO */
    SMS_EXT_ERR_CONDITIONAL_IE_ERROR = 0x40E4,  /* Direction: MO */
    SMS_EXT_ERR_LOW_LAYER_MSG_NOT_COMP_WITH_ST = 0x40E5,        /* Direction: MO */
    SMS_EXT_ERR_CS_BARRED = 0x40E8,     /* Direction: MO */
    SMS_EXT_ERR_LOW_LAYER_PROTOCOL_ERROR = 0x40EF       /* Direction: MO */
} SMS_Error_t;


/**
 * SMS Status Type describes different types of SMS, e.g. sent,
 * unsent, read, unread. It is also used when searching for SMS
 * of a certain type, or deleting SMS of a certain type.
 *
 * @param SMS_STATUS_READ          SMS read
 * @param SMS_STATUS_UNREAD        SMS unread
 * @param SMS_STATUS_SENT          SMS sent
 * @param SMS_STATUS_UNSENT        SMS unsent
 * @param SMS_STATUS_UNKNOWN       SMS status unknown
 * @param SMS_STATUS_ANY_STATUS    SMS any status can only be searched, for never set
 */
typedef enum {
    SMS_STATUS_READ = 0x01,
    SMS_STATUS_UNREAD = 0x03,
    SMS_STATUS_SENT = 0x05,
    SMS_STATUS_UNSENT = 0x07,
    SMS_STATUS_UNKNOWN = 0x00,
    SMS_STATUS_ANY_STATUS = 0x99
} SMS_Status_t;

/**
 * Browse Options describes search rules for asking the system to
 * find a certain message.
 *
 * @param SMS_BROWSE_OPTION_FIRST     Message first
 * @param SMS_BROWSE_OPTION_NEXT      Message next
 * @param SMS_BROWSE_OPTION_PREVIOUS  Message previous
 * @param SMS_BROWSE_OPTION_LAST      Message last
 */
typedef enum {
    SMS_BROWSE_OPTION_FIRST,
    SMS_BROWSE_OPTION_NEXT,
    SMS_BROWSE_OPTION_PREVIOUS,
    SMS_BROWSE_OPTION_LAST
} SMS_BrowseOption_t;

/**
 * Defines the size of the date and time structure.
 */
#define SMS_SIZEOF_DATE_TIME 7

/**
 * SMS_DateAndTime_t contains the time stamp of the message TP-SCTS
 * [1] (sec 9.2.3.11).
 */
typedef uint8_t SMS_DateAndTime_t;


/**
 * Defines the maximum number of octets in a packed format subscriber number.
 */
#define SMS_MAX_PACKED_ADDRESS_LENGTH 10

/**
 * Defines the maximum number of digits in a subscriber number.
 */
#define SMS_MAX_DIGITS_IN_NUMBER 20

/**
 * SMS_TypeOfNumber_t defines the type of subscriber number as defined
 * in the standards.
 */
typedef uint8_t SMS_TypeOfNumber_t;

/**
 * SMS_NumberingPlanId_t defines the numbering plan identification as
 * defined in the standards.
 */
typedef uint8_t SMS_NumberingPlanId_t;

/**
 * The subscriber number represented in packed address format.
 *
 * @param Length          The number of bytes in the AddressValue.
 * @param TypeOfAddress   Type of address as defined in the standards.
 * @param AddressValue    The address value in packed format.
 */
typedef struct {
    uint8_t Length;
    uint8_t TypeOfAddress;
    uint8_t AddressValue[SMS_MAX_PACKED_ADDRESS_LENGTH];
} SMS_PackedAddress_t;

/*
 * The subscriber number is coded as ASCII numbers, and NULL
 * terminated. TBD: UTF guidelines, pause, *, #, and + ????
 */

/**
 * The subscriber number is coded as ASCII numbers, and NULL terminated.
 *
 * @param Digits          The subscriber number coded as ASCII numbers, and
 *                        NULL terminated.
 * @param TypeOfNumber    Type of the subscriber number as defined in the
 *                        standards.
 * @param NumberingPlanId Numbering plan identification as defined in the
 *                        standards.
 */
typedef struct {
    uint8_t Digits[SMS_MAX_DIGITS_IN_NUMBER + 1];
    SMS_TypeOfNumber_t TypeOfNumber;
    SMS_NumberingPlanId_t NumberingPlanId;
} SMS_SubscriberNumber_t;


/**
 * SMS_ConcatParameters_t is the concatenated parameter information of an SMS.
 *
 * @param ConcatMessageRefNumber The concatenated message reference number.
 * @param MaxNumberOfConcatMsgs  The number of parts of this concatenated message.
 * @param MsgSequenceNumber      The message sequence number.
 * @param IsConcatenated         Whether or not the SMS is concatenated.
*/
typedef struct {
    SMS_ConcatMessageRefNumber_t ConcatMessageRefNumber;
    uint8_t MaxNumberOfConcatMsgs;
    uint8_t MsgSequenceNumber;
    uint8_t IsConcatenated;
} SMS_ConcatParameters_t;

/**
 * SMS_ReadInformation_t is the header and concatenated information of an SMS.
 *
 * @param Status            The type of SMS that the user wants to search for.
 *                          E.g. SMS_STATUS_READ.
 * @param Address           The Originating, or Destination Address
 *                          depending on if the message is MO or MT.
 * @param SMSC_Address      The Service Center Address.
 * @param DateAndTime       The Time Stamp of the message being read (only
 *                          on MT messages).
 * @param ConcatParameters  The SMS concatenated parameter information.
*/
typedef struct {
    SMS_Status_t Status;
    SMS_SubscriberNumber_t Address;
    SMS_SubscriberNumber_t SMSC_Address;
    SMS_DateAndTime_t DateAndTime[SMS_SIZEOF_DATE_TIME];
    SMS_ConcatParameters_t ConcatParameters;
} SMS_ReadInformation_t;


/**
 * SMS_Category_t is used to indicate what type of message that has been
 * received. SMS_CATEGORY_WAITING_IN_SERVICE_CENTER is used to indicate that the storage
 * is full and a message is waiting in the service center to be delivered.
 *
 * @param SMS_CATEGORY_CLASS_ZERO                 Message Class0
 * @param SMS_CATEGORY_STANDARD                   Message standard
 * @param SMS_CATEGORY_SIM_SPECIFIC               Message SIM specific
 * @param SMS_CATEGORY_SIM_SPECIFIC_TO_BE_ERASED  Message SIM specific to be erased
 * @param SMS_CATEGORY_TE_SPECIFIC                Message TE specific
 * @param SMS_CATEGORY_ANY_CATEGORY               Message any category
 * @param SMS_CATEGORY_ME_SPECIFIC                Message ME specific
 * @param SMS_CATEGORY_WAITING_IN_SERVICE_CENTER  Message waiting in Service Center
 */
typedef enum {
    SMS_CATEGORY_CLASS_ZERO,
    SMS_CATEGORY_STANDARD,
    SMS_CATEGORY_SIM_SPECIFIC,
    SMS_CATEGORY_SIM_SPECIFIC_TO_BE_ERASED,
    SMS_CATEGORY_TE_SPECIFIC,
    SMS_CATEGORY_ANY_CATEGORY,
    SMS_CATEGORY_ME_SPECIFIC,
    SMS_CATEGORY_WAITING_IN_SERVICE_CENTER
} SMS_Category_t;

/**
 * SMS_StorageStatus_t is sent when either of the storage media is filled, or
 * something is deleted to make it not full again.
 *
 * @param StorageFullSIM Is used for SIM storage.
 * @param StorageFullME  Is used for internal persistent data storage.
 */
typedef struct {
    uint8_t StorageFullSIM;
    uint8_t StorageFullME;
} SMS_StorageStatus_t;


/**
 * SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH is the maximum length of TPDU's allowed in the Messaging
 * Transport Server module.
 */
#define SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH 164

/**
 * SMS_TPDU_t contains the actual TPDU information. This information
 * depends on which TPDU type that is used (e.g. SMS-SUBMIT, SMS-DELIVER).
 *
 * @param Length Indicates the length of the TPDU data.
 * @param Data   The array that holds the TPDU data. Only the length
 *               indicated in the Length is relevant. The
 *               data of the TPDU must be in the format specified in
 *               ref 1.
 */
typedef struct {
    uint8_t Length;
    uint8_t Data[SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH];
} SMS_TPDU_t;


/**
 * SMS_SMSCAddress_TPDU_t is used to encapsulate the Service Center packed address and SMS_TPDU_t in
 * one type.
 *
 * @param ServiceCenterAddress The packed address of the Service Center of the Short Message.
 * @param TPDU                 The Short Message TPDU.
 */
typedef struct {
    SMS_PackedAddress_t ServiceCenterAddress;
    SMS_TPDU_t TPDU;
} SMS_SMSC_Address_TPDU_t;


/**
 * SMS_STATUS_REPORT_TPDU_MAX_LENGTH is the length of the part of the Status Report
 * that can be stored in the SIM card [2].
 */
#define SMS_STATUS_REPORT_TPDU_MAX_LENGTH 29

/**
 * SMS_StatusReport_t is the format for the status report
 * stored on the SIM card, or in the phone (when SIM is full, or for
 * phone stored messages).
 *
 * @param StatusReport The data of the Status Report, which is always
 *                     29 bytes long as specified in the SIM Spec [2].
 */
typedef struct {
    uint8_t StatusReport[SMS_STATUS_REPORT_TPDU_MAX_LENGTH];
} SMS_StatusReport_t;



/**
 * SMS_Storage_t is an enumeration of the storage locations used by AT
 * commands.
 *
 * @param SMS_STORAGE_ME  ME (EEPROM/FileSystem) storage.
 * @param SMS_STORAGE_SM  (U)SIM storage.
 * @param SMS_STORAGE_MT  Any storage.
 */
typedef enum {
    SMS_STORAGE_ME = 0,
    SMS_STORAGE_SM = 1,
    SMS_STORAGE_MT = 2
} SMS_Storage_t;

/**
 * SMS_NetworkAcknowledge_t define whether the client or the platform is
 * responsible for acknowledgement.
 *
 * @param SMS_NETWORK_ACKNOWLEDGE_NORMAL The platform will acknowledge received
 *                                       MT SMS that are indicated to SMS
 *                                       clients.
 * @param SMS_NETWORK_ACKNOWLEDGE_CLIENT An SMS Server client must acknowledge
 *                                       received MT SMS.
 */
typedef enum {
    SMS_NETWORK_ACKNOWLEDGE_NORMAL,
    SMS_NETWORK_ACKNOWLEDGE_CLIENT
} SMS_NetworkAcknowledge_t;

/**
 * SMS_MO_Route_t route information.
 *
 * @param SMS_MO_ROUTE_PS    PS route used
 * @param SMS_MO_ROUTE_CS    CS route used
 * @param SMS_MO_ROUTE_CS_PS CS route used preferred, use PS if not available
 */
typedef enum {
    SMS_MO_ROUTE_PS = 0,
    SMS_MO_ROUTE_CS = 1,
    SMS_MO_ROUTE_CS_PS = 3,
} SMS_MO_Route_t;

/**
 * This defines the SMS storage invalid slot number.
 */
#define SMS_STORAGE_POSITION_INVALID  0

/**
 * SMS_Storage_t is the slot position within the storage type
 */
typedef uint32_t SMS_Position_t;

/**
 * SMS_Slot_t is used with AT commands to define the position of a short
 * message (storage,type.position)
 *
 * @param Storage   Slot storage.
 * @param Position  Slot position.
 */
typedef struct {
    SMS_Storage_t Storage;
    SMS_Position_t Position;
} SMS_Slot_t;

/**
 * SMS_RelayControl_t is an enumeration that defines the control of the continuity
 * of the SMS relay protocol link used by AT commands.
 *
 * @param SMS_RELAY_CONTROL_DISABLED         SMS relay control disabled.
 * @param SMS_RELAY_CONTROL_TIMEOUT_ENABLED  SMS relay control enabled until relay
 *                                               protocol link timeout then disabled.
 * @param SMS_RELAY_CONTROL_ENABLED          SMS relay control enabled.
 */
typedef enum {
    SMS_RELAY_CONTROL_DISABLED = 0,
    SMS_RELAY_CONTROL_TIMEOUT_ENABLED = 1,
    SMS_RELAY_CONTROL_ENABLED = 2
} SMS_RelayControl_t;

/**
 * SMS_SearchInfo_t is used when searching for SMS in the system
 *
 * @param Status        The type of SMS that the user wants to search for.
 *                      Eg. SMS_STATUS_READ.
 * @param Slot          The slot of the previously found SMS. For first
 *                      search use 0 as position.
 * @param BrowseOption  Defines in what way the system should search for an SMS.
 */
typedef struct {
    SMS_Status_t Status;
    SMS_Slot_t Slot;
    SMS_BrowseOption_t BrowseOption;
} SMS_SearchInfo_t;


/**
 * SMS_ShortMessageReceived_t is used to indicate to clients that a new SMS
 * has been received.
 *
 * @param Category            Indicates the SMS category.
 * @param IsReplaceType       Indicates if the SMS is a Replace Type
 *                            a previously stored SMS.
 * @param SMSC_Address_TPDU   Indicates the SMSC and SMS TPDU.
 * @param Slot                Indicates the storage information.
 */
typedef struct {
    SMS_Category_t Category;
    uint8_t IsReplaceType;
    SMS_SMSC_Address_TPDU_t SMSC_Address_TPDU;
    SMS_Slot_t Slot;
} SMS_ShortMessageReceived_t;

/**
 * SMS_ApplicationPort_t specifies the type of application port
 *
 * @param SMS_APPLICATION_PORT_ORIGINATION  Origination application port.
 * @param SMS_APPLICATION_PORT_DESTINATION  Destination application port.
 */
typedef enum {
    SMS_APPLICATION_PORT_ORIGINATION,
    SMS_APPLICATION_PORT_DESTINATION
} SMS_ApplicationPort_t;

/**
 * SMS_ApplicationPortRange_t specifies an application port type and range
 * that may be used to subscribe to matching received MT SMS
 *
 * A matching port must be greater than or equal to the LowerPort, and
 * less than or equal to the UpperPort. If a single application port is
 * required, then LowerPort and UpperPort may be set to the same value.
 *
 * @param ApplicationPort  Defines the application port type.
 * @param LowerPort        An application port range 16 bit lower bound value.
 * @param UpperPort        An application port range 16 bit upper bound value.
 */
typedef struct {
    SMS_ApplicationPort_t ApplicationPort;
    uint16_t LowerPort;
    uint16_t UpperPort;
} SMS_ApplicationPortRange_t;

/**
 * SMS_SlotInformation_t is used to indicate the status of the SMS slots
 *
 * @param ReadCount   Indicates the number of read SMS.
 * @param UnreadCount Indicates the number of unread SMS.
 * @param SentCount   Indicates the number of sent SMS.
 * @param UnsentCount Indicates the number of unsent SMS.
 * @param FreeCount   Indicates the number of messages that could be stored.
 */
typedef struct {
    uint16_t ReadCount;
    uint16_t UnreadCount;
    uint16_t SentCount;
    uint16_t UnsentCount;
    uint16_t FreeCount;
} SMS_SlotInformation_t;


/**
 * SMS_StorageCapacity_t describes the volume capacity information.
 *
 * @param FreeSpace Indicates the number of additional SMS that can
 *                  be stored on that volume. For volumes other than SIM this
 *                  parameter will be decreased appropriately if the
 *                  application shares the volume as the storage for other
 *                  types of data.
 * @param UsedSpace Indicates the number of SMS that are currently
 *                  stored on that volume.
 */
typedef struct {
    uint16_t FreeSpace;
    uint16_t UsedSpace;
} SMS_StorageCapacity_t;


/**
 * SMS_ServerStatus_t indicates the status of the message transport server
 *
 * @param SMS_SERVER_STATUS_MTS_BUSY   MTS busy.
 * @param SMS_SERVER_STATUS_MTS_READY  MTS ready.
 */
typedef enum {
    SMS_SERVER_STATUS_MTS_BUSY = 0,     // Busy processing or initialising
    SMS_SERVER_STATUS_MTS_READY = 1     // Ready to service user requests
} SMS_ServerStatus_t;

/**
 * SMS_RP_ErrorCause_t defines the RP Error Cause to be sent with a Deliver
 * Report
 */
typedef uint16_t SMS_RP_ErrorCause_t;

/**
 * Client Tag is used in all Global Service request functions and their
 * associated request and response signals. It is also used in the Event
 * Channel event signals. In the latter case, the Client Tag information is
 * added to the signal by the dispatcher process. The client tag is used to
 * uniquely identify a service request within a client process. Note that it
 * can not be used in a server in anyway (for example to identify a session
 * with a client).
 */
typedef uint32_t SMS_ClientTag_t;

typedef uint32_t SMS_SIGSELECT;

/**
 * Used in the signals sent from within a Global Service request function
 * towards the serving process and in the response from the serving process to
 * the client. It is also used when sending an Event Channel signal to event
 * subscribers.
 *
 * @param Primitive The signal identity.
 * @param ClientTag The Client tag value.
 */
typedef struct {
    SMS_SIGSELECT Primitive;
    SMS_ClientTag_t ClientTag;
} SMS_SigselectWithClientTag_t;

/**
 * The union of all signals.
 *
 * @param Primitive -
 * @param sig_no -
 * @param signo -
 *
 */
union SMS_SIGNAL {
    SMS_SIGSELECT Primitive;
    SMS_SIGSELECT sig_no;
    SMS_SIGSELECT signo;
};

#endif                          //INCLUSION_GUARD_T_SMS_H
