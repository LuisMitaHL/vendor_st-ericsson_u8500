/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#ifndef COPS_PSOCK_H
#define COPS_PSOCK_H

#include <sys/select.h>
#include <sys/types.h>
#include <cops_socket.h>

int cops_psock_socket(bool server, struct cops_socket_data *data);

int cops_psock_bind(int sockfd, const char *path);

int cops_psock_listen(int sockfd, int backlog);

int cops_psock_accept(int sockfd);

int cops_psock_connect(int sockfd, const char *path);

int cops_psock_select(int ntds, fd_set *readfds, fd_set *writefds,
                      fd_set *exceptfds, const struct timeval* timeout);

int cops_psock_close(int fd);

ssize_t cops_psock_write(int fd, const void *buf, size_t count);

ssize_t cops_psock_read(int fd, void *buf, size_t count);

int cops_psock_errno(void);

const unsigned char *cops_psock_strerror(int errnum);

int cops_psock_error_is_eagain(int errnum);

#endif /* COPS_PSOCK_H */
