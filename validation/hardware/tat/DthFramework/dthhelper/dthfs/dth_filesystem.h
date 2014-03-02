/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   DTH filesystem
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTH_FILESYSTEM_INTERNAL_H_
#define DTH_FILESYSTEM_INTERNAL_H_

#include <dthfs/dth_filesystem.h>
#include <dth_utils.h>

/**
 * Get the QID of a file.
 * @param[in] file specifies the file
 * @param[out] qid returns the QID of the file.
 */
static inline void dth_get_file_qid(const struct dth_file *file,
		struct dth_qid *qid)
{
	qid->type = file->perm >> 24;
	qid->version = file->version;
	qid->path = file->path;
}

/**
* Check permissions against world access, group access and owner access.
* @param file: file against which to check permission
* @param bit: 0 for execution, 1 for write, 2 for read
* @todo: only stands for unauthenticated access as the user is not verified to
* be an actual member of the group or owner of the file.
*/
static inline int dth_check_file_permission(const struct dth_file *file, int bit)
{
    unsigned long perm = file->perm >> bit;

    return (perm & (1 << 0)) || (perm & (1 << 3)) || (perm & (1 << 6));
}

/**
 * Calculate a 9P directory entry encoded size.
 * @param[in] dir specifies the 9P directory entry in a machine intelligible
 *                format.
 * @return the amount of bytes required to encode the 9P directory entry.
 */
unsigned long dth_get_dir_size(const struct dth_dir *dir);


/**
 * Check if a directory is the root of a file tree.
 * @param[in] dir specifies the directory.
 * @return 0 if and only if the directory is not a root.
 */
static inline int dth_directory_is_root(const struct dth_directory *dir)
{
	return dir->file.parent == dir;
}

#endif /* DTH_FILESYSTEM_INTERNAL_H_ */
