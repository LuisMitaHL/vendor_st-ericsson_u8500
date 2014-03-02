/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "cn_log.h"
#include "cn_macros.h"
#include "cn_mal_utils.h"

#define MAX_NR_OF_CALLS 0x08

#ifdef USE_MAL_FTD
static bool empage_urc_issue                = false;
static bool empage_write_tofile             = false;
static int  empage_seqnr                    = 1;
static int  empage_pageno                   = 1;
static bool empage_activation_in_progress   = false;
static bool empage_deactivation_in_progress = false;
#endif /* USE_MAL_FTD */

/* The CN server runs in a single thread context. No practical issue with static keyword */
static cn_call_state_t current_call_state[MAX_NR_OF_CALLS] = { CN_CALL_STATE_IDLE };

int util_copy_neighbour_cells_info_data(cn_neighbour_cells_info_t *cn_neighbour_cells_info, mal_net_neighbour_cells_info *mal_neighbour_cells_info,cn_uint32_t *size){

    cn_uint32_t cell_index = 0;
    cn_uint32_t n_cell_count = 0;
    if (!cn_neighbour_cells_info || !mal_neighbour_cells_info) {
        CN_LOG_E("pointer check failed!");
        goto error;
    }
    cn_neighbour_cells_info->rat_type = mal_neighbour_cells_info->rat_type;
    *size+= sizeof(cn_neighbour_cells_info->rat_type);

    switch(cn_neighbour_cells_info->rat_type){
        case CN_NEIGHBOUR_RAT_TYPE_2G:
             CN_LOG_D("CN_NEIGHBOUR_RAT_TYPE_2G");
             *size+= sizeof(cn_neighbour_cells_info->cells_info.gsm_cells_info);
             cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc = mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc;
             cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc = mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc;
             cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac = mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac;
             cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid = mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid;
             cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance = mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance;
             cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn = mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn;
             cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic = mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic;
             cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev = mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev;
             cn_neighbour_cells_info->cells_info.gsm_cells_info.neigh_info_type = mal_neighbour_cells_info->cells_info.gsm_cells_info.neigh_info_type;

             CN_LOG_D("Serving Cell MCC = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc);
             CN_LOG_D("Serving Cell MNC = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc);
             CN_LOG_D("Serving Cell LAC = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac);
             CN_LOG_D("Serving Cell Cell id = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid);
             CN_LOG_D("Serving Cell TA = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance);
             CN_LOG_D("Serving Cell ARFCN = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn);
             CN_LOG_D("Serving Cell BSIC = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic);
             CN_LOG_D("Serving Cell RXLEV = %d",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev);
             CN_LOG_D("Serving Cell neigh_info_type = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.neigh_info_type);

             switch(cn_neighbour_cells_info->cells_info.gsm_cells_info.neigh_info_type){
                 case CN_NET_NEIGHBOUR_CELLS_BASIC_EXT:
                     CN_LOG_D("CN_NET_NEIGHBOUR_CELLS_BASIC_EXT");
                     cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours =
                         mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours;
                     CN_LOG_D("num_of_neighbours = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours);
                     for (cell_index = 0;
                          cell_index < cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours;
                          cell_index++){
                         cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].mcc =
                             mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].mcc;
                         cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].mnc =
                             mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].mnc;
                         cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].lac =
                             mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].lac;
                         cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].cid =
                             mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].cid;
                         cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].rxlev =
                             mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].rxlev; /* In dBm */

                         CN_LOG_D("MCC[%d] = %u",cell_index,cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].mcc);
                         CN_LOG_D("MNC[%d] = %u",cell_index,cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].mnc);
                         CN_LOG_D("LAC[%d] = %u",cell_index,cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].lac);
                         CN_LOG_D("CID[%d] = %u",cell_index,cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].cid);
                         CN_LOG_D("RXLEV[%d] = %d",cell_index,cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[cell_index].rxlev);

                     }
                 case CN_NET_NEIGHBOUR_CELLS_EXT:
                     CN_LOG_D("CN_NET_NEIGHBOUR_CELLS_EXT");
                     cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours =
                     mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours;
                     CN_LOG_D("num_of_neighbours = %u",cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours);
                     for (cell_index = 0;
                          cell_index < cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours;
                          cell_index++) {
                         cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[cell_index].arfcn =
                             mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[cell_index].arfcn;
                         cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[cell_index].bsic =
                             mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[cell_index].bsic;
                         cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[cell_index].rxlev =
                             mal_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[cell_index].rxlev;

                         CN_LOG_D("ARFCN[%d] = %u", cell_index, cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[cell_index].arfcn);
                         CN_LOG_D("BSIC[%d] = %u", cell_index, cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[cell_index].bsic);
                         CN_LOG_D("RXLEV[%d] = %d", cell_index, cn_neighbour_cells_info->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[cell_index].rxlev);
                     }
                     break;
             }

             break;
        case CN_NEIGHBOUR_RAT_TYPE_3G:
             CN_LOG_D("CN_NEIGHBOUR_RAT_TYPE_3G");
             *size+= sizeof(cn_neighbour_cells_info->cells_info.umts_cells_info);
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_ucid =
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_ucid;
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_mcc =
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_mcc;
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_mnc =
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_mnc;
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_psc =
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_psc;
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_dl_uarfcn =
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_dl_uarfcn;
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_utra_carrier_rssi=
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_utra_carrier_rssi;
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_ecno =
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_ecno;
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_rscp =
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_rscp;
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_pathloss =
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_pathloss;
             cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn =
                 mal_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn;

             CN_LOG_D("Serving Cell Cell id = %u", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_ucid);
             CN_LOG_D("Serving Cell MCC = %u", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_mcc);
             CN_LOG_D("Serving Cell MNC = %u", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_mnc);
             CN_LOG_D("Serving Cell PSC = %u", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_psc);
             CN_LOG_D("Serving Cell dl_uarfcn = %u", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_dl_uarfcn);
             CN_LOG_D("Serving Cell CPICH_ECNO = %u", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_ecno);
             CN_LOG_D("Serving Cell CPICH_RSCP = %d", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_rscp);
             CN_LOG_D("Serving Cell CPICH_PATHLOSS = %u", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_pathloss);
             CN_LOG_D("Serving Cell UTRA CARRIER RSSI = %u", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_serving_cell_info.current_utra_carrier_rssi);
             CN_LOG_D("No. of dl_uarfcns = %d", cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn);

             for(cell_index = 0;
                 cell_index < cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn;
                 cell_index++) {
                 /* Retrieve NET_UTRAN_NEIGH_LIST_SEQ */
                 cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].dl_uarfcn =
                     mal_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].dl_uarfcn;
                 cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].utra_carrier_rssi =
                     mal_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].utra_carrier_rssi;
                 cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].num_of_cells =
                     mal_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].num_of_cells;

                 CN_LOG_D("dl_uarfcn[%d] = %u",cell_index,cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].dl_uarfcn);
                 CN_LOG_D("utra_carrier_rssi[%d] = %u",cell_index,cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].utra_carrier_rssi);
                 CN_LOG_D("num_of_cells[%d] = %u",cell_index,cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].num_of_cells);

                 for(n_cell_count = 0;
                     n_cell_count < cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].num_of_cells;
                     n_cell_count++) {
                     cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].ucid =
                         mal_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].ucid;
                     cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].psc =
                         mal_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].psc;
                     cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].cpich_ecno =
                        mal_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].cpich_ecno;
                     cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].cpich_rscp =
                        mal_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].cpich_rscp;
                     cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].cpich_pathloss =
                        mal_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].cpich_pathloss;

                     CN_LOG_D("ucid[%d] = %u",n_cell_count,cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].ucid);
                     CN_LOG_D("psc[%d] = %u",n_cell_count,cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].psc);
                     CN_LOG_D("cpich_ecno[%d] = %u",n_cell_count,cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].cpich_ecno);
                     CN_LOG_D("cpich_rscp[%d] = %d",n_cell_count,cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].cpich_rscp);
                     CN_LOG_D("cpich_pathloss[%d] = %u",n_cell_count,cn_neighbour_cells_info->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[cell_index].umts_cell_detailed_info[n_cell_count].cpich_pathloss);
                 }
             }

             break;
        default:
             CN_LOG_E("Unknown RAT mode");
             goto error;
        }
    return 0;

