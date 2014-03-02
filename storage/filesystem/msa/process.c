/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include "process.h"

#include "convenience.h"
#include "fsa.h"
#include "log.h"
#include "msa.h"
#include "posix.h"
#include "state.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct atomicWrite {
	int         tempFd; //file descriptor for the open msaAtomicWriteFile
	const char *path; //Path of the true file we wanted to write atomicly to
} atomicWrite_t;

/* If a create file/directory request is sent from the client
   but the field for mode is set to MODE_IGNORE, then these
   are the modes that will be used. The protocol specification
   does not mention what mode to use. */
#define DEFAULT_CREATE_FILE_MODE S_IRWXU
#define DEFAULT_CREATE_DIR_MODE  S_IRWXU

static char msaRoot[PATH_MAX];
static size_t msaRootLen;

static char msaAtomicWriteFile[PATH_MAX];

static uint32_t mapToStatus(int theErrno)
{
	assert(theErrno != 0);

	switch (theErrno)
	{
		case EACCES:
		case EPERM:
		case ENXIO:
		case EBUSY:
			return ERR_ACCESS;

		case ENOENT:
		case EINVAL:
			return ERR_NOENT;

		case EIO:
		case EOVERFLOW:
		case EROFS:
		case EMFILE:
		case ENFILE:
		case ENOBUFS:
		case ENOMEM:
		case EPIPE:
		case EMLINK:
		case EXDEV:
			return ERR_IO;

		case EEXIST:
			return ERR_EXIST;

		case ENOTDIR:
			return ERR_NOTDIR;

		case EISDIR:
			return ERR_ISDIR;

		case EFBIG:
			return ERR_FBIG;

		case ENOSPC:
			return ERR_NOSPC;

		case ENAMETOOLONG:
		case ELOOP:
			return ERR_NAMETOOLONG;

		case ENOTEMPTY:
			return ERR_NOTEMPTY;

		case EBADF:
			return ERR_STALE;

		default:
			return ERR_IO;
	}
}

static fsa_stat_t makeAbsPath(
	char          *absPath,
	fsa_fhandle_t  handle,
	const char    *name,
	bool           pathShouldExist)
{
	char        pathBuf[PATH_MAX];
	const char *path;

	//Get path for the handle
	path = state_lookupHandle(handle);
	if ( unlikely(path == NULL) )
		return ERR_STALE; //Error already logged

	//'path' is already realpath():ed as we always run makAbsPath() on
	// anything we lookup and insert into the state

	if ( name ) //Add the name to the dir handle
	{
		int ret;

		//We do not need to check for '/' in the name. unpack_filename() which
		// should have been used before calling this function has already check
		// this for us.

		//Add 'name' to 'path', using 'pathBuf'
		ret = snprintf(pathBuf, sizeof(pathBuf), "%s/%s", path, name);
		if ( unlikely(ret < 0) )
		{
			logERROR("Can't make path of path='%s' and name='%s'. "
				"Output error.");
			return ERR_IO;
		}
		else if ( unlikely(ret >= PATH_MAX) )
		{
			logERROR("Can't make path of path='%s' and name='%s'. "
				"Path is too long.");
			return ERR_NAMETOOLONG;
		}
		path = pathBuf; //'path' is not in 'pathBuf'
	}

	if ( pathShouldExist )
	{
		//Copy the resolved path into absPath,
		// it will also check that the file (still) exists.
		if ( unlikely(!posix_realpath(path, absPath)) )
			return mapToStatus(errno); //Error already logged
	}
	else
	{
		//Copy the 'path' as is into 'absPath'
		strcpy(absPath, path);
	}

	//Check that we are within MSA server root
	if ( unlikely(strncmp(msaRoot, absPath, msaRootLen)) )
	{
		logERROR("Path='%s' is not within MSA root dir='%s'.",
			absPath, msaRoot);
		return ERR_NOENT;
	}

	return FSA_OK;
}

static inline fsa_stat_t getMeta(
	const char  *absPath,
	fsa_fattr_t *fattr)
{
	struct stat statinfo;

	if ( unlikely(posix_stat(absPath, &statinfo) < 0) )
		return mapToStatus(errno);

	linux2fsa_fattr(&statinfo, fattr);
	return FSA_OK;
}

