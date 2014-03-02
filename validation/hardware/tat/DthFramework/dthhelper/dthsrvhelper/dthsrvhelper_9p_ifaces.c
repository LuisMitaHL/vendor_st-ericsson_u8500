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

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include "dthsrvhelper.h"
#include "dthsrvhelper_9p_ifaces.h"
#include <dthsrvhelper/dthsrvhelper.h>
#include <dthfs/dth_filesystem.h>
#include <sys/time.h>

char *Edirchange = "cannot convert between files and directories";
char *Eunknownuser = "unknown user";

int init(void)
{
	int err;

	err = dth_helper_init();

	return err;
}

void uninit(void)
{
	dth_helper_uninit();
}

static int wstat_size(Spwstat *wstat, int dotu)
{
	int size = 0;

	if (wstat == NULL)
		return 0;

	size = 2 + 4 + 13 + 4 +	/* type[2] dev[4] qid[13] mode[4] */
	    4 + 4 + 8 +		/* atime[4] mtime[4] length[8] */
	    8;			/* name[s] uid[s] gid[s] muid[s] */

	if (wstat->name)
		size += strlen(wstat->name);
	if (wstat->uid)
		size += strlen(wstat->uid);
	if (wstat->gid)
		size += strlen(wstat->gid);
	if (wstat->muid)
		size += strlen(wstat->muid);

	if (dotu) {
		/* n_uid[4] n_gid[4] n_muid[4] extension[s] */
		size += 4 + 4 + 4 + 2;
		if (wstat->extension)
			size += strlen(wstat->extension);
	}

	return size;
}

static void ustat2qid(struct stat *st, Spqid * qid)
{
	qid->path = st->st_ino;
	qid->version = 0;
	qid->type = st->st_mode >> 24;
}

char *spstr_dup(Spstr * str)
{
	char *ret;

	ret = malloc(str->len + 1);
	if (!ret)
		return NULL;

	memmove(ret, str->str, str->len);
	ret[str->len] = '\0';

	return ret;
}

/**
* Check permissions against world access, group access and owner access.
* @param file: file against which to check permission
* @param bit: 0 for execution, 1 for write, 2 for read
* @todo: only stands for unauthenticated access as the user is not verified to
* be an actual member of the group or owner of the file.
*/
static int dth_check_file_permission(const struct dth_file *file, int bit)
{
	dth_u32 perm = file->perm >> bit;

	return (perm & (1 << 0)) || (perm & (1 << 3)) || (perm & (1 << 6));
}

void ustat2npwstat(char *path, struct stat *st, Spwstat * wstat, int dotu)
{
	char ext[256];
	ext[0] = '\0';

	memset(wstat, 0, sizeof(*wstat));
	ustat2qid(st, &wstat->qid);
	wstat->mode = st->st_mode;
	wstat->atime = st->st_atime;
	wstat->mtime = st->st_mtime;
	wstat->length = st->st_size;

	wstat->uid = "nobody";
	wstat->gid = "nobody";
	wstat->muid = "nobody";
	if (dotu)
		wstat->extension = strdup(ext);

	wstat->name = strdup(path);
}

int clone(Spfid *fid, Spfid *newfid)
{
	Fid *f, *nf;

	f = fid->aux;

	nf = malloc(sizeof(*f));
	nf->path = NULL;
	nf->omode = -1;
	nf->fd = -1;
	nf->dir = NULL;
	nf->diroffset = 0;
	nf->direntname = f->direntname;

	nf->path = strdup(f->path);
	newfid->aux = nf;

	return 1;
}

void attach_core(Spfid *nfid, Spqid *qid)
{
	Fid *fid;
	struct stat st;

	fid = malloc(sizeof(*fid));
	fid->omode = -1;

	fid->path = strdup("");
	rootDir->file.nfids += 1;

	nfid->aux = fid;

	fid->direntname = (void *)rootDir;

	dth_get_file_stat(&rootDir->file, &st);
	nfid->type = st.st_mode >> 24;
	ustat2qid(&st, qid);

	return;
}

