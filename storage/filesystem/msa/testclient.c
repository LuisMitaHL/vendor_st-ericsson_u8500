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

#include "convenience.h"
#include "fsa.h"
#include "log.h"
#include "msa.h"
#include "posix.h"
#include "test.h"
#include "testclient.h"

/* This file is a library of functions targeted a test client for MSA. It is
not optimized to be smart or fast. It has several debugging and dumping
functions, target to generate both correct and incorrect packets.

This means that you should NOT use this file for any real client, only for test
clients.
*/

static char *makeprefix(const char *prefix, const char *added)
{
	size_t prefixlen = strlen(prefix);
	size_t addedlen = strlen(added);
	char *str = easy_malloc(prefixlen + 1 + addedlen + 1);
	sprintf(str, "%s %s", prefix, added);
	return str;
}

void dump_status(const char *prefix, fsa_stat_t status)
{
	dump("%s uint32_t status=%u (%s)",
		prefix, status, fsaStatusToString(status));
}

void dump_data(const char *prefix, fsa_data_t *data)
{
	char *str = makeprefix(prefix, "fsa_data");

	dumpUint32(str, "len", data->len);
	logHexDump(data->data, data->len);

	easy_free(str);
}

void dump_timeval(const char *prefix, fsa_timeval_t *timeval)
{
	char *str = makeprefix(prefix, "fsa_timeval");

	dumpUint32(str, "seconds", timeval->seconds);
	dumpUint32(str, "usceonds", timeval->useconds);

	easy_free(str);
}

void dump_fattr(const char *prefix, fsa_fattr_t *fattr)
{
	char *str = makeprefix(prefix, "fsa_fattr");

	dumpUint64(str, "size", fattr->size);
	dumpHex32(str, "mode", fattr->mode);
	dump_timeval(str, &fattr->ctime);
	dumpUint32(str, "type", fattr->type);

	easy_free(str);
}

void dump_sattr(const char *prefix, fsa_sattr_t *sattr)
{
	char *str = makeprefix(prefix, "fsa_sattr");

	dumpUint64(str, "size", sattr->size);
	dumpHex32(str, "mode", sattr->mode);
	dump_timeval(str, &sattr->ctime);

	easy_free(str);
}

void dump_attrstat(const char *prefix, fsa_attrstat_t *attrstat)
{
	char *str = makeprefix(prefix, "fsa_attrstat");

	dump_status(str, attrstat->status);
	if ( attrstat->status == FSA_OK )
	{
		dump_fattr(str, &attrstat->attributes);
	}

	easy_free(str);
}

void dump_filename(const char *prefix, fsa_filename_t *filename)
{
	char namestr[FSA_NAMELEN + 1];
	char *str = makeprefix(prefix, "fsa_filename");

	memset(namestr, '\0', FSA_NAMELEN + 1);
	memcpy(namestr, filename->name, filename->len);

	dumpUint32(str, "len", filename->len);
	dumpStr(str, "name", namestr);

	easy_free(str);
}

void dump_sattrargs(const char *prefix, fsa_sattrargs_t *sattrargs)
{
	char *str = makeprefix(prefix, "fsa_sattrargs");

	dumpUint32(str, "file", sattrargs->file);
	dump_sattr(str, &sattrargs->attributes);

	easy_free(str);
}

void dump_diropargs(const char *prefix, fsa_diropargs_t *diropargs)
{
	char *str = makeprefix(prefix, "fsa_diropargs");

	dumpUint32(str, "dir", diropargs->dir);
	dump_filename(str, &diropargs->name);

	easy_free(str);
}

void dump_createargs(const char *prefix, fsa_createargs_t *createargs)
{
	fsa_sattr_t *attributes;
	char *str = makeprefix(prefix, "fsa_createargs");

	dump_diropargs(str, &createargs->where);

	attributes = (fsa_sattr_t *)
		( ((char *) &createargs->attributes) +
		BYTES_TO_WORDALIGNED(createargs->where.name.len) );
	dump_sattr(str, attributes);

	easy_free(str);
}

void dump_diropres(const char *prefix, fsa_diropres_t *diropres)
{
	char *str = makeprefix(prefix, "fsa_diropres");

	dump_status(str, diropres->status);
	if ( diropres->status == FSA_OK )
	{
		dumpUint32(str, "file", diropres->dirop_ok.file);
		dump_fattr(str, &diropres->dirop_ok.attributes);
	}

	easy_free(str);
}

