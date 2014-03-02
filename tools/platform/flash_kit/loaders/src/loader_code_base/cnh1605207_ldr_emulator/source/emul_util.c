/***********************************************************************
 * $Copyright Ericsson AB 2008 $
 **********************************************************************/

/**
 * \addtogroup ldr_emulator
 *  @{
 *      \addtogroup Utility
 *      \brief Utility functions for R15 emulator
 *      @{
*/
#include <string.h>

#include "c_system_v2.h"
#include "error_codes.h"
#include "t_basicdefinitions.h"
#include "r_debug.h"
#define EMP_Printf printf   /**< @todo remove in final version */
#include "c_pd_nand.h"
#include "t_emulator.h"
#include "r_emul_util.h"

ErrorCode_e EMUL_Util_ArgToManID(sint8 *Arg_p, uint8 *ManID_p)
{
    uint32 i;

    if (NULL == Arg_p) {
        A_(printf("emul_util.c (%d): **Error invalid input parameter*\n", __LINE__);)
        return E_INVALID_INPUT_PARAMETERS;
    }

    if (NULL == ManID_p) {
        A_(printf("emul_util.c (%d): **Error invalid input parameter*\n", __LINE__);)
        return E_INVALID_INPUT_PARAMETERS;
    }

    for (i = 0; i < EMUL_NUMBER_OF_MAN_ID; i++) {
        if (0 == strcmp(Arg_p, EMUL_MapManID[i].Name_p)) {
            *ManID_p = EMUL_MapManID[i].ID;
            return E_SUCCESS;
        }
    }

    A_(printf("emul_util.c (%d): Error the specified argument: %s is not a valid manufacturer ID\n", __LINE__, Arg_p);)
    *ManID_p = T_PD_NAND_CONF_MAN_ID_UNDEF;
    return E_UNKNOWN_MANUFACTURER_ID;
}

ErrorCode_e EMUL_Util_ArgToDevID(sint8 *Arg_p, uint8 *DevID_p)
{
    uint32 i;

    if (NULL == Arg_p) {
        A_(printf("emul_util.c (%d): **Error invalid input parameter*\n", __LINE__);)
        return E_INVALID_INPUT_PARAMETERS;
    }

    if (NULL == DevID_p) {
        A_(printf("emul_util.c (%d): **Error invalid input parameter*\n", __LINE__);)
        return E_INVALID_INPUT_PARAMETERS;
    }

    for (i = 0; i < EMUL_NUMBER_OF_DEV_ID; i++) {
        if (0 == strcmp(Arg_p, EMUL_MapDevID[i].Name_p)) {
            *DevID_p = (uint8)EMUL_MapDevID[i].ID;
            return E_SUCCESS;
        }
    }

    A_(printf("emul_util.c (%d): Error the specified argument: %s is not a valid device ID\n", __LINE__, Arg_p);)
    *DevID_p = T_PD_NAND_CONF_DEV_ID_128;
    return E_UNKNOWN_DEVICE_ID;
}

ErrorCode_e EMUL_Util_A01ToBuffer(sint8 *Name_p, sint8 *Buffer_p, uint32 *BufSize_p, uint32 *Address_p)
{
    uint32 i = 0;
    Line_t FileLine;
    sint8 Data;
    uint32 BytesCount = 0;
    uint32 Num = 0;
    uint32 Address;
    uint32 StartAdr;
    uint32 Size = 0;
    boolean First = TRUE;
    FILE *File_p = NULL;
    sint8 *Line_p = NULL;
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;

    if (NULL == Name_p) {
        A_(printf("emul_util.c (%d): Error NULL argument to function\n", __LINE__);)
        Result = E_INVALID_INPUT_PARAMETERS;
        goto error_exit;
    }

    if (NULL == Buffer_p) {
        A_(printf("emul_util.c (%d): Error NULL argument to function\n", __LINE__);)
        Result = E_INVALID_INPUT_PARAMETERS;
        goto error_exit;
    }

    if (NULL == BufSize_p) {
        A_(printf("emul_util.c (%d): Error NULL argument to function\n", __LINE__);)
        Result = E_INVALID_INPUT_PARAMETERS;
        goto error_exit;
    }

    if (NULL == Address_p) {
        A_(printf("emul_util.c (%d): Error NULL argument to function\n", __LINE__);)
        Result = E_INVALID_INPUT_PARAMETERS;
        goto error_exit;
    }

    if ((File_p = fopen(Name_p, "r")) == NULL) {
        A_(printf("emul_util.c (%d): Error can't open file: %s\n", __LINE__, Name_p);)
        Result = E_CANNOT_OPEN_FILE;
        goto error_exit;
    }

    strncpy(FileLine.FileName, Name_p, MAX_FILE_NAME);

    Size = *BufSize_p;
    *BufSize_p = 0;
    i = 1;

    while (NULL != fgets(FileLine.LineData, MAX_LINE_LENGTH, File_p)) {
        FileLine.LineNr = i;
        i++;

        Line_p = FileLine.LineData;



        if (('S' != Line_p[0]) || ('3' != Line_p[1])) {
            A_(printf("emul_util.c(%d): Error invalid A01 file format, line no: %d line = %s\n", __LINE__, FileLine.LineNr, FileLine.LineData);)
            Result = E_INVALID_A01_FORMAT;
            goto error_exit;
        } else {
            Line_p += 2;

            if (2 != sscanf(Line_p, "%2x%8x", &Num, &Address)) {
                printf("num = 0x%2x, address = 0x%8x\n", Num, Address);
                printf("line = %s\n", Line_p);
                A_(printf("emul_util.c(%d): Error reading from file %s line %d\n", __LINE__, FileLine.FileName, FileLine.LineNr);)
                Result = E_FAILED_READING_FROM_FILE;
                goto error_exit;
            }

            Line_p += 10;

            Size -= (Num - 5);

            if (0 >= Size) {
                A_(printf("emul_util.c (%d): Error buffer out of memory\n", __LINE__);)
                Result = E_A01_BUFFER_FULL;
                goto error_exit;
            }

            if (First) {
                StartAdr = Address;
                First = FALSE;

                for (BytesCount = 0; BytesCount < (Num - 5); BytesCount++) {
                    if (1 != sscanf(Line_p, "%2x", &Data)) {
                        A_(printf("emul_util.c(%d): Error reading from file %s line %d\n", __LINE__, FileLine.FileName, FileLine.LineNr);)
                        Result = E_FAILED_READING_FROM_FILE;
                        goto error_exit;
                    }

                    Line_p += 2;

                    Buffer_p[*BufSize_p] = Data;
                    (*BufSize_p)++;
                }

                sscanf(Line_p, "%2x", &Data);
                Line_p += 2;
            }

            else {
                for (BytesCount = 0; BytesCount < (Num - 5); BytesCount++) {
                    sscanf(Line_p, "%2x", &Data);
                    Line_p += 2;
                    Buffer_p[*BufSize_p] = Data;
                    (*BufSize_p)++;
                }

                sscanf(Line_p, "%2x", &Data);
                Line_p += 2;
            }
        }
    }

    *Address_p = StartAdr;

error_exit:

    if (NULL != File_p) {
        fclose(File_p);
    }

    return Result;
}
/**     @}*/
/**@}*/
