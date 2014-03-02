/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/*
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup path_utilities
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "r_path_utilities.h"
#include "r_serialization.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * Declarations of static functions
 ******************************************************************************/
static boolean FindColon(const char *GeneralPath_p, const uint32 PathLength, uint32 *CharsToColon_p);

/*******************************************************************************
 * Definitions of API functions
 ******************************************************************************/
boolean SearchBulkPath(const char *Path_p)
{
    const char SupportedDevices[] = "/COMM";
    const char SupportedDevicesL[] = "/comm";
    const uint32 Length = sizeof(SupportedDevices) - 1;

    if (NULL == Path_p) {
        goto Error_Exit;
    }

    if ((0 == strncmp(Path_p + 4, SupportedDevices, Length)) || (0 == strncmp(Path_p + 4, SupportedDevicesL, Length))) {
        return TRUE;
    }

Error_Exit:
    return FALSE;
}

/*
 * @brief Check if path is in one of secured folders
 *
 * @param [in]   GeneralPath_p  File system path.
 * @return TRUE  If folder is secured.
 * @return FALSE If folder is not secured.
 */
boolean CheckIfSecuredPath(const char *PathName_p)
{
    return FALSE;
}

/*
 * @brief Check if incoming file is properly signed
 * ex. /bin/some_file.bin from /flash0/cabs0:/bin/some_file.bin
 *
 * @param [in]   GeneralPath_p  File system path.
 * @return TRUE  If file is signed properly.
 * @return FALSE If file is not signed properly.
 */
boolean VerifyIncomingFile(const char *PathName_p)
{
    return TRUE;
}

/*
 * @brief Extracting path from general path
 *
 * @param [in] GeneralPath_p  File system path.
 * @return     Pointer to nul terminated copy of path string.
 * @retval     on error NULL pointer.
 */
char *ExtractPath(const char *GeneralPath_p)
{
    char *Path_p = NULL;
    uint32 PathLength = 0;
    uint32 InputPathLength = 0;
    uint32 CharsToColon = 0;

    InputPathLength = get_uint32_le((void *)&GeneralPath_p);

    if (0 == InputPathLength) {
        goto ErrorExit;
    }

    /*
     * Find the beginning of the path that needs to be extracted
     * from the general path given as input parameter.
     *
     * Example of a path extracted from a general path:
     * /bin/some_file.bin from /flash0/cabs0:/bin/some_file.bin
     *
     * The general path contains the flash and the cabs unit on which
     * the volume is mounted
     *
     * */
    if (FindColon(GeneralPath_p, InputPathLength, &CharsToColon)) {
        PathLength = InputPathLength - CharsToColon;

        if (PathLength < FILENAME_MAX) {
            Path_p = (char *)malloc(PathLength + sizeof(char));
            ASSERT(NULL != Path_p);
            memcpy(Path_p, GeneralPath_p + CharsToColon, PathLength);
            Path_p[PathLength] = '\0';
        }

        goto ErrorExit;
    } else {
        B_(printf("file_management_applications_functions.c(%d) ** Path not found!** \n", __LINE__);)
        goto ErrorExit;
    }

ErrorExit:
    return Path_p;
}

/*
 * @brief Extracting path from general path
 *
 * @param [in] GeneralPath_p  File system path.
 * @return     Pointer to nul terminated copy of path string.
 * @retval     on error NULL pointer.
 */
char *ExtractDevice(const char *GeneralPath_p)
{
    char *Path_p = NULL;
    uint32 InputPathLength = 0;
    uint32 CharsToColon = 0;

    InputPathLength = get_uint32_le((void *)&GeneralPath_p);

    if (0 == InputPathLength) {
        goto ErrorExit;
    }

    if (!FindColon(GeneralPath_p, InputPathLength, &CharsToColon)) {
        CharsToColon = InputPathLength;
    } else {
        CharsToColon--;
    }

    Path_p = (char *)malloc(CharsToColon + sizeof(char));
    ASSERT(NULL != Path_p);
    memcpy(Path_p, GeneralPath_p, CharsToColon);
    Path_p[CharsToColon] = '\0';

ErrorExit:
    return Path_p;
}

