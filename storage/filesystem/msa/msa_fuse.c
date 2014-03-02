/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include <assert.h>
#include <errno.h>
#include <libgen.h> //dirname() and basename()
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/un.h>


#define FUSE_USE_VERSION 26
#include <fuse.h>

#include "convenience.h"
#include "fsa.h"
#include "log.h"
#include "msa.h"
#include "posix.h"
#include "test.h"
#include "testclient.h"

#define MSAFUSE_VERSION "1.0"

//This funtion will only work if msaFuse_init() has been executed
static MsaClientEnv_t *getEnv(void)
{
	struct fuse_context *ctx;
	MsaClientEnv_t   *env;

	ctx = fuse_get_context();
	CHECK_NOT_EQUAL(ctx, NULL);

	env = ctx->private_data;
	CHECK_NOT_EQUAL(env, NULL);

	return env;
}

static void *msaFuse_init(struct fuse_conn_info *conn)
{
	MsaClientEnv_t       *env;

	logDBG2("msaFuse_init");

	NOTUSED(conn);

	env = easy_malloc(sizeof(MsaClientEnv_t));

	//Connect to MSA server
	env->socket = connectToMsa();

	//Just test to ping the MSA server
	ping(env);

	//Send PROC_ROOT request
	{
		fsa_packet_t    req, resp;
		fsa_diropres_t *diropres;
		fsa_stat_t		status;

		fsa_allocpacket(&req);
		fsa_allocpacket(&resp);

		fill_PROC_ROOT_req(&req);
		sendAndReceive(env, &req, &resp);
		status = parse_PROC_ROOT_resp(&resp, &diropres);

		CHECK_EQUAL(status, FSA_OK);

		env->root_handle = diropres->dirop_ok.file;

		fsa_freepacket(&req);
		fsa_freepacket(&resp);
	}

	return env;
}


static void msaFuse_destroy(void *arg)
{
	struct fsa_packet req;
	struct fsa_packet resp;
	MsaClientEnv_t *env = getEnv();

	logDBG2("msaFuse_destroy");

	NOTUSED(arg);

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_POWERCTRL_req(&req, true);
	sendAndReceive(env, &req, &resp);

	//There should be no body added to the response
	CHECK_EQUAL(resp.bodylen, 0);

	fsa_freepacket(&req);
	fsa_freepacket(&resp);

	logINFO("Successfully disconnected.");
}


static int msaFuse_getattr(const char *path, struct stat *stbuf)
{
	fsa_packet_t req, resp;
	fsa_attrstat_t *attrstat;

	fsa_fhandle_t handle;
	MsaClientEnv_t *env = getEnv();

	fsa_stat_t status;

	logDBG2("msaFuse_getattr path='%s'", path);

	status = lookuppath(env, path, &handle);
	if ( unlikely(status != FSA_OK) )
		goto End;

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_GETATTR_req(&req, handle);
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_GETATTR_resp(&resp, &attrstat);
	if ( unlikely(status != FSA_OK) )
		goto SentGETATTR;

	memset(stbuf, 0, sizeof(struct stat));
	if ( likely(attrstat->status == FSA_OK) )
	{
		stbuf->st_size = attrstat->attributes.size;
		stbuf->st_ctime = stbuf->st_atime = stbuf->st_mtime =
			attrstat->attributes.ctime.seconds;

		stbuf->st_mode = fsa2linux_mode(attrstat->attributes.mode);
		stbuf->st_mode |= fsa2linux_ftype(attrstat->attributes.type);
	}
	else
	{
		status = attrstat->status;
	}

SentGETATTR:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

End:
	logDBG3("msaFuse_getattr status=%u", status);

	return -maptoerrno(status);
}

static int msaFuse_opendir(const char *path, struct fuse_file_info *fi)
{
	MsaClientEnv_t *env = getEnv();
	fsa_fhandle_t   handle;
	fsa_stat_t     status;

	logDBG2("msaFuse_opendir path='%s'", path);

	//Perform lookup and save handle
	status = lookuppath(env, path, &handle);

	if ( status != FSA_OK )
	{
		fi->fh = (unsigned long) -1; //Error
		return -maptoerrno(status);
	}

	logDBG2("msaFuse_opendir path='%s' -> handle=%u",
		path, handle);
	fi->fh = (unsigned long) handle;

	return -maptoerrno(status);
}

