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
 * SMS Server customer configuration include file.
 *
 *************************************************************************/

#ifndef C_SMS_CONFIG_H
#define C_SMS_CONFIG_H

// Determines if Status Reports are matched with sent MO Messages and stored.
extern const unsigned char SMS_STATUS_REPORTS_MATCH_STORE;

// Determines the maximum number of slot entries in the SIM Cache.
extern const unsigned char SMS_SIM_CACHE_MAX_SIZE;

// Determines if the SIM RAM cache shall be used, set to SMS_SIM_RAM_CACHE_DISABLED to disable SIM RAM cache.
#define SMS_SIM_RAM_CACHE_DISABLED 0
#define SMS_SIM_RAM_CACHE_ENABLED  1
extern const unsigned char SMS_SIM_RAM_CACHE_USAGE;

// Determines the maximum number of slot entries in the control directory.
extern const unsigned char SMS_CONTROL_DIR_MAX_SIZE;

// Determines whether or not sending of Memory Capacity Available
// is under client application control
extern const unsigned char SMS_MEM_CAPACITY_AVAILABLE_CLIENT_SEND;

// Determines whether or not messages intended for WAP are stored in a separate directory
extern const unsigned char SMS_SMSTORAGE_WAP_SPECIFIED;

// Determines whether or not Message storage is ICCID related
extern const unsigned char SMS_STORAGE_IS_ICCID_RELATED;

// Concatenated reference bits is either 8 or 16
extern const unsigned char SMS_CONCAT_REFERENCE_BITS;

// Maximum number of SMs allowed to be stored in ME is from 1-65535
extern const unsigned short SMS_MAX_STORAGE_ME_SHORT_MESSAGES;

// The number of tpdus that can be cached
extern const unsigned char SMS_TPDU_CACHE_ENTRIES;

// Timeout value - used to control the period of inactivity before the MoreToSend mechanism is
// overriden, the radio channel is closed and other clients are unblocked from sending
extern const unsigned char SMS_MORETOSEND_TIMEOUT_VALUE;

// Timeout value - number of seconds before client application regarded as not repsonding by MSG.
extern const unsigned char SMS_DELIVER_REPORT_TIMEOUT_VALUE;

// Timer value in millisecs used for SM storage initialisation
extern const unsigned char SMS_STORAGE_INIT_TIMER_VALUE;

// Timer value for polling SM volumes to determine if Memoy Capacity is available
extern const unsigned char SMS_TIMER_STORAGE_POLL_INTERVAL;

// User defines which timestamp is to be used
extern const unsigned char SMS_USER_DEFINED_TIMESTAMP;

// Determines whether or not SIM stored SMS messages are validated on startup.
extern const unsigned char SMS_VALIDATE_SIM_STORED_SMS;

// Determines whether or not TP-RD is set if an attempt is made to send an SMS
// with a non-zero TP-MR.
extern const unsigned char SMS_MO_RETRY_IF_MESSAGE_REFERENCE_SET;

// Determines whether or not the radio link is maintained between the sending of concatenated short messages
extern const unsigned char SMS_ENABLE_CONCAT_SMS_SEND;

// User defines the preferred SM storage
extern const char *Preferred_Storage_Default[];

// Determines the default TP-MR
extern const unsigned char SMS_DEFAULT_TP_MR;

// Timeout value - used when the modem sends multiple changed EF file indications as a result of a CAT
// refresh. The SMS / CBS Server waits for this timeout (ms) after the last indication is received before
// updating its internal caches.
extern const unsigned int SMS_CAT_REFRESH_CHANGED_FILE_IND_TIMEOUT_VALUE;


