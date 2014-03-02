/*
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : tc_mal_basic.c
 * Description     : basic mal connectivity tests
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "tc_mal_basic.h"
#include "util.h"
#include "util_scenarios.h"
#include "util_conn_param.h"
#include "util_requests.h"
#include "pscc_msg.h"

/* Defines for SDU error conversions */
#define sdu_mpl_1502 ((uint8_t)0b10010111)
#define sdu_mpl_1510 ((uint8_t)0b10011000)
#define sdu_mpl_1520 ((uint8_t)0b10011001)

#define cutoff_td_150 ((uint8_t)0b10000)
#define cutoff_td_950 ((uint8_t)0b100000)
#define cutoff_td_max ((uint8_t)0b111110)

/* Defines for the bitrate conversions */
#define cutoff_1_kbps ((uint8_t)0b01000000)
#define cutoff_8_kbps ((uint8_t)0b10000000)
#define cutoff_64_kbps ((uint8_t)0b11111110)
#define cutoff_1_Mbps ((uint8_t)0b01001010)
#define cutoff_2_Mbps ((uint8_t)0b10111010)
#define maxLimit ((uint8_t)0b11111010)

uint8_t max_sdu_size[5] = {0, sdu_mpl_1502, sdu_mpl_1510, sdu_mpl_1520, 0xFF};
uint8_t transfer_delay[5] = {0, cutoff_td_150, cutoff_td_950, cutoff_td_max, cutoff_td_max+1};
uint8_t max_bitrate_ul_base[9] = {0, cutoff_1_kbps, cutoff_8_kbps, cutoff_64_kbps, cutoff_64_kbps, cutoff_64_kbps, cutoff_64_kbps, cutoff_64_kbps, cutoff_64_kbps+1};
uint8_t max_bitrate_ul_extended[9] = {0, 0, 0, 0, cutoff_1_Mbps, cutoff_2_Mbps, maxLimit, maxLimit+1, maxLimit+1};