static int msaFuse_readdir(const char *path,
	void *fillerBuf,
	fuse_fill_dir_t filler,
	off_t offset,
	struct fuse_file_info *fi)
{
	MsaClientEnv_t *env = getEnv();
	fsa_fhandle_t	  handle = (fsa_fhandle_t) fi->fh;
	fsa_stat_t        status;
	uint32_t          count;

	fsa_packet_t      req, resp;

	logDBG2("msaFuse_readdir path='%s' handle=%u offset=%u", path, handle, offset);

	if ( offset == INT_MAX || offset == COOKIE_LAST )
	{
		logDBG1("msaFuse_readdir offset=COOKIE_LAST. Returning.");
		/* BUG: return 0;
		Returning 0 here would be the most correct. There is no more entires,
		we never call filler(). However, if we do this, then seekdir won't work
		more. FUSE won't call us for older entires any more.
		*/
		return -ENOENT; //No more entries
	}

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	//Readdir requests must be at minimum 24 bytes (i.e. 6 words).
	// 128 bytes (i.e. 32 words) has been chosen as an arbitrary
	// maximum. Make sure that count is a multiple of 4 bytes.
	count = 6 + (rand() % (32 - 6));
	count *= 4;

	fill_PROC_READDIR_req(&req, handle, offset, count);
	sendAndReceive(env, &req, &resp);

	{
		fsa_stat_t operstatus; //The status the READIR operation returned

		//One dirent
		uint32_t     hasnext;
		char         filename[FSA_NAMELEN + 1];
		fsa_cookie_t cookie;

		status = parse_PROC_READDIR_resp_start(&resp, &operstatus);
		if ( unlikely(status != FSA_OK) )
			goto End;
		if ( unlikely(operstatus != FSA_OK ) )
		{
			status = operstatus;
			goto End; //readdirres doesn't have more data if status is not FSA_OK
		}

		status = parse_PROC_READDIR_resp_dirent(&resp,
					&hasnext, filename, &cookie);
		if ( unlikely(status != FSA_OK) )
			goto End;

		while ( likely(hasnext) )
		{
			logDBG2("msaFuse_readdir filename='%s' cookie=%u",
				filename, cookie);

			//Give data to FUSE
			int fillres = filler(fillerBuf, filename, NULL, cookie);
			if ( unlikely(fillres == 1) )
			{
				//Even if there is more data from MSA server, the FSU
				// readdir buf is full. We have to ask for new data from MSA
				// server using this cookie/offset we are at now.
				break;
			}

			status = parse_PROC_READDIR_resp_dirent(&resp,
							&hasnext, filename, &cookie);
				if ( unlikely(status != FSA_OK) )
					goto End;
		}
	}

End:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

	logDBG1("msaFuse_readdir status=%u", status);

	return -maptoerrno(status);
}



static int msaFuse_releasedir(const char *path, struct fuse_file_info *fi)
{
	logDBG2("msaFuse_releasedir path='%s'", path);

	//Nothing we can do. There is no close/release of handles in FSA protocol

	fi->fh = (unsigned long) -1; //Invalid, help finding bugs

	return 0;
}

