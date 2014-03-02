/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef WB_TEST_H
#define WB_TEST_H 1

/* Test case specific defines */
#define FD_CN_REQUEST    10
#define FD_CN_EVENT      11
#define FD_SMS_REQUEST   20
#define FD_SMS_EVENT     21
#define FD_PSCC_REQUEST  30
#define FD_PSCC_EVENT    31
#define FD_SIM_REQUEST   40
#define FD_SIM_EVENT     41
#define FD_SIMPB_REQUEST 42
#define FD_STERC_REQUEST 32
#define FD_STERC_EVENT   33
#define CLEAN_UP_BUFFER_SIZE 80
#define PHONE_NUMBER_INT "+4646143000"
#define PHONE_NUMBER     "046143099"

typedef enum {
    wb_testcase_setup,
    wb_testcase_a_do,
    wb_testcase_a_test,
    wb_testcase_and_c,
    wb_testcase_and_d,
    wb_testcase_cbm,
    wb_testcase_cds,
    wb_testcase_cdsi,
    wb_testcase_cnmi,
    wb_testcase_cnmi_read,
    wb_testcase_cnmi_test,
    wb_testcase_ccfc_set_1,
    wb_testcase_ccfc_set_2,
    wb_testcase_ccfc_set_query,
    wb_testcase_ccfc_set_register,
    wb_testcase_ccfc_test,
    wb_testcase_csim_test,
    wb_testcase_csim_set_1,
    wb_testcase_csim_set_1_1,
    wb_testcase_csim_set_2,
    wb_testcase_csim_set_2_1,
    wb_testcase_csim_set_3,
    wb_testcase_csim_set_4,
    wb_testcase_csim_set_5,
    wb_testcase_cchc_test,
    wb_testcase_cchc_set1,
    wb_testcase_cchc_set2,
    wb_testcase_cchc_set3,
    wb_testcase_cchc_set4,
    wb_testcase_ccho_test,
    wb_testcase_ccho_set1,
    wb_testcase_ccho_set2,
    wb_testcase_ccho_set3,
    wb_testcase_cgla_test,
    wb_testcase_cgla_set_1,
    wb_testcase_cgla_set_2,
    wb_testcase_cgla_set_3,
    wb_testcase_cgla_set_4,
    wb_testcase_cgla_set_5,
    wb_testcase_cgla_set_6,
    wb_testcase_cmec_1,
    wb_testcase_cmec_2,
    wb_testcase_cmec_3,
    wb_testcase_cmec_failure,
    wb_testcase_cmec_read,
    wb_testcase_cmec_test,
    wb_testcase_cmee_set_0,
    wb_testcase_cmee_set_default,
    wb_testcase_cmee_set_1,
    wb_testcase_cmee_set_2,
    wb_testcase_cmee_failure_1,
    wb_testcase_cmee_failure_2,
    wb_testcase_cmee_read,
    wb_testcase_cmee_test,
    wb_testcase_cmee_configure,
    wb_testcase_mal_client_tag,
    wb_testcase_cmgd,
    wb_testcase_cmgd1,
    wb_testcase_cmgd2,
    wb_testcase_cmgd3,
    wb_testcase_cmgd4,
    wb_testcase_cmgd5,
    wb_testcase_cmgd6,
    wb_testcase_cmgd7,
    wb_testcase_ccwa_unsol_1,
    wb_testcase_ccwa_unsol_2,
    wb_testcase_ccwa_set1,
    wb_testcase_ccwa_set2,
    wb_testcase_ccwa_set3,
    wb_testcase_ccwa_set4,
    wb_testcase_ccwa_set_query,
    wb_testcase_ccwa_query_fail1,
    wb_testcase_ccwa_query_fail2,
    wb_testcase_ccwa_query_fail3,
    wb_testcase_ccwa_read,
    wb_testcase_ccwa_test,
    wb_testcase_ceer_d_busy,
    wb_testcase_ceer_d_fcn_bad,
    wb_testcase_ceer_a_barred_cug,
    wb_testcase_ceer_h_ok,
    wb_testcase_ceer_h_ok_unspec,
    wb_testcase_ceer_h_no_call,
    wb_testcase_ceer_test,
    wb_testcase_ceer_disc_normal,
    wb_testcase_ceer_disc_channel_lost,
    wb_testcase_ceer_unsol_gsm,
    wb_testcase_ceer_unsol_ss_fail,
    wb_testcase_ceer_unsol_pscc,
    wb_testcase_cfun_1,
    wb_testcase_cfun_2,
    wb_testcase_cfun_3,
    wb_testcase_cfun_4,
    wb_testcase_cfun_failure,
    wb_testcase_cfun_rf_off,
    wb_testcase_cfun_rf_transition,
    wb_testcase_cfun_rf_preferred_wcdma,
    wb_testcase_cfun_rf_gsm_only,
    wb_testcase_cfun_rf_wcdma_only,
    wb_testcase_cfun_rf_gsm_wcdma_auto,
    wb_testcase_cfun_rf_cdma_evdo_auto,
    wb_testcase_cfun_rf_cdma_only,
    wb_testcase_cfun_rf_evdo_only,
    wb_testcase_cfun_rf_all_auto,
    wb_testcase_cfun_read_failure,
    wb_testcase_cgact_set_3gpp,
    wb_testcase_cgact_set_3gpp_nw_disconnect,
    wb_testcase_cgact_set_3gpp_failure,
    wb_testcase_cgact_set_two_cids,
    wb_testcase_cgatt_attach,
    wb_testcase_cgatt_detach,
    wb_testcase_cgatt_attach_fail,
    wb_testcase_cgatt_detach_fail,
    wb_testcase_cgatt_attach_when_attached,
    wb_testcase_cgatt_detach_when_detached,
    wb_testcase_cgatt_gprs_update_abort,
    wb_testcase_cgatt_get_status,
    wb_testcase_cgatt_is_supported,
    wb_testcase_cgatt_gprs_abort,
    wb_testcase_cgcmod_set1,
    wb_testcase_cgcmod_set2,
    wb_testcase_cgcmod_set3,
    wb_testcase_cgcmod_test,
    wb_testcase_cgdcont_1,
    wb_testcase_cgdcont_2,
    wb_testcase_cgdcont_3,
    wb_testcase_cgdcont_4,
    wb_testcase_cdcont_cleanup,
    wb_testcase_cgdcont_read,
    wb_testcase_cgdcont_parsing,
    wb_testcase_cgdcont_test,
    wb_testcase_cgqmin,
    wb_testcase_cgqreq,
    wb_testcase_cgeqmin,
    wb_testcase_cgeqmin_partialset,
    wb_testcase_cgeqreq,
    wb_testcase_cgeqreq_partialset,
    wb_testcase_cgeqneg,
    wb_testcase_cgerep_set_test_neg1,
    wb_testcase_cgerep_set_test_neg2,
    wb_testcase_cgerep_set_test_neg3,
    wb_testcase_cgerep_set_test1,
    wb_testcase_cgerep_set_test2,
    wb_testcase_cgerep_set_test3,
    wb_testcase_cgerep_set_test4,
    wb_testcase_cgerep_read_test,
    wb_testcase_cgerep_test_test,
    wb_testcase_cgmi_do,
    wb_testcase_cgmi_test,
    wb_testcase_cgmi_fail,
    wb_testcase_cgmm_do,
    wb_testcase_cgmm_test,
    wb_testcase_cgmm_fail,
    wb_testcase_cgmr_do,
    wb_testcase_cgmr_test,
    wb_testcase_cgmr_fail,
    wb_testcase_gmm_do,
    wb_testcase_gmm_test,
    wb_testcase_gmm_fail,
    wb_testcase_gmr_do,
    wb_testcase_gmr_test,
    wb_testcase_gmr_fail,
    wb_testcase_cgpaddr_0,
    wb_testcase_cgpaddr_1,
    wb_testcase_cgpaddr_2,
    wb_testcase_cgpaddr_all,
    wb_testcase_cgreg_reg_presentation_1,
    wb_testcase_cgreg_reg_presentation_2,
    wb_testcase_cgreg_reg_loc_presentation_1,
    wb_testcase_cgreg_reg_loc_presentation_2,
    wb_testcase_cgreg_reg_loc_presentation_3,
    wb_testcase_cgreg_is_supported,
    wb_testcase_cgreg_unsolicited_1,
    wb_testcase_cgreg_unsolicited_2,
    wb_testcase_cgreg_unsolicited_3,
    wb_testcase_cgreg_unsolicited_4,
    wb_testcase_cgreg_unsolicited_5,
    wb_testcase_cgsms_set,
    wb_testcase_cgsms_set_fail,
    wb_testcase_cgsms_read,
    wb_testcase_cgsms_test,
    wb_testcase_cgsn_do,
    wb_testcase_cgsn_test,
    wb_testcase_chld_0,
    wb_testcase_chld_1,
    wb_testcase_chld_2,
    wb_testcase_chld_3,
    wb_testcase_chld_4,
    wb_testcase_chld_13,
    wb_testcase_chld_22,
    wb_testcase_chld_5,
    wb_testcase_chld_29,
    wb_testcase_chld_read,
    wb_testcase_chld_test,
    wb_testcase_cimi_test,
    wb_testcase_cimi_do,
    wb_testcase_clcc_0,
    wb_testcase_clcc_1,
    wb_testcase_clcc_2,
    wb_testcase_clip_neg_1,
    wb_testcase_clip_0,
    wb_testcase_clip_1,
    wb_testcase_clip_2,
    wb_testcase_clip_read,
    wb_testcase_clip_test,
    wb_testcase_clir_read,
    wb_testcase_clir_set,
    wb_testcase_cmer_set_1,
    wb_testcase_cmer_set_2,
    wb_testcase_cmer_set_3,
    wb_testcase_cmer_set_4,
    wb_testcase_cmer_set_5,
    wb_testcase_cmer_set_6,
    wb_testcase_cmer_set_7,
    wb_testcase_cmer_set_8,
    wb_testcase_cmer_set_9,
    wb_testcase_cmer_set_10,
    wb_testcase_cmer_set_11,
    wb_testcase_cmer_set_12,
    wb_testcase_cmer_set_13,
    wb_testcase_cmer_set_14,
    wb_testcase_cmer_set_neg_1,
    wb_testcase_cmer_set_neg_2,
    wb_testcase_cmer_set_neg_3,
    wb_testcase_cmer_set_neg_4,
    wb_testcase_cmer_set_neg_5,
    wb_testcase_cmer_set_neg_6,
    wb_testcase_cmer_set_neg_7,
    wb_testcase_cmer_set_neg_8,
    wb_testcase_cmer_set_neg_9,
    wb_testcase_cmer_set_neg_10,
    wb_testcase_cmer_set_neg_11,
    wb_testcase_cmer_set_neg_12,
    wb_testcase_cmer_read,
    wb_testcase_cmer_test,
    wb_testcase_cmer_unsol_ciev_1,
    wb_testcase_cmer_unsol_ciev_2,
    wb_testcase_cmer_unsol_ciev_3,
    wb_testcase_cmer_unsol_ciev_4,
    wb_testcase_cmer_unsol_ciev_5,
    wb_testcase_cmer_unsol_ciev_6,
    wb_testcase_cmer_unsol_ciev_disabled,
    wb_testcase_cmt,
    wb_testcase_cmti,
    wb_testcase_cmgf_set,
    wb_testcase_cmgf_failure_1,
    wb_testcase_cmgf_failure_2,
    wb_testcase_cmgf_read,
    wb_testcase_cmgf_test,
    wb_testcase_cmgl_test,
    wb_testcase_cmgl,
    wb_testcase_cmgl_failure,
    wb_testcase_cmgr,
    wb_testcase_cmgr_failure,
    wb_testcase_cmgr_test,
    wb_testcase_cmgs,
    wb_testcase_cmgs_failure_1,
    wb_testcase_cmgs_cms_error,
    wb_testcase_cmgs_read,
    wb_testcase_cmgs_test,
    wb_testcase_cmgw,
    wb_testcase_cmgw_failure_1,
    wb_testcase_cmgw_failure_2,
    wb_testcase_cmgw_failure_3,
    wb_testcase_cmgw_read,
    wb_testcase_cmgw_test,
    wb_testcase_cmms_0,
    wb_testcase_cmms_1,
    wb_testcase_cmms_2,
    wb_testcase_cmms_3,
    wb_testcase_cmms_cmgs,
    wb_testcase_cmms_read,
    wb_testcase_cmms_test,
    wb_testcase_cmod_set_1,
    wb_testcase_cmod_set_2,
    wb_testcase_cmod_set_neg_1,
    wb_testcase_cmod_set_neg_2,
    wb_testcase_cmod_read,
    wb_testcase_cmod_test,
    wb_testcase_cmss_set,
    wb_testcase_cmss_set_fail,
    wb_testcase_cmss_set_new_recipient,
    wb_testcase_cmss_set_param_fail,
    wb_testcase_cmut_set_1,
    wb_testcase_cmut_set_2,
    wb_testcase_cmut_set_failure,
    wb_testcase_cmut_set,
    wb_testcase_cmut_read,
    wb_testcase_cmut_test,
    wb_testcase_cnap_set_0,
    wb_testcase_cnap_read_0,
    wb_testcase_cnap_set_1,
    wb_testcase_cnap_read_1,
    wb_testcase_cnap_set_2,
    wb_testcase_cnap_test,
    wb_testcase_cnap_unsolicited,
    wb_testcase_cscb,
    wb_testcase_cscs_set_hex,
    wb_testcase_cscs_set_utf_8,
    wb_testcase_cscs_set_failure,
    wb_testcase_cscs_set_noparameters,
    wb_testcase_cscs_short_escape,
    wb_testcase_cscs_short_empty,
    wb_testcase_cscs_short_onequote,
    wb_testcase_cscs_read_hex,
    wb_testcase_cscs_read_utf_8,
    wb_testcase_cscs_test,
    wb_testcase_cnma_0,
    wb_testcase_cnma_1,
    wb_testcase_cnma_2,
    wb_testcase_cnma_3,
    wb_testcase_cnma_failure_1,
    wb_testcase_cnma_read,
    wb_testcase_cnma_test,
    wb_testcase_cnum_do,
    wb_testcase_cnum_test,
    wb_testcase_cops_network_register,
    wb_testcase_cops_network_deregister,
    wb_testcase_cops_set_format,
    wb_testcase_cops_get_settings,
    wb_testcase_cops_network_search,
    wb_testcase_cops_network_manual_registration_with_automatic_fallback,
    wb_testcase_plmn_write,
    wb_testcase_plmn_write_illegal,
    wb_testcase_plmn_write_rejected,
    wb_testcase_plmn_write_retry,
    wb_testcase_plmn_remove_element,
    wb_testcase_plmn_read_two,
    wb_testcase_plmn_read_none,
    wb_testcase_plmn_read_failed,
    wb_testcase_plmn_read_retry,
    wb_testcase_plmn_read_retry_failed,
    wb_testcase_plmn_test,
    wb_testcase_plmn_test_failed,
    wb_testcase_cpin_test,
    wb_testcase_cpin_read,
    wb_testcase_cpin_set,
    wb_testcase_cpin_set_wrong_pin,
    wb_testcase_ecexpin_test,
    wb_testcase_ecexpin_read,
    wb_testcase_ecexpin_query_state,
    wb_testcase_ecexpin_set_puk,
    wb_testcase_ecexpin_set_incorrect_param_1,
    wb_testcase_ecexpin_set_incorrect_param_2,
    wb_testcase_ecexpin_set_incorrect_param_3,
    wb_testcase_clck_test,
    wb_testcase_clck_read,
    wb_testcase_clck_set_enable,
    wb_testcase_clck_set_enable_neg,
    wb_testcase_clck_query_neg,
    wb_testcase_clck_set_disable,
    wb_testcase_clck_set_dis_pin,
    wb_testcase_clck_set_dis_puk,
    wb_testcase_clck_set_query,
    wb_testcase_clck_set_query_neg,
    wb_testcase_clck_call_barring_lock,
    wb_testcase_clck_call_barring_unlock,
    wb_testcase_clck_call_barring_query,
    wb_testcase_colr_do,
    wb_testcase_colr_test,
    wb_testcase_colp,
    wb_testcase_colp_test,
    wb_testcase_cpms,
    wb_testcase_cpms_read,
    wb_testcase_cpms_test,
    wb_testcase_cpms_failure,
    wb_testcase_cr_set_0,
    wb_testcase_cr_set_1,
    wb_testcase_cr_set_default,
    wb_testcase_cr_set_neg,
    wb_testcase_cr_read_0,
    wb_testcase_cr_read_1,
    wb_testcase_cr_test,
    wb_testcase_cr_pscc_report,
    wb_testcase_cr,
    wb_testcase_crc,
    wb_testcase_creg_reg_presentation_1,
    wb_testcase_creg_reg_presentation_2,
    wb_testcase_creg_reg_loc_presentation_1,
    wb_testcase_creg_reg_loc_presentation_2,
    wb_testcase_creg_reg_loc_presentation_3,
    wb_testcase_creg_is_supported,
    wb_testcase_creg_unsolicited_1,
    wb_testcase_creg_unsolicited_2,
    wb_testcase_creg_unsolicited_3,
    wb_testcase_creg_unsolicited_4,
    wb_testcase_creg_unsolicited_5,
    wb_testcase_crsm_read_get_resp,
    wb_testcase_crsm_read_get_resp_2,
    wb_testcase_crsm_read_get_resp_3,
    wb_testcase_crsm_read_get_resp_4,
    wb_testcase_crsm_read_get_resp_5,
    wb_testcase_crsm_read_get_resp_6,
    wb_testcase_crsm_read_file_bin,
    wb_testcase_crsm_read_file_bin_2,
    wb_testcase_crsm_read_file_bin_3,
    wb_testcase_crsm_read_file_rec,
    wb_testcase_crsm_read_file_rec_2,
    wb_testcase_crsm_read_file_rec_3,
    wb_testcase_crsm_read_file_rec_4,
    wb_testcase_crsm_update_file_rec,
    wb_testcase_crsm_update_file_rec_1,
    wb_testcase_crsm_update_file_rec_1_2,
    wb_testcase_crsm_update_file_rec_2,
    wb_testcase_crsm_update_file_rec_3,
    wb_testcase_crsm_update_file_rec_4,
    wb_testcase_crsm_update_file_bin,
    wb_testcase_crsm_update_file_bin_1,
    wb_testcase_crsm_update_file_bin_1_2,
    wb_testcase_crsm_update_file_bin_2,
    wb_testcase_crsm_update_file_bin_3,
    wb_testcase_crsm_update_file_bin_4,
    wb_testcase_crsm_read_status,
    wb_testcase_crsm_read_status_2,
    wb_testcase_crsm_read_status_3,
    wb_testcase_crsm_test,
    wb_testcase_cuad_test,
    wb_testcase_cuad_do,
    wb_testcase_cuad_do_2,
    wb_testcase_cuad_do_3,
    wb_testcase_cssi,
    wb_testcase_cpwd_test,
    wb_testcase_cpwd_0,
    wb_testcase_cpwd_1,
    wb_testcase_cpwd_2,
    wb_testcase_cpwd_call_barring,
    wb_testcase_csca_set_smsc_1,
    wb_testcase_csca_set_smsc_2,
    wb_testcase_csca_set_smsc_3,
    wb_testcase_csca_set_smsc_4,
    wb_testcase_csca_set_smsc_5,
    wb_testcase_csca_get_smsc_1,
    wb_testcase_csca_get_smsc_2,
    wb_testcase_csca_get_smsc_3,
    wb_testcase_csca_get_smsc_4,
    wb_testcase_csca_get_smsc_5,
    wb_testcase_csca_set_smsc_failure_1,
    wb_testcase_csca_set_smsc_failure_2,
    wb_testcase_csca_get_smsc_failure_1,
    wb_testcase_csca_get_smsc_failure_2,
    wb_testcase_csca_test_is_supported,
    wb_testcase_csas_test,
    wb_testcase_csas_test_1,
    wb_testcase_csas_test_2,
    wb_testcase_csas_test_3,
    wb_testcase_csas_do,
    wb_testcase_csas_set_1,
    wb_testcase_csas_set_2,
    wb_testcase_csas_set_3,
    wb_testcase_csas_set_4,
    wb_testcase_cres_test,
    wb_testcase_cres_test_1,
    wb_testcase_cres_test_2,
    wb_testcase_cres_test_3,
    wb_testcase_cres_do,
    wb_testcase_cres_set_1,
    wb_testcase_cres_set_2,
    wb_testcase_cres_set_3,
    wb_testcase_cres_set_4,
    wb_testcase_cssi_1,
    wb_testcase_cssi_2,
    wb_testcase_csms_0,
    wb_testcase_csms_1,
    wb_testcase_csms_failure_1,
    wb_testcase_csms_failure_2,
    wb_testcase_csms_read_failure,
    wb_testcase_csms_test,
    wb_testcase_cssn,
    wb_testcase_cssu_1,
    wb_testcase_cssu_2,
    wb_testcase_cssu_3,
    wb_testcase_string_conversion,
    wb_testcase_cusd_send_ussd,
    wb_testcase_cusd_send_ussd_failure,
    wb_testcase_cusd_cancel_ussd,
    wb_testcase_cusd_get_settings,
    wb_testcase_cusd_get_supported_n,
    wb_testcase_cusd_unsolicited,
    wb_testcase_cusd_unsolicited_gsm8,
    wb_testcase_cusd_unsolicited_ucs2,
    wb_testcase_cusd_unsolicited_hex,
    wb_testcase_cuus1_0,
    wb_testcase_cuus1_1,
    wb_testcase_cuus1_read,
    wb_testcase_cuus1_test,
    wb_testcase_cuus1_neg_0,
    wb_testcase_cuus1_neg_1,
    wb_testcase_cuus1_unsol_1,
    wb_testcase_vtd_set1,
    wb_testcase_vtd_set2,
    wb_testcase_vtd_set3,
    wb_testcase_vtd_read,
    wb_testcase_vtd_test,
    wb_testcase_cscs_overall,
    wb_testcase_cscs_short_failure,
    wb_testcase_cusd_hex,
    wb_testcase_cusd_hex_fail,
    wb_testcase_dtmf,
    wb_testcase_dtmf_neg,
    wb_testcase_dtmf_test,
    wb_testcase_ecav,
    wb_testcase_ecam,
    wb_testcase_ecme_set_0,
    wb_testcase_ecme_read_0,
    wb_testcase_ecme_set_1,
    wb_testcase_ecme_read_1,
    wb_testcase_ecme_set_2,
    wb_testcase_ecme_test,
    wb_testcase_ecme_unsolicited,
    wb_testcase_ecrat_set_1,
    wb_testcase_ecrat_set_2,
    wb_testcase_ecrat_set_3,
    wb_testcase_ecrat_set_4,
    wb_testcase_ecrat_set_5,
    wb_testcase_ecrat_set_6,
    wb_testcase_ecrat_set_7,
    wb_testcase_ecrat_set_8,
    wb_testcase_ecrat_set_9,
    wb_testcase_ecrat_read_1,
    wb_testcase_ecrat_read_2,
    wb_testcase_ecrat_read_3,
    wb_testcase_ecrat_read_4,
    wb_testcase_ecrat_read_5,
    wb_testcase_ecrat_test_1,
    wb_testcase_ecrat_unsolicited_1,
    wb_testcase_ecrat_unsolicited_2,
    wb_testcase_ecrat_unsolicited_3,
    wb_testcase_ecrat_unsolicited_4,
    wb_testcase_ennir_set_1,
    wb_testcase_ennir_set_2,
    wb_testcase_ennir_set_3,
    wb_testcase_ennir_read_1,
    wb_testcase_ennir_read_2,
    wb_testcase_ennir_test_1,
    wb_testcase_ennir_unsolicited_1,
    wb_testcase_egnci_1,
    wb_testcase_egnci_2,
    wb_testcase_egsci_1,
    wb_testcase_egsci_2,
    wb_testcase_ehstact_1,
    wb_testcase_ehstact_2,
    wb_testcase_ehstact_fail_1,
    wb_testcase_ehstact_fail_2,
    wb_testcase_ehstact_fail_3,
    wb_testcase_ehstact_test,
    wb_testcase_elat_set_0,
    wb_testcase_elat_read_0,
    wb_testcase_elat_set_1,
    wb_testcase_elat_read_1,
    wb_testcase_elat_set_2,
    wb_testcase_elat_test,
    wb_testcase_elat_unsolicited,
    wb_testcase_elin_set_1,
    wb_testcase_elin_read_1,
    wb_testcase_elin_set_2,
    wb_testcase_elin_read_2,
    wb_testcase_elin_set_2_no_support,
    wb_testcase_elin_set_0,
    wb_testcase_elin_set_3,
    wb_testcase_elin_test_support,
    wb_testcase_elin_test_no_support,
    wb_testcase_eiaauw,
    wb_testcase_eiaauw_test,
    wb_testcase_enap,
    wb_testcase_esmsfull_set_0,
    wb_testcase_esmsfull_set_1,
    wb_testcase_esmsfull_read,
    wb_testcase_esmsfull_test,
    wb_testcase_epinr_set_1,
    wb_testcase_epinr_set_2,
    wb_testcase_epinr_test,
    wb_testcase_efsim_test,
    wb_testcase_efsim_read,
    wb_testcase_ereg_reg_presentation_1,
    wb_testcase_ereg_reg_presentation_2,
    wb_testcase_ereg_reg_ext_presentation_1,
    wb_testcase_ereg_reg_ext_presentation_2,
    wb_testcase_ereg_reg_ext_presentation_3,
    wb_testcase_ereg_is_supported,
    wb_testcase_ereg_unsolicited_1,
    wb_testcase_ereg_unsolicited_2,
    wb_testcase_ereg_unsolicited_3,
    wb_testcase_ereg_unsolicited_4,
    wb_testcase_ereg_unsolicited_5,
    wb_testcase_erfstate_enable,
    wb_testcase_erfstate_read_enabled,
    wb_testcase_erfstate_disable,
    wb_testcase_erfstate_read_disabled,
    wb_testcase_erfstate_error,
    wb_testcase_erfstate_test,
    wb_testcase_erfstate_unsolicited_on,
    wb_testcase_erfstate_unsolicited_off,
    wb_testcase_esimrf_0,
    wb_testcase_esimrf_1,
    wb_testcase_esimrf_2,
    wb_testcase_esimrf_3,
    wb_testcase_esimrf_4,
    wb_testcase_esimrf_5,
    wb_testcase_esimrf_6,
    wb_testcase_esimrf_7,
    wb_testcase_esimrf_8,
    wb_testcase_esimrf_9,
    wb_testcase_esimrf_10,
    wb_testcase_esimrf_11,
    wb_testcase_esimrf_12,
    wb_testcase_esimrf_13,
    wb_testcase_esimrf_test,
    wb_testcase_esimrf_read,
    wb_testcase_esimsr_test,
    wb_testcase_esimsr_0,
    wb_testcase_esimsr_1,
    wb_testcase_esimsr_2,
    wb_testcase_esimsr_3,
    wb_testcase_epee_test,
    wb_testcase_epee_0,
    wb_testcase_epee_1,
    wb_testcase_epee_2,
    wb_testcase_epsb,
    wb_testcase_epwrred_set_1,
    wb_testcase_epwrred_set_2,
    wb_testcase_epwrred_set_3,
    wb_testcase_epwrred_set_4,
    wb_testcase_epwrred_set_5,
    wb_testcase_epwrred_set_6,
    wb_testcase_epwrred_set_7,
    wb_testcase_epwrred_set_8,
    wb_testcase_epwrred_set_9,
    wb_testcase_epwrred_set_10,
    wb_testcase_epwrred_set_11,
    wb_testcase_epwrred_set_12,
    wb_testcase_epwrred_set_13,
    wb_testcase_epwrred_test_1,
    wb_testcase_etty_get,
    wb_testcase_etty_set_0,
    wb_testcase_etty_set_1,
    wb_testcase_etty_set_2,
    wb_testcase_etty_set_3,
    wb_testcase_etty_set_4,
    wb_testcase_etty_test,
    wb_testcase_etzr_1,
    wb_testcase_etzv_1,
    wb_testcase_etzr_2,
    wb_testcase_etzv_2,
    wb_testcase_etzr_3,
    wb_testcase_etzv_3,
    wb_testcase_etzr_4,
    wb_testcase_etzv_4,
    wb_testcase_etzr_5,
    wb_testcase_etzv_5,
    wb_testcase_etzr_6,
    wb_testcase_etzv_6,
    wb_testcase_etzv_7,
    wb_testcase_euplink_set,
    wb_testcase_ewnci_1,
    wb_testcase_ewnci_2,
    wb_testcase_ewsci_1,
    wb_testcase_ewsci_2,
    wb_testcase_gcap_do,
    wb_testcase_gcap_test,
    wb_testcase_d,
    wb_testcase_d_clir_invoke,
    wb_testcase_d_clir_suppress,
    wb_testcase_d_failure,
    wb_testcase_d_failure_1,
    wb_testcase_d_failure_2,
    wb_testcase_d_busy,
    wb_testcase_d_no_answer,
    wb_testcase_d_no_response,
    wb_testcase_d_no_dialtone,
    wb_testcase_d_ps,
    wb_testcase_d_ps_l2p,
    wb_testcase_d_ps_cid,
    wb_testcase_d_ps_l2p_cid,
    wb_testcase_d_ps_conn,
    wb_testcase_d_ps_failure,
    wb_testcase_d_ps_failure_1,
    wb_testcase_d_ps_failure_2,
    wb_testcase_d_ps_failure_3,
    wb_testcase_d_ps_failure_4,
    wb_testcase_h,
    wb_testcase_h_test,
    wb_testcase_csq_do,
    wb_testcase_csq_do_fail_mal,
    wb_testcase_csq_do_unknown_rssi,
    wb_testcase_csq_test,
    wb_testcase_pscc,
    wb_testcase_pscc_ipv4v6,
    wb_testcase_f,
    wb_testcase_f0,
    wb_testcase_f_0,
    wb_testcase_f1,
    wb_testcase_f_1,
    wb_testcase_f_test,
    wb_testcase_e_0,
    wb_testcase_e_1,
    wb_testcase_e0,
    wb_testcase_e1,
    wb_testcase_e_error,
    wb_testcase_e,
    wb_testcase_e_read,
    wb_testcase_e_test,
    wb_testcase_q_0,
    wb_testcase_q_1,
    wb_testcase_q0,
    wb_testcase_q1,
    wb_testcase_q_error,
    wb_testcase_q,
    wb_testcase_q_read,
    wb_testcase_q_test,
    wb_testcase_v_0,
    wb_testcase_v_1,
    wb_testcase_v0,
    wb_testcase_v1,
    wb_testcase_v_error,
    wb_testcase_v_num_error,
    wb_testcase_v,
    wb_testcase_v_read,
    wb_testcase_v_test,
    wb_testcase_E0Q0V1,
    wb_testcase_z_do_0,
    wb_testcase_z_do_1,
    wb_testcase_z_do_2,
    wb_testcase_z_set,
    wb_testcase_z_do_neg_1,
    wb_testcase_z_do_neg_2,
    wb_testcase_z_test,
    wb_testcase_sequence,
    wb_testcase_E,
    wb_testcase_q0v1,
    wb_testcase_star,
    wb_testcase_star_neg,
    wb_testcase_star_test,
    wb_testcase_star_test_neg,
    wb_testcase_s0,
    wb_testcase_s0_min,
    wb_testcase_s0_max,
    wb_testcase_s0_failure,
    wb_testcase_s0_read,
    wb_testcase_s0_test,
    wb_testcase_s0_noargs,
    wb_testcase_s0_default,
    wb_testcase_s3_min,
    wb_testcase_s3_max,
    wb_testcase_s3_failure,
    wb_testcase_s3_read,
    wb_testcase_s3_test,
    wb_testcase_s3_default,
    wb_testcase_s3_noargs,
    wb_testcase_s4_min,
    wb_testcase_s4_max,
    wb_testcase_s4_failure,
    wb_testcase_s4_read,
    wb_testcase_s4_test,
    wb_testcase_s4_noargs,
    wb_testcase_s4_default,
    wb_testcase_s5_min,
    wb_testcase_s5_max,
    wb_testcase_s5_failure,
    wb_testcase_s5_read,
    wb_testcase_s5_test,
    wb_testcase_s5_noargs,
    wb_testcase_s5_default,
    wb_testcase_s6_read,
    wb_testcase_s6_test,
    wb_testcase_s6_noargs,
    wb_testcase_s6_set,
    wb_testcase_s7_read,
    wb_testcase_s7_test,
    wb_testcase_s7_noargs,
    wb_testcase_s7_set,
    wb_testcase_s8_read,
    wb_testcase_s8_test,
    wb_testcase_s8_noargs,
    wb_testcase_s8_set,
    wb_testcase_s10_read,
    wb_testcase_s10_test,
    wb_testcase_s10_noargs,
    wb_testcase_s10_set,
    wb_testcase_stkc_set_0,
    wb_testcase_stkc_set_1,
    wb_testcase_stkc_set_neg,
    wb_testcase_stkc_set_exe_fail,
    wb_testcase_stkc_set_3_params_fail,
    wb_testcase_stkc_read_zero,
    wb_testcase_stkc_read_one,
    wb_testcase_stck_test,
    wb_testcase_stke_send,
    wb_testcase_stki,
    wb_testcase_stkn,
    wb_testcase_stkend,
    wb_testcase_stkr_set_param_error,
    wb_testcase_stkr_set_executer_error,
    wb_testcase_stkr_set_ok,
    wb_testcase_stkr_test,
    wb_testcase_eshlvocr_test,
    wb_testcase_eshlvocr_test_fail,
    wb_testcase_cmer_unsol_ciev_10,
    wb_testcase_ecpschblkr_do,
    wb_testcase_cops_rsa_authenticate,
    wb_testcase_cops_simlock_authenticate,
    wb_testcase_cops_auth_fail,
    wb_testcase_cops_auth_fail2,
    wb_testcase_cops_auth_fail3,
    wb_testcase_cops_auth_abort,
    wb_testcase_cops_imei,
    wb_testcase_cops_imei_fail,
    wb_testcase_cops_simlock,
    wb_testcase_cops_simlock_fail,
    wb_testcase_cops_simlockdata,
    wb_testcase_cops_simlockdata2,
    wb_testcase_cops_simlockdata_read,
    wb_testcase_cops_simlockdata_fail,
    wb_testcase_cops_simlockdata_fail2,
    wb_testcase_cops_simlockdata_abort,
    wb_testcase_ecspsar,
    wb_testcase_ecspsar_storage,
    wb_testcase_ecspsar_fail,
    wb_testcase_ecspsaw,
    wb_testcase_ecspsaw_storage,
    wb_testcase_ecspsaw_odd,
    wb_testcase_ecspsaw_fail,
    wb_testcase_ecspsaw_fail2,
    wb_testcase_eempage_read,
    wb_testcase_eempage_test,
    wb_testcase_eempage_set_error01p,
    wb_testcase_eempage_set_error02p,
    wb_testcase_eempage_set_error03p,
    wb_testcase_eempage_set_error04p,
    wb_testcase_eempage_set_error05p,
    wb_testcase_eempage_set_error06p,
    wb_testcase_eempage_set_error07p,
    wb_testcase_eempage_set_error01o,
    wb_testcase_eempage_set_error02o,
    wb_testcase_eempage_set_error03o,
    wb_testcase_eempage_set_error04o,
    wb_testcase_eempage_set_error05o,
    wb_testcase_eempage_set_error06o,
    wb_testcase_eempage_unsolicited1,
    wb_testcase_eempage_unsolicited2,
    wb_testcase_eempage_set_ok01,
    wb_testcase_eempage_set_ok01r,
    wb_testcase_eempage_set_ok02,
    wb_testcase_eempage_set_ok03,
    wb_testcase_eempage_set_ok03r,
    wb_testcase_eempage_set_ok04,
    wb_testcase_eempage_set_ok05,
    wb_testcase_eempage_set_ok06,
    wb_testcase_eempage_set_ok07,
    wb_testcase_eempage_set_ok08,
    wb_testcase_eempage_set_ok09,
    wb_testcase_esvn_read,
    wb_testcase_esvn_error1,
    wb_testcase_esvn_error2,
    wb_testcase_esvn_set,

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <inc_wb_extended_test.h>
#endif
    wb_testcase_x,
    wb_testcase_cleanup,
} wb_testcase_t;


