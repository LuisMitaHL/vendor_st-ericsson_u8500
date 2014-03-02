/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/


#include "fsa.h"

#include "convenience.h"
#include "log.h"
#include "posix.h"
#include "wakelock.h"

#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

static inline void pack_start(
	fsa_packet_t *pkt,
	const size_t  size)
{
	pkt->body = pkt->buf + FSA_HEADERSIZE;
	pkt->curr = pkt->body;

	pkt->bodylen = size; //Tell how much we will use at most

	logDBG3("pack_start() bodylen will be at most %u bytes.", size);
}

static inline void pack_rewind(
	fsa_packet_t *pkt)
{
	pkt->curr = pkt->body;
}

static inline fsa_stat_t unpack_checklength(
	fsa_packet_t *pkt,
	size_t        size,
	const char   *descr)
{
	size_t have = pkt->body + pkt->bodylen - pkt->curr;
	logDBG3("pkt->body=%p pkt->bodylen=%zu pkt->curr=%p have=%zu",
		pkt->body, pkt->bodylen, pkt->curr, have);
	if ( unlikely(size > have) )
	{
		logERROR("Packet too short for unpacking %s. "
			"Have %u bytes, need %u bytes",
			descr, pkt->body + pkt->bodylen - pkt->curr, size);
		return PARAM_INVALID;
	}
	return FSA_OK;
}

static inline void pack_uint32(
	fsa_packet_t   *pkt,
	const uint32_t  val)
{
	*( (uint32_t *) pkt->curr ) = val;
	pkt->curr += sizeof(uint32_t);
}

#define pack_boolean pack_uint32
#define pack_cookie  pack_uint32
#define pack_fhandle pack_uint32
#define pack_status  pack_uint32
#define pack_count  pack_uint32

static inline fsa_stat_t unpack_uint32(
	fsa_packet_t *pkt,
	uint32_t     *val,
	const char   *descr)
{
	fsa_stat_t status;

	status = unpack_checklength(pkt, sizeof(uint32_t), descr );
	if ( unlikely(status != FSA_OK) )
		return status;

	*val = *( (uint32_t *) pkt->curr );
	pkt->curr += sizeof(uint32_t);
	return FSA_OK;
}

#define unpack_boolean unpack_uint32
#define unpack_cookie  unpack_uint32
#define unpack_fhandle unpack_uint32
#define unpack_status  unpack_uint32

static inline void pack_uint64(
	fsa_packet_t   *pkt,
	const uint64_t  val)
{
	*( (uint64_t *) pkt->curr ) = val;
	pkt->curr += sizeof(uint64_t);
}
#define pack_uhyper pack_uint64
#define pack_offset pack_uint64

static inline void pack_data(
	fsa_packet_t   *pkt,
	const uint8_t  *data,
	const uint32_t  len)
{
	size_t len4 = BYTES_TO_WORDALIGNED(len); //Length rounded upwards by 4

	logDBG3("pack_data() data=%p len=%u len4=%u", data, len, len4);

	//len
	pack_uint32(pkt, len);

	//data
	memcpy(pkt->curr, data, len);
	memset(pkt->curr + len, '\0', len4 - len); //Fill unused with '\0'
	pkt->curr += len4;
}

static inline fsa_stat_t unpack_data(
	fsa_packet_t    *pkt,
	uint32_t        *len,
	uint8_t        **data)
{
	fsa_stat_t status;
	size_t     len4;

	//Assume error
	*len = 0;
	*data = NULL;

	//len
	status = unpack_uint32(pkt, len, "'data len' in 'fsa_readres'");
	if ( unlikely(status != FSA_OK) )
		goto End;

	len4 = BYTES_TO_WORDALIGNED(*len); //Length rounded upwards by 4

	logDBG3("Want to unpack len=%u bytes of data. "
		"Need len4=%u bytes of data in packet buffer.",
		*len, len4);
	status = unpack_checklength(pkt, len4, "'data' in 'fsa_readres'");
	if ( unlikely(status != FSA_OK) )
		goto End;

	//data, return pointer directly into packet body/data buffer
	*data = pkt->curr;
	pkt->curr += len4;
End:
	logDBG3("unpack_data() len=%u data=%p ", *len, *data);
	return status;
}

static inline void pack_filename(
	fsa_packet_t *pkt,
	const char   *name)
{
	size_t len = strlen(name);
	assert(len <= FSA_NAMELEN);
	logDBG3("pack_filename() name='%s' len=%u", name, len);

	pack_data(pkt, (const uint8_t *) name, len);
}

static fsa_stat_t unpack_filename(
	fsa_packet_t *pkt,
	char		  name[FSA_NAMELEN + 1])
{
	fsa_stat_t status;
	uint32_t len4;
	uint32_t len;

	//Get filename len
	status = unpack_uint32(pkt, &len, "'len' of filename");
	if ( unlikely(status != FSA_OK) )
		return status;

	if ( unlikely(len > FSA_NAMELEN) )
	{
		logERROR("Filename length=%u is longer than FSA_NAMELEN=%u.",
			len, FSA_NAMELEN);
		return ERR_NAMETOOLONG;
	}

	//Check filename availability in packet stream
	len4 = BYTES_TO_WORDALIGNED(len);
	status = unpack_checklength(pkt, len4, "'name' of filename");
	if ( unlikely(status != FSA_OK) )
		return status;

	//Get filename
	memcpy(name, pkt->curr, len4); //Copy all bytes, including pading
	name[len] = '\0'; //Null terminate string if there was no pading
	pkt->curr += len4;

	//Check whether path separator '/' is present in the filename,
	// which is not allowed since it can not be handled internally.
	if ( unlikely(strchr(name, '/')) )
	{
		logERROR("Found directory seperator ('/') in file name '%s'.", name);
		return PARAM_INVALID;
	}

	logDBG3("unpack_filename() len=%u len4=%u name='%s'",
		len, len4, name);

	return FSA_OK;
}