static fsa_stat_t setMeta(
	const char       *absPath,
	const fsa_mode_t  fsaMode,
	const fsa_size_t  size,
	const fsa_time_t  secs,
	fsa_fattr_t      *fattr)
{
	fsa_stat_t status;
	fsa_fattr_t curfattr;

	//Mode first, we might need the permission for the rest of the opers
	if (fsaMode != MODE_IGNORE)
	{
		mode_t mode = fsa2linux_mode(fsaMode);
		if ( unlikely(posix_chmod(absPath, mode) < 0) )
		{
			status = mapToStatus(errno);
			goto End;
		}
	}

	//Ignore size field for anything but regular files
	if ( (status = getMeta(absPath, &curfattr)) != FSA_OK)
	{
		goto End;
	}
	if ( curfattr.type != FTYPE_REG )
	{
		logINFO("Not setting size to %llu bytes for directory '%s'",
			size, absPath);
	}
	else if (size != SIZE_IGNORE)
	{
		if ( unlikely(posix_truncate(absPath, size) < 0) )
		{
			status = mapToStatus(errno);
			goto End;
		}
	}

	if (secs != TIME_IGNORE)
	{
		struct utimbuf timebuf;

		timebuf.actime = secs;
		timebuf.modtime = secs;

		if ( unlikely(posix_utime(absPath, &timebuf) < 0) )
		{
			status = mapToStatus(errno);
			goto End;
		}
	}

	//Get new values
	status = getMeta(absPath, fattr);
End:
	return status;
}

static inline void atomicWrite_cleanup(void)
{
	(void) posix_unlink(msaAtomicWriteFile);
}

static int atomicWrite_start(atomicWrite_t *atomic, const char *path)
{
	struct stat stbuf;
	int pathFd, tempFd;
	ssize_t bytesRead, bytesWritten;
	char buf[BUFSIZ];
	int res;

	//save 'path' in 'atomic'
	atomic->path = path;

	//get stat of 'path'
	if ( unlikely(posix_stat(path, &stbuf) < 0) )
		return -errno; //Nothing to cleanup

	//open 'path' for read
	pathFd = posix_open(path, O_RDONLY, 0);
	if ( unlikely(pathFd < 0) )
		return -errno; //Nothing to cleanup

	//create 'temp' for write, with stat of 'path'
	tempFd = posix_open(msaAtomicWriteFile,
				O_WRONLY | O_CREAT, stbuf.st_mode);
	if ( unlikely(tempFd < 0) )
	{
		res = -errno;
		goto ErrorClosePath;
	}

	//copy 'path'  to 'temp'
	bytesRead = posix_read(path, pathFd, buf, BUFSIZ);	//First read()
	while ( bytesRead > 0 )
	{
		bytesWritten = posix_write(msaAtomicWriteFile,
							tempFd, buf, bytesRead);
		if ( unlikely(bytesWritten != bytesRead) )
		{
			res = -errno;
			goto ErrorCloseTemp; //write Error, error is already logged
		}

		bytesRead = posix_read(path, pathFd, buf, BUFSIZ); //Next read()
	}
	if ( unlikely(bytesRead < 0) )
	{
		res = -errno;
		goto ErrorCloseTemp; //read Error, error is already logged
	}


	//close 'path'
	if ( unlikely(posix_close(path, pathFd) < 0) )
	{
		res = -errno;
		goto ErrorCloseTemp;
	}

	//rewind 'temp', making the 'temp' looks like it was newly opened
	//we don't need to do this as only atomicWrite_pwrite should work with the
	// fd returned.

	//Save the params in 'atomic'
	atomic->tempFd = tempFd;
	atomic->path = path;

	//return success
	return 0;

ErrorCloseTemp:
	(void) posix_close(msaAtomicWriteFile, tempFd);
	(void) posix_unlink(msaAtomicWriteFile);

ErrorClosePath:
	(void) posix_close(path, pathFd);

	return res;
}

static ssize_t atomicWrite_pwrite(
	atomicWrite_t *atomic, unsigned char *buf, size_t nbyte, off_t offset)
{
	int res;
	ssize_t written = 0, ret;

	res = posix_lseek(msaAtomicWriteFile, atomic->tempFd, offset, SEEK_SET);
	if (unlikely(res < 0))
		return res;

	//Write as much data as possible (but no more than we want)
	do
	{
		ret = posix_write(msaAtomicWriteFile,
				atomic->tempFd, buf + written, nbyte);
		if ( unlikely(ret < 0) )
			goto Error;

		written += ret;
	} while ( ret > 0 && written < (ssize_t) nbyte );

	assert(written == (ssize_t) nbyte); //We did loop until all was written

	return written;

Error:
	//We need to close and remove the temp on failure
	(void) posix_close(msaAtomicWriteFile, atomic->tempFd);
	(void) posix_unlink(msaAtomicWriteFile);

	return ret;
}

static int atomicWrite_end(
	atomicWrite_t *atomic)
{
	int res;

	//close fd for 'temp'
	if ( unlikely(posix_close(msaAtomicWriteFile, atomic->tempFd) < 0) )
	{
		res = -errno;
		goto ErrorUnlinkTemp;
	}

	//rename 'temp' to 'path'
	// This is what makes atomicWrite atomic. rename() is the single atomic
	// operation that puts the write into its true place.
	if ( unlikely(posix_rename(msaAtomicWriteFile, atomic->path) < 0) )
	{
		res = -errno;
		goto ErrorUnlinkTemp;
	}

	return 0;

ErrorUnlinkTemp:
	(void) posix_unlink(msaAtomicWriteFile);

	return res;
}

