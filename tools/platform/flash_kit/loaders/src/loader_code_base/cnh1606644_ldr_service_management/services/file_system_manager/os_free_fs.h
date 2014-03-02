/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _OS_FREE_FS_H_
#define _OS_FREE_FS_H_

/**
 *  @addtogroup ldr_service_management
 *  @{
 *     @addtogroup ldr_service_fs File System
 *     @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stddef.h>

#include "t_basicdefinitions.h"
#include "stat.h"
#include "dirent.h"
#include "fcntl.h"
#include "unistd.h"
#include "error_codes.h"
#include "r_os.h"

extern ErrorCode_e err_os_free;
/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define SEC_NO_INDENTION
#define SEC_NO_FUNCTION_NAME
#define SEC_NO_FILE_NUMBER

#define OS_FREE_FS_MAX_DIRECTORY_NAME_SIZE 255
#define OS_FREE_FS_FILENAME_MAX_LENGTH FILENAME_MAX
#define FS_ERROR 0xffffffffffffffff
#define NEW_UMASK  0
#define FULL_ACCESS_RIGHTS  (0777)
#define UCS2_FILENAME_MAX 255
#ifdef w_open
#undef w_open
#endif

#ifdef flush
#undef flush
#endif

#ifdef w_remove
#undef w_remove
#endif

#ifdef w_rename
#undef w_rename
#endif

//#define fileno(p)        (int)(p)
//#define fsync(a)         OS_Free_Fsync(a)

//#define w_cfs_os_free_format_volume(a,b,c) OS_Free_Format_Volume(a)

#define w_open(a,b,c)                      OS_Free_w_Open_File(a,b,c)
#define close(a)                           OS_Free_Close_File(a)

#define w_remove(a)                        OS_Free_w_Remove_File(a)

#define w_rename(a,b)                      OS_Free_w_Rename(a,b)
#define w_chmod(a,b)                       OS_Free_w_Chmod(a,b)

#define w_mkdir(a,b)                       OS_Free_w_Mkdir(a,b)
#define w_opendir(a)                       OS_Free_w_Opendir(a)
#define w_chdir(a)                         OS_Free_w_Chdir(a)
#define wchdir(a)                          OS_Free_w_Chdir(a)
#define w_rmdir(a)                         OS_Free_w_Rmdir(a)

#define w_stat(a,b)                        OS_Free_w_Stat(a,b)
#define fstat(a,b)                         OS_Free_Fstat(a,b)

#define cfs_os_free_sync                   OS_Free_Sync

#ifdef WIN32
#define read(a,b,c)                        (ssize_t)OS_Free_Read_File(a,b,c)
#define write(a,b,c)                       (ssize_t)OS_Free_Write_File(a,b,c)
#define close(a)                           OS_Free_Close_File(a)

#define fsync(a)                           OS_Free_Fsync(a)
#define lseek(a,b,c)                       (off_t)OS_Free_Lseek(a,b,c)
#endif

/*******************************************************************************
 *    File system function types
 ******************************************************************************/
#define SUPPORTED_FILE_SYSTEMS 2
#define JEFF_S_IFMT (S_IFDIR | S_IFCHR | S_IFBLK | S_IFREG | S_IFFIFO | S_IFLNK | S_IFSOCK)

typedef enum {
    OS_Free_JEFF,
    OS_Free_VFAT
} FS_types_t;

typedef enum {
    OS_FREE_S_IFMT   = 0170000,  /* Mask of file type. */
    OS_FREE_S_IFSOCK = 0140000,  /* UNIX domain socket. */
    OS_FREE_S_IFLNK  = 0120000,  /* Symbolic link. */
    OS_FREE_S_IFREG  = 0100000,  /* Regular file. */
    OS_FREE_S_IFBLK  =  060000,  /* Block device. */
    OS_FREE_S_IFDIR  =  040000,  /* Directory file. */
    OS_FREE_S_IFCHR  =  020000,  /* Character device. */
    OS_FREE_S_IFIFO  =  010000   /* Named pipe (fifo). */
} OS_Free_ObjectTypeMask_t;

#define stat OS_Free_Stat_s
typedef struct OS_Free_Stat_s {
    uint32 st_mode;
    off_t  st_size;
    uint32 st_atime;
    uint32 st_mtime;
    uint32 st_ctime;
} OS_Free_File_Stat_t;

typedef struct OS_Free_DIR_s {
    char d_name[256];
} OS_Free_DIR_t;

