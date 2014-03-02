/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _FM_ED_EMMC_H_
#define _FM_ED_EMMC_H_

/**
 * @addtogroup ldr_service_fpd Flash Physical Driver
 * @{
 *   @addtogroup ldr_service_fpd_hw_u8500
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "flash_manager.h"

#ifdef CNH1606644_FM_USES_EMMC
#include "mmc_irq.h"
#endif //CNH1606644_FM_USES_EMMC

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define EMMC_PAGE_SIZE 512

// eMMC extended CSD indexes
#define ECSD_HC_ERASE_GRP_SIZE           224
#define REL_WR_SEC_C                     222
#define ECSD_HC_WP_GRP_SIZE              221
#define ECSD_SEC_COUNT_WORD_OFFSET       212
#define ECSD_ERASED_MEM_CONT_BYTE_OFFSET 181
#define RPMB_SIZE_MULT                   168
#define ECSD_PARTITIONS_ATTRIBUTE        156
#define ECSD_ENH_SIZE_MULT               140
#define ECSD_ENH_START_ADDR              136

// Jedec 84-A44 : Size < 2GB => byte addressing, otherwise 0x200 sector addressing
#define MMC_ADDRESS_MODE_BOUNDARY     0x80000000
#define MMC_ADDRESS_MODE_FACTOR       0x200

typedef struct {
    uint32  hi;
    uint32  lo;
} BitRange_t;

// eMMC Card Specific Data according to Jedec Standard no. 84-A44
typedef enum {
    CSD_STRUCTURE      = 0, // 2 R    [127:126]
    SPEC_VERS          , // 4    R    [125:122]
    RESERVED_3         , // 2    R    [121:120]
    TAAC               , // 8    R    [119:112]
    NSAC               , // 8    R    [111:104]
    TRAN_SPEED         , // 8    R    [103:96]
    CCC                , //12    R    [95:84]
    READ_BL_LEN        , // 4    R    [83:80]
    READ_BL_PARTIAL    , // 1    R    [79:79]
    WRITE_BLK_MISALIGN , // 1    R    [78:78]
    READ_BLK_MISALIGN  , // 1    R    [77:77]
    DSR_IMP            , // 1    R    [76:76]
    RESERVED_2         , // 2    R    [75:74]
    C_SIZE             , //12    R    [73:62]
    VDD_R_CURR_MIN     , // 3    R    [61:59]
    VDD_R_CURR_MAX     , // 3    R    [58:56]
    VDD_W_CURR_MIN     , // 3    R    [55:53]
    VDD_W_CURR_MAX     , // 3    R    [52:50]
    C_SIZE_MULT        , // 3    R    [49:47]
    ERASE_GRP_SIZE     , // 5    R    [46:42]
    ERASE_GRP_MULT     , // 5    R    [41:37]
    WP_GRP_SIZE        , // 5    R    [36:32]
    WP_GRP_ENABLE      , // 1    R    [31:31]
    DEFAULT_ECC        , // 2    R    [30:29]
    R2W_FACTOR         , // 3    R    [28:26]
    WRITE_BL_LEN       , // 4    R    [25:22]
    WRITE_BL_PARTIAL   , // 1    R    [21:21]
    RESERVED_1         , // 4    R    [20:17]
    CONTENT_PROT_APP   , // 1    R    [16:16]
    FILE_FORMAT_GRP    , // 1   R/W   [15:15]
    COPY               , // 1   R/W   [14:14]
    PERM_WRITE_PROTECT , // 1   R/W   [13:13]
    TMP_WRITE_PROTECT  , // 1  R/W/E  [12:12]
    FILE_FORMAT        , // 2   R/W   [11:10]
    ECC                , // 2  R/W/E  [9:8]
    CRC                , // 7  R/W/E  [7:1]
    NOT_USED           , // 1    1    [0:0] ,always 1
    MAX_BITS
} CSD_eMMCBits_t;

/*
 * Macro used to loop some function N-times until proper result is returned
 * SYNTAX:
 * TRY_N_WAIT(<args>)
 * args:
 * 1. integer value, how many time should macro loop a function
 * 2. uint32 value, time(delay) to wait until it loops again
 * 3. expected returned value of a function
 * 4. variable where the returned value of the function should be stored
 * 5. function name
 * n. all function's arguments should be appended at the end separated with comma sign ","
 *
 * example:
 * error = test_function(1, 2, 3);
 * if we want to loop this function 5 times( with delay time 10) and try to get expected returned value E_SUCCESS
 * then we use this macro as follows:
 * TRY_N_WAIT(5, 10, E_SUCCESS, error, test_function, 1, 2, 3)
 */
