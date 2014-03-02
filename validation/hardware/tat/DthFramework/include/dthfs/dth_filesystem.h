/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

/**
 * @file dth_filesystem.h
 *
 * @brief Interface for handling files and directories within the server.
 */

#ifndef DTH_FILESYSTEM_H_
#define DTH_FILESYSTEM_H_

#include <stdio.h>

#include <dthfs/dth_types.h>
#include <dthfs/dth_queue.h>
#include <sys/stat.h>
/**
 * 9P access permissions bits.
 */
enum dth_permission_bits {
	/** file is an authentication file. */
	DTH_DMAUTH   = (1ul << 27),
	/** file is exclusive-use: only one client may open it at a time. */
	DTH_DMEXCL   = (1ul << 29),
	/** file is append-only: offset is ignored in writes. */
	DTH_DMAPPEND = (1ul << 30),
	/** file is a directory. */
	DTH_DMDIR    = (1ul << 31)
};

/**
 * 9P open modes.
 */
enum dth_open_modes {
	DTH_OREAD   = 0,	/**< open file for read access. */
	DTH_OWRITE  = 1,	/**< open file for write access. */
	DTH_ORDWR   = 2,	/**< open file for read and write access. */
	DTH_OEXEC   = 3,	/**< open file for execution. */
	DTH_OTRUNC  = 16,	/**< file contents will be erased at open. */
	DTH_ORCLOSE = 64	/**< file will be removed when it is clunked. */
};

/**
 * Definition of a 9P character string.
 *
 * This macro only takes references. It does not copy anything.
 *
 * @param[in,out] n specifies the name of the variable.
 * @param[in] s specifies a constant and persistent NUL-terminated string.
 */
#define DEFINE_DTH_STR(n, s) struct dth_str (n) = {			      \
	sizeof(s) - sizeof((s)[0]),					      \
	(s)								      \
}

/**
 * 9P QID.
 *
 * A QID is a structure containing type, path, and version fields.
 * The path is guaranteed to be unique among all names currently in the file
 * server. Version is changed each time a file is modified.
 */
struct dth_qid {
	dth_u8 type; /**< The type of the file, represented as a bit vector
				   corresponding to the high 8 bits of the file's
				   mode word (see dth_qid_types). */
	dth_u32 version; /**< Version number for a given path. */
	dth_s64 path; /**< The server's unique identification for the file. */
};

/**
 * 9P character string.
 *
 * The string is UTF-8 formated. It is not NUL terminated. NUL is an illegal
 * character.
 */
struct dth_str {
	dth_u16 len;	/**< length of the string. */
	char *buf;	/**< pointer to the array of characters. */
};

/* compare UTF8 string \as of length \al to null terminated string \aa.
 * @return same as strcmp()
 */
int dth_str_cmp(const char *s, unsigned long l, const char *a);

/**
 * Copy an UTF-8 string to a null terminated string
 * @param s UTF-8 string
 * @param ls number of characters in \as
 * @param a a buffer to be filled with the null terminated string
 * @param size size of the buffer \aa
 * @return the number of chars copied in \aa excluding the null terminating
 * char or -1 if one of the argument is not valid.
 */
int dth_str_tosz(const char *s, unsigned long ls, char *a, unsigned int size);

/**
 * Information carried by stat, wstat and on directories read transactions.
 *
 * Here is a description of the encoded format of such information:
 *
 * size[2] type[2] dev[4] qid[13] mode[4] atime[4] mtime[4] length[8]
 * name[s] uid[s] gid[s] muid[s]
 *
 * The mode contains permission bits defined in @ref dth_permission_bits.
 * They are echoed in qid.type as in @ref dth_qid_type_bits.
 *
 * The two time fields are measured in seconds since the epoch
 * (Jan 1 00:00 1970 GMT). For a plain file, mtime is the time of the most
 * recent create, open with truncation or write. For a directory, it is the
 * time the the most recent  remove, create or wstat of a file in the
 * directory.
 *
 * Similarly, the atime field records the last read of the contents; also, it
 * is set whenever mtime is set. In addition, for a directory, it is set by an
 * attach, walk or create, all whether successful or not.
 *
 * The stat request requires no special permissions.
 *
 * The wstat request can change some of the file status information. The name
 * can be altered by anyone having write access to the parent directory,
 * provided no file already exists with that name. The length may be changed
 * by anyone having write permissions on the file. It is an error to attempt
 * to set the length of a directory to a non zero value and servers can decide
 * to reject such operation for other reasons. The mode and mtime can be
 * changed by users having ownership of the file. The directory bit is the
 * only bit that cannot be changed. The gid can also be altered once ownership
 * is verified. Other data cannot be changed. A wstat request is atomic:
 * either is succeeds of it completely fails. Explicit "don't touch" values
 * can be specified by the requester (0 length strings, and maximum unsigned
 * integer values).
 */
