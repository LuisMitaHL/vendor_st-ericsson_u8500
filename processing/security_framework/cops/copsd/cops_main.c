/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_common.h>
#include <cops_state.h>
#include <cops_ipc_common.h>
#include <cops_tapp_common.h>
#include <cops_fd.h>
#include <cops_srv.h>
#include <cops_msg_handler.h>
#include <cops_router.h>
#include <cops_bootpartitionhandler.h>
#include <cops_tocaccess.h>
#include <cops_storage.h>
#include <cops_ipc_common.h>
#ifndef COPS_OSE_ENVIRONMENT
#include <cops_wakelock.h>
#endif
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef COPS_IN_ANDROID
#include <private/android_filesystem_config.h>
#endif

extern uint8_t run_mode;

static cops_return_code_t cops_setup(cops_server_context_id_t **ctxp,
                                     struct cops_state **statep,
                                     struct cops_ipc_context **ipc_client_ctxp,
                                     struct cops_ipc_context **ipc_server_ctxp);

static cops_return_code_t cops_shutdown(cops_server_context_id_t **ctxp,
                                     struct cops_state **statep,
                                     struct cops_ipc_context **ipc_client_ctxp,
                                     struct cops_ipc_context **ipc_server_ctxp);

#ifndef COPS_OSE_ENVIRONMENT
static void cops_shutdown_handler(int signo);
static void cops_signals_init(void);
/* Can be updated through a signal handler (cops_shutdown_handler()) */
sig_atomic_t cops_shut_down;
#else
bool cops_shut_down;
#endif

#ifdef COPS_IN_ANDROID
static cops_return_code_t cops_drop_privileges(void);
static inline void cops_change_file_permission(const char *filename);
#endif

static void cops_update_simlock_timers(struct cops_state *state);
#ifdef COPS_ENABLE_ARB_UPDATE
static cops_return_code_t cops_check_arb_update(struct cops_state *state);
#endif
static cops_return_code_t cops_set_run_mode_and_sync(void);
#ifndef COPS_OSE_ENVIRONMENT
#ifdef COPS_SYNC_FILE
static cops_return_code_t cops_create_sync_file(const char *filename);
#endif
#endif

static cops_return_code_t cops_setup_data(cops_data_t *data, uint32_t length);

#ifdef COPS_IN_ANDROID
static cops_return_code_t cops_drop_privileges(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int fd0;
    int fd1;
    int fd2;
    uint32_t i;

    /*
     * This will set file permissions to 777 since the argument provided to
     * umask will be binary AND with 0777, i.e. arg & 0777. This will make it
     * possible for everyone to read and write to cops.log and cops.socket
     * file.
     */
    umask(0);

    /*
     * We change to the root folder because it might be that our process is
     * spawned from a filesystem that is mounted. If that is the case, then one
     * cannot unmount that filesystem when our file is using it. By changing to
     * the root folder you don't occupy/lock any filesystem. This is considered
     * as a good practice when writing daemons (Advanced Programming in the
     * UNIX(r) Environment: Second Edition, ISBN: 0201433079).
     */
    if (chdir("/") < 0) {
        COPS_SET_RC(COPS_RC_UNSPECIFIC_ERROR,
                    "Cannot change directory to /\n");
    }

    /* Close stdin, stdout and stderr. */
    for (i = 0; i < 2; i++) {
        if (close(i) == -1 && errno != EBADF) {
            COPS_SET_RC(COPS_RC_UNSPECIFIC_ERROR,
                        "Failed closing fd: %d, (e: %d)\n", i, errno);
        }
    }

    /* Attach file descriptors 0, 1, and 2 to /dev/null. */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(fd0);
    fd2 = dup(fd0);

    if (fd0 == -1 || fd1 == -1 || fd2 == -1) {
        COPS_SET_RC(COPS_RC_UNSPECIFIC_ERROR,
                    "Failed duplicating /dev/null\n");

        /*
         * If we cannot create more file descriptors, then we just exit, since
         * we will very soon try to create more file descriptors anyway (which
         * also will fail), logging in cops for example. This is one of those
         * errors that never should occur.
         */
        if (errno == EMFILE) {
            COPS_SET_RC(COPS_RC_INTERNAL_ERROR,
                        "Cannot create more files, exit copsdaemon\n");
        }
    }

    /* If it's not possible to drop privileges, then just exit copsdaemon. */
    if (setgid(AID_NOBODY) != 0 || setuid(AID_NOBODY) != 0) {
        COPS_SET_RC(COPS_RC_INTERNAL_ERROR,
                    "Cannot drop privileges, exit copsdaemon\n");
    }

function_exit:

    if (ret_code != COPS_RC_OK) {

        if (ret_code == COPS_RC_INTERNAL_ERROR) {
            COPS_LOG(LOG_ERROR, "Failed to drop privileges, giving up!\n");
        } else {
            COPS_LOG(LOG_WARNING, "A non vital error (%d) occured when trying "
                    "to drop privileges.\n", ret_code);
            ret_code = COPS_RC_OK;
        }
    }
    return ret_code;
}