error:
    return -1;
}

cn_bool_t util_convert_cn_trigger_level_to_mal(cn_reg_status_trigger_level_t trigger_level, mal_net_reg_status_set_mode *reg_status_mode_p)
{
    cn_bool_t result = FALSE;

    if (!reg_status_mode_p) {
        CN_LOG_E("reg_status_mode_p is NULL!");
        goto exit;
    }

    switch (trigger_level) {
    case CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE:
        *reg_status_mode_p = MAL_NET_MSG_SEND_MODE_REG_STATUS_CHANGE;
        break;
    case CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC:
        *reg_status_mode_p = MAL_NET_MSG_SEND_MODE_REG_STATUS_CHANGE | MAL_NET_MSG_SEND_MODE_CELL_ID_OR_LAC_CHANGE;
        break;
    case CN_REG_STATUS_TRIGGER_LEVEL_ALL_PARAMS:
        *reg_status_mode_p = MAL_NET_MSG_SEND_MODE_SEND_ALL;
        break;
    case CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM: /* this value cannot be mapped to MAL */
    default:
        CN_LOG_E("unknown trigger level! (%d)", trigger_level);
        goto exit; /* we leave *reg_status_mode_p untouched */
        break;
    } /* end of switch-statement */

    CN_LOG_D("converting CN trigger level %d to MAL send mode %d", trigger_level, *reg_status_mode_p);

    result = TRUE;
exit:
    return result;
}


