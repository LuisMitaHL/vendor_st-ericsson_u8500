/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_service_management
 *  @{
 *     @addtogroup ldr_service_fs
 *     @{
 *        @addtogroup ldr_service_fs_jeff_vfat
 *        @{
 */


/*******************************************************************************
 * Includes
 ******************************************************************************/
#ifdef WIN32
#include <windows.h>
#else //WIN32
#include <stdio.h>
#include "jeff_format_factory.h"
#endif

#include "os_free_jeff_vfat.h"
#include "error_codes.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#ifdef WIN32
int *errno_p = NULL;
#endif


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
fs_format_factory_t      _fs_JEFF_format;
fs_format_factory_t      _fs_VFAT_format;
fs_mount_volume_t        _fs_mount_volume;
fs_unmount_all_volumes_t _fs_unmount_all_volumes;
fs_format_volume_t       _fs_format_volume;

fs_open_file_t           _fs_open_file;
fs_read_file_t           _fs_read_file;
fs_write_file_t          _fs_write_file;
fs_close_file_t          _fs_close_file;
fs_remove_file_t         _fs_remove_file;

fs_rename_t              _fs_rename;
fs_chmod_t               _fs_chmod;

fs_mkdir_t               _fs_mkdir;
fs_opendir_t             _fs_opendir;
fs_readdir_t             _fs_readdir;
fs_closedir_t            _fs_closedir;
fs_chdir_t               _fs_chdir;
fs_rmdir_t               _fs_rmdir;

fs_getcwd_t              _fs_getcwd;
fs_stat_t                _fs_stat;       //File or Dir stat, ref. by path
fs_fstat_t               _fs_fstat;      //Open file stat, ref. by File Desc.
fs_statvfs_t             _fs_statvfs;    //FS stat, ref. by path
fs_sync_t                _fs_sync;       //Sync whole FS
fs_fsync_t               _fs_fsync;      //Sync open file
fs_lseek_t               _fs_lseek;
fs_umask_t               _fs_umask;
fs_rewinddir_t           _fs_rewinddir;

int OS_Free_JEFF_VFAT_Init(void)
{
#ifdef WIN32

    HINSTANCE hDLL;

    hDLL = LoadLibrary("libcfs.dll");

    errno_p                 = (int *)GetProcAddress(hDLL, "_ZN9CfsHosted5errnoE");

    _fs_JEFF_format         = (fs_format_factory_t)GetProcAddress(hDLL, "JeffFormatFactory");
    _fs_VFAT_format         = (fs_format_factory_t)GetProcAddress(hDLL, "VFATFormatFactory");

    _fs_mount_volume        = (fs_mount_volume_t)GetProcAddress(hDLL, "cfs_os_free_mount_volume");
    _fs_unmount_all_volumes = (fs_unmount_all_volumes_t)GetProcAddress(hDLL, "cfs_os_free_unmount_all_volumes");
    _fs_format_volume       = (fs_format_volume_t)GetProcAddress(hDLL, "cfs_os_free_format_volume");

    _fs_open_file           = (fs_open_file_t)GetProcAddress(hDLL, "cfs_open");
    _fs_read_file           = (fs_read_file_t)GetProcAddress(hDLL, "cfs_read");
    _fs_write_file          = (fs_write_file_t)GetProcAddress(hDLL, "cfs_write");
    _fs_close_file          = (fs_close_file_t)GetProcAddress(hDLL, "cfs_close");
    _fs_remove_file         = (fs_remove_file_t)GetProcAddress(hDLL, "_ZN9CfsHosted6removeEPKc");

    _fs_rename              = (fs_rename_t)GetProcAddress(hDLL, "_ZN9CfsHosted6renameEPKcS1_");
    _fs_chmod               = (fs_chmod_t)GetProcAddress(hDLL, "cfs_chmod");

    _fs_mkdir               = (fs_mkdir_t)GetProcAddress(hDLL, "cfs_mkdir");
    _fs_opendir             = (fs_opendir_t)GetProcAddress(hDLL, "_ZN9CfsHosted7opendirEPKc");
    _fs_readdir             = (fs_readdir_t)GetProcAddress(hDLL, "_ZN9CfsHosted7readdirEPNS_4_DIRE");
    _fs_closedir            = (fs_closedir_t)GetProcAddress(hDLL, "_ZN9CfsHosted8closedirEPNS_4_DIRE");
    _fs_chdir               = (fs_chdir_t)GetProcAddress(hDLL, "cfs_chdir");
    _fs_rmdir               = (fs_rmdir_t)GetProcAddress(hDLL, "_ZN9CfsHosted5rmdirEPKc");

    //  _fs_getcwd                      = (fs_getcwd_t)GetProcAddress(hDLL, "_ZN9CfsHosted6getcwdEPcj");
    _fs_stat                = (fs_stat_t)GetProcAddress(hDLL, "_ZN9CfsHosted4statEPKcPNS_4statE");
    _fs_fstat               = (fs_fstat_t)GetProcAddress(hDLL, "_ZN9CfsHosted5fstatEiPNS_4statE");
    _fs_statvfs             = (fs_statvfs_t)GetProcAddress(hDLL, "_ZN9CfsHosted7statvfsEPKcPNS_7statvfsE");
    _fs_sync                = (fs_sync_t)GetProcAddress(hDLL, "cfs_os_free_sync");
    _fs_fsync               = (fs_fsync_t)GetProcAddress(hDLL, "_ZN9CfsHosted5fsyncEi");
    _fs_lseek               = (fs_lseek_t)GetProcAddress(hDLL, "cfs_lseek");
    _fs_umask               = (fs_umask_t)GetProcAddress(hDLL, "_ZN9CfsHosted5umaskEi");
    _fs_rewinddir            = (fs_rewinddir_t)GetProcAddress(hDLL, "_ZN9CfsHosted9rewinddirEPNS_4_DIRE");

#else

    _fs_JEFF_format         = (fs_format_factory_t)JeffFormatFactory;
    //  _fs_VFAT_format         = (fs_format_factory_t)VFATFormatFactory;

    _fs_mount_volume        = (fs_mount_volume_t)cfs_os_free_mount_volume;
    _fs_unmount_all_volumes = (fs_unmount_all_volumes_t)cfs_os_free_unmount_all_volumes;
    _fs_format_volume       = (fs_format_volume_t)cfs_os_free_format_volume;

    _fs_open_file           = (fs_open_file_t)open;
    _fs_read_file           = (fs_read_file_t)read;
    _fs_write_file          = (fs_write_file_t)write;
    _fs_close_file          = (fs_close_file_t)close;
    _fs_remove_file         = (fs_remove_file_t)remove;

    _fs_rename              = (fs_rename_t)rename;
    _fs_chmod               = (fs_chmod_t)chmod;

    _fs_mkdir               = (fs_mkdir_t)mkdir;
    _fs_opendir             = (fs_opendir_t)opendir;
    _fs_readdir             = (fs_readdir_t)readdir;
    _fs_closedir            = (fs_closedir_t)closedir;
    _fs_chdir               = (fs_chdir_t)chdir;
    _fs_rmdir               = (fs_rmdir_t)rmdir;

    //  _fs_getcwd                      = (fs_getcwd_t)getcwd;
    _fs_stat                = (fs_stat_t)stat;
    _fs_fstat               = (fs_fstat_t)fstat;
    _fs_statvfs             = (fs_statvfs_t)statvfs;
    _fs_sync                = (fs_sync_t)cfs_os_free_sync;
    _fs_fsync               = (fs_fsync_t)fsync;
    _fs_lseek               = (fs_lseek_t)lseek;
    _fs_umask               = (fs_umask_t)umask;
    _fs_rewinddir           = (fs_rewinddir_t)rewinddir;

#endif

    return E_SUCCESS;
}

/** @} */
/** @} */
/** @} */
