/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#ifndef ANDROID
#define UNIX_PATH_MAX (108)
#endif

#include <util_mainloop.h>
#include "mad_log.h"
#include "backend/at_handler.h"
#include "backend/tx_bo.h"

#ifdef ANDROID
#define AT_SOCKET_PATH "/dev/socket/at_core"
#else
#define AT_SOCKET_PATH "/tmp/mad_test_at_socket"
#endif /* ANDROID */

#define MAD_AT_CONNECT_MAX_RETRIES (10)
#define MAD_AT_CONNECT_RETRY_INTERVAL (1000)

#define MAD_AT_HANDSHAKE_MAX_RETRIES (2)
#define MAD_AT_HANDSHAKE_TIMEOUT (250)
#define MAD_AT_COMMAND_TIMEOUT (3 * 60 * 1000)
#define MAD_AT_RESPONSE_MAX_SIZE (8 * 1024)

#define MAD_AT_RESPONSE_OK "OK\r"
#define MAD_AT_RESPONSE_ERROR "ERROR\r"

typedef struct {
    pthread_t       thread;
    pthread_mutex_t mutex;
    pthread_cond_t  cond_var;
    bool            stop;
    bool            running;
    int             read_fd;
    int             write_fd;
    int             at_fd;
    char           *command;
    char            response[MAD_AT_RESPONSE_MAX_SIZE];
} thread_data_t;

static thread_data_t s_dispatch_thread = { 0,
                     PTHREAD_MUTEX_INITIALIZER,
                     PTHREAD_COND_INITIALIZER,
                     true,
                     true,
                     -1,
                     -1,
                     -1,
                     NULL,
{0}
                                         };

static void *dispatch_thread_runner(void *data);

static char *dispatch_command(char *command);

/* private thread methods */
static int thread_setup_socket(thread_data_t *data);
static int thread_write_command(thread_data_t *data);
static int thread_read_response(thread_data_t *data, long timeout);