// ! WARNING - THIS DEFINE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
#define SMS_MAX_SM_ADDRESS_LENGTH       (10)

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef struct {
    unsigned char Length;
    unsigned char TypeOfAddress;
    unsigned char AddressValue[SMS_MAX_SM_ADDRESS_LENGTH];
} ShortMessagePackedAddress_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef struct {
    unsigned int MCC1:4;
    unsigned int MCC2:4;
    unsigned int MCC3:4;
    unsigned int MNC3:4;
    unsigned int MNC1:4;
    unsigned int MNC2:4;
} MSG_PLMN_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef enum {
    SM_CLASS_0,
    SM_CLASS_1_ME_SPECIFIC,
    SM_CLASS_2_SIM_SPECIFIC,
    SM_CLASS_3_TE_SPECIFIC,
    SM_CLASS_UNKNOWN
} SMClass_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef enum {
    TP_NO_CAUSE = 0,            // No error cause specified in case of positive ack
    // Transfer layer failure causes (contained within TPDU)
    TP_ERROR_IN_MS = 0x00D2,
    TP_MEMORY_CAPACITY_EXCEEDED = 0x00D3,
    TP_SIM_APPLICATION_TOOLKIT_BUSY = 0x00D4,
    TP_SIM_DATA_DOWNLOAD_ERROR = 0x00D5,
    TP_UNSPECIFIED_ERROR_CAUSE = 0x00FF
} TP_FailureCause_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef enum {
    SM_DELIVER = 0,             // SC to MS RENAME TO TP_DELIVER etc check 23.040??
    SM_SUBMIT_REPORT_ERROR = 1, // SC to MS
    SM_STATUS_REPORT = 2,       // SC to MS
    SM_SUBMIT_REPORT_ACK = 3,   // SC to MS
    SM_DELIVER_REPORT_ERROR = 4,        // MS to SC
    SM_SUBMIT = 5,              // MS to SC
    SM_COMMAND = 6,             // MS to SC
    SM_DELIVER_REPORT_ACK = 7,  // MS to SC
    SM_TYPE_LAST = 8
} ShortMessageType_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef enum {
    SHORTMESSAGEROUTER_STORAGE_ME_SPECIFIED = 1,        //Store only in specified storage for message class (SIM or ME), fail otherwise
    SHORTMESSAGEROUTER_STORAGE_SIM_SPECIFIED,   //Store only in specified storage for message class (SIM or ME), fail otherwise
    SHORTMESSAGEROUTER_STORAGE_PREFERRED,       //Store in platform default preferred storage (SIM or ME), fallback to non-preferred, fail otherwise.
    SHORTMESSAGEROUTER_STORAGE_ME_PREFERRED,    //Store in ME storage if possible, fallback to non-preferred, fail otherwise.
    SHORTMESSAGEROUTER_STORAGE_SIM_PREFERRED,   //Store in SIM storage if possible, fallback to non-preferred, fail otherwise.
    SHORTMESSAGEROUTER_STORAGE_SIM_DELETE,      //Store then Delete(needed for one2one SIM specific delivery [15])
    SHORTMESSAGEROUTER_STORAGE_ME_DELETE,       //Store then delete
    SHORTMESSAGEROUTER_STORAGE_DELETE,  //Store then delete
    SHORTMESSAGEROUTER_STORAGE_NO_STORE,        //Do not store
    SHORTMESSAGEROUTER_STORAGE_CONTROL  //Store in control directory
} ShortMessageStorageRule_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef struct {
    unsigned int TPMTI_ACTIVE:1;
    unsigned int TPMTI_POSITIVE:1;

    unsigned int TPOA_ACTIVE:1;
    unsigned int TPOA_POSITIVE:1;

    unsigned int TPPID_ACTIVE:1;
    unsigned int TPPID_POSITIVE:1;

    unsigned int TPDCS_ACTIVE:1;
    unsigned int TPDCS_POSITIVE:1;

    unsigned int OTADP_ACTIVE:1;
    unsigned int OTADP_POSITIVE:1;

    unsigned int DEFAULT_ACTIVE:1;
    unsigned int DEFAULT_POSITIVE:1;

    unsigned int HomePLMN_ACTIVE:1;
    unsigned int HomePLMN_POSITIVE:1;

    unsigned int Class_ACTIVE:1;
    unsigned int Class_POSITIVE:1;

    unsigned int ORIG_PORT_RANGE_ACTIVE:1;
    unsigned int ORIG_PORT_RANGE_POSITIVE:1;

    unsigned int DEST_PORT_RANGE_ACTIVE:1;
    unsigned int DEST_PORT_RANGE_POSITIVE:1;

    unsigned int CHECK_FOR_IE_ACTIVE:1;
    unsigned int CHECK_FOR_IE_POSITIVE:1;
} ShortMessageRouterRulesUsed_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef enum {
    SHORTMESSAGEROUTER_NOTIFY_NORMAL = 1,       //Send to COPS module
    SHORTMESSAGEROUTER_NOTIFY_COPS,     //Send to COPS module
    SHORTMESSAGEROUTER_NOTIFY_SAT,      //Send to SIM Toolkit
    SHORTMESSAGEROUTER_NOTIFY_NONE      //No notification
} ShortMessageRouterNotify_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef enum {
    SHORTMESSAGEROUTER_NETACK_NORMAL = 1,       //Send normal ack/nack to network
    SHORTMESSAGEROUTER_NETACK_SPECIAL,  //Send the ACK/NACK specified in NETACKVAL/NETNACKVAL to the network
    SHORTMESSAGEROUTER_NETACK_CLIENT,   //The ACK/NACK to the network is handled by the client EXCEPT memory capacity exceeded
    SHORTMESSAGEROUTER_NETACK_NONE      //Send no ack/nack to network
} ShortMessageRouterNetAck_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef struct {
    unsigned int OTADP:1;       //Over The Air Depersonalisation
    unsigned int DEFAULT:1;     //Default rule (match everything)
    unsigned int CONTINUE:1;    //Continue parsing even if this rule matches
} ShortMessageRouterFlags_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef struct {
    unsigned short StartOfRange;
    unsigned short EndOfRange;
} ShortMessageRouterPortRange_t;