static inline void pack_fattr(
	fsa_packet_t      *pkt,
	const fsa_fattr_t *fattr)
{
	fsa_fattr_t *pack = (fsa_fattr_t *) pkt->curr;

	//Copy entire struct
	*pack = *fattr;

	pkt->curr += sizeof(fsa_fattr_t);
}

static inline void pack_diropargs(
	fsa_packet_t        *pkt,
	const fsa_fhandle_t  dir,
	const char          *name)
{
	pack_fhandle(pkt, dir);
	pack_filename(pkt, name);
}

static inline fsa_stat_t unpack_diropargs(
	fsa_packet_t        *pkt,
	fsa_fhandle_t       *dir,
	char                 name[FSA_NAMELEN + 1])
{
	fsa_stat_t status;
	status = unpack_fhandle(pkt, dir, "'dir' in 'fsa_diropargs'");
	if ( unlikely(status != FSA_OK) )
		return status;

	return unpack_filename(pkt, name);
}

//sattrargs is fixed length, check length and return pointer into the fsa_packet
static inline fsa_stat_t unpack_sattrargs(
	fsa_packet_t        *pkt,
	fsa_sattrargs_t    **sattrargs)
{
	fsa_stat_t status;

	status = unpack_checklength(pkt, sizeof(fsa_sattrargs_t),
		"fsa_sattrargs");
	if ( unlikely(status != FSA_OK) )
		return status;

	//map
	*sattrargs = (fsa_sattrargs_t *) pkt->curr;
	pkt->curr += sizeof(fsa_sattrargs_t);

	return FSA_OK;
}

static inline void pack_readargs(
	fsa_packet_t        *pkt,
	const fsa_fhandle_t  file,
	const fsa_offset_t   offset,
	const fsa_count_t    count)
{
	pack_fhandle(pkt, file);
	pack_offset(pkt, offset);
	pack_count(pkt, count);
}

//readargs is fixed length, check length and return pointer into the fsa_packet
static inline fsa_stat_t unpack_readargs(
	fsa_packet_t    *pkt,
	fsa_readargs_t **readargs)
{
	size_t maxDataLen;
	fsa_stat_t status;
	uint32_t   count;

	status = unpack_checklength(pkt, sizeof(fsa_readargs_t),
		"fsa_readargs_t");
	if ( unlikely(status != FSA_OK) )
		return status;

	//map
	*readargs = (fsa_readargs_t *) pkt->curr;
	pkt->curr += sizeof(fsa_readargs_t);

	count = (*readargs)->count;

	//check that the count isn't way too big
	maxDataLen = FSA_MAXPACKETLEN - FSA_HEADERSIZE - sizeof(fsa_readargs_t);
	if ( unlikely(count > maxDataLen) )
	{
		logERROR("Count=%u is more than max data len=%u "
			"in the PROC_READ request.",
			count, maxDataLen);
			return PARAM_INVALID;
	}

	return FSA_OK;
}

static inline void pack_attrstat(
	fsa_packet_t        *pkt,
	const fsa_stat_t     status,
	const fsa_fattr_t   *fattr)
{
	pack_status(pkt, status);
	if ( likely(status == FSA_OK) )
	{
		pack_fattr(pkt, fattr);
	}
}

static inline void pack_diropres(
	fsa_packet_t        *pkt,
	const fsa_stat_t     status,
	const fsa_fhandle_t  handle,
	const fsa_fattr_t   *fattr)
{
	pack_status(pkt, status);
	if ( likely(status == FSA_OK) )
	{
		assert(handle != INVALID_HANDLE);
		assert(fattr != NULL);

		pack_fhandle(pkt, handle);
		pack_fattr(pkt, fattr);
	}
}

static inline void pack_sattr(
	fsa_packet_t   *pkt,
	const uint64_t  size,
	const uint32_t  mode,
	const uint32_t  seconds,
	const uint32_t  useconds)
{
	fsa_sattr_t *sattr = (fsa_sattr_t *) pkt->curr;

	sattr->size = size;
	sattr->mode = mode;
	sattr->ctime.seconds = seconds;
	sattr->ctime.useconds = useconds;

	pkt->curr += sizeof(fsa_sattr_t);
}

//sattr is fixed length, check length and return pointer into the fsa_packet
static inline fsa_stat_t unpack_sattr(
	fsa_packet_t  *pkt,
	fsa_sattr_t  **sattr)
{
	fsa_stat_t status;

	status = unpack_checklength(pkt, sizeof(fsa_sattr_t),
		"fsa_sattr");
	if ( unlikely(status != FSA_OK) )
		return status;

	//map
	*sattr = (fsa_sattr_t *) pkt->curr;
	pkt->curr += sizeof(fsa_sattr_t);

	return FSA_OK;
}


//diropres is fixed length, check length and return pointer into the fsa_packet
static inline fsa_stat_t unpack_diropres(
	fsa_packet_t    *pkt,
	fsa_diropres_t **diropres)
{
	fsa_stat_t status;
	//status
	status = unpack_checklength(pkt, sizeof(fsa_stat_t),
		"'status' part of 'fsa_diropres'");
	if ( unlikely(status != FSA_OK) )
		return status;

	//map
	*diropres = (fsa_diropres_t *) pkt->curr;
	pkt->curr += sizeof(fsa_stat_t);

	//Is response status FSA_OK?
	if ( unlikely((*diropres)->status != FSA_OK) )
		return FSA_OK; //Parsing is done

	//check that there is bytes enough for the dirop_ok part of fsa_diropres
	status = unpack_checklength(pkt, sizeof(fsa_diropok_t),
		"'dirop_ok' part of 'fsa_diropres'");
	if ( unlikely(status != FSA_OK) )
		return status;

	pkt->curr += sizeof(fsa_diropok_t);

	return FSA_OK;
}

