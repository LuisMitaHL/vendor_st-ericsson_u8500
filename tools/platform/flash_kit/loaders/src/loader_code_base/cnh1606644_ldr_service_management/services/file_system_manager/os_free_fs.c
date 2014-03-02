/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_service_management
 *  @{
 *     @addtogroup ldr_service_fs
 *     @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system_v2.h"
#include <time.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#endif //WIN32

#include "os_free_fs.h"
#ifdef CNH1606644_FS_USES_JEFF_VFAT
#include "os_free_jeff_vfat.h"
#endif

#include "t_basicdefinitions.h"
#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "error_codes.h"
#include "block_device_management.h"
#include "errno.h"
#include "r_service_management.h"
#include "r_path_utilities.h"
#include "r_configuration_parser.h"

#ifdef CFG_ENABLE_LINT_TARGET_BUILD
#ifdef errno
#undef errno
#endif
int errno;  /* LINT reports error for extern errno */
#endif

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define FS_FATAL_ERROR      -1
#define MAX_NUMBER_OF_MOUNTED_VOLUMES  0x20  // The maximum number of mountable volumes

static Volume_Params_t *Saved_Volume_Params_p[MAX_NUMBER_OF_MOUNTED_VOLUMES + 1] = { NULL};
static OS_Free_DIR_t *MountedVolumes_p[MAX_NUMBER_OF_MOUNTED_VOLUMES + 1] = { NULL};
static uint32 nrMountedVolumes = 0;

ErrorCode_e err_os_free;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static void ConvertErrnoToLoaderErr(void);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 *  Initializes the FS. Add another FS if needed.
 */
int Do_FS_Init(void)
{
    int Result = E_SUCCESS;

    Saved_Volume_Params_p[0] = (Volume_Params_t *)malloc(sizeof(Volume_Params_t));
    ASSERT(NULL != Saved_Volume_Params_p[0]);

    Saved_Volume_Params_p[0]->Params_Saved = FALSE;

#ifdef CNH1606644_FS_USES_JEFF_VFAT
    Result = OS_Free_JEFF_VFAT_Init();
#endif

    return Result;
}

ErrorCode_e Do_FS_Stop(void)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    int JeffResult;

    C_(printf("os_free_fs.c(%d): Trying to stop FS...\n", __LINE__);)

    JeffResult = _fs_sync();
    C_(printf("os_free_fs.c(%d): OS_Free_Sync returned %d\n", __LINE__, JeffResult);)

    JeffResult = OS_Free_Unmount_All_Volumes(TRUE);
    C_(printf("os_free_fs.c(%d): OS_Free_Unmount_All_Volumes returned %d\n", __LINE__, JeffResult);)

    C_(printf("os_free_fs.c(%d): FS stopped successfully!\n", __LINE__);)

    IDENTIFIER_NOT_USED(JeffResult);

    //ErrorExit:
    return ReturnValue;
}

/*
 * Starts FS
 */
