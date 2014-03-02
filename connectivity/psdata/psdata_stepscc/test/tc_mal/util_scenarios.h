/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : util_scenarios.h
 * Description     : wrapper for using mal scenarios
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include "mal_gpds.h"
#include "shm_netlnk.h"

int util_setup_stecom();
int util_create(int cid);

int util_create_error(int cid);

int util_unexpected_events(int cid);

int util_error_paths(int cid);
int util_mal_failures(int cid);

int util_check(int cid , ip_address_t ip , mal_gpds_qos_profile_t qosreq , \
mal_gpds_qos_profile_t qosrel5 , mal_gpds_qos_profile_t qosneg);

int util_check_rel97(int cid , ip_address_t ip , mal_gpds_qos_profile_t qosreq , \
mal_gpds_qos_profile_t qosneg);

int util_connect(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg);

int util_connect_255(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg, uint8_t extended_ul_bitrate, uint8_t extended_dl_bitrate);

int util_connect_rel97(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq,mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg);

int util_connect_for_ipv4v6(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type);


int util_connect_for_ipv6(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type);


int util_connect_for_ipv4(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type);

int util_connect_for_chap_generation(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg, char user[]);

int util_connect_for_chap(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg, char user[]);

int util_connect_for_pap(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg, char user[], char password[]);

int util_connect_and_fetch_counters(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type);

int util_connect_and_reset_counters(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type);

int util_connect_for_uplink(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type);

int util_list_all_connection(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg);

int util_static_ipv4(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,char static_ip[],mal_gpds_pdp_type_t pdp_type);

int util_modify_failure(int cid, mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg);

int util_modify(int cid, mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg);

int util_modify_rel97(int cid, mal_gpds_qos_profile_t qosreq,mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg);

int util_disconnect(int cid);


int util_destroy(int cid);

int util_cleanup();