//attrstat is fixed length, check length and return pointer in the fsa_packet
static inline fsa_stat_t unpack_attrstat(
	fsa_packet_t    *pkt,
	fsa_attrstat_t **attrstat)
{
	fsa_stat_t status;
	//status
	status = unpack_checklength(pkt, sizeof(fsa_stat_t),
		"'status' part of 'fsa_attrstat'");
	if ( unlikely(status != FSA_OK) )
		return status;

	//map
	*attrstat = (fsa_attrstat_t *) pkt->curr;
	pkt->curr += sizeof(fsa_stat_t);

	//Is response status FSA_OK?
	if ( unlikely((*attrstat)->status != FSA_OK) )
		return FSA_OK; //Parsing is done

	//check that there are bytes enough for the
	//'attributes' part of attrstat body
	status = unpack_checklength(pkt, sizeof(fsa_fattr_t),
		"'attributes' part of 'fsa_attrstat'");
	if ( unlikely(status != FSA_OK) )
		return status;

	pkt->curr += sizeof(fsa_fattr_t);

	return FSA_OK;
}

static fsa_stat_t unpack_createargs(
	fsa_packet_t  *pkt,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1],
	fsa_sattr_t  **sattr)
{
	fsa_stat_t status;

	status = unpack_diropargs(pkt, dir, name);
	if ( unlikely(status != FSA_OK) )
		return status;

	return unpack_sattr(pkt, sattr);
}

static fsa_stat_t unpack_renameargs(
	fsa_packet_t  *pkt,
	fsa_fhandle_t *fromHandle,
	char           fromName[FSA_NAMELEN + 1],
	fsa_fhandle_t *toHandle,
	char           toName[FSA_NAMELEN + 1])
{
	fsa_stat_t status;
	status = unpack_diropargs(pkt, fromHandle, fromName);
	if ( unlikely(status != FSA_OK) )
		return status;

	return unpack_diropargs(pkt, toHandle, toName);
}

static fsa_stat_t unpack_readdirargs(
	fsa_packet_t  *pkt,
	fsa_fhandle_t *dir,
	fsa_cookie_t  *cookie,
	uint32_t      *count)
{
	fsa_stat_t status;

	status = unpack_fhandle(pkt, dir, "'dir' in 'fsa_readdirargs'");
	if ( unlikely(status != FSA_OK) )
		return status;

	status = unpack_cookie(pkt, cookie, "'cookie' in 'fsa_readdirargs'");
	if ( unlikely(status != FSA_OK) )
		return status;

	status = unpack_uint32(pkt, count, "'count' in 'fsa_readdirargs'");
	if ( unlikely(status != FSA_OK) )
		return status;

	if ( unlikely(*count < SMALLEST_READDIR_COUNT) )
	{
		logERROR("'count' in 'fsa_readdirargs' is only %u. "
			"Smallest acceptable is %u.",
			*count, SMALLEST_READDIR_COUNT);
		return PARAM_INVALID;
	}

	return FSA_OK;
}

static fsa_stat_t unpack_powerargs(
	fsa_packet_t  *pkt,
	bool          *restart)
{
	fsa_stat_t status;
	uint32_t value;

	status = unpack_boolean(pkt, &value, "'restart' in 'fsa_powerargs'");
	if ( unlikely(status != FSA_OK) )
		return status;

	*restart = value != 0;

	return FSA_OK;
}

static inline void set_proc(
	fsa_packet_t *req,
	unsigned      proc)
{
	req->header.procedure = proc;
}

// Conversion functions
///////////////////////

fsa_mode_t linux2fsa_mode(mode_t mode)
{
	fsa_mode_t fsamode;

	fsamode = (mode & S_IRUSR) ? FMODE_READ: 0;
	fsamode |= (mode & S_IWUSR) ? FMODE_WRITE: 0;
	fsamode |= (mode & S_IXUSR) ? FMODE_EXEC: 0;

	return fsamode;
}

mode_t fsa2linux_mode(fsa_mode_t fsamode)
{
	mode_t mode;

	mode = (fsamode & FMODE_READ) ? S_IRUSR : 0;
	mode |= (fsamode & FMODE_WRITE) ? S_IWUSR : 0;
	mode |= (fsamode & FMODE_EXEC) ? S_IXUSR : 0;

	return mode;
}


fsa_ftype_t linux2fsa_ftype(mode_t mode)
{
	if (S_ISREG(mode))
		return FTYPE_REG;
	else if (S_ISDIR(mode))
		return FTYPE_DIR;
	else
		return FTYPE_NONE;
}

mode_t fsa2linux_ftype(fsa_ftype_t ftype)
{
	switch ( ftype )
	{
		case FTYPE_REG:
			return S_IFREG;

		case FTYPE_DIR:
			return S_IFDIR;

		case FTYPE_NONE:
			return 0;

		default:
			logERROR("Unknown file ftype=%#x", ftype);
			assert(false && "Unknown file ftype");
			return 0; //Makes compiler happy
	}
}

void linux2fsa_fattr(const struct stat *stbuf, fsa_fattr_t *fattr)
{
	fattr->size = stbuf->st_size;
	fattr->ctime.seconds= stbuf->st_mtime;
	fattr->ctime.useconds = 0;
	fattr->type = linux2fsa_ftype(stbuf->st_mode);
	fattr->mode = linux2fsa_mode(stbuf->st_mode);
}

