/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "atc_log.h"
#include "exe_cn_utils.h"
#include "exe_internal.h"
#include "exe_last_fail_cause.h"


/* Utility for mapping the range of network registration states from the
 * representation in CN to the representation in AT core.
 */
bool map_registration_state_from_cn_to_at(uint32_t status_cn, exe_net_reg_stat_t *status_at_p)
{
    if (NULL == status_at_p) {
        ATC_LOG_E("%s: null parameter received", __FUNCTION__);
        return false;
    }

    switch (status_cn) {
    case CN_NOT_REG_NOT_SEARCHING:
        *status_at_p = EXE_NET_REG_STAT_NOT_REGISTERED;
        break;
    case CN_REGISTERED_TO_HOME_NW:
        *status_at_p = EXE_NET_REG_STAT_REGISTERED_HOME;
        break;
    case CN_NOT_REG_SEARCHING_OP:
        *status_at_p = EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH;
        break;
    case CN_REGISTRATION_DENIED:
        *status_at_p = EXE_NET_REG_STAT_REGISTRATION_DENIED;
        break;
    case CN_REGISTRATION_UNKNOWN:
        *status_at_p = EXE_NET_REG_STAT_UNKNOWN;
        break;
    case CN_REGISTERED_ROAMING:
        *status_at_p = EXE_NET_REG_STAT_REGISTERED_ROAMING;
        break;
    case CN_NOT_REG_NOT_SEARCHING_EMERGENCY_CALLS_ALLOWED:
        *status_at_p = EXE_NET_REG_STAT_NOT_REGISTERED_EMERGENCY_CALLS_ALLOWED;
        break;
    case CN_NOT_REG_SEARCHING_OP_EMERGENCY_CALLS_ALLOWED:
        *status_at_p = EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH_EMERGENCY_CALLS_ALLOWED;
        break;
    case CN_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED:
        *status_at_p = EXE_NET_REG_STAT_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED;
        break;
    case CN_REGISTRATION_UNKNOWN_EMERGENCY_CALLS_ALLOWED:
        *status_at_p = EXE_NET_REG_STAT_UNKNOWN_EMERGENCY_CALLS_ALLOWED;
        break;
    default:
        ATC_LOG_E("%s: unexpected status:%d", __FUNCTION__, status_cn);
        return false;
    }

    return true;
}

/* Utility for mapping the range of network states from the
 * representation in CN to the GPRS representation in AT core.
 *
 * Note: ME attached to GPRS implies the ME is also registered to the network, therefore
 *       the only valid GPRS registration states are - registered or roaming.
 *
 *       ME detached from GPRS combined with network registration states registered or roaming
 *       implies the ME is registered to the network but not GPRS. In this situation the
 *       only valid GPRS state is - not registered.
 */
