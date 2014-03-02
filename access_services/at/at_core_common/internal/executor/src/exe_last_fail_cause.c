/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <atc_log.h>

#ifdef EXE_USE_CN_SERVICE
#include "cn_data_types.h"
#endif
#include "exe.h"
#include "exe_internal.h"
#include "exe_last_fail_cause.h"


typedef struct {
    int32_t id;
    char   *text;
} exe_last_fail_cause_table_t;


/* NetWork causes are based on TS24.008 Annex H
 * NetWork strings based on Annex H & equivalent R13 strings.
 */
static const exe_last_fail_cause_table_t exe_last_fail_cause_table_cn_network[] = {
#ifdef EXE_USE_CN_SERVICE
    {0x01, "Exit Cause: CC1 (Unassigned number)"},
    {0x03, "Exit Cause: CC3 (No route to destination)"},
    {0x06, "Exit Cause: CC6 (Channel unacceptable)"},
    {0x08, "Exit Cause: CC8 (Operator determined barring)"},
    {0x10, "Exit Cause: CC16 (Normal call clearing)"},
    {0x11, "Exit Cause: CC17 (User busy)"},
    {0x12, "Exit Cause: CC18 (No user responding)"},
    {0x13, "Exit Cause: CC19 (User alerting no answer)"},
    {0x15, "Exit Cause: CC21 (Call rejected)"},
    {0x16, "Exit Cause: CC22 (Number changed)"},
    {0x1A, "Exit Cause: CC26 (Non selected user clearing)"},
    {0x1B, "Exit Cause: CC27 (Out of order)"},
    {0x1C, "Exit Cause: CC28 (Invalid number format)"},
    {0x1D, "Exit Cause: CC29 (Facility rejected)"},
    {0x1E, "Exit Cause: CC30 (Response to status enquiry)"},
    {0x1F, "Exit Cause: CC31 (Normal unspecified)"},
    {0x22, "Exit Cause: CC34 (No channel available)"},
    {0x26, "Exit Cause: CC38 (Network out of order)"},
    {0x29, "Exit Cause: CC41 (Temporary failure)"},
    {0x2A, "Exit Cause: CC42 (Switching congestion)"},
    {0x2B, "Exit Cause: CC43 (Access info dicarded)"},
    {0x2C, "Exit Cause: CC44 (Requested channel not available)"},
    {0x2F, "Exit Cause: CC47 (Resource unavailable)"},
    {0x31, "Exit Cause: CC49 (QOS unavailable)"},
    {0x32, "Exit Cause: CC50 (Requested facility not subscribed)"},
    {0x37, "Exit Cause: CC55 (Incoming call barred within CUG)"},
    {0x39, "Exit Cause: CC57 (BC not authorized)"},
    {0x3A, "Exit Cause: CC58 (BC not presently available)"},
    {0x3F, "Exit Cause: CC63 (Service or option not available)"},
    {0x41, "Exit Cause: CC65 (Bearer service not implemented)"},
    {0x44, "Exit Cause: CC68 (ACM equal or greater ACMMAX)"},
    {0x45, "Exit Cause: CC69 (Requested facility not implemented)"},
    {0x46, "Exit Cause: CC70 (Only RDI BC available)"},
    {0x4F, "Exit Cause: CC79 (Service or option not implemented)"},
    {0x51, "Exit Cause: CC81 (Invalid TI)"},
    {0x57, "Exit Cause: CC87 (User not member of CUG)"},
    {0x58, "Exit Cause: CC88 (Incompatible destination)"},
    {0x5B, "Exit Cause: CC91 (Invalid transit network selection)"},
    {0x5F, "Exit Cause: CC95 (Semantically incorrect message)"},
    {0x60, "Exit Cause: CC96 (Invalid mandatory information)"},
    {0x61, "Exit Cause: CC97 (MT non existent or not implemented)"},
    {0x62, "Exit Cause: CC98 (MT not compatible with state)"},
    {0x63, "Exit Cause: CC99 (IE non existent or not implemented)"},
    {0x64, "Exit Cause: CC100 (Conditional IE error)"},
    {0x65, "Exit Cause: CC101 (Message not compatible with state)"},
    {0x66, "Exit Cause: CC102 (Recovery on timer expiry)"},
    {0x6F, "Exit Cause: CC111 (Protocol error, unspecified)"},
    {0x7F, "Exit Cause: CC127 (Interworking unspecified)"},
#endif
};