static int msaFuse_mkdir(const char *path, mode_t mode)
{
	fsa_packet_t req, resp;
	fsa_diropres_t *diropres;

	fsa_fhandle_t dirhandle;
	MsaClientEnv_t *env = getEnv();

	fsa_stat_t status;

	char      *dircopy, *dirpart; //Directory part of path
	char      *basecopy, *basepart; //Ending basename part of path

	logDBG2("msaFuse_mkdir path='%s' mode=%#x", path, mode);

	//Need to make own memory for dirname() and basename() to work in
	dircopy = easy_strdup(path);
	basecopy = easy_strdup(path);
	dirpart = dirname(dircopy);
	basepart = basename(basecopy);

	logDBG3("dirpart='%s' basepart='%s'", dirpart, basepart);

	status = lookuppath(env, dirpart, &dirhandle);
	if ( status != FSA_OK )
		goto End;

	if (strlen(basepart) > 12)
	{
		status = ERR_NAMETOOLONG;
		logDBG2("basepart='%s' too long, it can't be created!", basepart);
		goto End;
	}

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_MKDIR_req(&req, dirhandle, basepart,
		linux2fsa_mode(mode), time(NULL));
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_MKDIR_resp(&resp, &diropres);
	if ( unlikely(status != FSA_OK) )
		goto SentMKDIR;

	status = diropres->status;

SentMKDIR:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

End:
	easy_free(dircopy);
	easy_free(basecopy);

	logDBG1("msaFuse_mkdir status=%u", status);

	return -maptoerrno(status);
}


static int msaFuse_rmdir(const char *path)
{
	fsa_packet_t req, resp;

	fsa_fhandle_t dirhandle;
	MsaClientEnv_t *env = getEnv();

	fsa_stat_t status, operstatus;

	char      *dircopy, *dirpart; //Directory part of path
	char      *basecopy, *basepart; //Ending basename part of path

	logDBG2("msaFuse_rmdir path='%s'", path);

	//Need to make own memory for dirname() and basename() to work in
	dircopy = easy_strdup(path);
	basecopy = easy_strdup(path);
	dirpart = dirname(dircopy);
	basepart = basename(basecopy);

	logDBG3("dirpart='%s' basepart='%s'", dirpart, basepart);

	status = lookuppath(env, dirpart, &dirhandle);
	if ( status != FSA_OK )
		goto End;

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_RMDIR_req(&req, dirhandle, basepart);
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_RMDIR_resp(&resp, &operstatus);
	if ( unlikely(status != FSA_OK) )
		goto SentRMDIR;

	logDBG1("msaFuse_rmdir returned operStatus=%u", operstatus);
	status = operstatus;

SentRMDIR:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

End:
	easy_free(dircopy);
	easy_free(basecopy);

	return -maptoerrno(status);
}




static int msaFuse_unlink(const char *path)
{
	fsa_packet_t req, resp;

	fsa_fhandle_t dirhandle;
	MsaClientEnv_t *env = getEnv();

	fsa_stat_t status, operstatus;

	char      *dircopy, *dirpart; //Directory part of path
	char      *basecopy, *basepart; //Ending basename part of path

	logDBG2("msaFuse_unlink path='%s'", path);

	//Need to make own memory for dirname() and basename() to work in
	dircopy = easy_strdup(path);
	basecopy = easy_strdup(path);
	dirpart = dirname(dircopy);
	basepart = basename(basecopy);

	logDBG3("dirpart='%s' basepart='%s'", dirpart, basepart);

	status = lookuppath(env, dirpart, &dirhandle);
	if ( status != FSA_OK )
		goto End;

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_REMOVE_req(&req, dirhandle, basepart);
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_REMOVE_resp(&resp, &operstatus);
	if ( unlikely(status != FSA_OK) )
		goto SentREMOVE;

	logDBG1("msaFuse_remove returned operStatus=%u", operstatus);
	status = operstatus;

SentREMOVE:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

End:
	easy_free(dircopy);
	easy_free(basecopy);

	return -maptoerrno(status);
}



