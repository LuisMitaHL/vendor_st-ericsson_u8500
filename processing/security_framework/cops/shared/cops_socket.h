/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#ifndef COPS_SOCKET_H
#define COPS_SOCKET_H

#include <stdbool.h>

#define COPS_PATH_MAX 108

enum cops_socket_state {
    COPS_SOCKET_STATE_UNUSED = 0,
    COPS_SOCKET_STATE_WAITING,
    COPS_SOCKET_STATE_INITIATED,
    COPS_SOCKET_STATE_READY
};

struct cops_socket_data {
    int listen_fd;
    int request_fd;
    enum cops_socket_state state;
    void *aux;
};

int cops_socket(bool server, struct cops_socket_data *data);

int cops_bind(int sockfd, const char *path);

int cops_listen(int sockfd, int backlog);

int cops_accept(int sockfd);

int cops_connect(int sockfd, const char *path);

#endif /* COPS_SOCKET_H */