/* Call server causes are based on error codes created by mal and remapped by cn.
 * Text strings are based on error codes description.
 */
static const exe_last_fail_cause_table_t exe_last_fail_cause_table_cn_server[] = {
#ifdef EXE_USE_CN_SERVICE
    {CN_CALL_SERVER_CAUSE_NO_CALL,            "Exit Cause: CS1 (Call does not exist)"},
    {CN_CALL_SERVER_CAUSE_ERROR_REQUEST,      "Exit Cause: CS5 (Call request erroneous)"},
    {CN_CALL_SERVER_CAUSE_CALL_ACTIVE,        "Exit Cause: CS7 (Call is active)"},
    {CN_CALL_SERVER_CAUSE_NO_CALL_ACTIVE,     "Exit Cause: CS8 (Call is not active)"},
    {CN_CALL_SERVER_CAUSE_INVALID_CALL_MODE,  "Exit Cause: CS9 (Call mode is invalid)"},
    {CN_CALL_SERVER_CAUSE_TOO_LONG_ADDRESS,   "Exit Cause: CS11 (Address is too long)"},
    {CN_CALL_SERVER_CAUSE_INVALID_ADDRESS,    "Exit Cause: CS12 (Address is invalid)"},
    {CN_CALL_SERVER_CAUSE_EMERGENCY,          "Exit Cause: CS13 (Emergency call has started)"},
    {CN_CALL_SERVER_CAUSE_NO_SERVICE,         "Exit Cause: CS14 (No service)"},
    {CN_CALL_SERVER_CAUSE_CODE_REQUIRED,      "Exit Cause: CS16 (SIM or PIN rights required)"},
    {CN_CALL_SERVER_CAUSE_NOT_ALLOWED,        "Exit Cause: CS17 (Not allowed)"},
    {CN_CALL_SERVER_CAUSE_DTMF_ERROR,         "Exit Cause: CS18 (DTMF error)"},
    {CN_CALL_SERVER_CAUSE_CHANNEL_LOSS,       "Exit Cause: CS19 (Call re-establishment failed)"},
    {CN_CALL_SERVER_CAUSE_FDN_NOT_OK,         "Exit Cause: CS20 (Fixed Dialing Number check)"},
    {CN_CALL_SERVER_CAUSE_BLACKLIST_BLOCKED,  "Exit Cause: CS22 (Call blocked by blacklist)"},
    {CN_CALL_SERVER_CAUSE_BLACKLIST_DELAYED,  "Exit Cause: CS23 (Call delayed by blacklist)"},
    {CN_CALL_SERVER_CAUSE_EMERGENCY_FAILURE,  "Exit Cause: CS26 (No network for emergency call)"},
    {CN_CALL_SERVER_CAUSE_NO_SIM,             "Exit Cause: CS31 (SIM card not found)"},
    {CN_CALL_SERVER_CAUSE_SIM_LOCK_OPERATIVE, "Exit Cause: CS32 (SIM lock is operative)"},
    {CN_CALL_SERVER_CAUSE_DTMF_SEND_ONGOING,  "Exit Cause: CS36 (DTMF sending is ongoing)"},
    {CN_CALL_SERVER_CAUSE_CS_INACTIVE,        "Exit Cause: CS37 (Radio path not available)"},
    {CN_CALL_SERVER_CAUSE_NOT_READY,          "Exit Cause: CS38 (Startup is ongoing)"},
    {CN_CALL_SERVER_CAUSE_INCOMPATIBLE_DEST,  "Exit Cause: CS39 (Bearer Capability negotiation failed)"},
#endif
};

/* SM Causes as taken from 3GPP 24.008, in Annex I & Chapter "10.5.6.6 SM Cause".
 * Text strings based on Annex I & equivalent R13 strings.
 */