void dump_powerargs(const char *prefix, fsa_powerargs_t *powerargs)
{
	char *str = makeprefix(prefix, "fsa_powerargs");

	dumpBool32(str, "restart", powerargs->restart);

	easy_free(str);
}

void dump_readargs(const char *prefix, fsa_readargs_t *readargs)
{
	char *str = makeprefix(prefix, "fsa_readargs");

	dumpUint32(str, "file", readargs->file);
	dumpUint64(str, "offset", readargs->offset);
	dumpUint32(str, "count", readargs->count);

	easy_free(str);
}

void dump_readdirargs(const char *prefix, fsa_readdirargs_t *readdirargs)
{
	char *str = makeprefix(prefix, "fsa_readdirargs");

	dumpUint32(str, "dir", readdirargs->dir);
	dumpUint32(str, "cookie", readdirargs->cookie);
	dumpUint32(str, "count", readdirargs->count);

	easy_free(str);
}

void dump_readres(const char *prefix, fsa_readres_t *readres)
{
	char *str = makeprefix(prefix, "fsa_readres");

	dump_status(str, readres->status);
	if ( readres->status == FSA_OK )
	{
		dump_fattr(str, &readres->attributes);
		dump_data(str, &readres->data);
	}

	easy_free(str);

}

void dump_renameargs(const char *prefix, fsa_renameargs_t *renameargs)
{
	fsa_diropargs_t *to;
	char *str = makeprefix(prefix, "fsa_renameargs");

	dump_diropargs(str, &renameargs->from);

	to = (fsa_diropargs_t *)
		( ((char *) &renameargs->to) +
		BYTES_TO_WORDALIGNED(renameargs->from.name.len));
	dump_diropargs(str, to);

	easy_free(str);
}

void dump_writeargs(const char *prefix, fsa_writeargs_t *writeargs)
{
	char *str = makeprefix(prefix, "fsa_writeargs");

	dumpUint32(str, "file", writeargs->file);
	dumpUint64(str, "offset", writeargs->offset);
	dump_data(str, &writeargs->data);

	easy_free(str);
}

static void dump_header_req(fsa_packet_t *pkt)
{
	dump("header->transaction=%u", pkt->header.transaction);
	dump("length=curr - body=%u", pkt->curr - pkt->body);
	dump("header->library=%u", pkt->header.library);
	dump("header->procedure=%u (%s)", pkt->header.procedure,
		fsaProcedureToString(pkt->header.procedure));
}

static void dump_header_resp(fsa_packet_t *pkt)
{
	dump("header->transaction=%u", pkt->header.transaction);
	dump("header->length=%u", pkt->header.length);
	dump("header->library=%u", pkt->header.library);
	dump("header->procedure=%u (%s)", pkt->header.procedure,
		fsaProcedureToString(pkt->header.procedure));
}

// Dump function for each PROC request and response
/////////////////////////////////////////////////

/* We do not check that enough bytes exist in the body buffer. This is a
test lib and it will support testing to send incomplete packets.
*/

void dump_PROC_NULL_req(fsa_packet_t *req)
{
	dump_header_req(req);
	dump("PROC_NULL request has no body.");
}

void dump_PROC_NULL_resp(fsa_packet_t *resp)
{
	dump_header_resp(resp);
	dump("PROC_NULL response has no body.");
}

void dump_PROC_GETATTR_req(fsa_packet_t *req)
{
	fsa_fhandle_t *handle = (fsa_fhandle_t *) req->body;
	dump_header_req(req);
	dumpUint32("PROC_GETATTR_req", "handle", handle);
}

void dump_PROC_GETATTR_resp(fsa_packet_t *resp)
{
	fsa_attrstat_t *attrstat = (fsa_attrstat_t *) resp->body;
	dump_header_resp(resp);
	dump_attrstat("PROC_GETATTR_resp", attrstat);
}

void dump_PROC_SETATTR_req(fsa_packet_t *req)
{
	fsa_fhandle_t *handle = (fsa_fhandle_t *) req->body;
	dump_header_req(req);
	dumpUint32("PROC_GETATTR_req", "handle", handle);
}

void dump_PROC_SETATTR_resp(fsa_packet_t *resp)
{
	fsa_attrstat_t *attrstat = (fsa_attrstat_t *) resp->body;
	dump_header_resp(resp);
	dump_attrstat("PROC_GETATTR_resp", attrstat);
}

void dump_PROC_ROOT_req(fsa_packet_t *req)
{
	dump_header_req(req);
	dump("PROC_ROOT request has no body.");
}

