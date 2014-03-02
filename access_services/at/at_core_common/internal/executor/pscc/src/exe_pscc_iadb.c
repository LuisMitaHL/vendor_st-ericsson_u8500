/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * IAdb  and user handling
 *
 */

#include <string.h>

#include "atc_log.h"
#include "exe_internal.h"
#include "exe_pscc_iadb.h"
#include "exe_request_record.h"

#define PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT  (psccclient_iadb_field_at_ct)
#define PSCCCLIENT_IADB_APN_SIZE_IN_BYTES            64
#define PSCCCLIENT_IADB_PDP_TYPE_SIZE_IN_BYTES        7
#define PSCCCLIENT_IADB_PDP_ADDRESS_SIZE_IN_BYTES    16
#define PSCCCLIENT_MAX_IADB_ENTRIES 25
#define PSCCCLIENT_MAX_IADB_STRING_SIZE 127

#define PSCCCLIENT_USER_USER_ID_SIZE_IN_BYTES        50
#define PSCCCLIENT_USER_PASSWD_SIZE_IN_BYTES         50
#define PSCCCLIENT_USER_AUTH_PROT_SIZE_IN_BYTES       6
#define PSCCCLIENT_MAX_USER_ENTRIES 25
#define PSCCCLIENT_ERROR_RATIO_LENGTH 3
#define PSCCCLIENT_RESIDUAL_BER_LENGTH 3

typedef long long int psccclient_set_flags_t;

struct psccclient_user_s {
    psccclient_set_flags_t      set_flags;
    int                         bearer_type;
    char                       *user_id_p;
    char                       *passwd_p;
    pscc_auth_method_t          auth_prot;
    int                         ask4passwd;
};

struct psccclient_iadb_s {
    psccclient_set_flags_t  set_flags;
    psccclient_set_flags_t  set_qos_flags;

    int                     conn_id; /* actual connection id, secondary key */

    /* cgdcont */
    char                   *apn_p;
    int                     pdp_type;
    int                     d_comp;
    int                     h_comp;
    char                   *pdp_address_p;
    char                   *ipv6_pdp_address_p;
    /* eppsd  */
    int                     state;
    int                     channel_id;  /* should be same as conn_id, but is what is suggested to pscc. If already in use, pscc will give another. */
    int                     activation_cause;

    /* eiaauw */
    int                     user_index;    /* referring to the user for this account */

    /* internal AT specifics */
    int                     at_ct;    /* request_record_t* to record related to current AT being executed. */

    /* internal AT Qos parameter to keep track on which parameter set to use when connecting. True=use rel97 parameters (2G), false=use rel 99 (3G) */
    int                     use_rel_97_qos;

    /* cgeqmin */
    int                     min_traffic_class;
    unsigned long           min_max_bitrate_ul;
    unsigned long           min_max_bitrate_dl;
    unsigned long           min_guaranteed_bitrate_ul;
    unsigned long           min_guaranteed_bitrate_dl;
    int                     min_delivery_order;
    unsigned long           min_max_sdu_size;
    char                    min_sdu_error_ratio_p[PSCCCLIENT_ERROR_RATIO_LENGTH];
    char                    min_residual_ber_p[PSCCCLIENT_RESIDUAL_BER_LENGTH];
    int                     min_delivery_err_sdu;
    unsigned long           min_transfer_delay;
    int                     min_traffic_handling_priority;
    int                     min_source_statistics_descriptor;
    int                     min_signalling_indication;

    /* cgeqreq */
    int                     req_traffic_class;
    unsigned long           req_max_bitrate_ul;
    unsigned long           req_max_bitrate_dl;
    unsigned long           req_guaranteed_bitrate_ul;
    unsigned long           req_guaranteed_bitrate_dl;
    int                     req_delivery_order;
    unsigned long           req_max_sdu_size;
    char                    req_sdu_error_ratio_p[PSCCCLIENT_ERROR_RATIO_LENGTH];
    char                    req_residual_ber_p[PSCCCLIENT_RESIDUAL_BER_LENGTH];
    int                     req_delivery_err_sdu;
    unsigned long           req_transfer_delay;
    int                     req_traffic_handling_priority;
    int                     req_source_statistics_descriptor;
    int                     req_signalling_indication;