struct dth_dir {
	dth_u16 size;        /**< length of following data. */
	dth_u16 type;        /**< reserved. */
	dth_u32 dev;         /**< reserved. */
	struct dth_qid qid;  /**< qid of the file. */
	dth_u32 mode;        /**< permissions and flags. */
	dth_u32 atime;       /**< last access time. */
	dth_u32 mtime;       /**< last modification time. */
	dth_u64 length;      /**< length of the file in bytes. */
	struct dth_str name; /**< name of the file. */
	struct dth_str uid;  /**< name of the owner of the file. */
	struct dth_str gid;  /**< name of the group of the file. */
	struct dth_str muid; /**< name of user who last modified the file. */
};

/**
 * Server record for files to export.
 */
struct dth_file {
	const struct dth_file_ops *ops; /**< file operations. */
	dth_u32 perm;			/**< file permissions. */
	dth_u32 version;		/**< file version. */
	dth_u64 path;			/**< file path. */
	dth_u32 atime;			/**< last access time. */
	dth_u32 mtime;			/**< last modification time. */
	dth_u64 length;			/**< length of file in bytes. */

	char name[128];			/**< name of the file. */
	dth_u16 nlen;			/**< length of the name of the file */

	void *service_data;		/**< left for service implementation purpose */

	struct dth_directory *parent;	/**< parent directory */
	unsigned nopens;		/**< how many open */
	unsigned nfids;			/**< how many dth_session_file references */
	struct dth_snode node;		/**< @internal */
};


/**
 * Virtual interface for specifying file related operations.
 */
struct dth_file_ops {
	/**
	 * This function is called by the server when the related file is
	 * opened by a client.
	 *
	 * It can be set to NULL if the event is to be ignored. Otherwise, it
	 * is the responsibility of its implementation to build and send the
	 * response so as to complete the transaction.
	 *
	 * @param[in]	file	specifies the file to open.
	 */
	void (*open)(struct dth_file *file);

	/**
	 * This function is called by the server when the related file is
	 * read by a client.
	 *
	 * It can be set to NULL. In that case, read requests will be
	 * automatically refused by the server. Otherwise, it is the
	 * responsibility of its implementation to build and send the response
	 * so as to complete the transaction.
	 *
	 * @param[in]	file	specifies the file where data will be write.
	 * @param[in]	data	specifies the buffer where data read
	 *						will be store.
	 * @param[in]	offset	specifies the position in the file where
	 *						data reading starts.
	 * @param[in]	count	specifies number of bytes to read.
	 * @param[in]	error	specifies the error code if an error
	 *						appears during writing.
	 *
	 * @retval nbbytes read.
	 * @retval -1 if an error occured while processing.
	 */
	int (*read)(const struct dth_file *file , dth_u8 *data, const dth_u64 offset,
			const dth_u32 count, int *error);

	/**
	 * This function is called by the server when the related file is
	 * written by a client.
	 *
	 * It can be set to NULL. In that case, write requests will be
	 * automatically refused by the server. Otherwise, it is the
	 * responsibility of its implementation to build and send the response
	 * so as to complete the transaction.
	 *
	 * @param[in]	file	specifies the file where data will be write.
	 * @param[in]	data	specifies data to write.
	 * @param[in]	offset	specifies the position in the file where
	 *						data writing starts.
	 * @param[in]	count	specifies number of bytes to write.
	 * @param[in]	error	specifies the error code if an error appears
	 *						during writing.
	 *
	 * @retval nbbytes written.
	 * @retval -1 if an error occured while processing.
	 */
	int (*write)(struct dth_file *file , const dth_u8 *data, const dth_u64 offset,
			const dth_u32 count, int *error);