/* Test functions generic */
void wb_testfcn_setup(char *at_string_p);
void wb_testfcn_cleanup(char *at_string_p);

void disconnect_and_delete_account_c();
void add_and_connect_account_c();

/* Test functions for CALL */
void wb_testfcn_a_do(char *at_string_p);
void wb_testfcn_a_test(char *at_string_p);
void wb_testfcn_chld(char *at_string_p);
void wb_testfcn_chld_1(char *at_string_p);
void wb_testfcn_chld_2(char *at_string_p);
void wb_testfcn_chld_failure(char *at_string_p);
void wb_testfcn_chld_read(char *at_string_p);
void wb_testfcn_chld_test(char *at_string_p);
void wb_testfcn_clcc_0(char *at_string_p);
void wb_testfcn_clcc_1(char *at_string_p);
void wb_testfcn_clcc_2(char *at_string_p);
void wb_testfcn_clip_failure(char *at_string_p);
void wb_testfcn_clip_0(char *at_string_p);
void wb_testfcn_clip_1(char *at_string_p);
void wb_testfcn_clip_read(char *at_string_p);
void wb_testfcn_clip_test(char *at_string_p);
void wb_testfcn_clir_set(char *at_string_p);
void wb_testfcn_clir_read(char *at_string_p);
void wb_testfcn_cssi(char *at_string_p);
void wb_testfcn_cssu(char *at_string_p);
void wb_testfcn_d(char *at_string_p);
void wb_testfcn_d_failure(char *at_string_p);
void wb_testfcn_d_busy(char *at_string_p);
void wb_testfcn_d_no_answer(char *at_string_p);
void wb_testfcn_d_no_response(char *at_string_p);
void wb_testfcn_d_no_dialtone(char *at_string_p);
void wb_testfcn_dtmf(char *at_string_p);
void wb_testfcn_dtmf_neg(char *at_string_p);
void wb_testfcn_dtmf_test(char *at_string_p);
void wb_testfcn_vtd_set(char *at_string_p);
void wb_testfcn_vtd_set_neg(char *at_string_p);
void wb_testfcn_vtd_read(char *at_string_p);
void wb_testfcn_vtd_test(char *at_string_p);
void wb_testfcn_h(char *at_string_p);
void wb_testfcn_h_test(char *at_string_p);
void wb_testfcn_cmod_set(char *at_string_p);
void wb_testfcn_cmod_set_neg(char *at_string_p);
void wb_testfcn_cmod_read(char *at_string_p);
void wb_testfcn_cmod_test(char *at_string_p);
void wb_testfcn_cmut_set(char *at_string_p);
void wb_testfcn_cmut_set_neg(char *at_string_p);
void wb_testfcn_cmut_read(char *at_string_p);
void wb_testfcn_cmut_test(char *at_string_p);
void wb_testfcn_epsb(char *at_string_p);
void wb_testfcn_etty_read(char *at_string_p);
void wb_testfcn_etty_set(char *at_string_p);
void wb_testfcn_etty_set_neg(char *at_string_p);
void wb_testfcn_etty_test(char *at_string_p);

