/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * IAdb  and user handling
 *
 * */
#ifndef EXE_PSCC_IADB_H
#define EXE_PSCC_IADB_H 1

typedef enum {
    psccclient_iadb_field_undefined,
    psccclient_iadb_field_conn_id,              /*1*/  /*actual conn_id given by pscc*/
    psccclient_iadb_field_apn_p,                /*2*/
    psccclient_iadb_field_pdp_type,             /*3*/
    psccclient_iadb_field_d_comp,               /*4*/
    psccclient_iadb_field_h_comp,               /*5*/
    psccclient_iadb_field_pdp_address_p,        /*6*/
    psccclient_iadb_field_ipv6_pdp_address_p,   /*7*/
    psccclient_iadb_field_channel_id,           /*8*/  /*desired conn_id */
    psccclient_iadb_field_user_index,           /*9*/
    psccclient_iadb_field_at_ct,                /*10*/
    psccclient_iadb_field_data_uplink_size,
    psccclient_iadb_field_sterc_handle,
    psccclient_iadb_field_use_rel_97_qos,
    psccclient_iadb_field_min_traffic_class,
    psccclient_iadb_field_min_max_bitrate_ul,
    psccclient_iadb_field_min_max_bitrate_dl,
    psccclient_iadb_field_min_guaranteed_bitrate_ul,
    psccclient_iadb_field_min_guaranteed_bitrate_dl,
    psccclient_iadb_field_min_delivery_order,
    psccclient_iadb_field_min_max_sdu_size,
    psccclient_iadb_field_min_sdu_error_ratio_p,
    psccclient_iadb_field_min_residual_ber_p,
    psccclient_iadb_field_min_delivery_err_sdu,
    psccclient_iadb_field_min_transfer_delay,
    psccclient_iadb_field_min_traffic_handling_priority,
    psccclient_iadb_field_min_source_statistics_descriptor,
    psccclient_iadb_field_min_signalling_indication,
    psccclient_iadb_field_req_traffic_class,
    psccclient_iadb_field_req_max_bitrate_ul,
    psccclient_iadb_field_req_max_bitrate_dl,
    psccclient_iadb_field_req_guaranteed_bitrate_ul,
    psccclient_iadb_field_req_guaranteed_bitrate_dl,
    psccclient_iadb_field_req_delivery_order,
    psccclient_iadb_field_req_max_sdu_size,
    psccclient_iadb_field_req_sdu_error_ratio_p,
    psccclient_iadb_field_req_residual_ber_p,
    psccclient_iadb_field_req_delivery_err_sdu,
    psccclient_iadb_field_req_transfer_delay,
    psccclient_iadb_field_req_traffic_handling_priority,
    psccclient_iadb_field_req_source_statistics_descriptor,
    psccclient_iadb_field_req_signalling_indication,
    psccclient_iadb_field_min_precedence,
    psccclient_iadb_field_min_delay,
    psccclient_iadb_field_min_reliability,
    psccclient_iadb_field_min_peak,
    psccclient_iadb_field_min_mean,
    psccclient_iadb_field_req_precedence,
    psccclient_iadb_field_req_delay,
    psccclient_iadb_field_req_reliability,
    psccclient_iadb_field_req_peak,
    psccclient_iadb_field_req_mean,
    psccclient_iadb_field_activation_cause,
    psccclient_iadb_field_enum_max_value
} psccclient_iadb_field_e;

typedef enum {
    psccclient_user_field_undefined,
    psccclient_user_field_bearer_type,          /*1*/
    psccclient_user_field_user_id_p,            /*2*/
    psccclient_user_field_passwd_p,             /*3*/
    psccclient_user_field_auth_prot,            /*4*/
    psccclient_user_field_ask4passwd,           /*5*/
    psccclient_user_field_enum_max_value
} psccclient_user_field_e;

/*  functions  */
bool psccclient_cid_valid(int cid);
bool psccclient_user_cid_valid(int cid);
void psccclient_iadb_remove_cid(int cid);
void psccclient_iadb_remove_all();
bool psccclient_iadb_remove_conn_id(int cid);
bool psccclient_iadb_remove_sterc_handle(int cid);

bool psccclient_iadb_set(int cid, psccclient_iadb_field_e field, void *value_p);
bool psccclient_iadb_get(int cid, psccclient_iadb_field_e field, void *value_p);
bool psccclient_user_get(int cid, psccclient_user_field_e field, void *value_p);
bool psccclient_user_set(int cid, psccclient_user_field_e field, void *value_p);
void psccclient_user_remove_all();

/* special functions */
bool psccclient_iadb_get_at_ct_on_conn_id(int conn_id, int *at_ct_p);
bool psccclient_iadb_get_cid_on_at_ct(int at_ct, int *cid_p);
bool psccclient_iadb_get_conn_id_on_at_ct(int at_ct, int *conn_id);
bool psccclient_iadb_get_cid_on_conn_id(int conn_id, int *cid);
bool psccclient_iadb_set_conn_id_on_at_ct(int at_ct, int conn_id);
bool psccclient_iadb_set_at_ct_on_conn_id(int at_ct, int conn_id);
bool psccclient_iadb_get_at_ct_on_sterc_handle(int sterc_handle, int *at_ct_p);
int *psccclient_iadb_list_cids_without_connection_ids(int *connection_ids_data_list_p, int connection_ids_length, int *cids_length_p);
int psccclient_iadb_get_set_flags_from_cid(int cid);
int psccclient_iadb_get_define_PSCCCLIENT_MAX_IADB_ENTRIES(void);
bool psccclient_iadb_existing_cid(int cid);

#endif
