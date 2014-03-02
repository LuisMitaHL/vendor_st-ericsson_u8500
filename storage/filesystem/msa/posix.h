#ifndef _POSIX_H
#define _POSIX_H

/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include "log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/select.h>
#include <dirent.h>
#include <stddef.h>
#include <utime.h>
#include <pwd.h>
#include <grp.h>

#include <sys/socket.h>


/**********************************************
 * Functions
 **********************************************/

int posix_creat(const char *path, mode_t mode);
int posix_open(const char *path, int oflag, mode_t mode);
int posix_close(const char *path, int fd);
int posix_dup(const char *path, int fd);
DIR *posix_opendir(const char *dirname);
int posix_readdir_r(const char *path,
	DIR *dirp, struct dirent *entry, struct dirent **result);
int posix_closedir(const char *path, DIR *dirp);
void posix_rewinddir(const char *path, DIR *dirp);
off_t posix_lseek(const char *path, int fd, off_t offset, int whence);
int posix_select(const char *path,
	int nfds,
	fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
	struct timeval *timeout);
ssize_t posix_read(const char *path,
	int fd, void *buf, size_t nbyte);
ssize_t posix_write(const char *path,
	int fd, const void *buf, size_t nbyte);
int posix_rename(const char *old, const char *new);
int posix_unlink(const char *path);
int posix_link(const char *path, const char *newlink);
int posix_mkdir(const char *path, mode_t mode);
int posix_rmdir(const char *path);
int posix_stat(const char *path, struct stat *buf);
int posix_fstat(const char *path, int fd, struct stat *buf);
int posix_chdir(const char *path);
int posix_chmod(const char *path, mode_t mode);
int posix_chown(	const char *path, uid_t owner, gid_t group);
int posix_fchmod(const char *path, int fd, mode_t mode);
int posix_utime(const char *path, struct utimbuf *times);
int posix_truncate(const char *path, off_t length);
int posix_ftruncate(const char *path, int fd, off_t length);
char *posix_realpath(const char *path, char *resolved);
void posix_sync(void);
int posix_fsync(const char *path, int fd);
int posix_statfs(const char *path, struct statfs *buf);

int posix_socket(int domain, int type, int protocol);
int posix_connect(const char *name,
	int socket, const struct sockaddr *address, socklen_t address_len);
int posix_bind(const char *name,
	int sockfd, const struct sockaddr *adress, socklen_t address_len);
int posix_accept(const char *name,
	int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
int posix_listen(const char *name, int socket, int backlog);
ssize_t posix_sendmsg(const char *name,
	int socket, const struct msghdr *msg, int flags);
ssize_t posix_recvmsg(const char *name,
	int socket, struct msghdr *msg, int flags);

#endif // _POSIX_H
