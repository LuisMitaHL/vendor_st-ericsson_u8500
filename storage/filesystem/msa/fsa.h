#ifndef _FSA_H
#define _FSA_H

/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

/**********************************************
 * Constants
 **********************************************/
/**********************************************
 * FSA protocol types
 **********************************************/

//Requests

enum
{
	PROC_NULL		=  0,
	PROC_GETATTR	=  1,
	PROC_SETATTR	=  2,
	PROC_ROOT		=  3,
	PROC_LOOKUP		=  4,
	PROC_READ		=  5,
	PROC_WRITE		=  6,
	PROC_CREATE		=  7,
	PROC_REMOVE		=  8,
	PROC_RENAME		=  9,
	PROC_MKDIR		= 10,
	PROC_RMDIR		= 11,
	PROC_READDIR	= 12,
	PROC_POWERCTRL	= 13,

	PROC_MAX,
};

//Primitive types

/* A boolean is an enum, and an enum is a 32bit signed entity.
   We'll treat both as unsigned however, and expect no ill effects from that. */
typedef uint64_t fsa_offset_t;
typedef uint32_t fsa_count_t;
typedef uint32_t fsa_bool_t;
typedef uint32_t fsa_enum_t;

enum
{
	FSA_OK			=  0,
	ERR_ACCESS		=  1,
	ERR_NOENT		=  2,
	ERR_IO			=  3,
	ERR_EXIST		=  4,
	ERR_NOTDIR		=  5,
	ERR_ISDIR		=  6,
	ERR_FBIG		=  7,
	ERR_NOSPC		=  8,
	ERR_NAMETOOLONG	=  9,
	ERR_NOTEMPTY	= 10,
	ERR_STALE		= 11,

	ERR_MAX,

	//There is no error for invalid parameter. We need this error in order to
	// report malformed incoming packets. It is the same as ERR_IO, meaning we
	// can never match to this.
	PARAM_INVALID = ERR_IO,
};
typedef fsa_enum_t fsa_stat_t;

enum
{
	FTYPE_NONE	=  0,
	FTYPE_REG	=  1,
	FTYPE_DIR	=  2,
};
typedef fsa_enum_t fsa_ftype_t;

enum
{
	//In octal
	FMODE_READ	= 0400,
	FMODE_WRITE	= 0200,
	FMODE_EXEC	= 0100,
};
typedef uint32_t fsa_mode_t;
#define MODE_IGNORE UINT32_MAX

typedef uint64_t fsa_size_t;
#define SIZE_IGNORE UINT64_MAX

typedef uint32_t fsa_time_t;
#define TIME_IGNORE UINT32_MAX

/* When first receiving a FSA packet,  a wake lock will be acquired if it is
   not already held. This is the time in milliseconds that FSA will wait,
   after sending a response, for the next incoming FSA packet, before
   releasing a held wake lock. */
#define FSA_WAKELOCK_TIMEOUT_MS	(1000)

/* This is the maximum size an FSA packet (including its header) can have.
   We can only find this figure in the old MSA implementation (4096 bytes)
   and kernel code (128Kbyte), not in the protocol. Since the /dev/rpc
   device is not stream based, but rather packet based, we need to know how
   big the biggest packet can be in order to allocate a buffer for it.
   It is not really a problem that this buffer i huge. Linux will only allocate
   space for it in the memory mapping table (MMU) but won't start populate it
   with actual memory pages until we write data into it.*/
#define FSA_MAXPACKETLEN (128*1024)

#define FSA_HEADERSIZE (2 * sizeof(uint32_t))

//It is most likely not needed to pack the structues as everything is on 32 bit
// boundry. However, we can't be careful enough
#define PACKED __attribute__((__packed__))

typedef struct PACKED fsa_timeval
{
	fsa_time_t seconds;
	fsa_time_t useconds;
} fsa_timeval_t;

typedef struct PACKED fsa_fattr
{
	fsa_size_t    size;
	fsa_mode_t    mode;
	fsa_timeval_t ctime;
	fsa_ftype_t   type;
}  fsa_fattr_t;

typedef struct PACKED fsa_sattr
{
	fsa_size_t    size;
	fsa_mode_t    mode;
	fsa_timeval_t ctime;
}  fsa_sattr_t;

typedef struct PACKED fsa_attrstat
{
	fsa_stat_t  status;

	//The following is only here/valid if status == FSA_OK
	fsa_fattr_t attributes; //64 + 32 + 2*32 + 32 bit = 8*4 bytes = 24 B
}  fsa_attrstat_t;

typedef uint32_t fsa_fhandle_t;
#define INVALID_HANDLE 0
#define ROOT_HANDLE 1