#ifdef USE_MAL_CS
/* \n Helper function to map call_cause info from MAL call_context.
 * \n Used by util_call_failure_response_cause()
 *
 */

void util_call_cause(exit_cause *mal_call_cause_p, cn_exit_cause_t *call_cause_p)
{
    static cn_uint32_t util_call_cause_error_count = 0;
    static cn_uint32_t util_call_cause_debug_count = 0;
    call_cause_p->cause = (cn_call_cause_cause_t)0;
    call_cause_p->cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NONE;


    switch (mal_call_cause_p->cause_type_sender) {
    case CALL_CAUSE_TYPE_CLIENT:
    case CALL_CAUSE_TYPE_SERVER:
    case CALL_CAUSE_TYPE_NETWORK: /* Pass it through unchanged */
        call_cause_p->cause = (cn_call_cause_cause_t)mal_call_cause_p->cause;
        call_cause_p->cause_type_sender = (cn_call_cause_type_sender_t)(mal_call_cause_p->cause_type_sender);

        if (CALL_CAUSE_TYPE_SERVER == mal_call_cause_p->cause_type_sender) {
            util_call_cause_debug_count++;
            CN_LOG_D("instance %d of a TYPE_SERVER exit cause (%d)",
                     util_call_cause_debug_count, mal_call_cause_p->cause);
        }

        break;

    case CALL_CAUSE_NONE:  /* No exit cause, no error */
        CN_LOG_D("CALL_CAUSE_NONE");
        break;

    default:
        CN_LOG_E("mal_call_cause_p->cause_type_sender unknown! (%d)", mal_call_cause_p->cause_type_sender);
        goto error;
    }

    call_cause_p->detailed_cause_length = (cn_uint8_t)(mal_call_cause_p->detailed_cause_length);

    if (0 < call_cause_p->detailed_cause_length &&
            CN_MAX_DETAILED_CAUSE_LENGTH >= call_cause_p->detailed_cause_length) {

        if (NULL != mal_call_cause_p->detailed_cause) {
            memcpy(call_cause_p->detailed_cause, mal_call_cause_p->detailed_cause, call_cause_p->detailed_cause_length * sizeof(cn_uint8_t));
        }
    }

    return;

error:
    util_call_cause_error_count++;

    CN_LOG_E("instance %d of an unsupported exit cause (%d, %d)",
             util_call_cause_error_count, mal_call_cause_p->cause_type_sender, mal_call_cause_p->cause);
}

/* Helper function to move call_context info from MAL. Used by both
 * handle_event_ring() and handle_event_call_state_changed().
 */
void util_call_context(mal_call_context *mal_call_context_p, cn_call_context_t *call_context_p)
{

    memset(call_context_p, 0, sizeof(*call_context_p));
    call_context_p->call_state        = mal_call_context_p->callState;
    call_context_p->prev_call_state   = current_call_state[mal_call_context_p->callId];
    current_call_state[mal_call_context_p->callId] = mal_call_context_p->callState;
    call_context_p->call_id           = mal_call_context_p->callId;
    call_context_p->address_type      = mal_call_context_p->addrType;
    call_context_p->message_type      = mal_call_context_p->message_type;
    call_context_p->message_direction = mal_call_context_p->message_direction;
    call_context_p->transaction_id    = mal_call_context_p->transaction_id;
    call_context_p->is_multiparty     = mal_call_context_p->isMpty;
    call_context_p->is_MT             = mal_call_context_p->isMT;
    call_context_p->als               = mal_call_context_p->als;
    call_context_p->mode              = (cn_call_mode_t)mal_call_context_p->call_mode;
    call_context_p->is_voice_privacy  = mal_call_context_p->isVoicePrivacy;
    call_context_p->cause_no_cli      = mal_call_context_p->call_cli_cause;
    util_call_cause(&(mal_call_context_p->call_cause), &(call_context_p->call_cause));

    call_context_p->number_presentation = (cn_presentation_t)mal_call_context_p->numberPresentation;

    if (mal_call_context_p->number) {
        (void)strncpy(call_context_p->number,
                      (const char *)mal_call_context_p->number, CN_MAX_STRING_SIZE);
    }

    call_context_p->name_presentation = (cn_presentation_t)mal_call_context_p->namePresentation;

    if (mal_call_context_p->name && 0 < mal_call_context_p->name_length) {
        size_t char_length = CN_MIN(sizeof(call_context_p->name) / sizeof(uint16_t), mal_call_context_p->name_length);

        if (mal_call_context_p->name_length > char_length) {
            CN_LOG_W("Truncating name with %d characters!", mal_call_context_p->name_length - char_length);
        }

        (void)memmove(call_context_p->name, mal_call_context_p->name, sizeof(uint16_t) * char_length);
        call_context_p->name_char_length = char_length;
    }

    if (mal_call_context_p->call_uus_info.uus_length > 0 &&
            mal_call_context_p->call_uus_info.uus_length < CN_MAX_UUS_SIZE) {
        (void)memmove(call_context_p->user_to_user,
                      (const char *)mal_call_context_p->call_uus_info.uus, mal_call_context_p->call_uus_info.uus_length);
        call_context_p->user_to_user_len = mal_call_context_p->call_uus_info.uus_length;
        call_context_p->user_to_user_type = 1; /* Only supported value for now and no value from mal */
    } else if (mal_call_context_p->call_uus_info.uus_length > CN_MAX_UUS_SIZE) {
        CN_LOG_W("call_uus_info->uus_length bigger than CN_MAX_UUS_SIZE, ignoring");
    }

}

