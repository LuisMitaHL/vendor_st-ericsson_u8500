/*
 * Copyright (C) ST-Ericsson SA 2010-2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Type assertions to enable assignment operations for 1:1 mapping between CN and MAL.
 */

#ifndef __cn_mal_assertions_h__
#define __cn_mal_assertions_h__ (1)

#include "cn_data_types.h"
#include "cn_pdc_ecc_list_handling.h"
#ifdef USE_MAL_MCE
#include "mal_mce.h"
#endif /* USE_MAL_MCE */

#ifdef USE_MAL_NET
#include "mal_net.h"
#endif /* USE_MAL_NET */

#ifdef USE_MAL_CS
#include "mal_call.h"
#endif /* USE_MAL_CS */

#ifdef USE_MAL_RF
#include "mal_rf.h"
#endif /* USE_MAL_RF */


#ifdef USE_MAL_MCE
/* cn_modem_status_t == mal_mce_status */
C_ASSERT(CN_MODEM_STATUS_OK                 == MAL_MCE_OK,                 CN_MODEM_STATUS_OK);
C_ASSERT(CN_MODEM_STATUS_REQ_FAIL           == MAL_MCE_REQ_FAIL,           CN_MODEM_STATUS_REQ_FAIL);
C_ASSERT(CN_MODEM_STATUS_NOT_ALLOWED        == MAL_MCE_NOT_ALLOWED,        CN_MODEM_STATUS_NOT_ALLOWED);
C_ASSERT(CN_MODEM_STATUS_ALREADY_ACTIVE     == MAL_MCE_ALREADY_ACTIVE,     CN_MODEM_STATUS_ALREADY_ACTIVE);
C_ASSERT(CN_MODEM_STATUS_TRANSITION_ONGOING == MAL_MCE_TRANSITION_ONGOING, CN_MODEM_STATUS_TRANSITION_ONGOING);
C_ASSERT(CN_MODEM_STATUS_RESET_REQUIRED     == MAL_MCE_RESET_REQUIRED,     CN_MODEM_STATUS_RESET_REQUIRED);

/* cn_rf_status_t == mal_mce_rf_status */
C_ASSERT(CN_RF_OFF        == MAL_MCE_RF_OFF,        CN_RF_OFF);
C_ASSERT(CN_RF_ON         == MAL_MCE_RF_ON,         CN_RF_ON);
C_ASSERT(CN_RF_TRANSITION == MAL_MCE_RF_TRANSITION, CN_RF_TRANSITION);
#endif /* USE_MAL_MCE */

#ifdef USE_MAL_GSS
/* cn_network_type_t == mal_gss_network_type */
C_ASSERT(CN_NETWORK_TYPE_GSM_WCDMA_PREFERRED_WCDMA     == MAL_GSS_GSM_WCDMA_PREFERRED_WCDMA,     CN_NETWORK_TYPE_GSM_WCDMA_PREFERRED_WCDMA);
C_ASSERT(CN_NETWORK_TYPE_GSM_ONLY                      == MAL_GSS_GSM_ONLY,                      CN_NETWORK_TYPE_GSM_ONLY);
C_ASSERT(CN_NETWORK_TYPE_WCDMA_ONLY                    == MAL_GSS_WCDMA_ONLY,                    CN_NETWORK_TYPE_WCDMA_ONLY);
C_ASSERT(CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE           == MAL_GSS_GSM_WCDMA_AUTO_MODE,           CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE);
C_ASSERT(CN_NETWORK_TYPE_CDMA_EVDO_AUTO_MODE           == MAL_GSS_CDMA_EVDO_AUTO_MODE,           CN_NETWORK_TYPE_CDMA_EVDO_AUTO_MODE);
C_ASSERT(CN_NETWORK_TYPE_CDMA_ONLY                     == MAL_GSS_CDMA_ONLY,                     CN_NETWORK_TYPE_CDMA_ONLY);
C_ASSERT(CN_NETWORK_TYPE_EVDO_ONLY                     == MAL_GSS_EVDO_ONLY,                     CN_NETWORK_TYPE_EVDO_ONLY);
C_ASSERT(CN_NETWORK_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO_MODE == MAL_GSS_GSM_WCDMA_CDMA_EVDO_AUTO_MODE, CN_NETWORK_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO_MODE);

