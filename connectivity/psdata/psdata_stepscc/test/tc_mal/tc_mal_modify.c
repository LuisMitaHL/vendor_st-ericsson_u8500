/*
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : tc_mal_modify.c
 * Description     : basic mal connectivity tests
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "tc_mal_modify.h"
#include "util.h"
#include "util_scenarios.h"
#include "util_conn_param.h"
#include "util_requests.h"
#include "pscc_msg.h"


psccd_test_state_type_t tc_modify(int state) {
  int cid = 1;
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
  qosrel5.qos_type = MAL_GPDS_QOS_REL5;
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect(cid, apn, ip, pdns, sdns, ifname, qosreq, qosrel5, qosmin, qosneg));
  TESTNOTNEG(util_check(cid, ip, qosreq, qosrel5, qosneg));


  qosreq.qos_type = MAL_GPDS_QOS_REL99;
  qosreq.precedence_class = MAL_GPDS_QOS_PRECEDENCE_CLASS_1;
  qosreq.delay_class = MAL_GPDS_QOS_DELAY_CLASS_1;
  qosreq.reliability_class = MAL_GPDS_QOS_RELIABILITY_CLASS_1;
  qosreq.peak_throughput_class = MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_8000;
  qosreq.mean_throughput_class = MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100;
  qosreq.traffic_class = MAL_GPDS_QOS_TRAFFICCLASS_CONVERSATIONAL;
  qosreq.delivery_order = MAL_GPDS_QOS_DELIVERY_ORDER_YES;
  qosreq.delivery_of_error_sdu = MAL_GPDS_QOS_DELIVERY_ERROR_SDU_YES;
  qosreq.residual_ber = 1;
  qosreq.sdu_error_ratio = 1;
  qosreq.trafic_handling_priority = MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_2;
  qosreq.transfer_delay = 0x01;
  qosreq.maximum_sdu_size = 0x32;
  qosreq.maximum_bitrate_ul =0x12;
  qosreq.maximum_bitrate_dl =0x12;
  qosreq.guaranteed_bitrate_ul =0x12;
  qosreq.guaranteed_bitrate_dl =0x12;

  qosrel5.qos_type = MAL_GPDS_QOS_REL99;
  qosrel5.src_stat_desc = MAL_GPDS_QOS_SRC_STAT_DESC_SPEECH;
  qosrel5.sgn_ind = MAL_GPDS_QOS_OPT_SGN;

  qosneg.qos_type = MAL_GPDS_QOS_REL99;
  qosneg.precedence_class = MAL_GPDS_QOS_PRECEDENCE_CLASS_1;
  qosneg.delay_class = MAL_GPDS_QOS_DELAY_CLASS_1;
  qosneg.reliability_class = MAL_GPDS_QOS_RELIABILITY_CLASS_1;
  qosneg.peak_throughput_class = MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_8000;
  qosneg.mean_throughput_class = MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100;
  qosneg.traffic_class = MAL_GPDS_QOS_TRAFFICCLASS_CONVERSATIONAL;
  qosneg.delivery_order = MAL_GPDS_QOS_DELIVERY_ORDER_YES;
  qosneg.delivery_of_error_sdu = MAL_GPDS_QOS_DELIVERY_ERROR_SDU_YES;
  qosneg.residual_ber = 1;
  qosneg.sdu_error_ratio = 1;
  qosneg.trafic_handling_priority = MAL_GPDS_QOS_TRAFFIC_HANDLING_PRIO_2;
  qosneg.transfer_delay = 0x01;
  qosneg.maximum_sdu_size = 0x32;
  qosneg.maximum_bitrate_ul =0x12;
  qosneg.maximum_bitrate_dl =0x12;
  qosneg.guaranteed_bitrate_ul =0x12;
  qosneg.guaranteed_bitrate_dl =0x12;
  qosneg.src_stat_desc = MAL_GPDS_QOS_SRC_STAT_DESC_SPEECH;
  qosneg.sgn_ind = MAL_GPDS_QOS_OPT_SGN;

  TESTNOTNEG(util_modify_failure(cid, qosreq, qosrel5, qosmin, qosneg));
  TESTNOTNEG(util_modify(cid, qosreq, qosrel5, qosmin, qosneg));
  TESTNOTNEG(util_check(cid, ip, qosreq, qosrel5, qosneg));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

  error:
  return PSCCD_TEST_FAIL;
}


psccd_test_state_type_t tc_rel97(int state) {
  int cid = 1;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  //mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  qosneg.qos_type = MAL_GPDS_QOS_REL97_98;
  memset(&qosreq,0,sizeof(mal_gpds_qos_profile_t));
  qosreq.qos_type = MAL_GPDS_QOS_REL97_98;
  memset(&qosmin,0,sizeof(mal_gpds_qos_profile_t));

  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);

  TESTNOTNEG(util_create(cid));
  TESTNOTNEG(util_connect_rel97(cid, apn, ip, pdns, sdns, ifname, qosreq, qosmin, qosneg));
  TESTNOTNEG(util_check_rel97(cid, ip, qosreq, qosneg));

  qosreq.qos_type = MAL_GPDS_QOS_REL97_98;
  qosreq.precedence_class = MAL_GPDS_QOS_PRECEDENCE_CLASS_1;
  qosreq.delay_class = MAL_GPDS_QOS_DELAY_CLASS_1;
  qosreq.reliability_class = MAL_GPDS_QOS_RELIABILITY_CLASS_1;
  qosreq.peak_throughput_class = MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_8000;
  qosreq.mean_throughput_class = MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100;

  qosneg.qos_type = MAL_GPDS_QOS_REL97_98;
  qosneg.precedence_class = MAL_GPDS_QOS_PRECEDENCE_CLASS_1;
  qosneg.delay_class = MAL_GPDS_QOS_DELAY_CLASS_1;
  qosneg.reliability_class = MAL_GPDS_QOS_RELIABILITY_CLASS_1;
  qosneg.peak_throughput_class = MAL_GPDS_QOS_PEAK_THROUGHPUT_CLASS_8000;
  qosneg.mean_throughput_class = MAL_GPDS_QOS_MEAN_THROUGHPUT_CLASS_100;

  TESTNOTNEG(util_modify_rel97(cid, qosreq, qosmin, qosneg));
  TESTNOTNEG(util_check_rel97(cid, ip, qosreq, qosneg));
  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  return PSCCD_TEST_SUCCESS;

  error:
  return PSCCD_TEST_FAIL;
}

