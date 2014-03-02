#ifndef _TESTCLIENT_H
#define _TESTCLIENT_H


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

typedef struct MsaClientEnv_st
{
	int socket;
	fsa_fhandle_t root_handle;
} MsaClientEnv_t;


#define dump(fmt, args...) logDBG1("Dump: " fmt, ##args)

#define dumpUint16(prefix, name, var) \
	dump("%s uint16_t %s=%u", prefix, name, var)
#define dumpInt32(prefix, name, var) \
	dump("%s int32_t %s=%d", prefix, name, var)
#define dumpUint32(prefix, name, var) \
	dump("%s uint32_t %s=%u", prefix, name, var)
#define dumpUint64(prefix, name, var) \
	dump("%s uint64_t %s=%llu", prefix, name, var)
#define dumpHex32(prefix, name, var) \
	dump("%s uint32_t %s=%#x", prefix, name, var)
#define dumpBool32(prefix, name, var) \
	dump("%s bool32 %s=%u", prefix, name, var)
#define dumpStr(prefix, name, var) \
	dump("%s string %s='%s'", prefix, name, var)

void dump_status(const char *prefix, fsa_stat_t status);
void dump_data(const char *prefix, fsa_data_t *data);
void dump_timeval(const char *prefix, fsa_timeval_t *timeval);
void dump_fattr(const char *prefix, fsa_fattr_t *fattr);
void dump_sattr(const char *prefix, fsa_sattr_t *sattr);
void dump_attrstat(const char *prefix, fsa_attrstat_t *attrstat);
void dump_filename(const char *prefix, fsa_filename_t *filename);
void dump_sattrargs(const char *prefix, fsa_sattrargs_t *sattrargs);
void dump_diropargs(const char *prefix, fsa_diropargs_t *diropargs);
void dump_createargs(const char *prefix, fsa_createargs_t *createargs);
void dump_diropres(const char *prefix, fsa_diropres_t *diropres);
void dump_powerargs(const char *prefix, fsa_powerargs_t *powerargs);
void dump_readargs(const char *prefix, fsa_readargs_t *readargs);
void dump_readdirargs(const char *prefix, fsa_readdirargs_t *readdirargs);
void dump_readres(const char *prefix, fsa_readres_t *readres);
void dump_renameargs(const char *prefix, fsa_renameargs_t *renameargs);
void dump_writeargs(const char *prefix, fsa_writeargs_t *writeargs);
void dump_header(fsa_packet_t *pkt);


// Dump function for each PROC request and response
/////////////////////////////////////////////////

/* We do not check that enough bytes exist in the body buffer. This is a
test lib and it will support testing to send incomplete packets.
*/

void dump_PROC_NULL_req(fsa_packet_t *req);
void dump_PROC_NULL_resp(fsa_packet_t *resp);
void dump_PROC_GETATTR_req(fsa_packet_t *req);
void dump_PROC_GETATTR_resp(fsa_packet_t *resp);
void dump_PROC_SETATTR_req(fsa_packet_t *req);
void dump_PROC_SETATTR_resp(fsa_packet_t *resp);
void dump_PROC_ROOT_req(fsa_packet_t *req);
void dump_PROC_ROOT_resp(fsa_packet_t *resp);
void dump_PROC_LOOKUP_req(fsa_packet_t *req);
void dump_PROC_LOOKUP_resp(fsa_packet_t *resp);
void dump_PROC_READ_req(fsa_packet_t *req);
void dump_PROC_READ_resp(fsa_packet_t *resp);
void dump_PROC_WRITE_req(fsa_packet_t *req);
void dump_PROC_WRITE_resp(fsa_packet_t *resp);
void dump_PROC_CREATE_req(fsa_packet_t *req);
void dump_PROC_CREATE_resp(fsa_packet_t *resp);
void dump_PROC_REMOVE_req(fsa_packet_t *req);
void dump_PROC_REMOVE_resp(fsa_packet_t *resp);
void dump_PROC_RENAME_req(fsa_packet_t *req);
void dump_PROC_RENAME_resp(fsa_packet_t *resp);
void dump_PROC_MKDIR_req(fsa_packet_t *req);
void dump_PROC_MKDIR_resp(fsa_packet_t *resp);
void dump_PROC_RMDIR_req(fsa_packet_t *req);
void dump_PROC_RMDIR_resp(fsa_packet_t *resp);
void dump_PROC_READDIR_req(fsa_packet_t *req);
void dump_PROC_READDIR_resp(fsa_packet_t *resp);
void dump_PROC_POWERCTRL_req(fsa_packet_t *req);
void dump_PROC_POWERCTRL_resp(fsa_packet_t *resp);
void dump_unknown(fsa_packet_t *pkt);
void dump_request(fsa_packet_t *req);
void dump_response(fsa_packet_t *resp);

unsigned nexttransaction(void);

int maptoerrno(uint32_t status);

int connectToMsa(void);

void sendAndReceive(
	MsaClientEnv_t *env,
	fsa_packet_t   *req,
	fsa_packet_t   *resp);

void ping(MsaClientEnv_t *env);

fsa_stat_t lookupname(
	MsaClientEnv_t *env,
	fsa_fhandle_t dir, const char *name, fsa_fhandle_t *handle);
fsa_stat_t lookuppath(
	MsaClientEnv_t *env,
	const char *abspath, fsa_fhandle_t *handle);

#endif // _TESTCLIENT_H
