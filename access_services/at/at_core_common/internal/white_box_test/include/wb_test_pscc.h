/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef WB_TEST_PSCC_H
#define WB_TEST_PSCC_H 1

#define cid_a "24"
#define cid_b "8"
#define cid_c "15"
#define cid_d "4"
#define cid_e "12"
#define cid_f "17"
#define cid_g "18"

#define eppsd_a_on  "*EPPSD=1,7," cid_a
#define eppsd_a_off "*EPPSD=0,7," cid_a
#define eppsd_a_test "*EPPSD=?"

#define eppsd_e_on  "*EPPSD=1,4," cid_e
#define eppsd_e_off "*EPPSD=0,4," cid_e
#define eppsd_e_test "*EPPSD=?"

#define eppsd_f_on  "*EPPSD=1,5," cid_f
#define eppsd_f_off "*EPPSD=0,5," cid_f
#define eppsd_f_test "*EPPSD=?"

#define eppsd_g_on  "*EPPSD=1,6," cid_g
#define eppsd_g_off "*EPPSD=0,6," cid_g
#define eppsd_g_test "*EPPSD=?"

#define cgdcont_a_create "+CGDCONT=" cid_a ",\"ip\",\"apn." cid_a ".se\",\"www.pdp_a\",0,0"
#define cgdcont_b_create "+CGDCONT=" cid_b ",\"IpV6\",\"apn." cid_b ".se\",\"\",0,1"
#define cgdcont_c_create "+CGDCONT=" cid_c ",\"ip\",\"apn." cid_c ".se\",\"www.pdp_c\",0,0"
#define cgdcont_d_create "+CGDCONT=" cid_d ",\"test\",\"apn." cid_d ".se\",\"www.pdp_d\",0,0"
#define cgdcont_e_create "+CGDCONT=" cid_e ",\"IpV4V6\",\"apn." cid_e ".se\",\"\",0,0"
#define cgdcont_f_create "+CGDCONT=" cid_f ",\"IpV4V6\",\"apn." cid_f ".se\",\"\",0,1"
#define cgdcont_g_create "+CGDCONT=" cid_g ",\"IpV4V6\",\"apn." cid_g ".se\",\"\",0,1"

#define cgdcont_a_delete "+CGDCONT=" cid_a
#define cgdcont_b_delete "+CGDCONT=" cid_b
#define cgdcont_c_delete "+CGDCONT=" cid_c
#define cgdcont_d_delete "+CGDCONT=" cid_d
#define cgdcont_e_delete "+CGDCONT=" cid_e
#define cgdcont_f_delete "+CGDCONT=" cid_f
#define cgdcont_g_delete "+CGDCONT=" cid_g

#define cgdcont_a_read "+CGDCONT: " cid_a ",\"IP\",\"apn." cid_a ".se\",\"www.pdp_a\",0,0"
#define cgdcont_b_read "+CGDCONT: " cid_b ",\"IPV6\",\"apn." cid_b ".se\",\"\",0,1"
#define cgdcont_c_read "+CGDCONT: " cid_c ",\"IP\",\"apn." cid_c ".se\",\"www.pdp_c\",0,0"
#define cgdcont_d_read "+CGDCONT: " cid_d ",\"TEST\",\"apn." cid_d ".se\",\"www.pdp_d\",0,0"
#define cgdcont_e_read "+CGDCONT: " cid_e ",\"IPV4V6\",\"apn." cid_e ".se\",\"\",0,0"
#define cgdcont_f_read "+CGDCONT: " cid_f ",\"IPV4V6\",\"apn." cid_f ".se\",\"\",0,1"
#define cgdcont_g_read "+CGDCONT: " cid_g ",\"IPV4V6\",\"apn." cid_g ".se\",\"\",0,1"