/* cn_user_status_t == mal_gss_user_activity_mode */
C_ASSERT(CN_USER_STATUS_UNKNOWN  == MAL_GSS_USER_STATUS_UNKNOWN, CN_USER_STATUS_UNKNOWN);
C_ASSERT(CN_USER_STATUS_ACTIVE   == MAL_GSS_USER_ACTIVE,         CN_USER_STATUS_ACTIVE);
C_ASSERT(CN_USER_STATUS_INACTIVE == MAL_GSS_USER_INACTIVE,       CN_USER_STATUS_INACTIVE);

/* cn_battery_status_t == mal_gss_battery_info */
C_ASSERT(CN_BATTERY_STATUS_UNKNOWN      == MAL_GSS_BATTERY_INFO_UNKNOWN,    CN_BATTERY_STATUS_UNKNOWN);
C_ASSERT(CN_BATTERY_STATUS_IN_CHARGER   == MAL_GSS_BATTERY_INFO_IN_CHARGER, CN_BATTERY_STATUS_IN_CHARGER);
C_ASSERT(CN_BATTERY_STATUS_NORMAL       == MAL_GSS_BATTERY_INFO_NORMAL,     CN_BATTERY_STATUS_NORMAL);
C_ASSERT(CN_BATTERY_STATUS_LOW          == MAL_GSS_BATTERY_INFO_LOW,        CN_BATTERY_STATUS_LOW);

/* cn_timing_advance_validity_t == mal_gss_timing_advance_validity */
C_ASSERT(CN_TIMING_ADVANCE_NOT_VALID == MAL_GSS_TIMING_ADVANCE_NOT_VALID,   CN_TIMING_ADVANCE_NOT_VALID);
C_ASSERT(CN_GSS_TIMING_ADVANCE_VALID == MAL_GSS_TIMING_ADVANCE_VALID,       CN_GSS_TIMING_ADVANCE_VALID);

/* cn_hsxpa_mode_t == mal_gss_hsxpa_mode */
C_ASSERT(CN_HSXPA_DISABLE       == MAL_GSS_HSXPA_DISABLE,       CN_HSXPA_DISABLE);
C_ASSERT(CN_HSXPA_ENABLE        == MAL_GSS_HSXPA_ENABLE,        CN_HSXPA_ENABLE);
#endif /* USE_MAL_GSS */

#ifdef USE_MAL_NET
/* cn_rat_type_t == mal_net_rat */
C_ASSERT(CN_RAT_TYPE_UNKNOWN    == MAL_NET_RAT_UNKNOWN,    CN_RAT_TYPE_UNKNOWN);
C_ASSERT(CN_RAT_TYPE_GPRS       == MAL_NET_RAT_GPRS,       CN_RAT_TYPE_GPRS);
C_ASSERT(CN_RAT_TYPE_EDGE       == MAL_NET_RAT_EDGE,       CN_RAT_TYPE_EDGE);
C_ASSERT(CN_RAT_TYPE_UMTS       == MAL_NET_RAT_UMTS,       CN_RAT_TYPE_UMTS);
C_ASSERT(CN_RAT_TYPE_IS95A      == MAL_NET_RAT_IS95A,      CN_RAT_TYPE_IS95A);
C_ASSERT(CN_RAT_TYPE_IS95B      == MAL_NET_RAT_IS95B,      CN_RAT_TYPE_IS95B);
C_ASSERT(CN_RAT_TYPE_1xRTT      == MAL_NET_RAT_1xRTT,      CN_RAT_TYPE_1xRTT);
C_ASSERT(CN_RAT_TYPE_EVDO_REV_0 == MAL_NET_RAT_EVDO_REV_0, CN_RAT_TYPE_EVDO_REV_0);
C_ASSERT(CN_RAT_TYPE_EVDO_REV_A == MAL_NET_RAT_EVDO_REV_A, CN_RAT_TYPE_EVDO_REV_A);
C_ASSERT(CN_RAT_TYPE_HSDPA      == MAL_NET_RAT_HSDPA,      CN_RAT_TYPE_HSDPA);
C_ASSERT(CN_RAT_TYPE_HSUPA      == MAL_NET_RAT_HSUPA,      CN_RAT_TYPE_HSUPA);
C_ASSERT(CN_RAT_TYPE_HSPA       == MAL_NET_RAT_HSPA,       CN_RAT_TYPE_HSPA);
C_ASSERT(CN_RAT_TYPE_GSM        == MAL_NET_RAT_GSM,        CN_RAT_TYPE_GSM);
C_ASSERT(CN_RAT_TYPE_EPS        == MAL_NET_RAT_EPS,        CN_RAT_TYPE_EPS);
C_ASSERT(CN_RAT_TYPE_GPRS_DTM   == MAL_NET_RAT_GPRS_DTM,   CN_RAT_TYPE_GPRS_DTM);
C_ASSERT(CN_RAT_TYPE_EDGE_DTM   == MAL_NET_RAT_EDGE_DTM,   CN_RAT_TYPE_EDGE_DTM);