void util_call_failure_response_cause(void *data_p, request_record_t *record_p, cn_message_type_t response_type, const char *calling_function)
{
    cn_exit_cause_t cn_exit_cause;
    cn_uint32_t payload_size = 0; /* default is to send no payload */


    (void) data_p;

    memset(&cn_exit_cause, 0, sizeof(cn_exit_cause));

    if (MAL_FAIL != record_p->response_error_code) {
        CN_LOG_W("%s returned %d not MAL_FAIL or MAL_SUCCESS", calling_function, record_p->response_error_code);
        send_response(record_p->client_id, response_type, CN_FAILURE, record_p->client_tag, 0, NULL);
    } else if (NULL != record_p->response_data_p) {
        util_call_cause((exit_cause *)record_p->response_data_p, &cn_exit_cause);
        payload_size = sizeof(cn_exit_cause);

        CN_LOG_D("Failure cause: %d", cn_exit_cause.cause);
        send_response(record_p->client_id, response_type, CN_FAILURE, record_p->client_tag, payload_size, &cn_exit_cause);
    } else {
        CN_LOG_D("%s failed with no exit cause given", calling_function);
        send_response(record_p->client_id, response_type, CN_FAILURE, record_p->client_tag, 0, NULL);
    }
}


int util_get_call_by_filter(cn_call_state_filter_t *filter_p, cn_call_context_t *call_context_p)
{
    int matches_found = -1;
    cn_call_state_filter_t cs_filter;
    mal_call_context *mal_call_context_p;
    int32_t no_of_calls;
    int32_t result;


    if (!filter_p) {
        CN_LOG_E("filter_p NULL");
        goto exit;
    }

    if (CN_CALL_STATE_FILTER_NONE == *filter_p) {
        CN_LOG_D("No filter");
        goto exit;
    }

    if (!call_context_p) {
        CN_LOG_E("call_context_p NULL");
        goto exit;
    }

    /* Get current calls list, synchronous */
    result = mal_call_request_getcalllist(NULL, (void **) &mal_call_context_p, &no_of_calls);

    if (MAL_SUCCESS != result) {
        CN_LOG_E("mal_call_request_getcalllist failed (result:%d)!", result);
        goto exit;
    }

    /* Process list in call-state order beginning with holding and ending with idle calls */
    matches_found = 0;

    for (cs_filter = CN_CALL_STATE_FILTER_WAITING;
            cs_filter <= CN_CALL_STATE_FILTER_IDLING;
            cs_filter <<= 0x1) {
        cscall_state call_state;
        int i;

        if (cs_filter & *filter_p) {

            switch (cs_filter) {
            case CN_CALL_STATE_FILTER_HOLDING:
                call_state = CALL_HOLDING;
                break;
            case CN_CALL_STATE_FILTER_ACTIVE:
                call_state = CALL_ACTIVE;
                break;
            case CN_CALL_STATE_FILTER_DIALLING:
                call_state = CALL_DIALING;
                break;
            case CN_CALL_STATE_FILTER_ALERTING:
                call_state = CALL_ALERTING;
                break;
            case CN_CALL_STATE_FILTER_INCOMING:
                call_state = CALL_INCOMING;
                break;
            case CN_CALL_STATE_FILTER_WAITING:
                call_state = CALL_WAITING;
                break;
            case CN_CALL_STATE_FILTER_IDLING:
                call_state = CALL_IDLE;
                break;
            default:
                call_state = -1;
                break;
            }

            /* Walk the list of current calls */
            for (i = 0; i < no_of_calls; i++) {

                if (call_state == mal_call_context_p[i].callState) {
                    matches_found++;

                    /* Copy the contents of the first matching entry */
                    if (1 == matches_found) {
                        util_call_context(&mal_call_context_p[i], call_context_p);
                    }
                }
            }

            /* If match(es) found, update filter & exit, unless INCLUSIVE is set */
            if (matches_found && !(CN_CALL_STATE_FILTER_INCLUSIVE & *filter_p)) {
                *filter_p = cs_filter;
                break;
            }
        }
    }

    /* Free current calls list, synchronous */
    mal_call_request_freecalllist(mal_call_context_p);

exit:

    return matches_found;
}