//The maximum filename length in bytes
#define FSA_NAMELEN 12

typedef struct PACKED fsa_filename
{
	fsa_count_t len;
	char        name[0]; //Name of 4, 8 or 12 bytes is added, padded with zeros
} fsa_filename_t;

typedef struct PACKED fsa_sattrargs
{
	fsa_fhandle_t file;
	fsa_sattr_t   attributes;
} fsa_sattrargs_t;

typedef struct PACKED fsa_diropargs {
	fsa_fhandle_t  dir;
	fsa_filename_t name;
} fsa_diropargs_t;

typedef struct PACKED fsa_createargs {
	fsa_diropargs_t where;
	fsa_sattr_t     attributes;
} fsa_createargs_t;

typedef struct PACKED fsa_diropok {
	fsa_fhandle_t file;
	fsa_fattr_t attributes;
} fsa_diropok_t;

typedef struct PACKED fsa_diropres {
	fsa_stat_t    status;

	//The following is only here/valid if status == FSA_OK
	fsa_diropok_t dirop_ok;
} fsa_diropres_t;

typedef struct PACKED fsa_powerargs {
	fsa_bool_t restart;
} fsa_powerargs_t;

typedef struct PACKED fsa_readargs {
	fsa_fhandle_t file;
	fsa_offset_t  offset;
	fsa_count_t   count;
} fsa_readargs_t;

typedef uint32_t fsa_cookie_t;
#define	COOKIE_FIRST 0
#define	COOKIE_LAST	UINT32_MAX

typedef struct PACKED fsa_readdirargs {
	fsa_fhandle_t dir;
	fsa_cookie_t  cookie;
	fsa_count_t   count;
} fsa_readdirargs_t;

/* fsa_direntry_t and fsa_readdirres_t are parsed incrementally instead of as a
   complete struct all at once. See process_readdir(). */

typedef struct PACKED fsa_data
{
	fsa_count_t len;
	uint8_t     data[0]; //data is padded with zero-bytes up to a length
						 // of a multiple of 4 bytes
} fsa_data_t;

struct read_ok {
	fsa_fattr_t attributes;
	fsa_data_t data;
};

typedef struct PACKED fsa_readres {
	fsa_stat_t  status;

	//The following is only here/valid if status == FSA_OK
	fsa_fattr_t attributes;
	fsa_data_t  data;
} fsa_readres_t;

typedef struct PACKED fsa_renameargs
{
	fsa_diropargs_t from;
	fsa_diropargs_t to;
} fsa_renameargs_t;

typedef struct PACKED fsa_writeargs
{
	fsa_fhandle_t file;
	fsa_offset_t  offset;
	fsa_data_t    data;
} fsa_writeargs_t;

#undef PACKED //Not needed anymore

typedef struct fsa_header
{
	unsigned int length;
	unsigned int transaction;
	unsigned int procedure;
	unsigned int library;
} fsa_header_t;

typedef struct fsa_packet
{
	uint8_t      *buf; //The full packet buffer, header followed by data

	//Parsed out header content
	fsa_header_t  header;

	//Data body
	uint8_t      *body;
	uint8_t      *curr; //Work pointer for unpack/pack functions
	size_t        bodylen; //Length of body in bytes, to be unpacked or sent
} fsa_packet_t;


//Used by PROC_READDIR to keep track of the state when filling a response
typedef struct dirent_filler
{
	size_t        count; //max size
	size_t        used; //Used bytes of count
	fsa_cookie_t  currCookie;
	fsa_cookie_t *cookiePtr; //Pointer to place of the last cookie
} dirent_filler_t;

//Requesting PROC_READDIR and giving a count less than this can result in
// only empty replies. In worst case, a file name is exactly 12 chars long. So
// we need: 4 (len) + 12 (filename) + 4 (cookie) + 4 (bool) bytes.
//We will otherwise risk not being able to return the next filename. Meaning we
// are forced to reply with and empty reply, with status OK even when there are
// more entries. It is better that we then reply with an error.
#define SMALLEST_READDIR_COUNT (4 + 12 + 4 + 4)

/**********************************************
 * Functions
 **********************************************/

//Will round upwards to closest multiple of 4 bytes
#define BYTES_TO_WORDALIGNED(len) ((len + 3) & (~0x3))

//Filling in packet header
void fill_header(fsa_packet_t *req, unsigned transaction);

//fsa packet handling functions
void fsa_allocpacket(fsa_packet_t *pkt);
void fsa_freepacket(fsa_packet_t *pkt);
bool fsa_receivepacket(int dev, fsa_packet_t *pkt);
bool fsa_sendpacket(int dev, fsa_packet_t *pkt);