//In counterpart of atomicWrite, we only have a normalRead.
static ssize_t normalRead_pread(
	const char *path, int fd, unsigned char *buf, size_t nbyte, off_t offset)
{
	int res;
	ssize_t bytesRead = 0, ret;

	res = posix_lseek(path, fd, offset, SEEK_SET);
	if (unlikely(res < 0))
		return res;

	//Read as much data as possible (but no more than we want)
	do
	{
		ret = posix_read(path, fd, buf + bytesRead, nbyte);
		if ( unlikely(ret < 0) )
			return ret;

		bytesRead += ret;
	} while ( ret > 0 && bytesRead < (ssize_t) nbyte );

	return bytesRead;
}


////
// Handlers of each FSA procedure
/////////////////////////////////////////////////////////////////

static void process_null(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	logREQ(FSA_PROC_NULL, "no params");

	//No need to parse the request. It has no body.
	NOTUSED(req);

	//Reponse has no body
	fill_PROC_NULL_resp(resp);

	logRESP(FSA_PROC_NULL, "no params");
}

static void process_getattr(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	const char    *path;

	//input
	fsa_fhandle_t  handle;

	//output
	fsa_stat_t     status;
	fsa_fattr_t    fattr;

	//parse request
	status = parse_PROC_GETATTR_req(req, &handle);
	if ( status != FSA_OK )
		goto Error;
	logREQ(FSA_PROC_GETATTR, "handle=%u", handle);

	//Get path for the handle
	path = state_lookupHandle(handle);
	if ( unlikely(path == NULL) )
	{
		status = ERR_STALE; //Error already logged
		goto Error;
	}
	logPATH(FSA_PROC_GETATTR, "handle=%u -> path='%s'",
		handle, path);

	//If the path doesn't exist anymore, getMeta() will notice
	status = getMeta(path, &fattr);
	if ( unlikely(status != FSA_OK) )
		goto Error;

	//fill response
	fill_PROC_GETATTR_resp(resp, status, &fattr);
	logRESP(FSA_PROC_GETATTR,
		"status=0 (FSA_OK) size=%llu mode=%#o ctime=%u,%u type=%u",
		fattr.size, fattr.mode,
		fattr.ctime.seconds, fattr.ctime.useconds, fattr.type);
	return;

Error:
	fill_PROC_GETATTR_resp(resp, status, NULL);
	logRESP(FSA_PROC_GETATTR, "status=%u (%s)");
}

static void process_setattr(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	const char  *path;
	fsa_sattr_t *sattr;

	//input
	fsa_sattrargs_t *sattrargs;

	//output
	fsa_stat_t  status;
	fsa_fattr_t fattr;

	//parse request
	status = parse_PROC_SETATTR_req(req, &sattrargs);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_PROC_SETATTR, "handle=%u size=%llu mode=%#o ctime=%u,%u",
		sattrargs->file, sattrargs->attributes.size, sattrargs->attributes.mode,
		sattrargs->attributes.ctime.seconds,
		sattrargs->attributes.ctime.useconds);

	//Get path for the handle
	path = state_lookupHandle(sattrargs->file);
	if ( unlikely(path == NULL) )
	{
		status = ERR_STALE; //Error already logged
		goto Error;
	}
	logPATH(FSA_PROC_SETATTR, "handle=%u -> path='%s'",
		sattrargs->file, path);

	//Set attributes
	sattr = &sattrargs->attributes;
	status = setMeta(path, sattr->mode, sattr->size,
				sattr->ctime.seconds, &fattr);
	if ( unlikely(status != FSA_OK) )
		goto Error;

	//fill response
	fill_PROC_SETATTR_resp(resp, status, &fattr);
	logRESP(FSA_PROC_SETATTR,
		"status=0 (FSA_OK) size=%llu mode=%#o ctime=%u,%u type=%u",
		fattr.size, fattr.mode,
		fattr.ctime.seconds, fattr.ctime.useconds, fattr.type);
	return;

Error:
	fill_PROC_SETATTR_resp(resp, status, NULL);
	logRESP(FSA_PROC_SETATTR, "status=%u (%s)",
		status, fsaStatusToString(status));
}