// ! WARNING - THIS TYPE MUST ONLY BE MODIFIED BY ST-Ericsson !
//
typedef struct {
    ShortMessageRouterRulesUsed_t ACTIVERULES;  //Bitfield indicating if a rule is active, and rule polarity (postive/negative)
    ShortMessageType_t TPMTI;   //Message type (e.g. SM_DELIVER, SM_STATUS_REPORT)
    ShortMessagePackedAddress_t TPOA;   //Originator address element coded as specified in 3GPP TS 24.011 , not including the IEI.
    unsigned char TPPID;        //Protocol Identifier TP-PID as defined in 3GPP TS 23.040
    unsigned char TPDCS;        //TP-Data coding scheme as defined is 3GPP TS 23.038
    ShortMessageRouterFlags_t Flags;    //Flags - OTADP, MWI, DEFAULT, CONTINUE
    MSG_PLMN_t HomePLMN;        //The home PLMN of the current IMSI
    SMClass_t Class;            //Message Class (can also be determined by specifying particular TPDCS values)
    ShortMessageRouterNotify_t APPNOTIFY;       //Application to notify (see below for options) 1
    ShortMessageStorageRule_t STORAGE;  //Storage preference (see below for storage actions)  1
    ShortMessageRouterNetAck_t NETACK;  //Send network ack/nak to notify (see below for options)  1
    unsigned char NETACKVAL;    //ACK value to be sent to network (if non-standard) 1
    unsigned char NETNACKVAL;   //NACK value to be sent to network (if non-standard)  1
    ShortMessageRouterPortRange_t OrigPortRange;        //Range for origination port
    ShortMessageRouterPortRange_t DestPortRange;        //Range for destination port
    unsigned char IEType;       //IE Type to match
} ShortMessageRule_t;


#define MAX_RULES 30


//______________ RuleTable created by customer _________________
extern ShortMessageRule_t SMRuleTable[];
//______________________________________________________________


// exported function declaration
unsigned char SMS_InitialiseShortMessageRoutingRules(
    void);

#endif                          // C_SMS_CONFIG_H