/* cn_neighbour_rat_type_t ==  mal_net_neighbour_rat_type */
C_ASSERT(CN_NEIGHBOUR_RAT_TYPE_UNKNOWN == MAL_NET_NEIGHBOUR_RAT_UNKNOWN, CN_NEIGHBOUR_RAT_TYPE_UNKNOWN);
C_ASSERT(CN_NEIGHBOUR_RAT_TYPE_2G      == MAL_NET_NEIGHBOUR_RAT_2G,      CN_NEIGHBOUR_RAT_TYPE_2G);
C_ASSERT(CN_NEIGHBOUR_RAT_TYPE_3G      == MAL_NET_NEIGHBOUR_RAT_3G,      CN_NEIGHBOUR_RAT_TYPE_3G);

/* cn_network_selection_mode_t == mal_net_selection_mode */
C_ASSERT(CN_NETWORK_SELECTION_MODE_UNKNOWN          == MAL_NET_SELECT_MODE_UNKNOWN,          CN_NETWORK_SELECTION_MODE_UNKNOWN);
C_ASSERT(CN_NETWORK_SELECTION_MODE_MANUAL           == MAL_NET_SELECT_MODE_MANUAL,           CN_NETWORK_SELECTION_MODE_MANUAL);
C_ASSERT(CN_NETWORK_SELECTION_MODE_AUTOMATIC        == MAL_NET_SELECT_MODE_AUTOMATIC,        CN_NETWORK_SELECTION_MODE_AUTOMATIC);
C_ASSERT(CN_NETWORK_SELECTION_MODE_USER_RESELECTION == MAL_NET_SELECT_MODE_USER_RESELECTION, CN_NETWORK_SELECTION_MODE_USER_RESELECTION);
C_ASSERT(CN_NETWORK_SELECTION_MODE_NO_SELECTION     == MAL_NET_SELECT_MODE_NO_SELECTION,     CN_NETWORK_SELECTION_MODE_NO_SELECTION);

/* cn_modem_reg_status_t == mal_net_selection_mode */
C_ASSERT(CN_NOT_REG_NOT_SEARCHING == MAL_NET_NOT_REG_NOT_SEARCHING,   CN_NOT_REG_NOT_SEARCHING);
C_ASSERT(CN_REGISTERED_TO_HOME_NW == MAL_NET_REGISTERED_TO_HOME_NW,   CN_REGISTERED_TO_HOME_NW);
C_ASSERT(CN_NOT_REG_SEARCHING_OP  == MAL_NET_NOT_REG_SEARCHING_OP,    CN_NOT_REG_SEARCHING_OP);
C_ASSERT(CN_REGISTRATION_DENIED   == MAL_NET_REGISTRATION_DENIED,     CN_REGISTRATION_DENIED);
C_ASSERT(CN_REGISTRATION_UNKNOWN  == MAL_NET_REGISTRATION_UNKNOWN,    CN_REGISTRATION_UNKNOWN);
C_ASSERT(CN_REGISTERED_ROAMING    == MAL_NET_REGISTERED_ROAMING,      CN_REGISTERED_ROAMING);
C_ASSERT(CN_NOT_REG_NOT_SEARCHING_EMERGENCY_CALLS_ALLOWED == MAL_NET_NOT_REG_NOT_SEARCHING_EMERGENCY_CALLS_ALLOWED, CN_NOT_REG_NOT_SEARCHING_EMERGENCY_CALLS_ALLOWED);
C_ASSERT(CN_NOT_REG_SEARCHING_OP_EMERGENCY_CALLS_ALLOWED  == MAL_NET_NOT_REG_SEARCHING_OP_EMERGENCY_CALLS_ALLOWED,  CN_NOT_REG_SEARCHING_OP_EMERGENCY_CALLS_ALLOWED);
C_ASSERT(CN_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED   == MAL_NET_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED,   CN_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED);
C_ASSERT(CN_REGISTRATION_UNKNOWN_EMERGENCY_CALLS_ALLOWED  == MAL_NET_REGISTRATION_UNKNOWN_EMERGENCY_CALLS_ALLOWED,  CN_REGISTRATION_UNKNOWN_EMERGENCY_CALLS_ALLOWED);

