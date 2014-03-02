/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*----------------------------------------------------------------------------------*/
/* Name: sim_stub.c                                                                 */
/* SIM stub implementation file for SIM Library                                     */
/* version:         0.1                                                             */
/*----------------------------------------------------------------------------------*/


#include <stdio.h>
#include "sim.h"
#include "sim_util.h"
#include "sim_stub.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#ifdef SMS_SIM_TEST_PLATFORM
#include <signal.h>
#endif                          // SMS_SIM_TEST_PLATFORM

#define LOG_TAG "SIM Stub"

#define SIM_LOG_D(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] DBG:" format, ## __VA_ARGS__); fflush(stdout);})
#define SIM_LOG_E(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] ERR:" format, ## __VA_ARGS__); fflush(stdout);})
#define SIM_LOG_I(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] INF:" format, ## __VA_ARGS__); fflush(stdout);})
#define SIM_LOG_V(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] VBE:" format, ## __VA_ARGS__); fflush(stdout);})
#define SIM_LOG_W(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] WRG:" format, ## __VA_ARGS__); fflush(stdout);})

/*------------------------------------------------------------------------------
 * Private Variables
 *------------------------------------------------------------------------------
 */

static int sim_stub_fd = -1;
static int sim_fd = -1;

#ifdef SMS_SIM_TEST_PLATFORM
static int sim_stub_terminate = 0;
#endif                          // SMS_SIM_TEST_PLATFORM

/* These two are used for testing SMS control disallowed and with modification,
 * will be compared to destination phone number to decide which MO_control that
 * are tested.
 */
#define SMS_CONTROL_PREFIX_DISALLOWED (0x22)
#define SMS_CONTROL_PREFIX_ALLOWED_WITH_MOD (0x33)

/*------------------------------------------------------------------------------
 * Private function prototypes
 *------------------------------------------------------------------------------
 */
static int sim_stub_parse(
    const uint16_t,
    const uintptr_t const,
    const char *,
    const size_t);

static int sim_stub_waitforselect(
    void);

static int sim_stub_connectsocket(
    void);

static void sim_stub_closeSocket(
    void);

static int sim_stub(
    void);

/*------------------------------------------------------------------------------
 * Private function declarations
 *------------------------------------------------------------------------------
 */