void dump_PROC_ROOT_resp(fsa_packet_t *resp)
{
	fsa_diropres_t *diropres = (fsa_diropres_t *) resp->body;
	dump_header_resp(resp);
	dump_diropres("PROC_ROOT_resp", diropres);
}

void dump_PROC_LOOKUP_req(fsa_packet_t *req)
{
	fsa_diropargs_t *diropargs = (fsa_diropargs_t *) req->body;
	dump_header_req(req);
	dump_diropargs("PROC_LOOKUP_req", diropargs);
}

void dump_PROC_LOOKUP_resp(fsa_packet_t *resp)
{
	fsa_diropres_t *diropres = (fsa_diropres_t *) resp->body;
	dump_header_resp(resp);
	dump_diropres("PROC_LOOKUP_resp", diropres);
}

void dump_PROC_READ_req(fsa_packet_t *req)
{
	fsa_readargs_t *readargs = (fsa_readargs_t *) req->body;
	dump_header_req(req);
	dump_readargs("PROC_READ_req", readargs);
}

void dump_PROC_READ_resp(fsa_packet_t *resp)
{
	fsa_readres_t *readres = (fsa_readres_t *) resp->body;
	dump_header_resp(resp);
	dump_readres("PROC_READ_resp", readres);
}

void dump_PROC_WRITE_req(fsa_packet_t *req)
{
	fsa_writeargs_t *writeargs = (fsa_writeargs_t *) req->body;
	dump_header_req(req);
	dump_writeargs("PROC_WRITE_req", writeargs);
}

void dump_PROC_WRITE_resp(fsa_packet_t *resp)
{
	fsa_attrstat_t *attrstat = (fsa_attrstat_t *) resp->body;
	dump_header_resp(resp);
	dump_attrstat("PROC_WRITE_resp", attrstat);
}

void dump_PROC_CREATE_req(fsa_packet_t *req)
{
	fsa_createargs_t *createargs = (fsa_createargs_t *) req->body;
	dump_header_req(req);
	dump_createargs("PROC_CREATE_req", createargs);
}

void dump_PROC_CREATE_resp(fsa_packet_t *resp)
{
	fsa_diropres_t *diropres = (fsa_diropres_t *) resp->body;
	dump_header_resp(resp);
	dump_diropres("PROC_CREATE_resp", diropres);
}

void dump_PROC_REMOVE_req(fsa_packet_t *req)
{
	fsa_diropargs_t *diropargs = (fsa_diropargs_t *) req->body;
	dump_header_req(req);
	dump_diropargs("PROC_REMOVE_req", diropargs);
}

void dump_PROC_REMOVE_resp(fsa_packet_t *resp)
{
	fsa_stat_t *status = (fsa_stat_t *) resp->body;
	dump_header_resp(resp);
	dump_status("PROC_REMOVE_resp", *status);
}

void dump_PROC_RENAME_req(fsa_packet_t *req)
{
	fsa_renameargs_t *renameargs = (fsa_renameargs_t *) req->body;
	dump_header_req(req);
	dump_renameargs("PROC_RENAME_req", renameargs);
}

void dump_PROC_RENAME_resp(fsa_packet_t *resp)
{
	fsa_stat_t *status = (fsa_stat_t *) resp->body;
	dump_header_resp(resp);
	dump_status("PROC_RENAME_resp", *status);
}

void dump_PROC_MKDIR_req(fsa_packet_t *req)
{
	fsa_createargs_t *createargs = (fsa_createargs_t *) req->body;
	dump_header_req(req);
	dump_createargs("PROC_MKDIR_req", createargs);
}

void dump_PROC_MKDIR_resp(fsa_packet_t *resp)
{
	fsa_diropres_t *diropres = (fsa_diropres_t *) resp->body;
	dump_header_resp(resp);
	dump_diropres("PROC_MKDIR_resp", diropres);
}

void dump_PROC_RMDIR_req(fsa_packet_t *req)
{
	fsa_diropargs_t *diropargs = (fsa_diropargs_t *) req->body;
	dump_header_req(req);
	dump_diropargs("PROC_RMDIR_req", diropargs);
}

void dump_PROC_RMDIR_resp(fsa_packet_t *resp)
{
	fsa_stat_t *status = (fsa_stat_t *) resp->body;
	dump_header_resp(resp);
	dump_status("PROC_RMDIR_resp", *status);
}

void dump_PROC_READDIR_req(fsa_packet_t *req)
{
	fsa_readdirargs_t *readdirargs = (fsa_readdirargs_t *) req->body;
	dump_header_req(req);
	dump_readdirargs("PROC_READDIR_req", readdirargs);
}