ErrorCode_e Do_FS_Start(void)
{
#ifdef CFG_ENABLE_USE_DEFAULT_FS_CONFIGURATION
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    int InterResult;
    uint32 SampleTextCounter;
    char SampleText[] = "Test file, only for internal test!";
    mode_t oldmask = 0;

    memset(MountedVolumes_p, 0x00, sizeof(MountedVolumes_p));

    InterResult = OS_Free_Set_Volume_Config("/boot", 0, 0, 7704, OS_Free_JEFF);
    C_(printf("os_free_fs.c (%d): OS_Free_Set_Volume_Config returned %d \n", __LINE__, InterResult);)
    InterResult = OS_Free_Mount_Volume("/boot");
    C_(printf("os_free_fs.c (%d): Mount BOOT returned %d \n", __LINE__, InterResult);)

    InterResult = OS_Free_Set_Volume_Config("/sys", 0, 7705, 7770, OS_Free_JEFF);
    C_(printf("os_free_fs.c (%d): OS_Free_Set_Volume_Config returned %d \n", __LINE__, InterResult);)
    InterResult = OS_Free_Mount_Volume("/sys");
    C_(printf("os_free_fs.c (%d): Mount SYS returned %d \n", __LINE__, InterResult);)

    if (-1 == InterResult) {
        A_(printf("os_free_fs.c (%d): Mount SYS failed (%d)! \n", __LINE__, InterResult);)
    }

    InterResult = OS_Free_Set_Volume_Config("/ifs", 0, 7771, 7836, OS_Free_JEFF);
    C_(printf("os_free_fs.c (%d): OS_Free_Set_Volume_Config returned %d \n", __LINE__, InterResult);)
    InterResult = OS_Free_Mount_Volume("/ifs");
    C_(printf("os_free_fs.c (%d): Mount IFS returned %d \n", __LINE__, InterResult);)

    InterResult = OS_Free_Set_Volume_Config("/afs", 0, 7837, 7902, OS_Free_JEFF);
    C_(printf("os_free_fs.c (%d): OS_Free_Set_Volume_Config returned %d \n", __LINE__, InterResult);)
    InterResult = OS_Free_Mount_Volume("/afs");
    C_(printf("os_free_fs.c (%d): Mount AFS returned %d \n", __LINE__, InterResult);)

    InterResult = _fs_chdir("/boot");

    if (-1 == InterResult) {
        /* FS image must be flashed before using the FS. We don't have FS image, so we will format the partitions. */
        InterResult = _fs_format_volume("/boot", "", TRUE);
        C_(printf("os_free_fs.c (%d): Format BOOT returned %d \n", __LINE__, InterResult);)

        InterResult = _fs_format_volume("/sys", "", TRUE);
        C_(printf("os_free_fs.c (%d): Format SYS returned %d \n", __LINE__, InterResult);)

        InterResult = _fs_format_volume("/ifs", "", TRUE);
        C_(printf("os_free_fs.c (%d): Format IFS returned %d \n", __LINE__, InterResult);)

        InterResult = _fs_format_volume("/afs", "", TRUE);
        C_(printf("os_free_fs.c (%d): Format AFS returned %d \n", __LINE__, InterResult);)

        oldmask = _fs_umask(NEW_UMASK);

        InterResult = _fs_open_file("/afs/test.txt", O_CREAT | O_EXCL | O_RDWR, S_IREAD | S_IWRITE | S_IEXEC);
        C_(printf("os_free_fs.c (%d): Open file returned %d \n", __LINE__, InterResult);)

        for (SampleTextCounter = 5000; SampleTextCounter != 0; SampleTextCounter--) {
            (void)_fs_write_file(InterResult, SampleText, sizeof(SampleText));
        }

        InterResult = _fs_close_file(InterResult);
        C_(printf("os_free_fs.c (%d): Close file returned %d \n", __LINE__, InterResult);)
    }

    if (-1 == InterResult) {
        A_(printf("os_free_fs.c (%d): Mount SYS failed (%d)! \n", __LINE__, InterResult);)
        return E_FS_INTERNAL;
    }

    ReturnValue = E_SUCCESS;

    IDENTIFIER_NOT_USED(oldmask);

    return ReturnValue;

#else
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ConfigurationParser_t VolumeConfig;
    uint8 *VolumeInfo_p = NULL;
    FS_types_t FS_type = OS_Free_JEFF;
    int FileHandle = -1;
    int InterResult = -1;
    int Unit = 0;
    int VolumeInfoLen = 0;
    int FileSize = 0;
    uint32 i = 0;

    /*
     * Mount boot volume on first CABS unit.
     */
    InterResult = OS_Free_Set_Volume_Config("/boot", 0, 0, -1, OS_Free_JEFF);
    VERIFY(InterResult != -1, E_GENERAL_FATAL_ERROR);
    InterResult = OS_Free_Mount_Volume("/boot");
    VERIFY(InterResult != -1, E_FS_NOT_MOUNTED);

    C_(printf("os_free_fs.c(%d): Search for fsconfig!\n", __LINE__);)

    FileHandle = _fs_open_file("/boot/fsconfig", O_RDONLY, 0777);

    if (-1 == FileHandle) {
        C_(printf("os_free_fs.c(%d): Error opening fsconfig!\n", __LINE__);)
        ReturnValue = E_FS_NO_SUCH_FILE_OR_DIR;
        goto ErrorExit;
    }

    C_(printf("os_free_fs.c(%d): fsconfig  FileHandle = %d!\n", __LINE__, FileHandle);)

    /* Calculate size of file, and than return stream pointer at begining.*/
    InterResult = _fs_lseek(FileHandle, 0, SEEK_SET);

    if (-1 == InterResult) {
        C_(printf("os_free_fs.c(%d): lseek failed!\n", __LINE__);)
        ConvertErrnoToLoaderErr();
        ReturnValue = err_os_free;
        goto ErrorExit;
    }

    FileSize = _fs_lseek(FileHandle, 0, SEEK_END);

    if (-1 == InterResult) {
        C_(printf("os_free_fs.c(%d): lseek failed!\n", __LINE__);)
        ConvertErrnoToLoaderErr();
        ReturnValue = err_os_free;
        goto ErrorExit;
    }

    C_(printf("os_free_fs.c(%d): fsconfig size = %d!\n", __LINE__, FileSize);)

    InterResult = _fs_lseek(FileHandle, 0, SEEK_SET);

    if (-1 == InterResult) {
        C_(printf("os_free_fs.c(%d): lseek failed!\n", __LINE__);)
        ConvertErrnoToLoaderErr();
        ReturnValue = err_os_free;
        goto ErrorExit;
    }

    VolumeInfo_p = (uint8 *)malloc(FileSize);
    ASSERT(NULL != VolumeInfo_p);

    VolumeInfoLen = _fs_read_file(FileHandle, (char *)VolumeInfo_p, (int)FileSize);
    VERIFY(-1 != VolumeInfoLen, E_FAILED_READING_FROM_FILE);

    C_(printf("os_free_fs.c(%d): VolumeInfoLen = %d!", __LINE__, VolumeInfoLen);)

    (void)_fs_close_file(FileHandle);

    Saved_Volume_Params_p[0]->Params_Saved = FALSE;

    //Get configuration for first volume
    ReturnValue = GetVolumeConfiguration(&VolumeConfig, VolumeInfo_p, VolumeInfoLen);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    C_(
        printf("\nVolume Configuration:\n");
        printf("VolumeName      = %s\n", VolumeConfig.VolumeName);
        printf("TypeName        = %s\n", VolumeConfig.TypeName);
        printf("FSMountingParam = %s\n", VolumeConfig.FSMountingParam);
        printf("DeviceName      = %s\n", VolumeConfig.DeviceName);
        printf("LoBlkNo         = %d\n", VolumeConfig.LoBlkNo);
        printf("HiBlkNo         = %d\n", VolumeConfig.HiBlkNo);
        printf("IsSet           = %d\n", VolumeConfig.IsSet);
    )

    while (FALSE != VolumeConfig.IsSet) {
        sscanf(VolumeConfig.DeviceName, "cabs%d", &Unit);

        if (0 == strncmp(VolumeConfig.TypeName, "jeff", strlen("jeff"))) {
            FS_type = OS_Free_JEFF;

            InterResult = OS_Free_Set_Volume_Config(VolumeConfig.VolumeName, Unit,
                                                    VolumeConfig.LoBlkNo, VolumeConfig.HiBlkNo, FS_type);
            VERIFY(InterResult != -1, E_GENERAL_FATAL_ERROR);
        } else {
            C_(printf("os_free_fs.c (%d): Unknown volume type. (%s)\n", __LINE__, VolumeConfig.TypeName);)
        }

        //Get configuration for next volume
        ReturnValue = GetVolumeConfiguration(&VolumeConfig, VolumeInfo_p, VolumeInfoLen);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        C_(
            printf("\nVolume Configuration:\n");
            printf("VolumeName      = %s\n", VolumeConfig.VolumeName);
            printf("TypeName        = %s\n", VolumeConfig.TypeName);
            printf("FSMountingParam = %s\n", VolumeConfig.FSMountingParam);
            printf("DeviceName      = %s\n", VolumeConfig.DeviceName);
            printf("LoBlkNo         = %d\n", VolumeConfig.LoBlkNo);
            printf("HiBlkNo         = %d\n", VolumeConfig.HiBlkNo);
            printf("IsSet           = %d\n", VolumeConfig.IsSet);
        )
    }

    /*
     * Unmount all volumes before startin mounting volumes that was readed from
     * fsconfig file.
     */
    (void)OS_Free_Unmount_All_Volumes(TRUE);

    while (TRUE == Saved_Volume_Params_p[i]->Params_Saved) {
        C_(printf("os_free_fs.c (%d): Trying to mount volume %s!\n", __LINE__, Saved_Volume_Params_p[i]->Name);)

        InterResult = OS_Free_Mount_Volume(Saved_Volume_Params_p[i]->Name);

        if (-1 == InterResult) {
            A_(printf("os_free_fs.c (%d): Mounting volume %s returned %d\n", __LINE__, Saved_Volume_Params_p[i]->Name, InterResult);)
            ConvertErrnoToLoaderErr();
            ReturnValue = err_os_free;
            goto ErrorExit;
        }

        i++;
    }

    if (i == 0) {
        A_(printf("\nos_free_fs.c(%d): Mounted Volumes = NONE!!!\n", __LINE__);)
        ReturnValue = E_FS_NOT_MOUNTED;
        goto ErrorExit;
    }

    (void)_fs_umask(NEW_UMASK);

    ReturnValue = E_SUCCESS;

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        (void)OS_Free_Unmount_All_Volumes(TRUE);
        Saved_Volume_Params_p[0]->Params_Saved = FALSE;
    }

    BUFFER_FREE(VolumeInfo_p);

    return ReturnValue;