static int sim_stub_parse(
    const uint16_t cmd,
    const uintptr_t const client_tag,
    const char *payload_p,
    const size_t payloadsize)
{
    int parse_status = 0;
    char *buf_p = NULL;
    size_t bufsize = 0;

    SIM_LOG_I("sim_stub_parse");

    switch (cmd) {
    case STE_SIM_REQ_CONNECT:
        SIM_LOG_I("sim_stub_parse: Received command: STE_SIM_REQ_CONNECT");
        buf_p = sim_util_encode_generic(STE_SIM_CAUSE_CONNECT, NULL,    /* No payload */
                                        0, client_tag, &bufsize);
        break;

    case STE_SIM_REQ_DISCONNECT:
        SIM_LOG_I("sim_stub_parse: Received command: STE_SIM_REQ_DISCONNECT");
        buf_p = sim_util_encode_generic(STE_SIM_CAUSE_DISCONNECT, NULL, /* No payload */
                                        0, client_tag, &bufsize);
        break;

    case STE_SIM_REQ_SHUTDOWN:
        SIM_LOG_I("sim_stub_parse: Received command: STE_SIM_REQ_SHUTDOWN");
        buf_p = sim_util_encode_generic(STE_SIM_CAUSE_SHUTDOWN, NULL,   /* No payload */
                                        0, client_tag, &bufsize);
        break;

    case STE_SIM_REQ_PING:
        SIM_LOG_I("sim_stub_parse: Received command: STE_SIM_REQ_PING");
        buf_p = sim_util_encode_generic(STE_SIM_CAUSE_PING, NULL,       /* No payload */
                                        0, client_tag, &bufsize);
        break;

    case STE_SIM_REQ_HANGUP:
        SIM_LOG_I("sim_stub_parse: Received command: STE_SIM_REQ_HANGUP");
        buf_p = sim_util_encode_generic(STE_SIM_CAUSE_HANGUP, NULL,     /* No payload */
                                        0, client_tag, &bufsize);
        break;

    case STE_SIM_REQ_STARTUP:
        SIM_LOG_I("sim_stub_parse: Received command: STE_SIM_REQ_STARTUP");
        buf_p = sim_util_encode_generic(STE_SIM_CAUSE_STARTUP, NULL,    /* No payload */
                                        0, client_tag, &bufsize);
        break;

    case STE_UICC_REQ_GET_SIM_STATE:
        {
            char SIM_State_Payload[] = { 0x05, 0x00, 0x00, 0x00,        // APDU Length
                0x69, 0x64, 0x6C, 0x65, 0x00
            };                  // APDU Data

            SIM_LOG_I("sim_stub_parse: Received command: STE_UICC_REQ_GET_SIM_STATE");
            buf_p = sim_util_encode_generic(STE_UICC_CAUSE_SIM_STATE_CHANGED, SIM_State_Payload, sizeof(SIM_State_Payload), client_tag, &bufsize);
        }
        break;

    case STE_CAT_REQ_EC:
        {
            printf("sim_stub_parse: Received command: STE_CAT_REQ_EC\n");

            /* MO_SMS_CONTROL */
            if (((uint8_t)payload_p[0]) == 0xd5) {
                ste_cat_cc_dialled_address_t* dest_p = NULL;

                /* Save destination adress */
                dest_p = calloc(1,sizeof(ste_cat_cc_dialled_address_t));
                if (!dest_p) {
                    printf("%s: calloc failed for dest_p", __FUNCTION__);
                    goto Error;
                }

                dest_p->dialled_string_p = calloc(1, sizeof(ste_sim_text_t));
                if (!dest_p->dialled_string_p) {
                    printf("%s: calloc failed for dest_p->dialled_string_p", __FUNCTION__);
                    goto Error;
                }

                payload_p = payload_p + 15; //Destination Adress Tag is here
                dest_p->dialled_string_p->text_coding = STE_SIM_BCD;
                dest_p->dialled_string_p->no_of_characters = *(payload_p+1);
                dest_p->dialled_string_p->text_p = (void*)payload_p;

                if (((unsigned char*)dest_p->dialled_string_p->text_p)[3] == SMS_CONTROL_PREFIX_DISALLOWED) { //SMS control disallowed
                    char EC_SMSC_Payload[] = { 0x02, 0x00, 0x00, 0x00,       // Status Length
                        0x01, 0x01,     // Status Word
                        0x03, 0x00, 0x00, 0x00, // APDU Length
                        0x01, 0x90, 0x00
                    };
                    buf_p = sim_util_encode_generic(STE_CAT_CAUSE_EC, EC_SMSC_Payload, sizeof(EC_SMSC_Payload), client_tag, &bufsize);
                } else if(((unsigned char*)dest_p->dialled_string_p->text_p)[3] == SMS_CONTROL_PREFIX_ALLOWED_WITH_MOD) {
                    char EC_SMSC_Payload[] = { 0x02, 0x00, 0x00, 0x00,       // Status Length
                        0x01, 0x01,     // Status Word
                        0x16, 0x00, 0x00, 0x00, // APDU Length
                        0x02, 0x12, 0x86, 0x06, 0x91, 0x44, 0x52, 0x78, 0x63, 0xF5, 0x86, 0x08, 0x91, 0x22, 0x22, 0x99, 0x03, 0x10, 0x57, 0xF9, 0x90, 0x00
                    };
                    buf_p = sim_util_encode_generic(STE_CAT_CAUSE_EC, EC_SMSC_Payload, sizeof(EC_SMSC_Payload), client_tag, &bufsize);

                } else {
                    /* MO SMS CONTROL Allowed */
                    char EC_SMSC_Payload[] = { 0x02, 0x00, 0x00, 0x00,       // Status Length
                        0x01, 0x01,     // Status Word
                        0x02, 0x00, 0x00, 0x00, // APDU Length
                        0x90, 0x00
                    };
                    buf_p = sim_util_encode_generic(STE_CAT_CAUSE_EC, EC_SMSC_Payload, sizeof(EC_SMSC_Payload), client_tag, &bufsize);
                }

Error:
                /* Free and cleanup memory allocations */
                if (dest_p->dialled_string_p) {
                    free(dest_p->dialled_string_p);
                    dest_p->dialled_string_p = NULL;
                }
                if (dest_p->dialled_string_p) {
                    free(dest_p->dialled_string_p);
                    dest_p->dialled_string_p = NULL;
                }

            /* Not MO_SMS_CONTROL */
            } else {
                printf("sim_stub_parse: Received: STE_CAT_REQ_EC, Not MO_SMS_CONTROL \n");

                char EC_Payload[] = { 0x02, 0x00, 0x00, 0x00,       // Status Length
                    0x01, 0x01,     // Status Word
                    0x10, 0x00, 0x00, 0x00, // APDU Length
                    0x53, 0x54, 0x45, 0x5F, 0x43, 0x41, 0x54, 0x5F, 0x43, 0x41, 0x55, 0x53, 0x45, 0x5F, 0x45, 0x43
                };                  // APDU Data

                buf_p = sim_util_encode_generic(STE_CAT_CAUSE_EC, EC_Payload, sizeof(EC_Payload), client_tag, &bufsize);
            }
        }
        break;

    default:
        SIM_LOG_E("sim_stub_parse: Error command %d not parsed", cmd);
        parse_status = -1;
        break;
    }

    if (buf_p != NULL && parse_status == 0) {   // send to SIM client
        parse_status = sim_util_writesocket(sim_fd, SIM_SOCKET_PATH, buf_p, bufsize);
        free(buf_p);
    } else {
        SIM_LOG_E("sim_stub_parse: Error parsing command: %d", cmd);
    }

    return (parse_status);
}

