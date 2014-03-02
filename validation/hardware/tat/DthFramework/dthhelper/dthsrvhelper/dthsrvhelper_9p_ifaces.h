/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   9P interfaces for DTH server helper
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <dirent.h>

extern struct dth_directory *rootDir;

typedef struct Fid Fid;

struct Fid {
	char *path;
	int omode;
	int fd;
	DIR *dir;
	int diroffset;
	char *direntname;
	void *aux;
	struct stat stat;
};

#define MAXWELEM	16

typedef struct Spstr Spstr;
typedef struct Spqid Spqid;
typedef struct Spstat Spstat;
typedef struct Spwstat Spwstat;
typedef struct Spfcall Spfcall;
typedef struct Spfid Spfid;
typedef struct Spbuf Spbuf;
typedef struct Sptrans Sptrans;
typedef struct Spconn Spconn;
typedef struct Spreq Spreq;
typedef struct Spwthread Spwthread;
typedef struct Spauth Spauth;
typedef struct Spsrv Spsrv;
typedef struct Spuser Spuser;
typedef struct Spgroup Spgroup;
typedef struct Spfile Spfile;
typedef struct Spfilefid Spfilefid;
typedef struct Spfileops Spfileops;
typedef struct Spdirops Spdirops;
typedef struct Spfd Spfd;

/* permissions */
enum {
	Dmdir		= 0x80000000,
	Dmappend	= 0x40000000,
	Dmexcl		= 0x20000000,
	Dmmount		= 0x10000000,
	Dmauth		= 0x08000000,
	Dmtmp		= 0x04000000,
	Dmsymlink	= 0x02000000,
	Dmlink		= 0x01000000,

	/* 9P2000.u extensions */
	Dmdevice	= 0x00800000,
	Dmnamedpipe	= 0x00200000,
	Dmsocket	= 0x00100000,
	Dmsetuid	= 0x00080000,
	Dmsetgid	= 0x00040000,
};

struct Spstr {
	uint16_t len;
	char *str;
};

struct Spqid {
	uint8_t		type;
	uint32_t		version;
	uint64_t		path;
};

struct Spwstat {
	uint16_t	size;
	uint16_t	type;
	uint32_t		dev;
	Spqid		qid;
	uint32_t		mode;
	uint32_t		atime;
	uint32_t		mtime;
	uint64_t		length;
	char *name;
	char *uid;
	char *gid;
	char *muid;
	char *extension;	/* 9p2000.u extensions */
	uint32_t		n_uid;		/* 9p2000.u extensions */
	uint32_t		n_gid;		/* 9p2000.u extensions */
	uint32_t		n_muid;		/* 9p2000.u extensions */
};

struct Spstat {
	uint16_t		size;
	uint16_t		type;
	uint32_t		dev;
	Spqid		qid;
	uint32_t		mode;
	uint32_t		atime;
	uint32_t		mtime;
	uint64_t		length;
	Spstr		name;
	Spstr		uid;
	Spstr		gid;
	Spstr		muid;

	/* 9P2000.u extensions */
	Spstr		extension;
	uint32_t		n_uid;
	uint32_t		n_gid;
	uint32_t		n_muid;
};

struct Spfcall {
	uint32_t		size;
	uint8_t		type;
	uint16_t		tag;
	uint8_t *pkt;

	uint32_t		fid;
	uint32_t		msize;			/* Tversion, Rversion */
	Spstr		version;		/* Tversion, Rversion */
	uint32_t		afid;			/* Tauth, Tattach */
	Spstr		uname;			/* Tauth, Tattach */
	Spstr		aname;			/* Tauth, Tattach */
	Spqid		qid;			/* Rauth, Rattach, Ropen, Rcreate */
	Spstr		ename;			/* Rerror */
	uint16_t		oldtag;			/* Tflush */
	uint32_t		newfid;			/* Twalk */
	uint16_t		nwname;			/* Twalk */
	Spstr		wnames[MAXWELEM];	/* Twalk */
	uint16_t		nwqid;			/* Rwalk */
	Spqid		wqids[MAXWELEM];	/* Rwalk */
	uint8_t		mode;			/* Topen, Tcreate */
	uint32_t		iounit;			/* Ropen, Rcreate */
	Spstr		name;			/* Tcreate */
	uint32_t		perm;			/* Tcreate */
	uint64_t		offset;			/* Tread, Twrite */
	uint32_t		count;			/* Tread, Rread, Twrite, Rwrite */
	uint8_t *data;			/* Rread, Twrite */
	Spstat		stat;			/* Rstat, Twstat */