static inline void cops_change_file_permission(const char *filename)
{
    if (chown(filename, AID_NOBODY, AID_NOBODY) != 0) {
        COPS_LOG(LOG_WARNING, "Couldn't change permissions for %s\n", filename);
    }
}
#endif /* COPS_IN_ANDROID */

#ifdef COPS_OSE_ENVIRONMENT
OS_PROCESS(COPS_Process)
#else
int main(int argc, char *argv[])
#endif
{
    cops_server_context_id_t *ctx = NULL;
    struct cops_state *state = NULL;
    struct cops_ipc_context *ipc_client_ctx = NULL;
    struct cops_ipc_context *ipc_server_ctx = NULL;

#ifndef COPS_OSE_ENVIRONMENT
    (void) argv;
    (void) argc;
    cops_shut_down = 0;
#else
    cops_shut_down = false;
#endif

    (void)cops_set_run_mode_and_sync();

    if (cops_setup(&ctx, &state, &ipc_client_ctx,
                   &ipc_server_ctx) != COPS_RC_OK) {
        COPS_LOG(LOG_ERROR, "cops_setup failed\n");
        goto function_exit;
    }

    COPS_LOG(LOG_INFO, "Entering main loop\n");

    while (!cops_shut_down) {
        fd_set    rfds;
        int       nfds;
        int       res;

        FD_ZERO(&rfds);
        nfds = 0;
        cops_server_context_get_fdset(ctx, &nfds, &rfds);

        if (COPS_RUN_MODE_RELAY == run_mode ||
            COPS_RUN_MODE_DAEMON == run_mode) {

            if (COPS_SOCKET_STATE_WAITING == ipc_client_ctx->data.state) {
                (void) cops_ipc_context_start(ipc_client_ctx);
            }
            cops_ipc_context_get_fdset(ipc_client_ctx, &nfds, &rfds);
        }

        if (COPS_RUN_MODE_MODEM == run_mode ||
            COPS_RUN_MODE_RELAY == run_mode) {
            cops_ipc_context_get_fdset(ipc_server_ctx, &nfds, &rfds);
        }

        do {

            if ((COPS_RUN_MODE_RELAY == run_mode ||
                COPS_RUN_MODE_DAEMON == run_mode) &&
                COPS_SOCKET_STATE_WAITING == ipc_client_ctx->data.state) {
                struct timeval timeout = {0, 100000};
                res = cops_select(nfds + 1, &rfds, NULL, NULL, &timeout);
            } else {
                res = cops_select(nfds + 1, &rfds, NULL, NULL, NULL);
            }

            /* We got a signal during select telling us to shut down. */
            if (cops_shut_down) {
                goto function_exit;
            }

        } while (res == -1 && errno == EINTR);

        if (res == -1) {
            COPS_LOG(LOG_ERROR, "Select failed: %s, giving up!\n",
                     strerror(errno));
            goto function_exit;
        }

        cops_server_context_handle_requests(ctx, nfds, &rfds);

        if (COPS_RUN_MODE_RELAY == run_mode ||
            COPS_RUN_MODE_DAEMON == run_mode) {
            cops_ipc_context_handle_signals(ipc_client_ctx, state, nfds, &rfds);
        }

        if (COPS_RUN_MODE_MODEM == run_mode ||
            COPS_RUN_MODE_RELAY == run_mode) {
            cops_ipc_context_handle_signals(ipc_server_ctx, state, nfds, &rfds);
        }
    }

function_exit:
    if (cops_shutdown(&ctx, &state, &ipc_client_ctx,
                      &ipc_server_ctx) != COPS_RC_OK) {
        COPS_LOG(LOG_ERROR, "cops_shutdown failed\n");
    }

#ifndef COPS_OSE_ENVIRONMENT
    return EXIT_SUCCESS;
#endif
}