/* Test functions for CALL */
void wb_testfcn_etzr(char *at_string);
void wb_testfcn_etzv(char *at_string);
void wb_testfcn_ceer_d_failure(char *at_string_p);
void wb_testfcn_ceer_a_failure(char *at_string_p);
void wb_testfcn_ceer_h_failure(char *at_string_p);
void wb_testfcn_ceer_h_ok(char *at_string_p);
void wb_testfcn_ceer_disconnect(char *at_string_p);
void wb_testfcn_ceer_test(char *at_string_p);
void wb_testfcn_ceer_netfail_1(char *at_string_p);
void wb_testfcn_ceer_pscc_1(char *at_string_p);
void wb_testfcn_user_activity(char *at_string_p);
void wb_testfcn_user_activity_failure(char *at_string_p);
void wb_testfcn_user_activity_test(char *at_string_p);

/* Test functions for AT CONTROL */
void wb_testfcn_and_c(char *at_string_p);
void wb_testfcn_and_d(char *at_string_p);
void wb_testfcn_cmee_set_0(char *at_string_p);
void wb_testfcn_cmee_set_1(char *at_string_p);
void wb_testfcn_cmee_set_2(char *at_string_p);
void wb_testfcn_cmee_failure(char *at_string_p);
void wb_testfcn_cmee_read(char *at_string_p);
void wb_testfcn_cmee_test(char *at_string_p);
void wb_testfcn_cmee_configure(char *at_string_p);
void wb_testfcn_crc(char *at_string_p);
void wb_testfcn_f(char *at_string_p);
void wb_testfcn_f_failure(char *at_string_p);
void wb_testfcn_f_test(char *at_string_p);
void wb_testfcn_ok(char *at_string_p);
void wb_testfcn_error(char *at_string_p);
void wb_testfcn_e(char *at_string_p);
void wb_testfcn_e_read(char *at_string_p);
void wb_testfcn_e_test(char *at_string_p);
void wb_testfcn_q_read(char *at_string_p);
void wb_testfcn_q_test(char *at_string_p);
void wb_testfcn_v0(char *at_string_p);
void wb_testfcn_z_do(char *at_string_p);
void wb_testfcn_z_set(char *at_string_p);
void wb_testfcn_z_do_neg(char *at_string_p);
void wb_testfcn_z_do_during_call(char *at_string_p);
void wb_testfcn_z_read(char *at_string_p);
void wb_testfcn_z_test(char *at_string_p);
void wb_testfcn_sequence(char *at_string_p);
void wb_testfcn_v_read(char *at_string_p);
void wb_testfcn_v_test(char *at_string_p);
void wb_testfcn_num_error(char *at_string_p);
void wb_testfcn_star(char *at_string_p);
void wb_testfcn_star_neg(char *at_string_p);
void wb_testfcn_star_test(char *at_string_p);
void wb_testfcn_star_test_neg(char *at_string_p);
void wb_testfcn_s0(char *at_string_p);
void wb_testfcn_s0_failure(char *at_string_p);
void wb_testfcn_s0_read(char *at_string_p);
void wb_testfcn_s0_test(char *at_string_p);
void wb_testfcn_s0_noargs(char *at_string_p);
void wb_testfcn_s3(char *at_string_p);
void wb_testfcn_s3_failure(char *at_string_p);
void wb_testfcn_s3_read(char *at_string_p);
void wb_testfcn_s3_test(char *at_string_p);
void wb_testfcn_s3_noargs(char *at_string_p);
void wb_testfcn_s4(char *at_string_p);
void wb_testfcn_s4_failure(char *at_string_p);
void wb_testfcn_s4_read(char *at_string_p);
void wb_testfcn_s4_test(char *at_string_p);
void wb_testfcn_s4_noargs(char *at_string_p);
void wb_testfcn_s5(char *at_string_p);
void wb_testfcn_s5_failure(char *at_string_p);
void wb_testfcn_s5_read(char *at_string_p);
void wb_testfcn_s5_test(char *at_string_p);
void wb_testfcn_s5_noargs(char *at_string_p);
void wb_testfcn_s6_read(char *at_string_p);
void wb_testfcn_s6_test(char *at_string_p);
void wb_testfcn_s6_set(char *at_string_p);
void wb_testfcn_s6_noargs(char *at_string_p);
void wb_testfcn_s7_read(char *at_string_p);
void wb_testfcn_s7_test(char *at_string_p);
void wb_testfcn_s7_set(char *at_string_p);
void wb_testfcn_s7_noargs(char *at_string_p);
void wb_testfcn_s8_read(char *at_string_p);
void wb_testfcn_s8_test(char *at_string_p);
void wb_testfcn_s8_set(char *at_string_p);
void wb_testfcn_s8_noargs(char *at_string_p);
void wb_testfcn_s10_read(char *at_string_p);
void wb_testfcn_s10_test(char *at_string_p);
void wb_testfcn_s10_set(char *at_string_p);
void wb_testfcn_s10_noargs(char *at_string_p);
void wb_testfcn_cscs_set(char *at_string_p);
void wb_testfcn_cscs_set_neg(char *at_string_p);
void wb_testfcn_cscs_set_noparameters(char *at_string_p);
void wb_testfcn_cscs_read(char *at_string_p);
void wb_testfcn_cscs_test(char *at_string_p);
void wb_testfcn_cscstest_cusd_send_ussd(char *at_string_p);
void wb_testfcn_cscstest_cusd_send_ussd_failure(char *at_string_p);
void wb_testfcn_cscs_cscstest(char *at_string_p);
void wb_testfcn_x(char *at_string_p);