static int msaFuse_rename(const char *from, const char *to)
{
	fsa_packet_t req, resp;

	fsa_fhandle_t dirhandle, newdirhandle;
	MsaClientEnv_t *env = getEnv();

	fsa_stat_t status, operstatus;

	char      *dircopy, *dirpart; //Directory part of path
	char      *basecopy, *basepart; //Ending basename part of path
	char      *newdircopy, *newdirpart; //Directory part of path
	char      *newbasecopy, *newbasepart; //Ending basename part of path

	logDBG2("msaFuse_rename from='%s' to='%s'", from, to);

	//Need to make own memory for dirname() and basename() to work in
	dircopy = easy_strdup(from);
	basecopy = easy_strdup(from);
	dirpart = dirname(dircopy);
	basepart = basename(basecopy);
	logDBG3("dirpart='%s' basepart='%s'", dirpart, basepart);

	newdircopy = easy_strdup(to);
	newbasecopy = easy_strdup(to);
	newdirpart = dirname(newdircopy);
	newbasepart = basename(newbasecopy);
	logDBG3("newdirpart='%s' newbasepart='%s'", newdirpart, newbasepart);

	status = lookuppath(env, dirpart, &dirhandle);
	if ( status != FSA_OK )
		goto End;

	if (strlen(basepart) > 12)
	{
		status = ERR_NOENT;
		logDBG2("basepart='%s' too long, claiming it can't be found!", basepart);
		goto End;
	}

	status = lookuppath(env, newdirpart, &newdirhandle);
	if ( status != FSA_OK )
		goto End;

	if (strlen(newbasepart) > 12)
	{
		status = ERR_NAMETOOLONG;
		logDBG2("newbasepart='%s' too long, it can't be created!", newbasepart);
		goto End;
	}

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_RENAME_req(&req, dirhandle, basepart,
			newdirhandle, newbasepart);
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_RENAME_resp(&resp, &operstatus);
	if ( unlikely(status != FSA_OK) )
		goto SentRENAME;

	logDBG1("msaFuse_rename returned operStatus=%u", operstatus);
	status = operstatus;

SentRENAME:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

End:
	easy_free(dircopy);
	easy_free(basecopy);

	easy_free(newdircopy);
	easy_free(newbasecopy);

	return -maptoerrno(status);
}


static int msaFuse_chmod(const char *path, mode_t mode)
{
	fsa_packet_t req, resp;
	fsa_attrstat_t *attrstat;

	fsa_fhandle_t handle;
	MsaClientEnv_t *env = getEnv();

	fsa_stat_t status;

	logDBG2("msaFuse_chmod path='%s' %#x", path, mode);

	status = lookuppath(env, path, &handle);
	if ( unlikely(status != FSA_OK) )
		goto End;

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_SETATTR_req(&req,
		handle,
		SIZE_IGNORE,
		linux2fsa_mode(mode),
		TIME_IGNORE);
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_SETATTR_resp(&resp, &attrstat);
	if ( unlikely(status != FSA_OK) )
		goto SentSETATTR;

	if ( unlikely(attrstat->status != FSA_OK) )
		status = attrstat->status;

	//FUSE can't use the info in attrstat

SentSETATTR:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

End:
	logDBG3("msaFuse_chmod status=%u", status);

	return -maptoerrno(status);
}

static int msaFuse_truncate(const char *path, off_t size)
{
	fsa_packet_t req, resp;
	fsa_attrstat_t *attrstat;

	fsa_fhandle_t handle;
	MsaClientEnv_t *env = getEnv();

	fsa_stat_t status;

	logDBG2("msaFuse_truncate path='%s' size=%u", path, size);

	status = lookuppath(env, path, &handle);
	if ( unlikely(status != FSA_OK) )
		goto End;

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_SETATTR_req(&req,
		handle,
		size,
		MODE_IGNORE,
		TIME_IGNORE);
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_SETATTR_resp(&resp, &attrstat);
	if ( unlikely(status != FSA_OK) )
		goto SentSETATTR;

	if ( unlikely(attrstat->status != FSA_OK) )
		status = attrstat->status;

	//FUSE can't use the info in attrstat

SentSETATTR:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

End:
	logDBG3("msaFuse_truncate status=%u", status);

	return -maptoerrno(status);
}