    /* cgqmin */
    int                     min_precedence;
    int                     min_delay;
    int                     min_reliability;
    int                     min_peak;
    int                     min_mean;

    /* cgqreq */
    int                     req_precedence;
    int                     req_delay;
    int                     req_reliability;
    int                     req_peak;
    int                     req_mean;

    /* sterc */
    int                     sterc_handle;  /* handle used towards sterc. */

    /* euplink */
    int                     data_uplink_size;  /* Size of uplink data */
};

static struct psccclient_user_s  psccclient_user_table[PSCCCLIENT_MAX_USER_ENTRIES + 1];
static struct psccclient_iadb_s  psccclient_iadb_table[PSCCCLIENT_MAX_IADB_ENTRIES + 1];

/* This function returns a list of cids that only exists in local iadb,
 * i.e. cids for which AT+CGDCONT has been made, but not AT*EPPSD,
 * and there for not created in PSCC */
int *psccclient_iadb_list_cids_without_connection_ids(int *connection_ids_data_list_p, int connection_ids_length, int *cids_length_p)
{
    int *cids_to_exclude_p = NULL;
    int *cids_p = NULL;
    int i;

    if (NULL == cids_length_p || 0 != *cids_length_p) {
        ATC_LOG_E("invalid param");
        goto error;
    }

    if (NULL == connection_ids_data_list_p) {
        connection_ids_length = 0;
    }

    if (NULL != connection_ids_data_list_p && 0 != connection_ids_length) {
        cids_to_exclude_p = calloc(connection_ids_length, sizeof(int));

        if (NULL == cids_to_exclude_p) {
            ATC_LOG_E("out of memory");
            goto error;
        }

        /* find out which cids to exclude */
        for (i = 0; i < connection_ids_length; i++) {
            if (!psccclient_iadb_get_cid_on_conn_id(connection_ids_data_list_p[i], &cids_to_exclude_p[i])) {
                ATC_LOG_E("psccclient_iadb_get_cid_on_conn_id failed");
            }
        }
    }

    cids_p = calloc(PSCCCLIENT_MAX_IADB_ENTRIES + 1, sizeof(int));

    if (NULL == cids_p) {
        ATC_LOG_E("out of memory");
        goto error;
    }

    for (i = 1; i <= PSCCCLIENT_MAX_IADB_ENTRIES; i++) {
        bool exclude = false;
        int j;

        for (j = 0; j < connection_ids_length; j++) {
            if (i == cids_to_exclude_p[j]) {
                exclude = true;
                break;
            }
        }

        if (!exclude) {
            /* cid is valid if set_flags is not 0 */
            if ((0 != psccclient_iadb_table[i].set_flags) || (0 != psccclient_iadb_table[i].set_qos_flags)) {
                cids_p[*cids_length_p] = i;
                (*cids_length_p)++;
            }
        }
    }

    if (NULL != cids_to_exclude_p) {
        free(cids_to_exclude_p);
    }

    if (0 == *cids_length_p) {
        free(cids_p);
        cids_p = NULL;
    }

    return cids_p;
error:

    if (NULL != cids_to_exclude_p) {
        free(cids_to_exclude_p);
    }

    free(cids_p);

    return NULL;
}

void psccclient_iadb_remove_cid(int cid)
{
    if (NULL != psccclient_iadb_table[cid].apn_p) {
        free(psccclient_iadb_table[cid].apn_p);
    }

    if (NULL != psccclient_iadb_table[cid].pdp_address_p) {
        free(psccclient_iadb_table[cid].pdp_address_p);
    }

    if (NULL != psccclient_iadb_table[cid].ipv6_pdp_address_p) {
        free(psccclient_iadb_table[cid].ipv6_pdp_address_p);
    }

    memset(&psccclient_iadb_table[cid], 0, sizeof(struct psccclient_iadb_s));
}

void psccclient_iadb_remove_all()
{
    int i;

    for (i = 1; i <= PSCCCLIENT_MAX_IADB_ENTRIES; i++) {
        psccclient_iadb_remove_cid(i);
    }
}

bool psccclient_cid_valid(int cid)
{
    if (cid > 0 &&
            cid <= PSCCCLIENT_MAX_IADB_ENTRIES) {
        return true;
    } else {
        return false;
    }
}