void fsa2linux_fattr(const fsa_fattr_t *fattr, struct stat *stbuf)
{
	stbuf->st_size = fattr->size;
	stbuf->st_ctime = stbuf->st_atime = stbuf->st_mtime =
				fattr->ctime.seconds;
	stbuf->st_mode = fsa2linux_mode(fattr->mode);
	stbuf->st_mode |= fsa2linux_ftype(fattr->type);
}


#define CASE_TO_STRING(s) \
	case s: \
		return #s;
const char * fsaStatusToString(fsa_stat_t status)
{
	switch ( status )
	{
		CASE_TO_STRING(FSA_OK)
		CASE_TO_STRING(ERR_ACCESS)
		CASE_TO_STRING(ERR_NOENT)
		CASE_TO_STRING(ERR_IO)
		CASE_TO_STRING(ERR_EXIST)
		CASE_TO_STRING(ERR_NOTDIR)
		CASE_TO_STRING(ERR_ISDIR)
		CASE_TO_STRING(ERR_FBIG)
		CASE_TO_STRING(ERR_NOSPC)
		CASE_TO_STRING(ERR_NAMETOOLONG)
		CASE_TO_STRING(ERR_NOTEMPTY)
		CASE_TO_STRING(ERR_STALE)

		default:
			return "Unknown FSA status.";
	}
}

const char * fsaProcedureToString(unsigned proc)
{
	switch ( proc )
	{
		CASE_TO_STRING(PROC_NULL)
		CASE_TO_STRING(PROC_GETATTR)
		CASE_TO_STRING(PROC_SETATTR)
		CASE_TO_STRING(PROC_ROOT)
		CASE_TO_STRING(PROC_LOOKUP)
		CASE_TO_STRING(PROC_READ)
		CASE_TO_STRING(PROC_WRITE)
		CASE_TO_STRING(PROC_CREATE)
		CASE_TO_STRING(PROC_REMOVE)
		CASE_TO_STRING(PROC_RENAME)
		CASE_TO_STRING(PROC_MKDIR)
		CASE_TO_STRING(PROC_RMDIR)
		CASE_TO_STRING(PROC_READDIR)
		CASE_TO_STRING(PROC_POWERCTRL)

		default:
			return "Unknown FSA procedure.";
	}
}
#undef CASE_TO_STRING

void fill_header(
	fsa_packet_t *req,
	unsigned      transaction)
{
	req->header.library = 1;
	req->header.transaction = transaction;
}

// PROC_NULL = 0
/////////////////
void fill_PROC_NULL_req(
	fsa_packet_t *req)
{
	set_proc(req, PROC_NULL);
	pack_start(req, 0);

	//PROC_NULL request has no body
}

//Since PROC_NULL request has no body, parsing of it is not needed

void fill_PROC_NULL_resp(
	fsa_packet_t *resp)
{
	set_proc(resp, PROC_NULL);
	pack_start(resp, 0);

	//PROC_NULL response has no body
}

//Since PROC_NULL response has no body, parsing of it is not needed


// PROC_GETATTR = 1
////////////////////
void fill_PROC_GETATTR_req(
	fsa_packet_t *req,
	fsa_fhandle_t handle)
{
	set_proc(req, PROC_GETATTR);
	pack_start(req, sizeof(fsa_fhandle_t));

	pack_fhandle(req, handle);
}

fsa_stat_t parse_PROC_GETATTR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *handle)
{
	return unpack_fhandle(req, handle, "fsa_handle");
}

void fill_PROC_GETATTR_resp(
	fsa_packet_t *resp,
	fsa_stat_t    status,
	fsa_fattr_t  *fattr)
{
	set_proc(resp, PROC_GETATTR);
	pack_start(resp, sizeof(fsa_attrstat_t));

	pack_attrstat(resp, status, fattr);

}

fsa_stat_t parse_PROC_GETATTR_resp(
	fsa_packet_t    *resp,
	fsa_attrstat_t **attrstat)
{
	return unpack_attrstat(resp, attrstat);
}


// PROC_SETATTR = 2
////////////////////
void fill_PROC_SETATTR_req(
	fsa_packet_t *req,
	fsa_fhandle_t handle,
	uint64_t      size,
	uint32_t      mode,
	uint32_t      seconds)
{
	set_proc(req, PROC_SETATTR);
	pack_start(req, sizeof(fsa_sattrargs_t));

	pack_fhandle(req, handle);
	pack_sattr(req, size, mode, seconds, 0);
}

fsa_stat_t parse_PROC_SETATTR_req(
	fsa_packet_t     *req,
	fsa_sattrargs_t **sattrargs)
{
	return unpack_sattrargs(req, sattrargs);
}

void fill_PROC_SETATTR_resp(
	fsa_packet_t *resp,
	fsa_stat_t    status,
	fsa_fattr_t  *fattr)
{
	set_proc(resp, PROC_SETATTR);
	pack_start(resp, sizeof(fsa_attrstat_t));

	pack_attrstat(resp, status, fattr);
}

fsa_stat_t parse_PROC_SETATTR_resp(
	fsa_packet_t    *resp,
	fsa_attrstat_t **attrstat)
{
	return unpack_attrstat(resp, attrstat);
}


// PROC_ROOT = 3
/////////////////
void fill_PROC_ROOT_req(
	fsa_packet_t *req)
{
	set_proc(req, PROC_ROOT);
	pack_start(req, 0);

	//PROC_ROOT request has no body
}

//Since PROC_ROOT request has no body, parsing of it is not needed

void fill_PROC_ROOT_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status,
	fsa_fhandle_t  handle,
	fsa_fattr_t   *attr)
{
	set_proc(resp, PROC_ROOT);
	pack_start(resp, sizeof(fsa_diropres_t));

	pack_diropres(resp, status, handle, attr);
}