#define cgdcont_read "+CGDCONT?"
#define cgerep "+CGEREP=1,0"
#define cgeqmin_read "+CGEQMIN?"
#define qos_standard ",3,5696,14000,5000,14000,0,1502,\"1E4\",\"1E5\",1,4,2"
#define qos_partial ",1,2345,"
#define qos_minimum ",0,0,0,0,0,0,0,\"1E6\",\"6E8\",0,0,0,0"
#define qos_min_maximum ",3,5696,21000,5696,21000,1,1502,\"1E1\",\"5E2\",2,4000,3,1,1"
#define qos_req_maximum ",4,5696,21000,5696,21000,2,1502,\"1E1\",\"5E2\",3,4000,3,1,1"
#define cgeqmin_set "+CGEQMIN=" cid_a qos_standard
#define cgeqmin_set_partial "+CGEQMIN=" cid_a qos_partial
#define cgeqmin_set_minimum "+CGEQMIN=" cid_a qos_minimum
#define cgeqmin_set_maximum "+CGEQMIN=" cid_a qos_min_maximum
#define cgeqreq_read "+CGEQREQ?"
#define cgeqreq_set "+CGEQREQ=" cid_a qos_standard
#define cgeqreq_set_partial "+CGEQREQ=" cid_a qos_partial
#define cgeqreq_set_minimum "+CGEQREQ=" cid_a qos_minimum
#define cgeqreq_set_maximum "+CGEQREQ=" cid_a qos_req_maximum
#define cgeqneg_set "+CGEQNEG=8"
#define cgeqneg_set_all "+CGEQNEG=8,24"
#define cgeqneg_set_none "+CGEQNEG="
#define cgeqneg_test "+CGEQNEG=?"

/* defines for cgqmin/cgqreq-tests*/
#define cgqmin_read_all "+CGQMIN?"
#define remove_1 "+CGDCONT=1"
#define create_1 "+CGDCONT=1,\"ip\",\"apn.se\""
#define q_maxset ",3,4,5,9,18"
#define q_leftouts ",2,2,,,31"
#define cgqmin_max_set "+CGQMIN=" cid_a q_maxset
#define cgqmin_set_leftouts "+CGQMIN=" cid_a q_leftouts
#define cgqmin_set_reset "+CGQMIN=" cid_a
#define cgqmin_max_response "+CGQMIN: " cid_a q_maxset
#define cgqmin_leftouts_response "+CGQMIN: " cid_a ",2,2,0,0,31"
#define cgqmin_set_reset_response "+CGQMIN: " cid_a ",0,0,0,0,0"

#define cgqreq_read_all "+CGQREQ?"
#define cgqreq_max_set "+CGQREQ=" cid_a q_maxset
#define cgqreq_set_leftouts "+CGQREQ=" cid_a q_leftouts
#define cgqreq_max_response "+CGQREQ: " cid_a q_maxset
#define cgqreq_leftouts_response "+CGQREQ: " cid_a ",2,2,0,0,31"
#define cgqreq_set_reset "+CGQREQ=" cid_a
#define cgqreq_set_reset_response "+CGQREQ: " cid_a ",0,0,0,0,0"

#define cgact_read "+CGACT?"
#define CGEV_CHECK_STRING "+CGEV: NW DEACT \"IP\", 123.0.0.1, 24"
#define CGEV_CHECK_STRING_IPV4V6 "+CGEV: NW DEACT \"IPV4V6\", 123.0.0.1, 12"
#define CGEV_CHECK_STRING_IPV4V6_1 "+CGEV: NW DEACT \"IPV4V6\", 123.0.0.1, 18"
#define CGEV_CHECK_STRING_IPV4V6_2 "+CGEV: NW DEACT \"IPV4V6\", 2001:cdba::3257:9652, 18"
#define cgreg_on "+CGREG=2"
#define cgreg_off "+CGREG=0"
#define CGREG_CHECK_STRING "+CGREG: 0"
#define CGPADDR_ALL "+CGPADDR="

#define cgact_a_on "+CGACT=1," cid_a
#define cgact_a_off "+CGACT=0," cid_a

#define cgact_c_on "+CGACT=1," cid_c
#define cgact_c_off "+CGACT=0," cid_c

#define cgact_d_on "+CGACT=1," cid_d
#define cgact_d_off "+CGACT=0," cid_d
#define cr_set_0 "+CR=0"
#define cr_set_1 "+CR=1"