void util_convert_ss_error_type(int mal_error, mal_ss_error_type_value *ss_error_type_value_p, cn_ss_error_t *ss_error_p)
{

    if (!ss_error_p) {
        CN_LOG_D("ss_error_p is NULL!");
        return;
    }

    /* Set the default */
    ss_error_p->cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_CN;
    ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_INTERNAL_ERROR;

    if (ss_error_type_value_p) {
        switch (ss_error_type_value_p->error_type) {
        case SS_ERROR_NONE:
            ss_error_p->cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_NONE;
            ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_NONE;
            CN_LOG_D("SS_ERROR_NONE code = %d", ss_error_p->cn_ss_error_value.cn_ss_error_code);
            break;
        case SS_ERROR_TYPE_GSM_PASSWORD:

            if (ss_error_type_value_p->error_value == MAL_SS_GSM_BAD_PASSWORD) {
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_BAD_PASSWORD;
                CN_LOG_D("SS_ERROR_TYPE_GSM_PASSWORD code = %d", ss_error_p->cn_ss_error_value.cn_ss_error_code);
            } else if (ss_error_type_value_p->error_value == MAL_SS_GSM_BAD_PASSWORD_FORMAT) {
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_BAD_PASSWORD_FORMAT;
                CN_LOG_D("SS_ERROR_TYPE_GSM_PASSWORD code = %d", ss_error_p->cn_ss_error_value.cn_ss_error_code);
            }

            break;
        case SS_ERROR_TYPE_GSM_MSG:

            if (ss_error_type_value_p->error_value == MAL_SS_GSM_MSG_INCORRECT) {
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_MSG_INCORRECT;
                CN_LOG_D("SS_ERROR_TYPE_GSM_MSG code = %d", ss_error_p->cn_ss_error_value.cn_ss_error_code);
            }

            break;
        case SS_ERROR_TYPE_MISC:

            switch (ss_error_type_value_p->error_value) {
            case MAL_SS_SERVICE_BUSY:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_SERVICE_BUSY;
                break;
            case MAL_SS_GSM_DATA_ERROR:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_DATA_ERROR;
                break;
            case MAL_SS_GSM_REQUEST_CANCELLED:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_REQUEST_CANCELLED;
                break;
            case MAL_SS_GSM_MM_ERROR:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_MM_ERROR;
                break;
            case MAL_SS_GSM_SERVICE_NOT_ON_FDN_LIST:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_SERVICE_NOT_ON_FDN_LIST;
                break;
            case MAL_SS_CS_INACTIVE:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_CS_INACTIVE;
                break;
            case MAL_SS_GSM_SS_NOT_AVAILABLE:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_SS_NOT_AVAILABLE;
                break;
            case MAL_SS_RESOURCE_CONTROL_DENIED:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_RESOURCE_CONTROL_DENIED;
                break;
            case MAL_SS_RESOURCE_CONTROL_FAILURE:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_RESOURCE_CONTROL_FAILURE;
                break;
            case MAL_SS_RESOURCE_CONTROL_CONF_FAIL:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_RESOURCE_CONTROL_CONF_FAIL;
                break;
            case MAL_SS_GSM_USSD_BUSY:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_GSM_USSD_BUSY;
                break;
            case MAL_SS_SERVICE_REQUEST_RELEASED:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_SERVICE_REQUEST_RELEASED;
                break;
            case MAL_SS_SERVICE_UNKNOWN_ERROR:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_SERVICE_UNKNOWN_ERROR;
                break;
            case MAL_SS_SERVICE_NOT_SUPPORTED:
                ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_SERVICE_NOT_SUPPORTED;
                break;
            default:
                CN_LOG_W("Unknown error value %d in util_convert_ss_error_type.\n", ss_error_type_value_p->error_value);
                break;
            }

            CN_LOG_D("SS_ERROR_TYPE_MISC code =%d", ss_error_p->cn_ss_error_value.cn_ss_error_code);
            break;
        case SS_ERROR_TYPE_GSM: /* 3GPP TS 24.080 specified error_value */
            ss_error_p->cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_GSM;
            ss_error_p->cn_ss_error_value.cause_value = (uint16_t) ss_error_type_value_p->error_value;
            CN_LOG_D("SS_ERROR_TYPE_GSM code =%d", ss_error_p->cn_ss_error_value.cause_value);
            break;
        case SS_ERROR_TYPE_MM: /* 3GPP TS 24.008 specified error_value */
            ss_error_p->cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_MM;
            ss_error_p->cn_ss_error_value.cause_value = (uint16_t) ss_error_type_value_p->error_value;
            CN_LOG_D("SS_ERROR_TYPE_MM code =%d", ss_error_p->cn_ss_error_value.cause_value);
            break;
        default:
            CN_LOG_W("Unknown error type %d in util_convert_ss_error_type.\n", ss_error_type_value_p->error_type);
            break;
        }
    } else if (mal_error) {
        CN_LOG_W("mal_error, internal error!");
    } else { /* No error */
        ss_error_p->cn_ss_error_code_type = CN_SS_ERROR_CODE_TYPE_NONE;
        ss_error_p->cn_ss_error_value.cn_ss_error_code = CN_SS_ERROR_CODE_NONE;
    }

    return;
}