typedef struct {
    char Name[OS_FREE_FS_MAX_DIRECTORY_NAME_SIZE + 1];
    int Unit;
    int Start_Block;
    int End_Block;
    FS_types_t FS_type;
    boolean Params_Saved;
} Volume_Params_t;

/*******************************************************************************
 * Function declarations
 ******************************************************************************/
/**
 * Function used to initialize the file system
 *
 */
int Do_FS_Init(void);

/**
 * Stops the file system
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_FS_Stop(void);

/**
 * Starts the file system
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_FS_Start(void);

/**
 * Retrieves the info of a certain volume
 *
 * @param [in]      VolumeInfo_p     Pointer to Volume_Params_t structure
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e GetVolumeInfo(Volume_Params_t *VolumeInfo_p);

/**
 * Returns the number of mounted volumes
 */
uint32 GetNrOfMountedVolumes(void);

/**
 * Set tuhe configuration of a volume
 *
 * @param [in]      Volume_Name_p     Name of the volume
 * @param [in]      Unit              CABS unit number
 * @param [in]      Start_Block       the start block of the volume
 * @param [in]      End_Block         the end block of the volume
 * @param [in]      FS_Type           type of the volume's file system
 *
 */
int OS_Free_Set_Volume_Config(const char *Volume_Name_p, const int Unit, const int Start_Block, const int End_Block, const FS_types_t FS_Type);

/**
 * Mounts volume to the ME's file system
 *
 * @param [in]      Volume_Name_p     Name of the volume
 *
 */
int OS_Free_Mount_Volume(const char *Volume_Name_p);

/**
 * Unmounts all mounted volumes
 *
 * @param [in]      force     indicates forceed unmount
 *
 */
int OS_Free_Unmount_All_Volumes(int force);

/**
 * Formats volume indetified by the input parameter
 *
 * @param [in]      RootPath_p     volume's root path
 *
 */
int OS_Free_Format_Volume(const char *RootPath_p);

/**
 * Returns a list of all mounted volumes
 *
 * @param [out]      Output_p    output buffer
 * @param [out]      OutputSize  size of the output buffer
 *
 */
int OS_Free_List_Mounted_Volumes(char *Output_p, uint32 *OutputSize);

/**
 * Returns the free space of a volume
 *
 * @param [in]      Path_p   the path for which the free space needs to
 *                            be retured
 *
 */
uint64 OS_Free_Free_Space(const char *Path_p);
/**
 * Widechar version of OS_Free_Free_Space
 *
 */
uint64 OS_Free_w_Free_Space(const wchar_t *Path_p);

/**
 * Returns the size of a volume
 *
 * @param [in]      Path_p   the path for which the size needs to
 *                            be retured
 *
 */
uint64 OS_Free_Total_Size(const char *Path_p);

/**
 *  Opens a file located at PathName_p
 *
 *  @param [in]        PathName_p  path of the file that needs to be opend
 *  @param [in]        Omode  the open mode of the file
 *  @param [in]        Amode  the access mode of the file
 */
int OS_Free_Open_File(const char *PathName_p, int Omode, mode_t Amode);

/**
 * Widechar version of OS_Free_Open_File
 *
 */
int OS_Free_w_Open_File(const wchar_t *PathName_p, int Omode, mode_t Amode);

/**
 *  Reads from a file identified by fd
 *
 *  @param [in]        fd   file handle
 *  @param [out]       buf  buffer to store the data read from the file
 *  @param [in]        len  total number of bytes to be read
 */
uint64 OS_Free_Read_File(int fd, char *buf, uint64 len);

/**
 *  Writes to a file identified by fd
 *
 *  @param [in]        fd   file handle
 *  @param [out]       buf  data to be written
 *  @param [in]        len  total number of bytes to be written
 */
uint64 OS_Free_Write_File(int fd, const char *buf, uint64 len);

/**
 *  Function used to close a file identified by fd
 */
int OS_Free_Close_File(int fd);

/**
 *  Removes a file from the file system
 *
 *  @param [in]        FileName_p   path of the file that needs
 *                                  to be removed
 */
int OS_Free_Remove_File(const char *FileName_p);

/**
 *  Widechar version of OS_Free_Remove_File
 *
 */
int OS_Free_w_Remove_File(const wchar_t *FileName_p);

/**
 *  Renames a file from the file system
 *
 *  @param [in]        OldFileName_p  the old name of the file
 *  @param [in]        NewFileName_p  the new name of the file
 */
int OS_Free_Rename(const char *const OldFileName_p, const char *const NewFileName_p);

/**
 *  Widechar version of OS_Free_Rename
 *
 */
