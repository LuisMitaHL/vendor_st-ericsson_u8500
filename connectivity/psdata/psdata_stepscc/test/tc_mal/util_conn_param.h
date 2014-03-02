/*
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : util_conn_param.h
 * Description     : wrapper for using mal get/set params
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */
#ifndef TC_CONN_PARAM2_H_
#define TC_CONN_PARAM2_H_

int waitAndRespondToGPDSSet_APN(int retval, char apn[], int cid);
int waitAndRespondToGPDSSet_AuthMeth(int retval, mal_gpds_auth_method_t auth_meth, int cid);
int waitAndRespondToGPDSSet_PDPContextType(int retval, mal_gpds_pdp_context_type_t pdp_context_type, int cid);
int waitAndRespondToGPDSSet_PDPType(int retval, mal_gpds_pdp_type_t pdp_type, int cid);
int waitAndRespondToGPDSSet_HCMP(int retval, mal_gpds_pdp_hcmp_t hcmp, int cid);
int waitAndRespondToGPDSSet_HCMP_on(int retval, mal_gpds_pdp_hcmp_t hcmp, int cid);
int waitAndRespondToGPDSSet_DCMP(int retval, mal_gpds_pdp_dcmp_t dcmp, int cid);
int waitAndRespondToGPDSSet_IPAddress(int retval, ip_address_t ip, int cid);
int waitAndRespondToGPDSSet_DCMP_on(int retval, mal_gpds_pdp_dcmp_t dcmp, int cid);

int waitAndRespondToGPDSSet_QoSReq(int retval, mal_gpds_qos_profile_t qosreq, int cid);
int waitAndRespondToGPDSSet_QoSRel5(int retval, mal_gpds_qos_profile_t qosrel5, int cid);
int waitAndRespondToGPDSSet_QoSMin(int retval, mal_gpds_qos_profile_t qosmin, int cid);

int waitAndRespondToGPDSGet_DontCare(int retval);
int waitAndRespondToGPDSGet_IPMAP(int retval, int ip_map, int cid);
int waitAndRespondToGPDSGet_IP(int retval, ip_address_t ip, int cid);
int waitAndRespondToGPDSGet_IPv6(int retval, ip_address_t ipv6, int cid);
int waitAndRespondToGPDSGet_PDNS(int retval, ip_address_t pdns, int cid);
int waitAndRespondToGPDSGet_PDNSv6(int retval, ip_address_t ipv6_pdns, int cid);
int waitAndRespondToGPDSGet_SDNS(int retval, ip_address_t sdns, int cid);
int waitAndRespondToGPDSGet_SDNSv6(int retval, ip_address_t ipv6_sdns, int cid);
int waitAndRespondToGPDSGet_IFNAME(int retval, char ifname[], int cid);
int waitAndRespondToGPDSGet_QoSNeg(int retval, mal_gpds_qos_profile_t qosneg, int cid);
int waitAndRespondToGPDSSet_ChapId(int retval, uint8_t ChapId, int cid);
int waitAndRespondToGPDSSet_Username(int retval, void *user, int cid);
int waitAndRespondToGPDSSet_Password(int retval, void *password, int cid);
int waitAndRespondToGPDSSet_Challenge(int retval, mal_gpds_chap_auth_data_t *challenge, int cid);
int waitAndRespondToGPDSSet_Response(int retval, void *challenge_response, int cid);

#endif /* TC_CONN_PARAM2_H_ */