/* Test functions and data for ME CONTROL */
void wb_testfcn_cmec(char *at_string_p);
void wb_testfcn_cmec_failure(char *at_string_p);
void wb_testfcn_cmec_read(char *at_string_p);
void wb_testfcn_cmec_test(char *at_string_p);
void wb_testfcn_cmer_set(char *at_string_p);
void wb_testfcn_cmer_set_neg(char *at_string_p);
void wb_testfcn_cmer_read(char *at_string_p);
void wb_testfcn_cmer_test(char *at_string_p);
void wb_testfcn_cmer_unsol_ciev_1(char *at_string_p);
void wb_testfcn_cmer_unsol_ciev_2(char *at_string_p);
void wb_testfcn_cmer_unsol_ciev_3(char *at_string_p);
void wb_testfcn_cmer_unsol_ciev_4(char *at_string_p);
void wb_testfcn_cmer_unsol_ciev_5(char *at_string_p);
void wb_testfcn_cmer_unsol_ciev_6(char *at_string_p);
void wb_testfcn_cmer_unsol_ciev_disabled(char *at_string_p);
void wb_testfcn_cgmi_do(char *at_string_p);
void wb_testfcn_cgmm_do(char *at_string_p);
void wb_testfcn_cgmr_do(char *at_string_p);
void wb_testfcn_gmm_do(char *at_string_p);
void wb_testfcn_gmr_do(char *at_string_p);
void wb_testfcn_cgmi_test(char *at_string_p);
void wb_testfcn_cgmm_test(char *at_string_p);
void wb_testfcn_cgmr_test(char *at_string_p);
void wb_testfcn_gmm_test(char *at_string_p);
void wb_testfcn_gmr_test(char *at_string_p);
void wb_testfcn_cfun_99(char *at_string_p);
void wb_testfcn_cfun_100(char *at_string_p);
void wb_testfcn_cfun_5_6(char *at_string_p);
void wb_testfcn_cfun_failure(char *at_string_p);
void wb_testfcn_cfun_rf_off(char *at_string_p);
void wb_testfcn_cfun_rf_transition(char *at_string_p);
void wb_testfcn_cfun_rf_preferred_wcdma(char *at_string_p);
void wb_testfcn_cfun_rf_gsm_only(char *at_string_p);
void wb_testfcn_cfun_rf_wcdma_only(char *at_string_p);
void wb_testfcn_cfun_rf_gsm_wcdma_auto(char *at_string_p);
void wb_testfcn_cfun_rf_cdma_evdo_auto(char *at_string_p);
void wb_testfcn_cfun_rf_cdma_only(char *at_string_p);
void wb_testfcn_cfun_rf_evdo_only(char *at_string_p);
void wb_testfcn_cfun_rf_all_auto(char *at_string_p);
void wb_testfcn_cfun_read_failure(char *at_string_p);
void wb_testfcn_pscc(char *at_string_p);
void wb_testfcn_pscc_ipv4v6(char *at_string_p);
void wb_testfcn_cgsn_do(char *at_string_p);
void wb_testfcn_cgsn_test(char *at_string_p);
void wb_testfcn_csq(char *at_string_p);
void wb_testfcn_ecam(char *at_string_p);
void wb_testfcn_erfstate_set(char *at_string_p);
void wb_testfcn_erfstate_read(char *at_string_p);
void wb_testfcn_erfstate_test(char *at_string_p);
void wb_testfcn_erfstate_unsolicited(char *at_string_p);
void wb_testfcn_gcap_do(char *at_string_p);
void wb_testfcn_gcap_test(char *at_string_p);
void wb_testfcn_epwrred_set(char *at_string_p);
void wb_testfcn_epwrred_test(char *at_string_p);
void wb_testfcn_esvn_read(char *at_string_p);