bool map_gprs_registration_state_from_cn_to_at(uint32_t status_cn, bool attached, exe_net_reg_stat_t *status_at_p)
{
    if (NULL == status_at_p) {
        ATC_LOG_E("%s: null parameter received", __FUNCTION__);
        return false;
    }

    if (attached) {
        switch (status_cn) {
        case CN_REGISTERED_TO_HOME_NW:
            *status_at_p = EXE_NET_REG_STAT_REGISTERED_HOME;
            break;
        case CN_REGISTERED_ROAMING:
            *status_at_p = EXE_NET_REG_STAT_REGISTERED_ROAMING;
            break;
        default:
            ATC_LOG_E("%s: unexpected status:%d attached:%d", __FUNCTION__, status_cn, attached);
            return false;
        }
    } else {
        switch (status_cn) {
        case CN_NOT_REG_NOT_SEARCHING:
            *status_at_p = EXE_NET_REG_STAT_NOT_REGISTERED;
            break;
        case CN_NOT_REG_SEARCHING_OP:
            *status_at_p = EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH;
            break;
        case CN_REGISTRATION_DENIED:
            *status_at_p = EXE_NET_REG_STAT_REGISTRATION_DENIED;
            break;
        case CN_REGISTRATION_UNKNOWN:
            *status_at_p = EXE_NET_REG_STAT_UNKNOWN;
            break;
        case CN_NOT_REG_NOT_SEARCHING_EMERGENCY_CALLS_ALLOWED:
            *status_at_p = EXE_NET_REG_STAT_NOT_REGISTERED_EMERGENCY_CALLS_ALLOWED;
            break;
        case CN_NOT_REG_SEARCHING_OP_EMERGENCY_CALLS_ALLOWED:
            *status_at_p = EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH_EMERGENCY_CALLS_ALLOWED;
            break;
        case CN_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED:
            *status_at_p = EXE_NET_REG_STAT_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED;
            break;
        case CN_REGISTRATION_UNKNOWN_EMERGENCY_CALLS_ALLOWED:
            *status_at_p = EXE_NET_REG_STAT_UNKNOWN_EMERGENCY_CALLS_ALLOWED;
            break;
        case CN_REGISTERED_TO_HOME_NW:
        case CN_REGISTERED_ROAMING:
            *status_at_p = EXE_NET_REG_STAT_NOT_REGISTERED;
            break;
        default:
            ATC_LOG_E("%s: unexpected status:%d attached:%d", __FUNCTION__, status_cn, attached);
            return false;
        }
    }

    return true;
}

/* Utility for mapping the range of radio access technologies from the
 * representation in CN to the representation used in *EREG and  +COPS.
 */
bool map_rat_to_basic_act(cn_rat_type_t rat, exe_basic_act_t *act_p)
{
    if (NULL == act_p) {
        ATC_LOG_E("%s: null parameter received", __FUNCTION__);
        return false;
    }

    switch (rat) {
    case CN_RAT_TYPE_GSM:
    case CN_RAT_TYPE_GPRS:
    case CN_RAT_TYPE_EDGE_DTM:
    case CN_RAT_TYPE_GPRS_DTM:
    case CN_RAT_TYPE_EDGE: {
        *act_p = EXE_BASIC_ACT_GSM;
        break;
    }
    case CN_RAT_TYPE_UMTS:
    case CN_RAT_TYPE_HSDPA:
    case CN_RAT_TYPE_HSUPA:
    case CN_RAT_TYPE_HSPA: {
        *act_p = EXE_BASIC_ACT_UTRAN;
        break;
    }
    case CN_RAT_TYPE_UNKNOWN:
    case CN_RAT_TYPE_IS95A:
    case CN_RAT_TYPE_IS95B:
    case CN_RAT_TYPE_1xRTT:
    case CN_RAT_TYPE_EVDO_REV_0:
    case CN_RAT_TYPE_EVDO_REV_A:
    default:
        ATC_LOG_E("%s: unexpected rat:%d", __FUNCTION__, rat);
        return false;
    }

    return true;
}

/* Utility for mapping the range of radio access technologies from the
 * representation in CN to the representation in AT core.
 */
bool map_rat_to_act(cn_rat_type_t rat, exe_act_t *act_p)
{
    if (NULL == act_p) {
        ATC_LOG_E("%s: null parameter received", __FUNCTION__);
        return false;
    }

    switch (rat) {
    case CN_RAT_TYPE_GPRS:
    case CN_RAT_TYPE_GSM:
        *act_p = EXE_ACT_GSM;
        break;
    case CN_RAT_TYPE_EDGE:
        *act_p = EXE_ACT_GSM_EGPRS;
        break;
    case CN_RAT_TYPE_UMTS:
        *act_p = EXE_ACT_UTRAN;
        break;
    case CN_RAT_TYPE_HSPA:
        *act_p = EXE_ACT_UTRAN_HSUPA_HSDPA;
        break;
    case CN_RAT_TYPE_HSDPA:
        *act_p = EXE_ACT_UTRAN_HDSPA;
        break;
    case CN_RAT_TYPE_HSUPA:
        *act_p = EXE_ACT_UTRAN_HSUPA;
        break;
    case CN_RAT_TYPE_GPRS_DTM:
        *act_p = EXE_ACT_GSM_GPRS_DTM;
        break;
    case CN_RAT_TYPE_EDGE_DTM:
        *act_p = EXE_ACT_GSM_EDGE_DTM;
        break;
    case CN_RAT_TYPE_UNKNOWN:
    case CN_RAT_TYPE_IS95A:       /* CDMA */
    case CN_RAT_TYPE_IS95B:       /* CDMA */
    case CN_RAT_TYPE_1xRTT:       /* CDMA */
    case CN_RAT_TYPE_EVDO_REV_0:  /* CDMA */
    case CN_RAT_TYPE_EVDO_REV_A:  /* CDMA */
    default:
        ATC_LOG_E("%s: unexpected rat:%d", __FUNCTION__, rat);
        return false;
    }

    return true;
}