static cops_return_code_t cops_setup(cops_server_context_id_t **ctxp,
                                     struct cops_state **statep,
                                     struct cops_ipc_context **ipc_client_ctxp,
                                     struct cops_ipc_context **ipc_server_ctxp)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_server_callbacks_t cops_srv_cbs;

#ifndef COPS_TAPP_EMUL
    TEEC_UUID cops_ta_uuid = {
        0x42beedc0,
        0x06ae,
        0x11e0,
        {0x85, 0x31, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}
    };

    TEEC_Result teec_ret = TEEC_ERROR_ITEM_NOT_FOUND;
#ifdef COPS_USE_GP_TEEV0_17
    TEEC_ErrorOrigin errorOrigin;
#else
    uint32_t errorOrigin;
#endif
#endif

    COPS_LOG(LOG_INFO, "Starting up\n");

    /* init the server callbacks */
    cops_srv_cbs = cops_srv_common_cbs_minimal;

#ifndef COPS_OSE_ENVIRONMENT
    cops_signals_init();

    /* We need to open wakelock fd's before we drop copsd privileges */
    cops_wakelock_init();
#endif

    /* Initialize COPS state */
    *statep = calloc(1, sizeof(**statep));
    COPS_CHK_ALLOC(*statep);
    (*statep)->session_key_ready = true;

    COPS_CHK_RC(cops_tapp_setup(&(*statep)->tapp_io));

    COPS_CHK_RC(cops_setup_data(&(*statep)->data,
                (*statep)->tapp_io->data.max_length));

    if (COPS_RUN_MODE_DAEMON == run_mode) {
        (void)cops_storage_read(&(*statep)->data, COPS_DATA);
    }

    COPS_CHK_RC(cops_setup_data(&(*statep)->perm_auth_state_data,
                          (*statep)->tapp_io->perm_auth_state_data.max_length));

    if (COPS_RUN_MODE_RELAY == run_mode ||
        COPS_RUN_MODE_DAEMON == run_mode) {

        if (COPS_RC_OK != cops_storage_read(&(*statep)->perm_auth_state_data,
                                            PERM_AUTH_STATE_DATA)) {
            COPS_CHK_RC(cops_data_init(&(*statep)->perm_auth_state_data));
        }
    }

    cops_router_init();

    COPS_CHK_RC(cops_server_context_create(ctxp, &cops_srv_cbs, (*statep)));
    (*statep)->server_ctx = *ctxp;

    if (COPS_RUN_MODE_RELAY == run_mode ||
        COPS_RUN_MODE_DAEMON == run_mode) {
        COPS_CHK_RC(cops_ipc_context_create(ipc_client_ctxp, false, *statep));
        (*statep)->ipc_client_ctx = *ipc_client_ctxp;
    }

    if (COPS_RUN_MODE_MODEM == run_mode ||
        COPS_RUN_MODE_RELAY == run_mode) {
        COPS_CHK_RC(cops_ipc_context_create(ipc_server_ctxp, true, *statep));
        COPS_CHK_RC(cops_ipc_context_start(*ipc_server_ctxp));
        (*statep)->ipc_server_ctx = *ipc_server_ctxp;
    }

#ifndef COPS_TAPP_EMUL
    memset(&(*statep)->teec_sess, 0, sizeof((*statep)->teec_sess));

    /* Create TEE Context and Initialize it */
    teec_ret = TEEC_InitializeContext(NULL, &(*statep)->teec_sess.ctx);

    if (teec_ret != TEEC_SUCCESS) {
        COPS_SET_RC(COPS_RC_TEEC_ERROR, "TEEC_Init failed (0x%x)\n", teec_ret);
    }

    teec_ret = TEEC_OpenSession(&(*statep)->teec_sess.ctx,
                                &(*statep)->teec_sess,
                                &cops_ta_uuid,
                                TEEC_LOGIN_PUBLIC,
                                NULL,
                                NULL,
                                &errorOrigin);

    if (teec_ret != TEEC_SUCCESS) {
        COPS_SET_RC(COPS_RC_TEEC_ERROR,
                    "TEEC_OpenSession failed (0x%x, org:0x%x)\n",
                    teec_ret, errorOrigin);
    }
