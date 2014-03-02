/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef INCLUSION_GUARD_T_RMS_H
#define INCLUSION_GUARD_T_RMS_H
/*************************************************************************
* $Copyright Ericsson AB 2009 $
**************************************************************************
*
* DESCRIPTION:
* ... 
* ...
*
*************************************************************************/

#include "t_basicdefinitions.h"
#include "t_catdefinitions.h"

/** Defines the category ID for RMS */
#define CAT_ID_RMS 148
CAT_INTERVAL_DEFINITION(CAT_ID_RMS)

/****************************************************************/
/*                                                              */
/*                 Defines                                      */
/*                                                              */
/****************************************************************/
/** Maximum Short Message (SM) address length */
#define RMS_V2_MAX_SM_ADDRESS_LENGTH       10
/** Maximum RP User data length */
#define RMS_V2_MAX_RP_USER_DATA_LENGTH    234
/**Relay layer (RP) error category*/
#define RMS_V2_RP_ERROR_CATEGORY         0x01
/**Short Message Control layer (SMC) error category*/
#define RMS_V2_CP_ERROR_CATEGORY         0x02
/**Internal RMS error category*/
#define RMS_V2_INTERN_ERROR_CATEGORY     0x03
/****************************************************************/
/*                                                              */
/* Type Definitions                                             */
/*                                                              */
/* Note: V2 indicates that this is the 3G (Version 2) of        */
/*       the RMS interface                                      */
/*                                                              */
/****************************************************************/
// Error values returned by SMR
/**
 * The error code is passed as an uint16 value. The error codes are separated
 * into different categories, depending on the entity that reported the error. 
 *
 * The higher significant byte contains the error category value, while the
 * lower byte contains the error code value.
 *
 * Relay layer (RP) errors, as specified in ref.[1], belong to the RP error
 * category. The errors appearing in the Short Message Control layer (SMC),
 * specified in ref.[1], are grouped into CP error codes. Errors internal to
 * the MS belong to the internal RMS error codes (INTERN error category).
 * They include indications on general failure to establish connection, or
 * indications on expiry of internal timers, relevant to the SMS transfer.
 *
 * The values allocated to relevant error categories are as follows:
 *
 * RMS_V2_RP_ERROR_CATEGORY -> 0x01
 *
 * RMS_V2_CP_ERROR_CATEGORY-> 0x02
 *
 * RMS_V2_INTERN_ERROR_CATEGORY -> 0x03
 *
 *
 * @param RMS_V2_ERROR_CODE_RP_UNALLOCATED_NUMBER                          Unallocated number
 * @param RMS_V2_ERROR_CODE_RP_OPERATOR_DETERMINED_BARRING                 Operator determined barring
 * @param RMS_V2_ERROR_CODE_RP_CALL_BARRED                                 Call barred
 * @param RMS_V2_ERROR_CODE_RP_RESERVED                                    Reserved
 * @param RMS_V2_ERROR_CODE_RP_SMS_TRANSFER_REJECTED                       SMS transfer rejected
 * @param RMS_V2_ERROR_CODE_RP_DESTINATION_OUT_OF_ORDER                    Destination out of order
 * @param RMS_V2_ERROR_CODE_RP_UNIDENTIFIED_SUBSCRIBER                     Unidentified subscriber
 * @param RMS_V2_ERROR_CODE_RP_FACILITY_REJECTED                           Facility rejected
 * @param RMS_V2_ERROR_CODE_RP_UNKNOWN_SUBSCRIBER                          Unknown subscriber
 * @param RMS_V2_ERROR_CODE_RP_NETWORK_OUT_OF_ORDER                        Network out of order
 * @param RMS_V2_ERROR_CODE_RP_TEMPORARY_FAILURE                           Temporary failure
 * @param RMS_V2_ERROR_CODE_RP_CONGESTION                                  Congestion
 * @param RMS_V2_ERROR_CODE_RP_RESOURCES_UNAVAILABLE_UNSPECIFIED           Resources unavailable, unspecified
 * @param RMS_V2_ERROR_CODE_RP_REQUESTED_FACILITY_NOT_SUSCRIBED            Requested facility not subscribed
 * @param RMS_V2_ERROR_CODE_RP_REQUESTED_FACILITY_NOT_IMPLEMENTED          Requested facility not implemented
 * @param RMS_V2_ERROR_CODE_RP_INVALID_SMS_TRANSFER_REFERENCE_VALUE        Invalid SMS transfer reference value
 * @param RMS_V2_ERROR_CODE_RP_SEMANTICALLY_INCORRECT_MESSAGE              Semantically incorrect message
 * @param RMS_V2_ERROR_CODE_RP_INVALID_MANDATORY_INFORMATION               Invalid mandatory information
 * @param RMS_V2_ERROR_CODE_RP_MSG_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED    Message type non-existent or not implemented
 * @param RMS_V2_ERROR_CODE_RP_MSG_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE  Message not compatible with SMS protocol state
 * @param RMS_V2_ERROR_CODE_RP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED          IE not existent or not implemented
 * @param RMS_V2_ERROR_CODE_RP_INTERWORKING_UNSPECIFIED_PROBLEM            Interworking unspecified problem
 * @param RMS_V2_ERROR_CODE_RP_PROTOCOL_ERROR_UNSPECIFIED                  Protocol error, unspecified
 * @param RMS_V2_ERROR_CODE_RP_MEMORY_CAPACITY_EXCEEDED                    Memory capacity exceeded
 * @param RMS_V2_ERROR_CODE_CP_NETWORK_FAILURE                             Network failure
 * @param RMS_V2_ERROR_CODE_CP_CONGESTION                                  Congestion
 * @param RMS_V2_ERROR_CODE_CP_INVALID_TI_VALUE                            Invalid TI value
 * @param RMS_V2_ERROR_CODE_CP_SEMANTICALLY_INCORRECT_MESSAGE              Semantically incorrect message
 * @param RMS_V2_ERROR_CODE_CP_INVALID_MANDATORY_INFORMATION               Invalid mandatory information
 * @param RMS_V2_ERROR_CODE_CP_MSG_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED    Message type non-existent or not implemented
 * @param RMS_V2_ERROR_CODE_CP_MSG_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE  Message not compatible with SMS protocol state
 * @param RMS_V2_ERROR_CODE_CP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED          IE not existent or not implemented
 * @param RMS_V2_ERROR_CODE_CP_PROTOCOL_ERROR_UNSPECIFIED                  Protocol error, unspecified

 * @param RMS_V2_ERROR_CODE_INTERN_CONNECTION_ESTABLISHMENT_FAILED         Connection establishment failed
 * @param RMS_V2_ERROR_CODE_INTERN_CONCATENATION_TIMER_EXPIRY              Concatenation timer expiry
 * @param RMS_V2_ERROR_CODE_INTERN_RELAY_LAYER_TIMER_EXPIRY                Relay layer timer expiry
 * @param RMS_V2_ERROR_CODE_INTERN_CONTROL_LAYER_TIMER_EXPIRY              Control layer timer expiry
 * @param RMS_V2_ERROR_CODE_INTERN_CONNECTION_LOST                         Connection lost
 * @param RMS_V2_ERROR_CODE_INTERN_RADIO_PATH_NOT_AVAILABLE                Radio path not available
 * @param RMS_V2_ERROR_CODE_INTERN_NO_ERROR                                No error
 * @param RMS_V2_ERROR_CODE_INTERN_NO_ERROR_NO_ACK                         A V3_Sendacknowledgement was issued and the network did
 *                                                                         not send an acknowledgement to the deliver report but disconnected
 *                                                                         properly instead.
 * @param RMS_V2_ERROR_CODE_INTERN_TIMEOUT                                 V3_Sendacknowledgement was issued and the MS has neither received an
 *                                                                         acknowledgement nor a release from the network within SMC's internal
 *                                                                         timeout period
 * @param RMS_V2_ERROR_CODE_INTERN_RELEASE_FAILURE                         V3_Sendacknowledgement was issued and an abnormal release was received
 */
    TYPEDEF_ENUM
{
RMS_V2_ERROR_CODE_RP_UNALLOCATED_NUMBER = 0x0101,
        RMS_V2_ERROR_CODE_RP_OPERATOR_DETERMINED_BARRING = 0x0108,
        RMS_V2_ERROR_CODE_RP_CALL_BARRED = 0x010a,
        RMS_V2_ERROR_CODE_RP_RESERVED = 0x010b,
        RMS_V2_ERROR_CODE_RP_SMS_TRANSFER_REJECTED = 0x0115,
        RMS_V2_ERROR_CODE_RP_DESTINATION_OUT_OF_ORDER = 0x011b,
        RMS_V2_ERROR_CODE_RP_UNIDENTIFIED_SUBSCRIBER = 0x011c,
        RMS_V2_ERROR_CODE_RP_FACILITY_REJECTED = 0x011d,
        RMS_V2_ERROR_CODE_RP_UNKNOWN_SUBSCRIBER = 0X011e,
        RMS_V2_ERROR_CODE_RP_NETWORK_OUT_OF_ORDER = 0x0126,
        RMS_V2_ERROR_CODE_RP_TEMPORARY_FAILURE = 0x0129,
        RMS_V2_ERROR_CODE_RP_CONGESTION = 0x012a,
        RMS_V2_ERROR_CODE_RP_RESOURCES_UNAVAILABLE_UNSPECIFIED = 0x012f,
        RMS_V2_ERROR_CODE_RP_REQUESTED_FACILITY_NOT_SUSCRIBED = 0x0132,
        RMS_V2_ERROR_CODE_RP_REQUESTED_FACILITY_NOT_IMPLEMENTED = 0x0145,
        RMS_V2_ERROR_CODE_RP_INVALID_SMS_TRANSFER_REFERENCE_VALUE = 0x0151,
        RMS_V2_ERROR_CODE_RP_SEMANTICALLY_INCORRECT_MESSAGE = 0x015f,
        RMS_V2_ERROR_CODE_RP_INVALID_MANDATORY_INFORMATION = 0x0160,
        RMS_V2_ERROR_CODE_RP_MSG_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 0x0161,
        RMS_V2_ERROR_CODE_RP_MSG_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE = 0x0162,
        RMS_V2_ERROR_CODE_RP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED = 0x0163,
        RMS_V2_ERROR_CODE_RP_INTERWORKING_UNSPECIFIED_PROBLEM = 0x017f,
        RMS_V2_ERROR_CODE_RP_PROTOCOL_ERROR_UNSPECIFIED = 0x016f,
        RMS_V2_ERROR_CODE_RP_MEMORY_CAPACITY_EXCEEDED = 0x0116,
        RMS_V2_ERROR_CODE_CP_NETWORK_FAILURE = 0x0211,
        RMS_V2_ERROR_CODE_CP_CONGESTION = 0x0216,
        RMS_V2_ERROR_CODE_CP_INVALID_TI_VALUE = 0x0251,
        RMS_V2_ERROR_CODE_CP_SEMANTICALLY_INCORRECT_MESSAGE = 0x025F,
        RMS_V2_ERROR_CODE_CP_INVALID_MANDATORY_INFORMATION = 0x0260,
        RMS_V2_ERROR_CODE_CP_MSG_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 0x0261,
        RMS_V2_ERROR_CODE_CP_MSG_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE = 0x0262,
        RMS_V2_ERROR_CODE_CP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED = 0x0263,
        RMS_V2_ERROR_CODE_CP_PROTOCOL_ERROR_UNSPECIFIED = 0x026f,
        RMS_V2_ERROR_CODE_INTERN_CONNECTION_ESTABLISHMENT_FAILED = 0x0301,
        RMS_V2_ERROR_CODE_INTERN_CONCATENATION_TIMER_EXPIRY = 0x0302,
        RMS_V2_ERROR_CODE_INTERN_RELAY_LAYER_TIMER_EXPIRY = 0x0303,
        RMS_V2_ERROR_CODE_INTERN_CONTROL_LAYER_TIMER_EXPIRY = 0x0304,
        RMS_V2_ERROR_CODE_INTERN_CONNECTION_LOST = 0x0305,
        RMS_V2_ERROR_CODE_INTERN_RADIO_PATH_NOT_AVAILABLE = 0x0396,
        RMS_V2_ERROR_CODE_INTERN_NO_ERROR = 0x03ff,
        RMS_V2_ERROR_CODE_INTERN_NO_ERROR_NO_ACK = 0x03FE,
        RMS_V2_ERROR_CODE_INTERN_TIMEOUT = 0x0306, RMS_V2_ERROR_CODE_INTERN_RELEASE_FAILURE = 0x0307, RMS_V2_ERROR_CODE_INTERN_ACCESS_CLASS_BARRED = 0x0308}