int mad_at_handler_init()
{
    int result = -1;
    int pipe_fd[2];
    int flags;

    /*
     * Create a pipe that can be used to wake us up when we are waiting
     * for an event to occur on any of the monitored file descriptors.
     */
    if (pipe(pipe_fd) < 0) {
        MAD_LOG_E("pipe() failed, errno=%d", errno);
        goto exit;
    }

    s_dispatch_thread.read_fd = pipe_fd[0];
    flags = fcntl(s_dispatch_thread.read_fd, F_GETFD, 0);

    if (flags < 0 || fcntl(s_dispatch_thread.read_fd, F_SETFD, flags | FD_CLOEXEC) < 0) {
        MAD_LOG_E("fcntl error(%d)=\"%s\"", errno, strerror(errno));
        goto exit;
    }

    s_dispatch_thread.write_fd = pipe_fd[1];
    flags = fcntl(s_dispatch_thread.write_fd, F_GETFD, FD_CLOEXEC);

    if (flags < 0 || fcntl(s_dispatch_thread.write_fd, F_SETFD, flags | FD_CLOEXEC) < 0) {
        MAD_LOG_E("fcntl error(%d)=\"%s\"", errno, strerror(errno));
        goto exit;
    }

    flags = fcntl(s_dispatch_thread.write_fd, F_GETFL, 0);

    if (flags < 0 || fcntl(s_dispatch_thread.write_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        MAD_LOG_E("fcntl error(%d)=\"%s\"", errno, strerror(errno));
        goto exit;
    }

    if (pthread_mutex_lock(&s_dispatch_thread.mutex) < 0) {
        MAD_LOG_E("Failed to acquire mutex lock!");
        goto exit;
    }

    if (pthread_create(&(s_dispatch_thread.thread), NULL,
    dispatch_thread_runner, &s_dispatch_thread) < 0) {
        MAD_LOG_E("Failed to create reader thread!");
        goto exit;
    }

    /* Wait for thread to start */
    while (s_dispatch_thread.stop) {
        if (pthread_cond_wait(&s_dispatch_thread.cond_var, &s_dispatch_thread.mutex) < 0) {
            MAD_LOG_E("Failed to wait for condition variable!");
            break;
        }
    }

    if (!s_dispatch_thread.running) {
        MAD_LOG_E("Dispatch thread failed to start!");

        if (pthread_mutex_unlock(&s_dispatch_thread.mutex) < 0) {
            MAD_LOG_E("Failed to unlock mutex!");
        }

        goto exit;
    }

    MAD_LOG_D("Dispatch thread started");

    if (pthread_mutex_unlock(&s_dispatch_thread.mutex) < 0) {
        MAD_LOG_E("Failed to unlock mutex!");
        goto exit;
    }

    result = 0;

exit:
    return result;

}

int mad_at_handler_shutdown()
{
    int result = -1;

    if (pthread_mutex_lock(&s_dispatch_thread.mutex) < 0) {
        MAD_LOG_E("Failed to acquire mutex lock!");
        goto exit;
    }

    if (!s_dispatch_thread.running) {
        pthread_mutex_unlock(&s_dispatch_thread.mutex);
        MAD_LOG_E("Dispatch thread is not running!");
        goto exit;
    }

    s_dispatch_thread.stop = true;

    write(s_dispatch_thread.write_fd, ".", 1);

    if (pthread_mutex_unlock(&s_dispatch_thread.mutex) < 0) {
        MAD_LOG_E("Failed to unlock mutex!");
        goto exit;
    }

    if (pthread_join(s_dispatch_thread.thread, NULL) < 0) {
        MAD_LOG_E("Failed to join thread");
        goto exit;
    }

    result = 0;

exit:
    return result;
}

int mad_at_handler_send_txbo(const mad_epwrred_event_values_t event)
{

    int result;
    char *at_cmd_str = NULL;
    char *at_cmd_resp = NULL;

    result = asprintf(&at_cmd_str, "AT*EPWRRED=%d\r", event);

    if (result < 0) {
        MAD_LOG_E("Failed to allocate AT command string!");
        goto exit;
    }

    at_cmd_resp = dispatch_command(at_cmd_str);

    if (at_cmd_resp == NULL) {
        MAD_LOG_E("No response received");
        result = -1;
        goto exit;
    }

    MAD_LOG_D("Got response: %s %d", at_cmd_resp, strlen(at_cmd_resp));

    if (strncmp(MAD_AT_RESPONSE_OK, at_cmd_resp, strlen(MAD_AT_RESPONSE_OK) != 0)) {
        MAD_LOG_E("Got error response: %s", at_cmd_resp);
        result = -1;
    } else {
        result = 0;
    }

exit:
    free(at_cmd_str);
    free(at_cmd_resp);
    return result;
}

/* sending AT*EHSTACT Fast dormancy command to At core */
int mad_at_handler_send_FD(int event){

    int result;
    char *at_cmd_str = NULL;
    char *at_cmd_resp = NULL;

    result = asprintf(&at_cmd_str, "AT*EHSTACT=%d\r", event);
    MAD_LOG_E("Inside mad_at_handler_send_FD and the command is: %s", at_cmd_str);

    if (result < 0) {
        MAD_LOG_E("Failed to allocate AT command string!");
        goto exit;
    }

    at_cmd_resp = dispatch_command(at_cmd_str);

    if (at_cmd_resp == NULL) {
        MAD_LOG_E("No response received");
        result = -1;
        goto exit;
    }


    if (strncmp(MAD_AT_RESPONSE_OK, at_cmd_resp, strlen(MAD_AT_RESPONSE_OK) != 0)) {
        MAD_LOG_E("Got error response: %s", at_cmd_resp);
        result = -1;
    } else {
        result = 0;
    }

exit:
    free(at_cmd_str);
    free(at_cmd_resp);
    return result;
}


static char *dispatch_command(char *command)
{
    char *response = NULL;

    if (command == NULL) {
        MAD_LOG_E("Command was NULL!");
        goto exit;
    }

    if (pthread_mutex_lock(&s_dispatch_thread.mutex) < 0) {
        MAD_LOG_E("Failed to acquire mutex lock!");
        goto exit;
    }

    if (!s_dispatch_thread.running) {
        MAD_LOG_E("Dispatch thread is not running!");
        goto exit;
    }

    s_dispatch_thread.command = command;

    write(s_dispatch_thread.write_fd, ".", 1);

    while (s_dispatch_thread.running && s_dispatch_thread.command != NULL) {
        pthread_cond_wait(&s_dispatch_thread.cond_var, &s_dispatch_thread.mutex);
    }

    if (!s_dispatch_thread.running) {
        goto fatal_error;
    }

    if (asprintf(&response, "%s", s_dispatch_thread.response) < 0) {
        MAD_LOG_E("Failed to allocate response string: %s", strerror(errno));
    }

    goto exit;

fatal_error:
    MAD_LOG_E("Dispatch thread died. Connection to AT lost!");
    /* Nothing to do but quit and get restarted */
    util_mainloop_quit();

exit:
    pthread_mutex_unlock(&s_dispatch_thread.mutex);
    return response;
}

/* Thread functions */

static void *dispatch_thread_runner(void *data)
{
    thread_data_t *mydata = (thread_data_t *)data;
    fd_set readfds;
    int max_fd;
    int result;

    pthread_mutex_lock(&mydata->mutex);

    mydata->stop = false; /* start by clearing the stop flag. */

    /* Connect to at socket */
    if (thread_setup_socket(mydata) < 0) {
        MAD_LOG_E("Connecting to AT socket failed");
        pthread_mutex_unlock(&mydata->mutex);
        goto exit;
    }

    /* Start looping */
    while (!mydata->stop) {

        mydata->running = true;
        pthread_cond_signal(&mydata->cond_var); /* signal that we are ready */

        FD_ZERO(&readfds);
        FD_SET(mydata->at_fd, &readfds);
        FD_SET(mydata->read_fd, &readfds);

        max_fd = mydata->at_fd > mydata->read_fd ? mydata->at_fd : mydata->read_fd;

        MAD_LOG_D("Waiting for next command...");

        pthread_mutex_unlock(&mydata->mutex);

        do {
            result = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        } while (result < 0 && errno == EINTR);

        pthread_mutex_lock(&mydata->mutex);

        if (result < 0) {
            MAD_LOG_E("select failed: %s", strerror(errno));
            break;
        } else if (result > 0) {
            if (FD_ISSET(mydata->at_fd, &readfds)) {
                MAD_LOG_D("Got data on AT channel to read");
                if (thread_read_response(mydata, MAD_AT_COMMAND_TIMEOUT) < 0) {
                    MAD_LOG_E("Failed to read response from socket!");
                    break;
                }
                MAD_LOG_D("Read from AT channel: \"%s\"", mydata->response);
            }

            if (FD_ISSET(mydata->read_fd, &readfds)) {
                MAD_LOG_D("New command to send: \"%s\"", mydata->command);

                read(mydata->read_fd, mydata->response, sizeof(mydata->response));
                mydata->response[0] = '\0';

                if(mydata->stop == true){
                    MAD_LOG_I("Stop signal received");
                    break;
                }

                if (thread_write_command(mydata) < 0) {
                    MAD_LOG_E("Failed to write command to socket!");
                    break;
                }

                if (thread_read_response(mydata, MAD_AT_COMMAND_TIMEOUT) < 0) {
                    MAD_LOG_E("Failed to read response from socket!");
                    break;
                }
            }
        } else {
            MAD_LOG_E("select failed, timeout occurred!");
            break;
        }
    }

exit:
    mydata->running = false;
    close(mydata->read_fd);

    if (mydata->at_fd >= 0) {
        close(mydata->at_fd);
    }

    pthread_mutex_unlock(&mydata->mutex);
    pthread_cond_signal(&mydata->cond_var);
    return NULL;
}

static int thread_setup_socket(thread_data_t *data)
{
    struct sockaddr_un at_socket_addr;
    int fd = -1;
    int result;
    int flags;
    unsigned int retries = MAD_AT_CONNECT_MAX_RETRIES;

    fd = socket(PF_UNIX, SOCK_SEQPACKET, 0);

    if (fd < 0) {
        MAD_LOG_E("Failed to create socket: %s", strerror(errno));
        result = fd;
        goto exit;
    }

    memset(&at_socket_addr, 0, sizeof(struct sockaddr_un));

    at_socket_addr.sun_family = AF_UNIX;
    strncpy(at_socket_addr.sun_path, AT_SOCKET_PATH, UNIX_PATH_MAX);

    while (retries > 0) {
        result = connect(fd, (struct sockaddr *)&at_socket_addr, sizeof(struct sockaddr_un));

        if (result == 0) {
            MAD_LOG_D("Connected to AT Socket");

            flags = fcntl(fd, F_GETFL, 0);

            if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
                MAD_LOG_W("fcntl() returns error(%d)=\"%s\"", errno, strerror(errno));
            }
            break;
        } else if (errno == ECONNREFUSED) {
            struct timespec timeout;
            MAD_LOG_I("Failed to connect to AT Socket.  Retrying...");

            timeout.tv_sec = MAD_AT_CONNECT_RETRY_INTERVAL / 1000;
            timeout.tv_nsec = (MAD_AT_CONNECT_RETRY_INTERVAL % 1000) * 1000 * 1000;

            if (nanosleep(&timeout, NULL) < 0) {
                MAD_LOG_W("Thread got woken up unexpectedly");
            }

            if (--retries == 0) {
                MAD_LOG_E("Failed to connect to AT Socket. Giving up!");
            }
        } else {
            MAD_LOG_E("Failed to connect to AT Socket: %s", strerror(errno));
            goto error;
        }
    }

    /* Send a handshake */
    retries = MAD_AT_HANDSHAKE_MAX_RETRIES;
    data->at_fd = fd;
    data->command = "ATE0Q0V1\r";

    while (retries > 0) {
        if (thread_write_command(data) < 0
        || thread_read_response(data, MAD_AT_HANDSHAKE_TIMEOUT) < 0) {
            MAD_LOG_E("Failed to perform AT handshake! ");
        } else {
            MAD_LOG_D("AT Handshake performed!");
            break;
        }

        if (--retries == 0) {
            MAD_LOG_E("Max number of AT handshake tries reached.");
            result = -1;
        }
    }

    goto exit;

error:

    if (fd >= 0) {
        close(fd);
    }

exit:
    data->command = NULL;
    data->response[0] = '\0';
    return result;
}