	/**
	 * This function is called by the server when the related file is
	 * clunked by a client.
	 *
	 * It can be set to NULL if the event is to be ignored. Otherwise, it
	 * is the responsibility of its implementation to build and send the
	 * response so as to complete the transaction.
	 *
	 * Any pending transaction related to the session_file referenced by
	 * the transaction should be replied with dth_bad_fid error before this
	 * clunk transaction is actually replied. Thus, if the service stores
	 * I/O transactions, it should implement this method.
	 *
	 * Note that the server won't call clunk if the related file was not
	 * previously open (it was just walked in that case). thus, there
	 * shall be as many call to clunk as to open.
	 *
	 */
	void (*clunk)();

	/**
	 * This function is called by the server when another on-going
	 * transaction on the related file is to be canceled.
	 *
	 */
	void (*flush)();
};

/**
 * Server record for directories to export.
 */
struct dth_directory {
	const struct dth_directory_ops *ops;	/**< directory operations. */
	struct dth_file file;			/**< file record of the directory. */
	struct dth_queue children;	/**< children files of the directory. */
};

/**
 * Virtual interface for specifying directory related operations.
 */
struct dth_directory_ops {
	/**
	 * This function is called by the server when the client attempts to
	 * create a file in the related directory.
	 *
	 * It can be set to NULL. In that case, create requests will be
	 * automatically refused by the server. Otherwise, it is the
	 * responsibility of its implementation to build and send the response
	 * so as to complete the transaction.
	 *
	 */
	void (*create)();

	/**
	 * This function is called by the server when the client attempts to
	 * remove a file from the related directory.
	 *
	 * It can be set to NULL. In that case, remove requests will be
	 * automatically refused by the server. Otherwise, it is the
	 * responsibility of its implementation to build and send the response
	 * so as to complete the transaction.
	 *
	 */
	void (*remove)();
};

/**
 * Default directory implementation for directory operations.
 */
extern const struct dth_directory_ops dth_default_directory_ops;

/**
 * Default directory implementation for file operations.
 */
extern const struct dth_file_ops dth_default_directory_file_ops;

/**
 * Default file implementation for file operations.
 */
extern const struct dth_file_ops dth_default_file_ops;

/**
 * Travel directories one by one, stating from a specified directory.
 *
 * Directories are traveled regardless of permissions.
 *
 * @param[in] parent specifies the directory which the first created directory
 *            will be a child of.
 * @param[in] name  is an UTF-8 NUL-terminated character string that specifies
 *            the names of directories to travel, separated by the slash ('/')
 *            character.
 *
 * @return the directory that was last created or parent if no directory could
 *         be traveled at all.
 */
struct dth_directory *dth_walk_directories(struct dth_directory *parent,
		const char *name, char **last);

/**
 * Create directories one by one, stating from a specified directory.
 *
 * The function fails when running out of memory or when a directory cannot be
 * created, either because its name contains illegal characters or because a
 * file with such a name that is not a directory already exists.
 * See @ref dth_initialize_file for a description of what are legal and
 * illegal characters.
 *
 * Directories are created with default values regardless of permissions.
 *
 * @param[in] parent specifies the directory, which the first created
 *			directory will be a child of.
 * @param[in] name is an UTF-8 character string that specifies the name of the
 *          device, that will also be the name of the directory that will be
 *          created.
 *
 * @return the directory that was last created or parent if no directory could
 *         be created at all.
 */
struct dth_directory *dth_create_directories(struct dth_directory *parent,
		const char *name);

/**
 * Create a new file in the specified parent directory.
 *
 * The function will fail if the name of the file is illegal or if it
 * already exists in the specified parent directory.
 * See @ref dth_initialize_file for a description of what are legal
 * and illegal names.
 *
 * The file is created with default values regardless of permissions.
 *
 * @param[in] parent specifies the parent directory where to create the
 *          new file.
 * @param[in] name is an UTF-8 character string that specifies the name of the
 *          file to be created.
 * @param[in] perm specifies the default permissions to attributes to
 *			the files in the created directory.
 *
 * @return the file that was created or NULL if the file could not be created.
 *
 */
struct dth_file *dth_create_file(struct dth_directory *parent,
		const char *filename, dth_u32 perm,
		const struct dth_file_ops *ops);

