/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _FLASH_MANAGER_H_
#define _FLASH_MANAGER_H_

/**
 * @addtogroup ldr_service_management
 * @{
 *   @addtogroup ldr_service_fpd Flash Physical Driver
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Indicates that the block is ok. */
#define FPD_BLOCK_STATUS_NOT_BAD     (0xff)
/** Indicates that the block is factory bad. */
#define FPD_BLOCK_STATUS_FACTORY_BAD (0xf0)
/** Indicates that the block is later damaged. */
#define FPD_BLOCK_STATUS_LATER_BAD   (0x00)

/** Size of page for small page FPD. */
#define FPD_PAGE_SIZE_SMALL          (512)
/** Size of page for large page FPD. */
#define FPD_PAGE_SIZE_LARGE          (2048)
/** Identifier for complete flash. */
#define COMPLETE_FLASH_MARKER        (0xffffffff)

#define INCLUDE_REDUNDANT_AREA  0
#define WITHOUT_REDUNDANT_AREA  1

/**
 * this are flags for determing the memory type present in ME
 */
TYPEDEF_ENUM {
    MEM_TYPE_NAND = 0, // Must be same as in CABS and MBBS
    MEM_TYPE_NOR = 1,
    MEM_TYPE_EMMC,
    MEM_TYPE_NONE
} ENUM32(Loader_Flash_Mem_Type_t);

TYPEDEF_ENUM {
    FLASH_0 = 0,
    FLASH_1
} ENUM8(FLASH_DEVICE_t);

/**
 * Physical driver configuration structure.
 *
 * @param Loader_Flash_Mem_Type Memory type detected(NAND, NOR).
 * @param BusWidth              Bus width in bits (8, 16, 32).
 * @param DeviceSize            Total size of memory in bytes.
 * @param EraseBlockSize        Size of an erase block.
 * @param PageSize              Size of a page.
 * @param PageExtraSize         Size of a page's extra/redundant area.
 * @param NbrOfAddressCycles    Number of address cycles used for a NAND flash.
 * @param Configuration_Set     Set to TRUE if driver is initialized.
 * @param Rpmb_size_mult        RPMB Size.
 * @param Rel_wr_sec_c          Reliable write sector count.
 */
typedef struct {
    Loader_Flash_Mem_Type_t Loader_Flash_Mem_Type;
    uint32 BusWidth;
    uint64 DeviceSize;
    uint32 EraseBlockSize;
    uint32 WriteProtectedSize;
    uint32 PageSize;
    uint32 PageExtraSize;
    uint32 NbrOfAddressCycles;
    boolean Configuration_Set;
    uint8  ErasedMemContent;
    uint8  DeviceId;
    uint8  RpmbSizeMult;
    uint8  RelWrSecC;
    sint8  DeviceStatus;
    boolean HC_Card;
    uint64 EnhancedAreaStart;
    uint64 EnhancedAreaEnd;
} FPD_Config_t;

/**
 * Contains FPD function pointers.
 */
typedef struct {
    ErrorCode_e(*FPD_Start)(FPD_Config_t *const FPD_Config_p);
    ErrorCode_e(*FPD_Stop)(FPD_Config_t *const FPD_Config_p);
    ErrorCode_e(*FPD_GetInfo)(FPD_Config_t *const FPD_Config_p, FPD_Config_t *Configuration_p);
    ErrorCode_e(*FPD_SetInfo)(FPD_Config_t *const FPD_Config_p, FPD_Config_t *NewConfig_p);
    ErrorCode_e(*FPD_ReadPage)(const uint32 PageOffset, FPD_Config_t *const FPD_Config_p, uint8 *Data_p, uint8 *ExtraData_p);
    ErrorCode_e(*FPD_ReadConsecutivePages)(const uint32 PageOffset, const uint32 NbrOfPages, FPD_Config_t *const FPD_Config_p, uint8 *Data_p, uint8 *ExtraData_p);
    ErrorCode_e(*FPD_WritePage)(const uint32 PageOffset, const uint8 *const Data_p, const uint8 *const ExtraData_p, FPD_Config_t *const FPD_Config_p);
    ErrorCode_e(*FPD_WriteConsecutivePages)(const uint32 PageOffset, const uint32 NbrOfPages, const uint8 *const Data_p,
                                            const uint8 *const ExtraData_p, FPD_Config_t *const FPD_Config_p);
    ErrorCode_e(*FPD_Erase)(uint32 PageOffset, uint32 NbrOfBlocks, FPD_Config_t *const FPD_Config_p);
    boolean(*FPD_IsStarted)(FPD_Config_t *const FPD_Config_p);
    ErrorCode_e(*FPD_GetPartitionsConfiguration)(void);
    ErrorCode_e(*FPD_SetEnhancedArea)(uint64 StartAddress, uint64 AreaSize, FPD_Config_t *const FPD_Config_p);
    ErrorCode_e(*FPD_EnPowerReset)(uint32 RegValue, FPD_Config_t *const FPD_Config_p);
} FPD_Functions_t;

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 */
void Do_FPD_Init(void);

/**
 * This function initialize Phisycal driver, sets memory properties.
 *
 * @return       @ref E_SUCCESS  � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_Start(void);


/**
 * Phisycal driver can't be stop. This function need to exist for service
 * manager to work properly.
 *
 * @return       @ref E_SUCCESS  � Success ending.
 */