char *GetLastWord(void *Device_p)
{
#define BACK_SLASH_ERROR 0xffff
    uint32 i = 0;
    //  uint32 BackSlashPlace = BACK_SLASH_ERROR;
    uint32 BackSlashPlace = 0;
    uint32 Length = 0;
    char *LastWord_p = NULL;
    char *Device = Device_p;

    /* finds last back slash */
    while (Device[i] != 0) {
        if (Device[i] == '/') {
            BackSlashPlace = i;
        }

        i++;
    }

    if (i > 0) {
        if ((i - BackSlashPlace) > 1) {
            BackSlashPlace = i;
        }

        i = BackSlashPlace - 1;

        while (i != 0) {
            if (Device[i] == '/') {
                break;
            }

            i--;
        }

        if (Device[i] == '/') {
            Length = BackSlashPlace - i;
            LastWord_p = (char *)malloc(Length);
            ASSERT(NULL != LastWord_p);
            memcpy(LastWord_p, Device + i + 1, Length);
        } else {
            Length = BackSlashPlace + 1;
            LastWord_p = (char *)malloc(Length);
            ASSERT(NULL != LastWord_p);
            memcpy(LastWord_p, Device, Length);
        }

        LastWord_p[Length - 1] = 0;
    }

    return LastWord_p;
}

char *GetLastWordn(char *Device_p, uint32 DeviceLength, uint32 *LastWordLength_p)
{
    char *Start_p = Device_p;
    char *End_p = Device_p + DeviceLength;
    char *Temp_p = End_p;

    *LastWordLength_p = DeviceLength;

    if ((Device_p != NULL) && (DeviceLength != 0)) {
        while (Temp_p >= Device_p) {
            if ('/' == *(Temp_p)) {
                Start_p = Temp_p + 1;
                *LastWordLength_p = End_p - Start_p;
                break;
            }

            Temp_p--;
        }
    }

    return Start_p;
}

char *GetFileName(const char *Path_p)
{
    uint32 PathLength = strlen(Path_p);
    uint32 FileNameLength = 0;
    char *FileName_p = NULL;

    while ((Path_p[PathLength] != '/') && PathLength) {
        PathLength--;
    }

    if (PathLength) {
        FileNameLength = strlen(Path_p) - PathLength;
        FileName_p = (char *)malloc((FileNameLength) * sizeof(char));

        if (FileName_p) {
            strncpy(FileName_p, Path_p + PathLength + 1, FileNameLength - 1);
            FileName_p[FileNameLength - 1] = '\0';
        }
    }

    return FileName_p;
}

uint32 GetDeviceNumber(void *Device_p)
{
    uint32 i = 0;
    uint32 DeviceNumber = 0;
    char *LastWord_p = NULL;

    LastWord_p = GetLastWord(Device_p);

    if (NULL != LastWord_p) {
        //lint --e(48)
        //lint --e(409)
        while ((0 != *(LastWord_p + i)) && !isdigit(*(LastWord_p + i))) {
            i++;
        }

        if ((i > 0) && (0 != *(LastWord_p + i))) {
            DeviceNumber = (uint32)atoi(LastWord_p + i);
        }

        BUFFER_FREE(LastWord_p);
    }

    return DeviceNumber;
}

/*******************************************************************************
 * Definitions of static functions
 ******************************************************************************/
static boolean FindColon(const char *GeneralPath_p, const uint32 PathLength, uint32 *CharsToColon_p)
{
    uint32 CharsToColon = 0;
    boolean Result = FALSE;

    do {
        if (':' == GeneralPath_p[CharsToColon]) {
            CharsToColon++;
            Result = TRUE;
            break;
        }

        CharsToColon++;
    } while ((CharsToColon < FILENAME_MAX) && (CharsToColon < PathLength));

    *CharsToColon_p = CharsToColon;
    return Result;
}

/* @} */
/* @} */