static const exe_last_fail_cause_table_t exe_last_fail_cause_table_net_pscc[] = {
#ifdef EXE_USE_PSCC_SERVICE
    {0x00, "Deactivate Cause: SM00 (GPDS Failure Cause Unknown)"}, /* error generated by Modem Interface */
    {0x02, "Deactivate Cause: SM02 (IMSI unknown in HLR)"}, /* GMM error reported as GPRS */
    {0x03, "Deactivate Cause: SM03 (Illegal MS)"}, /* GMM error reported as GPRS */
    {0x05, "Deactivate Cause: SM05 (IMEI not accepted)"}, /* GMM error reported as GPRS */
    {0x06, "Deactivate Cause: SM06 (Illegal ME)"}, /* GMM error reported as GPRS */
    {0x07, "Deactivate Cause: SM07 (GPRS services not allowed)"}, /* GMM error reported as GPRS */
    {0x08, "Deactivate Cause: SM08 (Operator determined barring)"},
    {0x09, "Deactivate Cause: SM09 (MS identity cannot be derived by the network)"}, /* GMM error reported as GPRS */
    {0x0A, "Deactivate Cause: SM10 (Implicitly detached)"}, /* GMM error reported as GPRS */
    {0x0B, "Deactivate Cause: SM11 (PLMN not allowed)"}, /* GMM error reported as GPRS */
    {0x0C, "Deactivate Cause: SM12 (Location Area not allowed)"}, /* GMM error reported as GPRS */
    {0x0D, "Deactivate Cause: SM13 (Roaming not allowed in this location area)"}, /* GMM error reported as GPRS */
    {0x0E, "Deactivate Cause: SM14 (GPRS services not allowed in this PLMN)"}, /* GMM error reported as GPRS */
    {0x0F, "Deactivate Cause: SM15 (No Suitable Cells In Location Area)"}, /* GMM error reported as GPRS */
    {0x10, "Deactivate Cause: SM16 (MSC temporarily not reachable)"}, /* GMM error reported as GPRS */
    {0x11, "Deactivate Cause: SM17 (Network failure)"}, /* GMM error reported as GPRS */
    {0x14, "Deactivate Cause: SM20 (MAC failure)"}, /* GMM error reported as GPRS */
    {0x15, "Deactivate Cause: SM21 (Synch failure)"}, /* GMM error reported as GPRS */
    {0x16, "Deactivate Cause: SM22 (Congestion)"}, /* GMM error reported as GPRS */
    {0x19, "Deactivate Cause: SM25 (LLC or SNDCP failure)"},
    {0x1A, "Deactivate Cause: SM26 (Insufficient resources)"},
    {0x1B, "Deactivate Cause: SM27 (Missing or unknown APN)"},
    {0x1C, "Deactivate Cause: SM28 (Unknown PDP address or PDP type)"},
    {0x1D, "Deactivate Cause: SM29 (User Authentication failed)"},
    {0x1E, "Deactivate Cause: SM30 (Activation rejected by GGSN)"},
    {0x1F, "Deactivate Cause: SM31 (Activation rejected, unspecified)"},
    {0x20, "Deactivate Cause: SM32 (Service option not supported)"},
    {0x21, "Deactivate Cause: SM33 (Requested service option not subscribed)"},
    {0x22, "Deactivate Cause: SM34 (Service option temporarily out of order)"},
    {0x23, "Deactivate Cause: SM35 (NSAPI already used)"},
    {0x24, "Deactivate Cause: SM36 (Regular deactivation)"},
    {0x25, "Deactivate Cause: SM37 (QoS not accepted)"},
    {0x26, "Deactivate Cause: SM38 (Network failure)"},
    {0x27, "Deactivate Cause: SM39 (Reactivation required)"},
    {0x28, "Deactivate Cause: SM40 (Feature not supported)"},
    {0x29, "Deactivate Cause: SM41 (Semantic error in the TFT operation)"},
    {0x2A, "Deactivate Cause: SM42 (Syntactical error in the TFT operation)"},
    {0x2B, "Deactivate Cause: SM43 (Unknown PDP context)"},
    {0x2C, "Deactivate Cause: SM44 (Semantic errors in packet filter(s) )"},
    {0x2D, "Deactivate Cause: SM44 (Syntactical errors in packet filter(s) )"},
    {0x2E, "Deactivate Cause: SM46 (PDP context without TFT already activated)"},
    {0x2F, "Deactivate Cause: SM47 (Multicast membership timeout)"},
    {0x30, "Deactivate Cause: SM48 (MBMS bearer capabilities insufficient for the service)"},
    {0x31, "Deactivate Cause: SM49 (Activation rejected, BCM violation)"},
    {0x32, "Deactivate Cause: SM50 (PDP type IPv4 only allowed)"},
    {0x33, "Deactivate Cause: SM51 (PDP type IPv6 only allowed)"},
    {0x34, "Deactivate Cause: SM52 (Single address bearers only allowed)"},
    {0x38, "Deactivate Cause: SM56 (Collision with network initiated request)"},
    {0x51, "Deactivate Cause: SM81 (Invalid transaction identifier value)"},
    {0x5F, "Deactivate Cause: SM95 (Semantically incorrect message)"},
    {0x60, "Deactivate Cause: SM96 (Invalid mandatory information)"},
    {0x61, "Deactivate Cause: SM97 (Message type non-existent or not implemented)"},
    {0x62, "Deactivate Cause: SM98 (Message type not compatible with the protocol state)"},
    {0x63, "Deactivate Cause: SM99 (Information element non-existent or not implemented)"},
    {0x64, "Deactivate Cause: SM100 (Conditional IE error)"},
    {0x65, "Deactivate Cause: SM101 (Message not compatible with the protocol state)"},
    {0x6F, "Deactivate Cause: SM111 (Protocol error, unspecified)"},
    {0x70, "Deactivate Cause: SM112 (APN restriction value incompatible with active PDP context)"} /* Length 85 bytes close to EXE_CEER_MAX_CHAR = 90 */
#endif
};