static void process_root(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	//output
	fsa_stat_t    status = FSA_OK;
	fsa_fhandle_t handle;
	fsa_fattr_t   fattr;

	//parse request, no body parsing not needed
	NOTUSED(req);
	logREQ(FSA_PROC_ROOT, "no params");


	//The root handle is always 1.
	handle = ROOT_HANDLE;

	//Check that root still exists, also fill fattr for response
	status = getMeta(msaRoot, &fattr);
	if ( unlikely(status != FSA_OK) )
		goto Error;

	//fill response
	fill_PROC_ROOT_resp(resp, status, handle, &fattr);
	logRESP(FSA_PROC_ROOT,
		"status=0 (FSA_OK) handle=%u size=%llu mode=%#o ctime=%u,%u type=%u",
		handle, fattr.size, fattr.mode,
		fattr.ctime.seconds, fattr.ctime.useconds, fattr.type);
	return;

Error:
	//fill response
	fill_PROC_ROOT_resp(resp, status, 0, NULL);
	logRESP(FSA_PROC_ROOT, "status=%u (%s)",
		status, fsaStatusToString(status));
}

static void process_lookup(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	char        absPath[PATH_MAX];

	//input
	fsa_fhandle_t dir;
	char		  name[FSA_NAMELEN + 1];

	//output
	fsa_stat_t    status;
	fsa_fhandle_t handle;
	fsa_fattr_t   fattr;

	//parse request
	status = parse_PROC_LOOKUP_req(req, &dir, name);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_PROC_LOOKUP, "dir=%u name='%s'",
		dir, name);

	status = makeAbsPath(absPath, dir, name, true);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logPATH(FSA_PROC_LOOKUP, "dir=%u + name='%s' -> absPath='%s'",
		dir, name, absPath);

	//Check the path, also needed for getting the meta info for the reply
	status = getMeta(absPath, &fattr);
	if ( unlikely(status != FSA_OK) )
		goto Error;

	//Need to insert it
	handle = state_insertPath(absPath);

	//fill response
	fill_PROC_LOOKUP_resp(resp, status, handle, &fattr);
	logRESP(FSA_PROC_LOOKUP, "status=0 (FSA_OK) handle=%u "
		"size=%llu, mode=%#o ctime=%u,%u type=%u",
		handle,
		fattr.size, fattr.mode,
		fattr.ctime.seconds, fattr.ctime.useconds, fattr.type);
	return;

Error:
	//fill response
	fill_PROC_LOOKUP_resp(resp, status, 0, NULL);
	logRESP(FSA_PROC_LOOKUP, "status=%u (%s)",
		status, fsaStatusToString(status));
}