void dump_PROC_READDIR_resp(fsa_packet_t *resp)
{
	dump_header_resp(resp);
	dump("Dump of fsa_readdirres_t is not implemented.");
}

void dump_PROC_POWERCTRL_req(fsa_packet_t *req)
{
	fsa_powerargs_t *powerargs = (fsa_powerargs_t *) req->body;

	dump_header_req(req);
	dump_powerargs("PROC_POWERCTRL_req", powerargs);
}

void dump_PROC_POWERCTRL_resp(fsa_packet_t *resp)
{
	dump_header_resp(resp);
	dump("PROC_POWERCTRL response has no body.");
}

static void dump_unknown_req(fsa_packet_t *req)
{
	dump_header_req(req);
	dump("Unknown FSA request procedure.");
}

static void dump_unknown_resp(fsa_packet_t *resp)
{
	dump_header_req(resp);
	dump("Unknown FSA response procedure.");
}

#define CASE_DUMP_REQUEST(proc) \
	case proc:\
		dump_##proc##_req(req); \
		break;
void dump_request(fsa_packet_t *req)
{
	dump("FSA request packet:");

	switch ( req->header.procedure )
	{
		CASE_DUMP_REQUEST(PROC_NULL)
		CASE_DUMP_REQUEST(PROC_GETATTR)
		CASE_DUMP_REQUEST(PROC_SETATTR)
		CASE_DUMP_REQUEST(PROC_ROOT)
		CASE_DUMP_REQUEST(PROC_LOOKUP)
		CASE_DUMP_REQUEST(PROC_READ)
		CASE_DUMP_REQUEST(PROC_WRITE)
		CASE_DUMP_REQUEST(PROC_CREATE)
		CASE_DUMP_REQUEST(PROC_REMOVE)
		CASE_DUMP_REQUEST(PROC_RENAME)
		CASE_DUMP_REQUEST(PROC_MKDIR)
		CASE_DUMP_REQUEST(PROC_RMDIR)
		CASE_DUMP_REQUEST(PROC_READDIR)
		CASE_DUMP_REQUEST(PROC_POWERCTRL)

		default:
			dump_unknown_req(req);
	}
}
#undef CASE_DUMP_REQUEST

#define CASE_DUMP_RESPONSE(proc) \
	case proc:\
		dump_##proc##_resp(resp); \
		break;
void dump_response(fsa_packet_t *resp)
{
	dump("FSA response packet:");

	switch ( resp->header.procedure )
	{
		CASE_DUMP_RESPONSE(PROC_NULL)
		CASE_DUMP_RESPONSE(PROC_GETATTR)
		CASE_DUMP_RESPONSE(PROC_SETATTR)
		CASE_DUMP_RESPONSE(PROC_ROOT)
		CASE_DUMP_RESPONSE(PROC_LOOKUP)
		CASE_DUMP_RESPONSE(PROC_READ)
		CASE_DUMP_RESPONSE(PROC_WRITE)
		CASE_DUMP_RESPONSE(PROC_CREATE)
		CASE_DUMP_RESPONSE(PROC_REMOVE)
		CASE_DUMP_RESPONSE(PROC_RENAME)
		CASE_DUMP_RESPONSE(PROC_MKDIR)
		CASE_DUMP_RESPONSE(PROC_RMDIR)
		CASE_DUMP_RESPONSE(PROC_READDIR)
		CASE_DUMP_RESPONSE(PROC_POWERCTRL)

		default:
			dump_unknown_resp(resp);
	}
}
#undef CASE_DUMP_RESPONSE

unsigned nexttransaction(void)
{
	static unsigned trans = 0;

	//Start the uniq transactions at 1
	trans++;

	if ( trans > ((1 << 10) - 1) )
		trans = 1;

	return trans;
}

int maptoerrno(uint32_t status)
{
	switch (status)
	{
		case FSA_OK:
			return 0;

		case ERR_ACCESS:
			return EACCES;

		case ERR_NOENT:
			return ENOENT;

		case ERR_IO:
			return EIO;

		case ERR_EXIST:
			return EEXIST;

		case ERR_NOTDIR:
			return ENOTDIR;

		case ERR_ISDIR:
			return EISDIR;

		case ERR_FBIG:
			return EFBIG;

		case ERR_NOSPC:
			return ENOSPC;

		case ERR_NAMETOOLONG:
			return ENAMETOOLONG;

		case ERR_NOTEMPTY:
			return ENOTEMPTY;

		case ERR_STALE:
			return EBADF;

		default:
			return EIO;
	}
}