#endif

    if (COPS_RUN_MODE_DAEMON == run_mode) {
        cops_update_simlock_timers(*statep);

        /*
         * only relevant when running on Linux, i.e. COPS_DAEMON
         */
#ifdef COPS_IN_ANDROID
        cops_change_file_permission(STR(COPS_LOG_FILE));
        cops_change_file_permission(STR(COPS_SOCKET_PATH));
#endif

        COPS_CHK_RC(cops_bootpart_open());

        COPS_CHK_RC(cops_open_tochandler());

#ifdef COPS_ENABLE_ARB_UPDATE
        COPS_CHK_RC(cops_check_arb_update(*statep));
#endif

#ifdef COPS_IN_ANDROID
        COPS_CHK_RC(cops_drop_privileges());
#endif

        /* Notify our API Proxy that we are up and running */
        COPS_CHK_RC(cops_router_notify_api_proxy(COPS_IPC_READY_PRIMITIVE));
    }

function_exit:
    return ret_code;
}

static cops_return_code_t cops_shutdown(cops_server_context_id_t **ctxp,
                                      struct cops_state **statep,
                                      struct cops_ipc_context **ipc_client_ctxp,
                                      struct cops_ipc_context **ipc_server_ctxp)
{
    cops_return_code_t ret_code = COPS_RC_INTERNAL_ERROR;
#ifndef COPS_TAPP_EMUL
#ifdef COPS_USE_GP_TEEV0_17
    TEEC_Result teec_ret = TEEC_ERROR_ITEM_NOT_FOUND;
#endif
#endif

    COPS_LOG(LOG_INFO, "Shutting down\n");

    (void)cops_bootpart_close();
    (void)cops_close_tochandler();

    if (COPS_RUN_MODE_DAEMON == run_mode) {
        cops_update_simlock_timers(*statep);
    }

    cops_server_context_destroy(ctxp);

    if (COPS_RUN_MODE_RELAY == run_mode ||
        COPS_RUN_MODE_DAEMON == run_mode) {
        cops_ipc_context_destroy(ipc_client_ctxp);
    }

    if (COPS_RUN_MODE_MODEM == run_mode ||
        COPS_RUN_MODE_RELAY == run_mode) {
        cops_ipc_context_destroy(ipc_server_ctxp);
    }

#ifndef COPS_TAPP_EMUL
    if ((*statep) != NULL && 0 != (*statep)->teec_sess.ctx) {
#ifdef COPS_USE_GP_TEEV0_17
        teec_ret = TEEC_CloseSession(&(*statep)->teec_sess);

        if (teec_ret == TEEC_SUCCESS) {
            /* Ignore error code. */
            (void) TEEC_FinalizeContext(&(*statep)->teec_sess.ctx);
        }
#else
        TEEC_CloseSession(&(*statep)->teec_sess);

        (void) TEEC_FinalizeContext(&(*statep)->teec_sess.ctx);
#endif
    }
#endif

    if ((*statep) != NULL) {
        if ((*statep)->data.data != NULL) {
            free((*statep)->data.data);
        }

        cops_tapp_destroy(&(*statep)->tapp_io);
        free(*statep);
    }

#ifndef COPS_OSE_ENVIRONMENT
    cops_wakelock_final();
#endif

    ret_code = COPS_RC_OK;

    return ret_code;
}

static void cops_update_simlock_timers(struct cops_state *state)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;

    /* Do get status since this function will update the actual timer values */
    COPS_CHK_RC(cops_sipc_alloc_message(COPS_RC_OK, 0,
                                        COPS_SIPC_SIMLOCK_GET_STATUS_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_msg_handle(state, msg, NULL));

function_exit:
    cops_sipc_free_message(&msg);
}

#ifndef COPS_OSE_ENVIRONMENT
static void cops_shutdown_handler(int signo)
{
    (void) signo;

    /* Shutdown signal received, let's shut down */
    cops_shut_down = 1;
}