cn_sint32_t cn_util_convert_ss_service_class_to_cn_class(cn_sint32_t ss_service_class)
{
    cn_sint32_t cn_class = 0;


    switch (ss_service_class) {
    case 1:
        cn_class = 7;
        break;
    case 0:
        cn_class = 15;
        break;
    case 10:
        cn_class = 13;
        break;
    case 11:
        cn_class = 1;
        break;
    case 12:
        cn_class = 12;
        break;
    case 13:
        cn_class = 4;
        break;
    case 16:
        cn_class = 8;
        break;
    case 19:
        cn_class = 5;
        break;
    case 17:
        cn_class = -1;
        break;
    case 18:
        cn_class = -1;
        break;
    case 20:
        cn_class = 2;
        break;
    case 21:
        cn_class = 160;
        break;
    case 22:
        cn_class = 80;
        break;
    case 24:
        cn_class = 16;
        break;
    case 25:
        cn_class = 32;
        break;
    case 99:
        cn_class = 64;
        break;
    case 26: /* Included in 3GPP TS 02.30, not in 3GPP TS 22.030 */
        cn_class = 64;
        break;
    case 27: /* Included in 3GPP TS 02.30, not in 3GPP TS 22.030 */
        cn_class = 128;
        break;
    default:
        cn_class = 7;
        break;
    }

    CN_LOG_D("ss_service_class: %d --> cn_class: %d", ss_service_class, cn_class);

    return cn_class;
}

int convert_toa(cn_ss_ton_t ton, cn_ss_npi_t npi)
{
    int type = (128 | (ton << 4) | npi);

    CN_LOG_D("type: %d", type);

    return type;
}

int cn_util_convert_cn_ss_type_to_mal_ss_command(cn_ss_string_compounds_t *ss_data_p, cn_ss_mal_command_t *command_p)
{

    NULL_CHECK_AND_GOTO_ERROR(ss_data_p, "ss_data_p");
    NULL_CHECK_AND_GOTO_ERROR(command_p, "command_p");

    switch (ss_data_p->ss_type) {
    case CN_SS_TYPE_CALL_BARRING_BAOC:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_BARRING;
        } else {
            *command_p = SS_MAL_SET_BARRING;
        }

        break;
    case CN_SS_TYPE_CALL_BARRING_BAOIC:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_BARRING;
        } else {
            *command_p = SS_MAL_SET_BARRING;
        }

        break;
    case CN_SS_TYPE_CALL_BARRING_BAOIC_EXCL_HOME:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_BARRING;
        } else {
            *command_p = SS_MAL_SET_BARRING;
        }

        break;
    case CN_SS_TYPE_CALL_BARRING_BAIC:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_BARRING;
        } else {
            *command_p = SS_MAL_SET_BARRING;
        }

        break;
    case CN_SS_TYPE_CALL_BARRING_BAIC_ROAMING:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_BARRING;
        } else {
            *command_p = SS_MAL_SET_BARRING;
        }

        break;
    case CN_SS_TYPE_CALL_BARRING_ALL:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_BARRING;
        } else {
            *command_p = SS_MAL_SET_BARRING;
        }

        break;
    case CN_SS_TYPE_CALL_BARRING_ALL_OUTGOING:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_BARRING;
        } else {
            *command_p = SS_MAL_SET_BARRING;
        }

        break;
    case CN_SS_TYPE_CALL_BARRING_ALL_INCOMING:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_BARRING;
        } else {
            *command_p = SS_MAL_SET_BARRING;
        }

        break;
    case CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_FORWARDING;
        } else {
            *command_p = SS_MAL_SET_FORWARDING;
        }

        break;
    case CN_SS_TYPE_CALL_FORWARDING_BUSY:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_FORWARDING;
        } else {
            *command_p = SS_MAL_SET_FORWARDING;
        }

        break;
    case CN_SS_TYPE_CALL_FORWARDING_NO_REPLY:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_FORWARDING;
        } else {
            *command_p = SS_MAL_SET_FORWARDING;
        }

        break;
    case CN_SS_TYPE_CALL_FORWARDING_NOT_REACHABLE:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_FORWARDING;
        } else {
            *command_p = SS_MAL_SET_FORWARDING;
        }

        break;
    case CN_SS_TYPE_CALL_FORWARDING_ALL:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_FORWARDING;
        } else {
            *command_p = SS_MAL_SET_FORWARDING;
        }

        break;
    case CN_SS_TYPE_CALL_FORWARDING_ALL_CONDITIONAL:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_FORWARDING;
        } else {
            *command_p = SS_MAL_SET_FORWARDING;
        }

        break;
    case CN_SS_TYPE_CALL_WAITING:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_WAITING;
        } else {
            *command_p = SS_MAL_SET_WAITING;
        }

        break;
    case CN_SS_TYPE_CALLING_LINE_IDENTIFICATION_PRESENTATION:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_CLIP;
        } else {
            goto error;
        }

        break;
    case CN_SS_TYPE_CALLING_LINE_IDENTIFICATION_RESTRICTION:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_CLIR;
        } else {
            goto error;
        }

        break;
    case CN_SS_TYPE_CONNECTED_LINE_IDENTIFICATION_RESTRICTION:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_COLR;
        } else {
            goto error;
        }

        break;
    case CN_SS_TYPE_CALLING_NAME_PRESENTATION:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_CNAP;
        } else {
            goto error;
        }

        break;
    case CN_SS_TYPE_USER_TO_USER_SIGNALLING_SERVICE_1:

        if (CN_SS_PROCEDURE_TYPE_INTERROGATION == ss_data_p->procedure_type) {
            *command_p = SS_MAL_QUERY_SIGNALING;
        } else {
            *command_p = SS_MAL_SET_SIGNALING;
        }

        goto error;
        break;
    case CN_SS_TYPE_EXPLICIT_CALL_TRANSFER:
        *command_p = SS_MAL_TRANSFER;
        goto error;
        break;
    default:
        CN_LOG_E("Unknown ss_data_p->ss_type\n");
        goto error;
        break;
    }

    return 0;