bool psccclient_user_cid_valid(int cid)
{
    if (cid > 0 &&
            cid <= PSCCCLIENT_MAX_USER_ENTRIES) {
        return true;
    } else {
        return false;
    }
}

bool psccclient_iadb_remove_conn_id(int cid)
{
    if (!psccclient_cid_valid(cid)) {
        return false;
    }

    psccclient_iadb_table[cid].conn_id = 0;
    psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags & ~((psccclient_set_flags_t)2 << psccclient_iadb_field_conn_id));
    return true;
}

bool psccclient_iadb_remove_sterc_handle(int cid)
{
    if (!psccclient_cid_valid(cid)) {
        return false;
    }

    psccclient_iadb_table[cid].sterc_handle = 0;
    psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags & ~((psccclient_set_flags_t)2 << psccclient_iadb_field_sterc_handle));
    return true;
}


bool psccclient_iadb_set(int cid, psccclient_iadb_field_e field, void *value_p)
{
    bool result = true;

    if (!psccclient_cid_valid(cid)) {
        return false;
    }

    switch (field) {
    case psccclient_iadb_field_conn_id:
        psccclient_iadb_table[cid].conn_id = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_conn_id));
        break;
    case psccclient_iadb_field_apn_p:

        if (NULL != psccclient_iadb_table[cid].apn_p) {
            free(psccclient_iadb_table[cid].apn_p);
        }

        if (NULL != *((char **)value_p)) {
            psccclient_iadb_table[cid].apn_p = (char *)calloc(1, strlen(*((char **)value_p)) + 1);

            if (NULL == psccclient_iadb_table[cid].apn_p) {
                ATC_LOG_E("could not allocate memory");
                result = false;
                break;
            }

            strcpy(psccclient_iadb_table[cid].apn_p , (char *)(*(char **)value_p));
        } else {
            psccclient_iadb_table[cid].apn_p = NULL;
        }

        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_apn_p));
        break;
    case psccclient_iadb_field_pdp_type:
        psccclient_iadb_table[cid].pdp_type = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_pdp_type));
        break;
    case psccclient_iadb_field_d_comp:
        psccclient_iadb_table[cid].d_comp = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_d_comp));
        break;
    case psccclient_iadb_field_h_comp:
        psccclient_iadb_table[cid].h_comp = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_h_comp));
        break;
    case psccclient_iadb_field_pdp_address_p:

        if (NULL != psccclient_iadb_table[cid].pdp_address_p) {
            free(psccclient_iadb_table[cid].pdp_address_p);
        }

        if (NULL != *((char **)value_p)) {
            psccclient_iadb_table[cid].pdp_address_p = (char *)calloc(1, strlen(*((char **)value_p)) + 1);

            if (NULL == psccclient_iadb_table[cid].pdp_address_p) {
                ATC_LOG_E("could not allocate memory");
                result = false;
                break;
            }

            strcpy(psccclient_iadb_table[cid].pdp_address_p, *((char **)value_p));
        } else {
            psccclient_iadb_table[cid].pdp_address_p = NULL;
        }
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_pdp_address_p));
        break;

    case psccclient_iadb_field_ipv6_pdp_address_p:

        if (NULL != psccclient_iadb_table[cid].ipv6_pdp_address_p) {
            free(psccclient_iadb_table[cid].ipv6_pdp_address_p);
        }

        if (NULL != *((char **)value_p)) {
            psccclient_iadb_table[cid].ipv6_pdp_address_p = (char *)calloc(1, strlen(*((char **)value_p)) + 1);

            if (NULL == psccclient_iadb_table[cid].ipv6_pdp_address_p) {
                ATC_LOG_E("could not allocate memory");
                result = false;
                break;
            }

            strcpy(psccclient_iadb_table[cid].ipv6_pdp_address_p, *((char **)value_p));
        } else {
            psccclient_iadb_table[cid].ipv6_pdp_address_p = NULL;
        }

        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_ipv6_pdp_address_p));
        break;
    case psccclient_iadb_field_channel_id:
        psccclient_iadb_table[cid].channel_id = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_channel_id));
        break;
    case psccclient_iadb_field_user_index:
        psccclient_iadb_table[cid].user_index = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_user_index));
        break;
    case psccclient_iadb_field_at_ct:
        psccclient_iadb_table[cid].at_ct = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_at_ct));
        break;
    case psccclient_iadb_field_sterc_handle:
        psccclient_iadb_table[cid].sterc_handle = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_sterc_handle));
        break;
    case psccclient_iadb_field_use_rel_97_qos:
        ATC_LOG_I("Setting psccclient_iadb_field_use_rel_97_qos to %d", *((int *)value_p));
        psccclient_iadb_table[cid].use_rel_97_qos = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_use_rel_97_qos - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_traffic_class:
        psccclient_iadb_table[cid].min_traffic_class = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_traffic_class - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_max_bitrate_ul:
        psccclient_iadb_table[cid].min_max_bitrate_ul = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_max_bitrate_ul - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_max_bitrate_dl:
        psccclient_iadb_table[cid].min_max_bitrate_dl = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_max_bitrate_dl - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_guaranteed_bitrate_ul:
        psccclient_iadb_table[cid].min_guaranteed_bitrate_ul = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_guaranteed_bitrate_ul - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_guaranteed_bitrate_dl:
        psccclient_iadb_table[cid].min_guaranteed_bitrate_dl = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_guaranteed_bitrate_dl - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_delivery_order:
        psccclient_iadb_table[cid].min_delivery_order = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_delivery_order - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_max_sdu_size:
        psccclient_iadb_table[cid].min_max_sdu_size = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_max_sdu_size - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_sdu_error_ratio_p: {
        if (NULL != *((char **)value_p)) {
            strncpy(psccclient_iadb_table[cid].min_sdu_error_ratio_p , (char *)(*(char **)value_p), PSCCCLIENT_ERROR_RATIO_LENGTH);
        } else {
            memset(psccclient_iadb_table[cid].min_sdu_error_ratio_p, 0, PSCCCLIENT_ERROR_RATIO_LENGTH);
        }

        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_sdu_error_ratio_p - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    }
    case psccclient_iadb_field_min_residual_ber_p:

        if (NULL != *((char **)value_p)) {
            strncpy(psccclient_iadb_table[cid].min_residual_ber_p, (char *)(*(char **)value_p), PSCCCLIENT_RESIDUAL_BER_LENGTH);
        } else {
            memset(psccclient_iadb_table[cid].min_residual_ber_p, 0, PSCCCLIENT_RESIDUAL_BER_LENGTH);
        }

        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_residual_ber_p - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_delivery_err_sdu:
        psccclient_iadb_table[cid].min_delivery_err_sdu = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_delivery_err_sdu - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_transfer_delay:
        psccclient_iadb_table[cid].min_transfer_delay = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_transfer_delay - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_traffic_handling_priority:
        psccclient_iadb_table[cid].min_traffic_handling_priority = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_traffic_handling_priority - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_source_statistics_descriptor:
        psccclient_iadb_table[cid].min_source_statistics_descriptor = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_source_statistics_descriptor - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_signalling_indication:
        psccclient_iadb_table[cid].min_signalling_indication = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_signalling_indication - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_traffic_class:
        psccclient_iadb_table[cid].req_traffic_class = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_traffic_class - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_max_bitrate_ul:
        psccclient_iadb_table[cid].req_max_bitrate_ul = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_max_bitrate_ul - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_max_bitrate_dl:
        psccclient_iadb_table[cid].req_max_bitrate_dl = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_max_bitrate_dl - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_guaranteed_bitrate_ul:
        psccclient_iadb_table[cid].req_guaranteed_bitrate_ul = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_guaranteed_bitrate_ul - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_guaranteed_bitrate_dl:
        psccclient_iadb_table[cid].req_guaranteed_bitrate_dl = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_guaranteed_bitrate_dl - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_delivery_order:
        psccclient_iadb_table[cid].req_delivery_order = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_delivery_order - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_max_sdu_size:
        psccclient_iadb_table[cid].req_max_sdu_size = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_max_sdu_size - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_sdu_error_ratio_p: {
        if (NULL != *((char **)value_p)) {
            strncpy(psccclient_iadb_table[cid].req_sdu_error_ratio_p , (char *)(*(char **)value_p), PSCCCLIENT_ERROR_RATIO_LENGTH);
        } else {
            memset(psccclient_iadb_table[cid].req_sdu_error_ratio_p, 0, PSCCCLIENT_ERROR_RATIO_LENGTH);
        }

        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_sdu_error_ratio_p - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    }
    case psccclient_iadb_field_req_residual_ber_p:

        if (NULL != *((char **)value_p)) {
            strncpy(psccclient_iadb_table[cid].req_residual_ber_p, (char *)(*(char **)value_p), PSCCCLIENT_RESIDUAL_BER_LENGTH);
        } else {
            memset(psccclient_iadb_table[cid].req_residual_ber_p, 0, PSCCCLIENT_RESIDUAL_BER_LENGTH);
        }

        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_residual_ber_p - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_delivery_err_sdu:
        psccclient_iadb_table[cid].req_delivery_err_sdu = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_delivery_err_sdu - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_transfer_delay:
        psccclient_iadb_table[cid].req_transfer_delay = *((unsigned long *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_transfer_delay - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_traffic_handling_priority:
        psccclient_iadb_table[cid].req_traffic_handling_priority = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_traffic_handling_priority - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_source_statistics_descriptor:
        psccclient_iadb_table[cid].req_source_statistics_descriptor = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_source_statistics_descriptor - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_signalling_indication:
        psccclient_iadb_table[cid].req_signalling_indication = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_signalling_indication - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_data_uplink_size:
        psccclient_iadb_table[cid].data_uplink_size = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_data_uplink_size));
        break;
    case psccclient_iadb_field_min_precedence:
        psccclient_iadb_table[cid].min_precedence = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_precedence - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_delay:
        psccclient_iadb_table[cid].min_delay = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_delay - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_reliability:
        psccclient_iadb_table[cid].min_reliability = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_reliability - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_peak:
        psccclient_iadb_table[cid].min_peak = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_peak - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_min_mean:
        psccclient_iadb_table[cid].min_mean = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_mean - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_precedence:
        psccclient_iadb_table[cid].req_precedence = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_precedence - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_delay:
        psccclient_iadb_table[cid].req_delay = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_delay - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_reliability:
        psccclient_iadb_table[cid].req_reliability = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_reliability - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_peak:
        psccclient_iadb_table[cid].req_peak = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_peak - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_req_mean:
        psccclient_iadb_table[cid].req_mean = *((int *)value_p);
        psccclient_iadb_table[cid].set_qos_flags = (psccclient_iadb_table[cid].set_qos_flags | ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_mean - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT)));
        break;
    case psccclient_iadb_field_activation_cause:
        psccclient_iadb_table[cid].activation_cause = *((int *)value_p);
        psccclient_iadb_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_activation_cause));
        break;
    default:
        ATC_LOG_E("Attempted to set unknown parameter to iadb:field value=%d", field);
        result = false;
        break;
    }

    return result;
}