/* cn_event_reporting_type_t == mal_net_ind_type */
C_ASSERT(CN_MODEM_REGISTRATION_STATUS         == MAL_NET_SELECT_MODEM_REG_STATUS_IND, CN_MODEM_REGISTRATION_STATUS);
C_ASSERT(CN_EVENT_REPORTING_TYPE_TIME         == MAL_NET_SELECT_TIME_IND,             CN_EVENT_REPORTING_TYPE_TIME);
C_ASSERT(CN_EVENT_REPORTING_TYPE_RAB_STATUS   == MAL_NET_SELECT_RADIO_INFO_IND,       CN_EVENT_REPORTING_TYPE_RAB_STATUS);
C_ASSERT(CN_EVENT_REPORTING_TYPE_NETWORK_INFO == MAL_NET_SELECT_NITZ_NAME_IND,        CN_EVENT_REPORTING_TYPE_NETWORK_INFO);

/* cn_network_access_config_t == mal_net_nw_access_conf_state */
C_ASSERT(CN_NETWORK_ACCESS_CONFIG_DISABLE == MAL_NET_CONF_DISABLE, CN_NETWORK_ACCESS_CONFIG_DISABLE);
C_ASSERT(CN_NETWORK_ACCESS_CONFIG_ENABLE == MAL_NET_CONF_ENABLE, CN_NETWORK_ACCESS_CONFIG_ENABLE);

/* cn_rab_status_t == mal_net_rab_status_type */
C_ASSERT(CN_RAB_STATUS_NEITHER_HSDPA_HSUPA == MAL_NET_NEITHER_HSDPA_HSUPA, CN_RAB_STATUS_NEITHER_HSDPA_HSUPA);
C_ASSERT(CN_RAB_STATUS_HSDPA               == MAL_NET_HSDPA,               CN_RAB_STATUS_HSDPA);
C_ASSERT(CN_RAB_STATUS_HSUPA               == MAL_NET_HSUPA,               CN_RAB_STATUS_HSUPA);
C_ASSERT(CN_RAB_STATUS_BOTH_HSDPA_HSUPA    == MAL_NET_BOTH_HSDPA_HSUPA,    CN_RAB_STATUS_BOTH_HSDPA_HSUPA);

C_ASSERT(CN_RSSI_CONF_MAX_SEGMENTS == NET_RSSI_CONF_MAX_SEGMENTS,     CN_RSSI_CONF_MAX_SEGMENTS);

/* cn_rate_name_t == mal_net_rat_name */
C_ASSERT(CN_RAT_NAME_UNKNOWN == MAL_NET_RAT_NAME_UNKNOWN, CN_RAT_NAME_UNKNOWN);
C_ASSERT(CN_RAT_NAME_GSM     == MAL_NET_RAT_NAME_GSM,     CN_RAT_NAME_GSM);
C_ASSERT(CN_RAT_NAME_UMTS    == MAL_NET_RAT_NAME_UMTS,    CN_RAT_NAME_UMTS);
C_ASSERT(CN_RAT_NAME_EPS     == MAL_NET_RAT_NAME_EPS,     CN_RAT_NAME_EPS);

/* cn_nw_service_status_t == mal_net_nw_service_status */
C_ASSERT(CN_NW_SERVICE                 == MAL_NET_SERVICE,                 CN_NW_SERVICE);
C_ASSERT(CN_NW_LIMITED_SERVICE         == MAL_NET_LIMITED_SERVICE,         CN_NW_LIMITED_SERVICE);
C_ASSERT(CN_NW_NO_COVERAGE             == MAL_NET_NO_COVERAGE,             CN_NW_NO_COVERAGE);
C_ASSERT(CN_NW_LIMITED_SERVICE_WAITING == MAL_NET_LIMITED_SERVICE_WAITING, CN_NW_LIMITED_SERVICE_WAITING);
#endif /* USE_MAL_NET */

#ifdef USE_MAL_CS
/* cn_call_cause_type_sender_t == call_cause_type_sender (mal) */
C_ASSERT(CN_CALL_CAUSE_TYPE_SENDER_NONE    == CALL_CAUSE_NONE,         CN_CALL_CAUSE_TYPE_SENDER_NONE);
C_ASSERT(CN_CALL_CAUSE_TYPE_SENDER_SERVER  == CALL_CAUSE_TYPE_SERVER,  CN_CALL_CAUSE_TYPE_SENDER_SERVER);
C_ASSERT(CN_CALL_CAUSE_TYPE_SENDER_NETWORK == CALL_CAUSE_TYPE_NETWORK, CN_CALL_CAUSE_TYPE_SENDER_NETWORK);