static int msaFuse_utimens(const char *path, const struct timespec ts[2])
{
	fsa_packet_t req, resp;
	fsa_attrstat_t *attrstat;

	fsa_fhandle_t handle;
	MsaClientEnv_t *env = getEnv();

	fsa_stat_t status;

	logDBG2("msaFuse_utimens path=%'s'", path);

	status = lookuppath(env, path, &handle);
	if ( unlikely(status != FSA_OK) )
		goto End;

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_SETATTR_req(&req,
		handle,
		SIZE_IGNORE,
		MODE_IGNORE,
		ts[1].tv_sec);
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_SETATTR_resp(&resp, &attrstat);
	if ( unlikely(status != FSA_OK) )
		goto SentSETATTR;

	if ( unlikely(attrstat->status != FSA_OK) )
		status = attrstat->status;

	//FUSE can't use the info in attrstat

SentSETATTR:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

End:
	logDBG3("msaFuse_utimens status=%u", status);

	return -maptoerrno(status);
}

static int msaFuse_open(const char *path, struct fuse_file_info *fi)
{
	MsaClientEnv_t *env = getEnv();
	fsa_fhandle_t  handle;
	fsa_stat_t     status;

	logDBG2("msaFuse_open path='%s' flags=%x#", path, fi->flags);

	//Perform lookup and save handle
	status = lookuppath(env, path, &handle);

	if ( status != FSA_OK )
	{
		fi->fh = (unsigned long) -1; //Error
		return -maptoerrno(status);
	}

	logDBG2("msaFuse_open path='%s' -> handle=%u",
		path, handle);
	fi->fh = (unsigned long) handle;

	return -maptoerrno(status);
}

static int msaFuse_create(const char *path,
	mode_t mode, struct fuse_file_info *fi)
{
	fsa_packet_t req, resp;
	fsa_diropres_t *diropres;

	fsa_fhandle_t dirhandle;
	MsaClientEnv_t *env = getEnv();

	fsa_stat_t status;

	char      *dircopy, *dirpart; //Directory part of path
	char      *basecopy, *basepart; //Ending basename part of path

	logDBG2("msaFuse_create path='%s' flags=%#x mode=%#x",
			path, fi->flags, mode);

	//Need to make own memory for dirname() and basename() to work in
	dircopy = easy_strdup(path);
	basecopy = easy_strdup(path);
	dirpart = dirname(dircopy);
	basepart = basename(basecopy);

	logDBG3("dirpart='%s' basepart='%s'", dirpart, basepart);

	status = lookuppath(env, dirpart, &dirhandle);
	if ( status != FSA_OK )
		goto End;

	if (strlen(basepart) > 12)
	{
		status = ERR_NAMETOOLONG;
		logDBG2("basepart='%s' too long, it can't be created!", basepart);
		goto End;
	}

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_CREATE_req(&req,
		dirhandle, basepart,
		0, linux2fsa_mode(mode), time(NULL));
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_CREATE_resp(&resp, &diropres);
	if ( unlikely(status != FSA_OK) )
		goto SentCREATE;

	status = diropres->status;
	fi->fh = (status == FSA_OK) ?
		(unsigned long) diropres->dirop_ok.file : (unsigned long) -1;

SentCREATE:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

End:
	easy_free(dircopy);
	easy_free(basecopy);

	logDBG1("msaFuse_create status=%u handle=%u", status, fi->fh);
	return -maptoerrno(status);

}

//Max packet size is 64k, but we need for all read() and write() to be atomic.
// They are with a small enough size
#define MAXREAD (4*1024)
static int msaFuse_read(const char *path, char *buf,
		size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	MsaClientEnv_t *env = getEnv();
	fsa_fhandle_t	  handle = (fsa_fhandle_t) fi->fh;
	fsa_stat_t        status = FSA_OK;

	fsa_packet_t      req, resp;

	fsa_attrstat_t   *attrstat;
	uint32_t          len;
	uint8_t          *data;

	size_t            readbytes = 0;

	logDBG2("msaFuse_read path='%s' buf=%p size=%u offset=%u",
			path, buf, size, offset);

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	//Create request
	while ( readbytes < size )
	{
		size_t leftbytes = size - readbytes;
		size_t requestbytes = leftbytes > MAXREAD ? MAXREAD : leftbytes;

		logDBG3("Read loop, will request=%u bytes. readbytes=%u leftbytes=%u ",
			requestbytes, readbytes, leftbytes);

		fill_PROC_READ_req(&req, handle, offset + readbytes, requestbytes);
		sendAndReceive(env, &req, &resp);
		status = parse_PROC_READ_resp(&resp, &attrstat, &len, &data);
		if ( unlikely(status != FSA_OK) )
			goto End;

		if ( unlikely(attrstat->status != FSA_OK) )
		{
			status = attrstat->status;
			goto End;
		}

		logDBG3("Read loop, received len=%u bytes", len);

		if ( len == 0 )
		{
			//End of file
			break;
		}

		//Check that we havn't fetch more bytes than requested
		CHECK_BIGGER_OR_EQUAL(requestbytes, len);
		CHECK_BIGGER_OR_EQUAL(size, readbytes + len);

		//Copy into target buffer
		memcpy(buf + readbytes, data, len);

		readbytes += len;
	}

	//No, fuse can't use the fsa_fattr inside attrstat

End:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

	logDBG1("msaFuse_read status=%u len=%u", status, len);
	if ( likely(status == FSA_OK) )
		return readbytes;
	else
		return -maptoerrno(status);
}

