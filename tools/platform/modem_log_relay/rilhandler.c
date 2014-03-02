/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>

#include "utils.h"
#include "rilhandler.h"

static int sock_ril_server = -1;
static int sock_ril_client = -1;

/**
 * @brief Opens a RIL server socket.
 *
 * @param [in] port Port to open socket.
 *
 * @return If successful, returns 0; otherwise it returns -1 and sets errno to indicate the error.
 */
int open_ril_server(int port)
{
    int ret;
    int yes = 1;
    struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    sock_ril_server = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_ril_server < 0) {
        ALOGE("Unable to create RIL socket, %d", errno);
        return -1;
    }

    setsockopt(sock_ril_server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    ret = bind(sock_ril_server, (struct sockaddr *) &server_address, sizeof(struct sockaddr));

    if (ret < 0) {
        ALOGE("bind error: RIL server socket, errno = %d", errno);
        return -1;
    }

    ret = listen(sock_ril_server, 1);

    if (ret < 0) {
        ALOGE("listen error: RIL server socket");
        return -1;
    }

    ALOGD("***** Server (ril_connection_handler) port=%d Waiting for client... *****", port);
    return 0;
}

/**
 * @brief Accept to connect RIL client.
 *
 * @return If successful, returns 0; otherwise it returns -1.
 */
int connect_client()
{
    if (sock_ril_server < 0) {
        ALOGE("RIL Server : Invalid server socket");
        return -1;
    }

    // Connect to the client
    if ((sock_ril_client = accept(sock_ril_server, NULL, 0)) < 0) {
        ALOGE("RIL Server : Unable to accept client connection");
        return -1;
    }

    ALOGI("RIL Client connected");
    return 0;
}

/**
 * @brief Receive message from client socket.
 *
 * @param [in] message Message received from client.
 *
 * @return If successful, returns number of bytes received; otherwise it returns -1.
 */
int receive_from_ril(char *message)
{
    int ril_fd, no_bytes = -1;
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(sock_ril_client, &readfds);
    ril_fd = select(sock_ril_client + 1, &readfds, NULL, NULL, NULL);

    if ((ril_fd < 0) && (errno != EINTR)) {
        ALOGW("receive_from_ril : select error");
        return -1;
    }

    if (FD_ISSET(sock_ril_client, &readfds)) {
        no_bytes = recv(sock_ril_client, message, RIL_MESSAGE_SIZE - 1, 0);
        EXTRADEBUG("Message received from RIL : %s, %d", message, no_bytes);
    }

    return no_bytes;
}

/**
 * @brief Send message to client socket.
 *
 * @param [in] message Message to be send to client.
 *
 * @return If successful, returns number of bytes sent; otherwise it returns -1.
 */
int send_to_ril(char *message)
{
    int no_bytes;

    if ((sock_ril_client < 0) || (message == NULL)) {
        ALOGE("RIL Server : No client connected or invalid client socket");
        return -1;
    }

    no_bytes = send(sock_ril_client, message, strlen(message), 0);

    if (no_bytes < 0) {
        ALOGE("Failed to send message to RIL, errno %d", errno);
    }

    EXTRADEBUG("Message sent to RIL %s, %d", message, no_bytes);
    return no_bytes;
}

/**
 * @brief Disconnect RIL client socket.
 *
 */
void disconnect_client()
{
    if (sock_ril_client > 0) {
        close(sock_ril_client);
        sock_ril_client = -1;
    }
}

/**
 * @brief Disconnect RIL server socket.
 *
 */
void close_ril_server()
{
    if (sock_ril_server > 0) {
        close(sock_ril_server);
        sock_ril_server = -1;
    }
}