bool psccclient_iadb_get(int cid, psccclient_iadb_field_e field, void *value_p)
{
    bool result = true;

    if (value_p == NULL) {
        ATC_LOG_E("value_p is NULL");
        return false;
    }

    if (!psccclient_cid_valid(cid)) {
        ATC_LOG_E("incorrect cid=%d", cid);
        return false;
    }

    switch (field) {
    case psccclient_iadb_field_conn_id:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_conn_id)) {
            *((int *)value_p) = psccclient_iadb_table[cid].conn_id;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_apn_p:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_apn_p)) {
            *((char **)value_p) = psccclient_iadb_table[cid].apn_p;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_pdp_type:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_pdp_type)) {
            *((int *)value_p) = psccclient_iadb_table[cid].pdp_type;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_d_comp:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_d_comp)) {
            *((int *)value_p) = psccclient_iadb_table[cid].d_comp;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_h_comp:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_h_comp)) {
            *((int *)value_p) = psccclient_iadb_table[cid].h_comp;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_pdp_address_p:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_pdp_address_p)) {
            *((char **)value_p) = psccclient_iadb_table[cid].pdp_address_p;
        } else {
            result = false;
        }
        break;
    case psccclient_iadb_field_ipv6_pdp_address_p:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_ipv6_pdp_address_p)) {
            *((char **)value_p) = psccclient_iadb_table[cid].ipv6_pdp_address_p;
        } else {
            result = false;
        }
        break;
    case psccclient_iadb_field_channel_id:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_channel_id)) {
            *((int *)value_p) = psccclient_iadb_table[cid].channel_id;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_user_index:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_user_index)) {
            *((int *)value_p) = psccclient_iadb_table[cid].user_index;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_at_ct:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_at_ct)) {
            *((int *)value_p) = psccclient_iadb_table[cid].at_ct;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_sterc_handle:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_sterc_handle)) {
            *((int *)value_p) = psccclient_iadb_table[cid].sterc_handle;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_data_uplink_size:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_data_uplink_size)) {
            *((int *)value_p) = psccclient_iadb_table[cid].data_uplink_size;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_use_rel_97_qos:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_use_rel_97_qos - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].use_rel_97_qos;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_traffic_class:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_traffic_class - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_traffic_class;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_max_bitrate_ul:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_max_bitrate_ul - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_max_bitrate_ul;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_max_bitrate_dl:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_max_bitrate_dl - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_max_bitrate_dl;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_guaranteed_bitrate_ul:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_guaranteed_bitrate_ul - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_guaranteed_bitrate_ul;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_guaranteed_bitrate_dl:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_guaranteed_bitrate_dl - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_guaranteed_bitrate_dl;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_delivery_order:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_delivery_order - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_delivery_order;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_max_sdu_size:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_max_sdu_size - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_max_sdu_size;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_sdu_error_ratio_p:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_sdu_error_ratio_p - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((char **)value_p) = psccclient_iadb_table[cid].min_sdu_error_ratio_p;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_residual_ber_p:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_residual_ber_p - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((char **)value_p) = psccclient_iadb_table[cid].min_residual_ber_p;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_delivery_err_sdu:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_delivery_err_sdu - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_delivery_err_sdu;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_transfer_delay:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_transfer_delay - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_transfer_delay;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_traffic_handling_priority:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_traffic_handling_priority - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_traffic_handling_priority;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_source_statistics_descriptor:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_source_statistics_descriptor - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_source_statistics_descriptor;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_signalling_indication:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_signalling_indication - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_signalling_indication;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_traffic_class:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_traffic_class - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_traffic_class;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_max_bitrate_ul:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_max_bitrate_ul - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_max_bitrate_ul;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_max_bitrate_dl:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_max_bitrate_dl - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_max_bitrate_dl;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_guaranteed_bitrate_ul:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_guaranteed_bitrate_ul - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_guaranteed_bitrate_ul;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_guaranteed_bitrate_dl:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_guaranteed_bitrate_dl - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_guaranteed_bitrate_dl;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_delivery_order:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_delivery_order - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_delivery_order;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_max_sdu_size:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_max_sdu_size - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_max_sdu_size;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_sdu_error_ratio_p:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_sdu_error_ratio_p - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((char **)value_p) = psccclient_iadb_table[cid].req_sdu_error_ratio_p;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_residual_ber_p:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_residual_ber_p - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((char **)value_p) = psccclient_iadb_table[cid].req_residual_ber_p;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_delivery_err_sdu:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_delivery_err_sdu - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_delivery_err_sdu;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_transfer_delay:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_transfer_delay - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_transfer_delay;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_traffic_handling_priority:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_traffic_handling_priority - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_traffic_handling_priority;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_source_statistics_descriptor:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_source_statistics_descriptor - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_source_statistics_descriptor;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_signalling_indication:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_signalling_indication - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_signalling_indication;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_precedence:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_precedence - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_precedence;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_delay:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_delay - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_delay;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_reliability:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_reliability - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_reliability;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_peak:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_peak - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_peak;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_min_mean:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_min_mean - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].min_mean;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_precedence:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_precedence - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_precedence;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_delay:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_delay - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_delay;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_reliability:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_reliability - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_reliability;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_peak:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_peak - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_peak;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_req_mean:

        if (psccclient_iadb_table[cid].set_qos_flags & ((psccclient_set_flags_t)2 << (psccclient_iadb_field_req_mean - PSCCCLIENT_SET_QOS_FLAG_STARTING_POINT))) {
            *((int *)value_p) = psccclient_iadb_table[cid].req_mean;
        } else {
            result = false;
        }

        break;
    case psccclient_iadb_field_activation_cause:

        if (psccclient_iadb_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_activation_cause)) {
            *((int *)value_p) = psccclient_iadb_table[cid].activation_cause;
        } else {
            result = false;
        }

        break;
    default:
        ATC_LOG_E("Attempted to get unknown parameter from iadb:field value=%d", field);
        break;
    }

    return result;
}


