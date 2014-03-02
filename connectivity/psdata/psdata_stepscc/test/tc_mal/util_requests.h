/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : util_requests.h
 * Description     : wrapper for using mal request functions
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include "mal_gpds.h"
int waitAndRespondToGPDSReq_Init(int retval);
int waitAndRespondToGPDSReq_RegisterCallback();
int waitAndRespondToGPDSReq_Request(int retval, int connid, mal_gpds_req_ids_t req_id, void* data, int datalen);
int waitAndRespondToGPDSReq_ResponseHandler();

int waitAndRespondToSim_ste_sim_new_st(ste_sim_t ste_sim, ste_sim_closure_t closure);
int waitAndRespondToSim_ste_sim_connect(int retval, ste_sim_t sim, uintptr_t client_tag);
int waitAndRespondToSim_ste_sim_fd(int fd, ste_sim_t sim);
int waitAndRespondToSim_ste_sim_disconnect(int retval, ste_sim_t sim, uintptr_t client_tag);
int waitAndRespondToSim_ste_sim_delete(int retval, ste_sim_t sim, uintptr_t client_tag);
int waitAndRespondToSim_ste_sim_read(int retval, ste_sim_t sim);
int waitAndRespondToSim_ste_uicc_get_service_availability(int retval, ste_sim_t sim, uintptr_t* client_tag_p,sim_service_type_t service_type);
int waitAndRespondToSim_ste_cat_call_control(int retval, ste_sim_t sim, uintptr_t client_tag, char* pdpactdata_p, uint32_t strlen);