#define cgact_e_on "+CGACT=1," cid_e
#define cgact_e_off "+CGACT=0," cid_e

#define cgact_f_on "+CGACT=1," cid_f
#define cgact_f_off "+CGACT=0," cid_f

#define cgact_g_on "+CGACT=1," cid_g
#define cgact_g_off "+CGACT=0," cid_g


#define euplink_d_def "*EUPLINK=" cid_d ",512"
#define euplink_d_max "*EUPLINK=" cid_d ",262143"
#define euplink_c_def "*EUPLINK=" cid_c ",512"

/* Defines for *ENAP */
#define enap_cid "5"
#define enap_account_create "+CGDCONT="enap_cid",\"ip\",\"apn.enap.se\",\"www.enap_pdp\",0,0"
#define enap_account_activate "+CGACT=1,"enap_cid
#define enap_nap_connect "*ENAP=1,"enap_cid",1"
#define enap_nap_read "*ENAP?"
#define enap_nap_disconnect "*ENAP=0,"enap_cid
#define enap_account_delete  "+CGDCONT=" enap_cid

#if 0
#define cgact_a_and_b_on "+CGACT=1," cid_a "," cid_b
#define cgact_a_and_b_off "+CGACT=0," cid_a"," cid_b
#else
#define cgact_a_and_b_on "+CGACT=1"
#define cgact_a_and_b_off "+CGACT=0"
#endif

/* Defines for +CGCMOD */
#define cgcmod_set         "+CGCMOD=" cid_c
#define cgcmod_set_b_and_c "+CGCMOD=" cid_c "," cid_b
#define cgcmod_set_all     "+CGCMOD="
#define cgcmod_set_err     "+CGCMOD=12"
#define cgeqreq_set_cid_c "+CGEQREQ=" cid_c ",3,5696,21000,5000,21000,0,1502,\"1E4\",\"1E5\",1,4,2"

/* Defines for *EIAAUW */
#define eiaauw_set_a_auth_all   "*EIAAUW=" cid_a ",1,\"user\",\"password\",00111,0"
#define eiaauw_set_a_auth_pap   "*EIAAUW=" cid_a ",1,\"user\",\"password\",00011,0"
#define eiaauw_set_a_auth_chap  "*EIAAUW=" cid_a ",1,\"user\",\"password\",00101,0"
#define eiaauw_set_a_auth_none  "*EIAAUW=" cid_a ",1,\"user\",\"password\",00001,0"
#define eiaauw_set_a_empty      "*EIAAUW=" cid_a ",1,\"\",\"\",00111,0"
#define eiaauw_set_a_auth_err   "*EIAAUW=" cid_a ",1,\"user\",\"password\",00000,0"
#define eiaauw_set_a_bearer_err "*EIAAUW=" cid_a ",2,\"user\",\"password\",00001,0"
#define eiaauw_set_c_err        "*EIAAUW=" cid_c ",1,\"user\",\"password\",00001,0"
#define eiaauw_test             "*EIAAUW=?"

/* Defines for ATD*99 */
#define atd_ps_set_a           "D*99*\"addr\"**" cid_a "#"
#define atd_ps_set_a_l2p       "D*99*\"addr\"*1*" cid_a "#"
#define atd_ps_set_l2p         "D*99*\"addr\"*1*#"
#define atd_ps_set             "D*99#"
#define atd_ps_set_bad_sc      "D*98#"
#define atd_ps_set_bad_l2p     "D*99*\"addr\"*0*#"
#define atd_ps_set_bad_cid_0     "D*99*\"addr\"**0#"
#define atd_ps_set_bad_cid     "D*99*\"addr\"**30#"
#define atd_ps_set_b_wrong_cid "D*99*\"addr\"**" cid_b "#"

extern bool wb_pscc_respond_with_just_client_tag(int msg_id);
extern bool wb_sterc_init_message(mpl_msg_type_t sterc_msg_type, int msg_id);
extern bool wb_pscc_init_message(mpl_msg_type_t pscc_msg_type, int msg_id);
extern char *wb_pscc_msg_id_string(int id);
extern char *wb_sterc_msg_id_string(int id);


#endif