error:
    return -1;
}

int cn_util_convert_cn_ss_type_to_mal_cf_reason(cn_ss_string_compounds_t *ss_data_p, cn_call_forwarding_reason_t *reason_p)
{

    NULL_CHECK_AND_GOTO_ERROR(ss_data_p, "ss_data_p");
    NULL_CHECK_AND_GOTO_ERROR(reason_p, "reason_p");

    switch (ss_data_p->ss_type) {
    case CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL:
        *reason_p = CN_CALL_FORWARDING_REASON_UNCONDITIONAL;
        break;
    case CN_SS_TYPE_CALL_FORWARDING_BUSY:
        *reason_p = CN_CALL_FORWARDING_REASON_MOBILE_BUSY;
        break;
    case CN_SS_TYPE_CALL_FORWARDING_NO_REPLY:
        *reason_p = CN_CALL_FORWARDING_REASON_NO_REPLY;
        break;
    case CN_SS_TYPE_CALL_FORWARDING_NOT_REACHABLE:
        *reason_p = CN_CALL_FORWARDING_REASON_NOT_REACHABLE;
        break;
    case CN_SS_TYPE_CALL_FORWARDING_ALL:
        *reason_p = CN_CALL_FORWARDING_REASON_ALL_CALL_FORWARDING;
        break;
    case CN_SS_TYPE_CALL_FORWARDING_ALL_CONDITIONAL:
        *reason_p = CN_CALL_FORWARDING_REASON_ALL_CONDITIONAL_CALL_FORWARDING;
        break;
    default:
        *reason_p = 0; /* this is not an error */
        break;
    }

    return 0;
error:
    return -1;
}

int cn_util_convert_cn_ss_type_to_mal_ss_facility(cn_ss_string_compounds_t *ss_data_p, char **facility_pp)
{

    NULL_CHECK_AND_GOTO_ERROR(ss_data_p,   "ss_data_p");
    NULL_CHECK_AND_GOTO_ERROR(facility_pp, "facility_pp");

    switch (ss_data_p->ss_type) {
    case CN_SS_TYPE_CALL_BARRING_BAOC:
        *facility_pp = "AO";
        break;
    case CN_SS_TYPE_CALL_BARRING_BAOIC:
        *facility_pp = "OI";
        break;
    case CN_SS_TYPE_CALL_BARRING_BAOIC_EXCL_HOME:
        *facility_pp = "OX";
        break;
    case CN_SS_TYPE_CALL_BARRING_BAIC:
        *facility_pp = "AI";
        break;
    case CN_SS_TYPE_CALL_BARRING_BAIC_ROAMING:
        *facility_pp = "IR";
        break;
    case CN_SS_TYPE_CALL_BARRING_ALL:
        *facility_pp = "AB";
        break;
    case CN_SS_TYPE_CALL_BARRING_ALL_OUTGOING:
        *facility_pp = "AG";
        break;
    case CN_SS_TYPE_CALL_BARRING_ALL_INCOMING:
        *facility_pp = "AC";
        break;
    default:
        *facility_pp = NULL; /* this is not an error */
        break;
    }

    return 0;
error:
    return -1;
}

