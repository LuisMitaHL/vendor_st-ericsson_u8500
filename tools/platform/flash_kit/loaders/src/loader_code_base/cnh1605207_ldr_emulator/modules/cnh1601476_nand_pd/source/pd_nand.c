/*******************************************************************************
* $Copyright Ericsson AB 2009 $
********************************************************************************
*
* DESCRIPTION:
*
* Implementation of public API for NAND PD.
*
*******************************************************************************/

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_ENABLE_OS_INDEPENDENT_CODE: When defined NAND PD will use an OS free implementation.

// =============================================================================
//  Global Lint Suppression
// =============================================================================

// - Suppress all lint warnings: "lint -e{<nr>} <text>"

// =============================================================================
//  Include Header Files
// =============================================================================

#include "c_system_v2.h"
#include "t_basicdefinitions.h"
#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "stddef.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "t_pd_nand.h"
#include "r_pd_nand.h"
#include "r_emul_nand.h"
#include "asic_emif_nand_map.h"
#include "asic_emif_nand_macro.h"
//#include "pd_nand_module_test.h"
#include "r_emul_internal.h"

// =============================================================================
//  Defines
// =============================================================================

// - Print headers:
#define D_PD_NAND_HEADER               "[NAND_PD]: "
#define D_PD_NAND_HEADER_ERROR         "\n" D_PD_NAND_HEADER " *** Error *** "

// =============================================================================
//  Macros
// =============================================================================

#define D_PD_NAND_FLASH_COMMAND_READ_1ST                      0x00UL
#define D_PD_NAND_FLASH_COMMAND_READ_2ND                      0x30UL
#define D_PD_NAND_FLASH_COMMAND_READ_RANDOM_1ST               0x05UL
#define D_PD_NAND_FLASH_COMMAND_READ_RANDOM_2ND               0xE0UL

#define D_PD_NAND_FLASH_COMMAND_WRITE_1ST                     0x80UL
#define D_PD_NAND_FLASH_COMMAND_WRITE_2ND                     0x10UL
#define D_PD_NAND_FLASH_COMMAND_WRITE_RANDOM                  0x85UL

#define D_PD_NAND_FLASH_COMMAND_COPY_BACK_1ST                 0x00UL
#define D_PD_NAND_FLASH_COMMAND_COPY_BACK_2ND                 0x35UL
#define D_PD_NAND_FLASH_COMMAND_COPY_BACK_3RD                 0x85UL
#define D_PD_NAND_FLASH_COMMAND_COPY_BACK_4TH                 0x10UL

#define D_PD_NAND_FLASH_COMMAND_ERASE_1ST                     0x60UL
#define D_PD_NAND_FLASH_COMMAND_ERASE_2ND                     0xD0UL

#define D_PD_NAND_FLASH_COMMAND_RESET                         0xFFUL
#define D_PD_NAND_FLASH_COMMAND_ID_READ                       0x90UL
#define D_PD_NAND_FLASH_COMMAND_STATUS_READ                   0x70UL

// - NAND flash command set fetched from ST data sheet. (E.g. NAND01G-B2B)

#define D_PD_NAND_FLASH_COMMAND_READ_CACHE_1ST                0x00UL
#define D_PD_NAND_FLASH_COMMAND_READ_CACHE_2ND                0x31UL
#define D_PD_NAND_FLASH_COMMAND_READ_CACHE_EXIT               0x34UL
#define D_PD_NAND_FLASH_COMMAND_WRITE_CACHE_1ST               0x80UL
#define D_PD_NAND_FLASH_COMMAND_WRITE_CACHE_2ND               0x15UL

// - NAND flash command set fetched from SAMSUNG data sheet. (E.g. K9F2G08R0A).

#define D_PD_NAND_FLASH_COMMAND_WRITE_TWO_PLANE_1ST           0x80UL
#define D_PD_NAND_FLASH_COMMAND_WRITE_TWO_PLANE_2ND           0x11UL
#define D_PD_NAND_FLASH_COMMAND_WRITE_TWO_PLANE_3RD           0x81UL
#define D_PD_NAND_FLASH_COMMAND_WRITE_TWO_PLANE_4TH           0x10UL

#define D_PD_NAND_FLASH_COMMAND_COPY_BACK_TWO_PLANE_1ST       0x85UL
#define D_PD_NAND_FLASH_COMMAND_COPY_BACK_TWO_PLANE_2ND       0x11UL
#define D_PD_NAND_FLASH_COMMAND_COPY_BACK_TWO_PLANE_3RD       0x81UL
#define D_PD_NAND_FLASH_COMMAND_COPY_BACK_TWO_PLANE_4TH       0x10UL

#define D_PD_NAND_FLASH_COMMAND_STATUS_READ_EDC               0x7BUL

// -----------------------------------------------------------------------------

