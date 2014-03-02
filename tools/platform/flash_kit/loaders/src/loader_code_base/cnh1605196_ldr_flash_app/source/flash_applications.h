#ifndef _FLASH_APPLICATIONS_H_
#define _FLASH_APPLICATIONS_H_
/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_flash_app
 *  @{
 *  This module implements functionalities of flash applications.
 *
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_communication_service.h"
#include "t_flash_applications.h"

#define DEST_MODE_PC(INSERT)\
  if (FetchMode == DEST_SERIALIZED_TO_PC) {\
       INSERT \
       }

#define DEST_MODE_TBL(INSERT)\
  if (FetchMode == DEST_STORE_PATHS_IN_TABLE) {\
       INSERT \
       }

typedef enum {
    PROCESS_END = 0,
    PROCESS_WRITE_BOOT_AREA,
    PROCESS_WRITE_PARTITION_ENTRIES,
    PROCESS_WRITE_EMPTY_PARTITIONS,
    PROCESS_READ_SIGNATURE,
    PROCESS_OPEN_SIGNATURE,
    PROCESS_WRITE_DATA_LOOP,
    PROCESS_WRITE_DATA,
    PROCESS_READ_FILE,
    PROCESS_FILESAFE_UPDATE,
    PROCESS_ALLOCATE_FILE,
    PROCESS_READ_TOC_AND_SUBTOC,
    PROCESS_READ_FLASHLAYOUT,
    PROCESS_OPEN_FLASHLAYOUT,
    PROCESS_READ_MANIFEST,
    PROCESS_ALLOCATE_MANIFEST,
    PROCESS_OPEN_MANIFEST,
    PROCESS_OPEN_VERSIONS,
    PROCESS_READ_VERSIONS,
    PROCESS_PARSE_ZIP_IN_ZIP,
    PROCESS_PARSE_ZIP,
    PROCESS_INIT
} ProcessFile_t;

typedef enum {
    DUMP_INIT = 0,
    DUMP_READ_DATA,
    DUMP_WRITE_DATA,
    DUMP_READ_NEXT_DATA,
    DUMP_WRITE_DATA_LOOP,
    DUMP_END
} DumpArea_t;

typedef enum {
    DEST_SERIALIZED_TO_PC = 0, //serialize data and send to PC
    DEST_STORE_PATHS_IN_TABLE  //store data in path table in RunTime

} FetchDeviceMode_t;

typedef enum {
    DISABLE_ENHANCED_AREA_CHECK = 0,
    DISABLE_SW_RESET            = 1
} SelectLoaderOptions_t;

//Variables that enables various loader options
extern uint32 DisabledEnhancedAreaCheck;


/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * @brief Erases data from flash memory
 *
 * Erases any area with any lenght on flash memory. If area is not aligned to erase block, than it will write
 * empty data on flash (0x00 or 0xFF)
 *
 * @param [in] FlashDevice       Flash device number
 * @param [in] AddressOffset     Erase area start address
 * @param [in] LengthBytes       Number of bytes to be erased
 *
 * @return   @ref E_SUCCESS      Success ending.
 */
ErrorCode_e Do_EraseAreaWithDriver(const uint8 FlashDevice, uint64 AddressOffset, uint64 LengthBytes);
/**
 * @brief Split string into tokens
 *
 * @param [in] Pointer to string from which to extract tokens
 * @param [in] Pointer to null-terminated set of delimiter characters
 * @param [in] Value-return parameter used by this function
 *
 * @return     Pointer to extracted token
 */
char *strtok_r(char *, const char *, char **);

/* @} */
#endif /*_FLASH_APPLICATIONS_H_*/