/*
 * USER handling
 *
 */

void psccclient_user_remove_on_index(int index)
{
    if (NULL != psccclient_user_table[index].user_id_p) {
        free(psccclient_user_table[index].user_id_p);
    }

    if (NULL != psccclient_user_table[index].passwd_p) {
        free(psccclient_user_table[index].passwd_p);
    }

    memset(&psccclient_user_table[index], 0, sizeof(struct psccclient_user_s));
}

void psccclient_user_remove_all()
{
    int i;

    for (i = 1; i <= PSCCCLIENT_MAX_USER_ENTRIES; i++) {
        psccclient_user_remove_on_index(i);
    }
}

bool psccclient_user_get(int cid, psccclient_user_field_e field, void *value_p)
{
    bool result = true;

    if (!psccclient_user_cid_valid(cid)) {
        ATC_LOG_E("incorrect cid=%d", cid);
        return false;
    }

    switch (field) {
    case psccclient_user_field_bearer_type:

        if (psccclient_user_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_user_field_bearer_type)) {
            *((int *)value_p) = psccclient_user_table[cid].bearer_type;
        } else {
            result = false;
        }

        break;
    case psccclient_user_field_user_id_p:

        if (psccclient_user_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_user_field_user_id_p)) {
            *((char **)value_p)  = psccclient_user_table[cid].user_id_p;
        } else {
            result = false;
        }

        break;
    case psccclient_user_field_passwd_p:

        if (psccclient_user_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_user_field_passwd_p)) {
            *((char **)value_p)  = psccclient_user_table[cid].passwd_p;
        } else {
            result = false;
        }

        break;
    case psccclient_user_field_auth_prot:

        if (psccclient_user_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_user_field_auth_prot)) {
            *((int *)value_p) = psccclient_user_table[cid].auth_prot;
        } else {
            result = false;
        }

        break;
    case psccclient_user_field_ask4passwd:

        if (psccclient_user_table[cid].set_flags & ((psccclient_set_flags_t)2 << psccclient_user_field_ask4passwd)) {
            *((int *)value_p) = psccclient_user_table[cid].ask4passwd;
        } else {
            result = false;
        }

        break;
    default:
        result = false;
        break;
    }

    return result;
}