#define D_PD_NAND_AWADDR_CHIP_ADDRESS                     (1UL << 24)
#define D_PD_NAND_AWADDR_NBR_OF_ADDRESS_CYCLES            (1UL << 21)
#define D_PD_NAND_AWADDR_END_CMD_VALID                    (1UL << 20)
#define D_PD_NAND_AWADDR_AWADDR                           (0UL << 19)
#define D_PD_NAND_AWADDR_END_CMD                          (1UL << 11)
#define D_PD_NAND_AWADDR_START_CMD                        (1UL <<  3)

#define D_PD_NAND_ARADDR_CHIP_ADDRESS                     (1UL << 24)
#define D_PD_NAND_ARADDR_CLEAR_CS                         (1UL << 21)
#define D_PD_NAND_ARADDR_END_CMD_VALID                    (1UL << 20)
#define D_PD_NAND_ARADDR_ARADDR                           (1UL << 19)
#define D_PD_NAND_ARADDR_END_CMD                          (1UL << 11)
#define D_PD_NAND_ARADDR_ECC_LAST                         (1UL << 10)

// =============================================================================
//  Local Types
// =============================================================================

// - bool32: 32-bit boolean.
typedef uint32 bool32;

typedef uint8 PD_NAND_AddressCycles_t[D_PD_NAND_MAX_NBR_OF_ADDRESS_CYCLES];

// =============================================================================
//  Local Constants
// =============================================================================

#ifdef PRINT_B_
static const char C_PD_NAND_HEADER[]       = D_PD_NAND_HEADER;
static const char C_PD_NAND_HEADER_ERROR[] = D_PD_NAND_HEADER_ERROR;
#endif

static uint32 C_PD_NAND_EccDataLength[T_PD_NAND_ECC_PAGE_SIZE_NUMBER_OF_ITEMS] = { 256, 512 };

// =============================================================================
//  Local Variables
// =============================================================================

// - V_PD_NAND_MemoryProperties: Local copy of memconf boot record.
static PD_NAND_MemoryProperties_t V_PD_NAND_MemoryProperties;

// - V_PD_NAND_EccShadow: Stores ECC when using ReadPage and WritePage.
static uint32 V_PD_NAND_EccShadow[D_PD_NAND_MAX_PAGE_SIZE / D_PD_NAND_MIN_ECC_CALC_LENGTH];

// XVSZOAL : From CABS

static const unsigned char ECCLookup_t[256] = {
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
};

// =============================================================================
//  Local Function Declarations
// =============================================================================

#ifdef PRINT_A_
static PD_NAND_Result_t F_PD_NAND_CheckConfig(const PD_NAND_Config_t *const Config_p);
#endif

static PD_NAND_Result_t F_PD_NAND_Erase(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint32 NbrOfBlocks,
    uint32 *NbrOfErasedBlocks_p
);
static void F_PD_NAND_ExitNandPd(void);
static void F_PD_NAND_ReadData(const uint32 Size, uint32 *Data_p);
//static void F_PD_NAND_ReadData8(const uint32* const BaseAddress_p, const uint32 Size, uint8* Data8_p);
static PD_NAND_Result_t F_PD_NAND_ReadPage(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    uint8 *Data_p,
    uint8 *ExtraData_p
);
static PD_NAND_Result_t F_PD_NAND_SetConfig(const PD_NAND_Config_t *const Config_p);
static void F_PD_NAND_WriteData(const uint32 Size, const uint32 *const Data32_p);
//static void F_PD_NAND_WriteData8(uint32* const BaseAddress_p, const uint32 Size, const uint8* const Data8_p);
static PD_NAND_Result_t F_PD_NAND_WritePage(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint8 *const Data_p,
    const uint8 *const ExtraData_p,
    const uint32 Program
);
static void CalculateECC512(void *Page_p, unsigned char *ECC_p);
// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_Erase(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint32 NbrOfBlocks,
    uint32 *NbrOfErasedBlocks_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    B_(
        printf(
            "%s(%u) Do_PD_NAND_Erase(0x%08X, 0x%08X, %u, 0x%08X)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            PageOffset,
            NbrOfBlocks,
            NbrOfErasedBlocks_p
        );
    )

#ifdef PRINT_A_
    // - Check for error in parameters.
    {
        bool32 BadParameter = FALSE;

        BadParameter = BadParameter || (NbrOfErasedBlocks_p == NULL);
        BadParameter = BadParameter || (PageOffset >= V_PD_NAND_MemoryProperties.Size);
        BadParameter = BadParameter || ((PageOffset & (V_PD_NAND_MemoryProperties.BlockSize / V_PD_NAND_MemoryProperties.PageSize - 1)) != 0);

        if (BadParameter) {
            Result = T_PD_NAND_RESULT_BAD_PARAMETER;
            goto ErrorExit;
        }

        Result = F_PD_NAND_CheckConfig(Config_p);

        if (Result != T_PD_NAND_RESULT_SUCCESS) {
            goto ErrorExit;
        }

    }
