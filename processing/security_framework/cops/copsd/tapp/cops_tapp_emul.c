/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_tapp_common.h>
#include <cops_common.h>
#include <cops_debug.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static cops_return_code_t
cops_tapp_fork_and_exec_service(cops_taf_id_t taf_id, int *readfd,
                                int *writefd,
                                pid_t *pid);

static void maybe_close(int *fd)
{
    if (*fd != -1) {
        (void)close(*fd);
        *fd = -1;
    }
}

cops_return_code_t
cops_tapp_invoke(struct cops_state *state, cops_tapp_io_t **tapp_io,
                 cops_taf_id_t taf_id)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int       readfd = -1;
    int       writefd = -1;
    pid_t     pid = -1;
    uint32_t  hdr[2];
    size_t    l;
    uint8_t  *p;
    ssize_t   res;

    (void) state;

    COPS_CHK_RC(cops_tapp_fork_and_exec_service(taf_id, &readfd, &writefd,
                &pid));
    (*tapp_io)->args.data = NULL;
    (*tapp_io)->data.data = NULL;
    (*tapp_io)->rvs.data = NULL;
    (*tapp_io)->perm_auth_state_data.data = NULL;

    p = (uint8_t *)*tapp_io;
    l = sizeof(cops_tapp_io_t) + (*tapp_io)->args.max_length +
        (*tapp_io)->data.max_length + (*tapp_io)->rvs.max_length +
        (*tapp_io)->perm_auth_state_data.max_length;

    hdr[0] = taf_id;
    hdr[1] = l;

wagain1:
    res = write(writefd, hdr, sizeof(hdr));

    if (res != sizeof(hdr)) {
        if (res == -1) {
            if (errno == EINTR) {
                goto wagain1;
            }

            COPS_SET_RC(COPS_RC_SERVICE_ERROR, "write(%d, x, %zu): %s",
                        writefd, sizeof(hdr), strerror(errno));
        } else {
            COPS_SET_RC(COPS_RC_SERVICE_ERROR, "write(%d, x, %zu): Short write",
                        writefd, sizeof(hdr));
        }
    }

    while (l > 0) {
        res = write(writefd, p, l);

        if (res == -1) {
            if (errno == EINTR) {
                continue;
            }

            COPS_SET_RC(COPS_RC_SERVICE_ERROR, "write(%d, x, %d): %s",
                        writefd, (int)l, strerror(errno));
        }

        l -= res;
        p += res;
    }

    p = (uint8_t *)*tapp_io;
    l = sizeof(cops_tapp_io_t) + (*tapp_io)->args.max_length +
        (*tapp_io)->data.max_length + (*tapp_io)->rvs.max_length +
        (*tapp_io)->perm_auth_state_data.max_length;

ragain1:
    res = read(readfd, hdr, sizeof(hdr));

    if (res == -1) {
        if (errno == EINTR) {
            goto ragain1;
        }

        COPS_SET_RC(COPS_RC_SERVICE_ERROR, "read(%d, x, %zu): %s",
                    readfd, sizeof(hdr), strerror(errno));
    }

    if (res == 0) {
        COPS_SET_RC(COPS_RC_SERVICE_ERROR,
                    "read(%d, x, %zu): Got end of file.\n",
                    readfd, sizeof(hdr));
    }

    if (taf_id != hdr[0]) {
        COPS_SET_RC(COPS_RC_SERVICE_ERROR,
                    "Got response from wrong service %d (expected %d)",
                    hdr[0], taf_id);
    }

    if (l != hdr[1]) {
        COPS_SET_RC(COPS_RC_SERVICE_ERROR,
                    "Got wrong size of respone %d (expected %zu)", hdr[1], l);
    }

    while (l > 0) {
        res = read(readfd, p, l);

        if (res == -1) {
            if (errno == EINTR) {
                continue;
            }

            COPS_SET_RC(COPS_RC_SERVICE_ERROR,
                        "read(%d, x, %zu): %s", readfd, l, strerror(errno));
        }

        if (res == 0) {
            COPS_SET_RC(COPS_RC_SERVICE_ERROR,
                        "read(%d, x, %zu): Got end of file ",
                        readfd, sizeof(hdr));
        }

        l -= res;
        p += res;
    }

    /* Restore pointers in tapp_io */
    p = (uint8_t *)*tapp_io;
    p += sizeof(cops_tapp_io_t);

    (*tapp_io)->args.data = p;
    p += (*tapp_io)->args.max_length;

    (*tapp_io)->data.data = p;
    p += (*tapp_io)->data.max_length;

    (*tapp_io)->rvs.data = p;
    p += (*tapp_io)->rvs.max_length;

    (*tapp_io)->perm_auth_state_data.data = p;

    cops_debug_print_error_stack(&(*tapp_io)->rvs);

