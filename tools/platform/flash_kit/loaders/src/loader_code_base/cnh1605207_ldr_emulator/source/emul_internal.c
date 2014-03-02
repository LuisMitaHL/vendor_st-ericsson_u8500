/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/

/**
 * \addtogroup ldr_emulator
 *  @{
 *      \addtogroup Internal
 *      \brief Internal functions for R15 emulator
 *      @{
 */

#include <stdlib.h>
#include <string.h>

#include "c_system_v2.h"
#include "error_codes.h"
#include "t_basicdefinitions.h"
#include "r_debug.h"
#define EMP_Printf printf   /**< @todo remove in final version */
#include "t_emulator.h"
#include "r_emul_nand.h"
#include "r_emul_nand_util.h"
#include "r_emul_internal.h"
#include "r_emul_util.h"


ErrorCode_e EMUL_ConfigInit()
{
    NAND_Memory_Emul.ManufacturerID = DEFAULT_MANUFACTURER_ID;
    NAND_Memory_Emul.DeviceID = DEFAULT_DEVICE_ID;
    return E_SUCCESS;
}

ErrorCode_e EMUL_ConfigNand(Line_t *FileLine_p)
{
    EMUL_ConfigNandState_t State;
    uint32 i;
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    sint8 *Buffer_p = NULL;
    uint32 BufferSize = 0;
    uint32 Address = 0;

    Buffer_p = (sint8 *)malloc(MAX_SIZE_OF_A01_BUFFER);
    BufferSize = MAX_SIZE_OF_A01_BUFFER;

    if (NULL == Buffer_p) {
        A_(printf("emul_internal.c (%d): Error xmalloc failed to allocate memory\n", __LINE__);)
        Result = E_ALLOCATE_FAILED;
        return Result;
    }

    if (NULL == FileLine_p) {
        A_(printf("emul_internal.c (%d): Error FileLine_p is NULL\n", __LINE__);)
        Result = E_INVALID_INPUT_PARAMETERS;
        goto error_exit;
    }

    if (0 != strcmp(FileLine_p->Arg_p[0], "-nand")) {
        A_(printf("emul_internal.c (%d): Error wrong function called for parametar %s\n", __LINE__, FileLine_p->Arg_p[0]);)
        Result = E_INVALID_INPUT_PARAMETERS;
        goto error_exit;
    }

    i = 1;

    while (i < FileLine_p->ArgNr) {
        if (strcmp(FileLine_p->Arg_p[i], "-s") == 0) {
            State = SMALL_PAGE_FILE;
        } else if (strcmp(FileLine_p->Arg_p[i], "-l") == 0) {
            State = LARGE_PAGE_FILE;
        } else if (strcmp(FileLine_p->Arg_p[i], "-man_id") == 0) {
            State = MAN_ID_FILE;
        } else if (strcmp(FileLine_p->Arg_p[i], "-dev_id") == 0) {
            State = DEV_ID_FILE;
        } else {
            A_(printf("emul_internal.c (%d): Error unrecognized argument in file %s, line %d, argument no %d, argument %s\n", __LINE__, FileLine_p->FileName, FileLine_p->LineNr, i + 1, FileLine_p->Arg_p[i]);)
            Result = E_INVALID_INPUT_PARAMETERS;
            goto error_exit;
        }

        switch (State) {
        case SMALL_PAGE_FILE: {
            C_(printf("emul_internal.c (%d): Small page file\n", __LINE__);)
            i++;

            if (NULL == FileLine_p->Arg_p[i]) {
                A_(printf("emul_internal.c (%d): Error file name expected in file %s, line %d\n", __LINE__, FileLine_p->FileName, FileLine_p->LineNr);)
                Result = E_INVALID_INPUT_PARAMETERS;
                goto error_exit;
            }

            C_(printf("emul_internal.c (%d): file name %s\n", __LINE__, FileLine_p->arg[i]);)

            EMUL_Util_A01ToBuffer(FileLine_p->Arg_p[i], Buffer_p, &BufferSize, &Address);
            C_(printf("emul_internal.c (%d): Starting address is 0x%08x\n", __LINE__, Address);)
            C_(printf("emul_internal.c (%d): Size of flash image is %d\n", __LINE__, BufferSize);)

            //        NANDHW_Memory_Init();
            EMUL_Nand_Util_SmallPageImage(Buffer_p, BufferSize);
        }
        break;

        case LARGE_PAGE_FILE: {
            C_(printf("emul_internal.c (%d): Large page file\n", __LINE__);)
            i++;

            if (NULL == FileLine_p->Arg_p[i]) {
                A_(printf("emul_internal.c (%d): Error file name expected in file %s, line %d\n", __LINE__, FileLine_p->FileName, FileLine_p->LineNr);)
                Result = E_INVALID_INPUT_PARAMETERS;
                goto error_exit;
            }

            C_(printf("emul_internal.c (%d): file name %s\n", __LINE__, FileLine_p->arg[i]);)

            EMUL_Util_A01ToBuffer(FileLine_p->Arg_p[i], Buffer_p, &BufferSize, &Address);
            C_(printf("emul_internal.c (%d): Starting address is 0x%08x\n", __LINE__, Address);)
            C_(printf("emul_internal.c (%d): Size of flash image is %d\n", __LINE__, BufferSize);)

            //        NANDHW_Memory_Init();
            EMUL_Nand_Util_LargePageImage(Buffer_p, BufferSize);
        }
        break;

        case MAN_ID_FILE: {
            i++;

            if (NULL == FileLine_p->Arg_p[i]) {
                A_(printf("emul_internal.c (%d): Error manufacturer ID expected in file %s, line %d, argument no %d, argument %s\n", __LINE__, FileLine_p->FileName, FileLine_p->LineNr, i + 1, FileLine_p->Arg_p[i]);)
                Result = E_INVALID_INPUT_PARAMETERS;
                goto error_exit;
            }

            Result = EMUL_Util_ArgToManID(FileLine_p->Arg_p[i], &NAND_Memory_Emul.ManufacturerID);

            if (Result != E_SUCCESS) {
                A_(printf("emul_internal.c (%d): Error argument: %s is not a valid manufacturer ID\n", __LINE__, FileLine_p->Arg_p[i]);)
                goto error_exit;
            }

            C_(printf("emul_internal.c (%d): Manufacturer_ID = %d\n", __LINE__, NAND_Memory_Emul.ManufacturerID);)
        }
        break;

        case DEV_ID_FILE: {
            i++;

            if (NULL == FileLine_p->Arg_p[i]) {
                A_(printf("emul_internal.c (%d): Error device ID expectedin file %s, line %d, argument no %d, argument %s\n", __LINE__, FileLine_p->FileName, FileLine_p->LineNr, i + 1, FileLine_p->Arg_p[i]);)
                Result = E_INVALID_INPUT_PARAMETERS;
                goto error_exit;
            }

            Result = EMUL_Util_ArgToDevID(FileLine_p->Arg_p[i], &NAND_Memory_Emul.DeviceID);

            if (Result != E_SUCCESS) {
                A_(printf("emul_internal.c (%d): Error argument: %s is not a valid device ID\n", __LINE__, FileLine_p->Arg_p[i]);)
                goto error_exit;
            }

            C_(printf("emul_internal.c (%d): Device_ID = %d\n", __LINE__, NAND_Memory_Emul.DeviceID);)
        }
        break;

        default: {
            A_(printf("emul_internal.c (%d): Error unrecognized state\n", __LINE__);)
            Result = E_UNRECOGNIZED_STATE;
            goto error_exit;
        }
        break;
        }

        i++;
    }

error_exit:
    free(Buffer_p);
    return E_GENERAL_FATAL_ERROR;
}
/**      @}*/
/**@}*/
