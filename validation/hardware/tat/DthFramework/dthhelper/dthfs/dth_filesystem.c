/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   DTH file system
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "dth_filesystem.h"
#include "dth_porting.h"
#include "dth_utils.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

const struct dth_file_ops dth_default_directory_file_ops = {
	NULL,			/* open */
	NULL,			/* read */
	NULL,			/* write */
	NULL,			/* close */
	NULL			/* flush */
};

const struct dth_directory_ops dth_default_directory_ops = {
	NULL,			/* create */
	NULL			/* remove */
};

const struct dth_file_ops dth_default_file_ops = {
	NULL,			/* open */
	NULL,			/* read */
	NULL,			/* write */
	NULL,			/* close */
	NULL			/* flush */
};

static int get_child_by_index_helper(const struct dth_snode *node, void *arg)
{
	if (node == NULL) node=0;
	unsigned *count = (unsigned *)arg;
	return (*count)-- == 0;
}

static int get_child_by_name_helper(const struct dth_snode *node, void *arg)
{
	struct dth_file *file = dth_container_of(node, struct dth_file, node);
	const struct dth_str *name = arg;

	return file->nlen == name->len &&
	    dth_strncmp(file->name, name->buf, name->len) == 0;
}

static inline int dth_refers_current_directory(const char *name, unsigned size)
{
	return size == 1 && name[0] == '.';
}

static inline int dth_refers_parent_directory(const char *name, unsigned size)
{
	return size == 2 && name[0] == '.' && name[1] == '.';
}