/* Test functions for SIM */
void wb_testfcn_cpin_test(char *at_string_p);
void wb_testfcn_cpin_read(char *at_string_p);
void wb_testfcn_cpin_set(char *at_string_p);
void wb_testfcn_cpin_set_wrong_pin(char *at_string_p);

void wb_testfcn_ecexpin_test(char *at_string_p);
void wb_testfcn_ecexpin_read(char *at_string_p);
void wb_testfcn_ecexpin_query_state(char *at_string_p);
void wb_testfcn_ecexpin_set_puk(char *at_string_p);
void wb_testfcn_ecexpin_set_incorrect_param_1(char *at_string_p);
void wb_testfcn_ecexpin_set_incorrect_param_2(char *at_string_p);
void wb_testfcn_ecexpin_set_incorrect_param_3(char *at_string_p);

void wb_testfcn_esimrf_read(char *at_string_p);
void wb_testfcn_esimrf_set_1(char *at_string_p);
void wb_testfcn_esimrf_set_2(char *at_string_p);
void wb_testfcn_esimrf_unsol_1(char *at_string_p);
void wb_testfcn_esimrf_unsol_2(char *at_string_p);
void wb_testfcn_esimrf_unsol_3(char *at_string_p);
void wb_testfcn_esimrf_unsol_4(char *at_string_p);
void wb_testfcn_esimrf_unsol_5(char *at_string_p);
void wb_testfcn_esimrf_unsol_6(char *at_string_p);
void wb_testfcn_esimrf_unsol_7(char *at_string_p);
void wb_testfcn_esimrf_unsol_8(char *at_string_p);
void wb_testfcn_esimrf_test(char *at_string_p);