static int sim_stub_waitforselect(
    void)
{
    fd_set rset;
    int fd;
    int status = -1;

    if (sim_stub_fd != -1) {
        fd = sim_stub_fd;

        for (;;) {
            FD_ZERO(&rset);
            FD_SET(fd, &rset);

            SIM_LOG_I("sim_stub_waitforselect");

            status = select(fd + 1, &rset, 0, 0, 0);
            if (0 > status) {  // Error
                SIM_LOG_E("select() errno %d: %s", errno, strerror(errno));
                break;
            } else if (0 < status) {   // OK, there are bytes to read.
                status = 0;
                break;
            }
            // If Status == 0, we try again.
        }
    }

    return (status);
}

static int sim_stub_connectsocket(
    void)
{
    struct sockaddr_un addr;
    int fd = -1;

    SIM_LOG_I("sim_stub_connectsocket");

    if (sim_stub_fd < 0) {
        /*Setting up connection sim -> stub */
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        addr.sun_path[0] = 0;   // abstract namespace socket
        strncpy(&addr.sun_path[1], SIMSTUB_SOCKET_PATH, sizeof(addr.sun_path) - 1);
        SIM_LOG_I("sim_stub_connectsocket: %s", &addr.sun_path[1]);

        fd = socket(PF_UNIX, SOCK_DGRAM, 0);

        if (fd < 0) {
            SIM_LOG_E("sim_stub_connectsocket - Failed to create socket");
            goto error_socket;
        }

        /* Bind the socket */
        unlink(&addr.sun_path[1]);

        if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            SIM_LOG_E("sim_stub_connectsocket - bind() returns error(%d)=\"%s\"", errno, strerror(errno));
            goto error_socket;
        }

        sim_stub_fd = fd;
    }

    if (sim_fd < 0) {
        /* Setting up connection stub -> sim */
        fd = socket(PF_UNIX, SOCK_DGRAM, 0);

        if (fd < 0) {
            SIM_LOG_E("sim_stub_connectsocket - Failed to create socket");
            goto error_socket;
        }

        sim_fd = fd;
    }

    return (0);

  error_socket:
    if (sim_stub_fd > -1) {
        close(sim_stub_fd);
    }

    if (sim_fd > -1) {
        close(sim_fd);
    }

    return (-1);
}

