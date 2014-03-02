/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @addtogroup ldr_service_fpd Flash Physical Driver
 * @{
 *   @addtogroup ldr_service_fpd_hw_u5700
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_service_management.h"
#include "fm_pd_nand_nor.h"

#include "r_pd_nand.h"
#include "c_pd_nand.h"

#include "flash_api.h"
#include "flash_conf.h"
#include "r_pd_cfi_flash.h"
#include "memmap_physical.h"
#include "asic_emif_nor_macro.h"
#include "asic_emif_norcon_macro.h"
#include "asic_ssppb0_macro.h"
#include "asic_syscon_macro.h"
#include "asic_emif_nandcon_macro.h"
#include "asic_intcon_macro.h"

#include "block_device_management.h"
#include "boot_area_management.h"
#include "r_boot_records_handler.h"
#include "r_bootparam.h"
#include "t_bootrom.h"
#include "jmptable.h"   /* jmptable.h always should be included last */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Offset of FPD PD_NAND error codes in loader local error code table. */
#define PD_NAND_ERROR_OFFSET  1750
/** Offset of FPD NOR error codes in loader local error code table. */
#define PD_CFI_ERROR_OFFSET  2650

#define NOR_PAGE_SIZE (1024)

/** Settings for NOR controller and NOR memory */
#define EMIF_NOR_INT_CLR0 0x00000008
#define EMIF_NOR_INT_CLR1 0x00000010
#define EMIF_NOR_ECC_INT_DISABLE0 0x00000020
#define EMIF_NOR_ECC_INT_DISABLE1 0x00000040
#define EMIF_NOR_CYCLES_T_RC 0x00000005
#define EMIF_NOR_CYCLES_T_WC 0x00000050
#define EMIF_NOR_CYCLES_T_CEOE 0x00000200
#define EMIF_NOR_CYCLES_T_WP 0x00002000
#define EMIF_NOR_CYCLES_T_PC 0x00008000
#define EMIF_NOR_CYCLES_T_TR 0x00020000
#define EMIF_NOR_CYCLES_WE_TIME 0x00100000
#define EMIF_NORCON_SRAM_0_SRAM_MASK_CS_ADDR 0x00000078
#define EMIF_NORCON_SRAM_1_SRAM_MASK_CS_ADDR 0x0000007C
#define EMIF_NORCON_SRAM_1_SRAM_CS_ADDR_MATCH 0x00000800
#define EMIF_NORCON_SRAM_2_SRAM_MASK_CS_ADDR 0x0000007E
#define EMIF_NORCON_SRAM_2_SRAM_CS_ADDR_MATCH 0x00000A00
#define EMIF_NORCON_SRAM_3_SRAM_MASK_CS_ADDR 0x0000007C
#define EMIF_NORCON_SRAM_3_SRAM_CS_ADDR_MATCH 0x00000C00
#define EMIF_NORCON_SRAM_4_SRAM_MASK_CS_ADDR 0x0000007C
#define EMIF_NORCON_SRAM_4_SRAM_CS_ADDR_MATCH 0x00001000
#define EMIF_NORCON_SRAM_5_SRAM_MASK_CS_ADDR 0x0000007C
#define EMIF_NORCON_SRAM_5_SRAM_CS_ADDR_MATCH 0x00001400
#define EMIF_NORCON_SRAM_6_SRAM_MASK_CS_ADDR 0x0000007C
#define EMIF_NORCON_SRAM_6_SRAM_CS_ADDR_MATCH 0x00001800
#define EMIF_NORCON_SRAM_7_SRAM_MASK_CS_ADDR 0x0000007C
#define EMIF_NORCON_SRAM_7_SRAM_CS_ADDR_MATCH 0x00001C00

static FPD_Config_t FPD_Config = {0};
static PD_CFI_InstancePtr_t Instance_p = NULL;

static PD_NAND_Config_t C_PD_NAND_Config = {
    0,                                      // - uint8 ChipSelect;
    T_PD_NAND_ECC_PAGE_SIZE_512,            // - EccPageSize_t EccPageSize;
    8,                                      // - uint8 ExtraDataLength;
    5,                                      // - uint32 MaxIterationsForUnrecoverableErrors
    T_PD_NAND_CONFIG_FLAG_ECC_ENABLE      |
    T_PD_NAND_CONFIG_FLAG_USE_AUTO_ECC    |
    T_PD_NAND_CONFIG_FLAG_BLOCKING_MUTEX  |
    T_PD_NAND_CONFIG_FLAG_POLL_AFTER_READ |
    T_PD_NAND_CONFIG_FLAG_POLL_AFTER_WRITE   // - PD_NAND_ConfigFlags_t Flags
};

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static void FPD_PDNAND_NOR_Detect(void);
static void FPD_PDNAND_NOR_GetMemConfig(void);
static void Do_Loader_Setup_NOR_HW(void);
static void Do_Loader_Setup_NOR_FAM(void);
static int famIrqPollFunction(void);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * This function initialize FPD_Config_t structure with flash memory properties
 * from FPD.
 *
 * @return       @ref E_SUCCESS  � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
void Do_PDNAND_NOR_Init(void)
{
    FPD_PDNAND_NOR_Detect();
    FPD_PDNAND_NOR_GetMemConfig();
}

/**
 * This function initialize Phisycal driver, sets memory properties.
 *
 * @return       @ref E_SUCCESS  � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_Start(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    PD_NAND_Result_t PD_NAND_Result = T_PD_NAND_RESULT_UNDEFINED;
    PD_NAND_MemoryProperties_t NandConfig = {0};
    uint8 Chip_Select = 0;

    if ((TRUE == FPD_Config.Configuration_Set) && (MEM_TYPE_NAND == FPD_Config.Loader_Flash_Mem_Type)) {
        NandConfig.BusWidthInBits     = FPD_Config.BusWidth;
        NandConfig.RecordLength       = sizeof(PD_NAND_MemoryProperties_t);
        NandConfig.Magic              = 0xEDA96521;
        NandConfig.MemoryType         = 1;
        NandConfig.Size               = FPD_Config.DeviceSize;
        NandConfig.BlockSize          = FPD_Config.EraseBlockSize;
        NandConfig.PageSize           = FPD_Config.PageSize;
        NandConfig.PageExtraSize      = FPD_Config.PageExtraSize;
        NandConfig.NbrOfAddressCycles = FPD_Config.NbrOfAddressCycles;

        C_PD_NAND_Config.ChipSelect = Chip_Select;

        PD_NAND_Result = Do_PD_NAND_SetMemoryProperties(&C_PD_NAND_Config, &NandConfig);
        VERIFY(T_PD_NAND_RESULT_SUCCESS == PD_NAND_Result, (ErrorCode_e)(PD_NAND_Result + PD_NAND_ERROR_OFFSET));
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * Phisycal driver can't be stop. This function need to exist for service
 * manager to work properly.
 *
 * @return       @ref E_SUCCESS  � Success ending.
 */
ErrorCode_e Do_PDNAND_NOR_Stop(void)
{
    return E_SUCCESS;
}

/**
 * Retrieves memory properties.
 *
 * @param [out]  Configuration_p - Pointer to configuration structure.
 * @return       @ref E_SUCCESS  � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_GetInfo(FPD_Config_t *Configuration_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    PD_NAND_Result_t PD_NAND_Result = T_PD_NAND_RESULT_UNDEFINED;
    PD_NAND_MemoryProperties_t NandConfig = {0};

    if ((TRUE == FPD_Config.Configuration_Set) && (MEM_TYPE_NOR == FPD_Config.Loader_Flash_Mem_Type)) {
        Configuration_p->BusWidth = 0;
        Configuration_p->DeviceSize = FPD_Config.DeviceSize;
        Configuration_p->EraseBlockSize = FPD_Config.EraseBlockSize;
        Configuration_p->PageSize = FPD_Config.PageSize;
        Configuration_p->PageExtraSize = 0;
        Configuration_p->NbrOfAddressCycles = 0;
        Configuration_p->Configuration_Set = TRUE;
        Configuration_p->Loader_Flash_Mem_Type = MEM_TYPE_NOR;
    } else if ((TRUE == FPD_Config.Configuration_Set) && (MEM_TYPE_NAND == FPD_Config.Loader_Flash_Mem_Type)) {
        PD_NAND_Result = Do_PD_NAND_GetMemoryProperties(&C_PD_NAND_Config, &NandConfig);
        VERIFY(T_PD_NAND_RESULT_SUCCESS == PD_NAND_Result, (ErrorCode_e)(PD_NAND_Result + PD_NAND_ERROR_OFFSET));

        Configuration_p->BusWidth = NandConfig.BusWidthInBits;
        Configuration_p->DeviceSize = NandConfig.Size;
        Configuration_p->EraseBlockSize = NandConfig.BlockSize;
        Configuration_p->PageSize = NandConfig.PageSize;
        Configuration_p->PageExtraSize = NandConfig.PageExtraSize;
        Configuration_p->NbrOfAddressCycles = NandConfig.NbrOfAddressCycles;
        Configuration_p->Loader_Flash_Mem_Type = MEM_TYPE_NAND;
        Configuration_p->Configuration_Set = TRUE;
    } else {
        ReturnValue = E_SERVICE_NOT_SUPPORTED;
        goto ErrorExit;
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * Retrieves memory properties.
 *
 * @param [in] NewConfig_p    - Contains necessary information for
 *                              configuring the FPD.
 * @return     @ref E_SUCCESS � Success ending.
 *
 * @return     @ref E_INVALID_INPUT_PARAMETER - If NULL is passed.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_SetInfo(FPD_Config_t *NewConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    VERIFY(NULL != NewConfig_p, E_INVALID_INPUT_PARAMETER);
    memcpy(&FPD_Config, NewConfig_p, sizeof(FPD_Config_t));

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * Reads one page and/or extra data from FPD. The length of the data buffer
 * Data_p must be "page size" * NbrOfPages. The length of the ExtraData_p must
 * be the ExtraDataLength * NbrOfPages.
 *
 * @param [in]   PageOffset     - An offset expressed in number of pages.
 * @param [out]  Data_p         - Data buffer. May be NULL if extra data is to
 *                                be read.
 * @param [out]  ExtraData_p    - Data buffer. May be NULL if page data is to
 *                                be read.
 * @return       @ref E_SUCCESS � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */

ErrorCode_e Do_PDNAND_NOR_ReadPage(const uint32 PageOffset,
                                   uint8 *Data_p,
                                   uint8 *ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    PD_NAND_Result_t PD_NAND_Result = T_PD_NAND_RESULT_UNDEFINED;
    PD_CFI_Result_t CFI_Result = PD_CFI_EHARDWARE_ERROR;

    if (MEM_TYPE_NAND == FPD_Config.Loader_Flash_Mem_Type) {
        PD_NAND_Result = Do_PD_NAND_ReadPage(&C_PD_NAND_Config, PageOffset, Data_p, ExtraData_p);

        if (T_PD_NAND_RESULT_SUCCESS != PD_NAND_Result) {
            if (T_PD_NAND_RESULT_PAGE_ERASED != PD_NAND_Result) {
                ReturnValue = (ErrorCode_e)(PD_NAND_Result + PD_NAND_ERROR_OFFSET);
                B_(printf("fm_pd_nand_nor.c (%d): Do_PD_NAND_ReadPage return %d", __LINE__, ReturnValue);)
            } else {
                ReturnValue = E_SUCCESS;
            }
        } else {
            ReturnValue = E_SUCCESS;
        }
    } else {
        CFI_Result = PD_CFI_ReadFromFlash(Instance_p, PD_CFI_ACCESSMODE_OBJECT, (PD_CFI_FlashAddr)(MEMMAP_PHY_APP_NOR_START + PageOffset * FPD_Config.PageSize), Data_p, FPD_Config.PageSize);
        VERIFY(PD_CFI_OK == CFI_Result, (ErrorCode_e)(CFI_Result + PD_CFI_ERROR_OFFSET));
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * Reads one or several consecutive pages and/or extra data from FPD. The size
 * of the data buffer Data_p must be "page size" * NbrOfPages. The size of the
 * data buffer ExtraData_p must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfPages     - Number of pages to read.
 * @param [out] Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be read.
 * @param [out] ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be read.
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_ReadConsecutivePages(const uint32 PageOffset,
        const uint32 NbrOfPages,
        uint8 *Data_p,
        uint8 *ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    PD_NAND_Result_t PD_NAND_Result = T_PD_NAND_RESULT_UNDEFINED;
    uint32 NbrOfReadPages = 0;

    PD_NAND_Result = Do_PD_NAND_ReadConsecutivePages(&C_PD_NAND_Config, PageOffset, NbrOfPages, Data_p, ExtraData_p, &NbrOfReadPages);

    if (T_PD_NAND_RESULT_SUCCESS != PD_NAND_Result) {
        if (T_PD_NAND_RESULT_PAGE_ERASED != PD_NAND_Result) {
            ReturnValue = (ErrorCode_e)(PD_NAND_Result + PD_NAND_ERROR_OFFSET);
            B_(printf("fm_pd_nand_nor.c (%d): Do_PD_NAND_ReadPage return %d", __LINE__, ReturnValue);)
        } else {
            ReturnValue = E_SUCCESS;
        }
    } else {
        ReturnValue = E_SUCCESS;
    }

    return ReturnValue;
}

/**
 * Writes one page and/or extra data to FPD. The size of the data buffer Data_p
 * must be "page size" * NbrOfPages. The size of the data buffer ExtraData_p
 * must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be written.
 * @param [in]  ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be written.
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_WritePage(const uint32 PageOffset,
                                    const uint8 *const Data_p,
                                    const uint8 *const ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    PD_NAND_Result_t PD_NAND_Result = T_PD_NAND_RESULT_UNDEFINED;
    PD_CFI_Result_t CFI_Result = PD_CFI_EHARDWARE_ERROR;
    uint32 Program = 0;
    size_t AmountWritten = 0;

    if (MEM_TYPE_NAND == FPD_Config.Loader_Flash_Mem_Type) {
        PD_NAND_Result = Do_PD_NAND_WritePage(&C_PD_NAND_Config, PageOffset, Data_p, ExtraData_p, Program);
        VERIFY(T_PD_NAND_RESULT_SUCCESS == PD_NAND_Result, (ErrorCode_e)(PD_NAND_Result + PD_NAND_ERROR_OFFSET));
    } else {
        CFI_Result = PD_CFI_WriteToFlash(Instance_p, (void *)(Data_p), (PD_CFI_FlashAddr)(MEMMAP_PHY_APP_NOR_START + PageOffset * FPD_Config.PageSize),
                                         FPD_Config.PageSize, &AmountWritten);

        if (PD_CFI_OK != CFI_Result) {
            A_(printf("fm_pd_nand_nor.c (%d): ** ERR: Failed to program the flash! **\n", __LINE__);)
            (void)PD_CFI_ResetFlash(Instance_p, (PD_CFI_FlashAddr)(MEMMAP_PHY_APP_NOR_START + PageOffset * FPD_Config.PageSize));
            ReturnValue = (ErrorCode_e)(CFI_Result + PD_CFI_ERROR_OFFSET);
            goto ErrorExit;
        }

        VERIFY(FPD_Config.PageSize <= AmountWritten, E_GENERAL_FATAL_ERROR);

        CFI_Result = PD_CFI_GetFlashStatus(Instance_p, (PD_CFI_FlashAddr)(MEMMAP_PHY_APP_NOR_START + PageOffset * FPD_Config.PageSize), FPD_Config.PageSize);
        VERIFY(PD_CFI_OK == CFI_Result, (ErrorCode_e)(CFI_Result + PD_CFI_ERROR_OFFSET));
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * Writes one or several consecutive pages and/or extra data to FPD. The size of
 * the data buffer Data_p must be "page size" * NbrOfPages. The size of the data
 * buffer ExtraData_p must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfPages     - Number of pages to write.
 * @param [in]  Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be written.
 * @param [in]  ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be written.
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_WriteConsecutivePages(const uint32 PageOffset,
        const uint32 NbrOfPages,
        const uint8 *const Data_p,
        const uint8 *const ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    PD_NAND_Result_t PD_NAND_Result = T_PD_NAND_RESULT_UNDEFINED;
    uint32 NbrOfWrittenPages = 0;

    PD_NAND_Result = Do_PD_NAND_WriteConsecutivePages(&C_PD_NAND_Config, PageOffset, NbrOfPages, Data_p, ExtraData_p, &NbrOfWrittenPages);
    VERIFY(T_PD_NAND_RESULT_SUCCESS == PD_NAND_Result, (ErrorCode_e)(PD_NAND_Result + PD_NAND_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * Erases one or several blocks in FPD.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfBlocks    - Number of blocks to erase.
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_Erase(uint32 PageOffset,
                                uint32 NbrOfBlocks)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    PD_NAND_Result_t PD_NAND_Result = T_PD_NAND_RESULT_UNDEFINED;
    PD_CFI_Result_t CFI_Result = PD_CFI_EHARDWARE_ERROR;
    uint32 NbrOfErasedBlocks = 0;

    if ((TRUE == FPD_Config.Configuration_Set) && (MEM_TYPE_NAND == FPD_Config.Loader_Flash_Mem_Type)) {
        PD_NAND_Result = Do_PD_NAND_Erase(&C_PD_NAND_Config, PageOffset, NbrOfBlocks, &NbrOfErasedBlocks);

        if (T_PD_NAND_RESULT_SUCCESS != PD_NAND_Result) {
            if (T_PD_NAND_RESULT_PAGE_ERASED != PD_NAND_Result) {
                ReturnValue = (ErrorCode_e)(PD_NAND_Result + PD_NAND_ERROR_OFFSET);
                B_(printf("fm_pd_nand_nor.c (%d): Do_PD_NAND_ReadPage return %d", __LINE__, ReturnValue);)
            } else {
                ReturnValue = E_SUCCESS;
            }
        } else {
            ReturnValue = E_SUCCESS;
        }
    }

    if ((TRUE == FPD_Config.Configuration_Set) && (MEM_TYPE_NOR == FPD_Config.Loader_Flash_Mem_Type)) {
        uint32 StartingBlock = 0;
        uint32 NorFlashSize = 0;
        uint32 MaxNbrBlocks = 0;

        C_(printf("fm_pd_nand_nor.c (%d): Do_FPD_Erase called **\n", __LINE__);)
        C_(printf("fm_pd_nand_nor.c (%d): PageOffset = (%d)**\n", __LINE__, PageOffset);)
        C_(printf("fm_pd_nand_nor.c (%d): NbrOfBlocks = (%d)**\n", __LINE__, NbrOfBlocks);)

        StartingBlock = PageOffset / (FPD_Config.EraseBlockSize / FPD_Config.PageSize);
        C_(printf("fm_pd_nand_nor.c (%d): ** Starting block  = (%d). **\n", __LINE__, StartingBlock);)
        NorFlashSize = PD_CFI_GetFlashSize((PD_CFI_FlashAddr *)MEMMAP_PHY_APP_NOR_START);
        MaxNbrBlocks = NorFlashSize / FPD_Config.EraseBlockSize;

        while (NbrOfErasedBlocks < NbrOfBlocks) {
            C_(printf("fm_pd_nand_nor.c (%d):trying to erase block at address %x \n", __LINE__, (MEMMAP_PHY_APP_NOR_START + StartingBlock * FPD_Config.EraseBlockSize + NbrOfErasedBlocks * FPD_Config.EraseBlockSize));)
            CFI_Result = PD_CFI_EraseFlashBlock(Instance_p, (PD_CFI_FlashAddr)(MEMMAP_PHY_APP_NOR_START + StartingBlock * FPD_Config.EraseBlockSize + NbrOfErasedBlocks * FPD_Config.EraseBlockSize));

            if (PD_CFI_OK != CFI_Result) {
                (void)PD_CFI_ResetFlash(Instance_p, (PD_CFI_FlashAddr)MEMMAP_PHY_APP_NOR_START);
                A_(printf("fm_pd_nand_nor.c (%d): ** ERR: Failed to erase flash block. ERROR is %x **\n", __LINE__, CFI_Result);)
            }

            do {
                CFI_Result = PD_CFI_GetFlashStatus(Instance_p, (PD_CFI_FlashAddr)(MEMMAP_PHY_APP_NOR_START + StartingBlock * FPD_Config.EraseBlockSize + NbrOfErasedBlocks * FPD_Config.EraseBlockSize), 0);

                if (PD_CFI_EHARDWARE_ERROR == CFI_Result) {
                    (void)PD_CFI_ResetFlash(Instance_p, (PD_CFI_FlashAddr)MEMMAP_PHY_APP_NOR_START);
                    A_(printf("fm_pd_nand_nor.c (%d): ** ERR: Status check has failed! ERROR is %x **\n", __LINE__, CFI_Result);)
                }
            } while (PD_CFI_OK != CFI_Result);

            NbrOfErasedBlocks++;

            if (NbrOfErasedBlocks >= MaxNbrBlocks) {
                A_(printf("fm_pd_nand_nor.c (%d): ** ERR: Block Number is out of range **\n", __LINE__);)
                break;
            }
        }

        (void)PD_CFI_ResetFlash(Instance_p, (PD_CFI_FlashAddr)MEMMAP_PHY_APP_NOR_START);
        ReturnValue = E_SUCCESS;
    }

    return ReturnValue;
}
static void MakeNORGoBurstMode(void)
{
    volatile uint16 *norptr;
    norptr = (uint16 *)0x8000520E;
    *norptr = 0x60;
    *norptr = 0x03;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static void Do_Loader_Setup_NOR_HW(void)
{
    uint32 Index = 0;

    //Enable clock for EMIF0
    SYSCON_WR_SCLKEN_0(IO_BASE_SYSCON_P0, SYSCON_SCLKEN_0_CTRL_EMIF_SRAM);

    // Reset EMIF0

    SSPPB0_WR_EMIF0_SRAM_RST_CTRL(SSPPB0_EMIF0_SRAM_RST_CTRL_MASK &
                                  (~SSPPB0_EMIF0_SRAM_RST_CTRL_MAIN_ENABLE)); //0xE0010000, OFFSET = 0x060C

    SSPPB0_WR_EMIF0_SRAM_RST_CTRL(SSPPB0_EMIF0_SRAM_RST_CTRL_MASK &
                                  (SSPPB0_EMIF0_SRAM_RST_CTRL_MAIN_ENABLE)); //0xE0010000, OFFSET = 0x060C

    SSPPB0_WR_EMIF0_SRAM_RST_CTRL(SSPPB0_EMIF0_SRAM_RST_CTRL_MASK &
                                  (~SSPPB0_EMIF0_SRAM_RST_CTRL_MAIN_ENABLE)); //0xE0010000, OFFSET = 0x060C

    EMIF_NANDCON_WR_IP_CONFIG_BUSY_PAUSE_SEL_DISABLE();

    SSPPB0_WR_EMIF0_SRAM_RST_CTRL_MAIN_ENABLE();
    //SSPPB0_WR_EMIF0_EBI_RST_CTRL_MAIN_ENABLE();
    SSPPB0_WR_EMIF0_NAND_RST_CTRL_MAIN_ENABLE();
    //SSPPB0_WR_EMIF0_SDRAM_RST_CTRL_MAIN_ENABLE();

    SSPPB0_WR_EMIF0_SRAM_RST_CTRL_MAIN_DISABLE();
    //SSPPB0_WR_EMIF0_EBI_RST_CTRL_MAIN_DISABLE();
    SSPPB0_WR_EMIF0_NAND_RST_CTRL_MAIN_DISABLE();
    //SSPPB0_WR_EMIF0_SDRAM_RST_CTRL_MAIN_DISABLE();
    // Set EMIF0 to Sync Burst Mode
    MakeNORGoBurstMode();
    EMIF_NOR_WR_MEMC_CFG_CLR(EMIF_NOR_ECC_INT_DISABLE1 |
                             EMIF_NOR_ECC_INT_DISABLE0 |
                             EMIF_NOR_INT_CLR1 |
                             EMIF_NOR_INT_CLR0 |
                             EMIF_NOR_MEMC_CFG_CLR_INT_DISABLE0_DISABLE |
                             EMIF_NOR_MEMC_CFG_CLR_INT_DISABLE1_DISABLE |
                             EMIF_NOR_MEMC_CFG_CLR_LOW_POWER_EXIT_DISABLE); //0xE0010000, OFFSET = 0x00C

    EMIF_NOR_WR_CYCLES(EMIF_NOR_CYCLES_T_RC |
                       EMIF_NOR_CYCLES_T_WC |
                       EMIF_NOR_CYCLES_T_CEOE |
                       EMIF_NOR_CYCLES_T_WP |
                       EMIF_NOR_CYCLES_T_PC |
                       EMIF_NOR_CYCLES_T_TR |
                       EMIF_NOR_CYCLES_WE_TIME); //0xE0010000, OFFSET = 0x014


    EMIF_NOR_WR_S_OPMODE(EMIF_NOR_S_OPMODE_MW_16BIT |
                         EMIF_NOR_S_OPMODE_READ_SYNC_ENABLE |
                         EMIF_NOR_S_OPMODE_READ_BL_16 |
                         EMIF_NOR_S_OPMODE_ADV_ENABLE |
                         EMIF_NOR_S_OPMODE_BURST_ALIGN_SPLIT_MEM_BURST); //0xE0010000, OFFSET = 0x018

    EMIF_NOR_WR_DIRECT_CMD(EMIF_NOR_DIRECT_CMD_CMD_TYPE_UPD_REGS); //0xE0010000, OFFSET = 0x010

    EMIF_NORCON_WR_SRAM_0_ADDR_CONFIG_CS(Index++, EMIF_NORCON_SRAM_0_SRAM_MASK_CS_ADDR); //0xE0010000, OFFSET = 0x50C

    EMIF_NORCON_WR_SRAM_0_ADDR_CONFIG_CS(Index++, EMIF_NORCON_SRAM_1_SRAM_MASK_CS_ADDR |
                                         EMIF_NORCON_SRAM_1_SRAM_CS_ADDR_MATCH); //0xE0010000, OFFSET = 0x510

    EMIF_NORCON_WR_SRAM_0_ADDR_CONFIG_CS(Index++, EMIF_NORCON_SRAM_2_SRAM_MASK_CS_ADDR |
                                         EMIF_NORCON_SRAM_2_SRAM_CS_ADDR_MATCH); //0xE0010000, OFFSET = 0x514

    EMIF_NORCON_WR_SRAM_0_ADDR_CONFIG_CS(Index++, EMIF_NORCON_SRAM_3_SRAM_MASK_CS_ADDR |
                                         EMIF_NORCON_SRAM_3_SRAM_CS_ADDR_MATCH); //0xE0010000, OFFSET = 0x518

    EMIF_NORCON_WR_SRAM_0_ADDR_CONFIG_CS(Index++, EMIF_NORCON_SRAM_4_SRAM_MASK_CS_ADDR |
                                         EMIF_NORCON_SRAM_4_SRAM_CS_ADDR_MATCH); //0xE0010000, OFFSET = 0x51C

    EMIF_NORCON_WR_SRAM_0_ADDR_CONFIG_CS(Index++, EMIF_NORCON_SRAM_5_SRAM_MASK_CS_ADDR |
                                         EMIF_NORCON_SRAM_5_SRAM_CS_ADDR_MATCH); //0xE0010000, OFFSET = 0x520

    EMIF_NORCON_WR_SRAM_0_ADDR_CONFIG_CS(Index++, EMIF_NORCON_SRAM_6_SRAM_MASK_CS_ADDR |
                                         EMIF_NORCON_SRAM_6_SRAM_CS_ADDR_MATCH); //0xE0010000, OFFSET = 0x524

    EMIF_NORCON_WR_SRAM_0_ADDR_CONFIG_CS(Index, EMIF_NORCON_SRAM_7_SRAM_MASK_CS_ADDR |
                                         EMIF_NORCON_SRAM_7_SRAM_CS_ADDR_MATCH); //0xE0010000, OFFSET = 0x528
}

static void Do_Loader_Setup_NOR_FAM(void)
{
    FlashConfResult Result;
    uint32 NorFlashSize;
    uint32 InstanceSize;
    static uint32 initialized = 0;
#define FAM_MAX_SIMULTANOUS_USERS 10

    if (0 == initialized) {
        Result = flash_initialize(famIrqPollFunction, NULL, FAM_MAX_SIMULTANOUS_USERS);

        if (Result != FLASHCONF_EOK) {
            A_(printf("fm_pd_nand_nor.c (%d) flash_initialize error=0x%lx \n", __LINE__, Result);)
        }

        extern const union FlashDriverInterface *cfi_flash_driverentry(int interface_version);
        Result = flash_install_driver((union FlashDriverInterface const * ( *)(U32))cfi_flash_driverentry, "cfi", "");

        if (Result != FLASHCONF_EOK) {
            A_(printf("fm_pd_nand_nor.c (%d) flash_initialize error=0x%lx \n", __LINE__, Result);)
        }

        NorFlashSize = PD_CFI_GetFlashSize((PD_CFI_FlashAddr *)MEMMAP_PHY_APP_NOR_START);
        InstanceSize = flash_add_instance("cfi", (char *)MEMMAP_PHY_APP_NOR_START, NorFlashSize, "");

        if (InstanceSize != NorFlashSize) {
            A_(printf("fm_pd_nand_nor.c (%d): ** ERR: flash_add_instance error=0x%lx **\n", __LINE__, InstanceSize);)
        } else {
            C_(printf("fm_pd_nand_nor.c (%d): ** Nor flash detected. Start: 0x%lx Size: 0x%lx **\n", __LINE__, MEMMAP_PHY_APP_NOR_START, InstanceSize);)
        }

        initialized = 1;
    }
}
uint32 Do_PD_INTCON_GetPendingInterrupt(void)
{
    return INTCON_RD_HIGHEST_ACTIVE_IRQ_VALUE(IO_BASE_INTCON_APP_SEC);
}

static int famIrqPollFunction(void)
{
    int returnVal;

    if (0 == Do_PD_INTCON_GetPendingInterrupt()) {
        returnVal = IRQ_POLL_NO_PENDING_INTERRUPT;
    } else {
        /* invert the bits to ensure something different */
        returnVal = ~IRQ_POLL_NO_PENDING_INTERRUPT;
    }

    return returnVal;
}

static void FPD_PDNAND_NOR_Detect(void)
{
    PD_CFI_Result_t CFI_Result = PD_CFI_EHARDWARE_ERROR;
    PD_NAND_MemoryProperties_t MemoryProperties = {0};
    PD_CFI_FlashAddr NorFlashStartAdress = 0;
    size_t InstanceBufferSize;
    uint32 NorFlashSize = 0;

    //Check if NOR flash device is present
    Do_Loader_Setup_NOR_HW();
    NorFlashStartAdress = (PD_CFI_FlashAddr)MEMMAP_PHY_APP_NOR_START;
    NorFlashSize = PD_CFI_GetFlashSize(NorFlashStartAdress);

    if (NorFlashSize != 0) {
        PD_CFI_FlashCharacteristics_t Characteristics = {0, };
        InstanceBufferSize = PD_CFI_GetInstanceBuffSize();
        Instance_p = (PD_CFI_InstancePtr_t)malloc(InstanceBufferSize);
        ASSERT(NULL != Instance_p);

        //Instantiation of devive must be dome before any other function that use Instance pointer is called
        CFI_Result = PD_CFI_InstantiateDevice(Instance_p, (PD_CFI_FlashAddr)MEMMAP_PHY_APP_NOR_START, NorFlashSize);

        if (PD_CFI_OK != CFI_Result) {
            A_(printf("fm_pd_nand_nor.c (%d): ** ERR: Failed to get instance information from flash. **\n", __LINE__);)
            return;
        }

        //Get instance size
        CFI_Result = PD_CFI_GetCharacteristics(Instance_p,
                                               PD_CFI_INSTANCE_SIZE,
                                               NorFlashStartAdress,
                                               &Characteristics);

        if (PD_CFI_OK != CFI_Result) {
            A_(printf("fm_pd_nand_nor.c (%d): ** ERR: Failed to get flash characteristics. **\n", __LINE__);)
            return;
        }

        FPD_Config.DeviceSize = Characteristics.range_length;
        C_(printf("fm_pd_nand_nor.c (%d): DeviceSize reported by FPD %d \n", __LINE__, FPD_Config.DeviceSize);)
        //get erase block size
        CFI_Result = PD_CFI_GetCharacteristics(Instance_p,
                                               PD_CFI_ERASEBLOCK_RANGE,
                                               NorFlashStartAdress,
                                               &Characteristics);

        if (PD_CFI_OK != CFI_Result) {
            A_(printf("fm_pd_nand_nor.c (%d): ** ERR: Failed to get flash characteristics. **\n", __LINE__);)
            return;
        }

        FPD_Config.EraseBlockSize = Characteristics.block_length;
        FPD_Config.PageSize = 4096;
        FPD_Config.Loader_Flash_Mem_Type = MEM_TYPE_NOR;
        FPD_Config.Configuration_Set     = TRUE;
        Do_Loader_Setup_NOR_FAM();
        return;
    }

    //Check if NAND flash device is present
    if (T_PD_NAND_RESULT_SUCCESS == Do_PD_NAND_LookUpNandFlashProperties(&C_PD_NAND_Config, &MemoryProperties)) {
        FPD_Config.Loader_Flash_Mem_Type = MEM_TYPE_NAND;
        FPD_Config.EraseBlockSize      = MemoryProperties.BlockSize;
        FPD_Config.DeviceSize          = MemoryProperties.Size * MemoryProperties.PageSize;
        FPD_Config.PageSize            = MemoryProperties.PageSize;
        FPD_Config.PageExtraSize       = MemoryProperties.PageExtraSize;
        FPD_Config.BusWidth            = MemoryProperties.BusWidthInBits;
        FPD_Config.NbrOfAddressCycles  = MemoryProperties.NbrOfAddressCycles;
        FPD_Config.Configuration_Set   = TRUE;
        return;
    } else {
        A_(printf("fm_pd_nand_nor.c (%d): ** ERR: Failed to detect flash. **\n", __LINE__);)
        return;
    }
}

static void FPD_PDNAND_NOR_GetMemConfig(void)
{
    BOOTROM_MemConfig_t    *BOOTROM_MemConfig_p = NULL;
    BOOTROM_Return_t        BOOTROM_Return = BOOTROM_RETURN_FAILURE;
    BOOTPARAM_Status_t      BOOTPARAM_Status = BOOTPARAM_STATUS_NOT_FOUND;

    Memconf_BootRecord_t    MemConf = {0};
    uint32                  MemConf_Length = 0;

    Partitions_BootRecord_t Partitions = {0};
    uint32                  Partitions_Length = 0;
    uint32                  Partition_Counter = 0;

    Partition_BootRecord_t  Partition_Config = {0};
    uint32                  Partition_Config_Length = 0;
    uint32                  MBBS_Counter = 0;

    BAM_Config_t BAM_Config = {0};

    C_(printf("fm_pd_nand_nor.c(%d): Search MEMConf with BootROM!", __LINE__);)

    BOOTROM_MemConfig_p = R_Do_BOOTROM_GetMemConfig();

    if (BOOTROM_MemConfig_p->DeviceType != 0x0) {
        C_(printf(" -> SUCCESS!\n");)
        /* BOOTROM doesn't include first 3 members in it's BootRecord structures! */
        BOOTROM_MemConfig_p->CopsRecordAddress = 0;    /* Start searching from the beginning of boot records. */
        MemConf_Length = sizeof(Memconf_BootRecord_t) - (3 * sizeof(uint32));

        MemConf.MemconfId   = BOOTROM_MemConfig_p->DeviceSize;
        MemConf.MemconfSize = sizeof(Memconf_BootRecord_t);
        MemConf.Magic       = MEMCONF_MAGIC;

        C_(printf("fm_pd_nand_nor.c(%d): Search MEMCONF BootRecord!", __LINE__);)
        BOOTROM_Return = R_Do_BOOTROM_GetBootSeqRecord(BOOTROM_MemConfig_p, BOOTROM_MemConfig_p->DeviceSize, MEMCONF_MAGIC, (uint32 *)&MemConf + 3, &MemConf_Length, 0);

        if (BOOTROM_Return == BOOTROM_RETURN_OK) {
            C_(printf(" -> SUCCESS!\n");)

            switch (MemConf.Type) {
            case NOR_MEMORY:
                FPD_Config.Loader_Flash_Mem_Type = MEM_TYPE_NOR;
                FPD_Config.DeviceSize = MemConf.TotalSize * NOR_PAGE_SIZE;
                break;

            case NAND_SMALL:
            case NAND_SMALL_BLOCK0_ONLY:
            case NAND_LARGE:

            case NAND_LARGE_BLOCK0_ONLY:
                FPD_Config.Loader_Flash_Mem_Type = MEM_TYPE_NAND;
                FPD_Config.DeviceSize = MemConf.TotalSize * MemConf.PageSize;
                break;

            default:
                A_(printf("fm_pd_nand_nor.c(%d): Unsupported flash type \n", __LINE__);)
                FPD_Config.Loader_Flash_Mem_Type = MEM_TYPE_NONE;
                break;
            }

            FPD_Config.BusWidth           = MemConf.MemconfId;
            FPD_Config.EraseBlockSize     = MemConf.BlockSize;
            FPD_Config.PageSize           = MemConf.PageSize;
            FPD_Config.PageExtraSize      = MemConf.RedundantAreaSize;
            FPD_Config.NbrOfAddressCycles = MemConf.AddrCycles;
            FPD_Config.Configuration_Set  = TRUE;

            /* Update BootParams */
            BOOTPARAM_Status = Do_BOOTPARAM_WriteNew(BOOTPARAM_IDENTIFIER_MEMCONF, (uint32 *)&MemConf + 2, MemConf.MemconfSize - 2 * sizeof(uint32));
            ASSERT(BOOTPARAM_Status == BOOTPARAM_STATUS_OK);
        } else {
            C_(printf(" -> FAILED!  BOOTROM_Return = %d\n", BOOTROM_Return);)
        }

        if (BOOTROM_Return == BOOTROM_RETURN_OK) {
            BOOTROM_MemConfig_p->CopsRecordAddress = 0;    /* Start searching from the beginning of boot records. */
            Partitions_Length = sizeof(Partitions_BootRecord_t) - (3 * sizeof(uint32));

            Partitions.Partitions_Id   = PARTITIONS_ID;
            Partitions.Partitions_Size = sizeof(Partitions_BootRecord_t);
            Partitions.RecordIndex     = 0;

            BOOTROM_Return = R_Do_BOOTROM_GetBootSeqRecord(BOOTROM_MemConfig_p, PARTITIONS_ID, 0, (uint32 *)&Partitions + 3, &Partitions_Length, 0);

            if (BOOTROM_Return == BOOTROM_RETURN_OK) {
                /* Update BootParams */
                BOOTPARAM_Status = Do_BOOTPARAM_WriteNew(BOOTPARAM_IDENTIFIER_PARTITIONS, (uint32 *)&Partitions + 2, Partitions.Partitions_Size - 2 * sizeof(uint32));
                ASSERT(BOOTPARAM_Status == BOOTPARAM_STATUS_OK);
            } else {
                A_(printf("fm_pd_nand_nor.c(%d): R_Do_BOOTROM_GetBootSeqRecord PARTITIONS_ID FAILED! BOOTROM_Return = %d\n", __LINE__, BOOTROM_Return);)
            }
        }

        if (BOOTROM_Return == BOOTROM_RETURN_OK) {
            MBBS_Counter = 0;

            for (Partition_Counter = 0; Partition_Counter < Partitions.NrOfPartitions; Partition_Counter++) {
                BOOTROM_MemConfig_p->CopsRecordAddress = 0;    /* Start searching from the beginning of boot records. */
                Partition_Config_Length = sizeof(Partition_BootRecord_t) - (3 * sizeof(uint32));

                BOOTROM_Return = R_Do_BOOTROM_GetBootSeqRecord(BOOTROM_MemConfig_p, PARTITION_ID, Partition_Counter, (uint32 *)&Partition_Config + 3, &Partition_Config_Length, 0);

                if (BOOTROM_Return == BOOTROM_RETURN_OK) {
                    if (Partition_Config.Type == MBBS) {
                        C_(printf("fm_pd_nand_nor.c(%d): BAM configuration found successfully!\n", __LINE__);)

                        switch (FPD_Config.Loader_Flash_Mem_Type) {
                        case MEM_TYPE_NAND:
                            BAM_Config.Unit_Configuration[MBBS_Counter].MediaType           = MEM_TYPE_NAND;
                            BAM_Config.Unit_Configuration[MBBS_Counter].SubMediaType        = MEM_TYPE_NAND;
                            break;

                        case MEM_TYPE_NOR:
                            BAM_Config.Unit_Configuration[MBBS_Counter].MediaType           = MEM_TYPE_NOR;
                            BAM_Config.Unit_Configuration[MBBS_Counter].SubMediaType        = MEM_TYPE_NOR;
                            break;

                        default:
                            A_(printf("fm_pd_nand_nor.c(%d): Unsupported flash type \n", __LINE__);)
                            break;
                        }

                        BAM_Config.Version         = Partition_Config.Version;
                        BAM_Config.Number_Of_Units = MBBS_Counter + 1;

                        BAM_Config.Unit_Configuration[MBBS_Counter].ChipSelect          = Partition_Config.Other[2];    /* @TODO: Add ChipSelect in FPD configuration!? */
                        BAM_Config.Unit_Configuration[MBBS_Counter].EraseBlockSize      = FPD_Config.EraseBlockSize;
                        BAM_Config.Unit_Configuration[MBBS_Counter].PageSize            = FPD_Config.PageSize;
                        BAM_Config.Unit_Configuration[MBBS_Counter].RedundantAreaSize   = FPD_Config.PageExtraSize;
                        BAM_Config.Unit_Configuration[MBBS_Counter].StartBlock          = Partition_Config.StartInBytes;
                        BAM_Config.Unit_Configuration[MBBS_Counter].PagesPerBlock       = FPD_Config.EraseBlockSize / FPD_Config.PageSize;
                        BAM_Config.Unit_Configuration[MBBS_Counter].SectorsPerBlock     = BAM_Config.Unit_Configuration[MBBS_Counter].PagesPerBlock;
                        BAM_Config.Unit_Configuration[MBBS_Counter].EndBlockByteAddress = Partition_Config.EndInBytes;
                        BAM_Config.Unit_Configuration[MBBS_Counter].UseBootableCABS     = Partition_Config.Other[0];
                        BAM_Config.Unit_Configuration[MBBS_Counter].BBMType             = (BAM_BBM_t)Partition_Config.Other[1];

                        BAM_Config.Configuration_Set = TRUE;
                        MBBS_Counter++;

                        (void)Do_BAM_SetConfig(&BAM_Config);
                    } else {
                        C_(printf("fm_pd_nand_nor.c(%d): PARTITION configuration found successfully!\n", __LINE__);)
                    }

                    /* Update all partitions in BootParams */
                    Partition_Config.Record_Id   = PARTITION_ID;
                    Partition_Config.Size        = Partition_Config_Length + 3 * sizeof(uint32);
                    Partition_Config.RecordIndex = Partition_Counter;

                    BOOTPARAM_Status = Do_BOOTPARAM_WriteNewIndexed(BOOTPARAM_IDENTIFIER_PARTITION, Partition_Config.RecordIndex, (uint32 *)&Partition_Config + 3, Partition_Config.Size - 3 * sizeof(uint32));
                    ASSERT(BOOTPARAM_Status == BOOTPARAM_STATUS_OK);
                } else {
                    A_(printf("fm_pd_nand_nor.c(%d): R_Do_BOOTROM_GetBootSeqRecord PARTITION_ID FAILED! BOOTROM_Return = %d\n", __LINE__, BOOTROM_Return);)
                }
            }
        }
    } else {
        C_(printf(" -> FAILED!\n");)
    }
}

/*
 * Read configurations from BootRecordsHandler one by one or add input parameter
 * and read only specified configuration?
 */
ErrorCode_e Do_PDNAND_NOR_GetPartitionsConfiguration(void)
{
    ErrorCode_e             ReturnValue = E_GENERAL_FATAL_ERROR;
    BOOTPARAM_Status_t BOOTPARAM_Status = BOOTPARAM_STATUS_NOT_FOUND;

    uint32 MemConf_ID = 1;
    Memconf_BootRecord_t   *MEMConf_p = NULL;
    uint32                  MEMConf_Length = 0;

    Partitions_BootRecord_t *Partitions_p = NULL;
    uint32 PartitionsRecord_Length = 0;

    Partition_BootRecord_t *Partition_p = NULL;
    uint32 PartitionRecord_Length = 0;
    uint32 Partitions_Counter = 0;

    uint32 BAM_Unit = 0;
    uint32 BDM_Unit = 0;

    BAM_Config_t BAM_Config = {0};
    BDM_Config_t BDM_Config = {0};

    /* 1. Flash PD - MEMCONF */
    for (MemConf_ID = MEMCONF_1_ID; MemConf_ID <= MEMCONF_32_ID; MemConf_ID = MemConf_ID << 1) {
        ReturnValue = Do_Read_BootRecord(MemConf_ID, (uint32 **)&MEMConf_p, &MEMConf_Length);

        if (ReturnValue == E_SUCCESS) {
            C_(printf("fm_pd_nand_nor.c(%d): MEMCONF 0x%08X found!\n", __LINE__, MemConf_ID);)

            /* Update FPD Configuration */
            switch (MEMConf_p->Type) {
            case NOR_MEMORY:
                FPD_Config.Loader_Flash_Mem_Type = MEM_TYPE_NOR;
                FPD_Config.DeviceSize = MEMConf_p->TotalSize * NOR_PAGE_SIZE;
                break;

            case NAND_SMALL:
            case NAND_SMALL_BLOCK0_ONLY:
            case NAND_LARGE:

            case NAND_LARGE_BLOCK0_ONLY:
                FPD_Config.Loader_Flash_Mem_Type = MEM_TYPE_NAND;
                FPD_Config.DeviceSize = MEMConf_p->TotalSize * MEMConf_p->PageSize;
                break;

            default:
                A_(printf("fm_pd_nand_nor.c(%d): Unsupported flash type \n", __LINE__);)
                FPD_Config.Loader_Flash_Mem_Type = MEM_TYPE_NONE;
                ReturnValue = E_UNSUPPORTED_FLASH_TYPE;
                goto ErrorExit;
            }

            FPD_Config.BusWidth           = MEMConf_p->MemconfId;
            FPD_Config.EraseBlockSize     = MEMConf_p->BlockSize;
            FPD_Config.PageSize           = MEMConf_p->PageSize;
            FPD_Config.PageExtraSize      = MEMConf_p->RedundantAreaSize;
            FPD_Config.NbrOfAddressCycles = MEMConf_p->AddrCycles;
            FPD_Config.Configuration_Set  = TRUE;

            BOOTPARAM_Status = Do_BOOTPARAM_WriteNew(BOOTPARAM_IDENTIFIER_MEMCONF, (uint32 *)MEMConf_p + 2, MEMConf_Length - 2 * sizeof(uint32));
            ASSERT(BOOTPARAM_Status == BOOTPARAM_STATUS_OK);
            break;
        }
    }

    if (FALSE == FPD_Config.Configuration_Set) {
        ReturnValue = E_FPD_NOT_CONFIGURED;
        goto ErrorExit;
    }

    /* 2-3 BAM(MBBS) & BDM(CABS) - MBBSConfig & CABSConfig, 2 (GD)TA*/
    ReturnValue = Do_Read_BootRecord(PARTITIONS_ID, (uint32 **)&Partitions_p, &PartitionsRecord_Length);

    if (ReturnValue == E_SUCCESS) {
        BOOTPARAM_Status = Do_BOOTPARAM_WriteNew(BOOTPARAM_IDENTIFIER_PARTITIONS, (uint32 *)Partitions_p + 2, PartitionsRecord_Length - 2 * sizeof(uint32));
        ASSERT(BOOTPARAM_Status == BOOTPARAM_STATUS_OK);

        for (Partitions_Counter = 0; Partitions_Counter < Partitions_p->NrOfPartitions; Partitions_Counter++) {
            ReturnValue = Do_Read_BootRecordIndexed(PARTITION_ID, Partitions_Counter, (uint32 **)&Partition_p, &PartitionRecord_Length);

            if (ReturnValue == E_SUCCESS) {
                BOOTPARAM_Status = Do_BOOTPARAM_WriteNewIndexed(BOOTPARAM_IDENTIFIER_PARTITION, Partitions_Counter, (uint32 *)Partition_p + 3, PartitionRecord_Length - 3 * sizeof(uint32));
                ASSERT(BOOTPARAM_Status == BOOTPARAM_STATUS_OK);

                switch (Partition_p->Type) {
                case MBBS:
                    C_(printf("fm_pd_nand_nor.c(%d): PARTITION record type MBBS was found!\n", __LINE__);)
                    /* Update BAM configuration */
                    BAM_Config.Version         = Partition_p->Version;
                    BAM_Config.Number_Of_Units = BAM_Unit + 1;

                    BAM_Config.Unit_Configuration[BAM_Unit].ChipSelect          = Partition_p->Other[2];    /* @TODO: Add ChipSelect in FPD configuration!? */
                    BAM_Config.Configuration_Set = TRUE;
                    BAM_Config.Unit_Configuration[BAM_Unit].EraseBlockSize      = FPD_Config.EraseBlockSize;
                    BAM_Config.Unit_Configuration[BAM_Unit].PageSize            = FPD_Config.PageSize;
                    BAM_Config.Unit_Configuration[BAM_Unit].RedundantAreaSize   = FPD_Config.PageExtraSize;
                    BAM_Config.Unit_Configuration[BAM_Unit].PagesPerBlock       = FPD_Config.EraseBlockSize / FPD_Config.PageSize;
                    BAM_Config.Unit_Configuration[BAM_Unit].SectorsPerBlock     = BAM_Config.Unit_Configuration[BAM_Unit].PagesPerBlock;
                    BAM_Config.Unit_Configuration[BAM_Unit].StartBlock          = Partition_p->StartInBytes;
                    BAM_Config.Unit_Configuration[BAM_Unit].EndBlockByteAddress = Partition_p->EndInBytes;

                    switch (FPD_Config.Loader_Flash_Mem_Type) {
                    case MEM_TYPE_NAND:
                        BAM_Config.Unit_Configuration[BAM_Unit].MediaType           = MEM_TYPE_NAND;
                        BAM_Config.Unit_Configuration[BAM_Unit].SubMediaType        = MEM_TYPE_NAND;

                        if (!(FPD_Config.DeviceSize >= Partition_p->StartInBytes)) {
                            BAM_Config.Configuration_Set = FALSE;
                        }

                        if (!(FPD_Config.DeviceSize >= Partition_p->EndInBytes)) {
                            BAM_Config.Configuration_Set = FALSE;
                        }

                        break;

                    case MEM_TYPE_NOR:
                        BAM_Config.Unit_Configuration[BAM_Unit].MediaType           = MEM_TYPE_NOR;
                        BAM_Config.Unit_Configuration[BAM_Unit].SubMediaType        = MEM_TYPE_NOR;

                        if (!(FPD_Config.DeviceSize >= (Partition_p->StartInBytes - MEMMAP_PHY_APP_NOR_START))) {
                            BAM_Config.Configuration_Set = FALSE;
                        }

                        if (!(FPD_Config.DeviceSize >= (Partition_p->EndInBytes - MEMMAP_PHY_APP_NOR_START))) {
                            BAM_Config.Configuration_Set = FALSE;
                        }

                        break;

                    default:
                        A_(printf("fm_pd_nand_nor.c(%d): Unsupported flash type \n", __LINE__);)
                        ReturnValue = E_UNSUPPORTED_FLASH_TYPE;
                        goto ErrorExit;
                    }

                    BAM_Config.Unit_Configuration[BAM_Unit].UseBootableCABS     = Partition_p->Other[0];
                    BAM_Config.Unit_Configuration[BAM_Unit].BBMType             = (BAM_BBM_t)Partition_p->Other[1];
                    BAM_Unit++;

                    (void)Do_BAM_SetConfig(&BAM_Config);
                    break;

                case CABS:
                    C_(printf("\nfm_pd_nand_nor.c(%d): PARTITION record type CABS was found!\n", __LINE__);)
                    /* Update BDM configuration */
                    BDM_Config.Version         = Partition_p->Version;
                    BDM_Config.Number_Of_Units = BDM_Unit + 1;
                    BDM_Config.Configuration_Set = TRUE;

                    BDM_Config.Unit_Configuration[BDM_Unit].EraseBlockSizeInBytes = FPD_Config.EraseBlockSize;
                    BDM_Config.Unit_Configuration[BDM_Unit].PageSizeInBytes       = FPD_Config.PageSize;
                    BDM_Config.Unit_Configuration[BDM_Unit].RedundantAreaInBytes  = FPD_Config.PageExtraSize;
                    BDM_Config.Unit_Configuration[BDM_Unit].StartBlock            = Partition_p->StartInBytes;

                    BDM_Config.Unit_Configuration[BDM_Unit].LogicalSectors        = Partition_p->Other[0];
                    BDM_Config.Unit_Configuration[BDM_Unit].BadBlockOverhead      = Partition_p->Other[1];

                    switch (FPD_Config.Loader_Flash_Mem_Type) {
                    case MEM_TYPE_NOR:
                        BDM_Config.Unit_Configuration[BDM_Unit].BadBlockOverhead = 0;
                        BDM_Config.Unit_Configuration[BDM_Unit].MediaType = MEM_TYPE_NOR;

                        if (!(FPD_Config.DeviceSize >= (Partition_p->StartInBytes - MEMMAP_PHY_APP_NOR_START))) {
                            BDM_Config.Configuration_Set = FALSE;
                        }

                        BDM_Config.Unit_Configuration[BDM_Unit].EndBlock              = Partition_p->EndInBytes;

                        if (!(FPD_Config.DeviceSize >= (Partition_p->EndInBytes - MEMMAP_PHY_APP_NOR_START))) {
                            BDM_Config.Configuration_Set = FALSE;
                        }

                        break;

                    case MEM_TYPE_NAND:

                        if (!(FPD_Config.DeviceSize >= Partition_p->StartInBytes)) {
                            BDM_Config.Configuration_Set = FALSE;
                        }

                        BDM_Config.Unit_Configuration[BDM_Unit].EndBlock              = Partition_p->EndInBytes;

                        if (!(FPD_Config.DeviceSize >= Partition_p->EndInBytes)) {
                            BDM_Config.Configuration_Set = FALSE;
                        }

                        BDM_Config.Unit_Configuration[BDM_Unit].MediaType = MEM_TYPE_NAND;
                        break;

                    default:
                        A_(printf("fm_pd_nand_nor.c(%d): Unsupported flash type \n", __LINE__);)
                        ReturnValue = E_UNSUPPORTED_FLASH_TYPE;
                        goto ErrorExit;
                    }

                    BDM_Config.Unit_Configuration[BDM_Unit].ResevedBootBlocks     = Partition_p->Other[2];
                    BDM_Config.Unit_Configuration[BDM_Unit].ChipSelect            = Partition_p->Other[3];
                    BDM_Config.Unit_Configuration[BDM_Unit].Attributes            = Partition_p->Other[4];

                    BDM_Unit++;

                    (void)Do_BDM_SetConfig(&BDM_Config);
                    break;

                case TRIM:
                    C_(printf("\nfm_pd_nand_nor.c(%d): PARTITION record type TRIMAREA was found!\n", __LINE__);)
                    break;

                default:
                    A_(printf("fm_pd_nand_nor.c(%d): Unknown partition type! (0x%08X)\n", __LINE__, Partition_p->Type);)
                    break;
                }
            }
        }
    } else {
        B_(printf("fm_pd_nand_nor.c(%d): Partitions BootRecord NOT found!\n", __LINE__);)
    }

ErrorExit:
    return ReturnValue;
}

/** @} */
/** @} */