/* cn_call_server_cause_t == "Call Server cause value" (mal_call.h) */
C_ASSERT(CN_CALL_SERVER_CAUSE_NO_CALL            == MAL_CALL_MODEM_CAUSE_NO_CALL,            CN_CALL_SERVER_CAUSE_NO_CALL);
C_ASSERT(CN_CALL_SERVER_CAUSE_ERROR_REQUEST      == MAL_CALL_MODEM_CAUSE_ERROR_REQUEST,      CN_CALL_SERVER_CAUSE_ERROR_REQUEST);
C_ASSERT(CN_CALL_SERVER_CAUSE_CALL_ACTIVE        == MAL_CALL_MODEM_CAUSE_CALL_ACTIVE,        CN_CALL_SERVER_CAUSE_CALL_ACTIVE);
C_ASSERT(CN_CALL_SERVER_CAUSE_NO_CALL_ACTIVE     == MAL_CALL_MODEM_CAUSE_NO_CALL_ACTIVE,     CN_CALL_SERVER_CAUSE_NO_CALL_ACTIVE);
C_ASSERT(CN_CALL_SERVER_CAUSE_INVALID_CALL_MODE  == MAL_CALL_MODEM_CAUSE_INVALID_CALL_MODE,  CN_CALL_SERVER_CAUSE_INVALID_CALL_MODE);
C_ASSERT(CN_CALL_SERVER_CAUSE_TOO_LONG_ADDRESS   == MAL_CALL_MODEM_CAUSE_TOO_LONG_ADDRESS,   CN_CALL_SERVER_CAUSE_TOO_LONG_ADDRESS);
C_ASSERT(CN_CALL_SERVER_CAUSE_INVALID_ADDRESS    == MAL_CALL_MODEM_CAUSE_INVALID_ADDRESS,    CN_CALL_SERVER_CAUSE_INVALID_ADDRESS);
C_ASSERT(CN_CALL_SERVER_CAUSE_EMERGENCY          == MAL_CALL_MODEM_CAUSE_EMERGENCY,          CN_CALL_SERVER_CAUSE_EMERGENCY);
C_ASSERT(CN_CALL_SERVER_CAUSE_NO_SERVICE         == MAL_CALL_MODEM_CAUSE_NO_SERVICE,         CN_CALL_SERVER_CAUSE_DTMF_ERROR);
C_ASSERT(CN_CALL_SERVER_CAUSE_CHANNEL_LOSS       == MAL_CALL_MODEM_CAUSE_CHANNEL_LOSS,       CN_CALL_SERVER_CAUSE_CHANNEL_LOSS);
C_ASSERT(CN_CALL_SERVER_CAUSE_FDN_NOT_OK         == MAL_CALL_MODEM_CAUSE_FDN_NOT_OK,         CN_CALL_SERVER_CAUSE_FDN_NOT_OK);
C_ASSERT(CN_CALL_SERVER_CAUSE_BLACKLIST_BLOCKED  == MAL_CALL_MODEM_CAUSE_BLACKLIST_BLOCKED,  CN_CALL_SERVER_CAUSE_BLACKLIST_BLOCKED);
C_ASSERT(CN_CALL_SERVER_CAUSE_BLACKLIST_DELAYED  == MAL_CALL_MODEM_CAUSE_BLACKLIST_DELAYED,  CN_CALL_SERVER_CAUSE_BLACKLIST_DELAYED);
C_ASSERT(CN_CALL_SERVER_CAUSE_EMERGENCY_FAILURE  == MAL_CALL_MODEM_CAUSE_EMERGENCY_FAILURE,  CN_CALL_SERVER_CAUSE_EMERGENCY_FAILURE);
C_ASSERT(CN_CALL_SERVER_CAUSE_NO_SIM             == MAL_CALL_MODEM_CAUSE_NO_SIM,             CN_CALL_SERVER_CAUSE_NO_SIM);
C_ASSERT(CN_CALL_SERVER_CAUSE_SIM_LOCK_OPERATIVE == MAL_CALL_MODEM_CAUSE_SIM_LOCK_OPERATIVE, CN_CALL_SERVER_CAUSE_SIM_LOCK_OPERATIVE);
C_ASSERT(CN_CALL_SERVER_CAUSE_DTMF_SEND_ONGOING  == MAL_CALL_MODEM_CAUSE_DTMF_SEND_ONGOING,  CN_CALL_SERVER_CAUSE_DTMF_SEND_ONGOING);
C_ASSERT(CN_CALL_SERVER_CAUSE_CS_INACTIVE        == MAL_CALL_MODEM_CAUSE_CS_INACTIVE,        CN_CALL_SERVER_CAUSE_CS_INACTIVE);
C_ASSERT(CN_CALL_SERVER_CAUSE_NOT_READY          == MAL_CALL_MODEM_CAUSE_NOT_READY,          CN_CALL_SERVER_CAUSE_NOT_READY_NOT_READY);
C_ASSERT(CN_CALL_SERVER_CAUSE_INCOMPATIBLE_DEST  == MAL_CALL_MODEM_CAUSE_INCOMPATIBLE_DEST,  CN_CALL_SERVER_CAUSE_INCOMPATIBLE_DEST);