ErrorCode_e Do_FPD_Stop(void);

/**
 * Phisycal driver can't be stop. This function need to exist for service
 * manager to work properly.
 *
 * @return       @ref E_SUCCESS  � Success ending.
 */
ErrorCode_e Do_FPD_Stop(void);

/**
 * Retrieves memory properties.
 *
 * @param [in]   FlashDevice     - Flash device number
 * @param [out]  Configuration_p - Pointer to configuration structure.
 * @return       @ref E_SUCCESS  � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 *
 */
ErrorCode_e Do_FPD_GetInfo(const FLASH_DEVICE_t FlashDevice, FPD_Config_t *Configuration_p);

/**
 * Retrieves memory properties.
 *
 * @param [in] FlashDevice    - Flash device number
 * @param [in] NewConfig_p    - Contains necessary information for
 *                              configuring the FPD.
 * @return     @ref E_SUCCESS � Success ending.
 *
 * @return     @ref E_INVALID_INPUT_PARAMETER - If NULL is passed.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_SetInfo(const FLASH_DEVICE_t FlashDevice, FPD_Config_t *NewConfig_p);

/**
 * Reads one page and/or extra data from FPD. The length of the data buffer
 * Data_p must be "page size" * NbrOfPages. The length of the ExtraData_p must
 * be the ExtraDataLength * NbrOfPages.
 *
 * @param [in]   PageOffset     - An offset expressed in number of pages.
 * @param [in]   FlashDevice    - Flash device number
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
ErrorCode_e Do_FPD_ReadPage(const uint32 PageOffset,
                            const FLASH_DEVICE_t FlashDevice,
                            uint8  *Data_p,
                            uint8  *ExtraData_p);

/**
 * Reads one or several consecutive pages and/or extra data from FPD. The size
 * of the data buffer Data_p must be "page size" * NbrOfPages. The size of the
 * data buffer ExtraData_p must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfPages     - Number of pages to read.
 * @param [in]  FlashDevice    - Flash device number
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
ErrorCode_e Do_FPD_ReadConsecutivePages(const uint32 PageOffset,
                                        const uint32 NbrOfPages,
                                        const FLASH_DEVICE_t FlashDevice,
                                        uint8 *Data_p,
                                        uint8 *ExtraData_p);

/**
 * Writes one page and/or extra data to FPD. The size of the data buffer Data_p
 * must be "page size" * NbrOfPages. The size of the data buffer ExtraData_p
 * must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  FlashDevice    - Flash device number
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
ErrorCode_e Do_FPD_WritePage(const uint32 PageOffset,
                             const FLASH_DEVICE_t FlashDevice,
                             const uint8 *const Data_p,
                             const uint8 *const ExtraData_p);

/**
 * Writes one or several consecutive pages and/or extra data to FPD. The size of
 * the data buffer Data_p must be "page size" * NbrOfPages. The size of the data
 * buffer ExtraData_p must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfPages     - Number of pages to write.
 * @param [in]  FlashDevice    - Flash device number
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
ErrorCode_e Do_FPD_WriteConsecutivePages(const uint32 PageOffset,
        const uint32 NbrOfPages,
        const FLASH_DEVICE_t FlashDevice,
        const uint8 *const Data_p,
        const uint8 *const ExtraData_p);

/**
 * Erases one or several blocks in FPD.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfBlocks    - Number of blocks to erase.
 * @param [in]  FlashDevice    - Flash device number
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_Erase(uint32 PageOffset,
                         uint32 NbrOfBlocks,
                         const FLASH_DEVICE_t FlashDevice);

/**
 * This function check is started FPD service.
 *
 * @param [in]   FlashDevice     - Flash device number
 *
 * @return  TRUE If FPD is started.
 */
boolean Do_FPD_IsStarted(const FLASH_DEVICE_t FlashDevice);

/**
 * Check for new partitions configuration.
 */
ErrorCode_e Do_FPD_GetPartitionsConfiguration(void);

/**
 * Defines Enhanced Area in flash memory.
 *
 * @param [in]  StartAddress   - Enhanced area start address in bytes.
 * @param [in]  AreaSize       - Enhanced area size in bytes.
 * @param [in]  FlashDevice    - Flash device number
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_SetEnhancedArea(uint64 StartAddress,
                                   uint64 AreaSize,
                                   const FLASH_DEVICE_t FlashDevice);
/**
 * This function returns number of available flash devices
 *
 * @return  Number of flash devices.
 */
uint8 Do_FPD_GetNrOfAvailableFlashDevices(void);
/**
 * Function for disable/enable HW reset
 *
 * @param [in] RegValue     - Value to be written in register (1- set, 0-reset)
 * @param [in] FlashDevice  - Flash device number
 * @return      Returns E_SUCCESS for success ending.
 *
 * @return      Returns E_ENABLE_HW_RESET_FAILED for fail.
 *
 * @return      Function can also return some internal MMC error codes.
 */
ErrorCode_e Do_FPD_EnPowerReset(uint32 RegValue, const FLASH_DEVICE_t FlashDevice);

/** @} */
/** @} */
#endif /*_FLASH_MANAGER_H_*/
