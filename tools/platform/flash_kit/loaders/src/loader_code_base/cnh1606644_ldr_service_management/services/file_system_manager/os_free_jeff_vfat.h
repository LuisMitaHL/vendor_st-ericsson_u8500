/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _OS_FREE_JEFF_VFAT_H_
#define _OS_FREE_JEFF_VFAT_H_

/**
 *  @addtogroup ldr_service_fs
 *  @{
 *     @addtogroup ldr_service_fs_jeff_vfat JEFF VFAT File System
 *     @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "unistd.h"
#include "dirent.h"
#include "types.h"
#include "cfs_os_free.h"
#include "statvfs.h"
#include "fcntl.h"
#include "stat.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
typedef struct {
    mode_t st_mode;
    ino_t st_ino;
    dev_t st_dev;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    off_t st_size;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
} Lowlevel_Fs_Stat_t;

typedef struct {
    unsigned d_namlen;
    char d_name[256];
} LowLevel_Fs_DIR_t;


typedef void *(*fs_format_factory_t)(void);
typedef int (*fs_mount_volume_t)(const char *rootPath, const char *params, fs_format_factory_t format, DdbHandle handle, const DdbFunctions *funcs);
typedef int (*fs_unmount_all_volumes_t)(int force);
typedef int (*fs_format_volume_t)(const char *rootPath, const char *params, int force);

typedef int (*fs_open_file_t)(const char *name, int mode, int access);
typedef int (*fs_read_file_t)(int fd, char *buf, int len);
typedef int (*fs_write_file_t)(int fd, const void *buffer, size_t nbyte);
typedef int (*fs_close_file_t)(int fd);
typedef int (*fs_remove_file_t)(const char *FileName_p);

typedef int (*fs_rename_t)(const char *oldPath, const char *newPath);
typedef int (*fs_chmod_t)(const char *path, mode_t mode);

typedef int (*fs_mkdir_t)(const char *name, unsigned int mode);
typedef DIR*(*fs_opendir_t)(const char *dirname);
typedef LowLevel_Fs_DIR_t*(*fs_readdir_t)(DIR *dirp);
typedef int (*fs_closedir_t)(DIR *dirp);
typedef int (*fs_chdir_t)(const char *pathname);
typedef int (*fs_rmdir_t)(const char *path);

typedef char*(*fs_getcwd_t)(char *path, size_t path_size);
typedef int (*fs_stat_t)(const char *pathname, Lowlevel_Fs_Stat_t *buf);
typedef int (*fs_fstat_t)(int fd, Lowlevel_Fs_Stat_t *buf);
typedef int (*fs_statvfs_t)(const char *path, struct statvfs *vfsbuf);
typedef int (*fs_sync_t)(void);
typedef int (*fs_fsync_t)(int fd);
typedef off_t(*fs_lseek_t)(int fd, off_t offset, int whence);
typedef mode_t (*fs_umask_t)(mode_t);
typedef void (*fs_rewinddir_t)(DIR *dirp);

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
extern fs_format_factory_t      _fs_JEFF_format;
extern fs_format_factory_t      _fs_VFAT_format;
extern fs_mount_volume_t        _fs_mount_volume;
extern fs_unmount_all_volumes_t _fs_unmount_all_volumes;
extern fs_format_volume_t       _fs_format_volume;

extern fs_open_file_t           _fs_open_file;
extern fs_read_file_t           _fs_read_file;
extern fs_write_file_t          _fs_write_file;
extern fs_close_file_t          _fs_close_file;
extern fs_remove_file_t         _fs_remove_file;

extern fs_rename_t              _fs_rename;
extern fs_chmod_t               _fs_chmod;

extern fs_mkdir_t               _fs_mkdir;
extern fs_opendir_t             _fs_opendir;
extern fs_readdir_t             _fs_readdir;
extern fs_closedir_t            _fs_closedir;
extern fs_chdir_t               _fs_chdir;
extern fs_rmdir_t               _fs_rmdir;

extern fs_getcwd_t              _fs_getcwd;
extern fs_stat_t                _fs_stat;       //File or Dir stat, ref. by path
extern fs_fstat_t               _fs_fstat;      //Open file stat, ref. by File Desc.
extern fs_statvfs_t             _fs_statvfs;    //FS stat, ref. by path
extern fs_sync_t                _fs_sync;       //Sync whole FS
extern fs_fsync_t               _fs_fsync;      //Sync open file
extern fs_lseek_t               _fs_lseek;
extern fs_umask_t               _fs_umask;
extern fs_rewinddir_t           _fs_rewinddir;

int OS_Free_JEFF_VFAT_Init(void);

/** @} */
/** @} */
#endif