psccd_test_state_type_t tc_basic_first(int state)
{
  TESTNOTNEG(util_setup_stecom());

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_basic_connect_disconnect(int state) {
  int cid = 1;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  char ip2[16];
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  qosneg.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosreq,0,sizeof(mal_gpds_qos_profile_t));
  qosreq.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosmin,0,sizeof(mal_gpds_qos_profile_t));
  memset(&qosrel5,0,sizeof(mal_gpds_qos_profile_t));
  qosrel5.qos_type = MAL_GPDS_QOS_REL99;
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_ntop(AF_INET, &ip, ip2, sizeof(ip_address_t));

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect(cid, apn, ip, pdns, sdns, ifname, qosreq, qosrel5, qosmin, qosneg));
  TESTNOTNEG(util_check(cid, ip, qosreq, qosrel5, qosneg));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_basic_connect_disconnect_ipv4v6(int state) {
  int cid = 1;
  int idx = 2;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t ipv6;
  ip_address_t pdns;
  ip_address_t ipv6_pdns;
  ip_address_t sdns;
  ip_address_t ipv6_sdns;
  int addr_type[3] = {MAL_PDP_TYPE_IPV4, MAL_PDP_TYPE_IPV6, MAL_PDP_TYPE_IPV4V6};
  char *addr_string[3] = { "only ipv4", "only ipv6", "both ipv4 and ipv6"};
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  memset(&ipv6,0,sizeof(ip_address_t));
  memset(&ipv6_pdns,0,sizeof(ip_address_t));
  memset(&ipv6_sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9652", &ipv6);
  inet_pton(AF_INET6, "2001:cdba::3257:9653", &ipv6_pdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9654", &ipv6_sdns);

  for (idx = 0 ; idx< 3; idx++) {
    TESTNOTNEG(util_create(cid));
    TESTNOTNEG(util_connect_for_ipv4v6(cid, apn, ip, ipv6, pdns, ipv6_pdns, sdns, ipv6_sdns, ifname, qosreq, qosrel5, qosmin, qosneg, pscc_pdp_type_ipv4v6, addr_type[idx]));
    TESTNOTNEG(util_disconnect(cid));
    TESTNOTNEG(util_destroy(cid));
  }

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}


psccd_test_state_type_t tc_basic_connect_disconnect_ipv6(int state) {
  int cid = 1;
  int idx = 1;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t ipv6;
  ip_address_t pdns;
  ip_address_t ipv6_pdns;
  ip_address_t sdns;
  ip_address_t ipv6_sdns;
  int addr_type[3] = {MAL_PDP_TYPE_IPV4, MAL_PDP_TYPE_IPV6, MAL_PDP_TYPE_IPV4V6};
  char *addr_string[3] = { "only ipv4", "only ipv6", "both ipv4 and ipv6"};
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  memset(&ipv6,0,sizeof(ip_address_t));
  memset(&ipv6_pdns,0,sizeof(ip_address_t));
  memset(&ipv6_sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9652", &ipv6);
  inet_pton(AF_INET6, "2001:cdba::3257:9653", &ipv6_pdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9654", &ipv6_sdns);

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect_for_ipv6(cid, apn, ip, ipv6, pdns, ipv6_pdns, sdns, ipv6_sdns, ifname, qosreq, qosrel5, qosmin, qosneg, pscc_pdp_type_ipv6, addr_type[idx]));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}



psccd_test_state_type_t tc_basic_connect_disconnect_ipv4(int state) {
  int cid = 1;
  int idx = 0;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t ipv6;
  ip_address_t pdns;
  ip_address_t ipv6_pdns;
  ip_address_t sdns;
  ip_address_t ipv6_sdns;
  int addr_type[3] = {MAL_PDP_TYPE_IPV4, MAL_PDP_TYPE_IPV6, MAL_PDP_TYPE_IPV4V6};
  char *addr_string[3] = { "only ipv4", "only ipv6", "both ipv4 and ipv6"};
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  memset(&ipv6,0,sizeof(ip_address_t));
  memset(&ipv6_pdns,0,sizeof(ip_address_t));
  memset(&ipv6_sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9652", &ipv6);
  inet_pton(AF_INET6, "2001:cdba::3257:9653", &ipv6_pdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9654", &ipv6_sdns);

  printf("\n Iteration %d with %s addresses",idx,addr_string[idx]);
  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect_for_ipv4(cid, apn, ip, ipv6, pdns, ipv6_pdns, sdns, ipv6_sdns, ifname, qosreq, qosrel5, qosmin, qosneg, pscc_pdp_type_ipv4, addr_type[idx]));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}


psccd_test_state_type_t tc_fetch_data_counters(int state) {
  int cid = 1;
  int idx = 0;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t ipv6;
  ip_address_t pdns;
  ip_address_t ipv6_pdns;
  ip_address_t sdns;
  ip_address_t ipv6_sdns;

  int addr_type[3] = {MAL_PDP_TYPE_IPV4, MAL_PDP_TYPE_IPV6, MAL_PDP_TYPE_IPV4V6};
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  memset(&ipv6,0,sizeof(ip_address_t));
  memset(&ipv6_pdns,0,sizeof(ip_address_t));
  memset(&ipv6_sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9652", &ipv6);
  inet_pton(AF_INET6, "2001:cdba::3257:9653", &ipv6_pdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9654", &ipv6_sdns);

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect_and_fetch_counters(cid, apn, ip, ipv6, pdns, ipv6_pdns, sdns, ipv6_sdns, ifname, qosreq, qosrel5, qosmin, qosneg, pscc_pdp_type_ipv4, addr_type[idx]));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_reset_data_counters(int state) {
  int cid = 1;
  int idx = 0;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t ipv6;
  ip_address_t pdns;
  ip_address_t ipv6_pdns;
  ip_address_t sdns;
  ip_address_t ipv6_sdns;

  int addr_type[3] = {MAL_PDP_TYPE_IPV4, MAL_PDP_TYPE_IPV6, MAL_PDP_TYPE_IPV4V6};
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  memset(&ipv6,0,sizeof(ip_address_t));
  memset(&ipv6_pdns,0,sizeof(ip_address_t));
  memset(&ipv6_sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9652", &ipv6);
  inet_pton(AF_INET6, "2001:cdba::3257:9653", &ipv6_pdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9654", &ipv6_sdns);

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect_and_reset_counters(cid, apn, ip, ipv6, pdns, ipv6_pdns, sdns, ipv6_sdns, ifname, qosreq, qosrel5, qosmin, qosneg, pscc_pdp_type_ipv4, addr_type[idx]));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_basic_255connections(int state) {
  int cid = 1;
  int i;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  qosneg.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosreq,0,sizeof(mal_gpds_qos_profile_t));
  qosreq.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosmin,0,sizeof(mal_gpds_qos_profile_t));
  memset(&qosrel5,0,sizeof(mal_gpds_qos_profile_t));
  qosrel5.qos_type = MAL_GPDS_QOS_REL99;
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);

  for (i = 0; i< 255; i++){
    qosreq.maximum_sdu_size = max_sdu_size[i%5];
    qosreq.transfer_delay = transfer_delay[i%5];
    qosreq.maximum_bitrate_ul = max_bitrate_ul_base[i%9];
    qosreq.maximum_bitrate_dl = max_bitrate_ul_base[i%9];
    TESTNOTNEG(util_create(cid));
    TESTNOTNEG(util_connect_255(cid, apn, ip, pdns, sdns, ifname, qosreq, qosrel5, qosmin, qosneg,
        max_bitrate_ul_extended[i%9],max_bitrate_ul_extended[i%9]));
    cid++;
  }

  cid = 1;
  for (i = 0; i< 255; i++){
    TESTNOTNEG(util_disconnect(cid));
    TESTNOTNEG(util_destroy(cid));
    cid++;
  }

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_basic_connect_disconnect_chap_generation(int state) {
  int cid = 1;
  char apn[] = "default-apn.services";
  char user[] = "ste";
  ip_address_t ip;
  char ip2[16];
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  qosneg.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosreq,0,sizeof(mal_gpds_qos_profile_t));
  qosreq.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosmin,0,sizeof(mal_gpds_qos_profile_t));
  memset(&qosrel5,0,sizeof(mal_gpds_qos_profile_t));
  qosrel5.qos_type = MAL_GPDS_QOS_REL99;
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_ntop(AF_INET, &ip, ip2, sizeof(ip_address_t));

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect_for_chap_generation(cid, apn, ip, pdns, sdns, ifname, qosreq, qosrel5, qosmin, qosneg, user));
  TESTNOTNEG(util_check(cid, ip, qosreq, qosrel5, qosneg));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_basic_connect_disconnect_chap(int state) {
  int cid = 1;
  char apn[] = "default-apn.services";
  char user[] = "ste";
  ip_address_t ip;
  char ip2[16];
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  qosneg.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosreq,0,sizeof(mal_gpds_qos_profile_t));
  qosreq.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosmin,0,sizeof(mal_gpds_qos_profile_t));
  memset(&qosrel5,0,sizeof(mal_gpds_qos_profile_t));
  qosrel5.qos_type = MAL_GPDS_QOS_REL99;
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_ntop(AF_INET, &ip, ip2, sizeof(ip_address_t));

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect_for_chap(cid, apn, ip, pdns, sdns, ifname, qosreq, qosrel5, qosmin, qosneg, user));
  TESTNOTNEG(util_check(cid, ip, qosreq, qosrel5, qosneg));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_basic_connect_disconnect_pap(int state) {
  int cid = 1;
  char apn[] = "default-apn.services";
  char user[] = "ste";
  char password[] = "ste";
  ip_address_t ip;
  char ip2[16];
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  qosneg.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosreq,0,sizeof(mal_gpds_qos_profile_t));
  qosreq.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosmin,0,sizeof(mal_gpds_qos_profile_t));
  memset(&qosrel5,0,sizeof(mal_gpds_qos_profile_t));
  qosrel5.qos_type = MAL_GPDS_QOS_REL99;
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_ntop(AF_INET, &ip, ip2, sizeof(ip_address_t));

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect_for_pap(cid, apn, ip, pdns, sdns, ifname, qosreq, qosrel5, qosmin, qosneg, user, password));
  TESTNOTNEG(util_check(cid, ip, qosreq, qosrel5, qosneg));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_basic_connect_disconnect_uplink(int state) {
  int cid = 1;
  int idx = 0;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t ipv6;
  ip_address_t pdns;
  ip_address_t ipv6_pdns;
  ip_address_t sdns;
  ip_address_t ipv6_sdns;
  int addr_type[3] = {MAL_PDP_TYPE_IPV4, MAL_PDP_TYPE_IPV6, MAL_PDP_TYPE_IPV4V6};
  char *addr_string[3] = { "only ipv4", "only ipv6", "both ipv4 and ipv6"};
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  memset(&ipv6,0,sizeof(ip_address_t));
  memset(&ipv6_pdns,0,sizeof(ip_address_t));
  memset(&ipv6_sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9652", &ipv6);
  inet_pton(AF_INET6, "2001:cdba::3257:9653", &ipv6_pdns);
  inet_pton(AF_INET6, "2001:cdba::3257:9654", &ipv6_sdns);

  printf("\n Iteration %d with %s addresses",idx,addr_string[idx]);
  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect_for_uplink(cid, apn, ip, ipv6, pdns, ipv6_pdns, sdns, ipv6_sdns, ifname, qosreq, qosrel5, qosmin, qosneg, pscc_pdp_type_test, addr_type[idx]));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}