/* cn_net_ss_cause_t == "SS_ERROR_TYPE_MISC" (mal_ss.h) */
C_ASSERT(CN_NET_SS_CAUSE_SERVICE_REQUEST_RELEASED == MAL_SS_SERVICE_REQUEST_RELEASED, CN_NET_SS_CAUSE_SERVICE_REQUEST_RELEASED);
C_ASSERT(CN_NET_SS_CAUSE_SERVICE_UNKNOWN_ERROR    == MAL_SS_SERVICE_UNKNOWN_ERROR,    CN_NET_SS_CAUSE_SERVICE_UNKNOWN_ERROR);

/* cn_clir_status_t == mal_ss_clir_state */
C_ASSERT(CN_CLIR_STATUS_NOT_PROVISIONED           == CLIR_NOT_PROVISIONED,           CN_CLIR_STATUS_NOT_PROVISIONED);
C_ASSERT(CN_CLIR_STATUS_PROVISIONED_PERMANENT     == CLIR_PROVISIONED_PERMANENT,     CN_CLIR_STATUS_PROVISIONED_PERMANENT);
C_ASSERT(CN_CLIR_STATUS_UNKNOWN                   == CLIR_UNKNOWN,                   CN_CLIR_STATUS_UNKNOWN);
C_ASSERT(CN_CLIR_STATUS_TEMPARORY_MODE_RESTRICTED == CLIR_TEMPARORY_MODE_RESTRICTED, CN_CLIR_STATUS_TEMPARORY_MODE_RESTRICTED);
C_ASSERT(CN_CLIR_STATUS_TEMPARORY_MODE_ALLOWED    == CLIR_TEMPARORY_MODE_ALLOWED,    CN_CLIR_STATUS_TEMPARORY_MODE_ALLOWED);

/* cn_modem_property_t == mal_call_modem_property */
C_ASSERT(CN_MODEM_PROPERTY_TYPE_CTM     == CALL_MODEM_PROPERTY_CTM,     CN_MODEM_PROPERTY_TYPE_CTM);
C_ASSERT(CN_MODEM_PROPERTY_TYPE_CLIR    == CALL_MODEM_PROPERTY_CLIR,    CN_MODEM_PROPERTY_TYPE_CLIR);
C_ASSERT(CN_MODEM_PROPERTY_TYPE_ALS     == CALL_MODEM_PROPERTY_ALS,     CN_MODEM_PROPERTY_TYPE_ALS);
C_ASSERT(CN_MODEM_PROPERTY_TYPE_UNKNOWN == CALL_MODEM_PROPERTY_UNKNOWN, CN_MODEM_PROPERTY_TYPE_UNKNOWN);

/* cn_cellular_system_command_t == mal_net_cs_request_type */
C_ASSERT(CN_CELLULAR_SYSTEM_COMMAND_POWER_OFF       == MAL_NET_CS_POWER_OFF,  CN_CELLULAR_SYSTEM_COMMAND_POWER_OFF);
C_ASSERT(CN_CELLULAR_SYSTEM_COMMAND_POWER_ON        == MAL_NET_CS_POWER_ON,   CN_CELLULAR_SYSTEM_COMMAND_POWER_ON);
C_ASSERT(CN_CELLULAR_SYSTEM_COMMAND_GAN_FLIGHT_MODE == MAL_NET_CS_POWER_WIFI, CN_CELLULAR_SYSTEM_COMMAND_GAN_FLIGHT_MODE);

/* cn_cssn_setting_t == mal_call_suppl_svc_notify */
C_ASSERT(CN_SUPPL_SVC_NOTIFY_DISABLE  == CALL_SUPPL_SVC_NOTIFY_DISABLE,   CN_SUPPL_SVC_NOTIFY_DISABLE);
C_ASSERT(CN_SUPPL_SVC_NOTIFY_ENABLE   == CALL_SUPPL_SVC_NOTIFY_ENABLE,    CN_SUPPL_SVC_NOTIFY_ENABLE);