fsa_stat_t parse_PROC_ROOT_resp(
	fsa_packet_t    *resp,
	fsa_diropres_t **diropres)
{
	return unpack_diropres(resp, diropres);
}


// PROC_LOOKUP = 4
////////////////////
void fill_PROC_LOOKUP_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  dir,
	const char    *name)
{
	set_proc(req, PROC_LOOKUP);
	pack_start(req, sizeof(fsa_diropargs_t) + FSA_NAMELEN);

	pack_diropargs(req, dir, name);
}

fsa_stat_t parse_PROC_LOOKUP_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1])
{
	return unpack_diropargs(req, dir, name);
}

void fill_PROC_LOOKUP_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status,
	fsa_fhandle_t  handle,
	fsa_fattr_t   *attr)
{
	set_proc(resp, PROC_LOOKUP);
	pack_start(resp, sizeof(fsa_diropres_t));

	pack_diropres(resp, status, handle, attr);
}

fsa_stat_t parse_PROC_LOOKUP_resp(
	fsa_packet_t	*resp,
	fsa_diropres_t **diropres)
{
	return unpack_diropres(resp, diropres);
}


// PROC_READ = 5
//////////////////
void fill_PROC_READ_req(
	fsa_packet_t       *req,
	const fsa_fhandle_t file,
	const uint64_t	    offset,
	const uint32_t	    count)
{
	set_proc(req, PROC_READ);
	pack_start(req, sizeof(fsa_readargs_t));

	pack_readargs(req, file, offset, count);
}

fsa_stat_t parse_PROC_READ_req(
	fsa_packet_t    *req,
	fsa_readargs_t **readargs)
{
	return unpack_readargs(req, readargs);
}


//Will only reserv space and return poiinters to the spot in the PROC_READ
// response where data should be filled in
void fill_PROC_READ_resp_start(
	fsa_packet_t    *resp,
	fsa_count_t      count,
	fsa_readres_t  **readresSpot,
	uint8_t        **dataSpot)
{
	uint8_t *curr;

	set_proc(resp, PROC_READ);
	pack_start(resp, sizeof(fsa_readres_t) + BYTES_TO_WORDALIGNED(count));
	curr = resp->curr;

	*readresSpot = (fsa_readres_t *) curr;
	curr += sizeof(fsa_readres_t);

	*dataSpot = (uint8_t *) curr;

	//Curr now points to where data starts. The true length will be updated on
	// a call to fille_PROC_READ_resp_end()
	resp->curr = curr;
}

void fill_PROC_READ_resp_end(
	fsa_packet_t   *resp,
	uint32_t        datalen)
{
	uint32_t len4 = BYTES_TO_WORDALIGNED(datalen);
	uint32_t trailing = len4 - datalen;

	memset(resp->curr + datalen, 0x01, trailing);
	resp->curr += len4;
}

void fill_PROC_READ_resp_failed(
	fsa_packet_t   *resp,
	fsa_stat_t      status)
{
	//We should only have a fsa_stat_t in the body
	pack_rewind(resp);
	pack_status(resp, status);
}

fsa_stat_t parse_PROC_READ_resp(
	fsa_packet_t    *resp,
	fsa_attrstat_t **attrstat,
	uint32_t        *len,
	uint8_t        **data)
{
	fsa_stat_t status;

	status = unpack_attrstat(resp, attrstat);
	if ( unlikely(status != FSA_OK) )
		goto End;

	if ( unlikely((*attrstat)->status != FSA_OK) )
		goto End; //Parsing is done, don't look for more data

	logDBG3("resp->body=%p resp->curr=%p diff=%u",
		resp->body, resp->curr, resp->curr - resp->body);

	//Read all data
	status = unpack_data(resp, len, data);

End:
	logDBG3("parse_PROC_READ_resp status=%u len=%u data=%p",
		status, *len, *data);
	return status;
}


// PROC_WRITE = 6
//////////////////
void fill_PROC_WRITE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  file,
	uint64_t	   offset,
	const uint8_t *data,
	uint32_t	   datalen)
{
	size_t len4 = BYTES_TO_WORDALIGNED(datalen);

	set_proc(req, PROC_WRITE);
	pack_start(req, sizeof(fsa_writeargs_t) + len4);

	pack_fhandle(req, file);
	pack_offset(req, offset);
	pack_data(req, data, datalen);
}

fsa_stat_t parse_PROC_WRITE_req(
	fsa_packet_t     *req,
	fsa_writeargs_t **writeargs)
{
	fsa_stat_t status;

	status = unpack_checklength(req, sizeof(fsa_writeargs_t),
		"fsa_writeargs_t");
	if ( unlikely(status != FSA_OK) )
		return status;

	//map
	*writeargs = (fsa_writeargs_t *) req->curr;

	//Will consume everything but the data buffer
	req->curr += sizeof(fsa_writeargs_t);

	//check that datalen is less or equal to bytes left in buffer
	return unpack_checklength(req, (*writeargs)->data.len,
		"'data' in 'fsa_writeargs'");
}

void fill_PROC_WRITE_resp(
	fsa_packet_t *resp,
	fsa_stat_t    status,
	fsa_fattr_t  *fattr)
{
	set_proc(resp, PROC_WRITE);
	pack_start(resp, sizeof(fsa_attrstat_t));

	pack_attrstat(resp, status, fattr);
}

fsa_stat_t parse_PROC_WRITE_resp(
	fsa_packet_t    *resp,
	fsa_attrstat_t **attrstat)
{
	return unpack_attrstat(resp, attrstat);
}