bool psccclient_user_set(int cid, psccclient_user_field_e field, void *value_p)
{
    bool result = true;

    if (!psccclient_user_cid_valid(cid)) {
        ATC_LOG_E("incorrect cid=%d", cid);
        return false;
    }

    switch (field) {
    case psccclient_user_field_bearer_type:
        psccclient_user_table[cid].bearer_type = *((int *)value_p);
        psccclient_user_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_user_field_bearer_type));
        break;
    case psccclient_user_field_user_id_p:

        if (NULL !=  *((char **)value_p)) {
            free(psccclient_user_table[cid].user_id_p);
            psccclient_user_table[cid].user_id_p = (char *)calloc(1, strlen(*((char **)value_p)) + 1);
            strcpy(psccclient_user_table[cid].user_id_p, *((char **)value_p));
        } else {
            psccclient_user_table[cid].user_id_p = NULL;
        }

        psccclient_user_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_user_field_user_id_p));
        break;
    case psccclient_user_field_passwd_p:

        if (NULL !=  *((char **)value_p)) {
            free(psccclient_user_table[cid].passwd_p);
            psccclient_user_table[cid].passwd_p = (char *)calloc(1, strlen(*((char **)value_p)) + 1);
            strcpy(psccclient_user_table[cid].passwd_p, *((char **)value_p));
        } else {
            psccclient_user_table[cid].passwd_p = NULL;
        }

        psccclient_user_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_user_field_passwd_p));
        break;
    case psccclient_user_field_auth_prot:

        psccclient_user_table[cid].auth_prot = *((int *)value_p);
        psccclient_user_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_user_field_auth_prot));
        break;
    case psccclient_user_field_ask4passwd:

        psccclient_user_table[cid].ask4passwd = *((bool *)value_p);
        psccclient_user_table[cid].set_flags = (psccclient_iadb_table[cid].set_flags | ((psccclient_set_flags_t)2 << psccclient_user_field_ask4passwd));
        break;
    default:
        result = false;
        break;
    }

    return result;
}