function_exit:

    /*
     * Restore pointers in tapp_io (we need to do it again in case we
     * goto:ed to function_exit:
     */
    p = (uint8_t *)*tapp_io;
    p += sizeof(cops_tapp_io_t);

    (*tapp_io)->args.data = p;
    p += (*tapp_io)->args.max_length;

    (*tapp_io)->data.data = p;
    p += (*tapp_io)->data.max_length;

    (*tapp_io)->rvs.data = p;
    p += (*tapp_io)->rvs.max_length;

    (*tapp_io)->perm_auth_state_data.data = p;

    if (pid != -1 && pid != 0) {
        if (kill(pid, SIGKILL) == -1)
            COPS_LOG(LOG_ERROR, "kill(%d /*child service*/, SIGKILL): %s\n",
                     pid, strerror(errno));
        else {
            wait(NULL);
        }
    }

    maybe_close(&readfd);
    maybe_close(&writefd);
    return ret_code;
}

static cops_return_code_t
cops_tapp_fork_and_exec_service(cops_taf_id_t taf_id,
                                int *readfd, int *writefd, pid_t *pid)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int       stdin_fds[2] = { -1, -1 };
    int       stdout_fds[2] = { -1, -1 };

    if (pipe(stdin_fds) == -1 || pipe(stdout_fds) == -1) {
        COPS_SET_RC(COPS_RC_SERVICE_NOT_AVAILABLE_ERROR,
                    "pipe: %s", strerror(errno));
    }

    *pid = fork();

    if (*pid == -1) {
        COPS_SET_RC(COPS_RC_SERVICE_NOT_AVAILABLE_ERROR,
                    "fork: %s", strerror(errno));
    }

    if (*pid == 0) {            /* Child */
        char      taf_str[4];
        int       stdinfd = stdin_fds[0];
        int       stdoutfd = stdout_fds[1];

        maybe_close(&stdin_fds[1]);
        maybe_close(&stdout_fds[0]);

        if (dup2(stdinfd, STDIN_FILENO) == -1 ||
                dup2(stdoutfd, STDOUT_FILENO) == -1) {

            COPS_LOG(LOG_ERROR, "dup2: %s\n", strerror(errno));
            exit(1);
        }

        if (stdinfd != STDIN_FILENO) {
            close(stdinfd);
        }

        if (stdoutfd != STDOUT_FILENO) {
            close(stdoutfd);
        }

        snprintf(taf_str, sizeof(taf_str), "%d", taf_id);

        /* only returns on error */
        execlp(COPS_TAPP_TEST_PATH "tapp_test", "tapp_test", taf_str, NULL);

        COPS_LOG(LOG_ERROR, "execlp(\"%s\"): %s\n",
                 COPS_TAPP_TEST_PATH "tapp_test", strerror(errno));
        exit(1);
    }

    *readfd = stdout_fds[0];
    stdout_fds[0] = -1;
    *writefd = stdin_fds[1];
    stdin_fds[1] = -1;

function_exit:
    /* Parent */
    maybe_close(&stdout_fds[0]);
    maybe_close(&stdout_fds[1]);
    maybe_close(&stdin_fds[0]);
    maybe_close(&stdin_fds[1]);
    return ret_code;
}