// PROC_CREATE = 7
///////////////////
void fill_PROC_CREATE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  dir,
	const char    *name,
	uint64_t	   size,
	uint32_t	   mode,
	uint32_t	   seconds)
{
	set_proc(req, PROC_CREATE);
	pack_start(req, sizeof(fsa_createargs_t) + FSA_NAMELEN);

	pack_diropargs(req, dir, name);
	pack_sattr(req, size, mode, seconds, 0);
}

fsa_stat_t parse_PROC_CREATE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1],
	fsa_sattr_t  **sattr)
{
	return unpack_createargs(req, dir, name, sattr);
}

void fill_PROC_CREATE_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status,
	fsa_fhandle_t  handle,
	fsa_fattr_t   *attr)
{
	set_proc(resp, PROC_CREATE);
	pack_start(resp, sizeof(fsa_diropres_t));

	pack_diropres(resp, status, handle, attr);
}

fsa_stat_t parse_PROC_CREATE_resp(
	fsa_packet_t    *resp,
	fsa_diropres_t **diropres)
{
	return unpack_diropres(resp, diropres);
}


// PROC_REMOVE = 8
////////////////////
void fill_PROC_REMOVE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  dir,
	const char    *name)
{
	set_proc(req, PROC_REMOVE);
	pack_start(req, sizeof(fsa_diropargs_t) + FSA_NAMELEN);

	pack_diropargs(req, dir, name);
}

fsa_stat_t parse_PROC_REMOVE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1])
{
	return unpack_diropargs(req, dir, name);
}

void fill_PROC_REMOVE_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status)
{
	set_proc(resp, PROC_REMOVE);
	pack_start(resp, sizeof(fsa_diropres_t));

	pack_status(resp, status);
}

fsa_stat_t parse_PROC_REMOVE_resp(
	fsa_packet_t    *resp,
	fsa_stat_t      *status)
{
	return unpack_status(resp, status, "status");
}


// PROC_RENAME = 9
////////////////////
void fill_PROC_RENAME_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  fromHandle,
	const char    *fromName,
	fsa_fhandle_t  toHandle,
	const char    *toName)

{
	set_proc(req, PROC_RENAME);
	pack_start(req, sizeof(fsa_renameargs_t) + 2 * FSA_NAMELEN);

	pack_diropargs(req, fromHandle, fromName);
	pack_diropargs(req, toHandle, toName);
}

fsa_stat_t parse_PROC_RENAME_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *fromHandle,
	char           fromName[FSA_NAMELEN + 1],
	fsa_fhandle_t *toHandle,
	char           toName[FSA_NAMELEN + 1])
{
	return unpack_renameargs(req, fromHandle, fromName,
			toHandle, toName);
}

void fill_PROC_RENAME_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status)
{
	set_proc(resp, PROC_RENAME);
	pack_start(resp, sizeof(fsa_diropres_t));

	pack_status(resp, status);
}

fsa_stat_t parse_PROC_RENAME_resp(
	fsa_packet_t    *resp,
	fsa_stat_t      *status)
{
	return unpack_status(resp, status, "status");
}

// PROC_MKDIR = 10
////////////////////
void fill_PROC_MKDIR_req(
	fsa_packet_t     *req,
	fsa_fhandle_t     dir,
	const char       *name,
	uint32_t	      mode,
	uint32_t	      seconds)
{
	set_proc(req, PROC_MKDIR);
	pack_start(req, sizeof(fsa_createargs_t) + FSA_NAMELEN);

	pack_diropargs(req, dir, name);
	pack_sattr(req, SIZE_IGNORE, mode, seconds, 0);
}

fsa_stat_t parse_PROC_MKDIR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1],
	fsa_sattr_t  **sattr)
{
	return unpack_createargs(req, dir, name, sattr);
}

void fill_PROC_MKDIR_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status,
	fsa_fhandle_t  handle,
	fsa_fattr_t   *attr)
{
	set_proc(resp, PROC_MKDIR);
	pack_start(resp, sizeof(fsa_diropres_t));

	pack_diropres(resp, status, handle, attr);
}

fsa_stat_t parse_PROC_MKDIR_resp(
	fsa_packet_t    *resp,
	fsa_diropres_t **diropres)
{
	return unpack_diropres(resp, diropres);
}


// PROC_RMDIR = 11
////////////////////
void fill_PROC_RMDIR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  dir,
	const char    *name)
{
	set_proc(req, PROC_RMDIR);
	pack_start(req, sizeof(fsa_diropargs_t) + FSA_NAMELEN);

	pack_diropargs(req, dir, name);
}

fsa_stat_t parse_PROC_RMDIR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1])
{
	return unpack_diropargs(req, dir, name);
}

void fill_PROC_RMDIR_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status)
{
	set_proc(resp, PROC_RMDIR);
	pack_start(resp, sizeof(fsa_diropres_t));

	pack_status(resp, status);
}

fsa_stat_t parse_PROC_RMDIR_resp(
	fsa_packet_t    *resp,
	fsa_stat_t      *status)
{
	return unpack_status(resp, status, "status");
}


// PROC_READDIR = 12
////////////////////
void fill_PROC_READDIR_req( // = 12
	fsa_packet_t *req,
	fsa_fhandle_t dir,
	fsa_cookie_t  cookie,
	uint32_t      count)
{
	assert(count % 4 == 0);

	set_proc(req, PROC_READDIR);
	pack_start(req, sizeof(fsa_readdirargs_t));

	pack_fhandle(req, dir);
	pack_cookie(req, cookie);
	pack_uint32(req, count);
}

fsa_stat_t parse_PROC_READDIR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	fsa_cookie_t  *cookie,
	uint32_t      *count)
{
	return unpack_readdirargs(req, dir, cookie, count);
}