/* special functions */

bool psccclient_iadb_get_at_ct_on_conn_id(int conn_id, int *at_ct_p)
{
    int i;
    bool result = false;

    for (i = 1; i <= PSCCCLIENT_MAX_IADB_ENTRIES; i++) {
        if (conn_id == psccclient_iadb_table[i].conn_id) {
            if (psccclient_iadb_table[i].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_at_ct)) {
                *at_ct_p = psccclient_iadb_table[i].at_ct;
                result = true;
            }

            break;
        }
    }

    return result;
}

bool psccclient_iadb_get_at_ct_on_sterc_handle(int sterc_handle, int *at_ct_p)
{
    int i;
    bool result = false;

    for (i = 1; i <= PSCCCLIENT_MAX_IADB_ENTRIES; i++) {
        if (sterc_handle == psccclient_iadb_table[i].sterc_handle) {
            if (psccclient_iadb_table[i].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_at_ct)) {
                *at_ct_p = psccclient_iadb_table[i].at_ct;
                result = true;
            }

            break;
        }
    }

    return result;
}

bool psccclient_iadb_get_cid_on_at_ct(int at_ct, int *cid_p)
{
    int i;
    bool result = false;

    for (i = 1; i <= PSCCCLIENT_MAX_IADB_ENTRIES; i++) {
        if (at_ct == psccclient_iadb_table[i].at_ct) {
            *cid_p = i;
            result = true;
            break;
        }
    }

    return result;
}