int walk_core(Spfid *fid, Spstr *wname, Spqid *wqid)
{

	Fid *f;
	struct dth_directory *directory;
	struct dth_file *file;
	struct stat st;
	struct timeval t;

	gettimeofday(&t, NULL);

	f = fid->aux;

	file = dth_find_file_by_name((void *)f->direntname, f->path);
	if (file == NULL)
		return EACCES;

	if (!(file->perm & DTH_DMDIR)) {
		dth_reset_message_pump();
		dth_push_message(LOG_ERR, "walk failed because not dir");
		return ENOTDIR;
	}

	if ((wname->len != 2 ||
	     strncmp("..", wname->str, 2) != 0) &&
	    !dth_check_file_permission(file, 0)) {
		return EACCES;
	}

	file->atime = t.tv_sec;
	directory = dth_container_of(file, struct dth_directory, file);
	file = dth_get_child_by_name(directory, wname->str, wname->len);
	if (file == NULL) {
		dth_reset_message_pump();
		dth_push_message(LOG_ERR,
				 "walk failed %s not found in file tree",
				 wname->str);
		return ENOENT;
	}

	free(f->path);
	f->path = malloc(wname->len + 1);
	snprintf(f->path, wname->len + 1, "%s", wname->str);
	f->direntname =
	    (void *)dth_container_of(file, struct dth_directory, file);
	file->nfids += 1;
	dth_get_file_stat(file, &st);

	ustat2qid(&st, wqid);

	return 0;
}

int open_core(Spfid *fid, unsigned char mode, Spqid *qid)
{
	Fid *f;
	struct dth_file *file;
	struct stat st;
	struct timeval t;

	gettimeofday(&t, NULL);

	f = fid->aux;

	file = dth_find_file_by_name((void *)f->direntname, f->path);
	if (file == NULL)
		return EACCES;

	dth_get_file_stat(file, &st);

	if (file->perm & DTH_DMEXCL && file->nopens)
		goto bad_excl;

	switch (mode & 3) {
	case DTH_OEXEC:
		if (!dth_check_file_permission(file, 0)) {
			dth_reset_message_pump();
			dth_push_message(LOG_ERR,
					 "no +x privilege to execute %s",
					 file->name);
			goto bad_perm;
		}
		break;

	case DTH_OWRITE:

		if (!dth_check_file_permission(file, 1)) {
			dth_reset_message_pump();
			dth_push_message(LOG_ERR, "no +w privilege to write %s",
					 file->name);
			goto bad_perm;
		}
		break;

	case DTH_OREAD:

		if (!dth_check_file_permission(file, 2)) {
			dth_reset_message_pump();
			dth_push_message(LOG_ERR, "no +r privilege to read %s",
					 file->name);
			goto bad_perm;
		}
		break;

	case DTH_ORDWR:
		if (!dth_check_file_permission(file, 1)
		    || !dth_check_file_permission(file, 2)) {
			dth_reset_message_pump();
			dth_push_message(LOG_ERR,
					 "no +r or -w privilege to access %s",
					 file->name);
			goto bad_perm;
		}
		break;
	}

	if (mode & DTH_ORCLOSE) {
		if (file->perm & DTH_DMDIR)
			goto bad_type;

		if (!dth_check_file_permission(&file->parent->file, 1))
			goto bad_perm;
	}

	if (mode & DTH_OTRUNC) {
		if (file->perm & DTH_DMDIR)
			goto bad_type;
		if (!dth_check_file_permission(file, 1))
			goto bad_perm;

		file->atime = t.tv_sec;
	}

	dth_get_file_stat(file, &st);
	f->omode = mode;
	ustat2qid(&st, qid);
	return 0;

bad_excl:
	dth_reset_message_pump();
	dth_push_message(LOG_ERR, "file is locked");
	return EAGAIN;

bad_perm:
	return EACCES;

bad_type:
	return EISDIR;
}

static long
dth_read_dir(Fid *f, const struct dth_file *file_ref,
	     unsigned long long offset, unsigned long count,
	     int dotu, Spwstat **stat_buf, int *statBuf_nbElement)
{
	unsigned int i, n = 0;
	Spwstat wstat;
	struct dth_file *file = NULL;
	struct dth_directory *directory = NULL;
	struct stat st;
	unsigned int vl_nb_element = 0;
	unsigned int vl_statBuf_size = 50;	/* 50 wstat */

	if (offset == 0) {
		f->diroffset = 0;
		f->fd = 0;
	}

	directory = dth_container_of(file_ref, struct dth_directory, file);

	*stat_buf = malloc(vl_statBuf_size * sizeof(Spwstat));
	while (n < (count - vl_nb_element * 2)) {
		file = dth_get_child_by_index(directory, f->fd);
		if (file != NULL) {
			dth_get_file_stat(file, &st);
			ustat2npwstat(file->name, &st, &wstat, dotu);
			/* size without size[2] of Spwstat */
			i = wstat_size(&wstat, dotu);

			if (i == 0)
				break;

			if ((n + i) < (count - (vl_nb_element + 1) * 2)) {
				n += i;
				f->fd += 1;
			} else {
				break;
			}

			/* Check buffer size */
			if (vl_nb_element == vl_statBuf_size) {
				vl_statBuf_size += 10;	/*Increase 10 by 10 */
				*stat_buf = realloc(*stat_buf,
						    vl_statBuf_size *
						    sizeof(Spwstat));

				if (*stat_buf == NULL) {
					n = -ENOMEM;
					goto end;
				}
			}
			*((*stat_buf) + vl_nb_element) = wstat;

			vl_nb_element += 1;
		} else {
			break;
		}
	}
	f->diroffset += n;
	*statBuf_nbElement = vl_nb_element;
end:
	return n;
}