static void cops_signals_init(void)
{
    /* POSIX signal handling functions (for Linux)
     * We need to handle this in a OSE environment!!
     * */
    struct sigaction act;

    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN;
    sigfillset(&act.sa_mask);

    if (sigaction(SIGPIPE, &act, NULL) == -1) {
        /*
         * Failed to install SIGPIPE handler, COPS will die if it
         * writes to a socket closed at the other end.
         */
        COPS_LOG(LOG_WARNING, "Ignoring error sigaction(SIGPIPE): %s\n",
                 strerror(errno));
    }

    act.sa_handler = cops_shutdown_handler;
    sigfillset(&act.sa_mask);

    if (sigaction(SIGHUP, &act, NULL) == -1) {
        /*
         * Failed to install SIGHUP handler, COPS will not shut down
         * clean on SIGHUP
         */
        COPS_LOG(LOG_WARNING, "Ignoring error sigaction(SIGHUP): %s\n",
                 strerror(errno));
    }

    act.sa_handler = cops_shutdown_handler;
    sigfillset(&act.sa_mask);

    if (sigaction(SIGTERM, &act, NULL) == -1) {
        /*
         * Failed to install SIGTERM handler, COPS will not shut down
         * clean on SIGTERM
         */
        COPS_LOG(LOG_WARNING, "Ignoring error sigaction(SIGTERM): %s\n",
                 strerror(errno));
    }

    act.sa_handler = cops_shutdown_handler;
    sigfillset(&act.sa_mask);

    if (sigaction(SIGINT, &act, NULL) == -1) {
        /*
         * Failed to install SIGINT handler, COPS will not shut down
         * clean on SIGINT
         */
        COPS_LOG(LOG_WARNING, "Ignoring error sigaction(SIGINT): %s\n",
                 strerror(errno));
    }
}
#endif

#ifdef COPS_ENABLE_ARB_UPDATE
static cops_return_code_t cops_check_arb_update(struct cops_state *state)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;

    COPS_CHK_RC(cops_sipc_alloc_message(ret_code, 0,
                                        COPS_SIPC_CHECK_ARB_UPDATE_MREQ,
                                        COPS_SENDER_UNSECURE, &msg));

    COPS_CHK_RC(cops_msg_handle(state, msg, NULL));

function_exit:
    /* If ARB support is stubbed that is ok */
    if (COPS_RC_SERVICE_NOT_AVAILABLE_ERROR == ret_code) {
        ret_code = COPS_RC_OK;
    }

    cops_sipc_free_message(&msg);
    return ret_code;
}
#endif

static cops_return_code_t cops_set_run_mode_and_sync(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;

#ifdef COPS_OSE_ENVIRONMENT
#ifdef COPS_SYNC_FILE
    int fd = -1;
    fd = open(STR(COPS_SYNC_FILE), O_RDONLY);

    if (-1 == fd) {
        run_mode = COPS_RUN_MODE_DAEMON;
    } else {
        run_mode = COPS_RUN_MODE_RELAY;

        if (-1 == close(fd)) {
            COPS_SET_RC(COPS_RC_STORAGE_ERROR, "close(%d): %s\n",
                        fd, strerror(errno));
        }
    }
#else
    run_mode = COPS_RUN_MODE_MODEM;
#endif /* COPS_SYNC_FILE */
#else
    run_mode = COPS_RUN_MODE_DAEMON;
#ifdef COPS_SYNC_FILE
    COPS_CHK_RC(cops_create_sync_file(STR(COPS_SYNC_FILE1)));
    COPS_CHK_RC(cops_create_sync_file(STR(COPS_SYNC_FILE2)));
#endif /* COPS_SYNC_FILE */
#endif /* COPS_OSE_ENVIRONMENT */

#ifdef COPS_SYNC_FILE
function_exit:
#endif

    return ret_code;
}

#ifndef COPS_OSE_ENVIRONMENT
#ifdef COPS_SYNC_FILE
static cops_return_code_t cops_create_sync_file(const char *filename)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int fd = -1;
    fd = open(filename, O_RDWR | O_CREAT);

    if (-1 == fd) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR, "open(\"%s\"): %s\n",
                    STR(COPS_SYNC_FILE), strerror(errno));
    }

    if (fchmod(fd, 0644) == -1) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR, "fchmod(%d, 0x644): %s\n",
                    fd, strerror(errno));
    }

    if (-1 == close(fd)) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR, "close(%d): %s\n",
                    fd, strerror(errno));
    }
function_exit:
    return ret_code;
}
#endif /* COPS_SYNC_FILE */
#endif /* COPS_OSE_ENVIRONMENT */

static cops_return_code_t cops_setup_data(cops_data_t *data, uint32_t length)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    data->data = calloc(1, length);
    COPS_CHK_ALLOC(data->data);
    data->max_length = length;

function_exit:
    return ret_code;
}
