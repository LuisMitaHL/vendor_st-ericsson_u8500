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
 * SMS Server customer configuration source file.
 *
 *************************************************************************/

#include <string.h>
#include <stdio.h>
#include "c_sms_config.h"


// Determines if Status Reports are matched with sent MO Messages and stored.
const unsigned char SMS_STATUS_REPORTS_MATCH_STORE = 0;

const unsigned char SMS_SIM_CACHE_MAX_SIZE = 0;

// Determines if the SIM RAM cache shall be used, set to SMS_SIM_RAM_CACHE_DISABLED to disable SIM RAM cache.
const unsigned char SMS_SIM_RAM_CACHE_USAGE = SMS_SIM_RAM_CACHE_ENABLED;

// Determines the maximum number of slot entries in the control directory.
const unsigned char SMS_CONTROL_DIR_MAX_SIZE = 0;

// Determines whether or not sending of Memory Capacity Available
// is under client application control
const unsigned char SMS_MEM_CAPACITY_AVAILABLE_CLIENT_SEND = 1;

// Determines whether or not messages intended for WAP are stored in a separate directory
const unsigned char SMS_SMSTORAGE_WAP_SPECIFIED = 0;

// Determines whether or not Message storage is ICCID related
const unsigned char SMS_STORAGE_IS_ICCID_RELATED = 1;

// Concatenated reference bits is either 8 or 16
const unsigned char SMS_CONCAT_REFERENCE_BITS = 8;

// Number of SM parts per MSG is from 1-255
const unsigned char SMS_MAX_SHORT_MESSAGE_PARTS = 20;

// Maximum number of SMs allowed to be stored in ME is from 1-65535
// When set to zero the storage of SMs is dependant upon whether or not
// there is any space available on the volumes in the preferred storage list.
#ifdef MSG_MAX_ME_SM_TEST
// This value has been derived in order to allow System Tests to pass
const unsigned short SMS_MAX_STORAGE_ME_SHORT_MESSAGES = 100;
#else
// This value has been derived in order to allow Type Approval Tests to pass
const unsigned short SMS_MAX_STORAGE_ME_SHORT_MESSAGES = 30;
#endif

// The number of tpdus that can be cached
const unsigned char SMS_TPDU_CACHE_ENTRIES = 5;

// Timeout value - used to control the period of inactivity before the MoreToSend mechanism is
// overriden, the radio channel is closed and other clients are unblocked from sending (in seconds)
const unsigned char SMS_MORETOSEND_TIMEOUT_VALUE = 5;

// Timeout value - number of seconds before client application regarded as not repsonding by MSG.
const unsigned char SMS_DELIVER_REPORT_TIMEOUT_VALUE = 20;

// Timer value in millisecs used for SM storage initialisation
const unsigned char SMS_STORAGE_INIT_TIMER_VALUE = 100;

// Timer value for polling SM volumes to determine if Memoy Capacity is available
const unsigned char SMS_TIMER_STORAGE_POLL_INTERVAL = 30;

// Default timestamp = 0;
// Returns the timestamp of the file created to store message = 0
// Returns the timestamp when the SMS was sent by the network = 1
const unsigned char SMS_USER_DEFINED_TIMESTAMP = 0;

// Default = 1
// SIM stored SMS messages are validated on startup.
//
// Config  = 0
// SIM stored SMS messages are not validated on startup.
const unsigned char SMS_VALIDATE_SIM_STORED_SMS = 1;

// Determines whether or not TP-RD is set if an attempt is made to send an SMS
// with a non-zero TP-MR.
const unsigned char SMS_MO_RETRY_IF_MESSAGE_REFERENCE_SET = 0;

// Determines whether or not the radio link is maintained between the sending of concatenated short messages
// Default = 1 i.e. the radio link is maintained unless overriden by AT+CMMS.
const unsigned char SMS_ENABLE_CONCAT_SMS_SEND = 1;

// Define the preferred storage locations in order of preference
// NB "/sim" folder name is used to represent the USIM as opposed
// to the ME where all other folder names are assumed to be.
#ifdef PLATFORM_ANDROID
static const char PREFERRED_STORAGE_FS[] = { '/', 'd', 'a', 't', 'a', '/', 'a', 'p', 'p', '-', 'p', 'r', 'i', 'v', 'a', 't', 'e', '\0' };       //folder name can be defined by customer !!! MUST BE REMOVED FROM DEFAULT MBL/FB VARIANT - REQUIRED FOR AT ME STORAGE ONLY !!!!!
#else
static const char PREFERRED_STORAGE_FS[] = { '/', 'd', 'e', 'v', '\0' };        //folder name can be defined by customer !!! MUST BE REMOVED FROM DEFAULT MBL/FB VARIANT - REQUIRED FOR AT ME STORAGE ONLY !!!!!
#endif
static const char PREFERRED_STORAGE_SIM[] = { '/', 's', 'i', 'm', '\0' };       //special folder name identifies the USIM
static const char PREFERRED_STORAGE_NUL[] = { '\0' };   //Always terminate with NULL

const char *Preferred_Storage_Default[] = {
    PREFERRED_STORAGE_FS,
    PREFERRED_STORAGE_SIM,
    PREFERRED_STORAGE_NUL
};

// Determines the default TP-MR
const unsigned char SMS_DEFAULT_TP_MR = 0xFF;