#define MAXWRITE MAXREAD
static int msaFuse_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	MsaClientEnv_t *env = getEnv();
	fsa_fhandle_t	  handle = (fsa_fhandle_t) fi->fh;
	fsa_stat_t		  status = FSA_OK;

	fsa_packet_t	  req, resp;

	fsa_attrstat_t	 *attrstat;

	size_t			  writtenBytes = 0;

	logDBG2("msaFuse_write path='%s' buf=%p size=%u offset=%u",
		path, buf, size, offset);

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	//Create request
	while ( writtenBytes < size )
	{
		size_t leftBytes = size - writtenBytes;
		size_t requestBytes =
			leftBytes > MAXWRITE ? MAXWRITE : leftBytes;

		logDBG1("Write loop, will request=%u bytes. "
			"writtenBytes=%u leftBytes=%u ",
			requestBytes, writtenBytes, leftBytes);

		fill_PROC_WRITE_req(&req,
				handle, offset + writtenBytes,
				(const uint8_t *) buf + writtenBytes, requestBytes);
		sendAndReceive(env, &req, &resp);
		status = parse_PROC_WRITE_resp(&resp, &attrstat);
		if ( unlikely(status != FSA_OK) )
			goto End;

		if ( unlikely(attrstat->status != FSA_OK) )
		{
			status = attrstat->status;
			goto End;
		}

		writtenBytes += requestBytes;
	}

	//No, fuse can't use the fsa_fattr inside attrstat


End:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

	logDBG1("msaFuse_write status=%u", status);
	if ( likely(status == FSA_OK) )
		return writtenBytes;
	else
		return -maptoerrno(status);

}

static int msaFuse_release(const char *path, struct fuse_file_info *fi)
{
	logDBG2("msaFuse_release path='%s'", path);

	//Nothing we can do. There is no close/release of handles in FSA protocol

	fi->fh = (unsigned long) -1; //Invalid, help finding bugs

	return 0;
}

static struct fuse_operations msaFuse_ops = {
	.init = msaFuse_init,  //FSA_PROC_ROOT

	.destroy = msaFuse_destroy, //FSA_PROC_POWERCTRL


	.getattr	= msaFuse_getattr, //PROC_LOOKUP

	//.fgetattr	= //not supported, MSA is state less
	//.access	= //not supported

	/* MSA doesn't support linking
	.link		=
	.symlink	=
	.readlink	=
	*/

	.opendir	= msaFuse_opendir, //FSA_PROC_LOOKUP
	.readdir	= msaFuse_readdir, //FSA_PROC_READDIR
	.releasedir	= msaFuse_releasedir, //nothing

	//.mknod		= //Not supported

	.mkdir		= msaFuse_mkdir, //FSA_PROC_MKDIR
	.rmdir		= msaFuse_rmdir, //FSA_PROC_RMDIR


	.unlink		= msaFuse_unlink, //FSA_PROC_REMOVE


	.rename		= msaFuse_rename, //FSA_PROC_RENAME

