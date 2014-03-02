/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

#include "phonet_lib_misc.h"
#include "phonet_lib_api.h"
#include "libmon.h"
#include "dgramsk.h"
#include "utils.h"

#define MALMON_MAX_REQ_SIZE 0x100

/*  libmon context  */
struct libmon_ctx_t {
    /*  thread listening to phonet  */
    pthread_t aphonet;
    /*  thread listening to modemlogrelay config */
    pthread_t amonreq;
    int fd_mon;
    int fd_ind;
    uint16_t client_tag;
    /*  buffer for reception */
    void *sckreq;
    void *sckrsp;
    void *sckind;
    void *sckind_inv;
};

/*  fdmon is used to detect if a previous init has been done */
static struct libmon_ctx_t libmon_ctx = {.fd_mon = 0 };



static void mon_callback_handler(int message_id, void *data, \
                                 uint32_t mal_error, void *client_tag)
{
    tLIB_Packet_Ind Ind;
    tLIB_Packet_Rsp Rsp;
    char *packet = (char *)NULL;
    int packetsize = 0;
    void *msg = (void *) NULL;
    void *dest = (void *) NULL;
    int msgsize = 0;

    if (mal_error) {
        return;
    }

    switch (message_id) {
        /*  indication message  */
    case MAL_MON_TRACE_INV_IND:
        EXTRADEBUG("MAL_MON_TRACE_INV_IND");
        Ind.id = message_id;
        packet = (char *)&Ind;
        packetsize = sizeof(Ind);
        msg = (void *) &Ind.msg.inv;
        msgsize = sizeof(mon_trace_inv_ind_t);
        dest = libmon_ctx.sckind_inv;
        break;
    case MAL_MON_TRACE_LOG_TRIGGER_IND:
        EXTRADEBUG("MAL_MON_TRACE_LOG_TRIGGER_IND");
        Ind.id = message_id;
        packet = (char *)&Ind;
        packetsize = sizeof(Ind);
        msg = (void *) &Ind.msg.trigger;
        msgsize = sizeof(mon_trace_log_trigger_ind_t);
        dest = libmon_ctx.sckind;
        break;
    case MAL_MON_TRACE_BUFFER_FLUSH_IND:
        EXTRADEBUG("MAL_MON_TRACE_BUFFER_FLUSH_IND");
        Ind.id = message_id;
        packet = (char *)&Ind;
        packetsize = sizeof(Ind);
        msg = (void *) &Ind.msg.flush;
        msgsize = sizeof(mon_trace_flush_ind_t);
        dest = libmon_ctx.sckind;
        break;

        /*  message with only status response  */
    case MAL_MON_TRACE_ACTIVATE_RESP:
    case MAL_MON_TRACE_DEACTIVATE_RESP:
    case MAL_MON_TRACE_CONFIG_SET_RESP:
    case MAL_MON_TRACE_ROUTING_RESP:
    case MAL_MON_TRACE_TRIGGER_RESP:
    case MAL_MON_TRACE_IDS_ACTIVATE_ROUTE_RESP:
    case MAL_MON_TRACE_BUFFER_MARKER_RESP:
    case MAL_MON_TRACE_BUFFERING_MODE_RESP:
        EXTRADEBUG("MAL_MON_TRACE _????_RESP");

        Rsp.id = message_id;
        packet = (char *)&Rsp;
        packetsize = sizeof(Rsp);
        msg = (void *) &Rsp.msg.stat;
        msgsize = sizeof(mon_trace_resp_t);
        dest = libmon_ctx.sckrsp;
        break;
        /*  messsage with additional content  */
    case MAL_MON_TRACE_CONFIG_GET_RESP:
        EXTRADEBUG("MAL_MON_TRACE _CONFIG_GET_RESP");

        Rsp.id = message_id;
        packet = (char *)&Rsp;
        packetsize = sizeof(Rsp);
        msg = (void *) &Rsp.msg.get;
        msgsize = sizeof(mon_trace_config_get_rsp_t);
        dest = libmon_ctx.sckrsp;
        break;
    case MAL_MON_TRACE_ACTIVATION_READ_STATUS_RESP:
        EXTRADEBUG("MAL_MON_TRACE_READ_STATUS_RESP");
        Rsp.id = message_id;
        packet = (char *)&Rsp;
        packetsize = sizeof(Rsp);
        msg = (void *) &Rsp.msg.read;
        msgsize = sizeof(mon_trace_act_read_status_rsp_t);
        dest = libmon_ctx.sckrsp;
        break;
    case MAL_MON_TRACE_BUFFERING_MODE_READ_RESP:
        EXTRADEBUG("MAL_MON_TRACE_BUFFERING_MODE_READ_RESP");
        Rsp.id = message_id;
        packet = (char *)&Rsp;
        packetsize = sizeof(Rsp);
        msg = (void *) &Rsp.msg.buffering_mode_get;
        msgsize = sizeof(mon_trace_buffering_mode_t);
        dest = libmon_ctx.sckrsp;
        break;
    default:
        break;
    }

    if (dest != NULL && packet != NULL) {
        memcpy(msg, data, msgsize);
        dgram_sk_send(dest, (char *) packet, packetsize);
    }
}

