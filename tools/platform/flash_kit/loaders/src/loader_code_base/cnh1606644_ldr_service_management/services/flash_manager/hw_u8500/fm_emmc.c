/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @addtogroup ldr_service_fpd Flash Physical Driver
 * @{
 *   @addtogroup ldr_service_fpd_hw_u8500
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "r_basicdefinitions.h"
#include "r_service_management.h"
#include "fm_emmc.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "memmap_physical.h"
#include "hcl_defs.h"
#include "services.h"
#include "memory_mapping.h"
#include "mmc.h"
#include "r_time_utilities.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define MIN_CLOCK_DIV   46
#define MAX_CLOCK_DIV   0
#define MAX_BLOCK_SIZE  512
#define BOOT_INFO_ADDR  0x80151FFC
#define TRY_N_TIMES 5
#define WAIT_TIME 10

// eMMC Card Specific Data according to Jedec Standard no. 84-A44
BitRange_t CSD_BitMap [MAX_BITS] = {
    {127, 126}, // CSD_STRUCTURE
    {125, 122}, // SPEC_VERS
    {121, 120}, // RESERVED_3
    {119, 112}, // TAAC
    {111, 104}, // NSAC
    {103, 96}, // TRAN_SPEED
    {95, 84},  // CCC
    {83, 80},  // READ_BL_LEN
    {79, 79},  // READ_BL_PARTIAL
    {78, 78},  // WRITE_BLK_MISALIGN
    {77, 77},  // READ_BLK_MISALIGN
    {76, 76},  // DSR_IMP
    {75, 74},  // RESERVED_2
    {73, 62},  // C_SIZE
    {61, 59},  // VDD_R_CURR_MIN
    {58, 56},  // VDD_R_CURR_MAX
    {55, 53},  // VDD_W_CURR_MIN
    {52, 50},  // VDD_W_CURR_MAX
    {49, 47},  // C_SIZE_MULT
    {46, 42},  // ERASE_GRP_SIZE
    {41, 37},  // ERASE_GRP_MULT
    {36, 32},  // WP_GRP_SIZE
    {31, 31},  // WP_GRP_ENABLE
    {30, 29},  // DEFAULT_ECC
    {28, 26},  // R2W_FACTOR
    {25, 22},  // WRITE_BL_LEN
    {21, 21},  // WRITE_BL_PARTIAL
    {20, 17},  // RESERVED_1
    {16, 16},  // CONTENT_PROT_APP
    {15, 15},  // FILE_FORMAT_GRP
    {14, 14},  // COPY
    {13, 13},  // PERM_WRITE_PROTECT
    {12, 12},  // TMP_WRITE_PROTECT
    {11, 10},  // FILE_FORMAT
    {9, 8},    // ECC
    {7, 1},    // CRC
    {0, 0}     // NOT_USED
};

typedef struct {
    boolean Enabled;
    uint8 DeviceId;
    uint8 MMCServiceMask;
} MMC_Device_t;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static uint32 CSD_GetValue(CSD_eMMCBits_t bits, uint32 *csd);
static uint32 CalcMMCSize(uint32 *csd, uint32 *extcsd, uint64 *ext_size);
static uint32 TwoPwrN(uint32 N);
static void GetMMCBootOrdered(const FLASH_DEVICE_t FlashDevice, MMC_Device_t *Device);
static boolean IsCardOKTotest(t_mmc_device_id FlashDevice);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * This function initialize FPD_Config_t structure with flash memory properties
 * from FPD.
 *
 * @param [in]   FlashDevice      Flash device number
 *
 * @return       @ref E_SUCCESS   Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
void Do_EMMC_Init(const FLASH_DEVICE_t FlashDevice, FPD_Config_t *const FPD_Config_p)
{
    t_mmc_error  err = MMC_ERROR;
    MMC_Device_t MMCDevice;
    t_mmc_card_info card_info;
    uint64 ext_size = 0;
    uint32 g_mmc_extcsd[128];
    uint32 ReadBlLen  = 0;
    uint32 WriteBlLen = 0;
    uint8  ErasedMemContent = 0;

    memset(FPD_Config_p, 0x00, sizeof(FPD_Config_t));
    FPD_Config_p->Configuration_Set  = FALSE;
    FPD_Config_p->Loader_Flash_Mem_Type = MEM_TYPE_NONE;

    GetMMCBootOrdered(FlashDevice, &MMCDevice);

    if (TRUE == MMCDevice.Enabled) {
        FPD_Config_p->DeviceId = MMCDevice.DeviceId;
        SER_MMC_Init(MMCDevice.MMCServiceMask);
    } else {
        A_(printf("Flash device %u not supported\n", FlashDevice);)
        goto ErrorExit;
    }

    //  UTIL_MMC_GetCardInitialInfo function calls:
    TRY_N_WAIT(TRY_N_TIMES, WAIT_TIME, MMC_OK, err, MMC_PowerON, FPD_Config_p->DeviceId)

    if (MMC_OK != err) {
        A_(printf("Device %d MMC_PowerON %d\n", FPD_Config_p->DeviceId, err);)
        goto ErrorExit;
    }

    TRY_N_WAIT(TRY_N_TIMES, WAIT_TIME, MMC_OK, err, MMC_InitializeCards, FPD_Config_p->DeviceId)

    if (MMC_OK != err) {
        A_(printf("MMC_InitializeCards %d\n", err);)
        goto ErrorExit;
    }

    TRY_N_WAIT(TRY_N_TIMES, WAIT_TIME, MMC_OK, err, MMC_GetCardInfo, 1, FPD_Config_p->DeviceId, &card_info)

    if (MMC_OK != err) {
        A_(printf("MMC_GetCardInfo %d\n", err);)
        goto ErrorExit;
    }

    TRY_N_WAIT(TRY_N_TIMES, WAIT_TIME, MMC_OK, err, MMC_GetExtCSD, 1, g_mmc_extcsd, FPD_Config_p->DeviceId)

    if (MMC_OK != err) {
        A_(printf("MMC_GetExtCSD %d\n", err);)
        goto ErrorExit;
    }

    switch (card_info.card_type) {
    case MMC_MULTIMEDIA_CARD:
        B_(printf("MMC_MULTIMEDIA_CARD\n");)
        break;
    case MMC_SECURE_DIGITAL_CARD:
        B_(printf("MMC_SECURE_DIGITAL_CARD\n");)
        break;
    case MMC_SECURE_DIGITAL_IO_CARD:
        B_(printf("MMC_SECURE_DIGITAL_IO_CARD\n");)
        break;
    case MMC_SECURE_DIGITAL_IO_COMBO_CARD:
        B_(printf("MMC_SECURE_DIGITAL_IO_COMBO_CARD\n");)
        break;
    case MMC_HIGH_CAPACITY_SD_CARD:
        B_(printf("MMC_HIGH_CAPACITY_SD_CARD\n");)
        break;
    case MMC_HIGH_SPEED_MULTIMEDIA_CARD:
        B_(printf("MMC_HIGH_SPEED_MULTIMEDIA_CARD\n");)
        /* Fall through */
    case MMC_HIGH_CAPACITY_MMC_CARD:
        B_(printf("MMC_HIGH_CAPACITY_MMC_CARD\n");)

        FPD_Config_p->PageExtraSize      = 0; // Not applicable for eMMC
        FPD_Config_p->BusWidth           = 0; // Not applicable for eMMC
        FPD_Config_p->NbrOfAddressCycles = 0; // Not applicable for eMMC
        FPD_Config_p->Loader_Flash_Mem_Type = MEM_TYPE_EMMC;
        FPD_Config_p->Configuration_Set  = TRUE;

        // eMMC Card Specific Data according to Jedec Standard no. 84-A44
        // The data block length is computed as 2READ_BL_LEN
        ReadBlLen = TwoPwrN(CSD_GetValue(READ_BL_LEN, card_info.csd));
        C_(printf("flash_manager.c (%d) Read BL LEN %d\n", __LINE__, ReadBlLen);)
        WriteBlLen = TwoPwrN(CSD_GetValue(WRITE_BL_LEN, card_info.csd));
        C_(printf("flash_manager.c (%d) Write BL LEN %d\n", __LINE__, WriteBlLen);)
        FPD_Config_p->PageSize = MIN(ReadBlLen, WriteBlLen);
        C_(printf("flash_manager.c (%d) PageSize %d\n", __LINE__, FPD_Config_p->PageSize);)

        // size of erasable unit = (ERASE_GRP_SIZE + 1) * (ERASE_GRP_MULT + 1)
        FPD_Config_p->EraseBlockSize = (CSD_GetValue(ERASE_GRP_SIZE, card_info.csd) + 1)
                                       * (CSD_GetValue(ERASE_GRP_MULT, card_info.csd) + 1)
                                       * FPD_Config_p->PageSize;

        FPD_Config_p->DeviceSize     = CalcMMCSize(card_info.csd, g_mmc_extcsd, &ext_size);

        if ((0 == FPD_Config_p->DeviceSize) && (0 != ext_size)) {
            FPD_Config_p->DeviceSize = ext_size;
        } else if (0 == FPD_Config_p->DeviceSize) {
            err = MMC_INTERNAL_ERROR;
            A_(printf("flash_manager.c (%d)** ERROR %d **\n", __LINE__, err);)
            goto ErrorExit;
        }

        ErasedMemContent = ((uint8 *)g_mmc_extcsd)[ECSD_ERASED_MEM_CONT_BYTE_OFFSET];

        if (0 == ErasedMemContent) {
            FPD_Config_p->ErasedMemContent = 0x00;
        } else {
            FPD_Config_p->ErasedMemContent = 0xFF;
        }

        /*
         * After setting Enhanced Area, HC card can have size less then 2GB
         * Add new boolean parameter, HC TRUE or FALSE
         */
        if (0xfff == CSD_GetValue(C_SIZE, card_info.csd)) {
            FPD_Config_p->HC_Card = TRUE;
        } else {
            FPD_Config_p->HC_Card = FALSE;
        }

        /* Write Protected Group Size */
        FPD_Config_p->WriteProtectedSize = ((uint8 *)g_mmc_extcsd)[ECSD_HC_WP_GRP_SIZE] * ((uint8 *)g_mmc_extcsd)[ECSD_HC_ERASE_GRP_SIZE] * 512 * 1024;

        FPD_Config_p->RpmbSizeMult = ((uint8 *)g_mmc_extcsd)[RPMB_SIZE_MULT];

        FPD_Config_p->RelWrSecC = ((uint8 *)g_mmc_extcsd)[REL_WR_SEC_C];

        /*
         * Check if Enhanced Area is set and read start and end addresses
         */
        if ((((uint8 *)g_mmc_extcsd)[ECSD_PARTITIONS_ATTRIBUTE] & 0x1) == 0x1) {
            FPD_Config_p->EnhancedAreaStart = g_mmc_extcsd[ECSD_ENH_START_ADDR / 4];

            if (FPD_Config_p->HC_Card) {
                FPD_Config_p->EnhancedAreaStart *= FPD_Config_p->PageSize;
            }

            FPD_Config_p->EnhancedAreaEnd = g_mmc_extcsd[ECSD_ENH_SIZE_MULT / 4] & 0x00FFFFFF;
            FPD_Config_p->EnhancedAreaEnd = FPD_Config_p->EnhancedAreaEnd * FPD_Config_p->WriteProtectedSize + FPD_Config_p->EnhancedAreaStart - 1;

            C_(printf("fm_emmc.c: EnhancedAreaStart 0x%08X%08X\n", (uint32)(FPD_Config_p->EnhancedAreaStart >> 32), (uint32)FPD_Config_p->EnhancedAreaStart);)
            C_(printf("fm_emmc.c: EnhancedAreaEnd   0x%08X%08X\n", (uint32)(FPD_Config_p->EnhancedAreaEnd >> 32), (uint32)FPD_Config_p->EnhancedAreaEnd);)
        }

        break;
    default:
        A_(printf("default, not supported\n");)
        break;
    }

    TRY_N_WAIT(TRY_N_TIMES, WAIT_TIME, MMC_OK, err, MMC_SetDeviceMode, MMC_POLLING_MODE, FPD_Config_p->DeviceId)

    if (MMC_OK != err) {
        A_(printf("MMC_SetDeviceMode %d\n", err);)
        goto ErrorExit;
    }

    TRY_N_WAIT(TRY_N_TIMES, WAIT_TIME, MMC_OK, err, MMC_SetClockFrequency, MAX_CLOCK_DIV, FPD_Config_p->DeviceId)

    if (MMC_OK != err) {
        A_(printf("MMC_SetClockFrequency %d\n", err);)
        goto ErrorExit;
    }

    Sleep(10);
    TRY_N_WAIT(TRY_N_TIMES, WAIT_TIME, MMC_OK, err, MMC_EnableHWFlowControl, MMC_ENABLE, FPD_Config_p->DeviceId)

    if (MMC_OK != err) {
        A_(printf("MMC_EnableHWFlowControl %d\n", err);)
        goto ErrorExit;
    }

    TRY_N_WAIT(TRY_N_TIMES, WAIT_TIME, MMC_OK, err, MMC_SetHighSpeedModeTiming, 1, MMC_ENABLE, FPD_Config_p->DeviceId)

    if (MMC_OK != err) {
        A_(printf("MMC_SetHighSpeedModeTiming %d\n", err);)
        goto ErrorExit;
    }

#if STE_PLATFORM_NUM == 8500
    err = MMC_EnableWideBusOperation(1, MMC_8_BIT_WIDE, FPD_Config_p->DeviceId);
#else

    if (FPD_Config_p->DeviceId == 0) {
        err = MMC_EnableWideBusOperation(1, MMC_4_BIT_WIDE, FPD_Config_p->DeviceId);
    } else {
        err = MMC_EnableWideBusOperation(1, MMC_8_BIT_WIDE, FPD_Config_p->DeviceId);
    }

#endif

    if (MMC_OK != err) {
        A_(printf("MMC_EnableWideBusOperation %d\n", err);)
        goto ErrorExit;
    }

ErrorExit:
    FPD_Config_p->DeviceStatus = err;

    C_(printf("DeviceSize 0x%08X%08X = ", (uint32)(FPD_Config_p->DeviceSize >> 32), (uint32)(FPD_Config_p->DeviceSize));)
    C_(printf("%d MB\n", (uint32)(FPD_Config_p->DeviceSize >> 20));)
    C_(printf("PageSize %d, Erase %d, ", FPD_Config_p->PageSize, FPD_Config_p->EraseBlockSize);)
    C_(printf("eMMCStatus %d %d\n", err, FPD_Config_p->DeviceStatus);)
    C_(printf("WriteProtectedSize %d\n", FPD_Config_p->WriteProtectedSize);)

    return;
}

/**
 * This function initialize Physical driver, sets memory properties.
 *
 * @param [in]   FPD_Config_p     Flash device configuration
 *
 * @return       @ref E_SUCCESS   Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_Start(FPD_Config_t *FPD_Config_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (MMC_OK == FPD_Config_p->DeviceStatus) {
        ReturnValue = E_SUCCESS;
    }

    return ReturnValue;
}

/**
 * Physical driver can't be stop. This function need to exist for service
 * manager to work properly.
 *
 * @param [in]   FPD_Config_p     Flash device configuration
 *
 * @return       @ref E_SUCCESS   Success ending.
 */
ErrorCode_e Do_EMMC_Stop(FPD_Config_t *FPD_Config_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (MMC_OK == FPD_Config_p->DeviceStatus) {
        ReturnValue = E_SUCCESS;
    }

    return ReturnValue;
}

/**
 * Retrieves memory properties.
 *
 * @param [in]   FPD_Config_p    - Flash device configuration
 * @param [out]  Configuration_p - Pointer to configuration structure.
 * @return       @ref E_SUCCESS   Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_GetInfo(FPD_Config_t *FPD_Config_p, FPD_Config_t *Configuration_p)
{
    ErrorCode_e ReturnValue = E_SERVICE_NOT_SUPPORTED;

    if ((MMC_OK == FPD_Config_p->DeviceStatus) && FPD_Config_p->Configuration_Set) {
        Configuration_p->DeviceStatus          = FPD_Config_p->DeviceStatus;
        Configuration_p->DeviceId              = FPD_Config_p->DeviceId;
        Configuration_p->BusWidth              = FPD_Config_p->BusWidth;
        Configuration_p->DeviceSize            = FPD_Config_p->DeviceSize;
        Configuration_p->EraseBlockSize        = FPD_Config_p->EraseBlockSize;
        Configuration_p->WriteProtectedSize    = FPD_Config_p->WriteProtectedSize;
        Configuration_p->PageSize              = FPD_Config_p->PageSize;
        Configuration_p->PageExtraSize         = FPD_Config_p->PageExtraSize;
        Configuration_p->NbrOfAddressCycles    = FPD_Config_p->NbrOfAddressCycles;
        Configuration_p->Configuration_Set     = TRUE;
        Configuration_p->ErasedMemContent      = FPD_Config_p->ErasedMemContent;
        Configuration_p->Loader_Flash_Mem_Type = FPD_Config_p->Loader_Flash_Mem_Type;
        Configuration_p->HC_Card               = FPD_Config_p->HC_Card;
        Configuration_p->EnhancedAreaStart     = FPD_Config_p->EnhancedAreaStart;
        Configuration_p->EnhancedAreaEnd       = FPD_Config_p->EnhancedAreaEnd;
        Configuration_p->RpmbSizeMult          = FPD_Config_p->RpmbSizeMult;
        Configuration_p->RelWrSecC             = FPD_Config_p->RelWrSecC;
        ReturnValue = E_SUCCESS;
    }

    return ReturnValue;
}

/**
 * Retrieves memory properties.
 *
 * @param [in] FPD_Config_p   - Flash device configuration
 * @param [in] NewConfig_p    - Contains necessary information for
 *                              configuring the FPD.
 * @return     @ref E_SUCCESS - Success ending.
 *
 * @return     @ref E_INVALID_INPUT_PARAMETER - If NULL is passed.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_SetInfo(FPD_Config_t *const FPD_Config_p, FPD_Config_t *NewConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    VERIFY(NULL != NewConfig_p, E_INVALID_INPUT_PARAMETER);
    memcpy(FPD_Config_p, NewConfig_p, sizeof(FPD_Config_t));

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
 * @param [in]   FPD_Config_p   - Flash device configuration
 * @param [out]  Data_p         - Data buffer. May be NULL if extra data is to
 *                                be read.
 * @param [out]  ExtraData_p    - Data buffer. May be NULL if page data is to
 *                                be read.
 * @return       @ref E_SUCCESS  Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_ReadPage(const uint32 PageOffset,
                             FPD_Config_t *const FPD_Config_p,
                             uint8 *Data_p,
                             uint8 *ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    void *TempData_p = Data_p;
    t_uint32        no_of_blocks = 1;
    t_mmc_error     err = MMC_ERROR;

    IDENTIFIER_NOT_USED(ExtraData_p);

    // MMC interface requires uint32* for data, Data_p must be 32-bit aligned
    if ((uint32)Data_p & 3) {
        A_(printf("Do_FPD_ReadPage : Misaligned data pointer!!!\n");)

        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    if (MMC_OK == FPD_Config_p->DeviceStatus) {
        uint32 Address = PageOffset;

        // Jedec 84-A44 : Size < 2GB => byte addressing
        if (!FPD_Config_p->HC_Card) {
            Address *= MMC_ADDRESS_MODE_FACTOR;
        }

        // Typecast on Data_p OK, alignment checked above
        err = MMC_ReadBlocks(1, Address, (uint32 *)TempData_p, FPD_Config_p->PageSize, no_of_blocks, FPD_Config_p->DeviceId);

        if (MMC_OK != err) {
            // TODO Add proper error code for eMMC failure
            A_(printf("Do_FPD_ReadPage failed! err %d\n", err);)
            ReturnValue = E_GENERAL_FATAL_ERROR;
        } else {
            ReturnValue = E_SUCCESS;
        }
    }

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
 * @param [in]  FPD_Config_p   - Flash device configuration
 * @param [out] Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be read.
 * @param [out] ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be read.
 * @return      @ref E_SUCCESS - Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_ReadConsecutivePages(const uint32 PageOffset,
        const uint32 NbrOfPages,
        FPD_Config_t *const FPD_Config_p,
        uint8 *Data_p,
        uint8 *ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    /*
     * alignment warning fix
     * the alignment of Data_p is checked later in the code
     *
     */
    void *TempData_p = Data_p;
    t_mmc_error     err = MMC_ERROR;

    IDENTIFIER_NOT_USED(ExtraData_p);

    // MMC interface requires uint32* for data, Data_p must be 32-bit aligned
    if ((uint32)Data_p & 3) {
        A_(printf("Do_FPD_ReadConsecutivePages : Misaligned data pointer!!!\n");)

        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    if (MMC_OK == FPD_Config_p->DeviceStatus) {
        uint32 Address = PageOffset;

        // Jedec 84-A44 : Size < 2GB => byte addressing
        if (!FPD_Config_p->HC_Card) {
            Address *= MMC_ADDRESS_MODE_FACTOR;
        }

        // Typecast on Data_p OK, alignment checked above
        err = MMC_ReadBlocks(1, Address, (uint32 *)TempData_p, FPD_Config_p->PageSize, NbrOfPages, FPD_Config_p->DeviceId);

        if (MMC_OK != err) {
            // TODO Add proper error code for eMMC failure
            ReturnValue = E_GENERAL_FATAL_ERROR;
        } else {
            ReturnValue = E_SUCCESS;
        }
    }

ErrorExit:
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
 * @param [in]  FPD_Config_p   - Flash device configuration
 * @return      @ref E_SUCCESS - Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_WritePage(const uint32 PageOffset,
                              const uint8 *const Data_p,
                              const uint8 *const ExtraData_p,
                              FPD_Config_t *const FPD_Config_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    /*
     * alignment warning fix
     * the alignment of Data_p is checked later in the code
     *
     */
    const void *const TempData_p = Data_p;
    t_uint32        no_of_blocks = 1;
    t_mmc_error     err = MMC_ERROR;

    IDENTIFIER_NOT_USED(ExtraData_p);

    // MMC interface requires uint32* for data, Data_p must be 32-bit aligned
    if ((uint32)Data_p & 3) {
        A_(printf("Do_FPD_WritePage : Misaligned data pointer!!!\n");)

        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    if (MMC_OK == FPD_Config_p->DeviceStatus) {
        uint32 Address = PageOffset;

        // Jedec 84-A44 : Size < 2GB => byte addressing
        if (!FPD_Config_p->HC_Card) {
            Address *= MMC_ADDRESS_MODE_FACTOR;
        }

        // Typecast on Data_p OK, alignment checked above
        err = MMC_WriteBlocks(1, Address, (uint32 *)TempData_p, FPD_Config_p->PageSize, no_of_blocks, FPD_Config_p->DeviceId, FALSE);

        if (MMC_OK != err) {
            // TODO Add proper error code for eMMC failure
            ReturnValue = E_GENERAL_FATAL_ERROR;
        } else {
            ReturnValue = E_SUCCESS;
        }
    }

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
 * @param [in]  FPD_Config_p   - Flash device configuration
 * @return      @ref E_SUCCESS - Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_WriteConsecutivePages(const uint32 PageOffset,
        const uint32 NbrOfPages,
        const uint8 *const Data_p,
        const uint8 *const ExtraData_p,
        FPD_Config_t *const FPD_Config_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    /*
     * alignment warning fix
     * the alignment of Data_p is checked later in the code
     *
     */
    const void *const TempData_p = Data_p;
    t_mmc_error       err        = MMC_ERROR;

    IDENTIFIER_NOT_USED(ExtraData_p);

    // MMC interface requires uint32* for data, Data_p must be 32-bit aligned
    if ((uint32)Data_p & 3) {
        A_(printf("Do_FPD_WriteConsecutivePages : Misaligned data pointer!!!\n");)

        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    if (MMC_OK == FPD_Config_p->DeviceStatus) {
        uint32 Address = PageOffset;

        // Jedec 84-A44 : Size < 2GB => byte addressing
        if (!FPD_Config_p->HC_Card) {
            Address *= MMC_ADDRESS_MODE_FACTOR;
        }

        // Typecast on Data_p OK, alignment checked above
        err = MMC_WriteBlocks(1, Address, (uint32 *)TempData_p, FPD_Config_p->PageSize, NbrOfPages, FPD_Config_p->DeviceId, FALSE);

        if (MMC_OK != err) {
            // TODO Add proper error code for eMMC failure
            ReturnValue = E_GENERAL_FATAL_ERROR;
        } else {
            ReturnValue = E_SUCCESS;
        }
    }

ErrorExit:
    return ReturnValue;

}

/**
 * Erases one or several blocks in FPD.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfBlocks    - Number of blocks to erase.
 * @param [in]  FPD_Config_p   - Flash device configuration
 * @return      @ref E_SUCCESS - Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_Erase(uint32 PageOffset,
                          uint32 NbrOfBlocks,
                          FPD_Config_t *const FPD_Config_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    t_uint32        start_addr;
    t_uint32        end_addr;
    t_mmc_error     err = MMC_ERROR;

    if (MMC_OK == FPD_Config_p->DeviceStatus) {
        C_(printf("flash_manager.c: PageOffset 0x%X NbrOfBlocks 0x%X\n", PageOffset, NbrOfBlocks);)

        // Translate PageOffset to eMMC address

        // Jedec 84-A44 : Size < 2GB => byte addressing
        if (!FPD_Config_p->HC_Card) {
            start_addr = PageOffset * FPD_Config_p->PageSize;
            end_addr = start_addr + ((NbrOfBlocks * FPD_Config_p->EraseBlockSize) - 1);
        } else {
            start_addr = PageOffset;
            end_addr = start_addr + NbrOfBlocks * (FPD_Config_p->EraseBlockSize / FPD_Config_p->PageSize) - 1;
        }

        C_(printf("flash_manager.c: Start Address %d End Address %d\n", start_addr, end_addr);)

        err = MMC_Erase(1, start_addr, end_addr, FPD_Config_p->DeviceId);

        if (MMC_OK != err) {
            A_(printf("MCC_Erase error %d\n", err);)
            // TODO Add proper error code for eMMC failure
            ReturnValue = E_GENERAL_FATAL_ERROR;
        } else {
            ReturnValue = E_SUCCESS;
        }
    }

    return ReturnValue;
}

/**
 * Checks if a flash device is started.
 *
 * @param [in]  FPD_Config_p   - Flash device configuration
 * @return      boolean
 *
 */
boolean Do_EMMC_IsStarted(FPD_Config_t *const FPD_Config_p)
{
    return (FPD_Config_p->DeviceStatus == MMC_OK);
}

/*
 * Gets the EMMC partitions configurations and stores it in an internal structure
 */
ErrorCode_e Do_EMMC_GetPartitionsConfiguration(void)
{
    return E_SUCCESS;
}

/**
 * Defines Enhanced Area in flash memory.
 *
 * @param [in]  StartAddress   - Enhanced area start address in bytes.
 * @param [in]  AreaSize       - Enhanced area size in bytes.
 * @param [in]  FPD_Config_p   - Flash device configuration
 * @return      @ref E_SUCCESS - Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_SetEnhancedArea(uint64 StartAddress, uint64 AreaSize, FPD_Config_t *const FPD_Config_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    uint32 EnhAreaStart = 0;
    uint32 EnhAreaSize  = 0;
    t_mmc_error err = MMC_ERROR;

    if (MMC_OK == FPD_Config_p->DeviceStatus) {

        if (!FPD_Config_p->HC_Card) {
            EnhAreaStart = (uint32)StartAddress;
        } else {
            EnhAreaStart = StartAddress / FPD_Config_p->PageSize;
        }

        EnhAreaSize = AreaSize / FPD_Config_p->WriteProtectedSize;

        C_(
            printf("fm_emmc.c(%d): Call MMC_SetEnhancedConfig with params:\n", __LINE__);
            printf("EnhAreaStart: %d\n", EnhAreaStart);
            printf("EnhAreaSize: %d\n", EnhAreaSize);
            printf("DeviceId: %d\n", FPD_Config_p->DeviceId);
        )

        err = MMC_SetEnhancedConfig(EnhAreaStart, EnhAreaSize, 0, FPD_Config_p->DeviceId);

        if (MMC_OK != err) {
            A_(printf("fm_emmc.c(%d): MMC_SetEnhancedConfig error %d\n", __LINE__, err);)
            ReturnValue = E_SET_ENHANCED_AREA_FAILED;
        } else {
            ReturnValue = E_SUCCESS;
        }
    }

    return ReturnValue;
}

/**
 * Function for disable/enable HW reset.
 *
 *  @param [in] RegValue     - Value to be written in register (1- set, 0-reset)
 *  @param [in] FlashDevice  - Flash device number
 *  @return      Returns E_SUCCESS for success ending.
 *
 *  @return      Returns E_ENABLE_HW_RESET_FAILED for fail.
 *
 *  @return      Function can also return some internal MMC error codes.
 */
ErrorCode_e Do_EMMC_MMC_EnPowerReset(uint32 RegValue, FPD_Config_t *const FPD_Config_p)
{
    ErrorCode_e ReturnValue = E_ENABLE_HW_RESET_FAILED;
    t_mmc_error err = MMC_GENERAL_UNKNOWN_ERROR;

    err = MMC_EnPowerReset((t_mmc_state) RegValue, FPD_Config_p->DeviceId);

    if (MMC_OK != err) {
        A_(printf("fm_emmc.c(%d): Do_EMMC_MMC_EnPowerReset error %d\n", __LINE__, err);)
        ReturnValue = E_ENABLE_HW_RESET_FAILED;
    } else {
        ReturnValue = E_SUCCESS;
    }

    return ReturnValue;
}

/***********************************************************************//**
* @brief
*
* @return boolean
*
****************************************************************************/
boolean Do_FPD_BlockIsBad(uint32 BlockNr, FPD_Config_t *const FPD_Config_p)
{
    boolean ReturnValue = TRUE;

    if (MMC_OK == FPD_Config_p->DeviceStatus) {
        if (BlockNr < (FPD_Config_p->DeviceSize / FPD_Config_p->EraseBlockSize)) {
            /* There are no bad block for MMC */
            ReturnValue = FALSE;
        }
    }

    return ReturnValue;
}

void Do_FPD_MarkBlockAsBad(uint32 BlockNr)
{
    /* TODO: Implement marking that block is bad here */
}



static uint32 CSD_GetValue(CSD_eMMCBits_t bits, uint32 *csd)
{
    uint32 value;
    uint32 index_hi;
    uint32 index_lo;
    uint32 n_bits;
    uint32 n_bits_lo;
    uint32 n_bits_hi;
    uint32 shift;
    uint32 mask;
    uint32 mask_lo;
    uint32 ii;

    // Determine index in csd vector from bits parameter
    // hi/lo since they might overlap and need different treatment
    index_hi = CSD_BitMap[bits].hi / 32;
    index_lo = CSD_BitMap[bits].lo / 32;

    // Determine number of bits
    n_bits = (CSD_BitMap[bits].hi - CSD_BitMap[bits].lo) + 1;

    // Create mask for n_bits shifted
    for (mask = 0, ii = 0; ii < n_bits; ii++) {
        mask = mask << 1;
        mask += 1;
    }

    // If indexes are the same, all bits reside in one 32-bit word
    if (index_hi == index_lo) {
        shift = CSD_BitMap[bits].lo % 32;
        value = (csd[index_hi] >> shift) & mask;
    } else {
        // n_bits from lo and hi respectively
        n_bits_lo = 32 - (CSD_BitMap[bits].lo % 32);
        n_bits_hi = (CSD_BitMap[bits].hi % 32) + 1;

        // assign value and make room for lo bits
        value = csd[index_hi] << n_bits_lo;

        // Determine lo bits left shift
        shift = 32 - n_bits_lo;

        // Determine lo bits mask
        for (mask_lo = 0, ii = 0; ii < n_bits_lo; ii++) {
            mask_lo = mask_lo << 1;
            mask_lo += 1;
        }

        // Calc final value
        value = (value | ((csd[index_lo] >> shift) & mask_lo)) & mask;
    }

    return value;
}


static uint32 CalcMMCSize(uint32 *csd, uint32 *extcsd, uint64 *ext_size)
{
    uint32 size;
    uint32 c_size;
    uint32 c_size_mult;
    uint32 read_bl_len;

    c_size = CSD_GetValue(C_SIZE, csd);
    c_size_mult = CSD_GetValue(C_SIZE_MULT, csd);
    read_bl_len = CSD_GetValue(READ_BL_LEN, csd);

    B_(printf("c_size 0x%08x c_size_mult 0x%08x read_bl_len 0x%08x\n", c_size, c_size_mult, read_bl_len);)
    *ext_size = 0;

    if (c_size == 0xfff) { // > 2GB
        *ext_size = (uint64)extcsd[ECSD_SEC_COUNT_WORD_OFFSET / 4];
        *ext_size = (uint64)(*ext_size * 512);
        size = 0;
    } else {              // <= 2GB
        if ((c_size_mult < 8) && (read_bl_len < 12)) {
            // memory capacity = BLOCKNR * BLOCK_LEN where
            // BLOCKNR = (C_SIZE+1) * MULT
            // MULT = 2^C_SIZE_MULT+2 (C_SIZE_MULT < 8)
            // BLOCK_LEN = 2^READ_BL_LEN, (READ_BL_LEN < 12)

            size = ((c_size + 1) * TwoPwrN(c_size_mult + 2)) * TwoPwrN(read_bl_len);
            B_(printf("size = 0x%08x\n", size);)
        } else { // Invalid parameters!
            A_(printf("flash_manager.c (%d) *** INVALID PARAMETER ***\n", __LINE__);)
            size = 0;
        }
    }

    return size;

}

static uint32 TwoPwrN(uint32 N)
{
    uint32 pwr;
    uint32 ii;

    if (N > 31) {
        return 0;
    }

    for (pwr = 1, ii = 0; ii < N; ii++) {
        pwr *= 2;
    }

    return pwr;
}

static void GetMMCBootOrdered(const FLASH_DEVICE_t FlashDevice, MMC_Device_t *const Device)
{
    uint32 MMCInterfaceSelection;
    boolean BootExternalMMC = FALSE;

    // Read Boot Info
    MMCInterfaceSelection = *((uint32 *)BOOT_INFO_ADDR);

    // Check boot order
#ifdef ST_HREFV2

    if (2 == ((MMCInterfaceSelection >> 12) & 0x03)) {
#else

    if (1 == ((MMCInterfaceSelection >> 10) & 0x01)) {
#endif
        BootExternalMMC = TRUE;
    }

    B_(if (TRUE == BootExternalMMC) {
    printf("fm_emmc.c (%d): BootExternalMMC\n", __LINE__);
    } else {
        printf("fm_emmc.c (%d): BootInternalMMC\n", __LINE__);
    })

    // Get correct flash device
    Device->Enabled = FALSE;

#if STE_PLATFORM_NUM == 8500
#ifdef STE_AP8540
    Device->DeviceId = MMC_DEVICE_ID_4;
    Device->MMCServiceMask = INIT_MMC4;
#else

    if ((FALSE == BootExternalMMC && FLASH_0 == FlashDevice) || (TRUE == BootExternalMMC && FLASH_1 == FlashDevice)) {
        Device->DeviceId = MMC_DEVICE_ID_2;
        Device->MMCServiceMask = INIT_MMC2;
    } else {
        Device->DeviceId = MMC_DEVICE_ID_4;
        Device->MMCServiceMask = INIT_MMC4;
    }

#endif

#else

#ifdef SWITCH_EMMC_ORDER

    if (FLASH_0 == FlashDevice) {
        Device->DeviceId = MMC_DEVICE_ID_2;
        Device->MMCServiceMask = INIT_MMC2;
    } else {
        Device->DeviceId = MMC_DEVICE_ID_0;
        Device->MMCServiceMask = INIT_MMC0;
    }

#else

    if (FLASH_0 == FlashDevice) {
        Device->DeviceId = MMC_DEVICE_ID_0;
        Device->MMCServiceMask = INIT_MMC0;
    } else {
        Device->DeviceId = MMC_DEVICE_ID_2;
        Device->MMCServiceMask = INIT_MMC2;
    }

#endif

#endif

    if (TRUE == IsCardOKTotest(Device->DeviceId)) {
        Device->Enabled = TRUE;
    }

    B_(if (FALSE == Device->Enabled) {
    printf("fm_emmc.c (%d): Flash%u NOT configured! Deviceid: %d BootInfo:%x\n", __LINE__, FlashDevice, Device->DeviceId, MMCInterfaceSelection);
    })
}

static boolean IsCardOKTotest(t_mmc_device_id FlashDevice)
{
    boolean result = FALSE;
    int i = 0;

    while (testablecards[i] != MMC_DEVICE_ID_INVALID) {
        if (FlashDevice == testablecards[i]) {
            result = TRUE;
            break;
        }

        i++;
    }

    B_(if (TRUE == result) {
    printf("fm_emmc.c (%d): FlashDevice %d available\n", __LINE__, FlashDevice);
    } else {
        printf("fm_emmc.c (%d): FlashDevice %d NOT available\n", __LINE__, FlashDevice);
    })

    return result;
}

/** @} */
/** @} */
/** @} */
