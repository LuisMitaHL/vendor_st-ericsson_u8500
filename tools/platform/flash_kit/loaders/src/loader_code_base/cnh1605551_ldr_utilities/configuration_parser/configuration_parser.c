/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup configuration_parser
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "error_codes.h"
#include "r_configuration_parser.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static boolean IsValidDataInRow(const uint8 *VolumeRow_p, uint64 VolumeRowLen);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
ErrorCode_e GetVolumeConfiguration(ConfigurationParser_t *VolumeConfig_p,
                                   uint8  *VolumeInfo_p,
                                   uint64  VolumeInfoLen)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    static  uint64  index = 0;
    static  uint64  offset = 0;
    boolean ValidDataFound = FALSE;
    uint8  *s;
    char *TempBuffer_p = NULL;

    VERIFY(NULL != VolumeInfo_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(VolumeInfoLen > 0, E_INVALID_INPUT_PARAMETERS);

    memset(VolumeConfig_p, 0x00, sizeof(ConfigurationParser_t));

    /* Reset internal counters when and of volume configurations found.*/
    if (index == VolumeInfoLen) {
        index = 0;
        offset = 0;
        VolumeConfig_p->IsSet = FALSE;

        goto ErrorExit;
    }

    s = VolumeInfo_p;

    /* Read the content of the file system settings
     *
     * Format expected (example):
     *
     * #File system volume configuration file
     * #volume   format   parameters          device  device start, end
     * /boot       jeff   do_format=auto       cabs0   lo=0,hi=12799
     * /sys        jeff   do_format=auto       cabs0   lo=12800,hi=13955
     * /ifs        jeff   do_format=auto       cabs0   lo=13956,hi=14183
     * /bin        jeff   do_format=auto       cabs0   lo=14184,hi=14695
     * /afs        jeff   do_format=auto       cabs0   lo=14696,hi=15719
     */
    while (index < VolumeInfoLen) {
        /* Once a newline character is found or end of file is reached then we have the complete line */
        if ((s[index] == '\n') || (index == (VolumeInfoLen - 1))) {
            ValidDataFound = IsValidDataInRow(s + offset, index - offset);
            index ++;

            if (TRUE == ValidDataFound) {
                //If last line is not determinated with "\n"
                if ((s[index - 1] != '\n') && (index == VolumeInfoLen)) {
                    TempBuffer_p = (char *)malloc(index - offset + 1);
                    ASSERT(NULL != TempBuffer_p);

                    memcpy(TempBuffer_p, s + offset, index - offset);
                    TempBuffer_p[index - offset] = '\n';

                    s = (uint8 *)TempBuffer_p;
                    offset = 0;
                }

                /* Look for the volume string in the buffer */
                /* Collect the volume mounting parameters */
                sscanf((const char *)(s + offset), "%s  %s  do_format=%s  %s",
                       VolumeConfig_p->VolumeName, VolumeConfig_p->TypeName, VolumeConfig_p->FSMountingParam, VolumeConfig_p->DeviceName);
                sscanf((const char *)(s + offset), "%*s %*s %*s %*s lo=%d,hi=%d", &(VolumeConfig_p->LoBlkNo), &(VolumeConfig_p->HiBlkNo));

                VolumeConfig_p->IsSet = TRUE;
                offset = index;
                break;
            } else {
                offset = index;
            }
        } else {
            index++;
        }
    }

    if (index > VolumeInfoLen) {
        VolumeConfig_p->IsSet = FALSE;
        index = 0;
        offset = 0;
    }

ErrorExit:
    BUFFER_FREE(TempBuffer_p);
    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static boolean IsValidDataInRow(const uint8 *VolumeRow_p, uint64 VolumeRowLen)
{
    boolean ValidDataFound = FALSE;
    uint64 index = 0;

    for (index = 0; index < VolumeRowLen; index++) {
        if (VolumeRow_p[index] == ' ') {
            continue;
        } else if (VolumeRow_p[index] == '#') {
            return FALSE;
        } else {
            ValidDataFound = TRUE;
            return ValidDataFound;
        }
    }

    return ValidDataFound;
}

/* @} */
/* @} */