/* Utility for setting the exit cause data received from CN to the AT usage. */
int32_t exe_cn_get_exit_cause(cn_exit_cause_t* exit_cause_p)
{
    int32_t exit_cause = 0;

    switch (exit_cause_p->cause_type_sender) {
    case CN_CALL_CAUSE_TYPE_SENDER_NETWORK:
    case CN_CALL_CAUSE_TYPE_SENDER_SERVER:
        exit_cause = exit_cause_p->cause;
        break;

    case CN_CALL_CAUSE_TYPE_SENDER_CLIENT:
        /* For this MAL specific client type we map back to the original 3gpp specified network cause values.
         * What we loose is the information that the disconnect was caused by the party using this device,
         * but so far we meet the usage and requirements from higher levels.
         */
        if (CN_CALL_CLIENT_CAUSE_RELEASE_BY_USER == exit_cause_p->cause) {
            exit_cause = CN_CALL_NETWORK_CAUSE_NORMAL;
        } else if (CN_CALL_CLIENT_CAUSE_BUSY_USER_REQUEST ==  exit_cause_p->cause) {
            exit_cause = CN_CALL_NETWORK_CAUSE_USER_BUSY;
        }
        break;

    default:
        ATC_LOG_E("%s: No valid sender type.", __FUNCTION__);
    }

    return exit_cause;
}

/* Utility for registering of last fail cause data in the executor. */
void exe_cn_register_fail_cause(cn_exit_cause_t *exit_cause_p, const char *function_name)
{
    exe_last_fail_cause_t fail_response;

    EXE_CHECK_GOTO_ERROR(NULL != exit_cause_p);

    switch (exit_cause_p->cause_type_sender) {
    case CN_CALL_CAUSE_TYPE_SENDER_CLIENT:
    case CN_CALL_CAUSE_TYPE_SENDER_NETWORK:
        fail_response.class = EXE_LAST_FAIL_CAUSE_CLASS_CN_NETWORK;
        break;

    case CN_CALL_CAUSE_TYPE_SENDER_SERVER:
        fail_response.class = EXE_LAST_FAIL_CAUSE_CLASS_CN_SERVER;
        break;

    default:
        goto error;
    }

    fail_response.cause = exe_cn_get_exit_cause(exit_cause_p);
    /* Register fail data in executor */
    exe_set_last_fail_cause(exe_get_exe(), &fail_response);
    return;

error:
    ATC_LOG_E("%s: CN_FAILURE no valid exit cause given", function_name);

}