/* GMM causes are taken from 3GPP 24.008 Annex G  Chapter "10.5.5.14 GMM Cause".
 * Text strings based on Annex G & equivalent R13 strings.
 */
static const exe_last_fail_cause_table_t exe_last_fail_cause_table_net_gsm[] = {
#ifdef EXE_USE_CN_SERVICE
    {0x02, "Detach Cause: GMM02 (IMSI unknown in HLR)"},
    {0x03, "Detach Cause: GMM03 (Illegal MS)"},
    {0x04, "Detach Cause: GMM04 (IMSI unknown in VLR)"},
    {0x05, "Detach Cause: GMM05 (IMEI not accepted)"},
    {0x06, "Detach Cause: GMM06 (Illegal ME)"},
    {0x07, "Detach Cause: GMM07 (GPRS services not allowed)"},
    {0x08, "Detach Cause: GMM08 (GPRS services and non-GPRS services not allowed)"},
    {0x09, "Detach Cause: GMM09 (MS identity cannot be derived by the network)"},
    {0x0A, "Detach Cause: GMM10 (Implicitly detached)"},
    {0x0B, "Detach Cause: GMM11 (PLMN not allowed)"},
    {0x0C, "Detach Cause: GMM12 (Location Area not allowed)"},
    {0x0D, "Detach Cause: GMM13 (Roaming not allowed in this location area)"},
    {0x0E, "Detach Cause: GMM14 (GPRS services not allowed in this PLMN)"},
    {0x0F, "Detach Cause: GMM15 (No Suitable Cells In Location Area)"},
    {0x10, "Detach Cause: GMM16 (MSC temporarily not reachable)"},
    {0x11, "Detach Cause: GMM17 (Network failure)"},
    {0x14, "Detach Cause: GMM20 (MAC failure)"},
    {0x15, "Detach Cause: GMM21 (Synch failure)"},
    {0x16, "Detach Cause: GMM22 (Congestion)"},
    {0x17, "Detach Cause: GMM23 (GSM authentication unacceptable)"},
    {0x20, "Detach Cause: GMM32 (Service option not supported)"},
    {0x21, "Detach Cause: GMM33 (Requested service option not subscribed)"},
    {0x22, "Detach Cause: GMM34 (Service option temporarily out of order)"},
    {0x28, "Detach Cause: GMM40 (No PDP context activated)"},
    {0x30, "Detach Cause: GMM48 (Retry upon entry into a new cell)"},
    {0x31, "Detach Cause: GMM49 (Retry upon entry into a new cell)"},
    {0x32, "Detach Cause: GMM50 (Retry upon entry into a new cell)"},
    {0x33, "Detach Cause: GMM51 (Retry upon entry into a new cell)"},
    {0x34, "Detach Cause: GMM52 (Retry upon entry into a new cell)"},
    {0x35, "Detach Cause: GMM53 (Retry upon entry into a new cell)"},
    {0x36, "Detach Cause: GMM54 (Retry upon entry into a new cell)"},
    {0x37, "Detach Cause: GMM55 (Retry upon entry into a new cell)"},
    {0x38, "Detach Cause: GMM56 (Retry upon entry into a new cell)"},
    {0x39, "Detach Cause: GMM57 (Retry upon entry into a new cell)"},
    {0x3A, "Detach Cause: GMM58 (Retry upon entry into a new cell)"},
    {0x3B, "Detach Cause: GMM59 (Retry upon entry into a new cell)"},
    {0x3C, "Detach Cause: GMM60 (Retry upon entry into a new cell)"},
    {0x3D, "Detach Cause: GMM61 (Retry upon entry into a new cell)"},
    {0x3E, "Detach Cause: GMM62 (Retry upon entry into a new cell)"},
    {0x3F, "Detach Cause: GMM63 (Retry upon entry into a new cell)"},
    {0x5F, "Detach Cause: GMM95 (Semantically incorrect message)"},
    {0x60, "Detach Cause: GMM96 (Invalid mandatory information)"},
    {0x61, "Detach Cause: GMM97 (Message type non-existent or not implemented)"},
    {0x64, "Detach Cause: GMM100 (Conditional IE error)"},
    {0x65, "Detach Cause: GMM101 (Message not compatible with the protocol state)"},
    {0x6F, "Detach Cause: GMM111 (Protocol error, unspecified)"},
#endif
};