bool psccclient_iadb_get_conn_id_on_at_ct(int at_ct, int *conn_id_p)
{
    int i;
    bool result = false;

    for (i = 1; i <= PSCCCLIENT_MAX_IADB_ENTRIES; i++) {
        if (at_ct == psccclient_iadb_table[i].at_ct) {
            if (psccclient_iadb_table[i].set_flags & ((psccclient_set_flags_t)2 << psccclient_iadb_field_conn_id)) {
                *conn_id_p = psccclient_iadb_table[i].conn_id;
                result = true;
            }

            break;
        }
    }

    return result;
}

bool psccclient_iadb_get_cid_on_conn_id(int conn_id, int *cid_p)
{
    int i;
    bool result = false;

    for (i = 1; i <= PSCCCLIENT_MAX_IADB_ENTRIES; i++) {
        if (conn_id == psccclient_iadb_table[i].conn_id) {
            *cid_p = i;
            result = true;
            break;
        }
    }

    return result;
}

bool psccclient_iadb_set_conn_id_on_at_ct(int at_ct, int conn_id)
{
    int i;
    bool result = false;

    for (i = 1; i <= PSCCCLIENT_MAX_IADB_ENTRIES; i++) {
        if (at_ct == psccclient_iadb_table[i].at_ct) {
            psccclient_iadb_table[i].conn_id = conn_id;
            psccclient_iadb_table[i].set_flags = (psccclient_iadb_table[i].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_conn_id));
            result = true;
            break;
        }
    }

    return result;
}

bool psccclient_iadb_set_at_ct_on_conn_id(int at_ct, int conn_id)
{
    int i;
    bool result = false;

    for (i = 1; i <= PSCCCLIENT_MAX_IADB_ENTRIES; i++) {
        if (conn_id == psccclient_iadb_table[i].conn_id) {
            psccclient_iadb_table[i].at_ct = at_ct;
            psccclient_iadb_table[i].set_flags = (psccclient_iadb_table[i].set_flags | ((psccclient_set_flags_t)2 << psccclient_iadb_field_at_ct));
            result = true;
            break;
        }
    }

    return result;
}

int psccclient_iadb_get_set_flags_from_cid(int cid)
{
    if (!psccclient_cid_valid(cid)) {
        return 0;
    }

    return psccclient_iadb_table[cid].set_flags;
}

int psccclient_iadb_get_define_PSCCCLIENT_MAX_IADB_ENTRIES(void)
{
    return PSCCCLIENT_MAX_IADB_ENTRIES;
}

bool psccclient_iadb_existing_cid(int cid)
{
    if (!psccclient_cid_valid(cid)) {
        return false;
    }

    if ((psccclient_iadb_table[cid].set_flags > 0) || (psccclient_iadb_table[cid].set_qos_flags > 0)) {
        return true;
    } else {
        return false;
    }
}
