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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

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

/**
 * @brief Internal to external state mapping structure.
 */
typedef struct {
    const char *state_name;                           /**< Name of the internal state */
    sim_state_t sim_state;                            /**< External state delivered to client */
} sim_external_state_t;

/**
 * @brief State table to deliver state events to the clients.
 */
static const sim_external_state_t uicc_external_state_table[] = {
    {"start", SIM_STATE_UNKNOWN},
    {"init", SIM_STATE_NOT_READY},
    {"pin_lock", SIM_STATE_PIN_NEEDED},
    {"puk_lock", SIM_STATE_PUK_NEEDED},
    {"idle_pin1", SIM_STATE_PIN_NEEDED},
    {"idle_pin2", SIM_STATE_PIN2_NEEDED},
    {"idle_puk1", SIM_STATE_PUK_NEEDED},
    {"idle_puk2", SIM_STATE_PUK2_NEEDED},
    {"idle", SIM_STATE_READY},
    {"no_card", SIM_STATE_SIM_ABSENT},
    {"closed", SIM_STATE_PERMANENTLY_BLOCKED}
};

#define MAX_ENTRIES_IN_STATE_MAPPING_TABLE (sizeof(uicc_external_state_table) / sizeof(sim_external_state_t))

#define EF_CBMID     (0x6F48)
#define EF_SMS       (0x6F3C)
#define EF_SMSR      (0x6F47)
#define EF_SMSS      (0x6F43)

typedef struct {
    const char *ef_file_id_str;
    int ef_file_id;
} sim_ef_file_id_t;

static const sim_ef_file_id_t sim_ef_file_id_table[] = {
    {"EF_CBMID.bin", EF_CBMID},
    {"EF_SMS.bin", EF_SMS},
    {"EF_SMSR.bin", EF_SMSR},
    {"EF_SMSS.bin", EF_SMSS}
};

static const uint8_t sim_default_smsc[] = {
    0x30, 0x37, 0x37, 0x37, 0x30, 0x32, 0x31, 0x31, 0x33, 0x38, 0x34
};

#define MAX_ENTRIES_IN_EF_FILE_ID_TABLE (sizeof(sim_ef_file_id_table) / sizeof(sim_ef_file_id_t))

/*
 * Use this command to generate printable strings from the macros in sim.h
 * awk '{ a=$2;sub("^STE_","",a); gsub("CAUSE_","",a); gsub("_"," ",a); print "  \"" a "\"," }'
 */
const char *ste_sim_cause_name[STE_UICC_CAUSE__MAX] = {
    "SIM NOOP",
    "SIM CONNECT",
    "SIM DISCONNECT",
    "SIM SHUTDOWN",
    "SIM PING",
    "SIM HANGUP",
    "SIM STARTUP",
    "CAT NOOP",
    "CAT REGISTER",
    "CAT PC",
    "CAT SIM EC CALLCONTROL",
    "CAT IND STATUS",
    "CAT EC",
    "CAT TR",
    "CAT SET TERMINAL PROFILE",
    "CAT GET TERMINAL PROFILE",
    "CAT GET CAT STATUS",
    "CAT PC NOTIFICATION",
    "CAT DEREGISTER",
    "CAT SETUP CALL IND",
    "CAT ANSWER CALL",
    "CAT EVENT DOWNLOAD",
    "CAT SMS CONTROL",
    "CAT SETUP CALL RESULT",
    "UICC NOOP",
    "UICC REGISTER",
    "UICC REQ PIN VERIFY",
    "UICC REQ PIN NEEDED",
    "UICC REQ READ SIM FILE RECORD",
    "UICC REQ UPDATE SIM FILE RECORD",
    "UICC REQ PIN CHANGE",
    "UICC REQ READ SIM FILE BINARY",
    "UICC REQ UPDATE SIM FILE BINARY",
    "UICC REQ SIM FILE GET FORMAT",
    "UICC REQ GET FILE INFORMATION",
    "UICC REQ GET SIM STATE",
    "UICC SIM STATE CHANGED",
    "UICC REQ PIN DISABLE",
    "UICC REQ PIN ENABLE",
    "UICC REQ PIN INFO",
    "UICC REQ PIN UNBLOCK",
    "UICC NOT READY",
    "UICC REQ APP INFO",
    "UICC REQ READ SMSC",
    "UICC REQ UPDATE SMSC",
    "UICC REQ READ GENERIC",
    "UICC REQ READ IMSI",
    "UICC SIM STATE",
    "UICC SIM ICON READ",
    "UICC SIM SUBSCRIBER_NUMBER"
};