//Conversion functions between Linux stat info and FSA protocol
fsa_mode_t linux2fsa_mode(mode_t mode);
mode_t fsa2linux_mode(fsa_mode_t fsamode);

fsa_ftype_t linux2fsa_ftype(mode_t mode);
mode_t fsa2linux_ftype(fsa_ftype_t ftype);

void linux2fsa_fattr(const struct stat *stbuf, fsa_fattr_t *fattr);
void fsa2linux_fattr(const fsa_fattr_t *fattr, struct stat *stbuf);

const char * fsaStatusToString(fsa_stat_t status);
const char * fsaProcedureToString(unsigned proc);


/*
 * Functions used to fill and parse request and response packets
 */


// PROC_NULL = 0
/////////////////
void fill_PROC_NULL_req(
	fsa_packet_t *req);

//Since PROC_NULL request has no body, parsing of it is not needed

void fill_PROC_NULL_resp(
	fsa_packet_t *resp);

//Since PROC_NULL response has no body, parsing of it is not needed


// PROC_GETATTR = 1
////////////////////
void fill_PROC_GETATTR_req(
	fsa_packet_t *req,
	fsa_fhandle_t handle);

fsa_stat_t parse_PROC_GETATTR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *handle);

void fill_PROC_GETATTR_resp(
	fsa_packet_t *resp,
	fsa_stat_t    status,
	fsa_fattr_t  *fattr);

fsa_stat_t parse_PROC_GETATTR_resp(
	fsa_packet_t    *resp,
	fsa_attrstat_t **attrstat);


// PROC_SETATTR = 2
////////////////////
void fill_PROC_SETATTR_req(
	fsa_packet_t *req,
	fsa_fhandle_t handle,
	uint64_t      size,
	uint32_t      mode,
	uint32_t      seconds);

fsa_stat_t parse_PROC_SETATTR_req(
	fsa_packet_t     *req,
	fsa_sattrargs_t **sattrargs);

void fill_PROC_SETATTR_resp(
	fsa_packet_t *resp,
	fsa_stat_t    status,
	fsa_fattr_t  *fattr);

fsa_stat_t parse_PROC_SETATTR_resp(
	fsa_packet_t    *resp,
	fsa_attrstat_t **attrstat);


// PROC_ROOT = 3
/////////////////
void fill_PROC_ROOT_req(
	fsa_packet_t *req);

//Since PROC_ROOT request has no body, parsing of it is not needed

void fill_PROC_ROOT_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status,
	fsa_fhandle_t  handle,
	fsa_fattr_t   *attr);

fsa_stat_t parse_PROC_ROOT_resp(
	fsa_packet_t    *resp,
	fsa_diropres_t **diropres);


// PROC_LOOKUP = 4
///////////////////
void fill_PROC_LOOKUP_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  dir,
	const char    *name);

fsa_stat_t parse_PROC_LOOKUP_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1]);

void fill_PROC_LOOKUP_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status,
	fsa_fhandle_t  handle,
	fsa_fattr_t   *attr);

fsa_stat_t parse_PROC_LOOKUP_resp(
	fsa_packet_t	*resp,
	fsa_diropres_t **diropres);


// PROC_READ = 5
/////////////////
void fill_PROC_READ_req(
	fsa_packet_t       *req,
	const fsa_fhandle_t file,
	const uint64_t	    offset,
	const uint32_t	    count);

fsa_stat_t parse_PROC_READ_req(
	fsa_packet_t    *req,
	fsa_readargs_t **readargs);

//Will only reserve space and return pointers to the spot in the PROC_READ
// response where data should be filled in
void fill_PROC_READ_resp_start(
	fsa_packet_t    *resp,
	fsa_count_t      count,
	fsa_readres_t  **readresSpot,
	uint8_t        **dataSpot);

void fill_PROC_READ_resp_end(
	fsa_packet_t   *resp,
	uint32_t        datalen);

void fill_PROC_READ_resp_failed(
	fsa_packet_t   *resp,
	fsa_stat_t      status);

fsa_stat_t parse_PROC_READ_resp(
	fsa_packet_t    *resp,
	fsa_attrstat_t **attrstat,
	uint32_t        *len,
	uint8_t        **data);


// PROC_WRITE = 6
//////////////////
void fill_PROC_WRITE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  file,
	uint64_t	   offset,
	const uint8_t *data,
	uint32_t	   datalen);

fsa_stat_t parse_PROC_WRITE_req(
	fsa_packet_t     *req,
	fsa_writeargs_t **writeargs);

void fill_PROC_WRITE_resp(
	fsa_packet_t *resp,
	fsa_stat_t    status,
	fsa_fattr_t  *fattr);

