/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include "cops_socket.h"
#ifdef COPS_OSE_ENVIRONMENT
#include "cops_psock.h"
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <string.h>
#endif

int cops_socket(bool server, struct cops_socket_data *data)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_socket(server, data);
#else
    (void) server;
    (void) data;
    return socket(AF_UNIX, SOCK_STREAM, 0);
#endif
}

int cops_bind(int sockfd, const char *path)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_bind(sockfd, path);
#else
    size_t plen;
    socklen_t slen;
    struct sockaddr_un saun;

    plen = strlen(path);
    memset(&saun, 0, sizeof(saun));
    saun.sun_family = AF_UNIX;
    memcpy(saun.sun_path, path, plen);  /* The terminating '\0' isn't needed */
    slen = offsetof(struct sockaddr_un, sun_path) + plen;

    return bind(sockfd, (const struct sockaddr *)&saun, slen);
#endif
}

int cops_listen(int sockfd, int backlog)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_listen(sockfd, backlog);
#else
    return listen(sockfd, backlog);
#endif
}

int cops_accept(int sockfd)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_accept(sockfd);
#else
    socklen_t slen;
    struct sockaddr_un saun;

    slen = sizeof(saun);
    return accept(sockfd, (struct sockaddr *)&saun, &slen);
#endif
}

int cops_connect(int sockfd, const char *path)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_connect(sockfd, path);
#else
    size_t plen;
    socklen_t slen;
    struct sockaddr_un saun;

    plen = strlen(path);
    memset(&saun, 0, sizeof(saun));
    saun.sun_family = AF_UNIX;
    memcpy(saun.sun_path, path, plen);  /* The terminating '\0' isn't needed */
    slen = offsetof(struct sockaddr_un, sun_path) + plen;

    return connect(sockfd, (const struct sockaddr *)&saun, slen);
#endif
}