#define SIM_EF_FILE_MOUNT_POINT "./test_harness/stubs/sim/include/"

/*------------------------------------------------------------------------------
 * Private function prototypes
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * Test Framework Implementation - function prototypes
 */
static int sim_get_ef_filesize(
    FILE *);

static int sim_connectsocket(
    int *);

static void sim_closesocket(
    void);

static int sim_parse(
    ste_sim_t *,
    const uint16_t,
    const uintptr_t const,
    const char *,
    const size_t);

static void *sim_find_ef_file_id(
    const int);

/*------------------------------------------------------------------------------
 * SIM Library - function prototypes
 */
static int findstatefromstatetable(
    const char *internal_state_name);

/*------------------------------------------------------------------------------
 * Private function declarations
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * Test Framework Implementation - function declarations
 */
static int sim_get_ef_filesize(
    FILE * file_p)
{
    int filesize = 0;

    SIM_LOG_I("sim_get_ef_filesize");

    if (file_p != NULL) {
        fseek(file_p, 0, SEEK_END);
        filesize = ftell(file_p);
        fseek(file_p, 0, SEEK_SET);
    }

    return (filesize);
}

static int sim_connectsocket(
    int *fd_p)
{
    struct sockaddr_un addr;

    SIM_LOG_I("sim_connectsocket");

    if (sim_stub_fd < 0) {
        sim_stub_fd = socket(PF_UNIX, SOCK_DGRAM, 0);

        if (sim_stub_fd < 0) {
            SIM_LOG_E("sim_connectsocket: Failed to create socket");
            goto error_socket;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        addr.sun_path[0] = 0;   // abstract namespace socket
        sprintf(&addr.sun_path[1], "%s", SIM_SOCKET_PATH);

        SIM_LOG_I("sim_connectsocket: SIM Socket Path = %s", &addr.sun_path[1]);

        *fd_p = socket(PF_UNIX, SOCK_DGRAM, 0);

        if (*fd_p < 0) {
            SIM_LOG_E("sim_connectsocket: Failed to create socket");
            goto error_socket;
        }

        /* bind the socket */
        unlink(&addr.sun_path[1]);

        if (bind(*fd_p, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            SIM_LOG_E("sim_connectsocket: bind() returns error(%d)=\"%s\"", errno, strerror(errno));
            goto error_socket;
        }
    }

    return (0);

  error_socket:
    if (sim_stub_fd > -1) {
        close(sim_stub_fd);
    }

    if (*fd_p > -1) {
        close(*fd_p);
    }

    return (-1);
}

static void sim_closesocket(
    void)
{
    SIM_LOG_I("sim_closesocket");

    if (sim_stub_fd > -1) {
        SIM_LOG_I("sim_closesocket: Closing sockets");
        close(sim_stub_fd);
    }

    sim_stub_fd = -1;

    return;
}

static int sim_parse(
    ste_sim_t * sim_p,
    const uint16_t cmd,
    const uintptr_t const client_tag,
    const char *payload_p,
    const size_t payloadsize)
{
    int status = 0;

    SIM_LOG_I("sim_parse");

    switch (cmd) {
    case STE_SIM_CAUSE_CONNECT:
        SIM_LOG_I("sim_parse: Parsing STE_SIM_CAUSE_CONNECT command");
        sim_p->closure.func(STE_SIM_CAUSE_CONNECT,      /* Cause */
                            client_tag, 0,      /* No payload */
                            sim_p);     //sim_p->closure.user_data);
        break;

    case STE_SIM_CAUSE_DISCONNECT:
        SIM_LOG_I("sim_parse: Parsing STE_SIM_CAUSE_DISCONNECT command");
        sim_p->closure.func(STE_SIM_CAUSE_DISCONNECT,   /* Cause */
                            client_tag, 0,      /* No payload */
                            sim_p);     // sim_p->closure.user_data);
        break;

    case STE_SIM_CAUSE_SHUTDOWN:
        SIM_LOG_I("sim_parse: Parsing STE_SIM_CAUSE_SHUTDOWN command");
        sim_p->closure.func(STE_SIM_CAUSE_SHUTDOWN,     /* Cause */
                            client_tag, 0,      /* No payload */
                            sim_p);     // sim_p->closure.user_data);
        break;

    case STE_SIM_CAUSE_PING:
        SIM_LOG_I("sim_parse: Parsing STE_SIM_CAUSE_PING command");
        sim_p->closure.func(STE_SIM_CAUSE_PING, /* Cause */
                            client_tag, 0,      /* No payload */
                            sim_p);     // sim_p->closure.user_data);
        break;

    case STE_SIM_CAUSE_HANGUP:
        SIM_LOG_I("sim_parse: Parsing STE_SIM_CAUSE_HANGUP command");
        sim_p->closure.func(STE_SIM_CAUSE_HANGUP,       /* Cause */
                            0,  /* No client tag */
                            0,  /* No payload */
                            sim_p);     //sim_p->closure.user_data);
        break;

    case STE_SIM_CAUSE_STARTUP:
        SIM_LOG_I("sim_parse: Parsing STE_SIM_CAUSE_STARTUP command");
        sim_p->closure.func(STE_SIM_CAUSE_STARTUP,      /* Cause */
                            client_tag, 0,      /* No payload */
                            sim_p);     //sim_p->closure.user_data);
        break;

    case STE_UICC_CAUSE_SIM_STATE_CHANGED:
        {
            ste_uicc_sim_state_changed_t simstatechanged;
            const char *payloadcur_p = payload_p;
            const char *payloadmax_p = payload_p + payloadsize;
            char *statename_str_p = NULL;
            size_t datalength = 0;

            SIM_LOG_I("sim_parse: Parsing STE_UICC_CAUSE_SIM_STATE_CHANGED command");

            payloadcur_p = sim_util_dec(payloadcur_p, &datalength, sizeof(datalength), payloadmax_p);
            SIM_LOG_I("sim_parse: STE_UICC_CAUSE_SIM_STATE_CHANGED - datalength = %x", datalength);

            if (datalength > 0) {
                statename_str_p = malloc(datalength);
                if (statename_str_p == NULL) {
                    status = -1;
                } else {
                    memset(statename_str_p, 0, datalength);
                    payloadcur_p = sim_util_dec(payloadcur_p, statename_str_p, datalength, payloadmax_p);
                    SIM_LOG_I("sim_parse: STE_UICC_CAUSE_SIM_STATE_CHANGED - statename_str_p = %s", statename_str_p);

                    simstatechanged.state = (sim_state_t) findstatefromstatetable(statename_str_p);
                    free(statename_str_p);

                    sim_p->closure.func(STE_UICC_CAUSE_SIM_STATE_CHANGED,       /* Cause */
                                        client_tag, &simstatechanged, sim_p);   // sim_p->closure.user_data);
                }
            } else {
                sim_p->closure.func(STE_UICC_CAUSE_SIM_STATE_CHANGED,   /* Cause */
                                    client_tag, 0,      /* No payload */
                                    sim_p);     // sim_p->closure.user_data);
            }

            SIM_LOG_I("sim_parse: STE_UICC_CAUSE_SIM_STATE_CHANGED - simstatechanged.state = %d", simstatechanged.state);
        }
        break;

    case STE_CAT_CAUSE_EC:
        {
            ste_cat_ec_response_t ec_response;
            const char *payloadcur_p = payload_p;
            const char *payloadmax_p = payload_p + payloadsize;
            unsigned statuslength = 0;
            unsigned APDUlength = 0;

            SIM_LOG_I("sim_parse: Parsing STE_CAT_CAUSE_EC command");

            payloadcur_p = sim_util_dec(payloadcur_p, &statuslength, sizeof(statuslength), payloadmax_p);
            SIM_LOG_I("sim_parse: STE_CAT_CAUSE_EC - statuslength = %x", statuslength);

            payloadcur_p = sim_util_dec(payloadcur_p, &(ec_response.ec_status), statuslength, payloadmax_p);
            SIM_LOG_I("sim_parse: STE_CAT_CAUSE_EC - ec_response.ec_status = %x", ec_response.ec_status);

            payloadcur_p = sim_util_dec(payloadcur_p, &APDUlength, sizeof(APDUlength), payloadmax_p);
            SIM_LOG_I("sim_parse: STE_CAT_CAUSE_EC - APDUlength = %x", APDUlength);

            if (APDUlength > 0) {
                ec_response.apdu.buf = malloc(APDUlength + 1);
                if (ec_response.apdu.buf == NULL) {
                    status = -1;
                } else {
                    memset(ec_response.apdu.buf, 0, APDUlength + 1);
                    payloadcur_p = sim_util_dec(payloadcur_p, ec_response.apdu.buf, APDUlength, payloadmax_p);

                    {
                        char *data_dump_p = malloc(3 * APDUlength + 1);
                        size_t data_pos = 0;
                        int i;
                        for (i = 0; i < APDUlength; i++) {
                            data_pos += sprintf(data_dump_p + data_pos, " %02x", (uint8_t)ec_response.apdu.buf[i]);
                        }

                        SIM_LOG_I("sim_parse: STE_CAT_CAUSE_EC - APDUlength =%s", data_dump_p);
                        free(data_dump_p);
                    }

                    ec_response.apdu.len = APDUlength;
                    sim_p->closure.func(STE_CAT_CAUSE_EC,       /* Cause */
                                        client_tag, &ec_response, sim_p);       // ->closure.user_data);
                    free(ec_response.apdu.buf);
                }
            } else {
                sim_p->closure.func(STE_CAT_CAUSE_EC,   /* Cause */
                                    client_tag, 0,      /* No payload */
                                    sim_p);     // sim_p->closure.user_data);
            }
        }
        break;

    default:
        SIM_LOG_E("sim_parse: Error command %d not found", cmd);
        status = -1;
        break;
    }

    if (status == -1) {
        SIM_LOG_E("sim_parse: Error command %d not parsed", cmd);
    } else {
        SIM_LOG_I("sim_parse: Command %d parsed successfully", cmd);
    }

    return (status);
}

static void *sim_find_ef_file_id(
    const int ef_file_id)
{
    int count;
    int filelength = 0;
    char *ef_file_id_p = NULL;

    SIM_LOG_I("sim_find_ef_file_id");

    for (count = 0; count < MAX_ENTRIES_IN_EF_FILE_ID_TABLE; count++) {
        if (ef_file_id == sim_ef_file_id_table[count].ef_file_id) {

            filelength = strlen(SIM_EF_FILE_MOUNT_POINT) + strlen(sim_ef_file_id_table[count].ef_file_id_str) + 1;
            ef_file_id_p = malloc(filelength);

            if (ef_file_id_p != NULL) {
                strcpy(ef_file_id_p, SIM_EF_FILE_MOUNT_POINT);
                strcat(ef_file_id_p, sim_ef_file_id_table[count].ef_file_id_str);

                SIM_LOG_I("sim_find_ef_file_id : ef_file_id_p = %s", ef_file_id_p);
            }
            break;
        }
    }

    return (ef_file_id_p);
}

/*------------------------------------------------------------------------------
 * SIM Library - function declarations
 */
static int findstatefromstatetable(
    const char *internal_state_name)
{
    uint32_t Count = 0;

    SIM_LOG_I("findstatefromstatetable");

    for (Count = 0; Count < MAX_ENTRIES_IN_STATE_MAPPING_TABLE; Count++) {
        if (strcmp(internal_state_name, uicc_external_state_table[Count].state_name) == 0) {
            return uicc_external_state_table[Count].sim_state;
        }
    }

    return (0);
}

/*------------------------------------------------------------------------------
 * Public function declarations
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * Test Framework Implementation - function declarations
 */

/*------------------------------------------------------------------------------
 * SIM Library - function declarations
 */
ste_sim_t *ste_sim_new_st(
    const ste_sim_closure_t * closure)
{
    ste_sim_t *ste_sim_p = malloc(sizeof(ste_sim_t));

    SIM_LOG_I("ste_sim_new_st");

    if (ste_sim_p) {
        ste_sim_p->state = ste_sim_state_disconnected;
        ste_sim_p->closure = *closure;
        ste_sim_p->thread_pipe = -1;
        ste_sim_p->fd = -1;
        ste_sim_p->rdr_tid = -1;
        ste_sim_p->is_st = 1;
        ste_sim_p->es = NULL;
    }

    return (ste_sim_p);
}

int ste_sim_connect(
    ste_sim_t * sim,
    uintptr_t client_tag)
{
    int fd = -1;
    ste_es_t *es = 0;
    int rv = STE_SIM_ERROR_UNKNOWN;

    SIM_LOG_I("ste_sim_connect");

    if (!sim) {
        rv = STE_SIM_ERROR_BAD_HANDLE;
    } else {
        if (sim_connectsocket(&fd) == 0) {
            sim->fd = fd;
            sim->es = es;
            sim->state = ste_sim_state_connected;

            rv = STE_SIM_SUCCESS;

            /* Call the call back to indicate the connection is up and running */
            sim->closure.func(STE_SIM_CAUSE_CONNECT,    /* Cause */
                              client_tag, 0,    /* No payload */
                              sim->closure.user_data);
        } else {
            rv = STE_SIM_ERROR_BAD_HANDLE;
        }
    }

    return (rv);
}

int ste_sim_fd(
    const ste_sim_t * sim)
{
    int fd = -1;

    SIM_LOG_I("ste_sim_fd");

    // Only return the fd if we are in single threaded mode.
    if (sim && sim->is_st) {
        fd = sim->fd;
    }

    return (fd);
}

int ste_sim_disconnect(
    ste_sim_t * sim,
    uintptr_t client_tag)
{
    int rv = STE_SIM_ERROR_UNKNOWN;

    SIM_LOG_I("ste_sim_disconnect");

    do {
        if (!sim) {
            rv = STE_SIM_ERROR_BAD_HANDLE;
            break;
        }
        if (sim->state != ste_sim_state_connected) {
            rv = STE_SIM_ERROR_NOT_CONNECTED;
            break;
        }

        if (sim->fd != -1) {
            close(sim->fd);
            sim->fd = -1;
        }
        sim->state = ste_sim_state_disconnected;

        rv = STE_SIM_SUCCESS;

        /* Call the call back to indicate the connection is up and running */
        sim->closure.func(STE_SIM_CAUSE_DISCONNECT,     /* Cause */
                          client_tag, 0,        /* No payload */
                          sim->closure.user_data);
    } while (0);

    return (rv);
}

void ste_sim_delete(
    ste_sim_t * sim,
    uintptr_t client_tag)
{
    SIM_LOG_I("ste_sim_delete");

    if (sim) {
        sim->es = 0;

        if (sim->state != ste_sim_state_disconnected) {
            int rv = ste_sim_disconnect((ste_sim_t *) sim, client_tag);

            sim_closesocket();
            if (rv) {
                SIM_LOG_E("ste_sim_delete: disconnect fail");
            }
        }

        free(sim);
    }
}

int ste_uicc_sim_get_state_sync(
    ste_sim_t * sim,
    uintptr_t client_tag,
    ste_uicc_get_sim_state_response_t * sim_state)
{
    int rv = STE_SIM_ERROR_BAD_HANDLE;

    SIM_LOG_I("ste_uicc_sim_get_state_sync");

    if (sim_state != NULL) {
        sim_state->state = (sim_state_t) findstatefromstatetable("idle");

        rv = STE_SIM_SUCCESS;
    }

    return (rv);
}

int ste_cat_envelope_command(
    ste_sim_t * cat,
    uintptr_t client_tag,
    const char *buf,
    unsigned len)
{
    char *buf_p = NULL;
    size_t bufsize = 0;
    int nobytes = 0;
    int rv = -1;

    SIM_LOG_I("ste_cat_envelope_command");

    if (cat && cat->state == ste_sim_state_connected) {
        buf_p = sim_util_encode_generic(STE_CAT_REQ_EC, buf, len, client_tag, &bufsize);

        if (buf_p != NULL) {
            nobytes = sim_util_writesocket(sim_stub_fd, SIMSTUB_SOCKET_PATH, buf_p, bufsize);
            if (nobytes <= 0) {
                rv = ste_sim_disconnect((ste_sim_t *) cat, client_tag);
            } else {
                rv = 0;
            }

            free(buf_p);
        }
    }

    return (rv);
}

int ste_cat_register(
    ste_sim_t * cat,
    uintptr_t client_tag,
    uint32_t reg_events)
{
    int rv = 0;

    SIM_LOG_I("ste_cat_register");

    return (rv);
}

int ste_cat_deregister(
    ste_sim_t * cat,
    uintptr_t client_tag)
{
    int rv = 0;

    SIM_LOG_I("ste_cat_deregister");

    return (rv);
}

int ste_sim_read(
    ste_sim_t * sim)
{
    int read_status = 0;
    int bufsize = 0;
    char *buf_p = NULL;

    SIM_LOG_I("ste_sim_read");

    if (!sim || !(sim->is_st))
        return (-1);

    buf_p = sim_util_readsocket(sim->fd, SIM_READER_INPUT_BUFFER_SIZE, &bufsize);

    if (buf_p != NULL) {
        uint16_t len = 0;
        uint16_t cmd = 0;
        uintptr_t client_tag = 0;
        size_t payloadsize = 0;
        char *payload_p = NULL;

        payload_p = sim_util_decode_generic(buf_p, bufsize, &len, &cmd, &client_tag, &payloadsize);
        free(buf_p);

        SIM_LOG_I("sim data:");
        SIM_LOG_I("\tlength = %d", len);
        SIM_LOG_I("\tcommand = %d", cmd);
        SIM_LOG_I("\tclient_tag = %d", client_tag);
        SIM_LOG_I("\tPayload Size = %d", payloadsize);

        if (payloadsize > 0) {
            char *data_dump_p = malloc(3 * payloadsize + 1);
            size_t data_pos = 0;
            int i;
            for (i = 0; i < payloadsize; i++) {
                data_pos += sprintf(data_dump_p + data_pos, " %02x", (uint8_t)payload_p[i]);
            }
            SIM_LOG_I("payload data =%s", data_dump_p);
            free(data_dump_p);
        }

        read_status = sim_parse(sim, cmd, client_tag, payload_p, payloadsize);
        free(payload_p);
    } else {
        return (-1);
    }

    return (read_status);
}

int ste_uicc_sim_file_get_format_sync(
    ste_sim_t * sim,
    uintptr_t client_tag,
    int file_id,
    const char *file_path,
    ste_uicc_sim_file_get_format_response_t * file_format)
{
    int rv = 0;
    FILE *file_p = NULL;
    char *filename_p = NULL;

    SIM_LOG_I("ste_uicc_sim_file_get_format_sync");

    filename_p = sim_find_ef_file_id(file_id);
    if (filename_p == NULL) {
        return (-1);
    }

    file_p = fopen(filename_p, "r+b");

    if (file_p == NULL) {
        free(filename_p);
        return (-1);
    }

    free(filename_p);

    switch (file_id) {
    case EF_CBMID:
        SIM_LOG_I("ste_uicc_sim_file_get_format_sync: file = EF_CBMID");
        file_format->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        file_format->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        file_format->status_word.sw1 = 0x01;
        file_format->status_word.sw2 = 0x02;
        file_format->file_type = SIM_FILE_STRUCTURE_TRANSPARENT;
        file_format->file_size = sim_get_ef_filesize(file_p);
        file_format->record_len = file_format->file_size;
        file_format->num_records = 1;
        break;

    case EF_SMS:
        SIM_LOG_I("ste_uicc_sim_file_get_format_sync: file = EF_SMS");
        file_format->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        file_format->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        file_format->status_word.sw1 = 0x01;
        file_format->status_word.sw2 = 0x02;
        file_format->file_type = SIM_FILE_STRUCTURE_LINEAR_FIXED;
        file_format->file_size = sim_get_ef_filesize(file_p);
        file_format->record_len = 176;
        file_format->num_records = file_format->file_size / file_format->record_len;
        break;

    case EF_SMSR:
        SIM_LOG_I("ste_uicc_sim_file_get_format_sync: file = EF_SMSR");
        file_format->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        file_format->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        file_format->status_word.sw1 = 0x01;
        file_format->status_word.sw2 = 0x02;
        file_format->file_type = SIM_FILE_STRUCTURE_LINEAR_FIXED;
        file_format->file_size = sim_get_ef_filesize(file_p);
        file_format->record_len = 30;
        file_format->num_records = file_format->file_size / file_format->record_len;
        break;

    default:
        rv = -1;
        SIM_LOG_E("ste_uicc_sim_file_get_format_sync ERROR no file to read");
        break;
    }

    fclose(file_p);

    return (rv);
}

int ste_uicc_sim_file_read_binary_sync(
    ste_sim_t * sim,
    uintptr_t client_tag,
    int file_id,
    int offset,
    int length,
    const char *file_path,
    ste_uicc_sim_file_read_binary_response_t * read_result)
{
    size_t nobytes = 0;
    int rv = 0;
    FILE *file_p = NULL;
    char *filename_p = NULL;

    SIM_LOG_I("ste_uicc_sim_file_read_binary_sync");

    filename_p = sim_find_ef_file_id(file_id);
    if (filename_p == NULL) {
        return (-1);
    }

    file_p = fopen(filename_p, "r+b");
    if (file_p == NULL) {
        free(filename_p);
        return (-1);
    }

    free(filename_p);

    read_result->uicc_status_code = STE_UICC_STATUS_CODE_OK;
    read_result->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    read_result->status_word.sw1 = 0x01;
    read_result->status_word.sw2 = 0x02;

    if (length > 0) {
        read_result->data = malloc(length);

        if (read_result->data == NULL) {
            read_result->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            rv = -1;
        } else {
            read_result->length = length;
            memset(read_result->data, 0, read_result->length);

            fseek(file_p, offset, SEEK_SET);
            nobytes = fread(read_result->data, sizeof(read_result->data[0]), read_result->length, file_p);
            fseek(file_p, 0, SEEK_SET);
        }
    } else {
        read_result->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        rv = -1;
    }

    fclose(file_p);

    return (rv);
}

int ste_uicc_sim_file_update_record_sync(
    ste_sim_t * sim,
    uintptr_t client_tag,
    int file_id,
    int record_id,
    int length,
    const char *file_path,
    const uint8_t * data,
    ste_uicc_update_sim_file_record_response_t * update_result)
{
    int rv = -1;
    int offset = 0;
    int filesize = 0;
    size_t nobytes = 0;
    FILE *file_p = NULL;
    char *filename_p = NULL;

    SIM_LOG_I("ste_uicc_sim_file_update_record_sync");

    filename_p = sim_find_ef_file_id(file_id);
    if (filename_p == NULL) {
        return (rv);
    }

    file_p = fopen(filename_p, "r+b");
    if (file_p == NULL) {
        free(filename_p);
        return (rv);
    }

    free(filename_p);

    filesize = sim_get_ef_filesize(file_p);
    if (filesize == 0) {
        fclose(file_p);
        return (rv);
    }

    offset = (record_id - 1) * length;

    fseek(file_p, offset, SEEK_SET);
    nobytes = fwrite(data, sizeof(data[0]), length, file_p);
    fseek(file_p, 0, SEEK_SET);
    fclose(file_p);

    if (nobytes == length) {
        rv = 0;
    }

    if (rv == 0) {
        update_result->uicc_status_code = STE_UICC_STATUS_CODE_OK;
    } else {
        update_result->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
    }

    update_result->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    update_result->status_word.sw1 = 0x01;
    update_result->status_word.sw2 = 0x02;

    return (rv);
}

int ste_uicc_sim_file_read_record_sync(
    ste_sim_t * sim,
    uintptr_t client_tag,
    int file_id,
    int record_id,
    int length,
    const char *file_path,
    ste_uicc_sim_file_read_record_response_t * read_result)
{
    int rv = -1;
    int offset = 0;
    int filesize = 0;
    size_t nobytes = 0;
    FILE *file_p = NULL;
    char *filename_p = NULL;

    SIM_LOG_I("ste_uicc_sim_file_read_record_sync");

    filename_p = sim_find_ef_file_id(file_id);
    if (filename_p == NULL) {
        return (rv);
    }

    file_p = fopen(filename_p, "r+b");
    if (file_p == NULL) {
        free(filename_p);
        return (rv);
    }

    free(filename_p);

    filesize = sim_get_ef_filesize(file_p);
    read_result->data = malloc(length);

    if (filesize == 0 || read_result->data == NULL) {
        if (read_result->data != NULL) {
            free(read_result->data);
        }
        fclose(file_p);
        return (rv);
    }

    offset = (record_id - 1) * length;

    fseek(file_p, offset, SEEK_SET);
    nobytes = fread(read_result->data, sizeof(read_result->data[0]), length, file_p);
    fseek(file_p, 0, SEEK_SET);
    fclose(file_p);

    if (nobytes == length) {
        read_result->length = nobytes;
        rv = 0;
    }

    read_result->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    read_result->status_word.sw1 = 0x01;
    read_result->status_word.sw2 = 0x02;

    if (rv == 0) {
        read_result->uicc_status_code = STE_UICC_STATUS_CODE_OK;
    } else {
        read_result->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;

        if (read_result->data != NULL) {
            free(read_result->data);
        }
        read_result->length = 0;
    }

    return (rv);
}

int ste_uicc_get_service_availability_sync(
    ste_sim_t * sim,
    uintptr_t client_tag,
    sim_service_type_t service_type,
    ste_uicc_get_service_availability_response_t * read_response)
{
    int rv = 0;

    SIM_LOG_I("ste_uicc_get_service_availability_sync");

    // MUST run in st mode!
    if (!sim || !sim->is_st) {
        return -1;
    }

    switch (service_type) {

    case SIM_SERVICE_TYPE_SMS_CB:
    case SIM_SERVICE_TYPE_SMS_PP:
    case SIM_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM:
        read_response->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        read_response->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        read_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
        break;

    default:
        read_response->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        read_response->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        read_response->service_availability = STE_UICC_SERVICE_AVAILABILITY_UNKNOWN;
        break;
    }

    return (rv);
}

int ste_uicc_get_service_table_sync(
    ste_sim_t * sim,
    uintptr_t client_tag,
    sim_service_type_t service_type,
    ste_uicc_get_service_table_response_t * read_response)
{
    int rv = 0;

    SIM_LOG_I("ste_uicc_get_service_table_sync");

    // MUST run in st mode!
    if (!sim || !sim->is_st) {
        return -1;
    }

    switch (service_type) {

    case SIM_SERVICE_TYPE_SMS_CB:
    case SIM_SERVICE_TYPE_SMS_PP:
    case SIM_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM:
        read_response->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        read_response->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED;
        read_response->status_word.sw1 = 0x01;
        read_response->status_word.sw2 = 0x01;
        read_response->service_status = STE_UICC_SERVICE_STATUS_ENABLED;
        break;

    default:
        read_response->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        read_response->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        read_response->status_word.sw1 = 0x00;
        read_response->status_word.sw2 = 0x00;
        read_response->service_status = STE_UICC_SERVICE_STATUS_UNKNOWN;
        break;
    }

    return (rv);
}

int ste_uicc_sim_smsc_get_active_sync(
    ste_sim_t * sim,
    uintptr_t client_tag,
    ste_uicc_sim_smsc_get_active_response_t * get_result)
{
    int rv = 0;

    SIM_LOG_I("ste_uicc_sim_smsc_get_active_sync");

    // MUST run in st mode!
    if (!sim || !sim->is_st) {
        return -1;
    }

    get_result->uicc_status_code = STE_UICC_STATUS_CODE_OK;
    get_result->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    get_result->status_word.sw1 = 0x01;
    get_result->status_word.sw1 = 0x02;

    get_result->smsc.num_text.text_coding = STE_SIM_GSM_Default;
    get_result->smsc.num_text.no_of_characters = sizeof(sim_default_smsc);
    get_result->smsc.num_text.text_p = malloc(get_result->smsc.num_text.no_of_characters);

    memset(get_result->smsc.num_text.text_p, 0x00, get_result->smsc.num_text.no_of_characters);
    memcpy(get_result->smsc.num_text.text_p, sim_default_smsc, get_result->smsc.num_text.no_of_characters);

    get_result->smsc.ton = STE_SIM_TON_NATIONAL;
    get_result->smsc.npi = STE_SIM_NPI_NATIONAL;

    return (rv);
}