fsa_stat_t parse_PROC_WRITE_resp(
	fsa_packet_t    *resp,
	fsa_attrstat_t **attrstat);


// PROC_CREATE = 7
///////////////////
void fill_PROC_CREATE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  dir,
	const char    *name,
	uint64_t	   size,
	uint32_t	   mode,
	uint32_t	   seconds);

fsa_stat_t parse_PROC_CREATE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1],
	fsa_sattr_t  **sattr);

void fill_PROC_CREATE_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status,
	fsa_fhandle_t  handle,
	fsa_fattr_t   *attr);

fsa_stat_t parse_PROC_CREATE_resp(
	fsa_packet_t    *resp,
	fsa_diropres_t **diropres);


// PROC_REMOVE = 8
////////////////////
void fill_PROC_REMOVE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  dir,
	const char    *name);

fsa_stat_t parse_PROC_REMOVE_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1]);

void fill_PROC_REMOVE_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status);

fsa_stat_t parse_PROC_REMOVE_resp(
	fsa_packet_t    *resp,
	fsa_stat_t      *status);


// PROC_RENAME = 9
////////////////////
void fill_PROC_RENAME_req( // = 9
	fsa_packet_t  *req,
	fsa_fhandle_t  fromHandle,
	const char    *fromName,
	fsa_fhandle_t  toHandle,
	const char    *toName);

fsa_stat_t parse_PROC_RENAME_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *fromHandle,
	char           fromName[FSA_NAMELEN + 1],
	fsa_fhandle_t *toHandle,
	char           toName[FSA_NAMELEN + 1]);

void fill_PROC_RENAME_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status);

fsa_stat_t parse_PROC_RENAME_resp(
	fsa_packet_t    *resp,
	fsa_stat_t      *status);


// PROC_MKDIR = 10
////////////////////
void fill_PROC_MKDIR_req(
	fsa_packet_t     *req,
	fsa_fhandle_t     dir,
	const char       *name,
	uint32_t	      mode,
	uint32_t	      seconds);

fsa_stat_t parse_PROC_MKDIR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1],
	fsa_sattr_t  **sattr);

void fill_PROC_MKDIR_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status,
	fsa_fhandle_t  handle,
	fsa_fattr_t   *attr);

fsa_stat_t parse_PROC_MKDIR_resp(
	fsa_packet_t    *resp,
	fsa_diropres_t **diropres);


// PROC_RMDIR = 11
////////////////////
void fill_PROC_RMDIR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t  dir,
	const char    *name);

fsa_stat_t parse_PROC_RMDIR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	char           name[FSA_NAMELEN + 1]);

void fill_PROC_RMDIR_resp(
	fsa_packet_t  *resp,
	fsa_stat_t     status);

fsa_stat_t parse_PROC_RMDIR_resp(
	fsa_packet_t    *resp,
	fsa_stat_t      *status);


// PROC_READDIR = 12
////////////////////
void fill_PROC_READDIR_req(
	fsa_packet_t *req,
	fsa_fhandle_t dir,
	fsa_cookie_t  cookie,
	uint32_t      count);

fsa_stat_t parse_PROC_READDIR_req(
	fsa_packet_t  *req,
	fsa_fhandle_t *dir,
	fsa_cookie_t  *cookie,
	uint32_t      *count);

void fill_PROC_READDIR_resp_start(
	fsa_packet_t    *resp,
	dirent_filler_t *filler,
	fsa_count_t      count);

bool fill_PROC_READDIR_resp_dirent(
	fsa_packet_t    *resp,
	dirent_filler_t *filler,
	struct dirent   *dirent);

void fill_PROC_READDIR_resp_end(
	fsa_packet_t   *resp);

void fill_PROC_READDIR_resp_failed(
	fsa_packet_t   *resp,
	fsa_stat_t      status);


fsa_stat_t parse_PROC_READDIR_resp_start(
	fsa_packet_t  *req,
	fsa_stat_t    *status);

fsa_stat_t parse_PROC_READDIR_resp_dirent(
	fsa_packet_t  *req,
	fsa_bool_t    *hasmore,
	char		   filename[FSA_NAMELEN + 1],
	fsa_cookie_t  *cookie);


// PROC_POWERCTRL = 13
////////////////////
void fill_PROC_POWERCTRL_req(
	fsa_packet_t  *req,
	bool           restart);

fsa_stat_t parse_PROC_POWERCTRL_req(
	fsa_packet_t  *req,
	bool		  *restart);

void fill_PROC_POWERCTRL_resp(
	fsa_packet_t *resp);

//Since PROC_POWERCTRL response has no body, parsing of it is not needed

#endif // _FSA_H

