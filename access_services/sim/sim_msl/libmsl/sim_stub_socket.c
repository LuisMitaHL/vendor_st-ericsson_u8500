/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_stub_socket.c
 * Description     : Simulated modem implementation with socket.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "simd.h"
#include "cat_internal.h"
#include "sim_internal.h"
#include "cat_barrier.h"
#include "sim_stub.h"
#include "func_trace.h"
#include "sim_unused.h"

#ifndef HAVE_ANDROID_OS
#ifndef HOST_FS_PATH
#define HOST_FS_PATH ""
#endif
static const char       socket_name[] = HOST_FS_PATH "/tmp/sim_stub_a";
#else
static const char       socket_name[] = "/dev/socket/sim_stub_a"; 
#endif

#define SIM_STUB_SOCKET_INPUT_BUFFER_SIZE       (2048)


#define SIM_STUB_SOCKET_MSG_START       (0x10000001)
#define SIM_STUB_SOCKET_MSG_STOP        (0x10000002)

struct sim_stub_socket_cmd_s 
{
    uint32_t                type;
    void                   *vptr;
};
typedef struct sim_stub_socket_cmd_s sim_stub_socket_cmd_t;


#define SIM_STUB_SOCKET_PIPE_BUFFER_SIZE    (64)


struct sim_stub_socket_s 
{
    int                     pipe_put;
};

#define SIM_STUB_MAX_NR_OF_CLIENTS   1

typedef struct 
{
    char                   *buf_lo;     /* points at start of data */
    char                   *buf_hi;     /* points at end of data + 1 */
    char                   *buf;        /* points to the buffer */
    char                   *buf_max;    /* points at top of buffer + 1 */
} sim_stub_socket_buf_t;


typedef struct 
{
    int                     fd;
    sim_stub_socket_buf_t   socket_buf;
} sim_stub_socket_client_t;

typedef struct {
    int                       pipe_get;
    sim_stub_socket_buf_t     pipe_buf;
    int                       input_socket;
    sim_stub_socket_client_t  client_socket[SIM_STUB_MAX_NR_OF_CLIENTS];    /* Max 1 client, for PC testing */
    cat_barrier_t            *bar;                                          /* barrier to be used in shutdown */
} sim_stub_socket_thread_data_t;

static int sim_stub_pc_fd = -1;

//#####################################################################################
// Functions
//#####################################################################################
static void
sim_stub_socket_close_client(sim_stub_socket_client_t * client_p)
{
    if (!client_p)
    {
        return;
    }
    if (client_p->fd != -1)
    {
        close(client_p->fd);
        client_p->fd = -1;
        free(client_p->socket_buf.buf);
        memset(&(client_p->socket_buf), 0, sizeof(sim_stub_socket_buf_t));
    }
}

static void
sim_stub_socket_send_tr_done(int fd)
{
    char done[] = "DONE\n";
    int rc = -1;

    if (fd != -1)
    {
        rc = write(fd, done, strlen(done));
        printf("stub_socket : Sent DONE to client, write = %d\n", rc);
    }
}