static void *mon_listen_skreq(void *data)
{
    uint32_t mal_error;
    tLIB_Packet_Req req;
    int ret;

    do {
        ret = dgram_sk_recv(libmon_ctx.sckreq, (char *)&req);

        if (ret >= 2) {
            /*  queue the request  */
            mal_error = mal_mon_request(req.id, &req.msg, NULL);

            if (mal_error) {
                ALOGE("error in malmon_request\n");
            }
        }

    } while (1);

    return NULL;
}


static void *mon_select_on_socket(void *data)
{
    fd_set readfds;
    int    ret = -1;
    int fd_max;

    /* struct timeval tv = {.tv_sec = 10, .tv_usec = 0, }; */
    do {
        FD_ZERO(&readfds);
#ifdef FD_IND

        if (libmon_ctx.fd_ind > libmon_ctx.fd_mon) {
            fd_max = libmon_ctx.fd_ind ;
        } else {
            fd_max = libmon_ctx.fd_mon ;
        }

        FD_SET(libmon_ctx.fd_mon, &readfds);
        FD_SET(libmon_ctx.fd_ind, &readfds);
#else
        fd_max = libmon_ctx.fd_mon ;
        FD_SET(libmon_ctx.fd_mon, &readfds);
#endif

        ret =  select(fd_max + 1, &readfds, NULL, NULL, \
                      NULL);

        if (ret == 0) {
            EXTRADEBUG("select: with timeout\n");
        } else if (ret < 0) {
            EXTRADEBUG("select: error returned\n");
        } else if (ret > 0) {

            if (FD_ISSET(libmon_ctx.fd_mon, &readfds)) {
                mal_mon_response_handler(libmon_ctx.fd_mon);
            }

#ifdef FD_IND

            if (FD_ISSET(libmon_ctx.fd_ind, &readfds)) {
                mal_mon_response_handler(libmon_ctx.fd_ind);
            }

#endif

        }
    } while (1);

    return NULL;
}


/*  at startup  */
int mon_trace_init(char *sckreq, char *sckrsp, char *sckind, char *sckind_inv)
{
    uint32_t mal_error = 0;

    /* initialization of interface with libmalmon */
    if (libmon_ctx.fd_mon) {
        return -1;
    }

    mal_error = mal_mon_init(&libmon_ctx.fd_mon);

    if (mal_error) {
        ALOGE("\nMAL MON INIT FAILED...\n");
        return -1;
    }

    mal_error = mal_mon_register_callback(mon_callback_handler);

    if (mal_error) {
        ALOGE("\nMAL MON REGISTER CALLBACK FAILED...\n");
    }

    /*  initialized socket with user */
    /*  */
    libmon_ctx.sckreq = dgram_sk_init(DGRAM_SK_SERVER, sckreq,
                                      MALMON_MAX_REQ_SIZE);
    libmon_ctx.sckrsp = dgram_sk_init(DGRAM_SK_CLIENT, sckrsp, 0);
    libmon_ctx.sckind = dgram_sk_init(DGRAM_SK_CLIENT, sckind, 0);
    libmon_ctx.sckind_inv = dgram_sk_init(DGRAM_SK_CLIENT, sckind_inv, 0);

    /*  launch phonet listening  */
    pthread_create(&libmon_ctx.aphonet, NULL, mon_select_on_socket, NULL);
    /*  launch req listening  */
    pthread_create(&libmon_ctx.amonreq, NULL, mon_listen_skreq, NULL);


#ifdef FD_IND
    mal_mon_config(&libmon_ctx.fd_ind);
#endif
    return 0;
}
