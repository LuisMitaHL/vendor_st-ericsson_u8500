/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include "posix.h"

#include "likely.h"
#include "log.h"

#include <sys/statfs.h>
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <dirent.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>

/* Maximum attempt upon failing and getting EINTR from POSIX */
#ifndef MAX_ATTEMPTS
#define MAX_ATTEMPTS 10
#endif // ifndef MAX_ATTEMPTS


#define LOG_OP_EXTRA(failed, myerrno, extra, fmt, args...) \
	do \
	{ \
		if (unlikely(failed)) \
			logERRNO(myerrno, fmt, ##args); \
		else \
		{ \
			logSUCC(fmt, ##args); \
			extra; \
		} \
	} while (0)

#define LOG_OP(failed, myerrno, fmt, args...) \
	LOG_OP_EXTRA(failed, myerrno, (void) 0, fmt, ##args)

#define LOOP_ON_EINTR(oper, failed, myerrno) \
	do { \
		bool interrupted; \
		unsigned attempt = 0; \
		do \
		{ \
			oper; \
			interrupted = (failed) && (myerrno) == EINTR; \
			if (unlikely(interrupted)) \
				logDBG3(#oper " was interrupted, looping again."); \
		} while (interrupted && ++attempt < MAX_ATTEMPTS); \
	} while (0)

//int creat(char *path, mode_t mode);
int posix_creat(const char *path, mode_t mode)
{
	int ret;
	LOOP_ON_EINTR(ret = creat(path, mode), ret < 0, errno);
	LOG_OP(ret < 0, errno,
			"creat(path='%s' mode=%#x) ret=%d",
			path, mode, ret);
	return ret;
}

//int open(char *path, int oflag, mode_t mode); We always use the 3 arguments version
int posix_open(const char *path, int oflag, mode_t mode)
{
	int ret;
	LOOP_ON_EINTR(ret = open(path, oflag, mode), ret < 0, errno);
	LOG_OP(ret < 0, errno,
			"open(path='%s' oflag=%#x mode=%#x) ret=%d",
			path, oflag, mode, ret);
	return ret;
}

//int close(int fd);
int posix_close(const char *path, int fd)
{
	int ret;
	LOOP_ON_EINTR(ret = close(fd), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"close(fd=%d) path='%s' ret=%d",
		fd, path, ret);
	return ret;
}

//int dup(int fildes);
int posix_dup(const char *path, int fd)
{
	int ret;
	LOOP_ON_EINTR(ret = dup(fd), ret < 0, errno);;
	LOG_OP(ret < 0, errno,
		"dup(fd=%d) path='%s'",
		fd, path);
	return ret;
}

//DIR *opendir(char *dirname);
DIR *posix_opendir(const char *dirname)
{
	DIR *dirp;
	dirp = opendir(dirname);
	LOG_OP(dirp == NULL, errno,
		"opendir(dirname='%s') dirp=%p",
		dirname, dirp);
	return dirp;
}

static inline void dbg_print_readdir(struct dirent *dirent)
{
	if ( dirent )
		logDBG3("readdir dirent: d_name='%s' d_ino=%lu",
			dirent->d_name, dirent->d_ino);
	else
		logDBG3("readdir dirent: NULL");
}

//int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
int posix_readdir_r(const char *path,
	DIR *dirp, struct dirent *entry, struct dirent **result)
{
	int ret;
	ret = readdir_r(dirp, entry, result);

	//readdir_r does not set errno, but returns error value instead
	LOG_OP_EXTRA(ret != 0, ret,
		dbg_print_readdir(*result),
		"readdir_r(dirp=%p entry=%p result=%p) path='%s' ret=%d",
		dirp, entry, *result, path, ret);
	return ret;
}

//int closedir(DIR *dirp);
int posix_closedir(const char *path, DIR *dirp)
{
	//this saves the address of the pointer without
	// keeping and passing the pointer itself to LOG_OP().
	ptrdiff_t ptradr = (char *) dirp - (char *) 0;
	int ret;

	ret = closedir(dirp);
	LOG_OP(ret < 0, errno,
		"closedir(dirp=0x%x) path='%s' ret=%d",
		ptradr, path, ret);
	return ret;
}

//void rewinddir(DIR *dirp);
void posix_rewinddir(const char *path, DIR *dirp)
{
	rewinddir(dirp);
	logSUCC("rewinddir(dirp=%p) path='%s'",
		dirp, path);
}


//off_t lseek(int fd, off_t offset, int whence);
off_t posix_lseek(const char *path, int fd, off_t offset, int whence)
{
	off_t ret;
	char whenceBuf[16];
	const char *whenceStr;

	switch (whence )
	{
		case SEEK_SET:
			whenceStr = "SEEK_SET";
			break;

		case SEEK_CUR:
			whenceStr = "SEEK_CUR";
			break;

		case SEEK_END:
			whenceStr = "SEEK_END";
			break;

		default:
			snprintf(whenceBuf, sizeof(whenceBuf), "%d", whence);
			whenceStr = whenceBuf;
	}

	ret = lseek(fd, offset, whence);
	LOG_OP(ret == (off_t) -1, errno,
		"lseek(fd=%d offset=%llu whence=%s) path='%s' ret=%lu",
		fd, (uint64_t) offset, whenceStr, path, ret);
	return ret;
}

//int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int posix_select(const char *path,
	int nfds,
	fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
	struct timeval *timeout)
{
	int ret;
	struct timeval tv;

	//select() updates timeout which is not permitted by POSIX.1-2001.
	// So copy the timeout and use the copy in the call.
	if (timeout != NULL) {
		tv = *timeout;
	}
	LOOP_ON_EINTR(ret = select(nfds, readfds, writefds, exceptfds, (timeout != NULL) ? &tv : NULL), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"select(nfds=%d readfds=%p writefds=%p exceptfds=%p timeout=%ld.%ld) path='%s' ret=%zd",
		nfds, readfds, writefds, exceptfds,
		(timeout != NULL) ? timeout->tv_sec : -1,
		(timeout != NULL) ? timeout->tv_usec : -1,
		path, ret);
	return ret;
}

//ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t posix_read(const char *path,
	int fd, void *buf, size_t nbyte)
{
	ssize_t ret;
	LOOP_ON_EINTR(ret = read(fd, buf, nbyte), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"read_packet(fd=%d buf=%p nbyte=%zu) path='%s' ret=%zd",
		fd, buf, nbyte, path, ret);
	return ret;
}

//ssize_t write(int fildes, const void *buf, size_t nbyte);
ssize_t posix_write(const char *path,
	int fd, const void *buf, size_t nbyte)
{
	ssize_t ret;
	LOOP_ON_EINTR(ret = write(fd, buf, nbyte), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"write_packet(fd=%d buf=%p nbyte=%zu) path='%s' ret=%zd",
		fd, buf, nbyte, path, ret);
	return ret;
}

//int rename(char *old, char *new);
int posix_rename(const char *old, const char *new)
{
	int ret;
	ret = rename(old, new);
	LOG_OP(ret < 0, errno,
		"rename(old='%s' new='%s') ret=%d",
		old, new, ret);
	return ret;
}

//int unlink(char *path);
int posix_unlink(const char *path)
{
	int ret;
	ret = unlink(path);
	LOG_OP(ret < 0, errno,
		"unlink(path='%s') ret=%d",
		path, ret);
	return ret;
}

//int link(const char *path1, const char *path2);
int posix_link(const char *path, const char *newlink)
{
	int ret;
	ret = link(path, newlink);
	LOG_OP(ret < 0, errno,
		"link(path='%s' newlink='%s') ret=%d",
		path, newlink, ret);
	return ret;
}

//int mkdir(char *path, mode_t mode);
int posix_mkdir(const char *path, mode_t mode)
{
	int ret;
	ret = mkdir(path, mode);
	LOG_OP(ret < 0, errno,
		"mkdir(path='%s' mode=%#x) ret=%d",
		path, mode, ret);
	return ret;
}

//int rmdir(char *path);
int posix_rmdir(const char *path)
{
	int ret;
	ret = rmdir(path);
	LOG_OP(ret < 0, errno,
		"rmdir(path='%s') ret=%d",
		path, ret);
	return ret;
}

static inline void dbg_print_stat(const char *path, struct stat *buf)
{
	logDBG3("stat path='%s' "
		"st_dev=%llu st_ino=%llu st_mode=%#x st_nlink=%u st_uid=%u st_gid=%u "
		"st_rdev=%llu st_size=%llu st_atime=%lu st_mtime=%lu st_ctime=%lu "
		"st_blksize=%llu st_blocks=%llu",
		path,
		(uint64_t) buf->st_dev, (uint64_t) buf->st_ino, (uint32_t) buf->st_mode,
		(uint32_t) buf->st_nlink,
		(uint32_t) buf->st_uid, (uint32_t) buf->st_gid,
		(uint64_t) buf->st_rdev, (uint64_t) buf->st_size,
		(uint32_t) buf->st_atime, (uint32_t) buf->st_mtime,
		(uint32_t) buf->st_ctime,
		(uint64_t) buf->st_blksize, (uint64_t) buf->st_blocks);
}

//int stat(char *path, struct stat *buf)
int posix_stat(const char *path, struct stat *buf)
{
	int ret;
	ret = stat(path, buf);
	LOG_OP_EXTRA(ret < 0, errno,
		dbg_print_stat(path, buf),
		"stat(path='%s' buf=%p) ret=%d",
		path, buf, ret);
	return ret;
}

//int fstat(int fildes, struct stat *buf);
int posix_fstat(const char *path,
	int fd, struct stat *buf)
{
	int ret;
	ret = fstat(fd, buf);
	LOG_OP_EXTRA(ret < 0, errno,
		dbg_print_stat(path, buf),
		"fstat(fd=%d buf=%p) path='%s' ret=%d",
		fd, buf, path, ret);
	return ret;
}

//int chdir(const char *path);
int posix_chdir(const char *path)
{
	int ret;
	LOOP_ON_EINTR(ret = chdir(path), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"chdir(path='%s') ret=%d",
		path, ret);
	return ret;
}

//int chmod(char *path, mode_t mode);
int posix_chmod(const char *path, mode_t mode)
{
	int ret;
	LOOP_ON_EINTR(ret = chmod(path, mode), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"chmod(path='%s' mode=%#o) ret=%d",
		path, mode, ret);
	return ret;
}

//int chown(const char *path, uid_t owner, gid_t group);
int posix_chown(	const char *path, uid_t owner, gid_t group)
{
	int ret;
	LOOP_ON_EINTR(ret = chown(path, owner, group), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"chown(path='%s' owner=%d group=%d) ret=%d",
		path, owner, group, ret);
	return ret;
}

//int fchmod(int fildes, mode_t mode);
int posix_fchmod(const char *path, int fd, mode_t mode)
{
	int ret;
	LOOP_ON_EINTR(ret = fchmod(fd, mode), ret < 0, errno);;
	LOG_OP(ret < 0, errno,
		"fchmod(fd=%d mode=%#o) path='%s' ret=%d",
		fd, mode, path, ret);
	return ret;
}

//int utime(char *path, struct utimbuf *times);
int posix_utime(const char *path, struct utimbuf *times)
{
	int ret;
	ret = utime(path, times);
	LOG_OP(ret < 0, errno,
		"utime(path='%s' times=%p) times->actime=%lu times->modtime=%lu ret=%d",
		path, times, times->actime, times->modtime, ret);
	return ret;
}

//int int ftruncate(int fd, off_t length)
int posix_ftruncate(const char *path, int fd, off_t length)
{
	ssize_t ret;
	LOOP_ON_EINTR(ret = ftruncate(fd, length), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"ftruncate(fd=%d length=%u) path='%s' ret=%d",
		fd, (unsigned) length, path, ret);
	return ret;
}

//int truncate(char *path, off_t length);
int posix_truncate(const char *path, off_t length)
{
	int ret;
	int fd;

	/* Androids Bionic is lacking truncate(), so open
	   file and use ftruncate() */
	fd = posix_open(path, O_WRONLY, 0);
	if (unlikely(fd < 0))
		return fd;

	ret = posix_ftruncate(path, fd, length);
	if (unlikely(ret < 0))
		return ret;

	return posix_close(path, fd);
}

//char *realpath(const char *path, char *resolved_path)
char *posix_realpath(const char *path, char *resolved)
{
	char *ret;
	ret = realpath(path, resolved);
	LOG_OP(ret == NULL, errno,
		"realpath(path='%s', resolved='%s') ret=%p",
		path, resolved, ret);
	return ret;
}

//void sync(void);
void posix_sync(void)
{
	sync(); //Always successful
	logSUCC("sync()");
}

//int fsync(int fildes);
int posix_fsync(const char *path,
	int fd)
{
	int ret;
	LOOP_ON_EINTR(ret = fsync(fd), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"fsync(fd=%d) path='%s' ret=%d",
		fd, path, ret);
	return ret;
}

static inline void dbg_print_statfs(
	const char *path, const struct statfs *buf)
{
	logDBG3("statfs path='%s' "
		"f_type=%ld f_bsize=%ld f_blocks=%ld f_bfree=%ld f_bavail=%ld "
		"f_files=%ld f_ffree=%ld f_fsid=%u f_namelen=%ld",
		path,
		buf->f_type, buf->f_bsize, buf->f_blocks,
		buf->f_bfree, buf->f_bavail,
		buf->f_files, buf->f_ffree,
		buf->f_fsid, buf->f_namelen);
}

//int statfs(const char *path, struct statfs *buf);
int posix_statfs(const char *path, struct statfs *buf)
{
	int ret;
	LOOP_ON_EINTR(ret = statfs(path, buf), ret < 0, errno);
	LOG_OP_EXTRA(ret < 0, errno,
		dbg_print_statfs(path, buf),
		"statfs(path='%s' buf=%p) ret=%d",
		 path, buf, ret);
	return ret;
}

//int socket(int domain, int type, int protocol);
int posix_socket(int domain, int type, int protocol)
{
	const char *domainStr = "unknown";
	const char *typeStr = "unknown";
	const char *protocolStr = "unknown";
	int ret;

	switch (domain)
	{
		case AF_INET:
			domainStr = "AF_INET";
			break;
		case AF_INET6:
			domainStr = "AF_INET6";
			break;
		case AF_UNIX:
			domainStr = "AF_UNIX";
			break;
		default:
			domainStr = "AF_UNKNOWN";
			break;
	}

	switch (type)
	{
		case SOCK_DGRAM:
			typeStr = "SOCK_DGRAM";
			break;
		case SOCK_RAW:
			typeStr = "SOCK_RAW";
			break;
		case SOCK_SEQPACKET:
			typeStr = "SOCK_SEQPACKET";
			break;
		case SOCK_STREAM:
			typeStr = "SOCK_STREAM";
			break;
		default:
			typeStr = "SOCK_UNKNOWN";
			break;
	}

	ret = socket(domain, type, protocol);
	LOG_OP(ret < 0, errno,
			"socket(domain=%d(%s) type=%d(%s) protocol=%d(%s)) ret=%d",
			domain, domainStr, type, typeStr, protocol, protocolStr, ret);

	return ret;
}

//int connect(int socket, const struct sockaddr *address, socklen_t address_len);
int posix_connect(const char *name,
int sockfd, const struct sockaddr *address, socklen_t address_len)
{
	int ret;
	LOOP_ON_EINTR(ret = connect(sockfd, address, address_len), ret < 0, errno);
	LOG_OP(ret < 0, errno,
			"connect(sockfd=%d address=%p address->sa_family=%u "
			"address_len=%zu) name='%s' ret=%d",
			sockfd, address, address->sa_family, address_len, name, ret);
	return ret;
}

// int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
int posix_bind(const char *name,
int sockfd, const struct sockaddr *address, socklen_t address_len)
{
	int ret;
	ret = bind(sockfd, address, address_len);
	LOG_OP(ret < 0, errno,
			"bind(sockfd=%d address=%p address->sa_family=%u "
			"address_len=%zu) name='%s' ret=%d",
			sockfd, address, address->sa_family, address_len, name, ret);
	return ret;
}

//int listen(int socket, int backlog);
int posix_listen(const char *name, int sockfd, int backlog)
{
	int ret;
	ret = listen(sockfd, backlog);
	LOG_OP(ret < 0, errno,
		"listen(sockfd=%d backlog=%d) name='%s' ret=%d",
		sockfd, backlog, name, ret);
	return ret;
}

//int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
int posix_accept(const char *name,
	int sockfd, struct sockaddr *restrict address,
	socklen_t *restrict address_len)
{
	int ret;
	LOOP_ON_EINTR(ret = accept(sockfd, address, address_len), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"accept(sockfd=%d address=%p address_len=%p) len=%d name='%s' ret=%d",
		sockfd, address, address_len, *address_len, name, ret);
	return ret;
}

static inline void dbg_print_msghdr(const struct msghdr *msg)
{
	if ( msg->msg_iov )
		logDBG3(
			"msg_name=%p msg_namelen=%u msg_iov=%p msg_iovlen=%zu msg_flags=%x "
			"msg_iov[0].iov_base=%p msg_iov[0].iov_len=%zu",
			msg->msg_name, msg->msg_namelen,
			msg->msg_iov, msg->msg_iovlen,
			msg->msg_flags,
			msg->msg_iov[0].iov_base, msg->msg_iov[0].iov_len);
	else
		logDBG3(
			"msg_name=%p msg_namelen=%u msg_iov=%p msg_iovlen=%zu msg_flags=%x",
			msg->msg_name, msg->msg_namelen,
			msg->msg_iov, msg->msg_iovlen,
			msg->msg_flags);
}

//ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
ssize_t posix_sendmsg(const char *name,
	int sockfd, const struct msghdr *msg, int flags)
{
	ssize_t ret;
	assert(msg != NULL);
	LOOP_ON_EINTR(ret = sendmsg(sockfd, msg, flags), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"sendmsg(sockfd=%d msghdr=%p flags=%x) name='%s' ret=%d ",
		sockfd, msg, flags, name, ret);
	dbg_print_msghdr(msg);
	return ret;
}

//ssize_t recvmsg(int socket, struct msghdr *message, int flags);
ssize_t posix_recvmsg(const char *name,
	int sockfd, struct msghdr *msg, int flags)
{
	ssize_t ret;
	assert(msg != NULL);
	LOOP_ON_EINTR(ret = recvmsg(sockfd, msg, flags), ret < 0, errno);
	LOG_OP(ret < 0, errno,
		"recvmsg(sockfd=%d msghdr=%p flags=%x) name='%s' ret=%d ",
		sockfd, msg, flags, name, ret);
	dbg_print_msghdr(msg);
	return ret;
}

