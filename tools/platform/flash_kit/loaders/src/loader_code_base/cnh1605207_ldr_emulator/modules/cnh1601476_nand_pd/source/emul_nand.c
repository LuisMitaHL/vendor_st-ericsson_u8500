/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/
/**
 * \addtogroup ldr_emulator
 * @{
 *      \addtogroup NAND
 *      \brief Emulated NAND functions for R15 emulator
 *      @{
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "c_system_v2.h"
//#define CFG_CNH1601476_INSTANTIATE_DEVICE_CONFIGURATION_TABLE
#include "c_pd_nand.h"
#include "r_emul_nand.h"
#include "r_basicdefinitions.h"
#include "r_service_management.h"
#include "os_free_fs.h"
#include "r_debug.h"
#define EMP_Printf printf   /**< @todo remove in final version */

//This variable  are used to make the code more readable
uint32 DeviceSize     = 0;
uint32 PageSize       = 0;
uint32 EraseBlockSize = 0;
uint32 RedundantSize  = 0;

NAND_Memory_t NAND_Memory_Emul;
static uint8 EMUL_NandHW_Initialized = 0;
uint8 NANDHW_Memory_ManID(void)
{
    return NAND_Memory_Emul.ManufacturerID;
}

uint8 NANDHW_Memory_DevID(void)
{
    return NAND_Memory_Emul.DeviceID;
}

void Do_FlushPDNandEmulatorToFile(void)
{
#ifdef WIN32
    FILE *FileId;
    size_t WriteSize;

    if (DeviceSize != 0) {
        (void)OS_Free_Sync();
        FileId = fopen("c:/nand_memory.bin", "wb");
        assert(FileId != 0);
        WriteSize = fwrite(NAND_Memory_Emul.MemBuffer, 1, DeviceSize, FileId);
        assert(WriteSize == DeviceSize);
        WriteSize = fclose(FileId);
        assert(WriteSize == 0);

        FileId = fopen("c:/redundant_memory.bin", "wb");
        assert(FileId != 0);
        WriteSize = fwrite(NAND_Memory_Emul.RedundantAreaBuff, 1, ((DeviceSize / PageSize + 1) * RedundantSize), FileId);
        assert(WriteSize == ((DeviceSize / PageSize + 1)*RedundantSize));
        WriteSize = fclose(FileId);
        assert(WriteSize == 0);
    }

#endif
}

