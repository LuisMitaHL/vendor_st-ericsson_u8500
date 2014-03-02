/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#ifndef COPS_FD_H
#define COPS_FD_H

#include <sys/select.h>
#include <sys/types.h>

int cops_fcntl(int fd, int cmd, int arg);

int cops_select(int nfds, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout);

int cops_close(int fd);

ssize_t cops_write(int fd, const void *buf, size_t count);

ssize_t cops_read(int fd, void *buf, size_t count);

#endif /* COPS_FD_H */