static void process_read(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	fsa_stat_t   status;
	const char  *path;
	ssize_t      bytesRead;
	int          fd, ret;
	fsa_fattr_t *fattr;

	//input
	fsa_readargs_t *readargs;

	//output: All data is pointers that will be filled in with spots in the
	// reponse packet, for this function to fill in. This is because different
	// data needs to be read out in different order than packing requries.
	fsa_readres_t *readresSpot;
	uint8_t       *dataSpot;

	//parse request
	status = parse_PROC_READ_req(req, &readargs);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_READ_PROC, "handle=%u offset=%llu count=%u",
		readargs->file, readargs->offset, readargs->count);

	//Start the filling of the response to get the spots
	fill_PROC_READ_resp_start(resp, readargs->count,
		&readresSpot, &dataSpot);

	//Get path for the handle
	path = state_lookupHandle(readargs->file);
	if ( unlikely(path == NULL) )
	{
		status = ERR_STALE; //Error already logged
		goto Error;
	}
	logPATH(FSA_PROC_READ, "handle=%u -> path='%s'",
		readargs->file, path);

	//A. Open the file
	fd = posix_open(path, O_RDONLY, 0);
	if ( unlikely(fd < 0) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	//Start filling, everything is filled in backwards

	//fill 4. data     B. Read the file
	bytesRead = normalRead_pread(path, fd, dataSpot,
					readargs->count, readargs->offset);
	assert(bytesRead <= (ssize_t) readargs->count);
	if ( unlikely(bytesRead < 0) )
	{
		status = mapToStatus(errno);

		//Need to clean up, but don't save the errno/status
		(void) posix_close(path, fd);

		goto Error;
	}

	//fill 3. datalen
	readresSpot->data.len = bytesRead;

	//C. Close the file
	ret = posix_close(path, fd);
	if ( unlikely(ret < 0) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	//fill 2. fsa_fattr_t
	fattr = &readresSpot->attributes;
	status = getMeta(path, fattr );
	if ( unlikely(status != FSA_OK) )
		goto Error;

	//fill 1. status
	readresSpot->status = status;

	fill_PROC_READ_resp_end(resp, readresSpot->data.len);
	logRESP(FSA_PROC_READ, "status=0 (FSA_OK) "
		"size=%llu, mode=%#o ctime=%u,%u type=%u "
		"datalen=%u data=%p",
		fattr->size, fattr->mode, fattr->ctime.seconds, fattr->ctime.useconds,
		fattr->type, readresSpot->data.len, dataSpot);
	return;

Error:
	fill_PROC_READ_resp_failed(resp, status);
	logRESP(FSA_PROC_READ, "status=%u (%s) ",
			status, fsaStatusToString(status));
}

static void process_write(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	const char   *path;
	ssize_t       bytesWritten;
	int           fd, ret;
	atomicWrite_t atomic;
        atomic.tempFd = -1;

	//input
	fsa_writeargs_t *writeargs;

	//output
	fsa_stat_t  status;
	fsa_fattr_t fattr;

	status = parse_PROC_WRITE_req(req, &writeargs);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_PROC_WRITE, "handle=%u offset=%llu datalen=%u data=%p",
		writeargs->file, writeargs->offset,
		writeargs->data.len, writeargs->data.data);

	//Get path for the handle
	path = state_lookupHandle(writeargs->file);
	if ( unlikely(path == NULL) )
	{
		status = ERR_STALE; //Error already logged
		goto Error;
	}
	logPATH(FSA_PROC_WRITE, "handle=%u -> path='%s'",
		writeargs->file, path);

	//A. Open the file
	fd = atomicWrite_start(&atomic, path);
	if ( unlikely(fd < 0) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	//B. Write the file
	bytesWritten = atomicWrite_pwrite(&atomic, writeargs->data.data,
		writeargs->data.len, writeargs->offset);
	if ( unlikely(bytesWritten < 0) )
	{
		status = mapToStatus(errno);
		//No need to clean up, that is already done by atomicWrite_pwrite()
		goto Error;
	}

	//C. Close the file
	ret = atomicWrite_end(&atomic);
	if ( unlikely(ret < 0) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	status = getMeta(path, &fattr);
	if ( unlikely(status != FSA_OK) )
		goto Error;

	//fill response
	fill_PROC_WRITE_resp(resp, status, &fattr);
	logRESP(FSA_PROC_WRITE,
		"status=0 (FSA_OK) size=%llu mode=%#o ctime=%u,%u type=%u",
		fattr.size, fattr.mode,
		fattr.ctime.seconds, fattr.ctime.useconds, fattr.type);
	return;

Error:
	fill_PROC_WRITE_resp(resp, status, NULL);
	logRESP(FSA_PROC_WRITE, "status=%u (%s)",
		status, fsaStatusToString(status));
}

static void process_create(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	char absPath[PATH_MAX];
	mode_t mode;
	int fd, ret;

	//input
	fsa_fhandle_t  dir;
	char           name[FSA_NAMELEN + 1];
	fsa_sattr_t   *sattr;

	//output
	fsa_stat_t    status;
	fsa_fhandle_t handle;
	fsa_fattr_t   fattr;

	//parse request
	status = parse_PROC_CREATE_req(req, &dir, name, &sattr);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_PROC_CREATE, "dir=%u name='%s' size=%llu mode=%#x ctime=%u,%u",
		dir, name, sattr->size, sattr->mode,
		sattr->ctime.seconds, sattr->ctime.useconds);

	//Get path for the handle
	status = makeAbsPath(absPath, dir, name, false);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logPATH(FSA_PROC_CREATE, "dir=%u + name='%s' -> absPath='%s'",
		dir, name, absPath);

	//Create the file, see comment for DEFAULT_CREATE_FILE_MODE
	if (sattr->mode != MODE_IGNORE)
		mode = fsa2linux_mode(sattr->mode);
	else
		mode = DEFAULT_CREATE_FILE_MODE;
	fd = posix_creat(absPath, mode);
	if ( unlikely(fd < 0) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	//posix_creat() leaves us with an open fd. Close it.
	ret = posix_close(absPath, fd);
	if ( unlikely(ret < 0) )
	{
		status = mapToStatus(errno);
		goto Close;
	}

	//Apply sattr
	status = setMeta(absPath,
				MODE_IGNORE, //We aready set the mode in posix_creat()
				sattr->size == 0 ? SIZE_IGNORE : sattr->size,
				sattr->ctime.seconds,
				&fattr);
	if ( unlikely(status != FSA_OK) )
		goto Remove;

	//It is now fully created to specifications, insert into state, get handle
	handle = state_insertPath(absPath);

	//fill response
	fill_PROC_CREATE_resp(resp, status, handle, &fattr);
	logRESP(FSA_PROC_CREATE, "status=0 (FSA_OK) handle=%u "
		"size=%llu, mode=%#o ctime=%u,%u type=%u",
		handle,
		fattr.size, fattr.mode,
		fattr.ctime.seconds, fattr.ctime.useconds, fattr.type);
	return;

//Error handling below
Close:
	//Need to clean up, but don't save the errno/status
	(void) posix_close(absPath, fd);

Remove:
	//we need to remove the file if any operation after creat() failed
	(void) posix_unlink(absPath);

Error:

	//fill response
	fill_PROC_CREATE_resp(resp, status, 0, NULL);
	logRESP(FSA_PROC_CREATE, "status=%u (%s)",
		status, fsaStatusToString(status));
}

static void process_remove(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	char        absPath[PATH_MAX];

	//input
	fsa_fhandle_t  handle;
	char           name[FSA_NAMELEN + 1];

	//output
	fsa_stat_t     status;

	//parse request
	status = parse_PROC_REMOVE_req(req, &handle, name);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_PROC_REMOVE, "handle=%u name='%s'",
		handle, name);

	status = makeAbsPath(absPath, handle, name, true);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logPATH(FSA_PROC_REMOVE, "handle=%u + name='%s' -> absPath='%s'",
		handle, name, absPath);

	if ( unlikely(posix_unlink(absPath) < 0) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	/* QUESTION: The file has now been deleted. What if a client already creates
	a new file, is it ok that the handle is the same for the new file?

	ASSUMPTION: We assume that the client doesn't have a problem with a handle
	staying the same for an identical, recreated path. */

	/* PROBLEM: We have no delete operation in the state. */
	//state_delete(req->handle);

	//fill response
	fill_PROC_REMOVE_resp(resp, status);
	logRESP(FSA_PROC_REMOVE, "status=0 (FSA_OK)");
	return;

Error:
	//fill response
	fill_PROC_REMOVE_resp(resp, status);
	logRESP(FSA_PROC_REMOVE, "status=%u (%s)",
		status, fsaStatusToString(status));
}

static void process_rename(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	char           fromAbsPath[PATH_MAX];
	char           toAbsPath[PATH_MAX];

	//input
	fsa_fhandle_t  fromHandle;
	char           fromName[FSA_NAMELEN + 1];
	fsa_fhandle_t  toHandle;
	char           toName[FSA_NAMELEN + 1];

	//output
	fsa_stat_t     status;

	//parse request
	status = parse_PROC_RENAME_req(req, &fromHandle, fromName,
			&toHandle, toName);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_PROC_RENAME,
		"fromHandle=%u fromName='%s' toHandle=%u toName='%s'",
		fromHandle, fromName, toHandle, toName);

	status = makeAbsPath(fromAbsPath, fromHandle, fromName, true);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logPATH(FSA_PROC_RENAME,
		"fromHandle=%u + fromName='%s' -> fromAbsPath='%s'",
		fromHandle, fromName, fromAbsPath);

	//New file can exist, but we won't check for it. rename() below will handle
	// any errors related to that
	status = makeAbsPath(toAbsPath, toHandle, toName, false);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logPATH(FSA_PROC_RENAME,
		"toHandle=%u + toName='%s' -> toAbsPath='%s'",
		toHandle, toName, toAbsPath);

	if ( unlikely(posix_rename(fromAbsPath, toAbsPath) < 0) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	/* QUESTION: The path has now changed. What if a client already has a
	handle to the old name. Is the handle valid for the new name?

	ASSUMPTION: We assume that the client will PROC_LOOKUP for the handle of
	the new name, and thereby getting a new handle.
	*/

	/* TODO: We have no remove operation in the state.
	state_delete( (old)abspath);
	We don't really need one as keep caching the old path will yield the same
	handle.
	*/

	//fill response
	fill_PROC_RENAME_resp(resp, status);
	logRESP(FSA_PROC_RENAME, "status=0 (FSA_OK)");
	return;

Error:
	//fill response
	fill_PROC_RENAME_resp(resp, status);
	logRESP(FSA_PROC_RENAME, "status=%u (%s)",
		status, fsaStatusToString(status));
}

static void process_mkdir(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	char absPath[PATH_MAX];
	mode_t mode;

	//input
	fsa_fhandle_t  dir;
	char           name[FSA_NAMELEN + 1];
	fsa_sattr_t   *sattr;

	//output
	fsa_stat_t    status;
	fsa_fhandle_t handle;
	fsa_fattr_t   fattr;

	//parse request
	status = parse_PROC_MKDIR_req(req, &dir, name, &sattr);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_PROC_MKDIR, "dir=%u name='%s' size=%llu mode=%#o ctime=%u,%u",
		dir, name, sattr->size, sattr->mode, sattr->ctime.seconds,
		sattr->ctime.useconds);

	//New directory might exist, but we won't check for it. mkdir() below will
	// handle any errors related to that
	status = makeAbsPath(absPath, dir, name, false);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logPATH(FSA_PROC_MKDIR, "dir=%u + name='%s' -> absPath='%s'",
		dir, name, absPath);

	//Create directory and apply mode part of satte
	// see comment for DEFAULT_CREATE_DIR_MODE
	if (sattr->mode != MODE_IGNORE)
		mode = fsa2linux_mode(sattr->mode);
	else
		mode = DEFAULT_CREATE_DIR_MODE;
	if ( unlikely(posix_mkdir(absPath, mode) < 0) )
	{
		status = mapToStatus(errno);
		if (status == ERR_EXIST)
			goto Error;
		else
			goto Remove;
	}

	//We can't set size, but it should be set to ignore
	if ( unlikely(sattr->size != SIZE_IGNORE) )
	{
		logWARN("size=%llu in PROC_MKDIR is not set to ignore.",
			sattr->size);
	}

	//Apply rest of sattr
	status = setMeta(absPath,
				MODE_IGNORE, //We aready set the mode in posix_mkdir()
				SIZE_IGNORE, //Specifying a size for a directory makes no sense
				sattr->ctime.seconds,
				&fattr);
	if ( unlikely(status != FSA_OK) )
		goto Remove;

	//It is now fully created to specifications, insert into state, get handle
	handle = state_insertPath(absPath);

	//fill response
	fill_PROC_MKDIR_resp(resp, status, handle, &fattr);
	logRESP(FSA_PROC_MKDIR, "status=0 (FSA_OK) handle=%u "
		"size=%llu, mode=%#o ctime=%u,%u type=%u",
		handle,
		fattr.size, fattr.mode,
		fattr.ctime.seconds, fattr.ctime.useconds, fattr.type);
	return;

//Error handling below
Remove:
	//we need to remove the file if any operation after mkdir() failed
	(void) posix_rmdir(absPath);

Error:
	//fill response
	fill_PROC_MKDIR_resp(resp, status, 0, NULL);
	logRESP(FSA_PROC_MKDIR, "status=%u (%s)",
		status, fsaStatusToString(status));
}

static void process_rmdir(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	char        absPath[PATH_MAX];

	//input
	fsa_fhandle_t  handle;
	char           name[FSA_NAMELEN + 1];

	//output
	fsa_stat_t     status;

	//parse request
	status = parse_PROC_RMDIR_req(req, &handle, name);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_PROC_RMDIR, "handle=%u name='%s'",
		handle, name);

	status = makeAbsPath(absPath, handle, name, true);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logPATH(FSA_PROC_RMDIR, "handle=%u + name='%s' -> absPath='%s'",
		handle, name, absPath);

	if ( unlikely(posix_rmdir(absPath) < 0) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	/* QUESTION: The directory has now been deleted. What if a client already
	creates a new directory, is it ok that the handle is the same for the new
	directory?

	ASSUMPTION: We assume that the client doesn't have a problem with a handle
	staying the same for an identical, recreated path. */

	/* PROBLEM: We have no delete operation in the state. */
	//state_delete(req->handle);

	//fill response
	fill_PROC_RMDIR_resp(resp, status);
	logRESP(FSA_PROC_RMDIR, "status=0 (FSA_OK)");
	return;

Error:
	//fill response
	fill_PROC_RMDIR_resp(resp, status);
	logRESP(FSA_PROC_RMDIR, "status=%u (%s)",
		status, fsaStatusToString(status));
}

static void process_readdir(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	const char    *path;
	int            res, dirres;
	DIR           *dir;
	struct dirent  dirbuf;
	struct dirent *dirent = NULL;
	dirent_filler_t filler;

	//input
	fsa_fhandle_t  handle;
	uint32_t       count;
	uint32_t       cookie;

	//output
	fsa_stat_t status = FSA_OK; //Assume FSA_OK

	//parse request
	status = parse_PROC_READDIR_req(req, &handle, &cookie, &count);
	if ( unlikely(status != FSA_OK) )
		goto Error;
	logREQ(FSA_PROC_READDIR, "handle=%u cookie=%u count=%u",
		handle, cookie, count);

	//Start packing a response
	fill_PROC_READDIR_resp_start(resp, &filler, count);

	//Check for an invalid cookie.
	if ( unlikely(cookie == COOKIE_LAST) )
	{
		status = ERR_NOENT;
		goto Error;
	}

	//Get path for the handle
	path = state_lookupHandle(handle);
	if ( unlikely(path == NULL) )
	{
		status = ERR_STALE; //Error already logged
		goto Error;
	}

	logPATH(FSA_PROC_READDIR, "handle=%u -> path='%s'",
		handle, path);

	dir = posix_opendir(path);
	if ( unlikely(dir == NULL) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	//Main loop filling dirents
	while ( likely(filler.used <= filler.count) )
	{
		dirres = posix_readdir_r(path, dir, &dirbuf, &dirent);
		if (unlikely(dirres != 0) )
		{
			status = mapToStatus(dirres);
			goto Close;
		}

		//End of directory was encountered, exit loop
		if ( unlikely(dirent == NULL) )
			break;

		//If the current cookie has arrived at the requested starting cookie
		// then pack it
		if ( filler.currCookie >= cookie )
		{
			if ( !fill_PROC_READDIR_resp_dirent(resp, &filler, dirent) )
				break; //dirent didn't fit in response, but still keep it

			//dirent has been packed into response, so discard it
			dirent = NULL;
		}

		filler.currCookie++; //Update for the next entry
	}

	//filler.currCookie is now a counter of how many files
	// there was in the directory.

	//If the last dirent was packed into the response, then read one
	// more entry to check if the last packed entry was the final entry
	// in the directory or not. Note that the end of directory triggers
	// this test as well, but doing readdir() again in that case will
	// have dirent stay NULL for repeated calls, so this is ok.
	if ( likely(dirent == NULL) )
	{
		dirres = posix_readdir_r(path, dir, &dirbuf, &dirent);
		if (unlikely(dirres != 0) )
		{
			status = mapToStatus(dirres);
			goto Close;
		}
	}

	//If there is less number of entries in the directory than requested,
	// we are out of range.
	if ( unlikely(filler.currCookie < cookie) )
	{
		status = ERR_NOENT;
		goto Close;
	}

	if ( dirent == NULL && filler.cookiePtr != NULL )
	{
		//End of directory reached and at least one dirent was packed,
		// mark the last dirent as COOKIE_LAST
		*(filler.cookiePtr) = COOKIE_LAST;
		logDBG2("PROC_READDIR entry=%u is COOKIE_LAST", filler.currCookie - 1);
	}

	res = posix_closedir(path, dir);
	if ( unlikely( res < 0) )
	{
		status = mapToStatus(errno);
		goto Error;
	}

	fill_PROC_READDIR_resp_end(resp);

	logRESP(FSA_PROC_READDIR, "status=0 (FSA_OK) <dirent info not listed>");
	return;

//Error handling below
Close:
	(void) posix_closedir(path, dir); //Clean up, don't care about error

Error:
	fill_PROC_READDIR_resp_failed(resp, status);
	logRESP(FSA_PROC_READDIR, "status=%u (%s)",
		status, fsaStatusToString(status));
}


static void process_powerctrl(
	fsa_packet_t *req, fsa_packet_t *resp)
{
	fsa_stat_t status;

	//input
	bool restart;

	//parse request
	status = parse_PROC_POWERCTRL_req(req, &restart);
	if ( unlikely(status != FSA_OK) )
		goto End; //We can't tell about this error in a reply, simply ignore it
	logREQ(FSA_PROC_POWERCTRL, "restart=%u", restart);

	if (restart)
	{
		/* QUESTION: The client has been restarted and is asking MSA to discard
		all file handles. Is it ok to retain them anyway?

		ASSUMPTION: We assume that the client doesn't have a problem with all
		handles staying the same since PROC_LOOKUP will be sent to retrieve a
		handle for each path anyway and as long as there is no caching of
		handles going on in the client across a restart then there should not be
		any problem. */

		/* TODO: We have no state_cleanup() to clean out the state.
		Not really needed as the information always stays true and the state is
		always very small.
		*/

	}

End:
	//fill response
	fill_PROC_POWERCTRL_resp(resp);
	logRESP(FSA_PROC_POWERCTRL, "no params");
}

static void (*procedures[])(fsa_packet_t *, fsa_packet_t *) =
{
	process_null,
	process_getattr,
	process_setattr,
	process_root,
	process_lookup,
	process_read,
	process_write,
	process_create,
	process_remove,
	process_rename,
	process_mkdir,
	process_rmdir,
	process_readdir,
	process_powerctrl,
};

void process_request(fsa_packet_t *req, fsa_packet_t *resp)
{
	assert(req);
	assert(resp);

	fill_header(resp, req->header.transaction);

	procedures[req->header.procedure](req, resp);
}

void process_setRoot(const char *root)
{
	fsa_fhandle_t rootHandle;

	//msaRoot is already all set to null and will be null terminated
	strncpy(msaRoot, root, PATH_MAX - 1);
	msaRootLen = strlen(msaRoot);

	rootHandle = state_insertPath(msaRoot);
	assert(rootHandle == ROOT_HANDLE);

	logDBG2("Configured the MSA root to '%s'. Len=%u",
		msaRoot, msaRootLen);

	//msaAtomicWriteFile is already all set to null and will be null terminated
	strncpy(msaAtomicWriteFile, root, PATH_MAX - 1);
	if (root[strlen(root) - 1] != '/')
		strncat(msaAtomicWriteFile, "/", PATH_MAX - 1);
	strncat(msaAtomicWriteFile, MSA_WRITE_ATOMIC_FILE, PATH_MAX - 1);

	logDBG2("Configured the MSA write atomic file to '%s'.",
		msaAtomicWriteFile);
}

void process_cleanup(void)
{
	atomicWrite_cleanup();
}