/* Utility for mapping cn call mode to clcc mode */
bool map_cn_call_mode_to_clcc_mode(cn_call_mode_t cn_call_mode, exe_clcc_mode_t *clcc_mode_p)
{
    if (!clcc_mode_p) {
        ATC_LOG_E("Parameter error!");
        return false;
    }

    switch (cn_call_mode) {
    case CN_CALL_MODE_EMERGENCY:
        *clcc_mode_p = EXE_CLCC_MODE_VOICE;
        break;
    case CN_CALL_MODE_SPEECH:
        *clcc_mode_p = EXE_CLCC_MODE_VOICE;
        break;
    case CN_CALL_MODE_ALS_2:
        ATC_LOG_D("Unsupported call mode: %d", cn_call_mode);
        *clcc_mode_p = EXE_CLCC_MODE_UNKNOWN;
        break;
    case CN_CALL_MODE_MULTIMEDIA:
        ATC_LOG_D("Unsupported call mode: %d", cn_call_mode);
        *clcc_mode_p = EXE_CLCC_MODE_UNKNOWN;
        break;
    case CN_CALL_MODE_UNKNOWN:
        /* Fall through */
    default:
        ATC_LOG_D("Unknown call mode: %d", cn_call_mode);
        *clcc_mode_p = EXE_CLCC_MODE_UNKNOWN;
        break;
    }

    return true;
}


/* Utility for mapping "Cause of no CLI" to CLI_validity */
bool map_cause_no_cli_to_cli_validity(cn_cause_no_cli_t cause_no_cli, const char *info_p, exe_cli_validity_t *cli_validity_p)
{
    if (!cli_validity_p) {
        ATC_LOG_E("Parameter error!");
        return false;
    }

    switch (cause_no_cli) {
    case CN_CAUSE_NO_CLI_UNAVAILABLE:
        *cli_validity_p = EXE_CLI_VALIDITY_NA_OTHER;
        break;
    case CN_CAUSE_NO_CLI_REJECT_BY_USER:
        *cli_validity_p = EXE_CLI_VALIDITY_WITHHELD;
        break;
    case CN_CAUSE_NO_CLI_INTERACTION:
        *cli_validity_p = EXE_CLI_VALIDITY_NA_INTERWORKING;
        break;
    case CN_CAUSE_NO_CLI_COIN_LINE_PAYPHONE:
        *cli_validity_p = EXE_CLI_VALIDITY_NA_PAYPHONE;
        break;

    default:
        if (NULL != info_p && 0 < strlen(info_p)) {
            *cli_validity_p = EXE_CLI_VALIDITY_VALID;
        } else {
            *cli_validity_p = EXE_CLI_VALIDITY_NA_OTHER;
        }
        break;
    }

    return true;
}


/* Utility for mapping presentation (and screening) indicator to CNI_validity */
bool map_presentation_to_cni_validity(cn_presentation_t presentation, exe_cni_validity_t *cni_validity_p)
{
    if (!cni_validity_p) {
        ATC_LOG_E("Parameter error!");
        return false;
    }

    switch (presentation & CN_PRESENTATION_MASK) {
    case CN_PRESENTATION_ALLOWED:
        *cni_validity_p = EXE_CNI_VALIDITY_VALID;
        break;
    case CN_PRESENTATION_RESTRICTED:
        *cni_validity_p = EXE_CNI_VALIDITY_WITHHELD;
        break;
    case CN_PRESENTATION_UNAVAILABLE:
        *cni_validity_p = EXE_CNI_VALIDITY_NA_INTERWORKING;
        break;

    default:
        return false;
    }

    return true;
}


