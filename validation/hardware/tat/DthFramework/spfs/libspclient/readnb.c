/*
 * Copyright (C) 2006 by Latchesar Ionkov <lucho@ionkov.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * LATCHESAR IONKOV AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "spfs.h"
#include "spclient.h"
#include "spcimpl.h"

typedef struct Spcread Spcread;
struct Spcread
{
	Spcfid*		fid;
	u64		offset;
	u32		count;
	u8*		buf;
	void		(*cb)(void *, int);
	void*		cba;
	Spfcall*	tc;
};

static void spc_read_cb(void *cba, Spfcall *rc);

static int
spc_send_read_request(Spcread *r)
{
	int n;
	Spcfid *fid;
	Spcfsys *fs;
	Spfcall *fc;

	fid = r->fid;
	fs = fid->fsys;

	n = fid->iounit;
	if (n == 0)
		n = fs->msize - IOHDRSZ;

	if (n > (r->count - r->offset))
		n = r->count - r->offset;

	r->tc = sp_create_tread(fid->fid, r->offset, n);
	if (spc_rpcnb(fs, r->tc, spc_read_cb, r) < 0) {
		free(fc);
		return -1;
	}

	return 0;
}

static void
spc_read_cb(void *cba, Spfcall *rc)
{
	int ecode;
	char *ename;
	Spcread *r;

	r = cba;
	free(r->tc);
	sp_rerror(&ename, &ecode);
	if (ename) {
		(*r->cb)(r->cba, -1);
		return;
	}

	memmove(r->buf + r->offset, rc->data, rc->count);
	r->offset += rc->count;
	if (!rc->count || r->offset==r->count) {
		(*r->cb)(r->cba, r->offset);
		free(rc);
		return;
	}

	free(rc);
	if (spc_send_read_request(r) < 0) 
		(*r->cb)(r->cba, -1);
}

int 
spc_readnb(Spcfid *fid, u8 *buf, u32 count, u64 offset, 
	void (*cb)(void *, int), void *cba)
{
	Spcread *r;

	r = sp_malloc(sizeof(*r));
	if (!r)
		return -1;

	r->fid = fid;
	r->offset = 0;
	r->count = count;
	r->buf = buf;
	r->cb = cb;
	r->cba = cba;
	r->tc = NULL;

	spc_send_read_request(r);
	return 0;
}