int connectToMsa(void)
{
	union {
		struct sockaddr sa;
		struct sockaddr_un sa_un;
	} addr;
	int sock, res;

	assert(1 + strlen(RPC_DEVICE_FILE) + 1 < sizeof(addr.sa_un.sun_path));

	sock = posix_socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (sock < 0)
	{
		logFATAL("Can not create RPC device socket");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0x00, sizeof(addr));
	addr.sa_un.sun_family = AF_UNIX;
	strcpy(addr.sa_un.sun_path + 1, RPC_DEVICE_FILE); //Use an anonymous socket

	res = posix_connect(RPC_DEVICE_FILE, sock, &addr.sa, sizeof(addr.sa_un));
	CHECK_NOT_EQUAL(res, -1);

	return sock;
}

void sendAndReceive(
	MsaClientEnv_t *env,
	fsa_packet_t   *req,
	fsa_packet_t   *resp)
{
	bool res;

	//Fill last parts of header
	fill_header(req, nexttransaction());
	CHECK_EQUAL(req->header.library, 1);

	//debug
	dump_request(req);

	//Send request
	res = fsa_sendpacket(env->socket, req);
	CHECK_EQUAL(res, true);

	//Receive reply
	res = fsa_receivepacket(env->socket, resp);
	CHECK_EQUAL(res, true);

	//Check that we received the RESPONSE to the former REQUEST
	CHECK_EQUAL(resp->header.library, 1);
	CHECK_EQUAL(req->header.procedure, resp->header.procedure);
	CHECK_EQUAL(req->header.transaction, resp->header.transaction);

	//debug
	dump_response(resp);
}

void ping(MsaClientEnv_t *env)
{
	fsa_packet_t req;
	fsa_packet_t resp;

	logINFO("Trying to ping the MSA server.");

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_NULL_req(&req);
	CHECK_EQUAL(req.bodylen, 0);

	sendAndReceive(env, &req, &resp);

	//parse_PROC_NULL_resp:  no body, no need to parse it
	CHECK_EQUAL(resp.bodylen, 0);

	fsa_freepacket(&req);
	fsa_freepacket(&resp);

	logINFO("Successfully pinged the MSA server!");
}

fsa_stat_t lookupname(
	MsaClientEnv_t *env,
	fsa_fhandle_t dir, const char *name, fsa_fhandle_t *handle)
{
	fsa_packet_t req;
	fsa_packet_t resp;
	fsa_diropres_t  *diropres;
	fsa_stat_t   status;

	*handle = INVALID_HANDLE;

	if (strlen(name) > 12)
	{
		status = ERR_NOENT;
		logDBG2("name='%s' is too long. I will claiming it doesn't exist!",
			name);
		logDBG3("lookupname dir=%u name='%s' -> handle=%u status=%u",
			dir, name, *handle, status);
		goto Error;
	}

	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	fill_PROC_LOOKUP_req(&req, dir, name);
	sendAndReceive(env, &req, &resp);
	status = parse_PROC_LOOKUP_resp(&resp, &diropres);
	if ( unlikely(status != FSA_OK) )
		goto End;

	status = diropres->status;
	if ( unlikely(status != FSA_OK) )
		goto End;

	*handle = diropres->dirop_ok.file;

	logDBG3("lookupname dir=%u name='%s' -> handle=%u status=%u",
		dir, name, *handle, status);

End:
	fsa_freepacket(&req);
	fsa_freepacket(&resp);

Error:
	return status;
}


fsa_stat_t lookuppath(
	MsaClientEnv_t *env,
	const char *abspath, fsa_fhandle_t *handle)
{
	const char *token;
	char *savePtr = NULL;
	char *mypath = easy_strdup(abspath);

	//If we are trying to lookup '/' we will just return the env->root_handle
	fsa_stat_t status = FSA_OK;

	assert(abspath[0] == '/');

	*handle = env->root_handle;

	token = strtok_r(mypath, "/", &savePtr);
	while ( token )
	{
		status = lookupname(env, *handle, token, handle);
		if ( status != FSA_OK )
		{
			logERROR("Path='%s token='%s' NOT FOUND!",
				abspath, token);
			break;
		}
		token = strtok_r(NULL, "/", &savePtr);
	}

	easy_free(mypath);

	if ( unlikely(status != FSA_OK) )
		logERROR("Error looking up path='%s'. Status=%u", abspath, status);

	logDBG2("lookuppath path='%s' -> handle=%u status=%u",
		abspath, *handle, status);
	return status;
}