bool convert_cn_neighbouring_cell_info_to_exe_info(exe_2g_3g_neigh_cell_response_t *response_p, cn_neighbour_cells_info_t *neighbour_cells_info_p)
{
    int i = 0;
    int j = 0;
    int total_no_of_cells = 0;

    if (!response_p || !neighbour_cells_info_p) {
        ATC_LOG_E("%s: at least one of the parameter pointers is NULL!", __FUNCTION__);
        goto error;
    }
    response_p->num_of_cells = 0;

    switch (neighbour_cells_info_p->rat_type) { /* Select union type based on specified RAT type */
    case CN_NEIGHBOUR_RAT_TYPE_2G:
        response_p->rat_info = neighbour_cells_info_p->rat_type;
        response_p->serving_cell_info.gsm_info.rxlev = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev;
        response_p->serving_cell_info.gsm_info.mcc = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc;
        response_p->serving_cell_info.gsm_info.mnc = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc;
        response_p->serving_cell_info.gsm_info.lac = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac;
        response_p->serving_cell_info.gsm_info.cid = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid;
        response_p->serving_cell_info.gsm_info.timing_advance =
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance;
        response_p->serving_cell_info.gsm_info.bsic = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic;
        response_p->serving_cell_info.gsm_info.arfcn = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn;


        response_p->num_of_cells = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours;

        if (response_p->num_of_cells > 0) {
            response_p->neigh_cell_info_p = (exe_cell_information_response_t *)malloc(sizeof(exe_cell_information_response_t) * (response_p->num_of_cells));
            if (!response_p->neigh_cell_info_p) {
                ATC_LOG_E("malloc failed for response.neigh_cell_info_p! 2G");
                goto error;
            }
        }
        for (i = 0; i < response_p->num_of_cells; i++) {
            response_p->neigh_cell_info_p[i].gsm_info.rxlev = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[i].rxlev;
            response_p->neigh_cell_info_p[i].gsm_info.mcc = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[i].mcc;
            response_p->neigh_cell_info_p[i].gsm_info.mnc = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[i].mnc;
            response_p->neigh_cell_info_p[i].gsm_info.lac = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[i].lac;
            response_p->neigh_cell_info_p[i].gsm_info.cid = neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[i].cid;
        }
        break;
    case CN_NEIGHBOUR_RAT_TYPE_3G:
        response_p->rat_info = neighbour_cells_info_p->rat_type;
        response_p->serving_cell_info.umts_info.cpich_ecno =
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_ecno;
        response_p->serving_cell_info.umts_info.cpich_pathloss =
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_pathloss;
        response_p->serving_cell_info.umts_info.cpich_rscp =
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_rscp;
        response_p->serving_cell_info.umts_info.dl_uarfcn =
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_dl_uarfcn;
        response_p->serving_cell_info.umts_info.psc =
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_psc;
        response_p->serving_cell_info.umts_info.mcc =
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_mcc;
        response_p->serving_cell_info.umts_info.mnc =
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_mnc;
        response_p->serving_cell_info.umts_info.ucid =
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_ucid;

        uint8_t no_of_uarfcn = neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn;

        for (i = 0; i < no_of_uarfcn; i++){ /*Count the number of cells*/
            response_p->num_of_cells += neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[i].num_of_cells;
        }

        if (response_p->num_of_cells > 0) {
            response_p->neigh_cell_info_p = (exe_cell_information_response_t *)malloc(sizeof(exe_cell_information_response_t) * (response_p->num_of_cells));
            if (!response_p->neigh_cell_info_p) {
                ATC_LOG_E("malloc failed for response.neigh_cell_info_p! 3G");
                goto error;
            }
        }

        for (i = 0; i < no_of_uarfcn; i++){ /*copy neighbour cell info*/
            int uarfcn_cells = neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[i].num_of_cells;
            for (j = 0; j < uarfcn_cells && total_no_of_cells < response_p->num_of_cells; j++){
                response_p->neigh_cell_info_p[total_no_of_cells].umts_info.cpich_ecno =
                neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[i].umts_cell_detailed_info[j].cpich_ecno;
                response_p->neigh_cell_info_p[total_no_of_cells].umts_info.cpich_pathloss =
                neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[i].umts_cell_detailed_info[j].cpich_pathloss;
                response_p->neigh_cell_info_p[total_no_of_cells].umts_info.cpich_rscp =
                neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[i].umts_cell_detailed_info[j].cpich_rscp;
                response_p->neigh_cell_info_p[total_no_of_cells].umts_info.dl_uarfcn =
                neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[i].dl_uarfcn;
                response_p->neigh_cell_info_p[total_no_of_cells].umts_info.psc =
                neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[i].umts_cell_detailed_info[j].psc;
                total_no_of_cells++;
            }
        }

        break;
    default:
        ATC_LOG_E("%s: unknown RAT mode!", __FUNCTION__);
        goto error;
    }

    return 0;
error:
    return -1;
}
