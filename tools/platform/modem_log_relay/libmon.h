/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "mal_monitor.h"

int mon_trace_init(char *skreq, char *sckrsp, char *sckind, char *sckind_inv);
int mon_trace_desinit(void);

/* socket message */
/*  packet for req */
typedef struct {
    uint8_t id;
    union {
        /*   MAL_MON_TRACE_ACTIVATE_REQ  */
        mon_trace_act_req_t act;
        /*   MAL_MON_TRACE_IDS_ACTIVATE_ROUTE_REQ */
        mon_trace_ids_act_route_req_t route;
        /*  MAL_MON_TRACE_CONFIG_SET_REQ */
        mon_trace_conf_set_req_t set;
        /*  MAL_MON_TRACE_DESACTIVATE_REQ   */
        mon_trace_deact_req_t deact;
        /*  MAL_MON_TRACE_ACTIVATION_READ_STATUS_REQ */
        mon_trace_act_read_status_req_t read;
        /*  MAL_MON_TRACE_CONFIG_GET_REQ */
        /*  no structure required */
        /*  MAL_MON_TRACE_ROUTING_REQ */
        mon_trace_routing_req_t routing;
        /*  MAL_MON_TRACE_TRIGGER_REQ */
        mon_trace_trigger_req_t trigger;
        /*  MAL_MON_TRACE_BUFFER_MARKER_REQ */
        mon_trace_buffer_marker_req_t marker;
        /* MAL_MON_TRACE_BUFFERING_MODE_REQ */
        mon_trace_buffering_mode_t buffering_mode_set;
    } msg;

} tLIB_Packet_Req;

/*  packet for resp */
typedef struct {
    uint8_t id;
    union {
        /*  MAL_MON_TRACE_ACTIVATE_RESP  */
        /*  MAL_MON_TRACE_DESACTIVATE_RESP */
        /*  MAL_MON_TRACE_CONFIG_SET_RESP  */
        /*  MAL_MON_TRACE_IDS_ACTIVATE_ROUTE_RESP */
        /*  MAL_MON_TRACE_ROUTING_RESP */
        /*  MAL_MON_TRACE_BUFFER_MARKER_RESP */
        /*  MAL_MON_TRACE_BUFFERING_MODE_RESP */
        uint8_t stat;
        /* MAL_MON_TRACE_ACTIVATION_READ_STATUS_RESP   */
        mon_trace_act_read_status_rsp_t read;
        /*  MAL_MON_TRACE_CONFIG_GET_RESP */
        mon_trace_config_get_rsp_t get;
        /*  MAL_MON_TRACE_BUFFERING_MODE_READ_RESP */
        mon_trace_buffering_mode_t buffering_mode_get;
    } msg;

} tLIB_Packet_Rsp;

/*  structure definition for indication */
/*  packet for indication */
typedef struct {
    uint8_t id;
    union {
        /* MAL_MON_TRACE_BUFFER_FLUSH_IND */
        mon_trace_flush_ind_t flush;
        /*  MAL_MON_TRACE_LOG_TRIGGER_IND */
        mon_trace_log_trigger_ind_t trigger;
        /*  MAL_MON_TRACE_INV_IND */
        mon_trace_inv_ind_t inv;
    } msg;
} tLIB_Packet_Ind;

enum {
    NO_SERVICE_POWER_SAVE = 0xB1,
    LIMITED_SERVICE = 0xB2,
    NO_SIM = 0xB3
} Modem_Trigger_Ind_type;