ENUM16(RMS_V2_ErrorCode_t);

// SMS address format

/**
 * The following data type is used for different address information, relevant
 * to SMS transfer (destination, originator, Service Center address).
 *
 * @param Length            includes the actual length of AddressValue
 *                          array plus length of  TypeOfAddress.
 * @param TypeOfAddess      Address type
 * @param AddressValue      Address
 */
typedef struct {
    uint8 Length;
    uint8 TypeOfAddess;
    uint8 AddressValue[RMS_V2_MAX_SM_ADDRESS_LENGTH];
} RMS_V2_Address_t;

// Messaging user data 

/**
 * Contains the TPDU.
 *
 * @param UserDataLength    TPDU Length
 * @param UserDataBody      TPDU
 */
typedef struct {
    uint8 UserDataLength;
    uint8 UserDataBody[RMS_V2_MAX_RP_USER_DATA_LENGTH + 1];
} RMS_V2_UserData_t;

/**
 * The RMS_V2_Status_t information contains the information on the outcome of
 * the requested activity. The boolean value indicates whether the outcome of
 * the requested activity has been successful. In case of failure, the
 * relevant error information is included in the error code.
 *
 * NOTE: This status does not refer to the status of the invoked requester
 *       function itself.
 *
 * @param SuccessfulOutcome   Indicates whether the outcome of the requested
 *                            activity has been successful or not.
 * @param ErrorCode           In case of failure, the relevant error information
 *                            is included here.
 */
typedef struct {
    boolean SuccessfulOutcome;
    RMS_V2_ErrorCode_t ErrorCode;
} RMS_V2_Status_t;

// Concatenation info 

/**
 * This data type contains information related to the transfer of long
 * (concatenated/enhanced) messages.
 *
 * @param SeqNum            Short message sequence number in a stream of short
 *                          messages belonging to a concatenated/enhanced message.
 *
 * @param Total             Total number of short messages in a concatenated/enhanced 
 *                          message Maximum (theoretical so far, please see GSM 03.40) 255
 */
typedef struct {
    uint8 SeqNum;
    uint8 Total;
} RMS_V2_ConcatMsgSequence_t;

// This structure is used as a container for additional transmission information 
// Adding any new transmission info will not affect the requester interface

/**
 * This data type acts as a container for transmission parameters relevant
 * for SMS transfer.
 *
 * There is currently only one transmission parameter defined (MsgSequence).
 *
 * @param MsgSequence       Message Sequence
 */
typedef struct {
    RMS_V2_ConcatMsgSequence_t MsgSequence;
} RMS_V2_TransmissionParams_t;


#endif                          // INCLUSION_GUARD_T_RMS_H