psccd_test_state_type_t tc_list_all_connection(int state) {
  int cid = 1;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  char ip2[16];
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  qosneg.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosreq,0,sizeof(mal_gpds_qos_profile_t));
  qosreq.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosmin,0,sizeof(mal_gpds_qos_profile_t));
  memset(&qosrel5,0,sizeof(mal_gpds_qos_profile_t));
  qosrel5.qos_type = MAL_GPDS_QOS_REL99;
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_ntop(AF_INET, &ip, ip2, sizeof(ip_address_t));

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_list_all_connection(cid, apn, ip, pdns, sdns, ifname, qosreq, qosrel5, qosmin, qosneg));
  TESTNOTNEG(util_check(cid, ip, qosreq, qosrel5, qosneg));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}



psccd_test_state_type_t tc_static_ip_ipv4(int state) {
  int cid = 1;
  char apn[] = "default-apn.services";
  char static_ip[] = "192.168.1.13";
  ip_address_t ip;
  char ip2[16];
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  qosneg.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosreq,0,sizeof(mal_gpds_qos_profile_t));
  qosreq.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosmin,0,sizeof(mal_gpds_qos_profile_t));
  memset(&qosrel5,0,sizeof(mal_gpds_qos_profile_t));
  qosrel5.qos_type = MAL_GPDS_QOS_REL99;
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_ntop(AF_INET, &ip, ip2, sizeof(ip_address_t));

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_static_ipv4(cid, apn, ip, pdns, sdns, ifname, qosreq, qosrel5, qosmin, qosneg,static_ip,pscc_pdp_type_ipv4));
  TESTNOTNEG(util_check(cid, ip, qosreq, qosrel5, qosneg));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_error_scenarios(int state) {
  int cid = 1;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  char ip2[16];
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  qosneg.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosreq,0,sizeof(mal_gpds_qos_profile_t));
  qosreq.qos_type = MAL_GPDS_QOS_REL99;
  memset(&qosmin,0,sizeof(mal_gpds_qos_profile_t));
  memset(&qosrel5,0,sizeof(mal_gpds_qos_profile_t));
  qosrel5.qos_type = MAL_GPDS_QOS_REL99;
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  inet_ntop(AF_INET, &ip, ip2, sizeof(ip_address_t));

  TESTNOTNEG(util_create_error(cid));
  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_unexpected_events(cid));
  TESTNOTNEG(util_error_paths(cid));
  //TESTNOTNEG(util_mal_failures(cid));
  TESTNOTNEG(util_connect(cid, apn, ip, pdns, sdns, ifname, qosreq, qosrel5, qosmin, qosneg));
  //TESTNOTNEG(util_check(cid, ip, qosreq, qosrel5, qosneg));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_basic_last(int state)
{
  TESTNOTNEG(util_cleanup());

  return PSCCD_TEST_SUCCESS;

error:
  return PSCCD_TEST_FAIL;
}

