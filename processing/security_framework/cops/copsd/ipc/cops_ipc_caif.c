/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <cops_ipc_common.h>
#include <cops_common.h>
#include <sys/socket.h>
#include <linux/caif/caif_socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#ifdef COPS_SILENT_REBOOT_SUPPORT
#include <dbus/dbus.h>
#include <cops_msg_handler.h>
#include <cops_router.h>
#endif

#ifdef __LBP__
/**
  * Copied from linux kernels, include/linux/socket.h since that is not
  * included in glibc compilers yet.
  */
#define AF_CAIF		37	/* CAIF sockets			*/
#define PF_CAIF		AF_CAIF
#endif

#ifdef COPS_SILENT_REBOOT_SUPPORT

#define COPS_DBUS_MATCH_STRING \
    "type='signal', interface='com.stericsson.mid.Modem'"
#define DBUS_CONNECTION_NAME "com.stericsson.mid"
#define DBUS_OBJECT_PATH "/com/stericsson/mid"
#define DBUS_OBJECT_INTERFACE "com.stericsson.mid.Modem"

enum cops_ipc_dbus_state {
    COPS_DBUS_MODEM_UNKNOWN = 0,
    COPS_DBUS_MODEM_OFF,
    COPS_DBUS_MODEM_BOOTING,
    COPS_DBUS_MODEM_ON,
};

struct cops_ipc_dbus_context {
    int fd;
    DBusConnection *conn;
    enum cops_ipc_dbus_state state;
};

static void cops_ipc_destroy(struct cops_ipc_context *ctx);
static void cops_ipc_get_fdset(struct cops_ipc_context *ctx,
                               int *nfds, fd_set *readfds);
static void cops_ipc_handle_signals(struct cops_ipc_context *ctx,
                                    int nfds, fd_set *readfds);

static dbus_bool_t cops_ipc_dbus_add(DBusWatch *watch, void *data);
static void cops_ipc_dbus_remove(DBusWatch *watch, void *data);
static void cops_ipc_dbus_toggle(DBusWatch *watch, void *data);
static cops_return_code_t cops_ipc_dbus_request(DBusConnection *conn,
                                                char *response);
#endif /* COPS_SILENT_REBOOT_SUPPORT */

cops_return_code_t cops_ipc_open(struct cops_ipc_context *ctx, int *fd)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int local_fd = -1;
#ifdef COPS_SILENT_REBOOT_SUPPORT
    DBusError err;
    DBusConnection *conn = NULL;
    struct cops_ipc_dbus_context *dbus_ctx = NULL;
#endif

    local_fd = socket(PF_CAIF, SOCK_STREAM, CAIFPROTO_UTIL);

    if (-1 == local_fd) {
        COPS_SET_RC(COPS_RC_INTERNAL_IPC_ERROR, "socket() failed\n");
    }
    *fd = local_fd;

#ifdef COPS_SILENT_REBOOT_SUPPORT
    /* Setup DBUS */
    dbus_ctx = calloc(1, sizeof(struct cops_ipc_dbus_context));
    COPS_CHK_ALLOC(dbus_ctx);
    dbus_ctx->fd = -1;
    dbus_error_init(&err);

    while (1) {
        conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);

        if (!conn || dbus_error_is_set(&err)) {
            COPS_LOG(LOG_ERROR, "Failed to connect to dbus: %s: %s. "
                     "Retrying...\n", err.name, err.message);
            usleep(500000);
            dbus_error_init(&err);
            continue;
        }
        break;
    }

    dbus_bus_add_match(conn, COPS_DBUS_MATCH_STRING, &err);

    if (dbus_error_is_set(&err)) {
        COPS_SET_RC(COPS_RC_INTERNAL_ERROR,
                    "Failed to add match: %s:%s\n", err.name, err.message);
    }

    if (!dbus_connection_set_watch_functions(conn, cops_ipc_dbus_add,
                                             cops_ipc_dbus_remove,
                                             cops_ipc_dbus_toggle, dbus_ctx,
                                             dbus_free)) {
        COPS_SET_RC(COPS_RC_INTERNAL_ERROR,
                    "Failed to setup watch functions\n");
    }

    dbus_ctx->conn = conn;
    /* When starting up we dont know the state of the modem */
    dbus_ctx->state = COPS_DBUS_MODEM_UNKNOWN;

    ctx->internal_ctx = dbus_ctx;
    ctx->internal_cbs.destroy = cops_ipc_destroy;
    ctx->internal_cbs.get_fdset = cops_ipc_get_fdset;
    ctx->internal_cbs.handle_signals = cops_ipc_handle_signals;
#else
    (void) ctx;
#endif

function_exit:
    return ret_code;
}

cops_return_code_t cops_ipc_start(struct cops_ipc_context *ctx)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;
#ifdef COPS_SILENT_REBOOT_SUPPORT
    struct cops_ipc_dbus_context *dbus_ctx = ctx->internal_ctx;
#endif

    /* Sanity check */
    if (ctx->server) {
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Cannot start a CAIF server\n");
    }