/********************************************************************
 * Private prototypes
 ********************************************************************
 */

static void exe_last_fail_cause_get_string_cn_network(const int32_t cause, char *output_p);
static void exe_last_fail_cause_get_string_cn_server(const int32_t cause, char *output_p);
static void exe_last_fail_cause_get_string_net_pscc(const int32_t cause, char *output_p);
static void exe_last_fail_cause_get_string_net_gsm(const int32_t cause, char *output_p);
static void exe_last_fail_cause_get_string_net_ss(const int32_t cause, char *output_p);

/********************************************************************
 * Private methods
 ********************************************************************
 */

static void exe_last_fail_cause_get_string_cn_network(const int32_t cause, char *output_p)
{
    int table_size = sizeof(exe_last_fail_cause_table_cn_network) / sizeof(exe_last_fail_cause_table_t);
    int index = 0;

    for (index = 0; index < table_size; index++) {
        if (cause == exe_last_fail_cause_table_cn_network[index].id) {
            break;
        }
    }

    if (index < table_size) {
        sprintf(output_p, "%s", exe_last_fail_cause_table_cn_network[index].text);
    } else {
        sprintf(output_p, "Exit Cause: CC%d (Unspecified)", cause);
        ATC_LOG_E("%s : Unspecified CC cause, %d\n", __FUNCTION__, cause);
    }
}


static void exe_last_fail_cause_get_string_cn_server(const int32_t cause, char *output_p)
{
    int table_size = sizeof(exe_last_fail_cause_table_cn_server) / sizeof(exe_last_fail_cause_table_t);
    int index = 0;

    for (index = 0; index < table_size; index++) {
        if (cause == exe_last_fail_cause_table_cn_server[index].id) {
            break;
        }
    }

    if (index < table_size) {
        sprintf(output_p, "%s", exe_last_fail_cause_table_cn_server[index].text);
    } else {
        sprintf(output_p, "Exit Cause: CS%d (Unspecified)", cause);
        ATC_LOG_E("%s : Unspecified CS cause, %d\n", __FUNCTION__, cause);
    }
}


static void exe_last_fail_cause_get_string_net_pscc(const int32_t cause, char *output_p)
{
    int table_size = sizeof(exe_last_fail_cause_table_net_pscc) / sizeof(exe_last_fail_cause_table_t);
    int index = 0;

    for (index = 0; index < table_size; index++) {
        if (cause == exe_last_fail_cause_table_net_pscc[index].id) {
            break;
        }
    }

    if (index < table_size) {
        sprintf(output_p, "%s", exe_last_fail_cause_table_net_pscc[index].text);
    } else {
        sprintf(output_p, "Deactivate Cause: SM%d (Unspecified)", cause);
        ATC_LOG_E("%s : Unspecified SM cause, %d\n", __FUNCTION__, cause);
    }
}