void fill_PROC_READDIR_resp_start(
	fsa_packet_t    *resp,
	dirent_filler_t *filler,
	fsa_count_t      count)
{
	filler->count = count;
	filler->used = 0;
	filler->currCookie = 0;
	filler->cookiePtr = NULL;

	set_proc(resp, PROC_READDIR);
	pack_start(resp, sizeof(fsa_stat_t) + sizeof(fsa_bool_t) + count);

	//Assume that everything will be FSA_OK. If not, we will rewind the packet
	// buffer and fix it at the end of this function
	pack_status(resp, FSA_OK);
}

bool fill_PROC_READDIR_resp_dirent(
	fsa_packet_t    *resp,
	dirent_filler_t *filler,
	struct dirent   *dirent)
{
	size_t direntSize;
	size_t namelen;

	/* The name will be padded with 0x00 to fill 1, 2 or 3 uint32_t,
	   in addition there is the preceeding string length, and the trailing
	   cookie and continuation boolean, each occupying a uint32_t. So the
	   number of bytes used are:

	   1. 4 (bool) Set to 1 to mark that there is one more entry
	   2. 4 (strlen) Length of filename
	   3. [4,12] (string) File name, NOT always null terminated. Only padded.
	   4. 4 (cookie) Sequence number or COOKIE_LAST

	   This gives the range [16,24] bytes in total.
	 */

	namelen = strlen(dirent->d_name);

	//Ignore any directory entries whose filenames are too long
	if ( unlikely(namelen > 12) )
	{
		logERROR("File '%s' is longer than 12 chars. "
			"Skipping to report it in response to PROC_READDIR request.",
			dirent->d_name);

		return true; //There might be room for another dirent
	}

	direntSize = 3 * sizeof(uint32_t) + BYTES_TO_WORDALIGNED(namelen);
	assert(direntSize >= 16 && direntSize <= 24);

	//Make sure that the directory entry fits inside the response
	if ( filler->used + direntSize <= filler->count )
	{
		//Indicate that a filename+cookie+another boolean follows this one
		pack_boolean(resp, 1);

		//Store the file as read by posix_readdir_r() above
		pack_filename(resp, dirent->d_name);

		//Save the cookie pointer so we can go back and
		//change it to be COOKIE_LAST if it happens to
		//be the last entry in the directory (currenctly unknown)
		filler->cookiePtr = (fsa_cookie_t *) resp->curr;

		//The cookie recorded in the dirent reply is the cookie for
		//the next entry.
		pack_cookie(resp, filler->currCookie + 1);

		logDBG2("PROC_READDIR added name='%s' cookie=%u",
				dirent->d_name, filler->currCookie);

		//Update used bytes
		filler->used += direntSize;

		return true; //There might be room for another dirent
	}
	else
	{
		return false; //No, we don't have room for another dirent
	}

}

void fill_PROC_READDIR_resp_end(
	fsa_packet_t   *resp)
{
	pack_boolean(resp, 0); //No more entries
}

void fill_PROC_READDIR_resp_failed(
	fsa_packet_t   *resp,
	fsa_stat_t      status)
{
	//We should only have a fsa_stat_t in the body
	pack_rewind(resp);
	pack_status(resp, status);
}

fsa_stat_t parse_PROC_READDIR_resp_start(
	fsa_packet_t  *req,
	fsa_stat_t    *status)
{
	return unpack_status(req, status, "'status' of 'fsa_readdirres'");
}

fsa_stat_t parse_PROC_READDIR_resp_dirent(
	fsa_packet_t  *req,
	fsa_bool_t    *hasmore,
	char		   filename[FSA_NAMELEN + 1],
	fsa_cookie_t  *cookie)
{
	fsa_stat_t status;
	status = unpack_boolean(req, hasmore, "'hasmore' of 'fsa_readdirres'");
	if ( unlikely(status != FSA_OK) )
		return status;

	if ( ! *hasmore )
		return FSA_OK; //Nothing more to parse

	status = unpack_filename(req, filename);
	if ( unlikely(status != FSA_OK) )
		return status;

	return unpack_cookie(req, cookie, "'cookie' of 'fsa_readdirres'");
}


// PROC_POWERCTRL = 13
////////////////////
void fill_PROC_POWERCTRL_req(
	fsa_packet_t  *req,
	bool           restart)
{
	set_proc(req, PROC_POWERCTRL);
	pack_start(req, sizeof(fsa_powerargs_t));

	pack_boolean(req, restart);
}

fsa_stat_t parse_PROC_POWERCTRL_req(
	fsa_packet_t  *req,
	bool		  *restart)
{
	return unpack_powerargs(req, restart);
}

void fill_PROC_POWERCTRL_resp(
	fsa_packet_t *resp)
{
	set_proc(resp, PROC_POWERCTRL);
	pack_start(resp, 0);

	//PROC_POWERCTRL response has no body
}

static inline void logPacket(fsa_packet_t *pkt)
{
	logDBG3("FSA packet: buf=%p", pkt->buf);

	logDBG3("Header: Length=%u words %u bytes "
		"Transaction=%u Procedure=%u Library=%u",
		pkt->header.length, pkt->bodylen,
		pkt->header.transaction, pkt->header.procedure, pkt->header.library);
	logHexDump(pkt->buf, FSA_HEADERSIZE);

	logDBG3("Body: body=%p curr=%p curr-body=%u",
		pkt->body, pkt->curr, pkt->curr - pkt->body);
	logHexDump(pkt->body, pkt->bodylen);
}

void fsa_allocpacket(fsa_packet_t *pkt)
{
	pkt->buf = easy_malloc(FSA_MAXPACKETLEN);
}

void fsa_freepacket(fsa_packet_t *pkt)
{
	easy_free(pkt->buf);
}

