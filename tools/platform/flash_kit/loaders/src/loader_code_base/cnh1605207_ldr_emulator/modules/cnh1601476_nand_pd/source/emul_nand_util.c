/***********************************************************************
 * $Copyright Ericsson AB 2008 $
 **********************************************************************/

/**
 * \addtogroup ldr_emulator
 * @{
 *      \addtogroup NAND
 *      @{
 */

#include <stdio.h>
#include <string.h>

#include "error_codes.h"
#include "c_system_v2.h"
#include "r_debug.h"
#define EMP_Printf printf   /**< @todo remove in final version */
#include "r_emul_nand.h"
#include "t_emulator.h"
#include "r_emul_nand_util.h"

#define EMUL_NAND_MAIN_AREA  0
#define EMUL_NAND_SPARE_AREA 1

ErrorCode_e EMUL_Nand_Util_SmallPageImage(const sint8 *Buffer, uint32 BufferSize)
{
    uint8 State;
    uint8 *MainArea_p = NAND_Memory_Emul.MemBuffer;
    uint8 *SpareArea_p = NAND_Memory_Emul.RedundantAreaBuff;
    State = EMUL_NAND_MAIN_AREA;

    while (BufferSize > 0) {
        if (EMUL_NAND_MAIN_AREA == State) {
            if (512 > BufferSize) {
                A_(printf("emul_nand_util.c (%d): Error Buffer is not complete\n", __LINE__);)
                return E_GENERAL_FATAL_ERROR;
            }

            memcpy(MainArea_p, Buffer, 512);
            State = EMUL_NAND_SPARE_AREA;
            BufferSize -= 512;
            Buffer += 512;
            MainArea_p += 512;
        } else {
            if (16 > BufferSize) {
                A_(printf("emul_nand_util.c (%d): Error Buffer is not complete\n", __LINE__);)
                return E_GENERAL_FATAL_ERROR;
            }

            memcpy(SpareArea_p, Buffer, 16);
            State = EMUL_NAND_MAIN_AREA;
            BufferSize -= 16;
            Buffer += 16;
            SpareArea_p += 16;
        }

    }

    return E_SUCCESS;
}

ErrorCode_e EMUL_Nand_Util_LargePageImage(const sint8 *Buffer, uint32 BufferSize)
{
    uint8 State;
    uint8 *MainArea_p = NAND_Memory_Emul.MemBuffer;
    uint8 *SpareArea_p = NAND_Memory_Emul.RedundantAreaBuff;
    State = EMUL_NAND_MAIN_AREA;

    while (BufferSize > 0) {
        if (EMUL_NAND_MAIN_AREA == State) {
            if (512 > BufferSize) {
                A_(printf("emul_nand_util.c (%d): Error Buffer is not complete\n", __LINE__);)
                return E_GENERAL_FATAL_ERROR;
            }

            memcpy(MainArea_p, Buffer, 2048);
            State = EMUL_NAND_SPARE_AREA;
            BufferSize -= 2048;
            Buffer += 2048;
            MainArea_p += 2048;
        } else {
            if (16 > BufferSize) {
                A_(printf("emul_nand_util.c (%d): Error Buffer is not complete\n", __LINE__);)
                return E_GENERAL_FATAL_ERROR;
            }

            memcpy(SpareArea_p, Buffer, 64);
            State = EMUL_NAND_MAIN_AREA;
            BufferSize -= 64;
            Buffer += 64;
            SpareArea_p += 64;
        }
    }

    return E_SUCCESS;
}

/**     @}*/
/**@}*/