PD_NAND_Result_t NANDHW_Memory_Init(const PD_NAND_MemoryProperties_t *const MemProp_p)
{
    if (!EMUL_NandHW_Initialized) {

#ifdef WIN32
        FILE *FileId;
        size_t ReadSize;
        size_t WriteSize;
#endif

        PD_NAND_Result_t RetVal = T_PD_NAND_RESULT_INTERNAL_ERROR;
        //    PD_NAND_CONF_DeviceType_t Result = T_PD_NAND_CONF_DEVICE_TYPE_UNDEFINED;

        C_(printf("nand_memory.c (%d): NANDHW memory initialization!\n", __LINE__);)

        NAND_Memory_Emul.DeviceInfo.EraseBlockSize = MemProp_p->BlockSize;
        NAND_Memory_Emul.DeviceInfo.DeviceSize = MemProp_p->Size;
        NAND_Memory_Emul.DeviceInfo.PageSize = MemProp_p->PageSize;
        NAND_Memory_Emul.DeviceInfo.PageExtraSize = MemProp_p->PageExtraSize;
        NAND_Memory_Emul.DeviceInfo.BusWidth = MemProp_p->BusWidthInBits;

        DeviceSize = NAND_Memory_Emul.DeviceInfo.DeviceSize;
        EraseBlockSize = NAND_Memory_Emul.DeviceInfo.EraseBlockSize;
        PageSize = NAND_Memory_Emul.DeviceInfo.PageSize;
        RedundantSize = NAND_Memory_Emul.DeviceInfo.PageExtraSize;

        A_(printf("nand_memory.c(%d):\n Device size = %d;\n Erase block size = %d;\n Page size = %d;\n Spare size = %d\n",
                  __LINE__,
                  DeviceSize,
                  EraseBlockSize,
                  PageSize,
                  RedundantSize
                 );
          )

        C_(printf("nand_memory.c(%d): Trying to initialize memory for emulated flash!\n", __LINE__);)
        NAND_Memory_Emul.MemBuffer = (uint8 *)malloc(DeviceSize);

        if (NAND_Memory_Emul.MemBuffer == NULL) {
            A_(printf("nand_memory.c(%d):Error malloc failed to allocate memory!\n", __LINE__);)
            goto ErrorExit;
        }

        C_(printf("nand_memory.c(%d):Memory for emulated flash initialized successfuly!\n", __LINE__);)

#ifdef WIN32
        FileId = fopen("c:/nand_memory.bin", "rb");

        if (FileId == 0) {
            memset(NAND_Memory_Emul.MemBuffer, 0xFF, DeviceSize);
            FileId = fopen("c:/nand_memory.bin", "wb");
            assert(FileId != 0);

            WriteSize = fwrite(NAND_Memory_Emul.MemBuffer, 1, DeviceSize, FileId);
            assert(WriteSize == DeviceSize);
            WriteSize = fclose(FileId);
            assert(WriteSize == 0);

            FileId = fopen("c:/nand_memory.bin", "rb");
            assert(FileId != 0);
        }

        ReadSize = fread(NAND_Memory_Emul.MemBuffer, 1, DeviceSize, FileId);

        if (ReadSize != DeviceSize) {
            memset(NAND_Memory_Emul.MemBuffer, 0xFF, DeviceSize);
        }

        WriteSize = fclose(FileId);
        assert(WriteSize == 0);
#else
        memset(NAND_Memory_Emul.MemBuffer, 0xFF, DeviceSize);
#endif

        NAND_Memory_Emul.Block_p = NAND_Memory_Emul.MemBuffer;

        C_(printf("nand_memory.c(%d):Trying to allocate memory for Redundant Area, Size = %d\n", __LINE__, (DeviceSize / PageSize)*RedundantSize);)
        NAND_Memory_Emul.RedundantAreaBuff = (uint8 *) malloc((DeviceSize / PageSize + 1) * RedundantSize);

        if (NAND_Memory_Emul.RedundantAreaBuff == NULL) {
            A_(printf("nand_memory.c(%d):Error malloc failed to allocate memory\n", __LINE__);)
            goto ErrorExit;
        }

#ifdef WIN32
        FileId = fopen("c:/redundant_memory.bin", "rb");

        if (FileId == 0) {
            memset(NAND_Memory_Emul.RedundantAreaBuff, 0xFF, (DeviceSize / PageSize + 1)*RedundantSize);
            FileId = fopen("c:/redundant_memory.bin", "wb");
            assert(FileId != 0);

            WriteSize = fwrite(NAND_Memory_Emul.RedundantAreaBuff, 1, (DeviceSize / PageSize + 1) * RedundantSize, FileId);
            assert(WriteSize == ((DeviceSize / PageSize + 1)*RedundantSize));
            WriteSize = fclose(FileId);
            assert(WriteSize == 0);

            FileId = fopen("c:/redundant_memory.bin", "rb");
            assert(FileId != 0);
        }

        ReadSize = fread(NAND_Memory_Emul.RedundantAreaBuff, 1, ((DeviceSize / PageSize + 1) * RedundantSize), FileId);

        if (ReadSize != ((DeviceSize / PageSize + 1)*RedundantSize)) {
            memset(NAND_Memory_Emul.RedundantAreaBuff, 0xFF, (DeviceSize / PageSize + 1)*RedundantSize);
        }

        WriteSize = fclose(FileId);
        assert(WriteSize == 0);
#else
        memset(NAND_Memory_Emul.RedundantAreaBuff, 0xFF, (DeviceSize / PageSize + 1)*RedundantSize);
#endif

        NAND_Memory_Emul.RedundantArea_p = NAND_Memory_Emul.RedundantAreaBuff;

        EMUL_NandHW_Initialized = 1;
ErrorExit:
        A_(printf("nand_memory.c(%d):Initialization of emulated NAND flash memory finished\n", __LINE__);)
        return RetVal;
    }

    return T_PD_NAND_RESULT_SUCCESS;
}

void NANDHW_Memory_SetBlock_p(const uint32 Addr)
{
    NAND_Memory_Emul.Block_p = NAND_Memory_Emul.MemBuffer + Addr * PageSize;
    NAND_Memory_Emul.ECC_p = NAND_Memory_Emul.RedundantAreaBuff + Addr * RedundantSize + NANDHW_ECC_OFFSET;
    //NANDHW_Memory_SetRedundantArea_p(Addr);
}

void NANDHW_Memory_SetRedundantArea_p(const uint32 Addr)
{
    NAND_Memory_Emul.RedundantArea_p = NAND_Memory_Emul.RedundantAreaBuff + Addr * RedundantSize;
    NAND_Memory_Emul.ECC_p = NAND_Memory_Emul.RedundantAreaBuff + Addr * RedundantSize + NANDHW_ECC_OFFSET;
}

void NANDHW_Memory_Reset()
{
    NAND_Memory_Emul.Block_p = NAND_Memory_Emul.MemBuffer;
    NAND_Memory_Emul.RedundantArea_p = NAND_Memory_Emul.RedundantAreaBuff;
    NAND_Memory_Emul.ECC_p = NAND_Memory_Emul.RedundantAreaBuff + NANDHW_ECC_OFFSET;
}

/**     @}*/
/**@}*/