/* emergency call service flags */
C_ASSERT(CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED     == MAL_CALL_MODEM_SERVICE_CAT_NONE,            CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED);
C_ASSERT(CN_EMERGENCY_CALL_SERVICE_POLICE          == MAL_CALL_MODEM_SERVICE_CAT_POLICE,          CN_EMERGENCY_CALL_SERVICE_POLICE);
C_ASSERT(CN_EMERGENCY_CALL_SERVICE_AMBULANCE       == MAL_CALL_MODEM_SERVICE_CAT_AMBULANCE,       CN_EMERGENCY_CALL_SERVICE_AMBULANCE);
C_ASSERT(CN_EMERGENCY_CALL_SERVICE_FIRE_BRIGADE    == MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE,    CN_EMERGENCY_CALL_SERVICE_FIRE_BRIGADE);
C_ASSERT(CN_EMERGENCY_CALL_SERVICE_MARINE_GUARD    == MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD,    CN_EMERGENCY_CALL_SERVICE_MARINE_GUARD);
C_ASSERT(CN_EMERGENCY_CALL_SERVICE_MOUNTAIN_RESCUE == MAL_CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE, CN_EMERGENCY_CALL_SERVICE_MOUNTAIN_RESCUE);

/* emergency call origin flags */
C_ASSERT(CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED == MAL_CALL_MODEM_EMERG_NBR_ORIG_HARD_CODED,    CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED);
C_ASSERT(CN_EMERGENCY_NUMBER_ORIGIN_SIM        == MAL_CALL_MODEM_EMERG_NBR_ORIG_SIM,           CN_EMERGENCY_NUMBER_ORIGIN_SIM);
C_ASSERT(CN_EMERGENCY_NUMBER_ORIGIN_NETWORK    == MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK,       CN_EMERGENCY_NUMBER_ORIGIN_NETWORK);
C_ASSERT(CN_EMERGENCY_NUMBER_ORIGIN_MEMORY     == MAL_CALL_MODEM_EMERG_NBR_ORIG_PERMANENT_MEM, CN_EMERGENCY_NUMBER_ORIGIN_MEMORY);

/* cn_service_class_t == mal_ss_serv_class_info */
C_ASSERT(CN_SERVICE_CLASS_GSM_VOICE == GSM_VOICE, CN_SERVICE_CLASS_GSM_VOICE);
C_ASSERT(CN_SERVICE_CLASS_GSM_BEARER == GSM_BEARER, CN_SERVICE_CLASS_GSM_BEARER);
C_ASSERT(CN_SERVICE_CLASS_GSM_FACSIMILE == GSM_FACSIMILE, CN_SERVICE_CLASS_GSM_FACSIMILE);
C_ASSERT(CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE == GSM_VOICE_FACSIMILE, CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE);
C_ASSERT(CN_SERVICE_CLASS_GSM_SMS == GSM_SMS, CN_SERVICE_CLASS_GSM_SMS);
C_ASSERT(CN_SERVICE_CLASS_GSM_FACSIMILE_SMS == GSM_FACSIMILE_SMS, CN_SERVICE_CLASS_GSM_FACSIMILE_SMS);
C_ASSERT(CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS == GSM_VOICE_FACSIMILE_SMS, CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS);
C_ASSERT(CN_SERVICE_CLASS_GSM_DATA_CIRCUIT_SYNC == GSM_DATA_CIRCUIT_SYNC, CN_SERVICE_CLASS_GSM_DATA_CIRCUIT_SYNC);
C_ASSERT(CN_SERVICE_CLASS_GSM_DATA_CIRCUIT_ASYNC == GSM_DATA_CIRCUIT_ASYNC, CN_SERVICE_CLASS_GSM_DATA_CIRCUIT_ASYNC);
C_ASSERT(CN_SERVICE_CLASS_GSM_PACKET_ACCESS == GSM_PACKET_ACCESS, CN_SERVICE_CLASS_GSM_PACKET_ACCESS);
C_ASSERT(CN_SERVICE_CLASS_GSM_PAD_ACCESS == GSM_PAD_ACCESS, CN_SERVICE_CLASS_GSM_PAD_ACCESS);
C_ASSERT(CN_SERVICE_CLASS_GSM_DATA_ASYNC_PAD_ACCESS == GSM_DATA_ASYNC_PAD_ACCESS, CN_SERVICE_CLASS_GSM_DATA_ASYNC_PAD_ACCESS);
C_ASSERT(CN_SERVICE_CLASS_GSM_DATA_SYNC_PACKET_ACCESS == GSM_DATA_SYNC_PACKET_ACCESS, CN_SERVICE_CLASS_GSM_DATA_SYNC_PACKET_ACCESS);
C_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT == GSM_DEFAULT, CN_SERVICE_CLASS_GSM_DEFAULT);