/**
 * Search a file or a directory from a location and by specifying
 * the file name and possibly its path.
 *
 * Unlike dth_get_child_by_name, this function searches for the file in the
 * specified directories as well as in all directories under this one.
 *
 * @param[in] dir the directory from where to start the search. If this
 * parameter is NULL, the root directory is used instead. The parameter is
 * ignored if \arg pathname is an absolute path name.
 *
 * @param[in] pathname specifies the name, and possibly the path,
 * of the file to find. Absolute pathname are
 * refering to the DTH root directory.
 *
 * @return 0 if the file is found and \arg file is filled with the address of
 * the corresponding dth_file struct, or -1 if an error occurred in which case
 * errno is set appropriately:
 * EINVAL if one of the argument is not valid,
 * EBADF if \arg pathname is not a valid file descriptor
 * ENOENT if the pathname is not pointing to an existing file or directory.
 * ESTALE on program integrity error.
 */
int dth_find_file(struct dth_directory *dir, const char *pathname,
		struct dth_file **file);

struct dth_file *dth_find_file_by_name(struct dth_directory *directory,
				       const char *name);

/**
 * Delete a file of a directory. If removing a directory then all of its
 * children are also deleted.
 *
 * @reetrant
 * @param[in] parent specifies the parent directory. Cannot be NULL.
 * @param[in] filename file name.
 */
void dth_delete_branch(struct dth_file *from);

/**
 * Delete a file in the specified directory.
 *
 * @param[in] parent specifies the parent directory. Cannot be NULL.
 * @param[in] filename file name.
 * @return 0 if the file was successfully deleted, or -1 if an error
 * occurred in which case errno is set to:
 * EINVAL if one or more argument is invalid,
 * ENOENT if the file does not exist,
 * ENOTEMPTY if trying to remove a non empty directory,
 * ESTALE on program integrity error.
 */
int dth_delete_file(struct dth_directory *parent, const char* filename);

/**
 * Initialize a file with default values.
 *
 * Illegal names are '.', '..' or any name that contains an illegal character.
 *
 * Legal characters are letters (upper and lower case), digits, dash ('-'),
 * underscore ('_') and dot ('.').
 *
 *
 * @warning The use of this function is discouraged. Consider using
 * @ref dth_register_device first.
 *
 * @param[out] file specifies the file to initialize.
 * @param[in] name is an UTF-8 character string that specifies the name of
 *            file.
 * @param[in] size specifies length the name of file.
 */
int dth_initialize_file(struct dth_file *file, const char *name,
		unsigned size);

/**
 * Initialize a directory with default values.
 *
 * @warning The use of this function is discouraged. Consider using
 * @ref dth_register_device first.
 *
 * @param[out] directory specifies the directory to initialize.
 * @param[in] name is an UTF-8 character string that specifies the name of
 *            file.
 * @param[in] size specifies length the name of directory.
 */
int dth_initialize_directory(struct dth_directory *directory,
		const char *name, unsigned size);

/**
 * Search a directory for its nth child.
 * @param[in] directory specifies the directory
 * @param[in] index specifies the rank of the file to find
 * @return the address of the file or NULL if index is out of range.
 */
struct dth_file *dth_get_child_by_index(const struct dth_directory *directory,
		unsigned index);

/**
 * Search a directory for a child with a specified name.
 * @param[in] directory specifies the directory.
 * @param[in] name specifies the name of the file to find.
 * @param[in] size specifies the length of the name of the file.
 * @return the address of the file or NULL if no file with such a name exists.
 */
struct dth_file *dth_get_child_by_name(struct dth_directory *directory,
		const char *name, unsigned size);

/**
 * Get the directory entry of a file.
 * @param[in] file specifies the file
 * @param[out] dir returns the direntry of the file.
 */
void dth_get_file_stat(const struct dth_file *file, struct stat *dir);

typedef struct {
    FILE *output;
	/* Is to be used to customize the behaviour of dth_dump_tree_elt */
    unsigned long options;	/* not used yet. */
} dth_dump_tree_elt_args;

typedef int(*dth_enum_tree_proc)(struct dth_file *file, int *depth,
		void *data);

int dth_dump_tree_elt(struct dth_file *file, int *depth, void *data);

int dth_enum_tree(struct dth_file *start, dth_enum_tree_proc proc,
		int *depth, void *data);

#endif /* DTH_FILESYSTEM_H_ */