static void exe_last_fail_cause_get_string_net_gsm(const int32_t cause, char *output_p)
{
    int table_size = sizeof(exe_last_fail_cause_table_net_gsm) / sizeof(exe_last_fail_cause_table_t);
    int index = 0;

    for (index = 0; index < table_size; index++) {
        if (cause == exe_last_fail_cause_table_net_gsm[index].id) {
            break;
        }
    }

    if (index < table_size) {
        sprintf(output_p, "%s", exe_last_fail_cause_table_net_gsm[index].text);
    } else {
        sprintf(output_p, "Detach Cause: GMM%d (Unspecified)", cause);
        ATC_LOG_E("%s : Unspecified GMM cause, %d\n", __FUNCTION__, cause);
    }
}


static void exe_last_fail_cause_get_string_net_ss(const int32_t cause, char *output_p)
{
    switch (cause) {
    default:
        ATC_LOG_I("%s: Fail cause class not supported", __FUNCTION__);
        break;
    }
}


/********************************************************************
 * Public methods
 ********************************************************************
 */

void exe_last_fail_cause_init(exe_last_fail_cause_t *last_fail_p)
{
    last_fail_p->class = EXE_LAST_FAIL_CAUSE_CLASS_NONE;
    last_fail_p->cause = 0;
}

void exe_last_fail_cause_set(exe_last_fail_cause_t *last_fail_info_p, exe_last_fail_cause_t *last_fail_stored_p)
{
    if (NULL == last_fail_info_p) {
        ATC_LOG_E("%s: error, last_fail_info_p is NULL", __FUNCTION__);
        return;
    }

    /* TODO Currently we do not support SS errors, so don't update in order to remain latest supported failure cause. */
    if (EXE_LAST_FAIL_CAUSE_CLASS_NET_SS != last_fail_info_p->class) {
        last_fail_stored_p->class = last_fail_info_p->class;
        last_fail_stored_p->cause = last_fail_info_p->cause;
    }

    if (EXE_LAST_FAIL_CAUSE_CLASS_NET_GSM == last_fail_info_p->class) {
        int table_size = sizeof(exe_last_fail_cause_table_net_gsm) / sizeof(exe_last_fail_cause_table_t);
        int index = 0;
        exe_ecme_response_t ecme;

        /* Send unsolicited reject cause */
        for (index = 0; index < table_size; index++) {
            if (last_fail_info_p->cause == exe_last_fail_cause_table_net_gsm[index].id) {
                break;
            }
        }

        if (index < table_size) {
            ecme.cause_text_p = exe_last_fail_cause_table_net_gsm[index].text;
        } else {
            ecme.cause_text_p = "Unspecified reject fault cause";
        }

        ecme.cause = last_fail_info_p->cause;
        exe_event(EXE_UNSOLICITED_ECME, &ecme);
    }
}

bool exe_last_fail_cause_get(exe_last_fail_cause_t *last_fail_p, char *error_report_p)
{
    if (NULL == error_report_p) {
        ATC_LOG_E("%s : no output buffer specified", __FUNCTION__);
        return false;
    }

    switch (last_fail_p->class) {
    case EXE_LAST_FAIL_CAUSE_CLASS_CN_NETWORK:
        exe_last_fail_cause_get_string_cn_network(last_fail_p->cause, error_report_p);
        break;

    case EXE_LAST_FAIL_CAUSE_CLASS_CN_SERVER:
        exe_last_fail_cause_get_string_cn_server(last_fail_p->cause, error_report_p);
        break;

    case EXE_LAST_FAIL_CAUSE_CLASS_NET_GSM:
        exe_last_fail_cause_get_string_net_gsm(last_fail_p->cause, error_report_p);
        break;

    case EXE_LAST_FAIL_CAUSE_CLASS_NET_SS:
        exe_last_fail_cause_get_string_net_ss(last_fail_p->cause, error_report_p);
        break;

    case EXE_LAST_FAIL_CAUSE_CLASS_NET_PSCC:
        exe_last_fail_cause_get_string_net_pscc(last_fail_p->cause, error_report_p);
        break;

    case EXE_LAST_FAIL_CAUSE_CLASS_NONE:
        strncpy(error_report_p, "", 1); /* nothing logged since initialized, outputs "+CEER: " */
        break;

    default:
        break;
    }

    return true;
}