int OS_Free_w_Rename(const wchar_t *const OldFileName_p, const wchar_t *const NewFileName_p);

/**
 *  Changes the access mode of a file
 *
 *  @param [in]        Path_p  the path of the file
 *  @param [in]        Amode   the new access mode of the file
 */
int OS_Free_Chmod(const char *Path_p, mode_t Amode);

/**
 *  Widechar version of OS_Free_Chmod
 *
 */
int OS_Free_w_Chmod(const wchar_t *Path_p, mode_t Amode);

/**
 *  Creates new directory on the file system
 *
 *  @param [in]        Path_p  the path of the directory
 *  @param [in]        Amode   the new access mode of the directory
 */
int OS_Free_Mkdir(const char *Path_p, mode_t Amode);

/**
 *  Widechar version of OS_Free_Mkdir
 *
 */
int OS_Free_w_Mkdir(const wchar_t *Path_p, mode_t Amode);

/**
 *  Opens directory on the file system
 *
 *  @param [in]        dirname_p  the path of the directory
 */
DIR *OS_Free_Opendir(const char *dirname_p);

/**
 *  Widechar version of OS_Free_Opendir
 *
 */
DIR *OS_Free_w_Opendir(const wchar_t *dirname_p);

/**
 *  Reads directory dir_p from the file system
 *
 */
OS_Free_DIR_t *OS_Free_Readdir(DIR *dir_p);

/**
 *  Function used to close directory dirp
 *
 */
int OS_Free_Closedir(DIR *dirp);

/**
 *  Function used to change the current directory
 *
 *  @param [in]        Path_p  the path of the new directory
 */
int OS_Free_Chdir(const char *Path_p);

/**
 *  Widechar version of OS_Free_Chdir
 *
 */
int OS_Free_w_Chdir(const wchar_t *Path_p);

/**
 *  Removes a directory from the file system
 *
 *  @param [in]        Path_p  the path of the directory
 *                     to be removed
 */
int OS_Free_Rmdir(const char *Path_p);

/**
 *  Widechar version of OS_Free_Rmdir
 *
 */
int OS_Free_w_Rmdir(const wchar_t *Path_p);

/**
 *  Returns the Stat of a directory or file located in the file system
 *
 *  @param [in]        Path_p  the path of the directory or the file
 *  @param [out]       Buf_p   the stat of the directory or the file
 */
int OS_Free_Stat(const char *Path_p, OS_Free_File_Stat_t *Buf_p);

/**
 *  Widechar version of OS_Free_Stat
 *
 */
int OS_Free_w_Stat(const wchar_t *Path_p, OS_Free_File_Stat_t *Buf_p);
int OS_Free_Fstat(int fd, OS_Free_File_Stat_t *Buf_p);

/**
 * Synchronises the file system with the current flash data
 */
int OS_Free_Sync(void);

/**
 * Synchronises the file system with the current flash data
 */
int OS_Free_Fsync(int fd);

uint64 OS_Free_Lseek(int fd, uint64 offset, int whence);
mode_t OS_Free_Umask(mode_t mask);
/**
 * Rewinds a directory
 *
 * @param [in]      dirp  the path of the directory to be rewind
 */
void OS_Free_Rewinddir(DIR *dirp);

/**
 * Conversts UTF8 string to  widechar string
 *
 * @param [in]      UTF8Str      the UTF8 string to be converted
 * @param [in]      UTF8StrLen   the length of the UTF8 string
 * @param [out]     WideCharStr  the widechar string
 * @param [out]     WideStrLen   the length of the widechar string
 *
 */
int UTF8ToWideChar(const char *UTF8Str, int UTF8StrLen, char *WideCharStr, int WideStrLen);

/**
 * Conversts widechar string to  UTF8 string
 *
 * @param [in]      WideCharStr  the widechar string to be converted
 * @param [in]      WideStrLen   the length of the widechar string
 * @param [out]     UTF8Str      the UTF8 string
 * @param [out]     UTF8StrLen   the length of the UTF8 string
 *
 */
int WideCharToUTF8(const uint8 *WideCharStr, int WideStrLen, char *UTF8Str, int UTF8StrLen);

/**
 * Returns the type of a volume's file system
 *
 * @param [in]      VolumeNameIn_p  the volume name
 */
char *Get_FS_Type(char *VolumeNameIn_p);

/**
 * Compares two paths from the file system
 *
 */
boolean CompareSourceAndDestVolumes(const char *Source_p, const char *Destination_p);

/** @} */
/** @} */
#endif