void wb_testfcn_esimsr_set_read(char *at_string_p);
void wb_testfcn_esimsr_unsol(char *at_string_p);
void wb_testfcn_esimsr_test(char *at_string_p);
void wb_testfcn_crsm_read_get_resp(char *at_string_p);
void wb_testfcn_crsm_read_get_resp_2(char *at_string_p);
void wb_testfcn_crsm_read_file_bin(char *at_string_p);
void wb_testfcn_crsm_read_file_bin_2(char *at_string_p);
void wb_testfcn_crsm_read_file_rec(char *at_string_p);
void wb_testfcn_crsm_read_file_rec_2(char *at_string_p);
void wb_testfcn_crsm_update_rec(char *at_string_p);
void wb_testfcn_crsm_update_rec_1(char *at_string_p);
void wb_testfcn_crsm_update_rec_1_2(char *at_string_p);
void wb_testfcn_crsm_update_bin(char *at_string_p);
void wb_testfcn_crsm_update_bin_1(char *at_string_p);
void wb_testfcn_crsm_update_bin_1_2(char *at_string_p);
void wb_testfcn_crsm_read_status(char *at_string_p);
void wb_testfcn_crsm_read_status_2(char *at_string_p);
void wb_testfcn_crsm_test(char *at_string_p);
void wb_testfcn_cuad_test(char *at_string_p);
void wb_testfcn_cuad_do(char *at_string_p);
void wb_testfcn_cuad_do_2(char *at_string_p);
void wb_testfcn_cuad_do_3(char *at_string_p);
void wb_testfcn_csas_test(char *at_string_p);
void wb_testfcn_csas_test_1(char *at_string_p);
void wb_testfcn_csas_test_2(char *at_string_p);
void wb_testfcn_csas_test_3(char *at_string_p);
void wb_testfcn_csas_do(char *at_string_p);
void wb_testfcn_csas_set_1(char *at_string_p);
void wb_testfcn_csas_set_2(char *at_string_p);
void wb_testfcn_csas_set_3(char *at_string_p);
void wb_testfcn_csas_set_4(char *at_string_p);
void wb_testfcn_cres_test(char *at_string_p);
void wb_testfcn_cres_test_1(char *at_string_p);
void wb_testfcn_cres_test_2(char *at_string_p);
void wb_testfcn_cres_test_3(char *at_string_p);
void wb_testfcn_cres_do(char *at_string_p);
void wb_testfcn_cres_set_1(char *at_string_p);
void wb_testfcn_cres_set_2(char *at_string_p);
void wb_testfcn_cres_set_3(char *at_string_p);
void wb_testfcn_cres_set_4(char *at_string_p);