	/* 9P2000.u extensions */
	uint32_t		ecode;			/* Rerror */
	Spstr		extension;		/* Tcreate */
	uint32_t		n_uname;		/* Tauth, Tattach */

	Spfcall *next;
};

struct Spfid {
	Spconn *conn;
	uint32_t		fid;
	int		refcount;
	uint16_t		omode;
	uint8_t		type;
	uint32_t		diroffset;
	Spuser *user;
	uint32_t		dev;	/* used by cellfs and kvmfs */
	void *aux;

	Spfid *next;	/* list of fids within a bucket */
};

struct Spauth {
	int		(*startauth)(Spfid *afid, char *aname, Spqid *aqid);
	int		(*checkauth)(Spfid *fid, Spfid *afid, char *aname);
	int		(*read)(Spfid *afid, unsigned long long offset, unsigned long count, unsigned char *data);
	int		(*write)(Spfid *afid, unsigned long long offset, unsigned long count, unsigned char *data);
	int		(*clunk)(Spfid *afid);
};

struct Spsrv {
	unsigned int		msize;
	int		dotu;		/* 9P2000.u support flag */
	void *srvaux;
	void *treeaux;
	int		debuglevel;
	Spauth *auth;

	void		(*start)(Spsrv *);
	void		(*shutdown)(Spsrv *);
	void		(*destroy)(Spsrv *);
	void		(*connopen)(Spconn *);
	void		(*connclose)(Spconn *);
	void		(*fiddestroy)(Spfid *);

	Spfcall *(*version)(Spconn *conn, unsigned long msize, Spstr *version);
	Spfcall *(*attach)(Spfid *fid, Spfid *afid, Spstr *uname, Spstr *aname, unsigned long n_uname);
	Spfcall *(*flush)(Spreq *req);
	int		(*clone)(Spfid *fid, Spfid *newfid);
	int		(*walk)(Spfid *fid, Spstr *wname, Spqid *wqid);
	Spfcall *(*open)(Spfid *fid, unsigned char mode);
	Spfcall *(*create)(Spfid *fid, Spstr *name, unsigned long perm, unsigned char mode,
				Spstr *extension);
	Spfcall *(*read)(Spfid *fid, unsigned long long offset, unsigned long count, Spreq *req);
	Spfcall *(*write)(Spfid *fid, unsigned long long offset, unsigned long count, unsigned char *data, Spreq *req);
	Spfcall *(*clunk)(Spfid *fid);
	Spfcall *(*remove)(Spfid *fid);
	Spfcall *(*stat)(Spfid *fid);
	Spfcall *(*wstat)(Spfid *fid, Spstat *stat);

	/* implementation specific */
	Spconn *conns;
	Spreq *workreqs;
	int		enomem;		/* if set, returning Enomem Rerror */
	Spfcall *rcenomem;	/* preallocated to send if no memory */
	Spfcall *rcenomemu;	/* same for .u connections */
};

struct Spconn {
	Spsrv *srv;
	char *address;	/* IP address!port */
	uint32_t		msize;
	int		dotu;
	int		flags;
	Spreq *ireqs;          /* requests that didn't enter the srv queues yet */
	Spreq *oreqs;          /* requests that left the srv queues */
	void *caux;           /* implementation specific */
	Spfid **fidpool;
	int		freercnum;
	Spfcall *freerclist;
	void		(*reset)(Spconn *);
	int		(*shutdown)(Spconn *);
	void		(*dataout)(Spconn *, Spreq *req);

	Spconn *next;	/* list of connections within a server */
};