#ifdef COPS_SILENT_REBOOT_SUPPORT
    /* Try to connect if modem state is NOT off */
    if (COPS_SOCKET_STATE_WAITING == ctx->data.state &&
        COPS_DBUS_MODEM_OFF != dbus_ctx->state) {
#else
    if (COPS_SOCKET_STATE_WAITING == ctx->data.state) {
#endif
        socklen_t slen;
        struct sockaddr_caif addr;
        const char *path = STR(SIPC_CLIENT);
        size_t plen = strlen(path);

        if (plen > sizeof(addr.u.util.service)) {
            COPS_SET_RC(COPS_RC_INTERNAL_ERROR, "Path name too long\n");
        }

        (void)unlink(path); /* may fail */

        memset(&addr, 0, sizeof(addr));
        addr.family = AF_CAIF;
        memcpy(addr.u.util.service, path, plen);
        slen = sizeof(struct sockaddr_caif);

        if (-1 == connect(ctx->data.request_fd, (struct sockaddr *)&addr,
                          slen)) {
            COPS_SET_RC(COPS_RC_IPC_ERROR,
                        "Error: Connect failed with errno = %d\n", errno);
        }

        ctx->data.state = COPS_SOCKET_STATE_READY;

#ifdef COPS_SILENT_REBOOT_SUPPORT
        /*
         * If modem state is NOT unknown it should be safe to assume that
         * we are booting for the first time or modem has done a silent reboot.
         * Derive session key.
         */
        if (COPS_DBUS_MODEM_UNKNOWN != dbus_ctx->state) {
            ((struct cops_state *)(ctx->data.aux))->session_key_ready = false;
            COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                              COPS_SIPC_DERIVE_SESSION_KEY_MREQ,
                                              COPS_SENDER_UNSECURE,
                                              &in_msg));
            COPS_LOG(LOG_INFO, "Triggering SIPC. Type = 0x%x, length = %u\n",
                     in_msg->msg_type, in_msg->length);
            COPS_CHK_RC(cops_msg_handle(ctx->data.aux, in_msg, &out_msg));
            COPS_CHK_RC(cops_router_handle_msg(ctx->data.aux, NO_FD, in_msg,
                                               out_msg));
        } else {
            dbus_ctx->state == COPS_DBUS_MODEM_ON;
        }
#endif
    } else {
        /* Booting event not received. Handled this way to not spam log */
        ret_code = COPS_RC_IPC_ERROR;
        goto function_exit;
    }

function_exit:
    cops_sipc_free_message(&in_msg);
    cops_sipc_free_message(&out_msg);
    return ret_code;
}

#ifdef COPS_SILENT_REBOOT_SUPPORT
static void cops_ipc_destroy(struct cops_ipc_context *ctx)
{
    /* TODO: Close DBUS */

    free(ctx->internal_ctx);
    ctx->internal_ctx = NULL;
}

static void cops_ipc_get_fdset(struct cops_ipc_context *ctx,
                               int *nfds, fd_set *readfds)
{
    struct cops_ipc_dbus_context *dbus_ctx = ctx->internal_ctx;

    FD_SET(dbus_ctx->fd, readfds);

    if (dbus_ctx->fd > *nfds) {
        *nfds = dbus_ctx->fd;
    }
}

static void cops_ipc_handle_signals(struct cops_ipc_context *ctx,
                                    int nfds, fd_set *readfds)
{
    char response[32];
    struct cops_ipc_dbus_context *dbus_ctx = ctx->internal_ctx;

    if (dbus_ctx->fd < 0 || dbus_ctx->fd > nfds ||
        !FD_ISSET(dbus_ctx->fd, readfds)) {
        return;
    }

    memset(&response[0], 0x0, 32);

    if (COPS_RC_OK == cops_ipc_dbus_request(dbus_ctx->conn, &response[0])) {

        if (0 == strncmp(&response[0], "booting", 7)) {
            dbus_ctx->state = COPS_DBUS_MODEM_BOOTING;
        } else if (0 == strncmp(&response[0], "on", 2)) {
            dbus_ctx->state = COPS_DBUS_MODEM_ON;
        } else if (0 == strncmp(&response[0], "off", 3) ||
                   0 == strncmp(&response[0], "prepare_off", 11) ||
                   0 == strncmp(&response[0], "dumping", 7)) {
            dbus_ctx->state = COPS_DBUS_MODEM_OFF;
            ctx->data.state = COPS_SOCKET_STATE_WAITING;
        } else {
            COPS_LOG(LOG_INFO, "%s returned and ignored.\n", &response[0]);
        }
    }
}

static dbus_bool_t cops_ipc_dbus_add(DBusWatch *watch, void *data)
{
    int fd = dbus_watch_get_fd(watch);
    int enabled = dbus_watch_get_enabled(watch);
    struct cops_ipc_dbus_context *ctx = data;

    if (NULL != ctx && enabled) {
        ctx->fd = fd;
    }

    return 1;
}

static void cops_ipc_dbus_remove(DBusWatch *watch, void *data)
{
    COPS_LOG(LOG_INFO, "Remove watch at %d\n", (int)watch);
}

static void cops_ipc_dbus_toggle(DBusWatch *watch, void *data)
{
    COPS_LOG(LOG_INFO, "Toggle watch at %d\n", (int)watch);
}

static cops_return_code_t cops_ipc_dbus_request(DBusConnection *conn,
                                                char *response)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    DBusMessage *msg = NULL;
    DBusMessageIter iter;

    dbus_connection_dispatch(conn);

    dbus_connection_read_write(conn, 0);
    msg = dbus_connection_pop_message(conn);

    if (msg != NULL && dbus_message_is_signal(msg, "com.stericsson.mid.Modem",
        "StateChange")) {
        if (!dbus_message_iter_init(msg, &iter)) {
            COPS_SET_RC(COPS_RC_INTERNAL_ERROR,
                        "Received message has no arguments\n");
        } else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&iter)) {
            COPS_SET_RC(COPS_RC_INTERNAL_ERROR,
                        "Argument is not a string\n");
        } else {
            char *value = NULL;
            dbus_message_iter_get_basic(&iter, &value);
            strncpy(response, value, 32 - 1);
            COPS_LOG(LOG_INFO, "Got message: \"%s\", response: \"%s\"\n",
                     value, response);
        }
    }

function_exit:
    dbus_message_unref(msg);
    return ret_code;
}
#endif /*COPS_SILENT_REBOOT_SUPPORT*/