int cn_util_convert_cn_procedure_type_to_mal_mode(cn_ss_string_compounds_t *ss_data_p, cn_sint32_t *mode_p)
{

    NULL_CHECK_AND_GOTO_ERROR(ss_data_p, "ss_data_p");
    NULL_CHECK_AND_GOTO_ERROR(mode_p, "mode_p");

    switch (ss_data_p->procedure_type) {
    case CN_SS_PROCEDURE_TYPE_DEACTIVATION:
        *mode_p = 0;
        break;
    case CN_SS_PROCEDURE_TYPE_ACTIVATION:
        *mode_p = 1;
        break;
    case CN_SS_PROCEDURE_TYPE_INTERROGATION:
        *mode_p = 2;
        break;
    case CN_SS_PROCEDURE_TYPE_REGISTRATION:
        *mode_p = 3;
        break;
    case CN_SS_PROCEDURE_TYPE_ERASURE:
        *mode_p = 4;
        break;
    default:
        CN_LOG_E("ss_data_p->procedure_type unknown! (%d)", ss_data_p->procedure_type);
        goto error;
        break;
    }

    return 0;
error:
    return -1;
}

#endif /* USE_MAL_CS */


cn_bool_t cn_util_path_present_in_fs(char *path_p)
{
    int fd = -1;
    cn_bool_t file_present = FALSE;

    NULL_CHECK_AND_GOTO_EXIT(path_p, "path_p");

    /* Check if the file can be opened (indicating whether or not the file exists) */
    fd = open(path_p, O_RDONLY);
    file_present = (-1 != fd) ? TRUE : FALSE;

    if (-1 != fd) {
        close(fd);
    }

exit:
    return file_present;
}

void cn_util_create_empty_file_in_fs(char *path_p)
{
    int fd = -1;
    int result = 0;

    /* Create empty file */
    fd = open(path_p, O_RDWR | O_CREAT);

    if (-1 == fd) {
        CN_LOG_E("unable to create \"%s\"", path_p);
        goto exit;
    }

    close(fd);

    /* Set file permissions.
     * at least read permission is needed for cn_util_path_present_in_fs()
     */
    result = chmod(path_p, S_IRUSR | S_IWUSR);

    if (0 != result) {
        CN_LOG_E("chmod failed for \"%s\"", path_p);
        goto exit;
    }

exit:
    return;
}

void set_empage_urc_issue(bool state)
{
#ifndef USE_MAL_FTD
    (void) state;
    CN_LOG_E("MAL FTD disabled");
#else
    empage_urc_issue = state;
#endif
}

bool get_empage_urc_issue(void)
{
#ifndef USE_MAL_FTD
    CN_LOG_E("MAL FTD disabled");
    return false;
#else
    return empage_urc_issue;
#endif
}

void set_empage_write_tofile(bool state)
{
#ifndef USE_MAL_FTD
    (void) state;
    CN_LOG_E("MAL FTD disabled");
#else
    empage_write_tofile = state;
#endif
}

bool get_empage_write_tofile(void)
{
#ifndef USE_MAL_FTD
    CN_LOG_E("MAL FTD disabled");
    return false;
#else
    return empage_write_tofile;
#endif
}

void set_empage_seqnr(int value)
{
#ifndef USE_MAL_FTD
    (void) value;
    CN_LOG_E("MAL FTD disabled");
#else
    empage_seqnr = value;
#endif
}

int get_empage_seqnr(void)
{
#ifndef USE_MAL_FTD
    CN_LOG_E("MAL FTD disabled");
    return 0;
#else
    return empage_seqnr;
#endif
}

void set_empage_activepage(int value)
{
#ifndef USE_MAL_FTD
    (void) value;
    CN_LOG_E("MAL FTD disabled");
#else
    empage_pageno = value;
#endif
}

int get_empage_activepage(void)
{
#ifndef USE_MAL_FTD
    CN_LOG_E("MAL FTD disabled");
    return 0;
#else
    return empage_pageno;
#endif
}

bool get_empage_activation_in_progress(void)
{
#ifndef USE_MAL_FTD
    CN_LOG_E("MAL FTD disabled");
    return false;
#else
    return empage_activation_in_progress;
#endif
}

void set_empage_activation_in_progress(bool state)
{
#ifndef USE_MAL_FTD
    (void) state;
    CN_LOG_E("MAL FTD disabled");
#else
    empage_activation_in_progress = state;
#endif
}

bool get_empage_deactivation_in_progress(void)
{
#ifndef USE_MAL_FTD
    CN_LOG_E("MAL FTD disabled");
    return false;
#else
    return empage_deactivation_in_progress;
#endif
}
void set_empage_deactivation_in_progress(bool state)
{
#ifndef USE_MAL_FTD
    (void) state;
    CN_LOG_E("MAL FTD disabled");
#else
    empage_deactivation_in_progress = state;
#endif
}