static void
sim_stub_socket_handle_command(uint32_t cmd,
                               uintptr_t UNUSED(client_tag),
                               const char *UNUSED(buf),
                               uint16_t UNUSED(len), 
                               void *ud)
{
    printf("stub_socket : received client command: cmd = %x\n", cmd);

    //save the client fd to send back the terminal response done signal
    //if for one specific cmd which does not need a terminal response, the value of
    //sim_stub_pc_fd should be reset to -1 in that case, such as the case SIM_MODEM_CMD_MODEM_DISCONNECT
    sim_stub_pc_fd = ((sim_stub_socket_client_t*)ud)->fd;

    switch ( cmd ) 
    {
        case SIM_MODEM_CMD_MODEM_DISCONNECT:
        {
            sim_stub_socket_client_t * client_p = (sim_stub_socket_client_t *) ud;
            printf("stub_socket : get disconnect req from client.\n");
            sim_stub_socket_close_client(client_p);
            //terminal response is not necessary here, so reset the value
            sim_stub_pc_fd = -1;
            return;
        }
        break;

        case SIM_MODEM_CMD_PC_DISPLAY_TEXT:
        {
            printf("stub_socket : get PC display text req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_display_text();
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_MENU:
        {
            printf("stub_socket : get PC setup menu req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_setup_menu();
        }
        break;

        case SIM_MODEM_CMD_PC_SELECT_ITEM:
        {
            printf("stub_socket : get PC select item req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_select_item();
        }
        break;

        case SIM_MODEM_CMD_PC_GET_INKEY:
        {
            printf("stub_socket : get PC get inkey req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_get_inkey();
        }
        break;

        case SIM_MODEM_CMD_PC_GET_INPUT:
        {
            printf("stub_socket : get PC get input req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_get_input();
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_IMEI:
        {
            printf("stub_socket : get PC provide local info req (IMEI).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ME_IMEI);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_IMEISV:
        {
            printf("stub_socket : get PC provide local info req (IMEISV).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ME_IMEISV);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_BATTERY:
        {
            printf("stub_socket : get PC provide local info req (battery).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_BATTERY);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_LOCATION:
        {
            printf("stub_socket : get PC provide local info req (location).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_LOCATION_INFO);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_NW_SEARCH_MODE:
        {
            printf("stub_socket : get PC provide local info req (network search mode).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_SEARCH_MODE);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_TIMING_ADVANCE:
        {
            printf("stub_socket : get PC provide local info req (timing advance).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_TIMING_ADVANCE);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_ACCESS_TECHNOLOGY:
        {
            printf("stub_socket : get PC provide local info req (access technology).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ACCESS_TECH);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_NMR:
        {
            printf("stub_socket : get PC provide local info req (nmr).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_NW_MEASURE_RESULT);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_DATE_TIME:
        {
            printf("stub_socket : get PC provide local info req (date, time and timezone).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_DATE_TIME_ZONE);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_LANGUAGE:
        {
            printf("stub_socket : get PC provide local info req (language).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_LANGUAGE);
        }
        break;

        case SIM_MODEM_CMD_PC_PROVIDE_LOCAL_INFO_DEFAULT:
        {
            printf("stub_socket : get PC provide local info req (default).\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_provide_local_info((uint8_t)STE_SAT_PROVIDE_LOCAL_INFO_TYPE_END);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_EVENT_LIST:
        {
            printf("stub_socket : get PC setup event list req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_setup_event_list();
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_EVENT_LIST_01:
        {
            printf("stub_socket : get PC setup event list 01 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_setup_event_list_no_list();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE:
        {
            printf("stub_socket : get PC send short message req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_send_short_message();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_01:
        {
            printf("stub_socket : get PC send short message 01 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_send_short_message_packing_required();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_02:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_CC_FAILURE);
            printf("stub_socket : get PC send short message 02 req.\n");
            ste_stub_pc_send_short_message();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_03:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_FAILURE);
            printf("stub_socket : get PC send short message 03 req.\n");
            ste_stub_pc_send_short_message();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SHORT_MESSAGE_04:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            printf("stub_socket : get PC send short message 04 req.\n");
            ste_stub_pc_send_short_message_wrong_dcs();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_DTMF:
        {
            printf("stub_socket : get PC send dtmf req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_send_dtmf();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_DTMF_01:
        {
            printf("stub_socket : get PC send dtmf 01 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_send_dtmf_01();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_DTMF_02:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_RESP_FAILURE);
            printf("stub_socket : get PC send dtmf 02 req.\n");
            ste_stub_pc_send_dtmf();
        }
        break;


        case SIM_MODEM_CMD_PC_SEND_SS:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            printf("stub_socket : get PC send ss 01 req.\n");
            ste_stub_pc_send_ss();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SS_01:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_CC_FAILURE);
            printf("stub_socket : get PC send ss 02 req.\n");
            ste_stub_pc_send_ss();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SS_02:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_CC_TEMP_FAILURE);
            printf("stub_socket : get PC send ss 03 req.\n");
            ste_stub_pc_send_ss();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SS_03:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_SS_BUSY_FAILURE);
            printf("stub_socket : get PC send ss 03 req.\n");
            ste_stub_pc_send_ss();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SS_04:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_SS_NOT_AVAILABLE_FAILURE);
            printf("stub_socket : get PC send ss 04 req.\n");
            ste_stub_pc_send_ss();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SS_05:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_USSD_BUSY_FAILURE);
            printf("stub_socket : get PC send ss 05 req.\n");
            ste_stub_pc_send_ss();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SS_06:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_ModMM_FAILURE);
            printf("stub_socket : get PC send ss 06 req.\n");
            ste_stub_pc_send_ss();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SS_07:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_ModCC_FAILURE);
            printf("stub_socket : get PC send ss 07 req.\n");
            ste_stub_pc_send_ss();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SS_08:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_FAILURE);
            printf("stub_socket : get PC send ss 08 req.\n");
            ste_stub_pc_send_ss();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_SS_09:
        {
            printf("stub_socket : get PC send ss 09 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_send_ss_no_string();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            printf("stub_socket : get PC send ussd req.\n");
            ste_stub_pc_send_ussd();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD_01:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_CC_FAILURE);
            printf("stub_socket : get PC send ussd 01 req.\n");
            ste_stub_pc_send_ussd();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD_02:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_CC_TEMP_FAILURE);
            printf("stub_socket : get PC send ussd 02 req.\n");
            ste_stub_pc_send_ussd();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD_03:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_SS_BUSY_FAILURE);
            printf("stub_socket : get PC send ussd 03 req.\n");
            ste_stub_pc_send_ussd();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD_04:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_USSD_BUSY_FAILURE);
            printf("stub_socket : get PC send ussd 04 req.\n");
            ste_stub_pc_send_ussd();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD_05:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_SS_NOT_AVAILABLE_FAILURE);
            printf("stub_socket : get PC send ussd 05 req.\n");
            ste_stub_pc_send_ussd();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD_06:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_ModMM_FAILURE);
            printf("stub_socket : get PC send ussd 06 req.\n");
            ste_stub_pc_send_ussd();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD_07:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_ModCC_FAILURE);
            printf("stub_socket : get PC send ussd 07 req.\n");
            ste_stub_pc_send_ussd();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD_08:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_FAILURE);
            printf("stub_socket : get PC send ussd 08 req.\n");
            ste_stub_pc_send_ussd();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_USSD_09:
        {
            printf("stub_socket : get PC send ussd 09 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_send_ussd_no_string();
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL:
        {
            printf("stub_socket : get PC setup call.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            // set up call, but only if not currently busy on another call
            ste_stub_pc_setup_call(0x00);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_00:
        {
            printf("stub_socket : get PC setup call 00.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_REQ_DIAL_DURATION);
            // set up call, but only if not currently busy on another call
            ste_stub_pc_setup_call_with_duration(0x00);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_01:
        {
            printf("stub_socket : get PC setup call 01.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            // set up call, with redial but only if not currently busy on another call
            ste_stub_pc_setup_call_with_sub_addr(0x01);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_02:
        {
            printf("stub_socket : get PC setup call 02.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_REQ_FAILURE);
            // set up call, but only if not currently busy on another call
            ste_stub_pc_setup_call(0x00);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_03:
        {
            printf("stub_socket : get PC setup call 03.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_REQ_DIAL_FAILURE);
            // set up call, but only if not currently busy on another call
            ste_stub_pc_setup_call(0x00);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_04:
        {
            printf("stub_socket : get PC setup call 04.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_RESP_DIAL_FAILURE);
            // set up call, but only if not currently busy on another call
            ste_stub_pc_setup_call(0x00);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_ECALL:
        {
            printf("stub_socket : get PC setup ecall.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_ecall(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT:
        {
            printf("stub_socket : get PC setup call, disconnect others.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_01:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_CC_FAILURE);
            printf("stub_socket : get PC setup call, disconnect others 01.\n");
            //set up call, disconnecting all other calls (if any), with redial
            ste_stub_pc_setup_call(0x05);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_02:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_RESP_FAILURE);
            printf("stub_socket : get PC setup call, disconnect others 02.\n");
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_03:
        {
            ste_stub_set_response_type(STE_SIM_STUB_SEND_USDD_AFTER_CC_ERROR);
            printf("stub_socket : get PC setup call, disconnect others 03.\n");
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_04:
        {
            ste_stub_set_response_type(STE_SIM_STUB_SEND_USDD_AFTER_CC);
            printf("stub_socket : get PC setup call, disconnect others 04.\n");
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_05:
        {
            printf("stub_socket : get PC setup call, disconnect others 05.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call_no_addr(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_06:
        {
            /* Set Call Status here so that Hangup Request is sent */
            ste_stub_set_call_status( CN_CALL_STATE_ACTIVE );
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_REQ_HANGUP_FAILURE);
            printf("stub_socket : get PC setup call, disconnect others 06.\n");
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_07:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_RESP_HANGUP_FAILURE);
            printf("stub_socket : get PC setup call, disconnect others 07.\n");
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_DISCONNECT_08:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_REQ_CALL_LIST_FAILURE);
            printf("stub_socket : get PC setup call, disconnect others 08.\n");
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_HOLD:
        {
            printf("stub_socket : get PC setup call, hold others.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call(0x02);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_HOLD_01:
        {
            /* Set Call Status here so that Hold Request is sent */
            ste_stub_set_call_status( CN_CALL_STATE_ACTIVE );
            printf("stub_socket : get PC setup call, hold others 01.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_REQ_HOLD_FAILURE);
            //set up call, disconnecting all other calls (if any), with redial
            ste_stub_pc_setup_call(0x03);
        }
        break;

        case SIM_MODEM_CMD_PC_SETUP_CALL_HOLD_02:
        {
            /* Set Call Status here so that Hold Request is sent */
            ste_stub_set_call_status( CN_CALL_STATE_ACTIVE );
            printf("stub_socket : get PC setup call, hold others 02.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_CN_RESP_HOLD_FAILURE);
            //set up call, disconnecting all other calls (if any)
            ste_stub_pc_setup_call(0x02);
        }
        break;

        case SIM_MODEM_CMD_PC_POLL_INTERVAL:
        {
            printf("stub_socket : get PC poll interval req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_poll_interval();
        }
        break;

        case SIM_MODEM_CMD_PC_POLL_INTERVAL_01:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_MODEM_REQ_FAILURE);
            printf("stub_socket : get PC poll interval 01 req.\n");
            ste_stub_pc_poll_interval();
        }
        break;

        case SIM_MODEM_CMD_PC_POLL_INTERVAL_02:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_MODEM_RESP_FAILURE);
            printf("stub_socket : get PC poll interval 02 req.\n");
            ste_stub_pc_poll_interval();
        }
        break;

        case SIM_MODEM_CMD_PC_POLLING_OFF:
        {
            printf("stub_socket : get PC polling off req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_polling_off();
        }
        break;

        case SIM_MODEM_CMD_PC_POLLING_OFF_01:
        {
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_MODEM_REQ_FAILURE);
            printf("stub_socket : get PC polling off 01 req.\n");
            ste_stub_pc_polling_off();
        }
        break;

        case SIM_MODEM_CMD_PC_POLLING_OFF_02:
        {
            ste_stub_set_response_type( STE_SIM_STUB_SEND_NON_ZERO_POLL_INTERVAL );
            printf("stub_socket : get PC polling off 02 req.\n");
            ste_stub_pc_polling_off();
        }
        break;

        case SIM_MODEM_CMD_PC_MORE_TIME:
        {
            printf("stub_socket : get PC more time req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_more_time();
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_01:
        {
            printf("stub_socket : get PC timer_management_01 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_01();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_02:
        {
            printf("stub_socket : get PC timer_management_02 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_02();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_03:
        {
            printf("stub_socket : get PC timer_management_03 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_03();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_04:
        {
            printf("stub_socket : get PC timer_management_04 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_04();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_05:
        {
            printf("stub_socket : get PC timer_management_05 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_05();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_06:
        {
            printf("stub_socket : get PC timer_management_06 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_06();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_07:
        {
            printf("stub_socket : get PC timer_management_07 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_07();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_08:
        {
            printf("stub_socket : get PC timer_management_08 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_08();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_09:
        {
            printf("stub_socket : get PC timer_management_09 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_09();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_10:
        {
            printf("stub_socket : get PC timer_management_10 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_10();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_11:
        {
            printf("stub_socket : get PC timer_management_11 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_11();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_12:
        {
            printf("stub_socket : get PC timer_management_12 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_12();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_13:
        {
            printf("stub_socket : get PC timer_management_13 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_13();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_14:
        {
            printf("stub_socket : get PC timer_management_14 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_14();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_15:
        {
            printf("stub_socket : get PC timer_management_15 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_15();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_TIMER_MANAGEMENT_16:
        {
            printf("stub_socket : get PC timer_management_16 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_HOLD_TIMER_EC_RESPONSE_TILL_TR_RESPONSE);
            //too many terminal responses in one timer management test case
            //so call the terminal response done function here and reset the fd value
            sim_stub_pc_fd = -1;
            ste_stub_pc_timer_management_16();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH:
        {
            printf("stub_socket : get PC refresh req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            //terminal response might not present here(for refresh type: uicc reset,
            //so call the terminal response done function here and reset the fd value
//            sim_stub_pc_fd = -1;
            ste_stub_pc_refresh();
//            sleep(1);
//            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_REFRESH_RESET_IND:
        {
            printf("stub_socket : get refresh reset ind.\n");
            ste_stub_refresh_reset_ind();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);

        }
        break;

        case SIM_MODEM_CMD_CAT_NOT_READY_IND:
        {
            printf("stub_socket : get cat not ready ind.\n");
            ste_stub_cat_not_ready_ind();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);

        }
        break;

        case SIM_MODEM_CMD_CAT_NOT_SUPPORTED:
        {
            printf("stub_socket : get cat not supported.\n");
            ste_stub_cat_not_supported();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);

        }
        break;

        case SIM_MODEM_CMD_CARD_REMOVED_IND:
        {
            printf("stub_socket : get card removed ind.\n");
            ste_stub_card_removed_ind();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);

        }
        break;

        case SIM_MODEM_CMD_CARD_SHUTDOWN:
        {
            printf("stub_socket : get card shutdown.\n");
            ste_stub_card_shutdown();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);

        }
        break;
        case SIM_MODEM_CMD_SET_UICC_FAIL_TESTS:
        {
            printf("stub_socket : get SIM_MODEM_CMD_UICC_FAIL_TESTS.\n");
            ste_stub_set_uicc_fail_tests();
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);

        }
        break;

        case SIM_MODEM_CMD_RESET_UICC_FAIL_TESTS:
        {
            printf("stub_socket : get SIM_MODEM_CMD_UICC_FAIL_TESTS.\n");
            ste_stub_reset_uicc_fail_tests();
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);

        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_01:
        {
            printf("stub_socket : get PC refresh 01 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_MODEM_REQ_FAILURE);
            ste_stub_pc_refresh();
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_02:
        {
            printf("stub_socket : get PC refresh 02 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_MODEM_RESP_FAILURE);
            ste_stub_pc_refresh();
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_03:
        {
            printf("stub_socket : get PC refresh 03 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_refresh_no_aid();
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_04:
        {
            printf("stub_socket : get PC refresh 04 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_refresh_no_file_list();
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_RESET:
        {
          printf("stub_socket : get PC refresh reset req.\n");
          ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
          //terminal response might not present here(for refresh type: uicc reset,
          //so call the terminal response done function here and reset the fd value
          sim_stub_pc_fd = -1;
          ste_stub_pc_refresh_opt(0x04);
          sleep(1);
          sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_RESET_01:
        {
          printf("stub_socket : get PC refresh reset 01 req.\n");
          ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_MODEM_REQ_FAILURE);
          ste_stub_pc_refresh_opt(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_RESET_02:
        {
          printf("stub_socket : get PC refresh reset 02 req.\n");
          ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_MODEM_RESP_FAILURE);
          ste_stub_pc_refresh_opt(0x04);
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION:
        {
          printf("stub_socket : get PC refresh full change notification req.\n");
          ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
          //terminal response might not present here(for refresh type: uicc reset,
          //so call the terminal response done function here and reset the fd value
//          sim_stub_pc_fd = -1;
          ste_stub_pc_refresh_opt(0x00);
//          sleep(1);
//          sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION_01:
        {
          printf("stub_socket : get PC refresh full change notification 01 req.\n");
          ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_MODEM_REQ_FAILURE);
          ste_stub_pc_refresh_opt(0x00);
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION_02:
        {
          printf("stub_socket : get PC refresh full change notification 02 req.\n");
          ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_MODEM_RESP_FAILURE);
          ste_stub_pc_refresh_opt(0x00);
        }
        break;

        case SIM_MODEM_CMD_PC_REFRESH_FULL_CHANGE_NOTIFICATION_03:
        {
          printf("stub_socket : get PC refresh full change notification 03 req.\n");
          ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
          ste_stub_pc_refresh_opt_with_aid(0x00);
        }
        break;

        case SIM_MODEM_CMD_IND_MODEM_RESET:
        {
            printf("stub_socket : get  Modem reset ind.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_ind_modem_reset();
        }
        break;

        case SIM_MODEM_CMD_PC_LANGUAGE_NOTIFICATION:
        {
            printf("stub_socket : get PC language notification req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_language_notification();
        }
        break;

        case SIM_MODEM_CMD_CARD_FALLBACK:
        {
            printf("sim_stub : get SIM card fallback ind\n");
            sim_stub_pc_fd = -1;
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_card_fallback_indication();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_PC_OPEN_CHANNEL:
        {
            printf("stub_socket : get PC Open Channel req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_open_channel();
        }
        break;

        case SIM_MODEM_CMD_PC_CLOSE_CHANNEL:
        {
            printf("stub_socket : get PC Close Channel req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_close_channel();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_DATA:
        {
            printf("stub_socket : get PC Send Data req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_send_data();
        }
        break;

        case SIM_MODEM_CMD_PC_SEND_DATA_01:
        {
            printf("stub_socket : get PC Send Data 01 req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_HOLD_TR_RESPONSE_FOR_ONE_MINUTE);
            ste_stub_pc_send_data();
        }
        break;

        case SIM_MODEM_CMD_PC_RECEIVE_DATA:
        {
            printf("stub_socket : get PC Receive Data req.\n");
            ste_stub_set_response_type(STE_SIM_STUB_RSP_TYPE_SUCCESS);
            ste_stub_pc_receive_data();
        }
        break;

        case SIM_MODEM_CMD_SEND_CAT_READY:
        {
            printf("stub_socket: get Send CAT Ready\n");
            ste_stub_send_cat_ready();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        case SIM_MODEM_CMD_SEND_UICC_READY:
        {
            printf("stub_socket: get Send UICC Ready\n");
            ste_stub_send_uicc_ready();
            sleep(5);
            sim_stub_socket_send_tr_done(((sim_stub_socket_client_t*)ud)->fd);
        }
        break;

        default:
        {
            //reset the value
            sim_stub_pc_fd = -1;
            printf("stub_socket : unknown cmd.\n");
        }
    }
}


static ssize_t
sim_stub_socket_socket_parse_func(char *buf, char *buf_max, void *ud)
{
    const char             *p = buf;
    uint16_t                len;
    uint16_t                cmd;
    uintptr_t               client_tag;
    ssize_t                 bytes_consumed = 0;

    if (!buf && !buf_max) 
    {
        return -1;
    }
    do 
    {
        p = sim_dec(p, &len, sizeof(len), buf_max);
        if (!p)
        {
            break;
        }

        p = sim_dec(p, &cmd, sizeof(cmd), buf_max);
        if (!p)
        {
            break;
        }

        p = sim_dec(p, &client_tag, sizeof(client_tag), buf_max);
        if (!p)
        {
            break;
        }

        len -= sizeof(cmd) + sizeof(client_tag);

        sim_stub_socket_handle_command(cmd, client_tag, p, len, ud);

        bytes_consumed += sizeof(len) + sizeof(cmd) + sizeof(client_tag) + len;
    } while (0);

    return bytes_consumed;
}

static ssize_t
sim_stub_socket_pipe_parse_func(char *buf, char *buf_max, void *ud)
{
    const char                     *p = buf;
    sim_stub_socket_cmd_t           cmd;
    ssize_t                         bytes_consumed = 0;
    sim_stub_socket_thread_data_t  *thread_data = (sim_stub_socket_thread_data_t *) ud;

    if (!buf && !buf_max) 
    {
        return -1;
    }
    do 
    {
        // Decode the command.
        p = sim_dec(p, &cmd, sizeof(cmd), buf_max);
        if (!p)
        {
            break;
        }
        bytes_consumed += sizeof(cmd);

        if (cmd.type == SIM_STUB_SOCKET_MSG_STOP) 
        {
            printf("stub_socket : received pipe command to close pipe.\n");
            close(thread_data->pipe_get);
            thread_data->pipe_get = -1;
            thread_data->bar = cmd.vptr;
            free(thread_data->pipe_buf.buf);
            memset(&(thread_data->pipe_buf), 0, sizeof(sim_stub_socket_buf_t));
        }
    } while (0);

    return bytes_consumed;
}

static void sim_stub_socket_buf_new(sim_stub_socket_buf_t * sbuf_p, size_t bsize)
{
    if (sbuf_p) 
    {
        sbuf_p->buf = malloc(bsize);
        if (sbuf_p->buf) 
        {
            sbuf_p->buf_lo = sbuf_p->buf;
            sbuf_p->buf_hi = sbuf_p->buf;
            sbuf_p->buf_max = sbuf_p->buf;
            sbuf_p->buf_max += bsize;
        }
        else 
        {
            printf("stub_socket : memory allocation failed.\n");
        }
    }
}


static ssize_t sim_stub_socket_read(int fd, sim_stub_socket_buf_t * buf)
{
    size_t                  n_max;
    ssize_t                 rv;

    if (fd == -1 || !buf)
    {
        return -1;
    }

    /* If buffer is empty move pointer to start of buffer */
    if (buf->buf_hi == buf->buf_lo) 
    {
        buf->buf_lo = buf->buf;
        buf->buf_hi = buf->buf;
    }
    /* If there is unused space at start of buffer, move data */
    else if (buf->buf_lo > buf->buf) 
    {
        char                   *dst = buf->buf;
        char                   *src = buf->buf_lo;
        char                   *top = buf->buf_hi;

        while (src < top) 
        {
            *dst = *src;
            ++dst;
            ++src;
        }
        buf->buf_lo = buf->buf;
        buf->buf_hi = dst;
    }

    /* Max bytes to read */
    n_max = buf->buf_max - buf->buf_hi;

    if (n_max > 0) 
    {
        rv = read(fd, buf->buf_hi, n_max);
        if (rv > 0) 
        {  /* OK */
            buf->buf_hi += rv;
            rv = buf->buf_hi - buf->buf_lo;
        } 
        else if (rv == 0) 
        {  /* EOF */
            rv = buf->buf_hi - buf->buf_lo;
        } 
        else 
        {  /* Error */
            int    err = errno;

            switch (err) 
            {
                case EAGAIN:
                {
                    /* No error, just normal operation. */
                    rv = buf->buf_hi - buf->buf_lo;
                }
                break;
                case EINTR:
                {
                    /* Interrupted by a signal, no real error */
                    rv = buf->buf_hi - buf->buf_lo;
                }
                break;
                default:
                {
                    /* Real errors */
                    printf("stub_socket : socket read error, fatal\n");
                }
            }
        }
    }
    else 
    {
        // No of bytes in buffer.
        rv = buf->buf_hi - buf->buf_lo;
    }

    return rv;
}


static void sim_stub_socket_client_connect_handler(sim_stub_socket_thread_data_t  *thread_data)
{
    int                     client_fd;
    struct sockaddr_un      addr;
    socklen_t               addrlen = sizeof(struct sockaddr_un);
    int                     rv = 0;
    uint8_t                 i;

    printf("stub_socket : received client request to connect.\n");
    for (i = 0; i < SIM_STUB_MAX_NR_OF_CLIENTS; i++)
    {
        if (thread_data->client_socket[i].fd == -1)
        {
            break;
        }
    }
    if (i == SIM_STUB_MAX_NR_OF_CLIENTS)  //no space
    {
        printf("stub_socket : maximum number of clients reached.\n");
        return;
    }

    client_fd = accept(thread_data->input_socket, (struct sockaddr *) &addr, &addrlen);
    if (client_fd > 0) 
    {
        rv = fcntl(client_fd, F_SETFL, O_NONBLOCK);
        if (rv < 0) 
        {
            close(client_fd);
        } 
        else 
        {
            thread_data->client_socket[i].fd = client_fd;
            sim_stub_socket_buf_new(&(thread_data->client_socket[i].socket_buf), SIM_STUB_SOCKET_INPUT_BUFFER_SIZE);
        }
    }
}

static void sim_stub_socket_pipe_get_handler(sim_stub_socket_thread_data_t  *thread_data)
{
    ssize_t rv;
    sim_stub_socket_buf_t * buf = &(thread_data->pipe_buf);

    printf("stub_socket : received pipe command.\n");
    rv = sim_stub_socket_read(thread_data->pipe_get, buf);

    if (rv > 0) 
    {
        while (buf->buf_lo < buf->buf_hi) 
        {
            ssize_t consumed = sim_stub_socket_pipe_parse_func( buf->buf_lo, buf->buf_hi, thread_data );

            if (consumed > 0) 
            {
                buf->buf_lo += consumed;
                if (buf->buf_lo > buf->buf_hi) 
                {
                    buf->buf_lo = buf->buf_hi;
                }
            } 
            else if (consumed == 0) 
            {
                /* Normal parse failure, just need more data. */
                break;
            } 
            else if (consumed < 0) 
            {
                /* Error, irrecoverable parse error */
                break;
            }
        }
    }
}
static void sim_stub_socket_client_req_handler(sim_stub_socket_thread_data_t  *thread_data, uint8_t client_id)
{
    ssize_t rv;
    sim_stub_socket_buf_t * buf = &(thread_data->client_socket[client_id].socket_buf);

    printf("stub_socket : received client request. client_id = %d\n", client_id);
    rv = sim_stub_socket_read(thread_data->client_socket[client_id].fd, buf);
    printf("stub_socket : read from socket. rv = %d\n", (int)rv);

    if (rv > 0) 
    {
        while (buf->buf_lo < buf->buf_hi) 
        {
            ssize_t consumed = sim_stub_socket_socket_parse_func( buf->buf_lo, buf->buf_hi, &(thread_data->client_socket[client_id]) );

            if (consumed > 0) 
            {
                buf->buf_lo += consumed;
                if (buf->buf_lo > buf->buf_hi) 
                {
                    buf->buf_lo = buf->buf_hi;
                }
            } 
            else if (consumed == 0) 
            {
                /* Normal parse failure, just need more data. */
                break;
            } 
            else if (consumed < 0) 
            {
                /* Error, irrecoverable parse error */
                break;
            }
        }
    }
    else if (rv == 0)
    {
        printf("stub_socket : close client, id = %d\n", client_id);
        sim_stub_socket_close_client(&(thread_data->client_socket[client_id]));
    }
}

static int sim_stub_socket_set_fd(sim_stub_socket_thread_data_t  *thread_data, fd_set *rset_p)
{
    int      max_fd = 0;
    uint8_t  i;

    if (thread_data->input_socket != -1)
    {
        max_fd = thread_data->input_socket;
        FD_SET(thread_data->input_socket, rset_p);
    }
    if (thread_data->pipe_get != -1)
    {
        FD_SET(thread_data->pipe_get, rset_p);
        max_fd = (max_fd > thread_data->pipe_get ? max_fd : thread_data->pipe_get);
    }

    for (i = 0; i < SIM_STUB_MAX_NR_OF_CLIENTS; i++)
    {
        if (thread_data->client_socket[i].fd != -1)
        {
            FD_SET(thread_data->client_socket[i].fd, rset_p);
            if (thread_data->client_socket[i].fd > max_fd)
            {
                max_fd = thread_data->client_socket[i].fd;
            }
        }
    }

    return max_fd;
}

static void            *sim_stub_socket_thread_func(void *vdata)
{
    sim_stub_socket_thread_data_t  *thread_data = (sim_stub_socket_thread_data_t *) vdata;
    struct timeval                  tv;
    fd_set                          rset;
    int                             max_fd;
    int                             rv;
    uint8_t                         i;
    cat_barrier_t                  *bar;

    setThreadName("stub_socket");
    printf("stub_socket : New Thread 0x%08lx \n", (unsigned long)pthread_self());

    for (;;) 
    {
        FD_ZERO(&rset);
        max_fd = sim_stub_socket_set_fd(thread_data, &rset);

        tv.tv_sec = 60;
        tv.tv_usec = 0;
        rv = select(max_fd + 1, &rset, 0, 0, &tv);

        if (rv == 0) 
        {   /* timer expired */
            static int count = 5;

            if ( count && count-- ) 
            {
                printf("stub_socket : timer expired \n");   // suppress after so many iterations
            }
        } 
        else if (rv < 0) 
        {
            printf("stub_socket : Reader select failed! \n");

            if (errno == EBADF) 
            {
                printf("stub_socket : select failed EBADF\n");
                break;
            } 
            else if (errno == EINTR) 
            {
                printf("stub_socket : select failed EINTR\n");
            } 
            else if (errno == EINVAL) 
            {
                printf("stub_socket : select failed EINVAL\n");
                break;
            } 
            else if (errno == ENOMEM) 
            {
                printf("stub_socket : select failed ENOMEM\n");
                break;
            } 
            else 
            {
                printf("stub_socket : select failed UNKNOWN\n");
                break;
            }
        } 
        else 
        {
            if (FD_ISSET(thread_data->input_socket, &rset)) 
            {
                sim_stub_socket_client_connect_handler(thread_data);
            }
            if (FD_ISSET(thread_data->pipe_get, &rset)) 
            {
                sim_stub_socket_pipe_get_handler(thread_data);
            }
            for (i = 0; i < SIM_STUB_MAX_NR_OF_CLIENTS; i++)
            {
                if (thread_data->client_socket[i].fd != -1)
                {
                    if (FD_ISSET(thread_data->client_socket[i].fd, &rset))
                    {
                        sim_stub_socket_client_req_handler(thread_data, i);
                    }
                }
            }  //end for(i =...)
            if (thread_data->pipe_get < 0)
            {
                break;
            }
        }  //end if (rv == 0)
    }  //end for(;;)

    printf("stub_socket : disconnect client.\n");
    // Cleanup.
    for (i = 0; i < SIM_STUB_MAX_NR_OF_CLIENTS; i++)
    {
        sim_stub_socket_close_client(&(thread_data->client_socket[i]));
    }

    close(thread_data->input_socket);

    bar = thread_data->bar;
    free(thread_data);

    if (bar) 
    {
        cat_barrier_release(bar, 0);
    }

    return 0;
}

static void sim_stub_socket_kill_thread(sim_stub_socket_t * stub_socket_p)
{
    cat_barrier_t          *bar = 0;        /* Barrier to sync shutdown */
    sim_stub_socket_cmd_t   scmd = 
    {
      .type = SIM_STUB_SOCKET_MSG_STOP,
      .vptr = 0
    };

    printf("stub_socket : kill socket thread.\n");
    bar = cat_barrier_new();

    if (!bar)
    {
        abort();
    }
    cat_barrier_set(bar);

    scmd.vptr = bar;
    write(stub_socket_p->pipe_put, &scmd, sizeof(scmd));

    cat_barrier_wait(bar, 0);   /* Wait for thread to die */

    cat_barrier_delete(bar);
    bar = 0;
}



sim_stub_socket_t* sim_stub_socket_new()
{
    sim_stub_socket_t   *stub_socket_p = malloc(sizeof(sim_stub_socket_t));

    printf("stub_socket : new socket data.\n");
    if (stub_socket_p) 
    {
        stub_socket_p->pipe_put = -1;
    }
    return stub_socket_p;
}


void sim_stub_socket_delete(sim_stub_socket_t * stub_socket_p)
{
    printf("stub_socket : delete socket data.\n");
    if (stub_socket_p) 
    {
        sim_stub_socket_stop(stub_socket_p);
        free(stub_socket_p);
    }
}



static int sim_create_unix_socket(const char *path)
{
    int                     fd;
    struct sockaddr_un      addr;
    int                     i;

    printf("stub_socket : create socket.\n");
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    ste_sim_i_safe_copy(addr.sun_path, path, sizeof(addr.sun_path));


    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) 
    {
        printf("stub_socket : failed to create socket \n");
        return -1;
    }

    unlink(addr.sun_path);      /* Remove old socket */

    i = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (i < 0) 
    {
        printf("stub_socket : fail to bind socket \n");
        return -1;
    }

    i = listen(fd, 3);          /* Allow 3 pending connections */
    if (i < 0) 
    {
        printf("stub_socket : failed to listen \n");
        return -1;
    }

    i = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (i < 0) 
    {
        printf("stub_socket : failed to set non-blocking \n");
        return -1;
    }

    return fd;
}

int sim_stub_socket_start(sim_stub_socket_t * stub_socket_p)
{
    int                     rv = -1;

    printf("stub_socket : start socket thread.\n");
    do 
    {
        int                             pipe_put;
        int                             pipe_get;
        int                             input_socket;
        pthread_t                       thread_id;
        sim_stub_socket_thread_data_t * thread_data;
        uint8_t                         i;

        if (!stub_socket_p)
        {
            break;
        }
        if (stub_socket_p->pipe_put != -1)
        {
            break;
        }

        rv = sim_create_pipe(&pipe_put, &pipe_get);
        if (rv < 0) 
        {
            break;
        }

        input_socket = sim_create_unix_socket(socket_name);
        if ( input_socket < 0 ) 
        {
            rv = -1;
            break;
        }

        thread_data = malloc(sizeof(sim_stub_socket_thread_data_t));
        if (!thread_data) 
        {
            rv = -1;
            close(pipe_put);
            close(pipe_get);
            close(input_socket);
            break;
        }

        thread_data->pipe_get = pipe_get;
        thread_data->input_socket = input_socket;
        sim_stub_socket_buf_new(&(thread_data->pipe_buf), SIM_STUB_SOCKET_PIPE_BUFFER_SIZE);

        for (i = 0; i < SIM_STUB_MAX_NR_OF_CLIENTS; i++)
        {
            thread_data->client_socket[i].fd = -1;
        }

        rv = sim_launch_thread(&thread_id, PTHREAD_CREATE_DETACHED, sim_stub_socket_thread_func, thread_data);
        if (rv < 0) 
        {
            close(pipe_put);
            close(pipe_get);
            close(input_socket);
            free(thread_data);
            break;
        }

        stub_socket_p->pipe_put = pipe_put;

    } while (0);
    return rv;
}

int sim_stub_socket_stop(sim_stub_socket_t * stub_socket_p)
{
    int  rv = -1;

    printf("stub_socket : stop socket thread.\n");
    if (!stub_socket_p)
    {
        return rv;
    }
    if (stub_socket_p->pipe_put == -1)
    {
        return rv;
    }

    sim_stub_socket_kill_thread(stub_socket_p);

    close(stub_socket_p->pipe_put);
    stub_socket_p->pipe_put = -1;

    rv = 0;

    return rv;
}

void sim_stub_socket_terminal_response_done()
{
    printf("stub_socket : terminal response done.\n");
    if (sim_stub_pc_fd != -1)
    {
        sim_stub_socket_send_tr_done(sim_stub_pc_fd);
    }
    else
    {
        printf("stub_socket : no client is interested in terminal response.\n");
    }
    //reset the value
    sim_stub_pc_fd = -1;
}