#endif // PRINT_A_

    // - Set transfer configuration.
    Result = F_PD_NAND_SetConfig(Config_p);

    if (Result != T_PD_NAND_RESULT_SUCCESS) {
        goto ErrorExit;
    }

    Result =
        F_PD_NAND_Erase(
            Config_p,
            PageOffset,
            NbrOfBlocks,
            NbrOfErasedBlocks_p
        );

    if (Result != T_PD_NAND_RESULT_SUCCESS) {
        goto ErrorExit;
    }

ErrorExit:

    B_(
        printf(
            "%s(%u) Do_PD_NAND_ReadPage: \n",
            C_PD_NAND_HEADER,
            __LINE__
        );
    )

    F_PD_NAND_ExitNandPd();

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_GetIDs(
    const PD_NAND_Config_t *const Config_p,
    uint32 *ManufacturerId_p,
    uint32 *DeviceId_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    B_(
        printf(
            "%s(%u) Do_PD_NAND_GetIDs(0x%08X)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            ManufacturerId_p,
            DeviceId_p
        );
    )

#ifdef PRINT_A_
    // - Check for error in parameters.
    {
        bool32 BadParameter = FALSE;

        BadParameter = BadParameter || (Config_p == NULL);
        BadParameter = BadParameter || (ManufacturerId_p == NULL);
        BadParameter = BadParameter || (DeviceId_p == NULL);

        if (BadParameter) {
            Result = T_PD_NAND_RESULT_BAD_PARAMETER;
            goto ErrorExit;
        }

        Result = F_PD_NAND_CheckConfig(Config_p);

        if (Result != T_PD_NAND_RESULT_SUCCESS) {
            goto ErrorExit;
        }

    }
#endif // PRINT_A_

    // set flags in emullated memory
    NAND_Memory_Emul.ManufacturerID = DEFAULT_MANUFACTURER_ID;
    NAND_Memory_Emul.DeviceID = DEFAULT_DEVICE_ID;


    *ManufacturerId_p =  NAND_Memory_Emul.ManufacturerID;
    *DeviceId_p = NAND_Memory_Emul.DeviceID;
    Result = T_PD_NAND_RESULT_SUCCESS;

    // - To avoid warning when building without printouts:
    goto ErrorExit;

ErrorExit:

    B_(
        printf(
            "%s(%u) Do_PD_NAND_ReadPage: \n",
            C_PD_NAND_HEADER,
            __LINE__
        );
    )

    //F_PD_NAND_ExitNandPd();

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_GetMemoryProperties(const PD_NAND_Config_t *const Config_p, PD_NAND_MemoryProperties_t *MemoryProperties_p)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    B_(
        printf(
            "%s(%u) Do_PD_NAND_GetMemoryProperties(0x%08X, 0x%08X)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            MemoryProperties_p
        );
    )

#ifdef PRINT_A_
    {
        bool32 BadParameter = FALSE;

        BadParameter = BadParameter || (MemoryProperties_p == NULL);
        BadParameter = BadParameter || (V_PD_NAND_MemoryProperties.BusWidthInBits != 8) && (V_PD_NAND_MemoryProperties.BusWidthInBits != 16);
        // - Don't check RecordLength.
        BadParameter = BadParameter || (V_PD_NAND_MemoryProperties.Magic != 0xEDA96521);
        BadParameter = BadParameter || (V_PD_NAND_MemoryProperties.MemoryType != 1);
        BadParameter = BadParameter || (V_PD_NAND_MemoryProperties.Size < 32 * 1024 * 1024 / V_PD_NAND_MemoryProperties.PageSize); // - Sanity check.
        // - Don't check BlockSize;
        // - Don't check PageSize;
        // - Don't check PageExtraSize;
        // - Don't check NbrOfAddressCycles;

        if (BadParameter) {
            Result = T_PD_NAND_RESULT_BAD_PARAMETER;
            goto ErrorExit;
        }
    }
#endif

    *MemoryProperties_p = V_PD_NAND_MemoryProperties;

    Result = T_PD_NAND_RESULT_SUCCESS;

    // - To avoid warning when building without printouts:
    goto ErrorExit;

ErrorExit:

    F_PD_NAND_ExitNandPd();

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_ReadConsecutivePages(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint32 NbrOfPages,
    uint8 *Data_p,
    uint8 *ExtraData_p,
    uint32 *NbrOfReadPages_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    B_(
        printf(
            "%s(%u) Do_PD_NAND_ReadConsecutivePages(0x%08X, 0x%08X, %u, 0x%08X, 0x%08X, 0x%08X)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            PageOffset,
            NbrOfPages,
            Data_p,
            ExtraData_p,
            NbrOfReadPages_p
        );
    )

#ifdef PRINT_A_
    // - Check for error in parameters.
    {
        bool32 BadParameter = FALSE;

        BadParameter = BadParameter || (PageOffset >= V_PD_NAND_MemoryProperties.Size);
        BadParameter = BadParameter || (PageOffset + NbrOfPages >= V_PD_NAND_MemoryProperties.Size);
        BadParameter = BadParameter || (PageOffset + NbrOfPages < PageOffset);
        BadParameter = BadParameter || (NbrOfPages == 0);
        BadParameter = BadParameter || ((Data_p == NULL) && (ExtraData_p == NULL));
        BadParameter = BadParameter || (NbrOfReadPages_p == NULL);

        if (BadParameter) {
            Result = T_PD_NAND_RESULT_BAD_PARAMETER;
            goto ErrorExit;
        }

        Result = F_PD_NAND_CheckConfig(Config_p);

        if (Result != T_PD_NAND_RESULT_SUCCESS) {
            goto ErrorExit;
        }

    }
#endif // PRINT_A_

    // - To avoid warning when building without printouts:
    goto ErrorExit;

ErrorExit:

    F_PD_NAND_ExitNandPd();

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_ReadEcc(
    const PD_NAND_Config_t *const Config_p,
    PD_NAND_Ecc2kPage_t PD_NAND_Ecc2kPage_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_SUCCESS;
    uint32 i;
    uint32 m = V_PD_NAND_MemoryProperties.PageSize / C_PD_NAND_EccDataLength[Config_p->EccPageSize];
    uint32 Temp32;

    for (i = 0; i < m; i++) {
        Temp32 = V_PD_NAND_EccShadow[i];
        PD_NAND_Ecc2kPage_p[i][0] = (uint8) Temp32;
        Temp32 = Temp32 >> 8;
        PD_NAND_Ecc2kPage_p[i][1] = (uint8) Temp32;
        Temp32 = Temp32 >> 8;
        PD_NAND_Ecc2kPage_p[i][2] = (uint8) Temp32;
    }

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_ReadPage(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    uint8 *Data_p,
    uint8 *ExtraData_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    B_(
        printf(
            "%s(%u) Do_PD_NAND_ReadPage(0x%08X, 0x%08X, 0x%08X, 0x%08X)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            PageOffset,
            Data_p,
            ExtraData_p
        );
    )

#ifdef PRINT_A_
    // - Check for error in parameters.
    {
        bool32 BadParameter = FALSE;

        BadParameter = BadParameter || (Config_p == NULL);
        BadParameter = BadParameter || (PageOffset > V_PD_NAND_MemoryProperties.Size);
        BadParameter = BadParameter || ((Data_p == NULL) && (ExtraData_p == NULL));

        if (BadParameter) {
            Result = T_PD_NAND_RESULT_BAD_PARAMETER;
            goto ErrorExit;
        }

        Result = F_PD_NAND_CheckConfig(Config_p);

        if (Result != T_PD_NAND_RESULT_SUCCESS) {
            goto ErrorExit;
        }

    }
#endif // PRINT_A_

    F_PD_NAND_SetConfig(Config_p);

    Result =
        F_PD_NAND_ReadPage(
            Config_p,
            PageOffset,
            Data_p,
            ExtraData_p
        );

    if (Result != T_PD_NAND_RESULT_SUCCESS) {
        goto ErrorExit;
    }

ErrorExit:

    B_(
        printf(
            "%s(%u) Do_PD_NAND_ReadPage: \n",
            C_PD_NAND_HEADER,
            __LINE__
        );
    )

    F_PD_NAND_ExitNandPd();

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_ReadScatteredPages(
    const PD_NAND_Config_t *const Config_p,
    const uint32 *PageOffsetArray_p,
    const uint32 NbrOfPages,
    const uint8 **DataArray_pp,
    const uint8 **ExtraDataArray_pp,
    uint32 *NbrOfReadPages_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    B_(
        printf(
            "%s(%u) Do_PD_NAND_ReadScatteredPages(0x%08X, 0x%08X, %u, 0x%08X, 0x%08X, 0x%08X)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            PageOffsetArray_p,
            NbrOfPages,
            DataArray_pp,
            ExtraDataArray_pp,
            NbrOfReadPages_p
        );
    )

#ifdef PRINT_A_
    // - Check for error in parameters.
    {
        bool32 BadParameter = FALSE;

        BadParameter = BadParameter || (Config_p == NULL);
        BadParameter = BadParameter || (PageOffsetArray_p == NULL);
        BadParameter = BadParameter || ((DataArray_pp == NULL) && (ExtraDataArray_pp == NULL));
        BadParameter = BadParameter || (NbrOfReadPages_p == NULL);

        if (BadParameter) {
            Result = T_PD_NAND_RESULT_BAD_PARAMETER;
            goto ErrorExit;
        }

        Result = F_PD_NAND_CheckConfig(Config_p);

        if (Result != T_PD_NAND_RESULT_SUCCESS) {
            goto ErrorExit;
        }

    }
#endif // PRINT_A_

    // - To avoid warning when building without printouts:
    goto ErrorExit;

ErrorExit:

    F_PD_NAND_ExitNandPd();

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_SetMemoryProperties(const PD_NAND_Config_t *const Config_p, const PD_NAND_MemoryProperties_t *const MemoryProperties_p)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    //  (void)EMUL_ConfigInit();
    (void)NANDHW_Memory_Init(MemoryProperties_p);

    B_(
        printf(
            "%s(%u) Do_PD_NAND_SetMemoryProperties(0x%08X, 0x%08X)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            MemoryProperties_p
        );
    )

    {
        bool32 BadParameter = FALSE;

        BadParameter = BadParameter || (MemoryProperties_p == NULL);
        BadParameter = BadParameter || (MemoryProperties_p->BusWidthInBits != 8) && (MemoryProperties_p->BusWidthInBits != 16);
        // - Don't check RecordLength.
        BadParameter = BadParameter || (MemoryProperties_p->Magic != 0xEDA96521);
        BadParameter = BadParameter || (MemoryProperties_p->MemoryType != 1);
        //BadParameter = BadParameter || (MemoryProperties_p->Size < 32*1024*1024 / MemoryProperties_p->PageSize); // - Sanity check.
        // - Don't check BlockSize;
        // - Don't check PageSize;
        // - Don't check PageExtraSize;
        // - Don't check NbrOfAddressCycles;

        if (BadParameter) {
            Result = T_PD_NAND_RESULT_BAD_PARAMETER;
            goto ErrorExit;
        }
    }

    V_PD_NAND_MemoryProperties = *MemoryProperties_p;
    Result = T_PD_NAND_RESULT_SUCCESS;

ErrorExit:

    B_(
        printf(
            "%s(%u) Do_PD_NAND_ReadPage: \n",
            C_PD_NAND_HEADER,
            __LINE__
        );
    )

    F_PD_NAND_ExitNandPd();

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_WriteConsecutivePages(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint32 NbrOfPages,
    const uint8 *const Data_p,
    const uint8 *const ExtraData_p,
    uint32 *NbrOfWrittenPages_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    B_(
        printf(
            "%s(%u) Do_PD_NAND_WriteConsecutivePages(0x%08X, 0x%08X, %u, 0x%08X, 0x%08X, 0x%08X)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            PageOffset,
            NbrOfPages,
            Data_p,
            ExtraData_p,
            NbrOfWrittenPages_p
        );
    )

    F_PD_NAND_ExitNandPd();

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_WritePage(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint8 *const Data_p,
    const uint8 *const ExtraData_p,
    const uint32 Program
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    B_(
        printf(
            "%s(%u) Do_PD_NAND_WritePage(0x%08X, 0x%08X, 0x%08X, 0x%08X, %u)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            PageOffset,
            Data_p,
            ExtraData_p,
            Program
        );
    )

#ifdef PRINT_A_
    // - Check for error in parameters.
    {
        bool32 BadParameter = FALSE;

        BadParameter = BadParameter || (Config_p == NULL);
        BadParameter = BadParameter || (PageOffset > V_PD_NAND_MemoryProperties.Size);
        BadParameter = BadParameter || ((Data_p == NULL) && (ExtraData_p == NULL));

        if (BadParameter) {
            Result = T_PD_NAND_RESULT_BAD_PARAMETER;
            goto ErrorExit;
        }

        Result = F_PD_NAND_CheckConfig(Config_p);

        if (Result != T_PD_NAND_RESULT_SUCCESS) {
            goto ErrorExit;
        }

    }
#endif // PRINT_A_

    Result =
        F_PD_NAND_WritePage(
            Config_p,
            PageOffset,
            Data_p,
            ExtraData_p,
            Program
        );

    if (Result != T_PD_NAND_RESULT_SUCCESS) {
        goto ErrorExit;
    }

ErrorExit:

    B_(
        printf(
            "%s(%u) Do_PD_NAND_ReadPage: \n",
            C_PD_NAND_HEADER,
            __LINE__
        );
    )

    F_PD_NAND_ExitNandPd();

    return Result;
}

// -----------------------------------------------------------------------------

PD_NAND_Result_t Do_PD_NAND_WriteScatteredPages(
    const PD_NAND_Config_t *const Config_p,
    const uint32 *const PageOffsetArray_p,
    const uint32 NbrOfPages,
    const uint8 **const DataArray_pp,
    const uint8 **const ExtraDataArray_pp,
    uint32 *NbrOfWrittenPages_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    B_(
        printf(
            "%s(%u) Do_PD_NAND_WriteScatteredPages(0x%08X, 0x%08X, %u, 0x%08X, 0x%08X, 0x%08X)\n",
            C_PD_NAND_HEADER,
            __LINE__,
            Config_p,
            PageOffsetArray_p,
            NbrOfPages,
            DataArray_pp,
            ExtraDataArray_pp,
            NbrOfWrittenPages_p
        );
    )

    F_PD_NAND_ExitNandPd();

    return Result;
}

// =============================================================================
//  Local Function Definitions
// =============================================================================

// -----------------------------------------------------------------------------
//  Function    : CalculateECC512
//  Description : Calculates ECC for 512 bytes of data
//  Parameter   : Page_p : Data for wich will be calculate ECC
//  Return      :
//  Notes       : Added for nand emulator
// -----------------------------------------------------------------------------


static void CalculateECC512(void *Page_p, unsigned char *ECC_p)
{
    unsigned int sumCol = 0;
    unsigned int datum, temp;
    unsigned int glob_parity;
    const int ECC_N_BYTES = 512;
    int  i;
    unsigned char *data = (unsigned char *) Page_p;

    unsigned int   parit1_1, parit1_2, parit2_1, parit2_2, parit4_1, parit4_2;
    unsigned int   parit8_1 = 0,    parit8_2 = 0,    parit16_1 = 0,   parit16_2 = 0,   parit32_1 = 0,   parit32_2 = 0;
    unsigned int   parit64_1 = 0,   parit64_2 = 0,   parit128_1 = 0,  parit128_2 = 0,  parit256_1 = 0,  parit256_2 = 0;
    unsigned int   parit512_1 = 0,  parit512_2 = 0,  parit1024_1 = 0, parit1024_2 = 0, parit2048_1 = 0, parit2048_2 = 0;

    for (i = ECC_N_BYTES - 1; i >= 0; --i) {
        datum = data[i];
        sumCol ^= datum;
        temp = ECCLookup_t[datum];

        if (i & 0x01) {
            parit8_1    ^= temp;
        }

        if (i & 0x02) {
            parit16_1   ^= temp;
        }

        if (i & 0x04) {
            parit32_1   ^= temp;
        }

        if (i & 0x08) {
            parit64_1   ^= temp;
        }

        if (i & 0x10) {
            parit128_1  ^= temp;
        }

        if (i & 0x20) {
            parit256_1  ^= temp;
        }

        if (i & 0x40) {
            parit512_1  ^= temp;
        }

        if (i & 0x80) {
            parit1024_1 ^= temp;
        }

        if (i & 0x100) {
            parit2048_1 ^= temp;
        }
    }

    glob_parity = ECCLookup_t[sumCol];

    /* Column parity */
    parit1_1  = ((sumCol >> 1) ^(sumCol >> 3) ^(sumCol >> 5) ^(sumCol >> 7)) & 1;
    parit1_2  = ((sumCol >> 0) ^(sumCol >> 2) ^(sumCol >> 4) ^(sumCol >> 6)) & 1;
    parit2_1  = ((sumCol >> 2) ^(sumCol >> 3) ^(sumCol >> 6) ^(sumCol >> 7)) & 1;
    parit2_2  = ((sumCol >> 0) ^(sumCol >> 1) ^(sumCol >> 4) ^(sumCol >> 5)) & 1;
    parit4_1  = ((sumCol >> 4) ^(sumCol >> 5) ^(sumCol >> 6) ^(sumCol >> 7)) & 1;
    parit4_2  = ((sumCol >> 0) ^(sumCol >> 1) ^(sumCol >> 2) ^(sumCol >> 3)) & 1;

    /* Row parity */
    parit8_2    = glob_parity ^ parit8_1;
    parit16_2   = glob_parity ^ parit16_1;
    parit32_2   = glob_parity ^ parit32_1;
    parit64_2   = glob_parity ^ parit64_1;
    parit128_2  = glob_parity ^ parit128_1;
    parit256_2  = glob_parity ^ parit256_1;
    parit512_2  = glob_parity ^ parit512_1;
    parit1024_2 = glob_parity ^ parit1024_1;
    parit2048_2 = glob_parity ^ parit2048_1;

    /* Pack bits */
    ECC_p[0] = ~((parit64_1  << 7) | (parit64_2  << 6) | (parit32_1 << 5) | (parit32_2 << 4) | (parit16_1 << 3) | (parit16_2 << 2) | (parit8_1   << 1) | parit8_2);
    ECC_p[1] = ~((parit1024_1 << 7) | (parit1024_2 << 6) | (parit512_1 << 5) | (parit512_2 << 4) | (parit256_1 << 3) | (parit256_2 << 2) | (parit128_1 << 1) | parit128_2);
    ECC_p[2] = ~((parit4_1   << 7) | (parit4_2   << 6) | (parit2_1  << 5) | (parit2_2  << 4) | (parit1_1  << 3) | (parit1_2  << 2) | (parit2048_1 << 1) | parit2048_2);
}

// -----------------------------------------------------------------------------
//  Function    : F_PD_NAND_CheckConfig
//  Description : Checks Config_p for errors. Returns T_PD_NAND_RESULT_BAD_PARAMETER
//                if faulty parameter was found.
//  Parameter   : Config_p : Configuration struct to be checked.
//  Return      : Returns T_PD_NAND_RESULT_BAD_PARAMETER if faulty parameter was found.
//  Notes       : -
// -----------------------------------------------------------------------------

#ifdef PRINT_A_

static PD_NAND_Result_t F_PD_NAND_CheckConfig(const PD_NAND_Config_t *const Config_p)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_SUCCESS;
    bool32 BadParameter = FALSE;
    //  uint32 EccDataLength = C_PD_NAND_EccDataLength[Config_p->EccPageSize];
    //  uint32 EccLength = (V_PD_NAND_MemoryProperties.PageSize / EccDataLength) * D_PD_NAND_MAX_ECC_LENGTH;

    BadParameter = BadParameter || (Config_p->ChipSelect >= D_PD_NAND_NBR_OF_CHIP_SELECTS);
    BadParameter = BadParameter || (Config_p->EccPageSize >= T_PD_NAND_ECC_PAGE_SIZE_NUMBER_OF_ITEMS);
    //  BadParameter = BadParameter || (Config_p->ExtraDataLength >= V_PD_NAND_MemoryProperties.PageExtraSize - EccLength);
    BadParameter = BadParameter || (Config_p->Flags >= T_PD_NAND_CONFIG_FLAG_UNDEFINED);

    if (BadParameter) {
        Result = T_PD_NAND_RESULT_BAD_PARAMETER;
    }

    return Result;
}

#endif // PRINT_A_

// -----------------------------------------------------------------------------
//  Function    : F_PD_NAND_ExitNandPd
//  Description : Must be called last in each Do_PD_NAND function.
//  Parameter   : -
//  Return      : -
//  Notes       : -
// -----------------------------------------------------------------------------

static void F_PD_NAND_ExitNandPd()
{
    int a = 10; //just to make it exist
    (void)(a);
    // return T_PD_NAND_RESULT_SUCCESS;
}

// -----------------------------------------------------------------------------
//  Function    : F_PD_NAND_SetConfig
//  Description : ...
//  Parameter   : -
//  Return      : -
//  Notes       : -
// -----------------------------------------------------------------------------

static PD_NAND_Result_t F_PD_NAND_SetConfig(const PD_NAND_Config_t *const Config_p)
{

    return T_PD_NAND_RESULT_SUCCESS;

}

// -----------------------------------------------------------------------------
//  Function    : F_PD_NAND_ReadData32
//  Description : ...
//  Parameter   : -
//  Return      : -
//  Notes       : -
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static PD_NAND_Result_t F_PD_NAND_Erase(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint32 NbrOfBlocks,
    uint32 *NbrOfErasedBlocks_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    uint32 EccDataLength = C_PD_NAND_EccDataLength[Config_p->EccPageSize];
    uint32 m = V_PD_NAND_MemoryProperties.PageSize / EccDataLength;
    uint32 *Data_p;
    uint32 *ExtraData_p;
    uint32 i;
    uint32 PageNum;
    uint32 PD_NAND_Pages;

    Data_p = (uint32 *)malloc(V_PD_NAND_MemoryProperties.PageSize);
    ASSERT(NULL != Data_p);
    ExtraData_p = (uint32 *)malloc(V_PD_NAND_MemoryProperties.PageExtraSize);
    ASSERT(NULL != ExtraData_p);

    memset(Data_p, 0xff, V_PD_NAND_MemoryProperties.PageSize);
    memset(ExtraData_p, 0xff, V_PD_NAND_MemoryProperties.PageExtraSize);

    //  Initialisation of NAND memory
    //    NANDHW_Memory_Init();
    PD_NAND_Pages = PageOffset + V_PD_NAND_MemoryProperties.BlockSize / V_PD_NAND_MemoryProperties.PageSize * NbrOfBlocks;

    //    PD_NAND_Pages = V_PD_NAND_MemoryProperties.Size / V_PD_NAND_MemoryProperties.PageSize;
    for (PageNum = PageOffset; PageNum < PD_NAND_Pages; PageNum++) {
        NANDHW_Memory_SetBlock_p(PageNum);

        for (i = 0; i < m; i++) {
            F_PD_NAND_WriteData(EccDataLength, Data_p);
        }

        NANDHW_Memory_SetRedundantArea_p(PageNum);
        F_PD_NAND_WriteData(V_PD_NAND_MemoryProperties.PageExtraSize, ExtraData_p);
    }

    *NbrOfErasedBlocks_p = (V_PD_NAND_MemoryProperties.PageSize * (PageNum - PageOffset)) / V_PD_NAND_MemoryProperties.BlockSize;

    if (*NbrOfErasedBlocks_p != NbrOfBlocks) {
        Result = T_PD_NAND_RESULT_BAD_PARAMETER;
    } else {
        Result = T_PD_NAND_RESULT_SUCCESS;
    }

    return Result;
}


static void F_PD_NAND_ReadData(const uint32 Size, uint32 *Data_p)
{
    switch (Size) {
    case 16: {
        memcpy(Data_p, NAND_Memory_Emul.RedundantArea_p, Size);
        NAND_Memory_Emul.RedundantArea_p += Size;
        NAND_Memory_Emul.ECC_p += Size;
    }
    break;
    case 64: {
        memcpy(Data_p, NAND_Memory_Emul.RedundantArea_p, Size);
    }
    break;
    case 512: {
        memcpy(Data_p, NAND_Memory_Emul.Block_p, Size);
        NAND_Memory_Emul.Block_p += Size;
        NAND_Memory_Emul.ECC_p += Size;
    }
    break;
    default: {
        A_(printf("pd_nand.c (%d): **Error: Size not supported, Size = %d**\n", __LINE__, Size);)
    }
    }
}


// -----------------------------------------------------------------------------
//  Function    : F_PD_NAND_WriteData32
//  Description : ...
//  Parameter   : -
//  Return      : -
//  Notes       : -
// -----------------------------------------------------------------------------

static void F_PD_NAND_WriteData(const uint32 Size, const uint32 *const Data32_p)
{
    switch (Size) {
    case 16: {
        memcpy(NAND_Memory_Emul.RedundantArea_p, Data32_p, Size);
        NAND_Memory_Emul.Block_p += Size;
        NAND_Memory_Emul.ECC_p += Size;
    }
    break;
    case 64: {
        memcpy(NAND_Memory_Emul.RedundantArea_p, Data32_p, Size);
    }
    break;
    case 512: {
        memcpy(NAND_Memory_Emul.Block_p, Data32_p, Size);
        NAND_Memory_Emul.Block_p += Size;
        NAND_Memory_Emul.ECC_p += Size;
    }
    break;

    default: {
        A_(printf("pd_nand.c (%d): **Error: Size not supported, Size = %d**\n", __LINE__, Size);)
    }
    }
}



// =============================================================================
//  Global Function Definitions
// =============================================================================

// -----------------------------------------------------------------------------

static PD_NAND_Result_t F_PD_NAND_WritePage(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint8 *const Data_p,
    const uint8 *const ExtraData_p,
    const uint32 Program
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    // - Read data in chunks of ECC calculation length (normally 256 or 512 B), with 8 or 16 bits reads.
    uint32 EccDataLength = C_PD_NAND_EccDataLength[Config_p->EccPageSize];
    uint32 m = V_PD_NAND_MemoryProperties.PageSize / EccDataLength;
    uint8 *Data8_p = (uint8 *) Data_p;
    uint32 i;
    uint8 Temp32[3];

    //  NANDHW_Memory_Init();

    if (Data_p != NULL) {
        NANDHW_Memory_SetBlock_p(PageOffset);

        for (i = 0; i < m; i++) {

            F_PD_NAND_WriteData(EccDataLength, (uint32 *)Data8_p);

            CalculateECC512((uint32 *)Data8_p, Temp32);
            V_PD_NAND_EccShadow[i] = 0xff000000;

            V_PD_NAND_EccShadow[i] = V_PD_NAND_EccShadow[i] |
                                     ((Temp32[0] << 0) |
                                      (Temp32[1] << 8) |
                                      (Temp32[2] << 16)
                                     );
            Data8_p += EccDataLength;
        }
    }

    if (ExtraData_p != NULL) {
        NANDHW_Memory_SetRedundantArea_p(PageOffset);
        F_PD_NAND_WriteData(V_PD_NAND_MemoryProperties.PageExtraSize, (uint32 *) ExtraData_p);
    }

    Result = T_PD_NAND_RESULT_SUCCESS;

    return Result;
}

static PD_NAND_Result_t F_PD_NAND_ReadPage(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    uint8 *Data_p,
    uint8 *ExtraData_p
)
{
    PD_NAND_Result_t Result = T_PD_NAND_RESULT_UNDEFINED;

    // - Read data in chunks of ECC calculation length (normally 256 or 512 B), with 8 or 16 bits reads.
    {
        uint32 EccDataLength = C_PD_NAND_EccDataLength[Config_p->EccPageSize];
        uint32 m = V_PD_NAND_MemoryProperties.PageSize / EccDataLength;
        uint8 *Data8_p = Data_p;
        uint8 Temp32[3];
        uint32 i;

        //    NANDHW_Memory_Init();

        if (Data_p != NULL) {
            NANDHW_Memory_SetBlock_p(PageOffset);

            for (i = 0; i < m; i++) {

                F_PD_NAND_ReadData(EccDataLength, (uint32 *)Data8_p);

                CalculateECC512((uint32 *)Data8_p, Temp32);
                V_PD_NAND_EccShadow[i] = 0xff000000;

                V_PD_NAND_EccShadow[i] = V_PD_NAND_EccShadow[i] |
                                         ((Temp32[0] << 0) |
                                          (Temp32[1] << 8) |
                                          (Temp32[2] << 16)
                                         );

                Data8_p += EccDataLength;
            }
        }

        if (ExtraData_p != NULL) {
            NANDHW_Memory_SetRedundantArea_p(PageOffset);

            F_PD_NAND_ReadData(V_PD_NAND_MemoryProperties.PageExtraSize, (uint32 *)ExtraData_p);
        }
    }

    Result = T_PD_NAND_RESULT_SUCCESS;

    return Result;
}