static bool fsa_waitpacket(int dev)
{
	struct timeval tv;
	fd_set    readfds;
	int       retval;

	assert(dev >= 0);

	logDBG2("Waiting for FSA packet.");

	tv.tv_sec = FSA_WAKELOCK_TIMEOUT_MS / 1000;
	tv.tv_usec = (FSA_WAKELOCK_TIMEOUT_MS % 1000) * 1000;

	do
	{
		FD_ZERO(&readfds);
		FD_SET(dev, &readfds);

		retval = posix_select("RPC", dev + 1, &readfds, NULL, NULL, wakelock_active() ? &tv : NULL);

		if (retval < 0)
		{
			logERROR("Unexpected error %d in select() call: %s",
				errno, strerror(errno));
			return false;
		}
		else if (retval > 0)
		{
			/* Data is available for reading.
			   Acquire a wakelock to force platform to stay awake
			   while the received RPC request is being processed. */
			wakelock_acquire();
			return true;
		}
		else
		{
			/* Timed out.
			   Release wake lock so platform is free to sleep. */
			wakelock_release();
		}
	} while (1);

}

bool fsa_receivepacket(int dev, fsa_packet_t *pkt)
{
	ssize_t   len;
	uint32_t *header;

	assert(dev >= 0);
	assert(pkt);

	/* Wait for a packet before trying to read one */

	if ( unlikely(fsa_waitpacket(dev) != true) )
	{
		logERROR("Error while waiting for FSA packet.");
		return false;
	}

	logDBG2("Receiving FSA packet.");

	/* Actually, /dev/rpc is not stream based. It is packet based. We will get
	   the full packet on a read even if we only request 1 byte of it. */

	len = posix_read("RPC", dev, pkt->buf, FSA_MAXPACKETLEN);
	if ( unlikely(len < 0) )
	{
		logERROR("Can not read FSA packet header.");
		return false;
	}
	else if ( unlikely(len == 0) )
	{
		logERROR("MSA RPC-device was closed.");
		return false;
	}
	else if ( unlikely(len < (ssize_t) FSA_HEADERSIZE) )
	{
		logERROR("Short read when reading from RPC device. "
			"Need at least %u bytes. Only got %u bytes.",
			FSA_HEADERSIZE, len);
		return false;
	}
	else if ( unlikely(len > (ssize_t) FSA_MAXPACKETLEN) )
	{
		logERROR("Too long read when reading from RPC device. "
			"Have room for at max %u bytes. Got %u bytes.",
			FSA_MAXPACKETLEN, len);
		return false;
	}

	//parse the raw header
	header = (uint32_t *) pkt->buf;
	pkt->header.length      = (header[0] >>  0) & 0x003fffff;
	pkt->header.transaction = (header[0] >> 22) & 0x000003ff;
	pkt->header.procedure   = (header[1] >>  0) & 0x0000ffff;
	pkt->header.library     = (header[1] >> 16) & 0x0000ffff;

	//header.length is in number of uint32_t
	pkt->bodylen = pkt->header.length * sizeof(uint32_t);

	//Map the body body, and have the curr pointer point to start
	pkt->body = pkt->buf + FSA_HEADERSIZE;
	pkt->curr = pkt->body;

	//debug
	logPacket(pkt);

	//Check the header for more inconsistency
	if ( unlikely(pkt->bodylen > FSA_MAXPACKETLEN - FSA_HEADERSIZE) )
	{
		logFATAL("Received packet says it is longer than my buffer. "
			"Packet.length=%u bytes. Max header+body length %u+%u=%u bytes.",
			pkt->bodylen, FSA_HEADERSIZE,
			FSA_MAXPACKETLEN - FSA_HEADERSIZE, FSA_MAXPACKETLEN);
			return false;
	}
	if ( unlikely(pkt->header.library != 1) )
	{
		logERROR("Library field=%u is not 1 in header "
			"in received FSA packet.",
			pkt->header.library);
		return false;
	}
	if ( unlikely(pkt->header.procedure >= PROC_MAX) )
	{
		logERROR("Procedure field=%u is bigger than %u "
			"in header in received FSA packet.",
			pkt->header.procedure, PROC_MAX - 1);
		return false;
	}

	return true;
}

bool fsa_sendpacket(int dev, fsa_packet_t *pkt)
{
	size_t bodysize;
	uint32_t *header;

	assert(dev >= 0);
	assert(pkt);

	logDBG2("Sending FSA packet.");

	bodysize = pkt->curr - pkt->body;
	//Verify that we did not use more bodysize than we inititially said we would
	assert(bodysize <= pkt->bodylen);

	pkt->bodylen = bodysize; //True body size
	assert(pkt->bodylen % 4 == 0);
	assert(pkt->bodylen / sizeof(uint32_t) < (1 << 22));
	assert(pkt->header.transaction < (1 << 10) );
	assert(pkt->header.procedure < PROC_MAX);
	assert(pkt->header.library == 1);

	//Fill header.length, other header body must already be filled in
	pkt->header.length = pkt->bodylen / sizeof(uint32_t);

	//Build raw header, no need to mask bits, members are within bounds
	header = (uint32_t *) pkt->buf;
	header[0]  = (uint32_t) pkt->header.length;
	header[0] |= ((uint32_t) pkt->header.transaction) << 22;
	header[1]  = (uint32_t) pkt->header.procedure;
	header[1] |= ((uint32_t) pkt->header.library) << 16;

	//debug
	logPacket(pkt);

	{
		ssize_t towrite;
		ssize_t written;

		towrite = FSA_HEADERSIZE + pkt->bodylen;
		written = posix_write("RPC", dev, pkt->buf, towrite);

		if ( written != towrite )
		{
			logERROR("Write size to RPC was a mismatch: "
				"Want to write %u bytes. Written=%u bytes.",
				towrite, written);
			return false;
		}
	}

	return true;
}