#endif //CFG_ENABLE_DEFAULT_FS_CONFIGURATION
}

/*
 *  Saves volume configuration. Should be modified for YAFFS probably...
 */
int OS_Free_Set_Volume_Config(const char *Volume_Name_p, const int Unit, const int Start_Block, const int End_Block, const FS_types_t FS_Type)
{
    uint8 Free_Config_Space = 0;

    if (NULL == Volume_Name_p) {
        A_(printf("os_free_fs.c (%d): Invalid volume name!\n", __LINE__);)
        return FS_FATAL_ERROR;
    }

    //lint --e(650)
    if (FS_Type >= SUPPORTED_FILE_SYSTEMS) {
        A_(printf("os_free_fs.c (%d): Unsupported FS type!\n", __LINE__);)
        return FS_FATAL_ERROR;
    }

    while ((Free_Config_Space < MAX_NUMBER_OF_MOUNTED_VOLUMES) && Saved_Volume_Params_p[Free_Config_Space]->Params_Saved) {
        if (0 == strcmp(Saved_Volume_Params_p[Free_Config_Space]->Name, Volume_Name_p)) {
            A_(printf("os_free_fs.c (%d): Volume name %s already exists! Replacing old params!\n", __LINE__, Saved_Volume_Params_p[Free_Config_Space]->Name);)
            break;
        }

        Free_Config_Space ++;
    }

    if (Free_Config_Space == MAX_NUMBER_OF_MOUNTED_VOLUMES) {
        A_(printf("os_free_fs.c (%d): No free space to store volume params!\n", __LINE__);)
        return FS_FATAL_ERROR;
    }

    ASSERT(NULL != Saved_Volume_Params_p[Free_Config_Space]);
    memcpy(Saved_Volume_Params_p[Free_Config_Space]->Name, Volume_Name_p, OS_FREE_FS_MAX_DIRECTORY_NAME_SIZE + 1);

    Saved_Volume_Params_p[Free_Config_Space]->Unit = Unit;
    Saved_Volume_Params_p[Free_Config_Space]->Start_Block = Start_Block;
    Saved_Volume_Params_p[Free_Config_Space]->End_Block = End_Block;
    Saved_Volume_Params_p[Free_Config_Space]->FS_type = FS_Type;
    Saved_Volume_Params_p[Free_Config_Space]->Params_Saved = TRUE;

    if (Saved_Volume_Params_p[Free_Config_Space + 1] == NULL) {
        Saved_Volume_Params_p[Free_Config_Space + 1] = (Volume_Params_t *)malloc(sizeof(Volume_Params_t));
        ASSERT(NULL != Saved_Volume_Params_p[Free_Config_Space + 1]);

        Saved_Volume_Params_p[Free_Config_Space + 1]->Params_Saved = FALSE;
    }

    return E_SUCCESS;
}

/*
 *  Mounts volume. Should be modified for YAFFS
 */
int OS_Free_Mount_Volume(const char *Volume_Name_p)
{
    int           ErrorCode = FS_FATAL_ERROR;
    uint8         Volume_Config_Space = 0;
    char          Volume_Params[OS_FREE_FS_MAX_DIRECTORY_NAME_SIZE];
    DdbHandle     ddbHandle;
    DdbFunctions *DdbFuncs_p = NULL;

    if (nrMountedVolumes >= MAX_NUMBER_OF_MOUNTED_VOLUMES) {
        return E_MAX_NUMBER_OF_MOUNTED_VOLUMES_EXCEEDED;
    }

    while ((Volume_Config_Space < MAX_NUMBER_OF_MOUNTED_VOLUMES) && Saved_Volume_Params_p[Volume_Config_Space]->Params_Saved) {
        if (0 == strcmp(Saved_Volume_Params_p[Volume_Config_Space]->Name, Volume_Name_p)) {
            A_(printf("os_free_fs.c (%d): Found configuration for volume %s!\n", __LINE__, (uint8 *)Volume_Name_p);)
            break;
        }

        Volume_Config_Space ++;
    }

    if ((Volume_Config_Space == MAX_NUMBER_OF_MOUNTED_VOLUMES) || (Saved_Volume_Params_p[Volume_Config_Space]->Params_Saved == FALSE)) {
        A_(printf("os_free_fs.c (%d): Configuration for %s not found!\n", __LINE__, (uint8 *)Volume_Name_p);)
        return FS_FATAL_ERROR;
    }

    if (E_SUCCESS != (Do_BDM_GetFSInterface(Saved_Volume_Params_p[Volume_Config_Space]->Unit, &ddbHandle, &DdbFuncs_p))) {
        A_(printf("os_free_fs.c (%d): Getting DDB interface failed!\n", __LINE__);)
        return FS_FATAL_ERROR;
    }

    if (0 > _snprintf(Volume_Params, OS_FREE_FS_MAX_DIRECTORY_NAME_SIZE,
                      "unit=%d,lo=%d,hi=%d,global", Saved_Volume_Params_p[Volume_Config_Space]->Unit,
                      Saved_Volume_Params_p[Volume_Config_Space]->Start_Block,
                      Saved_Volume_Params_p[Volume_Config_Space]->End_Block)) {
        A_(printf("os_free_fs.c (%d): Error when formating Volume Parameters string !\n", __LINE__);)
        return FS_FATAL_ERROR;
    }

    switch (Saved_Volume_Params_p[Volume_Config_Space]->FS_type) {
    case OS_Free_JEFF:
        ErrorCode = _fs_mount_volume(Volume_Name_p, Volume_Params, _fs_JEFF_format, ddbHandle, DdbFuncs_p);
        break;

    case OS_Free_VFAT:
        ErrorCode = _fs_mount_volume(Volume_Name_p, Volume_Params, _fs_VFAT_format, ddbHandle, DdbFuncs_p);
        break;

    default:
        A_(printf("os_free_fs.c (%d): Not supported FS format!\n", __LINE__);)
        ErrorCode = FS_FATAL_ERROR;
        break;
    }

    if (E_SUCCESS == ErrorCode) {
        MountedVolumes_p[nrMountedVolumes] = (OS_Free_DIR_t *)malloc(sizeof(OS_Free_DIR_t));
        ASSERT(NULL != MountedVolumes_p[nrMountedVolumes]);

        strncpy(MountedVolumes_p[nrMountedVolumes]->d_name, Volume_Name_p, OS_FREE_FS_MAX_DIRECTORY_NAME_SIZE);
        nrMountedVolumes++;
        MountedVolumes_p[nrMountedVolumes] = NULL;

        if (nrMountedVolumes == MAX_NUMBER_OF_MOUNTED_VOLUMES) {
            B_(printf("os_free_fs.c(%d): Maximum number of volumes achieved!\n", __LINE__);)
        }
    }

    //  (void)Do_UnRegister_Service(FS_SERVICE);
    return ErrorCode;
}