/* cn_comfort_tone_generation_t == mal_call_alert_tone */
C_ASSERT(CN_COMFORT_TONE_GENERATION_STOP  == MAL_CALL_STOP_ALERT_TONE, CN_COMFORT_TONE_GENERATION_STOP);
C_ASSERT(CN_COMFORT_TONE_GENERATION_START == MAL_CALL_START_ALERT_TONE, CN_COMFORT_TONE_GENERATION_START);

/* cn_call_mode_t == */
C_ASSERT(CN_CALL_MODE_EMERGENCY == MAL_CALL_MODE_EMERGENCY, CN_CALL_MODE_EMERGENCY);
C_ASSERT(CN_CALL_MODE_SPEECH == MAL_CALL_MODE_SPEECH, CN_CALL_MODE_SPEECH);
C_ASSERT(CN_CALL_MODE_ALS_2 == MAL_CALL_MODE_ALS_LINE_2, CN_CALL_MODE_ALS_2);
C_ASSERT(CN_CALL_MODE_MULTIMEDIA == MAL_CALL_MODE_MULTIMEDIA, CN_CALL_MODE_MULTIMEDIA);
C_ASSERT(CN_CALL_MODE_UNKNOWN == MAL_CALL_MODE_UNKNOWN, CN_CALL_MODE_UNKNOWN);

#endif /* USE_MAL_CS */

#ifdef USE_MAL_RF
/* cn_tx_back_off_event_t == mal_rf_tx_back_off_event */
C_ASSERT(CN_TX_BACK_OFF_EVENT_NO_EVENT                        == MAL_RF_TX_BACK_OFF_EVENT_NO_EVENT,                        CN_TX_BACK_OFF_EVENT_NO_EVENT);
C_ASSERT(CN_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_CLOSED     == MAL_RF_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_CLOSED,     CN_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_CLOSED);
C_ASSERT(CN_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_OPEN       == MAL_RF_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_OPEN,       CN_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_OPEN);
C_ASSERT(CN_TX_BACK_OFF_EVENT_DVBH_INACTIVE                   == MAL_RF_TX_BACK_OFF_EVENT_DVBH_INACTIVE,                   CN_TX_BACK_OFF_EVENT_DVBH_INACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_DVBH_ACTIVE                     == MAL_RF_TX_BACK_OFF_EVENT_DVBH_ACTIVE,                     CN_TX_BACK_OFF_EVENT_DVBH_ACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_FAR            == MAL_RF_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_FAR,            CN_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_FAR);
C_ASSERT(CN_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR           == MAL_RF_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR,           CN_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR);
C_ASSERT(CN_TX_BACK_OFF_EVENT_GPS_INACTIVE                    == MAL_RF_TX_BACK_OFF_EVENT_GPS_INACTIVE,                    CN_TX_BACK_OFF_EVENT_GPS_INACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_GPS_ACTIVE                      == MAL_RF_TX_BACK_OFF_EVENT_GPS_ACTIVE,                      CN_TX_BACK_OFF_EVENT_GPS_ACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_ANTENNA_INACTIVE                == MAL_RF_TX_BACK_OFF_EVENT_ANTENNA_INACTIVE,                CN_TX_BACK_OFF_EVENT_ANTENNA_INACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_ANTENNA_ACTIVE                  == MAL_RF_TX_BACK_OFF_EVENT_ANTENNA_ACTIVE,                  CN_TX_BACK_OFF_EVENT_ANTENNA_ACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_INACTIVE     == MAL_RF_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_INACTIVE,     CN_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_INACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_ACTIVE       == MAL_RF_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_ACTIVE,       CN_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_ACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_INACTIVE           == MAL_RF_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_INACTIVE,           CN_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_INACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_ACTIVE             == MAL_RF_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_ACTIVE,             CN_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_ACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_INACTIVE == MAL_RF_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_INACTIVE, CN_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_INACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_ACTIVE   == MAL_RF_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_ACTIVE,   CN_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_ACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_INACTIVE      == MAL_RF_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_INACTIVE,      CN_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_INACTIVE);
C_ASSERT(CN_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_ACTIVE        == MAL_RF_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_ACTIVE,        CN_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_ACTIVE);

#endif /* USE_MAL_RF */

#endif /* __cn_mal_assertions_h__ */