int stat_core(Spwstat *wstat, Spfid *fid)
{
	int err = 0;
	Fid *f;
	struct stat st;
	struct dth_file *file;

	f = fid->aux;

	file = dth_find_file_by_name((void *)f->direntname, f->path);
	if (file == NULL)
		return EACCES;

	dth_get_file_stat(file, &st);
	ustat2npwstat(file->name, &st, wstat, fid->conn->dotu);

	return err;
}

int read_core(Spfid *fid, unsigned long long offset, unsigned long count,
	      Spfcall *ret, Spwstat **stat_buf, int *statBuf_nbElement)
{
	Fid *f;
	struct dth_file *file;
	int err = 0;
	long n;

	f = fid->aux;

	file = dth_find_file_by_name((void *)f->direntname, f->path);
	if (file == NULL) {
		errno = EACCES;
		goto callback_error;
	}

	if (file->perm & DTH_DMDIR) {
		n = dth_read_dir(f, file, offset, count,
				 fid->conn->dotu, stat_buf, statBuf_nbElement);
		if (n < 0) {
			errno = -n;
		}
	} else {
		if (file->ops->read == NULL) {
			goto no_data;
		} else {
			if (!dth_check_file_permission(file, 2)) {
				errno = EACCES;
				goto callback_error;
			}

			unsigned char *buffer =
			    calloc(count, sizeof(unsigned char));
			n = file->ops->read(file, buffer, offset, count,
					    &err);
			if (n > 0) {
				if ((unsigned long)n > count)
					n = count;

				long i;
				for (i = 0; i < n; i++)
					*(ret->data + i) = *(buffer + i);
				*statBuf_nbElement = n;
			} else if (n < 0) {
				errno = err;
				goto callback_error;
			}

			free(buffer);
		}
	}

	return err;

no_data:
	return err;

callback_error:
	return -1;

}

int write_core(Spfid *fid, unsigned long long offset, unsigned long count,
	       unsigned char *data)
{
	int n = -1;
	Fid *f;
	struct dth_file *file;
	int error = 0;

	f = fid->aux;

	file = dth_find_file_by_name((void *)f->direntname, f->path);
	if (file == NULL) {
		errno = EACCES;
		goto end;
	}
	if (file->ops->write == NULL)
		goto bad_perm;

	n = file->ops->write(file, data, offset, count, &error);
	if (n < 0) {
		errno = error;
		goto callback_error;
	}
end:
	return n;

callback_error:
	dth_reset_message_pump();
	dth_push_message(LOG_ERR, "write %s exited with code %d", f->path, n);
	return n;

bad_perm:
	dth_reset_message_pump();
	dth_push_message(LOG_ERR, "no write privilege for %s", f->path);
	return n;
}

void wstat_core(Spfid *fid, Spstat *stat)
{
	Fid *f;
	Spfcall *ret;
	struct dth_file *file;

	ret = NULL;
	f = fid->aux;

	file = dth_find_file_by_name((void *)f->direntname, f->path);
	if (file == NULL)
		goto out;

	f->stat.st_size = stat->length;

	if (stat->mode != ~0UL) {
		if (stat->mode & Dmdir && !S_ISDIR(f->stat.st_mode))
			goto out;

		f->stat.st_mode = stat->mode & 0777;
		file->perm = stat->mode & 0777;
		printf("change mode %d\n", stat->mode);
	}

	if (stat->mtime != ~0UL) {
		f->stat.st_atime = stat->atime;
		f->stat.st_mtime = stat->mtime;
		file->atime =  stat->atime;
		file->mtime =  stat->mtime;
	}

out:
	return;
}