	.chmod		= msaFuse_chmod, //FSA_PROC_SETATTR

	//.chown		= //not supported


	.truncate	= msaFuse_truncate, //FSA_PROC_SETATTR

	//.ftruncate	= //not supported, MSA is state less


	.utimens	= msaFuse_utimens, //FSA_PROC_SETATTR


	.create		= msaFuse_create, //FSA_PROC_CREATE

	.open		= msaFuse_open, //FSA_PROC_LOOKUP
	.read		= msaFuse_read, //FSA_PROC_READ
	.write		= msaFuse_write, //FSA_PROC_WRITE


	//.flush		= //not supported, MSA is state less

	.release	= msaFuse_release, //nothing

	//.statfs		= //not supported
	//.fsync		= //not supported, MSA is state less

	/*
	MSA does not support extended attributes
	.setxattr	= rfmFuse_setxattr,
	.getxattr	= rfmFuse_getxattr,
	.listxattr	= rfmFuse_listxattr,
	.removexattr	= rfmFuse_removexattr,
	*/

	/*
	MSA does not support locking
	.lock		= rfmFuse_lock,
	*/

	//.flag_nullpath_ok = 1,
};

/*
static struct fuse_opt msaFuse_opts[] =
{
	FUSE_OPT_KEY("-V",          KEY_VERSION),
	FUSE_OPT_KEY("--version",   KEY_VERSION),
	FUSE_OPT_KEY("-h",          KEY_HELP),
	FUSE_OPT_KEY("--help",      KEY_HELP),
	FUSE_OPT_KEY("-d",          KEY_DEBUG),
	FUSE_OPT_KEY("debug",       KEY_DEBUG),
	FUSE_OPT_END,
};

static void usage(char *binary)
{
	fprintf(stderr, "Usage: %s [options] <mountpoint>\n", binary);
	fprintf(stderr, "\n");
	fprintf(stderr, "Where options may be:\n");
	fprintf(stderr, "    -h, --help         Show help text.\n");
	fprintf(stderr, "    -V, --version      Show version of MSA-fuse.\n");
	fprintf(stderr, "    -o opt1,[opt2...]  Set mount options opt1, opt2..."
		"as described below.\n");
}

static int msaFuse_optProc(
	void *data, const char *arg, int key, struct fuse_args *args)
{
	MsaFuse_Config_t *config = (MsaFuse_Config_t *) data;

	NOTUSED(arg);

	switch (key)
	{
		case FUSE_OPT_KEY_OPT:
			return 1;

		case FUSE_OPT_KEY_NONOPT:
			return 1;

		case KEY_HELP:
			usage(args->argv[0]);
			fuse_opt_add_arg(args, "-ho");
			fuse_main(args->argc, args->argv, &msaFuse_ops, NULL);
			exit(1);

		case KEY_VERSION:
			fprintf(stderr, "MSA FUSE version %s\n", MSAFUSE_VERSION);
			fuse_opt_add_arg(args, "--version");
			fuse_main(args->argc, args->argv, &msaFuse_ops, NULL);
			exit(0);

		case KEY_DEBUG:
			return 1; //FUSE can parse debug option

		default:
			fprintf(stderr, "Unknown key: %d\n", key);
			return 1;
	}
}
*/


int main(int argc, char *argv[])
{
	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	//Setup logger
	log_setDebugLevel(4);
	log_setLogToConsole(LOGTOCONSOLE_STDOUT);

	logINFO("MSA FUSE version %s\n", MSAFUSE_VERSION);

	if (fuse_opt_parse(&args, NULL, NULL, NULL))
	{
		logFATAL("failed to parse options.");
		return 1;
	}

	//Always enable FUSE debugging. We are a test client
	fuse_opt_add_arg(&args, "-d");

	//Fuse MSA does not support multi threaded (parallel) calls
	fuse_opt_add_arg(&args, "-s");

	ret = fuse_main(args.argc, args.argv, &msaFuse_ops, NULL);

	logFATAL("Ending MSA FUSE client. ret=%d", ret);
	log_closeAllLog();

	return ret;
}