/*
 *  Unmounts all volumes (doesn't exist in YAFFS) XVSZOAN
 */
int OS_Free_Unmount_All_Volumes(int force)
{
    int ErrorCode = FS_FATAL_ERROR;

    if (nrMountedVolumes != 0) {
#ifdef CNH1606644_FS_USES_JEFF_VFAT
        ErrorCode = _fs_unmount_all_volumes(force);
#endif
    } else {
        ErrorCode = E_NO_MOUNTED_DEVICES_ARE_FOUND;
    }

    nrMountedVolumes = 0;
    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 *  YAFFS uses different params (only one) XVSZOAN
 */
int OS_Free_Format_Volume(const char *RootPath_p)
{
    int ErrorCode = FS_FATAL_ERROR;

    if (NULL == RootPath_p) {
        A_(printf("os_free_fs.c (%d) Invalid input params!\n", __LINE__);)
        err_os_free = E_INVALID_INPUT_PARAMETERS;
        return FS_FATAL_ERROR;
    }

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    /* If the volume is "/sys", GDFS must be closed for successfully sync of the *.ps files. */
    if (0 == strncmp("/sys", RootPath_p, strlen("/sys"))) {
        (void)Do_Stop_Service(GD_SERVICE);
    }

#ifdef CNH1606644_FS_USES_JEFF_VFAT
    ErrorCode = _fs_format_volume(RootPath_p, "", TRUE);
#endif

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 *  Lists mounted volumes
 */
int OS_Free_List_Mounted_Volumes(char *Output_p, uint32 *OutputSize)
{
    uint8 Index = 0;
    uint8 i;
    uint8 TmpLen;
    uint8 OutIndex;

    C_(printf("os_free_fs.c (%d): Trying to list volumes...\n", __LINE__);)

    if (NULL == Output_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    if (NULL == OutputSize) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    OutIndex = 0;

    while (MountedVolumes_p[Index] != NULL) {
        //lint --e(734)
        TmpLen = strlen(MountedVolumes_p[Index]->d_name);

        for (i = 0; i < TmpLen; i++) {
            Output_p[OutIndex++] = MountedVolumes_p[Index]->d_name[i];
            Output_p[OutIndex++] = 0;
        }

        Output_p[OutIndex++] = 0;
        Output_p[OutIndex++] = 0;
        Index++;
    }

    *OutputSize = OutIndex;

    return E_SUCCESS;
}

/*
 * Returns volumes free space. Should be modified for YAFFS XVSZOAN
 */
uint64 OS_Free_Free_Space(const char *Path_p)
{
    uint64 FreeSpaceLeft = FS_ERROR;

#ifdef CNH1606644_FS_USES_JEFF_VFAT
    struct statvfs FS_Stat;
#endif

    if (NULL == Path_p) {
        A_(printf("os_free_fs.c (%d) Invalid input parameter!\n", __LINE__);)
        err_os_free = E_INVALID_INPUT_PARAMETERS;
        return FS_ERROR;
    }

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

#ifdef CNH1606644_FS_USES_JEFF_VFAT

    if (0 == _fs_statvfs(Path_p, &FS_Stat)) {
        FreeSpaceLeft = (uint64)FS_Stat.f_bfree * FS_Stat.f_bsize;
    }

#endif

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return FreeSpaceLeft;
}

uint64 OS_Free_Total_Size(const char *Path_p)
{
    uint64 TotalFS_Size = FS_ERROR;

#ifdef CNH1606644_FS_USES_JEFF_VFAT
    struct statvfs FS_Stat;
#endif

    if (NULL == Path_p) {
        A_(printf("os_free_fs.c (%d) Invalid input parameter!\n", __LINE__);)
        err_os_free = E_INVALID_INPUT_PARAMETERS;
        return FS_ERROR;
    }

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

#ifdef CNH1606644_FS_USES_JEFF_VFAT

    if (0 == _fs_statvfs(Path_p, &FS_Stat)) {
        TotalFS_Size = (uint64)FS_Stat.f_blocks * FS_Stat.f_frsize;
    }

#endif

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return TotalFS_Size;
}

/*
 * Returns volumes free space. Should be modified for YAFFS XVSZOAN
 */
uint64 OS_Free_w_Free_Space(const wchar_t *Path_p)
{
    uint64 FreeSpaceLeft = FS_ERROR;
    char Temp_Path[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];

    if (Path_p == NULL) {
        A_(printf("os_free_fs.c (%d) Invalid input parameter!\n", __LINE__);)
        err_os_free = E_INVALID_INPUT_PARAMETERS;
        return FS_ERROR;
    }

    (void)WideCharToUTF8((uint8 *)Path_p, -1, Temp_Path, OS_FREE_FS_FILENAME_MAX_LENGTH);

    FreeSpaceLeft = OS_Free_Free_Space(Temp_Path);

    return FreeSpaceLeft;
}

/*
 * XVSZOAN: Problem with omode and amode
 */
int OS_Free_Open_File(const char *PathName_p, int Omode, mode_t Amode)
{
    int Result = FS_FATAL_ERROR;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    if (CheckIfSecuredPath(PathName_p)) {
        // here goes code to check if writing is allowed
        // disalowed by default
        if (VerifyIncomingFile(PathName_p)) {
            Result = _fs_open_file(PathName_p, Omode, Amode);
        } else {
            Result = -1;
        }
    } else {
        Result = _fs_open_file(PathName_p, Omode, Amode);
    }


    C_(printf("os_free_fs.c (%d) openfile(%s) Result(%d)\n", __LINE__, PathName_p, Result);)

    if (-1 == Result) {
        if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
            return FS_FATAL_ERROR;
        }
    }

    ConvertErrnoToLoaderErr();
    return Result;
}

/*
 * XVSZOAN: Problem with omode and amode
 */
int OS_Free_w_Open_File(const wchar_t *PathName_p, int Omode, mode_t Amode)
{
    char Temp_Path[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];
    int Result = FS_FATAL_ERROR;

    if (NULL == PathName_p) {
        A_(printf("os_free_fs.c (%d) Invalid input parameter!\n", __LINE__);)
        err_os_free = E_INVALID_INPUT_PARAMETERS;
        return FS_FATAL_ERROR;
    }

    (void)WideCharToUTF8((uint8 *)PathName_p, -1, Temp_Path, OS_FREE_FS_FILENAME_MAX_LENGTH);

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    Result = _fs_open_file(Temp_Path, Omode, Amode);

    C_(printf("os_free_fs.c (%d) openfile(%s) Result(%d)\n", __LINE__, Temp_Path, Result);)

    if (-1 == Result) {
        if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
            return FS_FATAL_ERROR;
        }
    }

    ConvertErrnoToLoaderErr();
    return Result;
}

/*
 * Reads from file
 */
uint64 OS_Free_Read_File(int fd, char *buf, uint64 len)
{
    uint64 Result = FS_ERROR;
    int BytesRead = 0;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

    BytesRead = _fs_read_file(fd, buf, (int)len);

    if (-1 != BytesRead) {
        Result = BytesRead;
    }

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return Result;
}

/*
 * Writes to file
 */
uint64 OS_Free_Write_File(int fd, const char *buf, uint64 len)
{
    uint64 Result = FS_ERROR;
    int BytesWritten = 0;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

    //lint --e(732)
    BytesWritten = _fs_write_file(fd, buf, (int)len);

    if (-1 != BytesWritten) {
        Result = BytesWritten;
    }

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return Result;
}

/*
 * Closes file
 */
int OS_Free_Close_File(int fd)
{
    int ErrorCode = FS_FATAL_ERROR;

    ErrorCode = _fs_close_file(fd);

    C_(printf("os_free_fs.c (%d) close file(%d) Result(%d)\n", __LINE__, fd, ErrorCode);)

    if (-1 != ErrorCode) {
        if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
            return FS_FATAL_ERROR;
        }
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Deletes file
 */
int OS_Free_Remove_File(const char *FileName_p)
{
    int ErrorCode = FS_FATAL_ERROR;

    if (NULL == FileName_p) {
        A_(printf("os_free_fs.c (%d) Invalid input parameter!\n", __LINE__);)
        err_os_free = E_INVALID_INPUT_PARAMETERS;
        return FS_FATAL_ERROR;
    }

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_remove_file(FileName_p);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Deletes file
 */
int OS_Free_w_Remove_File(const wchar_t *FileName_p)
{
    char Temp_Path[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];
    int ErrorCode = FS_FATAL_ERROR;

    if (NULL == FileName_p) {
        A_(printf("os_free_fs.c (%d) Invalid input parameter!\n", __LINE__);)
        err_os_free = E_INVALID_INPUT_PARAMETERS;
        return FS_FATAL_ERROR;
    }

    (void)WideCharToUTF8((uint8 *)FileName_p, -1, Temp_Path, OS_FREE_FS_FILENAME_MAX_LENGTH);

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_remove_file(Temp_Path);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Renames file or directory
 */
int OS_Free_Rename(const char *const OldFileName_p, const char *const NewFileName_p)
{
    int ErrorCode;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_rename(OldFileName_p, NewFileName_p);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Renames file or directory
 */
int OS_Free_w_Rename(const wchar_t *const OldFileName_p, const wchar_t *const NewFileName_p)
{
    char Source[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];
    char Destination[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];
    int ErrorCode;

    (void)WideCharToUTF8((uint8 *)OldFileName_p, -1, Source, OS_FREE_FS_FILENAME_MAX_LENGTH);
    (void)WideCharToUTF8((uint8 *)NewFileName_p, -1, Destination, OS_FREE_FS_FILENAME_MAX_LENGTH);

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_rename(Source, Destination);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Change access mode. Problem with amode
 */
int OS_Free_Chmod(const char *Path_p, mode_t Amode)
{
    int ErrorCode = FS_FATAL_ERROR;

    if (NULL == Path_p) {
        A_(printf("os_free_fs.c (%d) Invalid input parameter!\n", __LINE__);)
        err_os_free = E_INVALID_INPUT_PARAMETERS;
        return FS_FATAL_ERROR;
    }

    C_(printf("chmod(%s)\n", Path_p);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_chmod(Path_p, Amode);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Change access mode. Problem with amode
 */
int OS_Free_w_Chmod(const wchar_t *Path_p, mode_t Amode)
{
    int ErrorCode = FS_FATAL_ERROR;
    char FileName[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];

    if (NULL == Path_p) {
        A_(printf("os_free_fs.c (%d) Invalid input parameter!\n", __LINE__);)
        err_os_free = E_INVALID_INPUT_PARAMETERS;
        return FS_FATAL_ERROR;
    }

    (void)WideCharToUTF8((uint8 *)Path_p, -1, FileName, OS_FREE_FS_FILENAME_MAX_LENGTH);
    C_(printf("chmod(%s)\n", FileName);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_chmod(FileName, Amode);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Makes directory
 */
int OS_Free_Mkdir(const char *Path_p, mode_t Amode)
{
    int ErrorCode = FS_FATAL_ERROR;
    mode_t AccessRightsConstant = 0111;

#ifdef CNH1606644_FS_USES_JEFF_VFAT
    AccessRightsConstant = 0777;
#endif

    C_(printf("mkdir(%s)\n", Path_p);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_mkdir(Path_p, AccessRightsConstant);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Makes directory
 */
int OS_Free_w_Mkdir(const wchar_t *Path_p, mode_t Amode)
{
    char Temp_Path[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];
    int ErrorCode = FS_FATAL_ERROR;

    (void)WideCharToUTF8((uint8 *)Path_p, -1, Temp_Path, OS_FREE_FS_FILENAME_MAX_LENGTH + 1);
    C_(printf("mkdir(%s)\n", Temp_Path);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_mkdir(Temp_Path, Amode);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 *  Opens directory
 */
DIR *OS_Free_Opendir(const char *dirname)
{
    DIR *Result = NULL;

    C_(printf("opendir(%s)\n", dirname);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return NULL;
    }

    Result = _fs_opendir(dirname);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return NULL;
    }

    ConvertErrnoToLoaderErr();
    return Result;
}

/*
 *  Opens directory
 */
DIR *OS_Free_w_Opendir(const wchar_t *dirname)
{
    char Temp_Path[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];
    DIR *Result = NULL;

    (void)WideCharToUTF8((uint8 *)dirname, -1, Temp_Path, OS_FREE_FS_FILENAME_MAX_LENGTH + 1);
    C_(printf("opendir(%s)\n", Temp_Path);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return NULL;
    }

    Result = _fs_opendir(Temp_Path);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return NULL;
    }

    ConvertErrnoToLoaderErr();
    return Result;
}

/*
 * Reads directory
 */
OS_Free_DIR_t *OS_Free_Readdir(DIR *dirp)
{
    LowLevel_Fs_DIR_t *Temp_DIR_p = NULL;
    OS_Free_DIR_t     *Result_DIR_p = NULL;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return NULL;
    }

    Temp_DIR_p  = _fs_readdir(dirp);

    if (NULL != Temp_DIR_p) {
        Result_DIR_p = (OS_Free_DIR_t *)malloc(sizeof(OS_Free_DIR_t));
        ASSERT(NULL != Result_DIR_p);
        memcpy(Result_DIR_p->d_name, Temp_DIR_p->d_name, Temp_DIR_p->d_namlen);
    } else {
        ConvertErrnoToLoaderErr();
    }

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        BUFFER_FREE(Result_DIR_p);

        return Result_DIR_p;
    }

    return Result_DIR_p;
}

/*
 * Closes directory
 */
int OS_Free_Closedir(DIR *dirp)
{
    int ErrorCode = FS_FATAL_ERROR;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_closedir(dirp);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Changes active directory. Not impl i YAFFS ?!?! XVSZOAN
 */
int OS_Free_Chdir(const char *Path_p)
{
    int ErrorCode = FS_FATAL_ERROR;

    C_(printf("chdir(%s)\n", Path_p);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_chdir(Path_p);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Changes active directory. Not impl i YAFFS ?!?! XVSZOAN
 */
int OS_Free_w_Chdir(const wchar_t *Path_p)
{
    char Temp_Path[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];
    int ErrorCode = FS_FATAL_ERROR;

    (void)WideCharToUTF8((uint8 *)Path_p, -1, Temp_Path, OS_FREE_FS_FILENAME_MAX_LENGTH);
    C_(printf("chdir(%s)\n", Temp_Path);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_chdir(Temp_Path);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Removes directory
 */
int OS_Free_Rmdir(const char *Path_p)
{
    int ErrorCode = FS_FATAL_ERROR;

    C_(printf("rmdir(%s)\n", Path_p);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_rmdir(Path_p);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Removes directory
 */
int OS_Free_w_Rmdir(const wchar_t *Path_p)
{
    char Temp_Path[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];
    int ErrorCode = FS_FATAL_ERROR;

    (void)WideCharToUTF8((uint8 *)Path_p, -1, Temp_Path, OS_FREE_FS_FILENAME_MAX_LENGTH);
    C_(printf("rmdir(%s)\n", Temp_Path);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_rmdir(Temp_Path);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

void OS_Free_Rewinddir(DIR *dirp)
{
    (void)Do_Register_Service(FS_SERVICE);

    _fs_rewinddir(dirp);

    (void)Do_UnRegister_Service(FS_SERVICE);
}

/*
 * Statistics of FS
 */
int OS_Free_Stat(const char *Path_p, OS_Free_File_Stat_t *Buf_p)
{
    int ErrorCode = FS_FATAL_ERROR;
    OS_Free_ObjectTypeMask_t ObjectMask;
    Lowlevel_Fs_Stat_t Fs_Stat;
    uint32 st_mode_temp = 0;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_stat(Path_p, &Fs_Stat);

    if (ErrorCode != FS_FATAL_ERROR) {

#ifdef CNH1606644_FS_USES_JEFF_VFAT
        Buf_p->st_size  = Fs_Stat.st_size;
        Buf_p->st_atime = Fs_Stat.st_atime;
        Buf_p->st_mtime = Fs_Stat.st_mtime;
        Buf_p->st_ctime = Fs_Stat.st_ctime;
        Buf_p->st_mode  = Fs_Stat.st_mode;
        st_mode_temp = Buf_p->st_mode;
        st_mode_temp &= JEFF_S_IFMT;

        switch (st_mode_temp) {
        case S_IFDIR:
            ObjectMask = OS_FREE_S_IFDIR;
            Buf_p->st_size  = 0;
            Buf_p->st_mode &= ~(S_IFDIR);
            Buf_p->st_mode |= ObjectMask;
            break;

        case S_IFCHR:
            ObjectMask = OS_FREE_S_IFCHR;
            Buf_p->st_mode &= ~(S_IFCHR);
            Buf_p->st_mode |= ObjectMask;
            break;

        case S_IFBLK:
            ObjectMask = OS_FREE_S_IFBLK;
            Buf_p->st_mode &= ~(S_IFBLK);
            Buf_p->st_mode |= ObjectMask;
            break;

        case S_IFREG:
            ObjectMask = OS_FREE_S_IFREG;
            Buf_p->st_mode &= ~(S_IFREG);
            Buf_p->st_mode |= ObjectMask;
            break;

        case S_IFFIFO:
            ObjectMask = OS_FREE_S_IFIFO;
            Buf_p->st_mode &= ~(S_IFFIFO);
            Buf_p->st_mode |= ObjectMask;
            break;

        case S_IFLNK:
            ObjectMask = OS_FREE_S_IFLNK;
            Buf_p->st_mode &= ~(S_IFLNK);
            Buf_p->st_mode |= ObjectMask;
            break;

        case S_IFSOCK:
            ObjectMask = OS_FREE_S_IFSOCK;
            Buf_p->st_mode &= ~(S_IFSOCK);
            Buf_p->st_mode |= ObjectMask;
            break;

        default:
            A_(printf("os_free_fs.c(%d): Stat Error. Unknown file type! temp(0x%08X) buf(0x%08X)!\n", __LINE__, st_mode_temp, Buf_p->st_mode);)
            ErrorCode = FS_FATAL_ERROR;
            err_os_free = E_ITEM_STAT;
            goto ErrorExit;
        }

#endif

        C_(printf("stat(%s) Size = %d B\n", Path_p, Buf_p->st_size);)
    }

    ConvertErrnoToLoaderErr();

ErrorExit:

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    return ErrorCode;
}

/*
 * Statistics of FS
 */
int OS_Free_w_Stat(const wchar_t *Path_p, OS_Free_File_Stat_t *Buf_p)
{
    char Temp_Path[OS_FREE_FS_FILENAME_MAX_LENGTH + 1];
    int ErrorCode = FS_FATAL_ERROR;

    (void)WideCharToUTF8((uint8 *)Path_p, -1, Temp_Path, OS_FREE_FS_FILENAME_MAX_LENGTH);

    ErrorCode = OS_Free_Stat(Temp_Path, Buf_p);

    return ErrorCode;
}

/*
 * Reads statistics of the specified file.
 * The structure stat differs for different FS.
 * We will make our structure only with params that we need.
 * XVSZOAN: Not done yet!
 */
int OS_Free_Fstat(int fd, OS_Free_File_Stat_t *Stat_p)
{
    int ErrorCode = FS_FATAL_ERROR;

    Lowlevel_Fs_Stat_t Fs_Stat;

    C_(printf("fstat( fd=%d)\n", fd);)

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_fstat(fd, &Fs_Stat);

#ifdef CNH1606644_FS_USES_JEFF_VFAT
    Stat_p->st_mode  = Fs_Stat.st_mode;
    Stat_p->st_size  = Fs_Stat.st_size;
    Stat_p->st_atime = Fs_Stat.st_atime;
    Stat_p->st_mtime = Fs_Stat.st_mtime;
    Stat_p->st_ctime = Fs_Stat.st_ctime;
#endif

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 *  Synchronizes the FS
 */
int OS_Free_Sync(void)
{
    int ErrorCode = FS_FATAL_ERROR;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_sync();

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * XVSZOAN: Problem with YAFFS
 */
int OS_Free_Fsync(int fd)
{
    int ErrorCode = FS_FATAL_ERROR;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ErrorCode = _fs_fsync(fd);

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_FATAL_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return ErrorCode;
}

/*
 * Seeks in specified file
 */
uint64 OS_Free_Lseek(int fd, uint64 offset, int whence)
{
    uint64 Result = FS_ERROR;
    off_t newpos;

    if (E_SUCCESS != Do_Register_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

    newpos = _fs_lseek(fd, (off_t)offset, whence);

    if ((off_t)(MAX_UINT32) != newpos) { // checking if result is ERROR
        Result = (uint64)newpos;
    }

    if (E_SUCCESS != Do_UnRegister_Service(FS_SERVICE)) {
        return FS_ERROR;
    }

    ConvertErrnoToLoaderErr();
    return Result;
}

mode_t OS_Free_Umask(mode_t mask)
{
    mode_t oldmask;

    (void)Do_Register_Service(FS_SERVICE);

    oldmask = _fs_umask(mask);

    (void)Do_UnRegister_Service(FS_SERVICE);

    return oldmask;
}

int UTF8ToWideChar(const char *UTF8Str, int UTF8StrLen, char *WideCharStr, int WideStrLen)
{
    /*
     Char. number range  |        UTF-8 octet sequence
        (hexadecimal)    |              (binary)
     --------------------+---------------------------------------------
     0000 0000-0000 007F | 0xxxxxxx
     0000 0080-0000 07FF | 110xxxxx 10xxxxxx
     0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
     0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     */

    int Len = 0;
    int Cnt = 0;
    int Out = 0;
    unsigned short Val;

    if ((UTF8Str[0] == (char)0xEF) && (UTF8Str[1] == (char)0xBB) && (UTF8Str[2] == (char)0xBF)) {
        Cnt = 3;
    }

    while (Len < WideStrLen) {
        if ((UTF8Str[Cnt] & 0xF0) == 0xE0) {
            /* Subsequent bytes must start with 10 */
            if (((UTF8Str[Cnt + 1] & 0xC0) != 0x80) &&
                    ((UTF8Str[Cnt + 2] & 0xC0) != 0x80)) {
                break;
            }

            Val = (UTF8Str[Cnt++] & 0x0F) << 12;
            Val |= (UTF8Str[Cnt++] & 0x3F) << 6;
            Val |= (UTF8Str[Cnt++] & 0x3F);

        } else if ((UTF8Str[Cnt] & 0xE0) == 0xC0) {
            /* Subsequent byte must start with 10 */
            if ((UTF8Str[Cnt + 1] & 0xC0) != 0x80) {
                break;
            }

            Val = (UTF8Str[Cnt++] & 0x1F) << 6;
            Val |= (UTF8Str[Cnt++] & 0x3F);
        } else if ((UTF8Str[Cnt] & 0x80) == 0x00) {
            Val = UTF8Str[Cnt++] & 0x7F;
        } else {
            break;
        }

        /*
         * 1. wide char is big-endian
         * 2. UTF-8 is big-endian
         * 3. processor is running little-endian
         * Hence, mempcy() is used insted of simple assignment.
         * Using 16-bit integer assignment,
         * will break the functionality.
         */
        memcpy((char *)WideCharStr + 2 * Out, &Val, sizeof(Val));

        if (Val == 0) {
            break;
        }

        if (UTF8StrLen != -1 && Cnt >= UTF8StrLen) {
            break;
        }

        Out++;

        if ((Out * 2) >= WideStrLen) {
            break;
        }
    }

    return Out;
}

int WideCharToUTF8(const uint8 *WideCharStr, int WideStrLen, char *UTF8Str, int UTF8StrLen)
{
    /*
     Char. number range  |        UTF-8 octet sequence
        (hexadecimal)    |              (binary)
     --------------------+---------------------------------------------
     0000 0000-0000 007F | 0xxxxxxx
     0000 0080-0000 07FF | 110xxxxx 10xxxxxx
     0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
     0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     */

    int Len = 0;
    int Cnt = 0;
    int Ret = 0;

    if ((WideCharStr == NULL) || (UTF8Str == NULL)) {
        return 0;
    }

    while (Len < UTF8StrLen) {
        unsigned short Val;

        /*
         * 1. wide char is big-endian
         * 2. UTF-8 is big-endian
         * 3. processor is running little-endian
         * Hence, mempcy() is used insted of simple assignment.
         * Using 16-bit integer assignment,
         * will break the functionality.
         */
        memcpy(&Val, WideCharStr + Len, sizeof(unsigned short));

        if (Val < 0x80) {

            if (Cnt < UTF8StrLen) {
                UTF8Str[Cnt++] = Val;
            } else {
                break;
            }

        } else if (Val <= 0x800) {

            if ((Cnt + 2) > UTF8StrLen) {
                break;
            }

            UTF8Str[Cnt++] = (char)(0xC0 | ((Val & 0x7C0) >> 6));
            UTF8Str[Cnt++] = (char)(0x80 | (Val & 0x3F));
        } else if (Val < 0xFFFE) {

            if ((Cnt + 3) > UTF8StrLen) {
                break;
            }

            UTF8Str[Cnt++] = (char)(0xE0 | ((Val & 0xF000) >> 12));
            UTF8Str[Cnt++] = (char)(0x80 | ((Val & 0xFC0) >> 6));
            UTF8Str[Cnt++] = (char)(0x80 | ((Val & 0x3F)));
        } else {
            break;
        }

        if (Val == 0) {
            break;
        }

        Len += 2;
        Ret++;

        if (WideStrLen != -1 && Len >= WideStrLen) {
            break;
        }
    }

    // Make sure the string is nullterminated
    if (UTF8StrLen > 0) {
        UTF8Str[Cnt] = 0;
    }

    return Ret;
}

char *Get_FS_Type(char *VolumeNameIn_p)
{
    char *FS_Type_p = NULL;
    uint32 StringCount = 0;
    uint32 VolumeLen = 0;
    uint32 SavedVolumeLen = 0;
    uint32 i = 0;

    if (NULL == VolumeNameIn_p) {
        A_(printf("os_free_fs.c (%d): ** ERR: Invalid input parameter!\n", __LINE__);)
        goto Exit;
    }

    VolumeLen = strlen(VolumeNameIn_p);

    if ('/' == VolumeNameIn_p[VolumeLen - 1]) {
        VolumeLen--;
    }

    while (TRUE == Saved_Volume_Params_p[i]->Params_Saved) {
        SavedVolumeLen = strlen(Saved_Volume_Params_p[i]->Name);

        if ('/' == Saved_Volume_Params_p[i]->Name[SavedVolumeLen - 1]) {
            SavedVolumeLen--;
        }

        if (SavedVolumeLen == VolumeLen) {
            if (0 == strncmp(Saved_Volume_Params_p[i]->Name, VolumeNameIn_p, SavedVolumeLen)) {
                if (OS_Free_JEFF == Saved_Volume_Params_p[i]->FS_type) {
                    StringCount = strlen("jeff");
                    FS_Type_p = (char *)malloc(sizeof(uint32) + StringCount);
                    ASSERT(NULL != FS_Type_p);
                    memcpy(FS_Type_p, &StringCount, sizeof(uint32));
                    memcpy(FS_Type_p + sizeof(uint32), "jeff", StringCount);
                } else if (OS_Free_VFAT == Saved_Volume_Params_p[i]->FS_type) {
                    StringCount = strlen("vfat");
                    FS_Type_p = (char *)malloc(sizeof(uint32) + StringCount);
                    ASSERT(NULL != FS_Type_p);
                    memcpy(FS_Type_p, &StringCount, sizeof(uint32));
                    memcpy(FS_Type_p + sizeof(uint32), "vfat", StringCount);
                } else {
                    B_(printf("os_free_fs.c (%d): **Unknown FS type.**\n", __LINE__);)
                }

                goto Exit;
            }
        }

        i++;
    }

Exit:
    return FS_Type_p;
}

boolean CompareSourceAndDestVolumes(const char *Source_p, const char *Destination_p)
{
    uint32 VolumeNameLen = 0;
    uint32 i = 0;

    if ((NULL == Source_p) || (NULL == Destination_p)) {
        return FALSE;
    }

    for (i = 0; i < nrMountedVolumes; i++) {
        VolumeNameLen = strlen(MountedVolumes_p[i]->d_name);

        if (0 == strncmp(MountedVolumes_p[i]->d_name, Source_p, VolumeNameLen)) {
            if (0 == strncmp(MountedVolumes_p[i]->d_name, Destination_p, VolumeNameLen)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

uint32 GetNrOfMountedVolumes(void)
{
    uint32 VolumesFound = 0;

    (void)Do_Register_Service(FS_SERVICE);

    while (TRUE == Saved_Volume_Params_p[VolumesFound]->Params_Saved) {
        VolumesFound++;
    }

    (void)Do_UnRegister_Service(FS_SERVICE);
    return VolumesFound;
}

ErrorCode_e GetVolumeInfo(Volume_Params_t *VolumeInfo_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    static uint32 index = 0;

    VERIFY(NULL != VolumeInfo_p, E_INVALID_INPUT_PARAMETER);
    ReturnValue = Do_Register_Service(FS_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    memcpy(VolumeInfo_p, Saved_Volume_Params_p[index], sizeof(Volume_Params_t));

    if (FALSE == Saved_Volume_Params_p[index + 1]->Params_Saved) {
        index = 0;
    } else {
        index++;
    }

    ReturnValue = Do_UnRegister_Service(FS_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, E_UNREGISTER_FS_SERVICE_FAILED);

    ReturnValue = E_SUCCESS;
ErrorExit:

    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static void ConvertErrnoToLoaderErr(void)
{
#ifdef CNH1606644_FS_USES_JEFF_VFAT

    switch (errno) {
    case  0:
        err_os_free = E_SUCCESS;
        break; /* No error. */
    case  5:
        err_os_free = E_FS_IO;
        break; /* Input/output error. */
    case  7:
        err_os_free = E_FS_ARGUMENT_LIST_2BIG;
        break; /* Argument list too long. */
    case  9:
        err_os_free = E_FS_BAD_FILE_DESC;
        break; /* Bad file descriptor. */
    case 13:
        err_os_free = E_FS_ACCESS;
        break; /* Permission denied. */
    case 14:
        err_os_free = E_FS_BAD_ADDRESS;
        break; /* Bad address. */
    case 17:
        err_os_free = E_FS_FILE_EXIST;
        break; /* File exists. */
    case 20:
        err_os_free = E_FS_NOT_DIR;
        break; /* Not a directory. */
    case 21:
        err_os_free = E_FS_IS_DIR;
        break; /* Is a directory. */
    case 27:
        err_os_free = E_FS_FILE_TOO_LARGE;
        break; /* File too large. */
    case 28:
        err_os_free = E_INSUFFICENT_SPACE;
        break; /* Insuficient space. */
    case 30:
        err_os_free = E_FS_READ_ONLY_FS;
        break; /* Read-only file system. */
    case 45:
        err_os_free = E_FS_OPER_NOT_SUPP;
        break; /* Operation not supported. */
    case 63:
        err_os_free = E_FS_NAME_TOO_LONG;
        break; /* File name too long. */
    case 84:
        err_os_free = E_FS_OPERATION_CANCELED;
        break; /* Operation canceled. */
    case 85:
        err_os_free = E_FS_FAIL;
        break; /* Cannot start operation. */
    case 91:
        err_os_free = E_FS_INTERNAL;
        break; /* Internal error. */
    case 94:
        err_os_free = E_FS_NOT_MOUNTED;
        break; /* Not mounted. */
    case 96:
        err_os_free = E_FS_NOT_PERMITED;
        break; /* Operation not permitted. */
    case 97:
        err_os_free = E_FS_NO_SUCH_FILE_OR_DIR;
        break; /* No such file or directory. */

    default:
        err_os_free = E_FS_UNKNOWN_ERROR;
        break;
    }

#endif //CNH1606644_FS_USES_JEFF_VFAT
}

/** @} */
/** @} */
