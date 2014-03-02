#ifndef INCLUSION_GUARD_T_CBS_H
#define INCLUSION_GUARD_T_CBS_H

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************************
*
* DESCRIPTION:
*     Sw Backplane Cell Broadcast Transport Server type definitions
*
********************************************************************************************
*/

#include <stdint.h>


/**
  *  @brief The maximum length in bytes of the CB message content.
  */
#define CBS_MESSAGE_MAX_PAGE_LENGTH 82

/**
  *  @brief The maximum length in bytes of the ETWS warning content.
  */
#define CBS_MESSAGE_MAX_ETWS_LENGTH 50


/**
  *  @brief The maximum length in bytes of the CB message PDU (UMTS CB Message with 15 filled pages).
  */
#define CBS_MESSAGE_MAX_LENGTH 1252


/**
  * @brief Specifies the Pdu. The Cell Broadcast PDU containing the entire CB Message as specified in document 3G TS 23.041[1].
  *
  * This type specifies SerialNumber.
  */
typedef struct {
    uint16_t PduLength;
    uint8_t Pdu[CBS_MESSAGE_MAX_LENGTH];
} CB_Pdu_t;


/**
  * @brief Specifies SerialNumber. The Cell Broadcast serial number as specified in document 3G TS 23.041[1].
  *
  * This type specifies SerialNumber.
  */
typedef uint16_t CBS_SerialNumber_t;


/**
  * @brief Specifies MessageIdentifier. The Cell Broadcast message identifier in the range specified in document 3G TS 23.041[1].
  *
  * This type specifies MessageIdentifier.
  */
typedef uint16_t CBS_MessageIdentifier_t;


/**
  * @brief Specifies DataCodingScheme. The Cell Broadcast Data Coding Scheme as specified in document 3G TS 23.041[1].
  *
  * This type specifies DataCodingScheme.
  */
typedef uint8_t CBS_DataCodingScheme_t;


/**
  * @brief Specifies PageParameter. The Cell Broadcast page parameter indicates the total number of message pages and sequence number as specified in document 3G TS 23.041[1].
  *
  * This type specifies PageParameter.
  */
typedef uint8_t CBS_PageParameter_t;


/**
  * @brief Specifies MessageHeader. The Cell Broadcast message serial number, CBMI, DCS and page parameter.
  *
  * This type specifies MessageHeader.
  */
typedef struct {
  /**
  * @brief The SerialNumber specifies the Cell Broadcast serial number.
  */
    CBS_SerialNumber_t SerialNumber;

  /**
  * @brief The MessageIdentifier specifies the Cell Broadcast message identifier (CBMI).
  */
    CBS_MessageIdentifier_t MessageIdentifier;

  /**
  * @brief The DataCodingScheme specifies the Cell Broadcast Data Coding Scheme (DCS).
  */
    CBS_DataCodingScheme_t DataCodingScheme;

  /**
  * @brief The PageParameter specifies the Cell Broadcast page parameter.
  */
    CBS_PageParameter_t PageParameter;
} CBS_MessageHeader_t;


/**
  * @brief The Cell Broadcast message data corresponds to one page as specified in document 3G TS 23.041[1].
  *
  * This type specifies PageParameter.
  */
typedef uint8_t CBS_MessageData_t[CBS_MESSAGE_MAX_PAGE_LENGTH];


/**
  * \struct CBS_Message_t
  * @brief CB Message
  *
  * Data structure to hold a CB message block.
  */
typedef struct {
    CBS_MessageHeader_t Header;
    uint8_t UsefulData;
    CBS_MessageData_t Data;
} CBS_Message_t;


/**
  * \struct CBS_MessagesList_t
  * @brief CB Message list
  *
  * Data structure to hold a list of CB message blocks.
  */
typedef struct CBS_MessagesList_Tag {
    CBS_Message_t Message;
    struct CBS_MessagesList_Tag *Next_p;
} CBS_MessagesList_t;


#define SMS_MAL_CB_CONTENTS_MAX_LENGTH 82

/**
 * \struct SMS_MAL_CALLBACK_CB_Message_Data_t
 * \brief CB SMS data
 *
 *  This structure holds the CB Message data block
 */