void wb_testfcn_csim_test(char *at_string_p);
void wb_testfcn_csim_set_1(char *at_string_p);
void wb_testfcn_csim_set_2(char *at_string_p);
void wb_testfcn_csim_set_3(char *at_string_p);
void wb_testfcn_csim_set_4(char *at_string_p);

void wb_testfcn_cchc_test(char *at_string_p);
void wb_testfcn_cchc_set_1(char *at_string_p);
void wb_testfcn_cchc_set_2(char *at_string_p);

void wb_testfcn_ccho_test(char *at_string_p);
void wb_testfcn_ccho_set_1(char *at_string_p);
void wb_testfcn_ccho_set_2(char *at_string_p);

void wb_testfcn_cgla_test(char *at_string_p);
void wb_testfcn_cgla_set_1(char *at_string_p);
void wb_testfcn_cgla_set_2(char *at_string_p);

void wb_testfcn_cimi_test(char *at_string_p);
void wb_testfcn_cimi_do(char *at_string_p);

void wb_testfcn_epinr_test(char *at_string_p);
void wb_testfcn_epinr_set_1(char *at_string_p);
void wb_testfcn_epinr_set_2(char *at_string_p);

void wb_testfcn_efsim_test(char *at_string_p);
void wb_testfcn_efsim_read(char *at_string_p);

void wb_testfcn_epee_set_read(char *at_string_p);
void wb_testfcn_epee_unsol(char *at_string_p);
void wb_testfcn_epee_test(char *at_string_p);

void wb_testfcn_cpwd_set_one(char *at_string_p);
void wb_testfcn_cpwd_set_two(char *at_string_p);
void wb_testfcn_cpwd_set(char *at_string_p);
void wb_testfcn_cpwd_test(char *at_string_p);

void wb_testfcn_stkc_set(char *at_string_p);
void wb_testfcn_stkc_set_neg(char *at_string_p);
void wb_testfcn_stkc_set_exe_fail(char *at_string_p);
void wb_testfcn_stkc_set_3_params_neg(char *at_string_p);
void wb_testfcn_stkc_read_zero(char *at_string_p);
void wb_testfcn_stkc_read_one(char *at_string_p);
void wb_testfcn_stkc_test(char *at_string_p);
void wb_testfcn_stke_send(char *at_string_p);
void wb_testfcn_stki_unsol(char *at_string_p);
void wb_testfcn_stkn_unsol(char *at_string_p);
void wb_testfcn_stkr_set(char *at_string_p);
void wb_testfcn_stkr_test(char *at_string);
void wb_testfcn_stkend_unsol(char *at_string_p);
void wb_testfcn_eshlvocr_answer(char *at_string_p);
void wb_testfcn_eshlvocr_answer_wrong(char *at_string_p);


void wb_testfcn_cnum_do(char *at_string_p);
void wb_testfcn_cnum_test(char *at_string_p);


/* Security  */
void wb_testfcn_clck_test(char *at_string_p);
void wb_testfcn_clck_read(char *at_string_p);
void wb_testfcn_clck_set_enable(char *at_string_p);
void wb_testfcn_clck_query_neg(char *at_string_p);
void wb_testfcn_clck_set_enable_neg(char *at_string_p);
void wb_testfcn_clck_set_disable(char *at_string_p);
void wb_testfcn_clck_set_disable_pin(char *at_string_p);
void wb_testfcn_clck_set_disable_puk(char *at_string_p);
void wb_testfcn_clck_set_disable_pin_fd(char *at_string_p);
void wb_testfcn_clck_set_disable_puk_fd(char *at_string_p);
void wb_testfcn_clck_set_query(char *at_string_p);
void wb_testfcn_clck_set_query_pn(char *at_string_p);
void wb_testfcn_clck_set_query_neg(char *at_string_p);
void wb_testfcn_ecpschblkr_do(char *at_string_p);
void wb_testfcn_cops_rsa_authenticate(char *at_string_p);
void wb_testfcn_cops_simlock_authenticate(char *at_string_p);
void wb_testfcn_cops_auth_fail(char *at_string_p);
void wb_testfcn_cops_auth(char *at_string_p);
void wb_testfcn_cops_imei(char *at_string_p);
void wb_testfcn_cops_imei_fail(char *at_string_p);
void wb_testfcn_cops_simlock(char *at_string_p);
void wb_testfcn_cops_simlock_fail(char *at_string_p);
void wb_testfcn_cops_simlock_data(char *at_string_p);
void wb_testfcn_cops_simlock_data_odd(char *at_string_p);
void wb_testfcn_cops_simlock_data_read(char *at_string_p);
void wb_testfcn_cops_simlock_data_fail(char *at_string_p);

/* Test functions for SMS */
void wb_testfcn_cbm(char *at_string_p);
void wb_testfcn_cds(char *at_string_p);
void wb_testfcn_cdsi(char *at_string_p);
void wb_testfcn_cnmi(char *at_string_p);
void wb_testfcn_cscb(char *at_string_p);
void wb_testfcn_cmgd(char *at_string_p);
void wb_testfcn_cmgd_all(char *at_string_p);
void wb_testfcn_cmgd_fail(char *at_string_p);
void wb_testfcn_cmgd_test(char *at_string_p);
void wb_testfcn_cmgd_read(char *at_string_p);
void wb_testfcn_cmgf_set(char *at_string_p);
void wb_testfcn_cmgf_failure(char *at_string_p);
void wb_testfcn_cmgf_read(char *at_string_p);
void wb_testfcn_cmgf_test(char *at_string_p);
void wb_testfcn_cmgl_test(char *at_string_p);
void wb_testfcn_cmgl(char *at_string_p);
void wb_testfcn_cmgl_failure(char *at_string_p);
void wb_testfcn_cmgr(char *at_string_p);
void wb_testfcn_cmgr_failure(char *at_string);
void wb_testfcn_cmgr_test(char *at_string_p);
void wb_testfcn_cmgs(char *at_string_p);
void wb_testfcn_cmgs_failure(char *at_string_p);
void wb_testfcn_cmgs_cms_error(char *at_string_p);
void wb_testfcn_cmms_cmgs(char *at_string_p);
void wb_testfcn_cmgs_read(char *at_string_p);
void wb_testfcn_cmgs_test(char *at_string_p);
void wb_testfcn_cmgw(char *at_string_p);
void wb_testfcn_cmgw_failure(char *at_string_p);
void wb_testfcn_cmgw_read(char *at_string_p);
void wb_testfcn_cmgw_test(char *at_string_p);
void wb_testfcn_cmms(char *at_string_p);
void wb_testfcn_cmt(char *at_string_p);
void wb_testfcn_cmti(char *at_string_p);
void wb_testfcn_cmms_failure(char *at_string_p);
void wb_testfcn_cmms_test(char *at_string_p);
void wb_testfcn_cmms_read(char *at_string_p);
void wb_testfcn_cnma_failure_1(char *at_string_p);
void wb_testfcn_cnma_read(char *at_string_p);
void wb_testfcn_cnma_set(char *at_string_p);
void wb_testfcn_cnma_set2(char *at_string_p);
void wb_testfcn_cnma_test(char *at_string_p);
void wb_testfcn_cpms(char *at_string_p);
void wb_testfcn_cpms_read(char *at_string_p);
void wb_testfcn_cpms_test(char *at_string_p);
void wb_testfcn_cpms_failure(char *at_string_p);
void wb_testfcn_csms(char *at_string_p);
void wb_testfcn_esmsfull_set(char *at_string_p);
void wb_testfcn_esmsfull_read(char *at_string);
void wb_testfcn_esmsfull_test(char *at_string_p);
void wb_testfcn_cgsms_set(char *at_string_p);
void wb_testfcn_cgsms_set_fail(char *at_string_p);
void wb_testfcn_cgsms_read(char *at_string_p);
void wb_testfcn_cgsms_test(char *at_string_p);
void wb_testfcn_cmss_set(char *at_string_p);
void wb_testfcn_cmss_set_fail(char *at_string_p);
void wb_testfcn_cmss_set_new_recipient(char *at_string_p);
void wb_testfcn_cmss_set_param_fail(char *at_string_p);
void wb_testfcn_ciev_storage_status(char *at_string_p);

/* Test functions for CSCA */
void wb_testfcn_csca_set_smsc(char *at_string_p);
void wb_testfcn_csca_set_smsc_failure(char *at_string_p);
void wb_testfcn_csca_get_smsc(char *at_string_p);
void wb_testfcn_csca_get_smsc_failure(char *at_string_p);
void wb_testfcn_csca_test_is_supported(char *at_string);

/* Test functions for CGEREP */
void wb_testfcn_cgerep_set(char *at_string);
void wb_testfcn_cgerep_set_failure(char *at_string);
void wb_testfcn_cgerep_read(char *at_string);
void wb_testfcn_cgerep_test(char *at_string);

/* Test functions for +COPS */
void wb_testfcn_cops_network_register(char *at_string_p);
void wb_testfcn_cops_network_deregister(char *at_string_p);
void wb_testfcn_cops_set_format(char *at_string_p);
void wb_testfcn_cops_get_settings(char *at_string_p);
void wb_testfcn_cops_network_search(char *at_string_p);
void wb_testfcn_cops_network_manual_registration_with_automatic_fallback(char *at_string_p);