static void sim_stub_closeSocket(
    void)
{
    SIM_LOG_I("sim_stub_closeSocket");

    if (sim_stub_fd > -1) {
        close(sim_stub_fd);
    }

    if (sim_fd > -1) {
        close(sim_fd);
    }

    sim_stub_fd = -1;
    sim_fd = -1;

    return;
}

int sim_stub(
    void)
{
    int is_select = -1;
    char *buf_p = NULL;
    ssize_t bufsize = 0;

    SIM_LOG_I("sim_stub");

    while (sim_stub_terminate != 1) {

        is_select = sim_stub_waitforselect();
        if (is_select) {
            // Something failed
            is_select = -1;
            break;
        } else {
            buf_p = sim_util_readsocket(sim_stub_fd, SIM_READER_INPUT_BUFFER_SIZE, &bufsize);

            if (buf_p != NULL && bufsize > 0) {
                if (bufsize > 0) {
                    ssize_t i;
                    uint16_t len = 0;
                    uint16_t cmd = 0;
                    uintptr_t client_tag = 0;
                    size_t payloadsize = 0;
                    char *payload_p = NULL;
                    char *data_dump_p = malloc(10 + 3 * bufsize);
                    size_t data_pos = 0;

                    SIM_LOG_I("sim_stub: Received Data from SIM: Length = %d", bufsize);

                    for (i = 0; i < bufsize; i++) {
                        data_pos += sprintf(data_dump_p + data_pos, " %02X", (uint8_t)buf_p[i]);
                    }
                    SIM_LOG_D("\tData =%s", data_dump_p);
                    free(data_dump_p);

                    payload_p = sim_util_decode_generic(buf_p, bufsize, &len, &cmd, &client_tag, &payloadsize);
                    free(buf_p);

                    if (payload_p) {
                        ssize_t i;

                        SIM_LOG_D("Decoded Data = ");
                        SIM_LOG_D("\tlength = %d", len);
                        SIM_LOG_D("\tcommand = %d", cmd);
                        SIM_LOG_D("\tclient tag = %d", client_tag);
                        SIM_LOG_D("\tpayload size = %d", payloadsize);

                        if (payloadsize > 0) {
                            char *payload_dump_p = malloc(16 + 3 * payloadsize);
                            size_t payload_pos = 0;

                            for (i = 0; i < payloadsize; i++) {
                                payload_pos += sprintf(payload_dump_p + payload_pos, " %02X", (uint8_t)payload_p[i]);
                            }
                            SIM_LOG_D("\tpayload =%s", payload_dump_p);
                            free(payload_dump_p);
                        } else {
                            SIM_LOG_D("\tpayload is EMPTY");
                        }
                    }

                    sim_stub_parse(cmd, client_tag, payload_p, payloadsize);
                    free(payload_p);
                }
            }
        }
    }

    return (0);
}

#ifdef SMS_SIM_TEST_PLATFORM
void sim_stub_signal_handler(int signo) {
    SIM_LOG_I("sim_stub_signal_handler");
    if (SIGTERM == signo) {
        sim_stub_terminate = 1;
        sim_stub_closeSocket();
    }
}
#endif                          // SMS_SIM_TEST_PLATFORM

/*------------------------------------------------------------------------------
 * Test framework Entry Point
 *------------------------------------------------------------------------------
 */
int main()
{
    SIM_LOG_I("*** SIM STUB STARTED! ***");

#ifdef SMS_SIM_TEST_PLATFORM
    signal(SIGTERM, sim_stub_signal_handler);
#endif                          // SMS_SIM_TEST_PLATFORM

    if (sim_stub_connectsocket() < 0) {
        SIM_LOG_E("sim_stub: MAIN - ERROR Connecting SIM STUB");

        sim_stub_closeSocket();

        return (0);
    }

    sim_stub();
    sim_stub_closeSocket();

    return (0);
}