typedef struct {
    CBS_MessageHeader_t header;
    uint8_t useful_data_length;
    uint8_t data[SMS_MAL_CB_CONTENTS_MAX_LENGTH];
} SMS_MAL_CALLBACK_CB_Message_Data_t;

/**
 * \struct SMS_MAL_Callback_CB_Routing_Ind_Data_t
 * \brief CB SMS data
 *
 *  This structure holds a CB Routing indication
 *  with a list of CB Message blocks.
 */
typedef struct {
    uint8_t number_of_messages;
    SMS_MAL_CALLBACK_CB_Message_Data_t messages[1];
} SMS_MAL_Callback_CB_Routing_Ind_Data_t;


/**
  * @brief Specifies Subscription. Ranges of CBMI and DCS to subscribe to.
  *
  * This type specifies Subscription.
  */
typedef struct {
  /**
    * @brief The FirstMessageIdentifier specifies the first CBMI in the subscription range.
    */
    CBS_MessageIdentifier_t FirstMessageIdentifier;

  /**
    * @brief The LastMessageIdentifier specifies the last CBMI in the subscription range.
    */
    CBS_MessageIdentifier_t LastMessageIdentifier;

  /**
    * @brief The FirstDataCodingScheme specifies the first DCS in the subscription range.
    */
    CBS_DataCodingScheme_t FirstDataCodingScheme;

  /**
    * @brief The LastDataCodingScheme specifies the last DCS in the subscription range.
    */
    CBS_DataCodingScheme_t LastDataCodingScheme;
} CBS_Subscription_t;


/**
  * @brief Specifies Error. Defines the error codes returned by the Cell Broadcast Server.
  *
  * This type specifies Subscription.
  */
typedef enum {
  /**
    * @brief The CBS_ERROR_NONE indicates that no error occurred.
    */
    CBS_ERROR_NONE,

  /**
    * @brief The CBS_ERROR_TEMPORARY_FAILURE indicates that temporary failure occurred.
    */
    CBS_ERROR_TEMPORARY_FAILURE,

  /**
    * @brief The CBS_ERROR_MEMORY_ALLOCATION_FAILED indicates that a failure to allocate heap memory occurred.
    */
    CBS_ERROR_MEMORY_ALLOCATION_FAILED,

  /**
    * @brief The CBS_ERROR_TOO_MANY_SUBSCRIBERS indicates that maximum number of subscriptions has been exceeded.
    */
    CBS_ERROR_TOO_MANY_SUBSCRIBERS,

  /**
    * @brief The CBS_ERROR_ALREADY_SUBSCRIBED indicates that an attempt has been made to add a subscription which already exists for that process.
    */
    CBS_ERROR_ALREADY_SUBSCRIBED,

  /**
    * @brief The CBS_ERROR_SUBSCRIPTION_FAILED indicates that the subscription failed.
    */
    CBS_ERROR_SUBSCRIPTION_FAILED,

  /**
    * @brief The CBS_ERROR_INVALID_SUBSCRIPTION indicates that the subscription does not exist.
    */
    CBS_ERROR_INVALID_SUBSCRIPTION,

  /**
    * @brief The CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG indicates that the request control block is incorrect.
    */
    CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG,

  /**
    * @brief The CBS_ERROR_INVALID_PARAMETER indicates that an invalid parameter has been used.
    */
    CBS_ERROR_INVALID_PARAMETER,

  /**
    * @brief The CBS_ERROR_REQUESTED_MESSAGE_NO_LONGER_EXISTS indicates that an attempt has been made to retrieve a message which no longer exists in RAM.
    */
    CBS_ERROR_REQUESTED_MESSAGE_NO_LONGER_EXISTS,

  /**
    * @brief The CBS_ERROR_RADIO_INACTIVE indicates that the radio link is inactive.
    */
    CBS_ERROR_RADIO_INACTIVE,

  /**
    * @brief The CBS_ERROR_CS_INACTIVE indicates that the CS is inactive.
    */
    CBS_ERROR_CS_INACTIVE,

    CBS_ERROR_CS_INTERNAL_ERROR,

  /**
    * @brief The CBS_ERROR_LAST_ERROR_TYPE indicates last error type.
    */
    CBS_ERROR_LAST_ERROR_TYPE
} CBS_Error_t;

#endif                          // INCLUSION_GUARD_T_CBS_H