static inline int dth_character_is_allowed(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
	    (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.';
}

static int dth_check_name(const char *name, unsigned size)
{
	if (dth_refers_parent_directory(name, size) ||
	    dth_refers_current_directory(name, size))
		return -1;

	while (size--)
		if (!dth_character_is_allowed(*name++))
			return -1;

	return 0;
}

/**
 * Add a child to a directory.
 * @param[in,out] directory specifies the directory
 * @param[in] file specifies the file
 * @retval  0 success,
 * @retval -1 file already not exist.
 */
static int dth_add_child(struct dth_directory *directory, struct dth_file *file)
{
	struct dth_queue *queue;
	struct dth_snode *prev, *node;
	struct dth_str str;

	if (dth_unlikely(dth_check_name(file->name, file->nlen)))
		goto illegal_name;

	queue = &directory->children;
	str.buf = file->name;
	str.len = file->nlen;
	prev = dth_queue_find_before(queue, &queue->head,
				     get_child_by_name_helper, &str);
	node = dth_queue_get_next(prev);
	if (dth_unlikely(node != &queue->tail))
		goto already_exist;

	dth_queue_add_after(prev, &file->node);
	file->parent = directory;

	return 0;

illegal_name:
	fprintf(stderr,
		"dthfs: fail to initialize file cause illegal name: %*s\n",
		file->nlen, file->name);
	return -1;

already_exist:
	fprintf(stderr,
		"dthfs: cannot add child cause file '%*s' already exists!\n",
		file->nlen, file->name);
	return -2;
}

/**
 * Remove a child from a directory.
 * @bug Do not use this function on children directories or dangling references
 *      will be created. It is tested yet whether underlying resources are
 *      still referenced by clients.
 * @param[in,out] directory specifies the directory.
 * @param[in] file specifies the file.
 * @retval  0 success,
 * @retval -1 file does not exist.
 */
static int dth_del_child(struct dth_directory *directory, struct dth_file *file)
{
	struct dth_queue *queue;
	struct dth_snode *prev, *node;
	struct dth_str str;

	queue = &directory->children;
	str.buf = file->name;
	str.len = file->nlen;
	prev = dth_queue_find_before(queue, &queue->head,
				     get_child_by_name_helper, &str);
	node = dth_queue_get_next(prev);
	if (dth_unlikely(node == &queue->tail))
		goto file_does_not_exist;

	dth_queue_del_after(prev);
	file->parent = NULL;

	return 0;

file_does_not_exist:
	return -1;
}

void dth_get_file_stat(const struct dth_file *file, struct stat *dir)
{
	dir->st_ino = file->path;
	dir->st_mode = file->perm;
	dir->st_atime = file->atime;
	dir->st_mtime = file->mtime;
	dir->st_size = file->length;
}

unsigned long dth_get_dir_size(const struct dth_dir *dir)
{
	return 49 + dir->name.len + dir->uid.len + dir->gid.len + dir->muid.len;
}

struct dth_file *dth_find_file_by_name(struct dth_directory *directory,
				       const char *name)
{
	struct dth_queue *queue;
	struct dth_snode *prev, *node;
	struct dth_file *file;
	struct dth_str str;

	queue = &directory->children;
	str.buf = (char *)name;
	str.len = strlen(name);

	if (strlen(directory->file.name) == 0)
		return &directory->file;

	if (strncmp(directory->file.name, name, str.len) == 0)
		return &directory->file;

	prev = dth_queue_find_before(queue, &queue->head,
				     get_child_by_name_helper, &str);

	node = dth_queue_get_next(prev);

	if (dth_unlikely(node == &queue->tail))
		goto not_found;

	file = dth_container_of(node, struct dth_file, node);

	return file;

not_found:
	return NULL;
}

struct dth_file *dth_get_child_by_name(struct dth_directory *directory,
				       const char *name, unsigned size)
{
	struct dth_queue *queue;
	struct dth_snode *prev, *node;
	struct dth_file *file;
	struct dth_str str;

	queue = &directory->children;
	str.buf = (char *)name;
	str.len = size;
	prev = dth_queue_find_before(queue, &queue->head,
				     get_child_by_name_helper, &str);
	node = dth_queue_get_next(prev);
	if (dth_unlikely(node == &queue->tail))
		goto not_found;

	file = dth_container_of(node, struct dth_file, node);

	return file;
not_found:
	return NULL;
}

struct dth_file *dth_get_child_by_index(const struct dth_directory *directory,
					unsigned index)
{
	const struct dth_queue *queue;
	struct dth_snode *prev, *node;
	unsigned temp;
	struct dth_file *file;

	queue = &directory->children;
	temp = index;
	prev = dth_queue_find_before(queue, &queue->head,
				     get_child_by_index_helper, &temp);
	node = dth_queue_get_next(prev);
	if (dth_unlikely(node == &queue->tail))
		goto not_found;

	file = dth_container_of(node, struct dth_file, node);

	return file;

not_found:
	return NULL;
}

int dth_initialize_file(struct dth_file *file, const char *name, unsigned size)
{
	dth_u32 time = dth_get_time();

	file->ops = &dth_default_file_ops;
	file->perm = 0444;
	file->version = 0;
	file->path = ((dth_u64) time << 32) | (dth_u32) file;
	file->atime = time;
	file->mtime = time;

	if (dth_unlikely(size >= sizeof(file->name)))
		goto illegal_name;

	file->nlen = size;
	strcpy(file->name, name);
	file->nfids = 0;
	file->nopens = 0;
	file->parent = NULL;
	file->service_data = NULL;

	return 0;

illegal_name:
	fprintf(stderr,
		"dthfs: fail to initialize file: illegal name: '%*s' (%u chars)\n",
		size, name, size);
	return -1;
}

int dth_initialize_directory(struct dth_directory *directory,
			     const char *name, unsigned size)
{
	int retval;

	retval = dth_initialize_file(&directory->file, name, size);
	if (dth_unlikely(retval != 0))
		goto bail_out;

	directory->file.ops = &dth_default_directory_file_ops;
	directory->file.perm = DTH_DMDIR | 0755;
	directory->ops = &dth_default_directory_ops;
	dth_queue_init(&directory->children);

bail_out:
	return retval;
}

#if defined(_DEBUG)

#define max(a, b) ((a) > (b) ? (a) : (b))

#define SWAP(T, a, b) \
    do { T save = (a); (a) = (b); (b) = save; } while (0)

/* strrev not defined in string.h in ubuntu so need a substitute */
char *reverse_string(char *s, unsigned long len)
{
	if (len > 1) {
		char *a = s;
		char *b = s + len - 1;
		for (; a < b; ++a, --b)
			SWAP(char, *a, *b);
	}

	return s;
}

char *dth_file_pathname(struct dth_file *file, char *path,
			unsigned long path_size)
{
	*path = 0;
	char name[128];
	unsigned long vl_max_len = 0;

	while (NULL != file) {
		strncpy(name, file->name, file->nlen);
		name[file->nlen] = 0;
		/* path content: "file" */

		reverse_string(name, file->nlen);
		/* path content: "elif" */

		vl_max_len = max(0, path_size - strlen(path));
		strncat(path, name, vl_max_len);	/* "elif/ridbus/rid/toor/" */

		struct dth_directory *pl_parent = file->parent;
		if ((pl_parent != NULL)
		    && (pl_parent->file.parent != pl_parent)) {
			vl_max_len = max(0, path_size - strlen(path));
			strncat(path, "/", vl_max_len);

			file = &file->parent->file;
		} else {
			file = NULL;
		}
	}

	reverse_string(path, dth_strlen(path));

	return path;
}

#endif /* _DEBUG */

typedef int (*LPENUMPATHPROC) (const char *name, unsigned long len, void *data);

/* return 0 if OK or a negative value if an error occurred. */
int dth_enum_path_elements(const char *name, LPENUMPATHPROC fn, void *data,
			   char **last)
{
	int res = 0;		/* on return: 0 if OK, < 0 if error */
	char *path = strdup(name);
	char *token = path;
	*last = (char *)name + 1;	/* after root char */

	token = strtok(path, "/");
	if (token) {
		int resfn;
		do {
			unsigned long token_len = dth_strlen(token);
			/* remember: path is temporary local variable */
			*last =
			    (char *)((unsigned long)name +
				     (unsigned long)token -
				     (unsigned long)path);

			resfn = fn(token, token_len, data);
			/* resfn: >0 = continue, 0 = stop, <0 = error */

			if (resfn < 0)
				res = resfn;
			else if (resfn > 0)
				*last += token_len;

			token = strtok(NULL, "/");
		} while ((NULL != token) && (resfn > 0));
	}

	dth_free(path);

	return res;
}

typedef struct {
	struct dth_directory *dir;
} walk_directory_data;

int walk_directory(const char *name, unsigned long len, void *data)
{
	int res = 1;

	walk_directory_data *wdata = (walk_directory_data *) data;
	struct dth_directory *dir = wdata->dir;

	if (dth_refers_parent_directory(name, len)) {
		/* leave if already at root directory */
		if (dir->file.parent == dir)
			goto no_parent;

		wdata->dir = dir->file.parent;
	} else if (!dth_refers_current_directory(name, len)) {
		struct dth_file *file;
		file = dth_get_child_by_name(dir, name, len);
		if (NULL != file) {
			if ((file->perm & DTH_DMDIR) == 0)
				goto not_dir;

			wdata->dir =
			    dth_container_of(file, struct dth_directory, file);
		} else {
			res = 0;	/* not found: stop iteration here */
		}
	}

	/* @postcondition */
	if (NULL == wdata->dir)
		goto no_parent;

	return res;

not_dir:
	fprintf(stderr, "%*s is not a directory!\n", (int)len, name);
	return -1;

no_parent:
	fprintf(stderr, "path '%*s' leads before root dir!\n", (int)len, name);
	return -2;
}

/* return 1 if created, -1 if fail to create or -2 if memory error  */
int create_directory(const char *name, unsigned long len, void *data)
{
	int res = 1;

	walk_directory_data *wdata = (walk_directory_data *) data;
	struct dth_directory *parent = wdata->dir;

	wdata->dir = dth_malloc(sizeof(struct dth_directory));
	if (wdata->dir != NULL) {
		if ((0 != dth_initialize_directory(wdata->dir, name, len))
		    || (0 != dth_add_child(parent, &(wdata->dir)->file))) {
			dth_free(wdata->dir);
			wdata->dir = NULL;
			res = -1;
		}
	} else {
		/* bad alloc */
		res = -2;
	}

	return res;
}

struct dth_directory *dth_create_directories(struct dth_directory *parent,
					     const char *name)
{

	walk_directory_data wdata;
	wdata.dir = parent;

	if (strlen(name) <= 0)
		return parent;

	char *last = NULL;	/* unused */
	int res = dth_enum_path_elements(name, create_directory, &wdata, &last);
	if (res >= 0)
		parent = wdata.dir;
	else
		parent = NULL;

	return parent;
}

struct dth_file *dth_create_file(struct dth_directory *parent,
				 const char *filename, dth_u32 perm,
				 const struct dth_file_ops *ops)
{

	struct dth_file *file;
	int retval;

	file = dth_malloc(sizeof(struct dth_file));
	if (dth_unlikely(file == NULL))
		goto bad_file_alloc;

	retval = dth_initialize_file(file, filename, dth_strlen(filename));
	if (retval == 0) {
		file->perm = perm & ~DTH_DMDIR;
		if (ops != NULL)
			file->ops = ops;

		retval = dth_add_child(parent, file);
	}

	if (0 != retval) {
		fprintf(stderr, "dthfs: fail to create file %s from %*s\n",
			filename, parent->file.nlen, parent->file.name);
		dth_free(file);
		file = NULL;
	}

	return file;

bad_file_alloc:
	fprintf(stderr, "dthfs: fail to allocate dth_file for '%s'\n",
		filename);
	return NULL;
}

struct dth_directory *dth_walk_directories(struct dth_directory *parent,
					   const char *name, char **last)
{
	walk_directory_data wdata;
	wdata.dir = parent;

	int res = dth_enum_path_elements(name, walk_directory, &wdata, last);
	if (res >= 0)
		parent = wdata.dir;
	else			/* negative means error */
		parent = NULL;

	return parent;
}

int dth_str_cmp(const char *s, unsigned long l, const char *a)
{
	unsigned int al = strlen(a);
	if (al > l)
		return -1;
	if (al < l)
		return 1;

	return strncmp(s, a, l);
}

int dth_str_tosz(const char *s, unsigned long ls, char *a, unsigned int size)
{
	if ((NULL == s) || (NULL == a))
		return -1;

	/* truncate if buffer is not large enough */
	if (ls > (size - 1))
		ls = size - 1;
	strncpy(a, s, ls);
	a[ls] = '\0';

	return ls;
}

int dth_dump_tree_elt(struct dth_file *file, int *depth, void *data)
{
	char str[255];
	dth_dump_tree_elt_args *args = (dth_dump_tree_elt_args *) data;

	if (NULL == args->output)
		return -1;

	int i;
	for (i = 0; i < *depth; i++)
		fputc('\t', args->output);

	dth_str_tosz(file->name, file->nlen, str, sizeof(str));
	fprintf(args->output, "%s", str);

	if (file->perm & DTH_DMDIR)
		fputc('/', args->output);

	fputc('\n', args->output);

	return 1;
}

int dth_enum_tree(struct dth_file *start, dth_enum_tree_proc proc, int *depth,
		  void *data)
{
	if (NULL == start)
		return -1;

	int root_depth = 0;
	if (NULL == depth)
		depth = &root_depth;

	struct dth_file *file = start;
	int res = proc(file, depth, data);
	if (res > 0) {
		/*fprintf(stdout, "proc returned %d\n", res); */
		if (file->perm & DTH_DMDIR) {
			/*fprintf(stdout, "is directory\n"); */
			(*depth)++;
			int this_depth = *depth;

			/* directory: enum children */
			struct dth_directory *dir =
			    dth_container_of(file, struct dth_directory, file);
			struct dth_queue *queue = &dir->children;
			struct dth_snode *node = &queue->head;
			node = dth_queue_get_next(node);
			do {
				if (node != &queue->tail) {
					file =
					    dth_container_of(node,
							     struct dth_file,
							     node);
					/*fprintf(stdout, "file = %p\n", file); */
					*depth = this_depth;
					res =
					    dth_enum_tree(file, proc, depth,
							  data);
					/*fprintf(stdout, "dth_enum_tree returned %d\n", res); */
				}
				node = dth_queue_get_next(node);
			} while ((node != &queue->tail) && (res > 0));
		}
	}

	return res;
}

void dth_delete_branch(struct dth_file *from)
{
	struct dth_queue *vp_queue;
	struct dth_file *vp_file;
	struct dth_directory *vp_dir;
	struct dth_snode *vp_node;

	if ((from->perm & DTH_DMDIR) != 0) {
		vp_dir = dth_container_of(from, struct dth_directory, file);
		vp_queue = &vp_dir->children;
		if (!dth_queue_is_empty(vp_queue)) {
			vp_node = dth_queue_get_next(&vp_queue->head);
			do {
				vp_file =
				    dth_container_of(vp_node, struct dth_file,
						     node);
				dth_delete_branch(vp_file);

				if (vp_node != &vp_queue->tail) {
					vp_node =
					    dth_queue_get_next(vp_node);
				}
			} while (vp_node != &vp_queue->tail);

			/* do not free the node itself because if is a part of dth_file */
		}
	}

	if (&from->parent->file != from)
		dth_del_child(from->parent, from);

	if (from->perm & DTH_DMDIR) {
		vp_dir = dth_container_of(from, struct dth_directory, file);
		free(vp_dir);
	} /* Do not free from it is not a pointer to a dynamic memory allocated
		 else {
		free(from);
	}*/
}

int dth_delete_file(struct dth_directory *parent, const char *filename)
{
	int vl_ret = 0;		/* 0: OK, 1: file not found, -1: error */
	char *pl_lastdir;

	/* @paramcond parent must not be null */
	if (NULL == parent) {
		errno = EINVAL;
		return -1;
	}

	/* @paramcond filename must not be null or an empty string */
	if ((NULL == filename) || (strlen(filename) <= 0)) {
		errno = EINVAL;
		return -1;
	}

	struct dth_file *vp_file = NULL;
	vl_ret = dth_find_file(parent, filename, &vp_file);

	if (0 == vl_ret) {
		/* see if the file is in fact a directory */
		if ((vp_file->perm & DTH_DMDIR) != 0) {
			struct dth_directory *vp_file_as_dir =
			    dth_walk_directories(parent,
						 vp_file->name, &pl_lastdir);
			if (!dth_queue_is_empty(&vp_file_as_dir->children)) {
				/* @case if the file is a non-empty directory then the function
				 * fails to avoid having dandling pointers after the call. */
				errno = ENOTEMPTY;
				vl_ret = -1;
			}
		}

		if (0 == vl_ret) {
			/* ok to remove the file whatever is type */
			vl_ret = dth_del_child(parent, vp_file);
		}
	}
	/* else error cases are managed in dth_find_file, which had set errno */

	return vl_ret;
}

int dth_find_file(struct dth_directory *dir, const char *pathname,
		  struct dth_file **file)
{
	int vl_ret = 0, vl_errno = 0;

	/* @paramcond dir cannot be null */
	if (NULL == dir) {
		errno = EINVAL;
		return -1;
	}

	/* @paramcond pathname cannot be an absolute path because the root
	 * directory is unknown by this package */
	if ((NULL == pathname) || (*pathname == '/') || (*pathname == 0)) {
		errno = EINVAL;
		return -1;
	}

	/* parse path elements one by one.
	 * strotk modifies the string passed
	 * to the function so use a copy of it. */
	char *vl_dup_pathname = strdup(pathname);
	char *vl_s, *vl_snext;
	struct dth_directory *vp_curdir = dir;

	vl_s = strtok(vl_dup_pathname, "/");
	while (!vl_ret && vl_s) {
		int vl_slen = strlen(vl_s);
		vl_snext = strtok(NULL, "/");

		/* @case parent directory */
		if (dth_refers_parent_directory(vl_s, vl_slen)) {
			vp_curdir = vp_curdir->file.parent;
			if (vp_curdir == NULL) {
				/* @error moved back before the root:
				 * pathname is not valid. */
				vl_errno = EBADF;
				vl_ret = -1;
			}
		}
		/* @case not a relative token (neither .. nor . ) */
		else if (!dth_refers_current_directory(vl_s, vl_slen)) {
			struct dth_file *vp_file;
			vp_file =
			    dth_get_child_by_name(vp_curdir, vl_s, vl_slen);
			*file = vp_file;
			/* @error pathname not pointing to an existing file */
			if (vp_file == NULL) {
				vl_errno = ENOENT;
				vl_ret = -1;
			}
			/* @integrity file shoud have parent for parent */
			else if (vp_file->parent != vp_curdir) {
				errno = ESTALE;
				vl_ret = -1;
			}
			if (!vl_ret) {
				/* if this not the last token in pathname then the
				 * file found should be a directory else the pathname
				 * is not valid */
				if (NULL != vl_snext) {
					if ((vp_file->perm & DTH_DMDIR) != 0) {
						vp_curdir =
						    dth_container_of(vp_file,
								     struct
								     dth_directory,
								     file);
					} else {
						vl_errno = EBADF;
						vl_ret = -1;
					}
				}
				/* element is last in pathname. */
			}
		}
		/* @case current directory: nothing to be done */
		vl_s = vl_snext;
	}

	free(vl_dup_pathname);

	errno = vl_errno;
	return vl_ret;
}