// Timeout value - used when the modem sends multiple changed EF file indications as a result of a CAT
// refresh. The SMS / CBS Server waits for this timeout (ms) after the last indication is received before
// updating its internal caches.
const unsigned int SMS_CAT_REFRESH_CHANGED_FILE_IND_TIMEOUT_VALUE = 2000;

/********************************************************************
 * Static Declarations
 ********************************************************************/


//______________ RuleTable created by customer _________________
ShortMessageRule_t SMRuleTable[MAX_RULES];
//______________________________________________________________


/*******************************************************************
 *              E X P O R T E D   F U N C T I O N
 ********************************************************************/
/**
 *
 * @function     SMS_InitialiseShortMessageRoutingRules
 *
 * @description  Read a routing rule from the SM routing table
 *
 * @return       unsigned char : The number of the routing rules
 */
/********************************************************************/
unsigned char SMS_InitialiseShortMessageRoutingRules(
    void)
{
    unsigned char RuleCount = 0;

    memset(SMRuleTable, 0x00, sizeof(ShortMessageRule_t) * MAX_RULES);

  /********************************************************************
   * RULE 1:
   *
   * Over The Air De-Personalisation - (3GPP TS 22.022)
   *
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.OTADP_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.OTADP_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].Flags.OTADP = 1;

    // Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_COPS;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_NONE;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_NO_STORE;
    RuleCount++;

  /********************************************************************
   * RULE 2:
   * SIM Application Toolkit
   * Check the PID indicates SAT download and DCS is 8-bit class 2 general
   * data coding
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.TPPID_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.TPPID_POSITIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.TPDCS_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.TPDCS_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].TPPID = 0x7F;

    // DCS see 23.040 9.2.3.9
    SMRuleTable[RuleCount].TPDCS = 0x16;

    // Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_SAT;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_NONE;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_NO_STORE;
    RuleCount++;

  /********************************************************************
   * RULE 3:
   * SIM Application Toolkit
   * Check the PID indicates SAT download and DCS is 8-bit class 2 data
   * coding
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.TPPID_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.TPPID_POSITIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.TPDCS_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.TPDCS_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].TPPID = 0x7F;

    // DCS see 23.040 9.2.3.9
    SMRuleTable[RuleCount].TPDCS = 0xF6;

    // Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_SAT;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_NONE;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_NO_STORE;
    RuleCount++;

  /********************************************************************
   * RULE 4:
   *
   * Class 2 Messages
   *
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.Class_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.Class_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].Class = SM_CLASS_2_SIM_SPECIFIC;

    //Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_NORMAL;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_NORMAL;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_SIM_SPECIFIED;
    RuleCount++;

  /********************************************************************
   * RULE 5:
   *
   * Class0 Messages
   *
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.Class_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.Class_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].Class = SM_CLASS_0;

    //Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_NORMAL;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_CLIENT;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_NO_STORE;
    RuleCount++;
#if 0                           //Android shall handle the WAP messages.
  /********************************************************************
   * RULE 6:
   *
   * Test For WAP Messages
   *
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.DEST_PORT_RANGE_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.DEST_PORT_RANGE_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].DestPortRange.StartOfRange = 2948;   // WAP PUSH
    SMRuleTable[RuleCount].DestPortRange.EndOfRange = 2949;     // WAP PUSH SECURE

    //Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_NORMAL;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_CLIENT;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_CONTROL;
    RuleCount++;

  /********************************************************************
   * RULE 7:
   *
   * Test For WAP Messages
   *
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.DEST_PORT_RANGE_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.DEST_PORT_RANGE_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].DestPortRange.StartOfRange = 49996;  // WAP Synch Provisioning
    SMRuleTable[RuleCount].DestPortRange.EndOfRange = 49997;    // WAP provisioning?

    //Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_NORMAL;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_CLIENT;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_CONTROL;
    RuleCount++;

  /********************************************************************
   * RULE 8:
   *
   * Test For WAP Messages
   *
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.DEST_PORT_RANGE_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.DEST_PORT_RANGE_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].DestPortRange.StartOfRange = 49999;  // WAP Provisioning
    SMRuleTable[RuleCount].DestPortRange.EndOfRange = 49999;    // WAP provisioning

    //Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_NORMAL;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_CLIENT;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_CONTROL;
    RuleCount++;

  /********************************************************************
   * RULE 9:
   *
   * Test For WAP Messages
   *
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.CHECK_FOR_IE_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.CHECK_FOR_IE_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].IEType = 0x09;       // WAP CTRL MSG PROTOCOL

    //Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_NORMAL;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_CLIENT;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_CONTROL;
    RuleCount++;
#endif
  /********************************************************************
   * RULE 6:
   *
   * DEFAULT rule - the last rule must be a default rule
   *
   ********************************************************************/
    // Active Rules
    SMRuleTable[RuleCount].ACTIVERULES.DEFAULT_ACTIVE = 1;
    SMRuleTable[RuleCount].ACTIVERULES.DEFAULT_POSITIVE = 1;

    // Values
    SMRuleTable[RuleCount].Flags.DEFAULT = 1;

    //Actions
    SMRuleTable[RuleCount].Flags.CONTINUE = 0;
    SMRuleTable[RuleCount].APPNOTIFY = SHORTMESSAGEROUTER_NOTIFY_NORMAL;
    SMRuleTable[RuleCount].NETACK = SHORTMESSAGEROUTER_NETACK_CLIENT;
    SMRuleTable[RuleCount].STORAGE = SHORTMESSAGEROUTER_STORAGE_NO_STORE;
    RuleCount++;

    return RuleCount;
}