static int thread_write_command(thread_data_t *data)
{
    struct msghdr msg;
    struct iovec data_vec[1];
    ssize_t result = 0;

    memset(&msg, 0, sizeof(struct msghdr));
    data_vec[0].iov_base = data->command;
    data_vec[0].iov_len = strlen(data->command);
    msg.msg_iov = data_vec;
    msg.msg_iovlen = 1;

    /*
     * clear the command from the data struct to
     * indicate the it has been processed.
     */
    data->command = NULL;

    do {
        result = sendmsg(data->at_fd, &msg, MSG_NOSIGNAL);
    } while (result < 0 && errno == EINTR);

    if (result <= 0) {
        MAD_LOG_E("Failed to send message: %s",
        result == 0 ? "Disconnected" : strerror(errno));
        return -1;
    }

    MAD_LOG_D("Sent message of size: %d", result);
    return 0;
}

static int thread_read_response(thread_data_t *data, long timeout)
{
    struct timeval tv, *tvp;
    ssize_t result = 0;
    fd_set readfds;
    struct msghdr msg;
    struct iovec data_vec[1];

    if (timeout < 0) {
        tvp = NULL;
    } else {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        tvp = &tv;
    }

    FD_ZERO(&readfds);
    FD_SET(data->at_fd, &readfds);

    do {
        result = select(data->at_fd + 1, &readfds, NULL, NULL, tvp);
    } while (result < 0 && errno == EINTR);

    if (result < 0) {
        MAD_LOG_E("select failed: %s", strerror(errno));
        goto exit;
    } else if (result == 0) {
        MAD_LOG_E("Timeout occured!");
        result = -1;
        goto exit;
    }

    if (!FD_SET(data->at_fd, &readfds)) {
        /* Shouldn't happen */
        MAD_LOG_E("select malfunctioned!");
        result = -1;
        goto exit;
    }

    memset(&msg, 0, sizeof(struct msghdr));
    memset(&data_vec, 0, sizeof(data_vec));

    data_vec[0].iov_base = data->response;
    data_vec[0].iov_len = MAD_AT_RESPONSE_MAX_SIZE;
    msg.msg_iov = data_vec;
    msg.msg_iovlen = 1;

    do {
        result = recvmsg(data->at_fd, &msg, MSG_NOSIGNAL);
    } while (result < 0 && errno == EINTR);

    if (result <= 0) {
        MAD_LOG_E("Failed to receive message: %s",
        result == 0 ? "Disconnected" : strerror(errno));
        result = -1;
        goto exit;
    }

    /* Make sure its null terminated */
    data->response[result] = '\0';

    MAD_LOG_D("Read response of length: %d", result);


exit:
    return result;

}