#ifdef TRY_N_WAIT_DISABLED
#define TRY_N_WAIT(loop_no, time, expected_rv, rv, loop_func,...) \
rv = loop_func(__VA_ARGS__);
#else
#define TRY_N_WAIT(loop_no, time, expected_rv, rv, loop_func,...) \
for(int loop_counter = 0; loop_counter < loop_no; loop_counter++) { \
if ((rv = loop_func(__VA_ARGS__)) == expected_rv) break; \
Sleep(time);}
#endif

#if STE_PLATFORM_NUM == 8500

/* This tells the max number of flashdevices when there are no
 * disabled devices*/
#ifdef STE_AP8540
#define MAX_NR_OF_FLASH_DEVICES 1
#else
#define MAX_NR_OF_FLASH_DEVICES 2
#endif

static const t_mmc_device_id testablecards[] = {
#ifndef DISABLE_MC2
    MMC_DEVICE_ID_2,
#endif
#ifndef DISABLE_MC4
    MMC_DEVICE_ID_4,
#endif
    MMC_DEVICE_ID_INVALID
};
//U5500 starts here
#elif STE_PLATFORM_NUM == 5500

/* This tells the max number of flashdevices when there are no
 * disabled devices*/
#define MAX_NR_OF_FLASH_DEVICES 2

static const t_mmc_device_id testablecards[] = {
#ifndef DISABLE_MC0
    MMC_DEVICE_ID_0,
#endif
#ifndef DISABLE_MC2
    MMC_DEVICE_ID_2,
#endif
    MMC_DEVICE_ID_INVALID
};
#else
#error "No MC configuration for this platform"
#endif

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * This function initialize FPD_Config_t structure with flash memory properties
 * from FPD.
 *
 * @param [in]     FlashDevice  - Flash device number
 * @param [in out] FPD_Config_p - Flash device configuration to fill
 * @return       @ref E_SUCCESS   Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
void Do_EMMC_Init(const FLASH_DEVICE_t FlashDevice, FPD_Config_t *const FPD_Config_p);

/**
 * This function initialize Phisycal driver, sets memory properties.
 *
 * @param [in]   FPD_Config_p   - Flash device configuration
 * @return       @ref E_SUCCESS   Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_Start(FPD_Config_t *const FPD_Config_p);

/**
 * Phisycal driver can't be stop. This function need to exist for service
 * manager to work properly.
 *
 * @param [in]   FPD_Config_p   - Flash device configuration
 * @return       @ref E_SUCCESS   Success ending.
 */
ErrorCode_e Do_EMMC_Stop(FPD_Config_t *const FPD_Config_p);

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
ErrorCode_e Do_EMMC_GetInfo(FPD_Config_t *const FPD_Config_p, FPD_Config_t *Configuration_p);

/**
 * Retrieves memory properties.
 *
 * @param [in] FPD_Config_p   - Flash device configuration to update
 * @param [in] NewConfig_p    - Contains necessary information for
 *                              configuring the FPD.
 * @return     @ref E_SUCCESS - Success ending.
 *
 * @return     @ref E_INVALID_INPUT_PARAMETER - If NULL is passed.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_EMMC_SetInfo(FPD_Config_t *const FPD_Config_p, FPD_Config_t *NewConfig_p);

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
                             uint8 *ExtraData_p);

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
        uint8 *ExtraData_p);

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
                              FPD_Config_t *const FPD_Config_p);

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
        FPD_Config_t *const FPD_Config_p);

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
                          FPD_Config_t *const FPD_Config_p);

/**
 * This function check is started FPD service.
 *
 * @param [in]   FPD_Config_p   - Flash device configuration
 * @return  TRUE If FPD is started.
 */
boolean Do_EMMC_IsStarted(FPD_Config_t *const FPD_Config_p);

/**
 * This function check the block.
 *
 * param [in] BlockNr Block that will be checked.
 * param [in] FPD_Config_p Flash device configuration
 *
 * @return  TRUE If block is bad.
 */
boolean Do_FPD_BlockIsBad(uint32 BlockNr, FPD_Config_t *const FPD_Config_p);

/**
 * This function mark the block if is bad.
 *
 * @param [in] BlockNr The block that will be mark as bad.
 *
 * @return none
 */
void Do_FPD_MarkBlockAsBad(uint32 BlockNr);

/**
 * Check for new partitions configuration.
 * @return      @ref E_UNSUPPORTED_FLASH_TYPE flash type is not supported by loader
 */
ErrorCode_e Do_EMMC_GetPartitionsConfiguration(void);

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
ErrorCode_e Do_EMMC_SetEnhancedArea(uint64 StartAddress, uint64 AreaSize, FPD_Config_t *const FPD_Config_p);

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
ErrorCode_e Do_EMMC_MMC_EnPowerReset(uint32 RegValue, FPD_Config_t *const FPD_Config_p);

/** @} */
/** @} */
#endif /*_FM_ED_EMMC_H_*/