/* Test functions for +CPOL */
void wb_testfcn_plmn_write(char *at_string_p);
void wb_testfcn_plmn_write_illegal(char *at_string_p);
void wb_testfcn_plmn_write_rejected(char *at_string_p);
void wb_testfcn_plmn_write_retry(char *at_string_p);
void wb_testfcn_plmn_remove_element(char *at_string_p);
void wb_testfcn_plmn_read_two(char *at_string_p);
void wb_testfcn_plmn_read_none(char *at_string_p);
void wb_testfcn_plmn_read_failed(char *at_string_p);
void wb_testfcn_plmn_test(char *at_string_p);
void wb_testfcn_plmn_test_failed(char *at_string_p);
void wb_testfcn_plmn_read_retry(char *at_string_p);
void wb_testfcn_plmn_read_retry_failed(char *at_string_p);

/* Test functions for PS DATA */
void wb_testfcn_cgact_set_3gpp(char *at_string);
void wb_testfcn_cgact_set_two_cids(char *at_string);
void wb_testfcn_cgatt_gprs_update(char *at_string_p);
void wb_testfcn_cgatt_gprs_update_ignored(char *at_string_p);
void wb_testfcn_cgatt_gprs_update_failed(char *at_string_p);
void wb_testfcn_cgatt_gprs_update_abort(char *at_string_p);
void wb_testfcn_cgatt_get_status(char *at_string_p);
void wb_testfcn_cgatt_is_supported(char *at_string_p);
void wb_testfcn_cgatt_gprs_abort(char *at_string_p);
void wb_testfcn_cgdcont(char *at_string);
void wb_testfcn_cgdcont_read(char *at_string);
void wb_testfcn_cgdcont_parsing(char *at_string);
void wb_testfcn_cgdcont_test(char *at_string);
void wb_testfcn_cgcmod_set_1(char *at_string);
void wb_testfcn_cgcmod_set_2(char *at_string);
void wb_testfcn_cgcmod_set_3(char *at_string);
void wb_testfcn_cgcmod_test(char *at_string);
void wb_testfcn_cgqmin(char *at_string);
void wb_testfcn_cgqreq(char *at_string);
void wb_testfcn_cgeqreq(char *at_string);
void wb_testfcn_cgeqreq_partialset(char *at_string);
void wb_testfcn_cgeqmin(char *at_string);
void wb_testfcn_cgeqmin_partialset(char *at_string);
void wb_testfcn_cgeqneg(char *at_string);
void wb_testfcn_enap(char *at_string);
void wb_testfcn_cgpaddr_set(char *at_string);
void wb_testfcn_cgpaddr_set_two_cids(char *at_string);
void wb_testfcn_cgpaddr_test(char *at_string);
void wb_testfcn_cgpaddr_all(char *at_string);
void wb_testfcn_d_ps(char* at_string);
void wb_testfcn_d_ps_failure(char* at_string);
void wb_testfcn_euplink(char *at_string);
void wb_testfcn_cr_set(char *at_string_p);
void wb_testfcn_cr_set_neg(char *at_string_p);
void wb_testfcn_cr_read_0(char *at_string_p);
void wb_testfcn_cr_read_1(char *at_string_p);
void wb_testfcn_cr_test(char *at_string_p);
void wb_testfcn_pscc_with_report(char *at_string_p);
void wb_testfcn_eiaauw_set(char *at_string);
void wb_testfcn_eiaauw_test(char *at_string);

/* Test functions for MAL - MISC */
void wb_testfcn_ecav(char *at_string_p);
void wb_testfcn_cssi(char *at_string_p);
void wb_testfcn_creg_ereg_cgreg_registration_presentation(char *at_string_p);
void wb_testfcn_creg_ereg_cgreg_is_registration_presentation_supported(char *at_string_p);
void wb_testfcn_creg_ereg_cgreg_unsolicited_registration_presentation(char *at_string_p);
void wb_testfcn_egnci(char *at_string_p);
void wb_testfcn_ewnci(char *at_string_p);
void wb_testfcn_egsci(char *at_string_p);
void wb_testfcn_ewsci(char *at_string_p);

/* Test functions for +CUSD */
void wb_testfcn_cusd_send_ussd(char *at_string_p);
void wb_testfcn_cusd_send_ussd_failure(char *at_string_p);
void wb_testfcn_cusd_cancel_ussd(char *at_string_p);
void wb_testfcn_cusd_get_settings(char *at_string_p);
void wb_testfcn_cusd_get_supported_n(char *at_string_p);
void wb_testfcn_cusd_unsolicited(char *at_string_p);
void wb_testfcn_cusd_unsolicited_hex(char *at_string_p);

void wb_testfcn_cuus1_set(char *at_string_p);
void wb_testfcn_cuus1_read(char *at_string_p);
void wb_testfcn_cuus1_test(char *at_string_p);
void wb_testfcn_cuus1_neg(char *at_string_p);
void wb_testfcn_cuus1_unsol(char *at_string_p);

void wb_testfcn_ccwa_unsolicited(char *at_string_p);
void wb_testfcn_ccwa_set_1(char *at_string_p);
void wb_testfcn_ccwa_set_neg(char *at_string_p);
void wb_testfcn_ccwa_set_query(char *at_string_p);
void wb_testfcn_ccwa_read(char *at_string_p);
void wb_testfcn_ccwa_test(char *at_string_p);

void wb_testfcn_cpwd_call_barring(char *at_string_p);
void wb_testfcn_clck_call_barring_lock(char *at_string_p);
void wb_testfcn_clck_call_barring_unlock(char *at_string_p);
void wb_testfcn_clck_call_barring_query(char *at_string_p);

void wb_testfcn_ccfc_set_1(char *at_string_p);
void wb_testfcn_ccfc_set_2(char *at_string_p);
void wb_testfcn_ccfc_set_query(char *at_string_p);
void wb_testfcn_ccfc_set_register(char *at_string_p);
void wb_testfcn_ccfc_test(char *at_string_p);

void wb_testfcn_cnap_set(char *at_string_p);
void wb_testfcn_cnap_read(char *at_string_p);
void wb_testfcn_cnap_test(char *at_string_p);
void wb_testfcn_cnap_unsolicited(char *at_string_p);

void wb_testfcn_colr_do(char *at_string_p);
void wb_testfcn_colr_test(char *at_string_p);
void wb_testfcn_colp(char *at_string_p);
void wb_testfcn_colp_test(char *at_string_p);

void wb_testfcn_ecme_set(char *at_string_p);
void wb_testfcn_ecme_read(char *at_string_p);
void wb_testfcn_ecme_test(char *at_string_p);
void wb_testfcn_ecme_unsolicited(char *at_string_p);

void wb_testfcn_elat_set(char *at_string_p);
void wb_testfcn_elat_read(char *at_string_p);
void wb_testfcn_elat_test(char *at_string_p);
void wb_testfcn_elat_unsolicited(char *at_string_p);

void wb_testfcn_elin_set(char *at_string_p);
void wb_testfcn_elin_read(char *at_string_p);
void wb_testfcn_elin_test(char *at_string_p);

void wb_testfcn_ecrat_set(char *at_string_p);
void wb_testfcn_ecrat_read(char *at_string_p);
void wb_testfcn_ecrat_test(char *at_string_p);
void wb_testfcn_ecrat_unsolicited(char *at_string_p);
void wb_testfcn_ennir_set(char *at_string_p);
void wb_testfcn_ennir_read(char *at_string_p);
void wb_testfcn_ennir_test(char *at_string_p);
void wb_testfcn_ennir_unsolicited(char *at_string_p);

void wb_testfcn_string_conversion(char *at_string_p);

/* Storage */
void wb_testfcn_cspsar_read(char *at_string_p);
void wb_testfcn_cspsar_read_fail(char *at_string_p);
void wb_testfcn_cspsar_write(char *at_string_p);
void wb_testfcn_cspsar_write_fail(char *at_string_p);

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <exe_extended_testcases.h>
#endif

void wb_testfcn_cssn(char *at_string);

void wb_testfcn_string_conversion(char *at_string_p);

/*Test functions for Engineer mode - EMPAGE */
void wb_testfcn_eempage_read(char *at_string_p);
void wb_testfcn_eempage_test(char *at_string_p);
void wb_testfcn_eempage_set_error_par(char *at_string_p);
void wb_testfcn_eempage_set_error_op(char *at_string_p);
void wb_testfcn_eempage_set_ok(char *at_string_p);
void wb_testfcn_eempage_unsolicited(char *at_string_p);

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
void wb_testutil_extended_disable_realtime_reporting(void);
void wb_testutil_extended_enable_realtime_reporting(void);
#endif
void wb_testutil_disable_realtime_reporting(void);
void wb_testutil_enable_realtime_reporting(void);

#endif /* WB_TEST_H 1 */
