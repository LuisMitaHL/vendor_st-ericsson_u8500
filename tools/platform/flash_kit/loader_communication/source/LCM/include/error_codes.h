/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2012
 * License terms: 3-clause BSD license
 ******************************************************************************/
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

#ifndef _ERRORCODE_H
#define _ERRORCODE_H


/**
 *  @addtogroup ldr_LCM
 *  Error codes for internal loader commands.
 *  @{
 */

/*************************************************************************
* Includes
*************************************************************************/
#include "t_basicdefinitions.h"

#define A2_ERROR_CODES_OFFSET 5000
/**
 *  Internal loader command error codes. Possible range 0x0000 - 0x0FFE (4094).
 */


/**
 * Table for Error groups range
 *
 * General Fatal 0-50
 * General non-fatal 51-99
 *
 * IO Fatal 100-150
 * IO non-fatal 151-199
 *
 * Communication Fatal 200-250
 * Communication non-fatal 251-299
 *
 * Signature Fatal 300-350
 * Signature non-fatal 351-399
 *
 * Authentication Fatal 400-450
 * Authentication non-fatal 451-499
 *
 * Zip Parser Fatal 500-550
 * Zip Parser non-fatal 551-599
 *
 * System Fatal 600-650
 * System non-fatal 651-699
 *
 * Flash Fatal 700-750
 * Flash non-fatal 751-799
 *
 *
 * File management Fatal 900-950
 * File management non-fatal 951-999
 *
 * Command Auditing and execution Fatal 1000-1050
 * Command Auditing and execution non-fatal 1051-1099
 *
 *
 * Timers Fatal 1200-1250
 * Timers non-fatal 1251-1299
 *
 * Parameter Fatal 1300-1350
 * Parameter non-fatal 1351-1399
 *
 * Block device Fatal 1400-1450
 * Block device non-fatal 1451-1499
 *
 * Boot area Fatal 1500-1550
 * Boot area non-fatal 1551-1599
 *
 * Cops data Fatal 1600-1650
 * Cops data non-fatal 1651-1699
 *
 * PD NAND Fatal 1700-1750
 * PD NAND non-fatal 1751-1799
 *
 * Trim Area non-fatal 1851-1899
 *
 * Loader utilities Fatal 1900-1950
 * Loader utilities non-fatal 1951-1999
 *
 * Loader ADBG Fatal 2000-2050
 * Loader ADBG non-fatal 2051-2099
 *
 * OTP applications Fatal 2100-2150
 * OTP applications non-fatal 2151-2199
 *
 * Security applications Fatal 2200-2250
 * Security applications non-fatal 2251-2299
 *
 * Trim Area Fatal 2300-2350
 * Trim Area non-fatal 2351-2499
 *
 * Recovery applications Fatal 2500-2550
 * Recovery applications non-fatal 2551-2599
 *
 * PD CFI Flash Fatal 2600-2650
 * PD CFI Flash non-fatal 2651-2699
 *
 * HSI Driver Fatal 2700-2724
 * HSI Driver Non-Fatal 2725-2749
 *
 * HSI Driver Error Callback Fatal 2750-2774
 * HSI Driver Error Callback Non-Fatal 2775-2799
 *
 * Communication Relay Fatal 2800-2849
 * Communication Relay Non-Fatal 2850-2899
 *
 * SDIO Driver Fatal 2900-2924
 * SDIO Driver Non-Fatal 2925-2949
 *
 * SDIO Driver Error Callback Fatal 2950-2974
 * SDIO Driver Error Callback Non-Fatal 2975-2999
 *
 * External BAM Interface Fatal 3000-3049
 * External BAM Interface non-fatal 3050-3099
 *
 * Security Library Fatal 4000-4050
 * Security Library non-fatal 4051-4250
 *
 * Emulator Fatal 4300-4350
 * Emulator non-fatal 4351-4399
 *
 * A2 and lower versions error codes translation 5000+
 */


typedef enum {
    E_SUCCESS = 0, /**<  Operation successful. */
    E_GENERAL_FATAL_ERROR = 1, /**<  General Failure. */
    E_ALLOCATE_FAILED = 2, /**<  Failed to allocate memory. */
    E_INVALID_INPUT_PARAMETERS = 51, /**<  The expected value into the function was incorrect. */
    E_INVALID_CURRDATE_STRING_LENGTH = 52, /**<  Indicate that the currdate string array variable has invalid length. */
    E_UNALIGNED_DATA = 53, /**<  Indicate that a variable is not aligned. */
    E_COPS_DEAUTHENTICATION_FAILED = 54, /**<  Indicate that ME De-Authentication failed. */
    E_CS_LOCK_FAILED = 55, /**<  Failed to lock code region protected with critical section. */
    E_INVALID_INPUT_PARAMETERS_ENHANCED_AREA = 56, /**<  The parameter is not dividable by the parameter WriteProtectedGroupSize. */
    E_VECTOR_CREATE_FAIL = 100, /**<  Failed to create vector in IO Layer. */
    E_VECTOR_DESTROY_FAIL = 101, /**<  Failed to destroy vector in IO Layer. */
    E_GENERAL_IO_ERROR = 151, /**<  Unknown IO error. */
    E_IO_FAILED_TO_READ = 152, /**<  IO failed to read from source. */
    E_IO_FAILED_TO_WRITE = 153, /**<  IO failed to write to destination. */
    E_IO_FAILED_TO_CLOSE = 154, /**<  IO failed to close media. */
    E_IO_FAILED_TO_OPEN = 155, /**<  IO failed to open media. */
    E_IO_FAILED_TO_GET_LENGTH = 156, /**<  IO failed to get length of media. */
    E_GENERAL_COMMUNICATION_ERROR = 200, /**<  General communication error. */
    E_FAILED_TO_START_BULK_SESSION = 201, /**<  Failed to start bulk session. */
    E_FAILED_TO_CLOSE_BULK_SESSION = 202, /**<  Failed to initialize the transport layer. */
    E_FAILED_TO_FIND_CHUNK_DATA_BLOCK = 203, /**<  Failed to find chunk data block. */
    E_FAILED_TO_INIT_COM_DEVICE = 204, /**<  Failed to reinitialize communication device. */
    E_FAILED_TO_USE_COM_DEVICE = 205, /**<  Failed to use communication device. */
    E_FAILED_TO_ALLOCATE_COMM_BUFFER = 206, /**<  Failed to allocate communication buffer. */
    E_FAILED_TO_FLUSH_RXFIFO = 207, /**<  Failed to flush RX FIFO. */
    E_RETRANSMITION_FAILED = 208, /**<  Retransmission failed. After MAX_RESENDS attempt, failed to send packet. */
    E_COMMAND_NO_ERROR = 209, /**<  Stopped command error sequence. */
    E_FAILED_TO_RELEASE_COMM_BUFFER = 210, /**<  Failed to release communication buffer. */
    E_FAILED_TO_INTIALIZE_TIMER_FUNCTIONS = 211, /**<  Failed to initialize timer functions. */
    E_FAILED_TO_INTIALIZE_QUEUE_FUNCTIONS = 212, /**<  Failed to initialize queue functions. */
    E_DMA_INIT_ERROR = 213, /**<  Failed to initialize DMA. */
    E_FAILED_TO_STOP_DMA = 214, /**<  Failed to stop DMA channel. */
    E_FAILED_TO_OPEN_DMA_CHANNEL = 215, /**<  Failed to open DMA channel. */
    E_FAILED_TO_SET_DMA_CHANNEL_TYPE = 216, /**<  Failed to set DMA channel type. */
    E_FAILED_TO_SET_DMA_CHANNEL_MODE = 217, /**<  Failed to set DMA channel mode. */
    E_FAILED_TO_OPEN_DMA_PIPE = 218, /**<  Failed to open DMA pipe. */
    E_FAILED_TO_SET_DMA_IT_LOGIC = 219, /**<  Failed to set DMA interrupt logic. */
    E_FAILED_TO_CLOSE_DMA_PIPE = 220, /**<  Failed to close DMA pipe. */
    E_FAILED_TO_CLOSE_DMA_CHANNEL = 221, /**<  Failed to close DMA channel. */
    E_FAILED_TO_CONFIG_SRC_DMA_DEVICE = 222, /**<  Failed to configure DMA source device. */
    E_FAILED_TO_UPDATE_DMA_SRC_MEMORY_BUFFER = 223, /**<  Failed to update DMA source memory buffer. */
    E_FAILED_TO_CONFIG_DEST_DMA_DEVICE = 224, /**<  Failed to configure DMA destination device. */
    E_FAILED_TO_UPDATE_DMA_DEST_MEMORY_BUFFER = 225, /**<  Failed to update DMA destination memory buffer. */
    E_FAILED_TO_RUN_DMA = 226, /**<  Failed to run DMA transfer. */
    E_INVALID_BULK_MODE = 251, /**<  Invalid bulk mode. */
    E_FAILED_TO_FIND_COMM_BUFFER = 252, /**<  Failed to find communication buffer. */
    E_INVALID_TYPE_OF_BUFFER = 253, /**<  Invalid type of buffer. */
    E_COM_DEVICE_BUSY = 254, /**<  Communication device is busy. */
    E_NOT_FOUND_ELEMENT_IN_RETRANSMISSION_LIST = 255, /**<  The element in retransmission list is not found. */
    E_FAILED_READING_FROM_BULK = 256, /**<  Failed to read from bulk. */
    E_FAILED_WRITING_TO_BULK = 257, /**<  Failed to write to bulk. */
    E_FAILED_TO_GET_UART_DESCRIPTOR = 258, /**<  Failed to get UART descriptor. */
    E_FAILED_TO_GET_USB_DESCRIPTOR = 259, /**<  Failed to get USB descriptor. */
    E_INVALID_BULK_SESSION_ID = 260, /**<  Invalid bulk session ID is used. */
    E_PREVIOUS_BULK_SESSION_IS_NOT_CLOSED = 261, /**<  Previous bulk session not closed. */
    E_INVALID_BULK_PROTOCOL_STATE = 262, /**<  Invalid bulk protocol state. */
    E_UNSUPPORTED_USB_TRANSFER_TYPE = 263, /**<  Type of transfer is unsupported. */
    E_RETRANSMISSION_LIST_FULL = 264, /**<  Packet can't be registered for retransmission because retransmission list is full. */
    E_UNKNWON_PROPERTY = 351, /**<  Unknown property id. */
    E_CYCLIC_GRAPH = 451, /**<  Cyclic graph in services detected. */
    E_SERVICE_NOT_SUPPORTED = 452, /**<  The service is not supported. */
    E_INCONSISTENCY_IN_SERVICES = 453, /**<  Inconsistency in services is detected. */
    E_SERVICE_IN_USE = 454, /**<  The service is in use and can't be stopped. */
    E_UNREGISTER_BDM_SERVICE_FAILED = 455, /**<  Unregistering of Block Device Manager Service failed. */
    E_UNREGISTER_BAM_SERVICE_FAILED = 456, /**<  Unregistering of Boot Area Manager Service failed. */
    E_UNREGISTER_COPS_SERVICE_FAILED = 457, /**<  Unregistering of COPS Data Manager Service failed. */
    E_UNREGISTER_FS_SERVICE_FAILED = 458, /**<  Unregistering of File System Manager Service failed. */
    E_UNREGISTER_FPD_SERVICE_FAILED = 459, /**<  Unregistering of Flash Physical Driver Service failed. */
    E_UNREGISTER_GD_SERVICE_FAILED = 460, /**<  Unregistering of Global Data Manager Service failed. */
    E_GENERAL_ZIP_ERROR = 551, /**<  General error. */
    E_ZIP_FAILED_TO_CREATE_CONTEXT = 552, /**<   Failed to parse the zip. */
    E_ZIP_FAILED_TO_OPEN_FILE = 553, /**<   Failed to open file from the zip. */
    E_FILESYS_APP_INIT_FAILURE = 600, /**<  Failed to initialize the file system application. */
    E_NO_FILESYSTEM_PROPERTY = 601, /**<  Failed to start file system. */
    E_UNDEFINED_AUTHENTICATION_TYPE = 602, /**<  Undefined authentication type. */
    E_RTC_TIME_NOT_ACCURATE = 651, /**<  System Time is not properly set. */
    E_RTC_INTIALIZATION_FAILED = 652, /**<  Hardware error occurred during initialization of RTC. */
    E_UNDEFINED_DEAUTHENTICATION_TYPE = 653, /**<  Undefined deauthentication type. */
    E_COPS_DEVICE_STATE_FULL = 654, /**<  Indicate that ME is in full security mode,Full authentication is needed prior to use COPS functionalities. */
    E_COPS_DEVICE_STATE_RELAXED = 655, /**<  Indicate that ME is in authenticate security mode, Permanent type authentication is needed prior to use COPS functionalities. */
    E_DIFFERENT_FLASHLAYOUT = 751, /**<  Different flashlayout. */
    E_EMPTY_FILE_IN_ARCHIVE = 752, /**<  Archive contain empty file. */
    E_UNKNOWN_COMM_DEVICE = 753, /**<  Unknown communication device was detected. */
    E_FLASH_APP_INTERNAL_ERROR = 754, /**<  Internal during execution of flash commands. */
    E_DEVICE_NAME_TOO_LONG = 755, /**<  Device name is too long. */
    E_FLASH_ARCHIVE_MISMATCH = 756, /**<  Mismatch between archive that is flashed and previously flashed one. */
    E_UNSUPPORTED_FLASH_TYPE = 757, /**<  Flash memory device type is not supported. */
    E_FPD_NOT_CONFIGURED = 758, /**<  Flash physical driver is not configured. */
    E_INVALID_SIZE_IN_MEMCONF = 759, /**<  MEMCONF boot record contain invalid TotalSize field. */
    E_ARCHIVE_TO_LARGE = 760, /**<  Flash archive larger than available space in BDM. */
    E_ENTRY_NOT_FOUND_IN_FLASHLAYOUT = 761, /**<  Entry not found in flashlayout file. */
    E_MISMATCH_MANIFEST_FLASHLAYOUT = 762, /**<  Mismatch between manifest and flash layout. */
    E_INVALID_SUBTOC_PARAMETERS = 763, /**<  SUBTOC is invalid. */
    E_FAILED_TO_READ_SUBTOC = 764, /**<  Reading SUBTOC from flash failed. */
    E_OVERLAPPING_PARTITIONS_FOUND = 765, /**<  Overlapping partitions found in the archive. */
    E_PARTITION_NOT_FOUND = 766, /**<  Partition not found in TOC. */
    E_FLASHLAYOUT_NULL = 767, /**<  Flashlayout data equals NULL. */
    E_INVALID_TOC_TYPE = 768, /**<  Invalid TOC type supplied during TOC list creation. */
    E_TOC_HANDLER_INPUT_DATA_EMPTY = 769, /**<  TOC list cannot be created because supplied buffer is empty. */
    E_TOC_HANDLER_INPUT_DATA_CORRUPTED = 770, /**<  TOC list cannot be created because supplied buffer contains corrupted data. */
    E_TOC_LIST_CORRUPTED = 771, /**<  TOC list cannot be created. Some malicious state occurs. */
    E_TOC_BOOT_AREA_EMPTY = 772, /**<  TOC list cannot be created. Boot area is empty. */
    E_BOOT_AREA_NOT_FOUND = 773, /**<  Boot Area not found. */
    E_MISMATCH_MANIFEST_TOC = 774, /**<  Mismatch between manifest and TOC. */
    E_FILE_TOO_LARGE = 775, /**<  File too large. */
    E_FILE_OUT_OF_BOOT_AREA = 776, /**<  File outside of boot area */
    E_DUMP_OUT_OF_FLASH_RANGE = 777, /**<  Dump outside of flash range */
    E_ERASING_BAM_AREA_FAILED = 778, /**<  Boot area is not empty. Erasing boot area on a logical level is not allowed. */
    E_ENHANCED_AREA_NOT_SET = 779, /**<  Enhanced area is not set */
    E_ENHANCED_IMAGE_OUT_OF_ENHANCED_AREA = 780, /**<  Image out of enhanced area */
    E_SET_ENHANCED_AREA_FAILED = 781, /**<  Setting of enhanced area failed */
    E_PATH_NOT_EXISTS = 951, /**<  Path not exist. */
    E_CLOSE_FILE = 952, /**<  Failed to close a file in the file system. */
    E_INSUFFICENT_SPACE = 953, /**<  Not enough memory space for desired operation. */
    E_REMOVE_FILE = 954, /**<  Failed to remove a file from the file system. */
    E_RENAME_FILE = 955, /**<  Failed to rename a file from the file system. */
    E_CREATE_DIR = 956, /**<  Failed to create a new directory in the file system. */
    E_REMOVE_DIRECTORY = 957, /**<  Failed to remove directory. */
    E_FREE_SPACE = 958, /**<  No free space left. */
    E_ITEM_STAT = 959, /**<  Failed to retrieve status data. */
    E_CLOSE_DIRECTORY = 960, /**<  Failed to close a directory in the file system. */
    E_MOUNT_VOLUME = 961, /**<  Failed to mount volume in the file system. */
    E_READ_DIRECTORY = 962, /**<  Failed to read directory in the file system. */
    E_FS_IO = 963, /**<  Input/output error. */
    E_FS_ARGUMENT_LIST_2BIG = 964, /**<  Argument list too long. */
    E_FS_BAD_FILE_DESC = 965, /**<  Bad file descriptor. */
    E_FS_ACCESS = 966, /**<  Permission denied. */
    E_FS_BAD_ADDRESS = 967, /**<  Bad address. */
    E_FS_FILE_EXIST = 968, /**<  File exists. */
    E_FS_NOT_DIR = 969, /**<  Not a directory. */
    E_FS_IS_DIR = 970, /**<  Is a directory. */
    E_FS_FILE_TOO_LARGE = 971, /**<  File too large. */
    E_FS_READ_ONLY_FS = 972, /**<  Read-only file system. */
    E_FS_OPER_NOT_SUPP = 973, /**<  Operation not supported. */
    E_FS_NAME_TOO_LONG = 974, /**<  File name too long. */
    E_FS_OPERATION_CANCELED = 975, /**<  Operation canceled. */
    E_FS_FAIL = 976, /**<  Cannot start operation. */
    E_FS_INTERNAL = 977, /**<  Internal error. */
    E_FS_NOT_MOUNTED = 978, /**<  Volume not mounted. */
    E_FS_NOT_PERMITED = 979, /**<  Operation not permitted. */
    E_FS_NO_SUCH_FILE_OR_DIR = 980, /**<  No such file or directory. */
    E_FS_NOT_EXIST = 981, /**<  Specified FS does not exist. */
    E_FS_UNKNOWN_ERROR = 982, /**<  Error is unknown. */
    E_FAILED_WRITING_TO_FILE = 983, /**<  Failed to write to file. */
    E_FAILED_READING_FROM_FILE = 984, /**<  Failed to read from the specified file. */
    E_ACCESS_DENIED = 985, /**<  The access permission attributes do not allow operation. */
    E_CANNOT_OPEN_FILE = 986, /**<  Can not open the specified file. */
    E_FAILED_TO_STOP_FS = 987, /**<  Can not stop file system. */
    E_FILE_NAME_TOO_LONG = 988, /**<  File name is too long. */
    E_FAILED_TO_FIND_ELF_SECTION = 989, /**<  Can not find elf section. */
    E_MAX_NUMBER_OF_MOUNTED_VOLUMES_EXCEEDED = 990, /**<  Unable to mount volume due to exceeding the maximum number of allowed volumes. */
    E_NO_MOUNTED_DEVICES_ARE_FOUND = 991, /**<  Information message that no mounted devices are found. */
    E_NOT_FREE_CMD_SPACE = 1000, /**<  Index for new command is not founded in execution queue. */
    E_INVALID_INPUT_PARAMETER = 1001, /**<  Invalid input parameter */
    E_UNSUPPORTED_CMD = 1002, /**<  The loader does not support the requested command. */
    E_UNSUPPORTED_GROUP = 1003, /**<  The loader does not support the requested group. */
    E_INVALID_COMMAND_SIZE = 1051, /**<  The size of the command is invalid. */
    E_OVERLOAD_COMMAND_TABLE = 1052, /**<  Too many commands are registered. */
    E_COMMAND_ALREADY_REGISTERED = 1053, /**<  Command has already registered in execution queue. */
    E_AUDITING_FAILED = 1054, /**<   Command auditing failed. */
    E_NONEXIST_TIMER = 1251, /**<  Timer with the specified index does not exist. */
    E_FAILED_TO_SET_TIMER = 1252, /**<  Failed to start the timer. */
    E_TIMER_INIT_FAILED = 1253, /**<  Timer initialization failed. */
    E_TIMER_IRQ_CONF_FAILED = 1254, /**<   */
    E_GD_INVALID_UNIT_SIZE = 1351, /**<  GD/GDFS: Error in specifying unit size. */
    E_GD_LL_ILLEGAL_SIZE = 1352, /**<  GD/GDFS: Size too large for the block. */
    E_GD_LL_WRITE_FAILED = 1353, /**<  GD/GDFS: Write failed on FLASH device level. */
    E_GD_LL_ERASE_FAILED = 1354, /**<  GD/GDFS: Erase failed on FLASH device level. */
    E_GD_LL_UNKNOWN_DEVICE = 1355, /**<  GD/GDFS: FLASH device unknown. */
    E_GD_STARTUP_LOG_BLK_MISSING = 1356, /**<  GD/GDFS: Logical block not found during start-up scan. */
    E_GD_STARTUP_DUPLICATE_LOG_BLK = 1357, /**<  GD/GDFS: Duplicate instances of same logical block. */
    E_GD_ERASE_ILLEGAL_BLK_NR = 1358, /**<  GD/GDFS: Attempt to erase non-existing physical block. */
    E_GD_FG_UNIT_NOT_FOUND = 1359, /**<  GD/GDFS: Requested unit not found, probably never written or has been deleted. */
    E_GD_FG_UNIT_SIZE_MISMATCH = 1360, /**<  GD/GDFS: Attempt to access a unit outside its beyond its end. */
    E_GD_FG_ILLEGAL_LOG_BLK_NR = 1361, /**<  GD/GDFS: Attempt to access a logical block that does not exist. (Hardware, fatal). */
    E_GD_FG_ILLEGAL_PHYS_BLK_NR = 1362, /**<  GD/GDFS: Attempt to access a physical block that does not exist (internal error). */
    E_GD_FG_BLK_FULL = 1363, /**<  GD/GDFS: Attempted to write more data to a block than could be placed into one FLASH block. */
    E_GD_FG_NO_BLK_FREE = 1364, /**<  GD/GDFS: Internal error (no free blocks are available). */
    E_GD_FG_UNIT_CHECKSUM = 1365, /**<  GD/GDFS: The checksum or a unit being read is wrong. */
    E_GD_FG_NOT_DIRECT_BLOCK = 1366, /**<  GD/GDFS: Block is not direct. */
    E_GD_FG_NOT_FREE_BLOCK = 1367, /**<  GD/GDFS: For some reason, a free block could not be properly erased. */
    E_GD_FG_ILLEGAL_SIZE = 1368, /**<  GD/GDFS: The size of a unit being written exceeds the maximum limit of a unit. */
    E_GD_E_FG_FREE_AREA_DIRTY = 1369, /**<  GD/GDFS: The startup check found non-FF bytes in unused range within a block. */
    E_GD_FG_SYNC_FAILED = 1370, /**<  GD/GDFS: Failed to synchronize. */
    E_GD_NOT_OPEN = 1371, /**<  GD/GDFS: Tried to access data although GD was in closed state. */
    E_GD_NOT_ALLOWED = 1372, /**<  GD/GDFS: Operation not allowed in the current mode. */
    E_GD_ALREADY_OPEN = 1373, /**<  GD/GDFS: Tried to open or format when GD was already opened. */
    E_GD_FRONKENSTIENS_PATTERN_MISMATCH = 1374, /**<  GD/GDFS: The id mark of the GDVAR file does not match the data in the flash. Fatal and makes GD read only. */
    E_GD_ILLEGAL_ALIGNMENT = 1375, /**<  GD/GDFS: Tried to erase at an address that was not aligned to a flash block boundary. Internal error. */
    E_GD_ILLEGAL_SIZE = 1376, /**<  GD/GDFS: The operation may not be performed with the specified size. Tried to write a to large unit. */
    E_GD_ACCESS_DENIED = 1377, /**<  GD/GDFS: Operation not allowed. Flash device protected. Possible wrong flash driver configuration. */
    E_GD_ILLEGAL_INDEX = 1378, /**<  GD/GDFS: Illegal GD index. */
    E_GD_MEMORY_ALLOCATION_FAILED = 1379, /**<  GD/GDFS: Fatal. Failed to allocate dynamic memory. */
    E_GD_MISSING_CONFIG = 1380, /**<  GD/GDFS: Missing configuration parameter. */
    E_GD_ILLEGAL_CONFIG = 1381, /**<  GD/GDFS: Illegal configuration. */
    E_GD_TRANSACTION_LOG_CORRUPT = 1382, /**<  GD/GDFS: The transaction log used for tracking updates to GD contents is corrupt and prevents roll-back from working properly. */
    E_CSPSA_RESULT_E_INVALID_KEY = 1383, /**<  Error, parameter key not valid. */
    E_CSPSA_RESULT_E_OUT_OF_SPACE = 1384, /**<  Error, there is not space enough on memory media to update the parameter area. */
    E_CSPSA_RESULT_E_NO_VALID_IMAGE = 1385, /**<  Error, no valid CSPSA image found. */
    E_CSPSA_RESULT_E_MEDIA_INACCESSIBLE = 1386, /**<  Memory media could not be accessed. */
    E_CSPSA_RESULT_E_READ_ONLY = 1387, /**<  Image is read-only. */
    E_CSPSA_RESULT_E_READ_ERROR = 1388, /**<  Error occurred while reading from media. */
    E_CSPSA_RESULT_E_WRITE_ERROR = 1389, /**<  Error occurred while writing to media. */
    E_CSPSA_RESULT_UNDEFINED = 1390, /**<  Represents an undefined value of this enum. */
    E_CSPSA_RESULT_E_END_OF_DATA = 1391, /**<  No more parameters, end of data has been reached. */
    E_CSPSA_RESULT_E_OPEN_ERROR = 1392, /**<  Parameter storage area could not be opened (media error). */
    E_CSPSA_RESULT_E_ALREADY_EXISTS = 1393, /**<  Parameter storage area with same name was already registered. */
    E_CSPSA_RESULT_E_OUT_OF_MEMORY = 1394, /**<  There was not enough memory to perform the operation. */
    E_CSPSA_RESULT_E_BAD_PARAMETER = 1395, /**<  Error because of bad input parameter. */
    E_GD_NO_DATA_TO_READ = 1399, /**<  GD: No data to read. */
    E_BDM_W_NO_MORE_GC_POSSIBLE = 1451, /**<  BDM: No more garbage collection is possible. */
    E_BDM_UNIT_STARTED = 1452, /**<  BDM: Unit has already been started. */
    E_BDM_UNIT_NOT_STARTED = 1453, /**<  BDM: Unit has not yet been started. */
    E_BDM_NOT_CONFIGURED = 1454, /**<  BDM: Has not been configured yet. */
    E_BDM_STARTUP_FAILED = 1455, /**<  BDM: Startup failed. */
    E_BDM_SHUTDOWN_FAILED = 1456, /**<  BDM: Shutdown failed. */
    E_BDM_WRITE_FAILED = 1457, /**<  BDM: An error occurred while writing. */
    E_BDM_READ_FAILED = 1458, /**<  BDM: An error occurred while reading */
    E_BDM_ERASE_FAILED = 1459, /**<  BDM: An error occurred while erasing. */
    E_BDM_JUNK_FAILED = 1460, /**<  BDM: An error occurred while junking. */
    E_BDM_GC_FAILED = 1461, /**<  BDM: An error occurred while garbage collecting. */
    E_BDM_GET_INFO_FAILED = 1462, /**<  BDM: An error occurred while retrieving info about a BDM unit. */
    E_BDM_WRITE_NOT_SUPPORTED = 1463, /**<  BDM: Write not supported. */
    E_BDM_JUNK_NOT_SUPPORTED = 1464, /**<  BDM: Junk not supported. */
    E_BDM_GC_NOT_SUPPORTED = 1465, /**<  BDM: Garbage collection not supported. */
    E_BDM_SYNC_FAILED = 1466, /**<  BDM: Synchronize failed. */
    E_BDM_NOT_SUPPORTED = 1467, /**<  BDM: Function not supported. */
    E_BDM_PAUSED = 1468, /**<  BDM: Paused. */
    E_BDM_NOT_PAUSED = 1469, /**<  BDM: Not paused. */
    E_BDM_FINDING_BAM_BLOCKS = 1470, /**<  BDM: BAM blocks not found. */
    E_BDM_BAD_PARAM = 1471, /**<  BDM: Argument invalid or out of range. */
    E_BDM_FORMAT_FAILED = 1472, /**<  BDM: Formatting failed. */
    E_BDM_INVALID_UNIT = 1473, /**<  BDM: The unit number is out of range. */
    E_BDM_INVALID_CONFIG = 1474, /**<  BDM: Some part of the configuration is invalid. */
    E_BDM_PRE_FLASH_FINISHED = 1475, /**<  BDM: No more preflash blocks can be fetched. */
    E_BDM_PRE_FLASH_TERMINATE_FAILED = 1476, /**<  BDM: Termination of preflash failed. */
    E_BDM_OUT_OF_MEM = 1477, /**<  BDM: Could not allocate enough memory. */
    E_BDM_XSR_CRITICAL_ERROR = 1481, /**<  BDM XSR: Critical error. */
    E_BDM_XSR_INVALID_PARAMS = 1482, /**<  BDM XSR: Invalid parameters. */
    E_BDM_XSR_PARTITION_NOT_OPENED = 1483, /**<  BDM XSR: Could not open partition. */
    E_BDM_XSR_UNFORMATTED_FLASH = 1484, /**<  BDM XSR: Unformatted flash. */
    E_BDM_XSR_ALLOCATION_ERROR = 1485, /**<  BDM XSR: Failed to allocate. */
    E_BDM_XSR_INVALID_PARTITION = 1486, /**<  BDM XSR: Invalid partition. */
    E_BDM_XSR_READ_ERROR = 1487, /**<  BDM XSR: An error occurred while reading. */
    E_BDM_XSR_WRITE_ERROR = 1488, /**<  BDM XSR: An error occurred while writing. */
    E_BDM_XSR_ERASE_ERROR = 1489, /**<  BDM XSR: An error occurred while erasing. */
    E_BDM_XSR_DEVICE_ERROR = 1490, /**<  BDM XSR: Device error. */
    E_BDM_XSR_GOODBLOCK = 1491, /**<  BDM XSR: Good block. */
    E_BDM_XSR_BADBLOCK = 1492, /**<  BDM XSR: Bad block. */
    E_BAM_NOT_CONFIGURED = 1551, /**<  BAM: Is not configured. */
    E_BAM_ERR_UNSUPPORTED_PAGE_SIZE = 1552, /**<  BAM: Unsupported page size. */
    E_BAM_ERR_ERASING_BLOCK = 1553, /**<  BAM: Erase block could not be properly erased. */
    E_BAM_ERR_OUT_OF_MEMORY = 1554, /**<  BAM: Not enough free memory to serve request. */
    E_BAM_ERR_CHECKING_BADNESS = 1555, /**<  BAM: There was a problem checking if a block was bad or not. */
    E_BAM_ERR_FINDING_BLOCK = 1556, /**<  BAM: Reserved block could not be found. */
    E_BAM_ERR_READING_PAGE = 1557, /**<  BAM: An error occurred while reading a page in a block. */
    E_BAM_ERR_MARKING_BLOCK_BAD = 1558, /**<  BAM: A block went bad and BAM could not mark it as bad properly. */
    E_BAM_ERR_READING_OUTSIDE_BLOCK = 1559, /**<  BAM: Read would result in a read beyond end of reserved block. */
    E_BAM_ERR_UNCORRECTABLE_ERROR = 1560, /**<  BAM: An uncorrectable read error occurred while reading block. */
    E_BAM_ERR_WRITING_PAGE = 1561, /**<  BAM: An error occurred while writing a page to a reserved block. */
    E_BAM_ERR_WRITING_BLOCK = 1562, /**<  BAM: An error occurred while writing to a reserved block. */
    E_BAM_ERR_RESCUING_BLOCK = 1563, /**<  BAM: There was a problem rescuing a block that went bad during the request. */
    E_BAM_ERR_CONFIG_MISSING = 1564, /**<  BAM: No configuration was given. */
    E_BAM_ERR_ALREADY_STARTED = 1565, /**<  BAM: Has already been started. */
    E_BAM_ERR_NOT_STARTED = 1566, /**<  BAM: Has not been started yet. */
    E_BAM_ERR_UNSUPPORTED_OPERATION = 1567, /**<  BAM: Requested operation is not supported. */
    E_BAM_ERR_CORRUPT_STATE = 1568, /**<  BAM: Internal state has been corrupted. */
    E_BAM_ERR_UNSUPPORTED_MEDIA = 1569, /**<  BAM: The configured media type is not supported. */
    E_BAM_ERR_UNSUPPORTED_REDUNDANT_AREA_SIZE = 1570, /**<  BAM: Unsupported redundant area size. */
    E_BAM_ERR_READING_BLOCK = 1571, /**<  BAM: Reading block failed. */
    E_BAM_ERR_SHUTTING_DOWN = 1572, /**<  BAM: Shutting down failed. */
    E_BAM_ERR_UNINITIALIZING = 1573, /**<  BAM: An error occurred with uninitializing. */
    E_BAM_ERR_INVALID_OFFSET = 1574, /**<  BAM: Invalid offset. */
    E_BAM_ERR_INVALID_SIZE = 1575, /**<  BAM: Invalid size. */
    E_BAM_ERR_RETRIEVING_INFO = 1576, /**<  BAM: Retrieving info failed. */
    E_BAM_ERR_INVALID_CONFIG = 1577, /**<  BAM: Invalid configuration. */
    E_BAM_ERR_ERASING_ALL_BLOCKS = 1578, /**<  BAM: An error occurred while erasing all blocks. */
    E_BAM_ERR_BLOCK_MARKED_BAD = 1579, /**<  BAM: Block was marked as bad while doing an operation. */
    E_BAM_ERR_VALIDATING_START_BLOCK = 1580, /**<  BAM: Can not assure that start block configuration refers to a block boundary. */
    E_BAM_ERR_FAILED_TO_GET_MEMORYTECHNOLOGY = 1581, /**<  BAM: Unable the query FAM about the memory technology used in the memory we are trying to initialize BAM on. */
    E_BAM_ERR_BLOCK_RESCUED = 1582, /**<  BAM: Block rescue failed. */
    E_BAM_ERR_DETERMINING_BBM = 1583, /**<  BAM: Failed to determine what BBM type configuration has been written to flash. */
    E_BAM_ERR_SET_ECC_LENGTH = 1584, /**<  BAM: Failed to set the length to use with HW ECC acceleration. */
    E_BAM_ERR_INVALID_BBM_TYPE = 1585, /**<  BAM: Invalid BBM type. */
    E_BAM_ERR_BOOT_BDM_CONFIG_FAILED = 1586, /**<  BAM: Failed to configure boot block device management. */
    E_BAM_ERR_BOOT_BDM_STARTUP_FAILED = 1587, /**<  BAM: Failed to startup boot block device management. */
    E_BAM_ERR_BOOT_BDM_INSTANCE_NOT_FOUND = 1588, /**<  BAM: Boot block device management instance not found. */
    E_BAM_ERR_BOOT_BDM_BLOCKS_NOT_FOUND = 1589, /**<  BAM: Boot block device management blocks not found. */
    E_BAM_ERR_BOOT_BDM_NOT_STARTED = 1590, /**<  BAM: Boot block device management not started. Requested operation require BDM to be started. */
    E_BAM_ERR_UNIT_OUT_OF_RANGE = 1591, /**<  BAM: Unit is out of range. */
    E_BAM_ERR_BDM_STARTUP_FAILED = 1592, /**<  BAM: Block device management startup failed. */
    E_BAM_ERR_BLOCK_NOT_ERASED = 1593, /**<  BAM: Block not erased. */
    E_BAM_ONLD_CRITICAL_ERROR = 1601, /**<  BAM ONLD: Critical error. */
    E_BAM_ONLD_INVALID_PARAMS = 1602, /**<  BAM ONLD: Invalid parameters. */
    E_BAM_ONLD_INITIALISATION_ERROR = 1603, /**<  BAM ONLD: Initialization error. */
    E_BAM_ONLD_READ_ERROR = 1604, /**<  BAM ONLD: An error occurred while reading. */
    E_BAM_ONLD_WRITE_ERROR = 1605, /**<  BAM ONLD: An error occurred while writing. */
    E_BAM_ONLD_ERASE_ERROR = 1606, /**<  BAM ONLD: An error occurred while erasing. */
    E_BAM_ONLD_DEVICE_ERROR = 1607, /**<  BAM ONLD: Device error. */
    E_BAM_ONLD_GOODBLOCK = 1608, /**<  BAM ONLD: Good block. */
    E_BAM_ONLD_BADBLOCK = 1609, /**<  BAM ONLD: Bad block. */
    E_COPS_RC_ASYNC_CALL = 1621, /**<  COPS: Asynchronous call initiated. */
    E_COPS_RC_IPC_ERROR = 1622, /**<  COPS: IPC failed. */
    E_COPS_RC_INTERNAL_IPC_ERROR = 1623, /**<  COPS: Internal IPC lib error. */
    E_COPS_RC_ASYNC_IPC_ERROR = 1624, /**<  COPS: Asynchronous IPC is not supported/setup for this function. */
    E_COPS_RC_ARGUMENT_ERROR = 1625, /**<  COPS: Incorrect arguments for function. */
    E_COPS_RC_STORAGE_ERROR = 1626, /**<  COPS: Storage error (read/write flash failed). */
    E_COPS_RC_MEMORY_ALLOCATION_ERROR = 1627, /**<  COPS: Failed to allocate memory. */
    E_COPS_RC_UNSPECIFIC_ERROR = 1628, /**<  COPS: Unspecified error. */
    E_COPS_RC_SERVICE_NOT_AVAILABLE_ERROR = 1629, /**<  COPS: Service not available error. */
    E_COPS_RC_SERVICE_ERROR = 1630, /**<  COPS: Error in a service. */
    E_COPS_RC_NOT_AUTHENTICATED_ERROR = 1631, /**<  COPS: Not authenticated error. */
    E_COPS_RC_CHALLENGE_MISSING_ERROR = 1632, /**<  COPS: No challenge found. */
    E_COPS_RC_SIGNATURE_VERIFICATION_ERROR = 1633, /**<  COPS: Signature validation failed. */
    E_COPS_RC_DATA_TAMPERED_ERROR = 1634, /**<  COPS: Data tampered. */
    E_COPS_RC_DATA_CONFIGURATION_ERROR = 1635, /**<  COPS: Data configuration error. */
    E_COPS_RC_INCORRECT_SIM = 1636, /**<  COPS: SIM card is not OK with SIMLock settings. */
    E_COPS_RC_TIMER_RUNNING = 1637, /**<  COPS: A simlock unlock timer is currently running. */
    E_COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT = 1638, /**<  COPS: No simlock unlock attempts left. */
    E_COPS_RC_INVALID_SIMLOCK_KEY = 1639, /**<  COPS: The key is invalid. */
    E_COPS_RC_INTERNAL_ERROR = 1640, /**<  COPS: Internal error. */
    E_COPS_RC_LOCKING_ERROR = 1641, /**<  COPS: Not allowed to lock a simlock. */
    E_COPS_RC_UNLOCK_ATTEMPTS_STILL_LEFT = 1642, /**<  COPS: Still unlock attempts left for the lock. */
    E_COPS_RC_ROUTED = 1643, /**<  COPS: Message should be routed. */
    E_COPS_RC_AUTOLOCK_NOT_ALLOWED = 1644, /**<  COPS: Autolock only allowed during first boot. */
    E_COPS_MEMORY_ALLOC_FAILED = 1651, /**<  COPS: Memory allocation failed. */
    E_COPS_DATA_TAMPERED = 1652, /**<  COPS: Data is tempered. */
    E_COPS_IMEI_MISSMATCH = 1653, /**<  COPS: IMEI mismatch. */
    E_COPS_OTP_LOCKED = 1654, /**<  COPS: OTP is locked. */
    E_COPS_MAC_FUNCTION_LOCKED_DOWN = 1655, /**<  COPS: Function for calculating MAC is locked down. */
    E_COPS_AUTHENTICATION_FAILED = 1656, /**<  COPS: Authentication failed. */
    E_COPS_DATA_NOT_PRESENT = 1657, /**<  COPS: Default Data is not present. */
    E_COPS_IMEI_UPDATE_NOT_ALLOWED = 1658, /**<  COPS: IMEI update is not allowed. */
    E_COPS_LOCK_PERMANENTLY_DISABLED = 1659, /**<  COPS: Locking of SIMLocks is disabled. */
    E_COPS_NO_ATTEMPTS_LEFT = 1660, /**<  COPS: No more attempts for verification left. */
    E_COPS_INCORRECT_CONTROLKEY = 1661, /**<  COPS: Control key is not correct. */
    E_COPS_TOO_SHORT_CONTROLKEY = 1662, /**<  COPS: Control key is too short. */
    E_COPS_TOO_LONG_CONTROLKEY = 1663, /**<  COPS: Control key is too long. */
    E_COPS_INVALID_CONTROLKEY = 1664, /**<  COPS: Control key is not valid. */
    E_COPS_TIMER_RUNNING = 1665, /**<  COPS: Timer is running. */
    E_COPS_SIM_ERROR = 1666, /**<  COPS: SIM error. */
    E_COPS_LOCKING_FAILED = 1667, /**<  COPS: Locking failed. */
    E_COPS_OTA_UNLOCK_IMEI_MISMATCH = 1668, /**<  COPS: OTA unlock IMEI mismatch. */
    E_COPS_INCORRECT_IMSI = 1669, /**<  COPS: Incorrect IMSI. */
    E_COPS_PARAMETER_ERROR = 1670, /**<  COPS: Parameter error. */
    E_COPS_BUFFER_TOO_SMALL = 1671, /**<  COPS: Memory buffer is too small. */
    E_COPS_FORBIDDEN_PARAMETER_ID = 1672, /**<  COPS: Parameter is not allowed. */
    E_COPS_UNKNOWN_PARAMETER_ID = 1673, /**<  COPS: Parameter can not be recognized. */
    E_COPS_ARGUMENT_ERROR = 1674, /**<  COPS: Argument error! */
    E_COPS_VERIFY_FAILED = 1698, /**<  COPS: Failed to verify internal data. */
    E_COPS_UNDEFINED_ERROR = 1699, /**<  COPS: Undefined error. */
    E_PD_NAND_RESULT_BIT_ERROR_CORRECTED = 1751, /**<  PD NAND: A bit error was detected and corrected. */
    E_PD_NAND_RESULT_UNCORRECTABLE_BIT_ERROR = 1752, /**<  PD NAND: An uncorrectable bit error was detected. */
    E_PD_NAND_RESULT_BAD_PARAMETER = 1753, /**<  PD NAND: The function could not perform the requested operation due to a bad parameter. */
    E_PD_NAND_RESULT_HW_ERROR = 1754, /**<  PD NAND: A hardware error occurred. */
    E_PD_NAND_RESULT_INTERNAL_ERROR = 1755, /**<  PD NAND: A module internal error has occurred. The module has reach an unexpected state or request. */
    E_PD_NAND_RESULT_BUSY = 1756, /**<  PD NAND: Busy flag was returned. */
    E_PD_NAND_RESULT_READING_ERASED_PAGE = 1757, /**<  PD NAND: Attempting to read erased page. */
    E_PD_NAND_RESULT_NUMBER_OF_ITEMS = 1758, /**<  PD NAND: Number of valid states of this type. */
    E_PD_NAND_RESULT_UNDEFINED = 1759, /**<  PD NAND: Represents an undefined value of this type. */
    E_GD_TA_BASE = 1851, /**<  GD/TA: TA base. */
    E_GD_TA_UNKNOWN_PARTITION = 1852, /**<  GD/TA: Unknown partition. */
    E_GD_TA_UNKNOWN_CONFIG = 1853, /**<  GD/TA: Unknown configuration. */
    E_GD_TA_ILLOGICAL_CONFIGURATION = 1854, /**<  GD/TA: Illogical configuration. */
    E_GD_TA_UNKNOWN_MEMORY_TYPE = 1855, /**<  GD/TA: Unknown memory type. */
    E_GD_TA_WRONG_PARAMETER = 1856, /**<  GD/TA: Wrong parameter. */
    E_GD_TA_OUT_OF_MEMORY = 1857, /**<  GD/TA: Out of memory. */
    E_GD_TA_INVALID_ADRESS = 1858, /**<  GD/TA: Invalid address. */
    E_GD_TA_UNUSED_ADRESS = 1859, /**<  GD/TA: Unused address. */
    E_GD_TA_UNIT_NOT_FOUND = 1860, /**<  GD/TA: Unit not found. */
    E_GD_TA_NOT_IMPLEMENTED = 1861, /**<  GD/TA: TA is not supported. */
    E_GD_TA_FAIL = 1862, /**<  GD/TA: TA fail. */
    E_GD_UNKNOWN_UNIT_NAME = 1863, /**<  GD: Unknown unit name. */
    E_GD_LAST = 1864, /**<  GD: Last enumeration (last valid + 1). */
    E_FAILED_TO_STORE_IN_FIFO = 1900, /**<  Failed to store data in FIFO. */
    E_FAILED_TO_SET_COMM_DEVICES = 1951, /**<  Failed to set parameters of communications devices. */
    E_FAILED_TO_STORE_IN_STACK = 1952, /**<  Failed to store in stack. */
    E_ZIP_PARSER_FILE_NOT_FOUND = 1954, /**<  The requested file is not found in the specified Zip archive. */
    E_ELF_FILE_FORMAT = 1955, /**<  The requested file is not an elf file. */
    E_ELF_OPEN_SECTION = 1956, /**<  Can not open elf section. */
    E_NAME_TOO_LONG = 1957, /**<  The name of parameter is too long. */
    E_BOOTRECORDS_MISMATCH = 1958, /**<  Error in boot records. */
    E_BOOTRECORD_EMPTY = 1959, /**<  Boot record is empty and operation cannot be executed. */
    E_INVALID_BOOTRECORD_IMAGE = 1960, /**<  Boot record image is not valid. */
    E_BOOTRECORD_FULL = 1961, /**<  Boot record has no space to accept new boot record. */
    E_BOOTRECORD_NOT_EXIST = 1962, /**<  Boot record not exist. */
    E_BOOTRECORD_WRITE_FAILED = 1963, /**<  Writing failed. */
    E_BOOTRECORD_UNALIGNED_DATA = 1964, /**<  Boot record data is unaligned. */
    E_CONTENT_TYPE = 1965, /**<  Content type to long. */
    E_DEVICE_TYPE = 1966, /**<  Device type to long. */
    E_NUMBER_OF_TARGET_DEVICES = 1967, /**<  Number of target devices to big. */
    E_DESCRIPTION_TOO_LONG = 1968, /**<  Description field too long. */
    E_MODULE_NOT_FOUND = 2000, /**<  Testing this module currently not supported in ADbg. */
    E_MODULE_LIST_EMPTY = 2001, /**<  There isn't any module available for testing. */
    E_CASE_NOT_FOUND = 2002, /**<  Specified case can not be found. */
    E_CASE_LIST_EMPTY = 2003, /**<  There isn't any test cases in specified module. */
    E_INT_GROUP_NOT_FOUND = 2004, /**<  Specified internal group can not be found. */
    E_INT_GROUP_LIST_EMPTY = 2005, /**<  There isn't any internal group. */
    E_INT_FUNCTION_NOT_FOUND = 2006, /**<  Specified interface function cannot be found. */
    E_INT_FUNCTION_LIST_EMPTY = 2007, /**<  There isn't any interface function in interface group. */
    E_PRECONDITION_IS_ALREADY_SET = 2008, /**<  Request for setting precondition that is already set. */
    E_PRECONDITION_IS_NOT_SET = 2009, /**<  Request to recover condition that is not changed. */
    E_INIT_OTP_PD_FAILED = 2100, /**<  Failed to initialize OTD driver. */
    E_READING_OTP_FAILED = 2101, /**<  Failed to read data from OTP. */
    E_WRITTING_OTP_FAILED = 2102, /**<  Failed to write data in OTP. */
    E_INVALID_CID_VALUE = 2103, /**<  Trying to write invalid CID in OTP. */
    E_INIT_OTP_LD_FAILED = 2104, /**<  Failed to initialize OTD logical driver. */
    E_OTP_AREA_LOCKED = 2105, /**<  Requested OTP area is already locked. */
    E_SEC_APP_PROPERTY_NOT_FOUND = 2251, /**<  App property cannot be found. */
    E_SEC_APP_IMEI_NOT_CHANGABLE = 2252, /**<  IMEI not changeable. */
    E_SEC_APP_OPERATION_DENIED = 2253, /**<  Security operation denied. */
    E_SEC_APP_UNABLE_TO_READ_BS_PARAMETERS = 2254, /**<  Incorrect Boot Stage Parameter vector. */
    E_SEC_APP_ROM_ERROR_CRITICAL = 2255, /**<  Critical error in ROM has occurred. */
    E_SEC_APP_ROM_ERROR = 2256, /**<  Unexpected ROM error. */
    E_SEC_APP_PATCH_EXISTS = 2257, /**<  ROM Patch is already installed. */
    E_SEC_APP_PATCH_REINSTALLED = 2258, /**<  Operation successful. */
    E_SEC_APP_ROOTKEY_EXISTS = 2259, /**<  RootKey is already installed. */
    E_SEC_APP_ROOTKEY_REINSTALLED = 2260, /**<  Operation successful. */
    E_TA_WRONG_PARTITION = 2351, /**<  TA: Unknown partition. */
    E_TA_ILLOGICAL_CONFIGURATION = 2352, /**<  TA: Configuration error. */
    E_TA_UNKNOWN_MEMORY_TYPE = 2353, /**<  TA: Unsupported memory type. */
    E_TA_WRONG_PARAMETER = 2354, /**<  TA: Wrong parameter. */
    E_TA_OUT_OF_MEMORY = 2355, /**<  TA: No heap memory left. */
    E_TA_INVALID_ADRESS = 2356, /**<  TA: Invalid address. */
    E_TA_UNUSED_ADRESS = 2357, /**<  TA: Unused address. */
    E_TA_UNIT_NOT_FOUND = 2358, /**<  TA: Unit was not found. */
    E_TA_WRONG_SIZE = 2359, /**<  TA: Wrong size when reading unit. */
    E_TA_INSUFFICIANT_SPACE = 2360, /**<  TA: Not enough space to flush the Trim Area. */
    E_TA_UNKNOWN_PARTITION = 2361, /**<  TA: Unknown partition. */
    E_TA_FAIL = 2362, /**<  TA: Fail. */
    E_TA_MEDIA_ERROR = 2460, /**<  TA: Media error. */
    E_TA_NOT_CONFIGURED = 2461, /**<  TA: Error occurs during configuration. */
    E_REQUEST_DENIED = 2500, /**<  Request for change operation denied. */
    E_UNDEFINED_CHANGE_OPERATION = 2501, /**<  Requested change operation is not supported. */
    E_PD_CFI_IN_PROGRESS = 2651, /**<  PD CFI: The operation is in progress. Additional poll calls must be done. */
    E_PD_CFI_UNKNOWN_REQUEST = 2652, /**<  PD CFI: Type not recognized. */
    E_PD_CFI_HARDWARE_ERROR = 2653, /**<  PD CFI: Operation could not be completed because of a hardware malfunction. */
    E_PD_CFI_NOT_SUPPORTED = 2654, /**<  PD CFI: The driver does not implement the requested function. */
    E_PD_CFI_PARAMETER_ERROR = 2655, /**<  PD CFI: Invalid parameter value. */
    E_PD_CFI_PROTECTED = 2656, /**<  PD CFI: The requested flash address is protected from the requested type of access. */
    E_PD_CFI_UNSUPPORTED_DEVICE = 2657, /**<  PD CFI: Flash devices not supported by this driver. */
    E_PD_CFI_OPERATION_COMPLETE = 2658, /**<  PD CFI: Requested operation was not suspended as it has completed. */
    E_PD_CFI_HARDWARE_NOT_DETECTED = 2659, /**<  PD CFI: NOR memory was not detected on current configuration. */
    E_HSI_BSC_RESULT_FAILED = 2701, /**<  General Error. */
    E_HSI_BSC_RESULT_FAILED_INVALIDARGS = 2702, /**<  Invalid arguments. */
    E_HSI_BSC_RESULT_FAILED_NOTSUPPORTED = 2703, /**<  Implementation is removed. */
    E_HSI_BSC_RESULT_FAILED_HSIERR = 2704, /**<  HSI internal error. */
    E_HSI_BSC_RESULT_FAILED_BUSY = 2705, /**<  Communication device is busy. */
    E_HSI_BSC_ERROR_SIGNAL = 2751, /**<  Signal error. */
    E_HSI_BSC_ERROR_TIMEOUT = 2752, /**<  Timeout error. */
    E_HSI_BSC_ERROR_BREAK = 2753, /**<  Break received. */
    E_HSI_BSC_ERROR_RECEIVE = 2754, /**<  Receive buffer is not provided. */
    E_HSI_BSC_ERROR_TRANSMIT = 2755, /**<  Transmit buffer is not provided. */
    E_CR_INVALID_STATE = 2850, /**<  Invalid state for communication relay detected. */
    E_CR_NOT_RUNNING = 2851, /**<  Communication Relay is not running. */
    E_SDIO_BSC_RESULT_FAILED = 2901, /**<  General Error. */
    E_SDIO_BSC_RESULT_FAILED_INVALIDARGS = 2902, /**<  Invalid arguments. */
    E_SDIO_BSC_RESULT_FAILED_NOTSUPPORTED = 2903, /**<  Implementation is removed. */
    E_SDIO_BSC_RESULT_FAILED_SDIOERR = 2904, /**<  SDIO internal error. */
    E_SDIO_BSC_RESULT_FAILED_BUSY = 2905, /**<  Communication device is busy. */
    E_SDIO_BSC_ERROR_TIMEOUT = 2951, /**<  Timeout error. */
    E_SDIO_BSC_ERROR_BREAK = 2952, /**<  Break received. */
    E_SDIO_BSC_ERROR_RECEIVE = 2953, /**<  Receive buffer is not provided. */
    E_SDIO_BSC_ERROR_TRANSMIT = 2954, /**<  Transmit buffer is not provided. */
    E_BAM_INVALID_HANDLE = 3051, /**<  The handle supplied to BAM is invalid. */
    E_BAM_ALREADY_INITIALIZED = 3052, /**<  The instance if BAM has already been initialized. */
    E_BAM_INVALID_CONF = 3053, /**<  BAM configuration is invalid. */
    E_BAM_BAD_PARAM = 3054, /**<  One of the parameter passed to BAM makes no sense. */
    E_BAM_ENOMEM = 3055, /**<  BAM could not allocate enough memory to complete the operation. */
    E_BAM_ENOENT = 3056, /**<  BAM could not locate the requested image/partition. */
    E_BAM_EEXIST = 3057, /**<  An image with the same ID has already been written. */
    E_BAM_ENOSPACE = 3058, /**<  BAM could not complete the operation due to insufficient storage space. */
    E_BAM_ERANGE = 3059, /**<  A TOC entry specifies a location outside the area. */
    E_BAM_INVALID_TOC = 3060, /**<  The TOC passed to BAM was detected as corrupt. */
    E_BAM_INVALID_AREA = 3061, /**<  An area found in the TOC or passed to BAM is invalid. */
    E_BAM_NOT_INITIALIZED = 3062, /**<  The instance or update mode of BAM has not been initialized. */
    E_BAM_HW_ERR = 3063, /**<  BAM was unable to perform the operation due to failure in HW. */
    E_BAM_STATE_CORRUPT = 3064, /**<  The BAM state is corrupt. */
    E_BAM_NOT_ALLOWED = 3065, /**<  The operation is not allowed. */
    E_BAM_FS_ERROR = 3066, /**<  An Operation towards the FileSystem return an unexpected result. */
    E_BAM_BAD_FS_STATE = 3067, /**<  BAM state files in the FS is not in the expected state. */
    E_BAM_HASH_ERROR = 3068, /**<  The hash callback function failed unexpectedly. */
    E_BAM_UPDATE_IN_PROGRESS = 3069, /**<  An update is already in progress. */
    E_BAM_BUF_TOO_SMALL = 3070, /**<  The supplied buffer is to small.The image will not be written. */
    E_BAM_EXTERNAL_ERROR = 3071, /**<  An external error occurred. */
    E_BAM_NOT_INSTANTIATED = 3072, /**<  The requested function is not compiled into the loadmodule that supplies the service. */
    E_BAM_UPDATE_NOT_POSSIBLE = 3073, /**<  The state in the flash prevents BAM from performing any updates. */
    E_LOADER_SEC_LIB_CHIP_ID_INVALID = 4000, /**<  Invalid input parameters. */
    E_LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC = 4096, /**<  Invalid input parameters. */
    E_LOADER_SEC_LIB_FAILURE = 4120, /**<  Failure. */
    E_LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE = 4121, /**<  hash list verification failed. */
    E_LOADER_SEC_LIB_HEADER_VERIFICATION_FAILURE = 4122, /**<  Header verification failed.  */
    E_LOADER_SEC_LIB_HEADER_VERIFIED = 4123, /**<  Successful verification of the header */
    E_LOADER_SEC_LIB_VERIFY_FAILURE = 4124, /**<  Unsuccessful verification. */
    E_LOADER_SEC_LIB_INIT_CALLED_TWICE = 4150, /**<  The security library init function has been called 2 times. */
    E_LOADER_SEC_LIB_MEMORY_RELEASE_FAILED = 4180, /**<  Memory release failed. */
    E_LOADER_SEC_LIB_MEMORY_ALLOCATION_FAILED = 4181, /**<  Memory allocation failed. */
    E_LOADER_SEC_LIB_DATA_BLOCK_EXIST = 4182, /**<  Data block exist in the linked list. */
    E_LOADER_SEC_LIB_DATA_BLOCK_DO_NOT_EXIST = 4183, /**<  Data block do not exist in the linked list. */
    E_LOADER_SEC_LIB_INVALID_AUTHENTICATION_TYPE = 4184, /**<  Invalid authentication type. */
    E_LOADER_SEC_LIB_EXCEEDED_NUMBER_OF_AUTHENTICATION = 4185, /**<  Exceeded number of authentication. Loader will be shut downed. */
    E_LOADER_SEC_LIB_ESB_MAC_INIT_FAILED = 4186, /**<  Initialization of ESB block for MAC calculation failed. */
    E_LOADER_SEC_LIB_ESB_MAC_UPDATE_FAILED = 4187, /**<  MAC update with ESB block failed. */
    E_LOADER_SEC_LIB_ESB_MAC_FINAL_FAILED = 4188, /**<  MAC finalize with ESB block failed. */
    E_LOADER_SEC_LIB_ESB_MAC_NOT_VERIFIED = 4189, /**<  MAC verification with ESB block failed. */
    E_LOADER_SEC_LIB_ESB_DOWNLOCK_FAILED = 4190, /**<  ESB downlock failed. */
    E_LOADER_SEC_LIB_CONTROL_KEY_VERIFICATION_FAILURE = 4191, /**<  Control key verification failed. */
    E_LOADER_SEC_LIB_CA_CERTIFICATE_VERIFICATION_FAILURE = 4192, /**<  CA certificate verification failed. */
    E_LOADER_SEC_LIB_X509_ERROR_IN_CERTIFICATE = 4193, /**<  X509 certificate error. */
    E_LOADER_SEC_LIB_COPS_INIT_FAILED = 4194, /**<  COPS initialization failed. */
    E_LOADER_SEC_LIB_COPS_PROTECT_DATA_INIT_FAILED = 4195, /**<  COPS protect data initialization failed. */
    E_LOADER_SEC_LIB_COPS_DATA_READ_FAILED = 4196, /**<  COPS data read failed. */
    E_LOADER_SEC_LIB_COPS_DATA_WRITE_FAILED = 4197, /**<  COPS data write failed. */
    E_LOADER_SEC_LIB_READ_OTP_FAILED = 4198, /**<  Reading OTP data failed. */
    E_LOADER_SEC_LIB_WRITE_OTP_FAILED = 4199, /**<  Writing OTP data failed. */
    E_LOADER_SEC_LIB_LOCK_OTP_FAILED = 4200, /**<  Locking OTP data failed. */
    E_LOADER_SEC_LIB_UNPACKING_IMEI_FAILED = 4201, /**<  Unpacking IMEI data failed. */
    E_LOADER_SEC_LIB_PACKING_IMEI_FAILED = 4202, /**<  Packing IMEI data failed. */
    E_LOADER_SEC_LIB_OTP_ALREADY_LOCKED = 4203, /**<  OTP is already locked. */
    E_LOADER_SEC_LIB_INVALID_CID_VALUE = 4204, /**<  CID value is out of range. */
    E_LOADER_SEC_LIB_OTP_LOCKBITS_MISSMATCH = 4205, /**<  OTP lock bits have different values. */
    E_LOADER_SEC_LIB_WRITING_BOOTRECORD_FAILED = 4206, /**<  Failed to write in boot records. */
    E_LOADER_SEC_LIB_UNSUPPORTED_NO_DEBUG_HW = 4207, /**<  No debug hardware detected. */
    E_LOADER_SEC_LIB_CHANGE_OPERATION_NOT_SUPPORTED = 4208, /**<  Requested change operation is not supported or not allowed. */
    E_LOADER_SEC_LIB_INVALID_CHANGE_OPERATION = 4209, /**<  Invalid change operation. */
    E_LOADER_SEC_LIB_RWIMEI_NOT_ALLOWED = 4210, /**<  Re-writable IMEI is not allowed to change. */
    E_LOADER_SEC_LIB_REQUEST_DENIED = 4211, /**<  Request for change operation is denied. */
    E_LOADER_SEC_LIB_BOOT_BLOCK_DO_NOT_EXIST = 4212, /**<  Boot record do not exist. */
    E_LOADER_SEC_LIB_CORRUPTED_DOMAIN_DATA = 4213, /**<  Corrupted or do not exist domain data in boot block. */
    E_LOADER_SEC_LIB_INVALID_DOMAIN = 4214, /**<  Invalid domain. */
    E_LOADER_SEC_LIB_INVALID_CHALLENGE_DATA_BLOCK = 4215, /**<  Invalid challenge data block. */
    E_LOADER_SEC_LIB_NO_DEBUG_HW_NOT_ALLOWED = 4216, /**<  Not allowed operation on NoDebug HW. */
    E_FIFO_OVERFLOW = 4300, /**<  UART FIFO overflow. */
    E_FIFO_UNDERFLOW = 4301, /**<  UART FIFO underflow. */
    E_OBJECT_NULL = 4302, /**<  Null pointer to Object. */
    E_POINTER_NOT_NULL = 4303, /**<  Pointer is not NULL. */
    E_UNRECOGNIZED_STATE = 4304, /**<  The state in the State Machine is invalid. */
    E_UNKNOWN_MANUFACTURER_ID = 4305, /**<  The provided manufacturer ID is not valid. */
    E_UNKNOWN_DEVICE_ID = 4306, /**<  The provided device ID is not valid. */
    E_INVALID_A01_FORMAT = 4308, /**<  Something wrong with the A01 file containing the flash image. */
    E_A01_BUFFER_FULL = 4309, /**<  Buffer holding the data from the A01 file is full. */
    E_CONFIG_FILE_NOT_SPECIFIED = 4350, /**<  Configuration file for the emulator is not specified. */
    A2_E_SUCCESS = 5000, /**<  Operation successful. */
    A2_E_PROP_NOT_SUPPORTED = 5001, /**<  The property is not supported. */
    A2_E_PROP_READ_ONLY = 5002, /**<  The property is read only. */
    A2_E_PROP_INVALID = 5003, /**<  The property value is invalid. */
    A2_E_AUTH_DECLINED = 5004, /**<  Authentication declined. The ME is automatically shut down after sending this. */
    A2_E_AUTH_UNSUPPORTED = 5005, /**<  The authentication type is not supported. */
    A2_E_ALLOCATE_FAILED = 5006, /**<  Failed to allocate memory. */
    A2_E_INVALID_TIME = 5007, /**<  Invalid time specified. */
    A2_E_UNKNWON_PROPERTY = 5008, /**<  Unknown property id. */
    A2_E_START_AAIF_FAILED = 5009, /**<  Failed to start the AAIF in the loader on loader. */
    A2_E_UNSUPPORTED_CMD = 5010, /**<  Unsupported command. */
    A2_E_POINTER_NOT_ALIGNED = 5011, /**<  Pointer not aligned. */
    A2_E_ERROR_WRITING_BOOTRECORD = 5012, /**<  Writing of the boot record failed. */
    A2_E_INVALID_CURRDATE_STRING_LENGTH = 5013, /**<  String data length is invalid. */
    A2_E_NO_COMMAND_GROUPS_DEFINED = 5014, /**<  Unknown command group. */
    A2_E_READ_OTP_FAILED = 5015, /**<  Read OTP failed. */
    A2_E_COMMAND_IS_NOT_IMPLEMENTED = 5016, /**<  Specified command is not implemented. */
    A2_E_FAILED_TO_GET_ASIC_COPS_SETTINGS = 5017, /**<  Failed to get ASIC COPS setting. */
    A2_E_FAILED_TO_SET_PLAT_PROP = 5018, /**<  Failed to set platform properties. */
    A2_E_MEMORY_FAILED = 5019, /**<  Memory fail. */
    A2_E_JTAG_UNLOCK_FAILED = 5020, /**<  JTAG unlock fail. */
    A2_E_AUTOCONFIGURE_FLASH = 5021, /**<   */
    A2_E_E_GET_FIRST_FLASH_DEV = 5022, /**<   */
    A2_E_E_FAILED_INIT_COPS_LIB = 5023, /**<  Failed to initialize COPS library. */
    A2_E_OTP_SECURITY_ERROR = 5024, /**<  OTP security error. */
    A2_E_I2C_BUS_SECURITY_ERROR = 5025, /**<  I2C bus security error. */
    A2_E_GET_STATIC_DATA_FAILED = 5026, /**<  Can't get static data. */
    A2_E_STORE_MAC_TO_BOOTIMAGE_FAILED = 5027, /**<  Failed to store MAC in boot image. */
    A2_E_SEC_GENERAL_COPS_LIB_ERROR = 5028, /**<  General COPS error. */
    A2_E_HASH_VERIFICATION_ERROR = 5029, /**<  Hash verification failed. */
    A2_E_READ_FLASH_FAILED = 5030, /**<  Failed to read hash. */
    A2_E_APP_HANDSHAKE_FAILED = 5031, /**<  Processors handshake failed. */
    A2_E_BOOTIMAGE_MEMCONF_INVALID = 5032, /**<  Invalid memconfig in boot image. */
    A2_E_BOOTIMAGE_SIGNATURE_FAILED = 5033, /**<  Boot image signature failed. */
    A2_E_BOOTIMAGE_FAILED_TO_READ_IMAGE = 5034, /**<  Failed to read image. */
    A2_E_BOOTIMAGE_FAILED_ALLOCATE_MEM = 5035, /**<  Failed to allocate memory. */
    A2_E_BOOTIMAGE_INVALID_PARAM = 5036, /**<  Boot image invalid parameters. */
    A2_E_BOOTIMAGE_INVALID_LENGTH = 5037, /**<  Boot image has invalid length. */
    A2_E_BOOTIMAGE_MACED_HEADER_SIZE_ZERO = 5038, /**<  Header size is zero. */
    A2_E_FLASH_RESULT_DEVICE_PROTECTED = 5039, /**<  The flash device was protected. */
    A2_E_FLASH_RESULT_WRITE_SUSPENDED = 5040, /**<  The last write process was suspended. */
    A2_E_FLASH_RESULT_VOLTAGE_RANGE_ERROR = 5041, /**<  The voltage range is invalid. */
    A2_E_FLASH_RESULT_PROGRAM_ERROR = 5042, /**<  Failed to write to the flash device. */
    A2_E_FLASH_RESULT_ERASE_ERROR = 5043, /**<  Failed to erase a block in the flash device. */
    A2_E_FLASH_RESULT_ERASE_SUSPENDED = 5044, /**<  The erase process was suspended. */
    A2_E_FLASH_RESULT_COMMAND_SEQUENCE_ERROR = 5045, /**<  The sequence of the flash commands was invalid. */
    A2_E_FLASH_RESULT_OPERATION_NOT_SUPPORTED = 5046, /**<  Operation was not supported in flashdriver. */
    A2_E_FLASH_RESULT_INVALID_PARAMETER = 5047, /**<  Invalid in-parameter specified when reading/writing to flash. */
    A2_E_FLASH_RESULT_NO_FLASH_DEVICE = 5048, /**<  No flash device was found on the physical address. */
    A2_E_FLASH_RESULT_CONFIGURATION_ERROR = 5049, /**<  Configuration error of flash device. */
    A2_E_FLASH_RESULT_INVALID_STARTADDRESS = 5050, /**<  Invalid start address of the parameter. */
    A2_E_FLASH_RESULT_INVALID_PHYSICAL_ADDRESS = 5051, /**<  Invalid physical address of the parameter. */
    A2_E_FLASH_RESULT_INVALID_DATALENGTH = 5052, /**<  The data length of the parameter is invalid. */
    A2_E_FLASH_RESULT_GETREGION_ERROR = 5053, /**<  Invalid flash region specified. */
    A2_E_FLASH_RESULT_NULL_POINTER_BUF = 5054, /**<  A buffer was NULL. */
    A2_E_FLASH_RESULT_NAND_READ_FAILED = 5055, /**<  Failed to read from the NAND flash. */
    A2_E_FLASH_RESULT_NAND_PAGE_SIZE_UNSUPPORTED = 5056, /**<  The nand page size is unsupported. */
    A2_E_FLASH_RESULT_WRITE_ERROR = 5057, /**<  Failed to write to the flash device. */
    A2_E_FLASH_RESULT_READ_ID_ERROR = 5058, /**<  Failed to read the device ID from the flash device. */
    A2_E_FLASH_RESULT_HARDWARE_ERROR = 5059, /**<  Hardware error in the flash status. */
    A2_E_FLASH_RESULT_READ_FROM_FLASH = 5060, /**<  Failed to read from the flash device. */
    A2_E_FLASH_RESULT_ADD_NEW_DEVICE = 5061, /**<  Failed to add a new instance of a flash device. */
    A2_E_FLASH_RESULT_OTP_UNSUPPORTED_IN_FLASH = 5062, /**<  OTP is not support in the current flash device. */
    A2_E_FLASH_RESULT_OTP_TOO_SMALL = 5063, /**<  The OTP size is to small for the length requested. */
    A2_E_FLASH_RESULT_OTP_READ_FAILED = 5064, /**<  Failed to read the OTP area in the flash device. */
    A2_E_FLASH_RESULT_INVALID_FLASH_TYPE = 5065, /**<  The flash type is unknown. */
    A2_E_FLASH_RESULT_NAND_FLUSH_ERROR = 5066, /**<  Failed to flush the NAND write buffer. */
    A2_E_FLASH_RESULT_NOR_FLUSH_ERROR = 5067, /**<  Failed to flush the NOR write buffer. */
    A2_E_INVALID_LOL_STATE = 5068, /**<  Invalid Loader on loader state. */
    A2_E_INVALID_STATUS_LOLSTATE = 5069, /**<  Invalid loader on loader status. */
    A2_E_WRITE_STATIC_DATA = 5070, /**<  Write static data failed. */
    A2_E_HEADER_NOT_VERIFIED_YET = 5071, /**<  Header is still not verified. */
    A2_E_ALL_BLOCKS_VERIFIED_FAILED = 5072, /**<  Blocks verification failed. */
    A2_E_HANDSHAKE_WITH_APP_SIDE = 5073, /**<  Handshake with App side failed. */
    A2_E_READ_STATIC_DATA = 5074, /**<  Failed to read static data. */
    A2_E_ENABLE_CLOCK_HARDWARE_FAILED = 5075, /**<  Failed to enable clock hardware. */
    A2_E_ENABLE_BLOCK_HARDWARE_FAILED = 5076, /**<  Failed to enable block hardware. */
    A2_E_NO_HEADER_TO_MAC_IN_BOOT = 5077, /**<  Indicates that the loader did not find any header in the bootimage records that could be MAC'ed. */
    A2_E_MMU_SETUP = 5078, /**<  MMU setup failed. */
    A2_E_INVALID_COMMAND_SIZE = 5079, /**<  Invalid command size. */
    A2_E_HASH_LIST_LENGTH_INVALID = 5080, /**<  Invalid hash list length. */
    A2_E_INVALID_INPUT_PARAMETERS = 5081, /**<  Invalid input parameters. */
    A2_E_FAILED_SETUP_MSL_DRIVER = 5082, /**<  MSL driver setup failed. */
    A2_E_REQUEST_DENIED = 5083, /**<  Request denied. */
    A2_E_ASIC_FUSES_INVALID = 5084, /**<  Fuses for the ASIC are invalid. */
    A2_E_STATIC_DATA_NOT_CHECKED = 5085, /**<  Static data is not checked. */
    A2_E_COPS_DATA_MAN_INIT = 5086, /**<  Failed to initialize the cops data man, this can be caused by an corrupt boot image. You probably just need to flash a platform software to get it work. */
    A2_E_UART_DRIVER_ERROR = 5087, /**<  UART driver error. */
    A2_E_INVALID_DOMAIN_FOR_LOADER_TYPE = 5088, /**<  Loader type is not compatible with this domain. */
    A2_E_FUNCTIONALITY_NOT_IN_PRODUCT = 5089, /**<  Functionality not supported. */
    A2_E_DEFALT_DATA_NOT_FOUND = 5090, /**<  Default data can not be found. */
    A2_E_FAILED_TO_MAC_HEADER = 5091, /**<  Failed to send the header to access side to be MAC'ed, could be something wrong with the header. */
    A2_E_APP_PRELOADER_NOT_STARTED = 5092, /**<  App preloader not started. */
    A2_E_INVALID_DEST_ADDRESS = 5093, /**<  Invalid destination address. */
    A2_E_SYS_APP_INIT_FAILURE = 5094, /**<  Failed to initialize the system application. */
    A2_E_FLASH_APP_INIT_FAILURE = 5095, /**<  Failed to initialize the flash application. */
    A2_E_SIGNATURE_APP_INIT_FAILURE = 5096, /**<  Failed to initialize the signature application. */
    A2_E_RESET_APP_INIT_FAILURE = 5097, /**<  Failed to initialize the reset application. */
    A2_E_INT_SEC_APP_INIT_FAILURE = 5098, /**<  Failed to initialize the internal security application. */
    A2_E_COPS_DATA_MAN_FORMAT = 5099, /**<  COPS data man Format. */
    A2_E_COPS_DATA_MAN_WRITE = 5100, /**<  COPS data man write. */
    A2_E_COPS_DATA_MAN_FLUSH = 5101, /**<  Failed to Flush the memory into the security partition. */
    A2_E_COPS_DATA_MAN_GETBLOCKSIZE = 5102, /**<  Failed to get the block size of a unit in the security partition. */
    A2_E_COPS_DATA_MAN_READBLOCK = 5103, /**<  Failed to read an index from the security partition. */
    A2_E_INVALID_FLASH_VAR_LENGTH = 5104, /**<  The bytes left to program should be less than a NAND page. */
    A2_E_BOOTIMAGE_TOO_BIG = 5105, /**<  Boot image is too big. */
    A2_E_GDFS_APP_INIT_FAILURE = 5106, /**<  App GD init failed. */
    A2_E_FILESYS_APP_INIT_FAILURE = 5107, /**<  Failed to initialize the file system. */
    A2_E_INVALID_SOURCE_DEST_ADDRESS = 5108, /**<  Invalid source or destination address. */
    A2_E_INVALID_RESPONSE_COMMAND = 5109, /**<  Invalid Response command number on the internal security command group, was expecting command 0xFF. */
    A2_E_INVALID_RESP_TO_CMD = 5110, /**<  The internal sec command response should have responded to another command. */
    A2_E_VERIFICATION_OF_WRITTEN_DATA_FAILED = 5111, /**<  The data programmed into flash was not the same as the data received! */
    A2_E_INVALID_NAND_PADMUX_SETTING = 5112, /**<  The PADMUX configuration has not been set. */
    A2_E_FLASH_DRIVER_FAILED_SET_BOOT_ADDR = 5113, /**<  Failed to set the start boot address, this address are used to read the static data. */
    A2_E_FLASH_BOOT_IS_NOT_EMPTY = 5114, /**<  if the static data was not found, the flash should be empty! */
    A2_E_BOOTIMAGE_PMC_NOT_NEEDED = 5115, /**<  This error code informs that found boot container holds information that is not PMC protected. */
    A2_E_UNSUPPORTED_PMC = 5116, /**<  The PMC ID is not supported. */
    A2_E_FSVN_VS_PMC = 5117, /**<  The PMC ID is not coordinated with FSVN. */
    A2_E_WRONG_PMC_START_COUNT = 5118, /**<  PMC start count to high or zero. */
    A2_E_UNSUITABLE_PMC_FOR_LOCATION = 5119, /**<  Unsuitable PMC for the location. */
    A2_E_PMC_MISSING = 5120, /**<  The PMC can not be found. */
    A2_E_VERSION_NOT_ACCEPTED = 5121, /**<  FSVN is too low. */
    A2_E_PMC_OVERFLOWN = 5122, /**<  All steps of the PMC are destroyed. */
    A2_E_PMC_ONCE_REQUIRED = 5123, /**<  New version of once protected module attempted to be loaded without ARB flag set. */
    A2_E_PMC_BAD_N_OF_STEPS = 5124, /**<  Unsuitable number of steps within PMC. */
    A2_E_UNIT_MISSING = 5125, /**<  Dynamic variable is missing. */
    A2_E_UNIT_TO_BIG = 5126, /**<  Dynamic variable found is to big. */
    A2_E_PARTMAN_READ_ERROR = 5127, /**<  Error reading partition manger status. */
    A2_E_PARTMAN_INIT_ERROR = 5128, /**<  Error initializing partition manger. */
    A2_E_BOOTIMAGE_INVALID_ALLIGNMENT = 5129, /**<  Boot container size is not word aligned. */
    A2_E_BOOTIMAGE_INSUFFICIENT_CONTAINERS = 5130, /**<  Not enough boot containers are allocated. */
    A2_E_BOOTIMAGE_INVALID_CONTAINER_TYPE = 5131, /**<  Invalid boot container type. At this position other type of container is expected. */
    A2_E_DATA_LENGTH_IS_NOT_ALIGNED = 5132, /**<  The data size must be aligned to 512 bytes when reading or writing pages to/from the NAND. */
    A2_E_LOADER_SWINIT_JTAG_UNLOCK_FAILED = 5133, /**<  TEST JTAG EANBLENODEBUG. */
    A2_E_BOOTIMAGE_RECORD_NOT_FOUND = 5134, /**<  Required record is not found in the boot containers. */
    A2_E_BUFFER_OVERFLOW = 5135, /**<  Buffer overflow. */
    A2_E_BUFFER_NOT_ALLOCATED = 5136, /**<  Buffer has not been allocated. */
    A2_E_WRONG_DATA_SIZE = 5137, /**<  Size of data has an unexpected value. */
    A2_E_NO_STATIC_DATA_IN_BOOTIMAGE = 5138, /**<  Static data in boot record is corrupted. */
    A2_E_NOT_ALLOWED_TO_FLASH = 5139, /**<  Not allowed to flash this block. */
    A2_E_UNABLE_TO_CHANGE_MMU_SETTINGS = 5140, /**<  Unable to change MMU settings. */
    A2_E_INVALID_HW_FOR_LOADER_SETTINGS = 5141, /**<  Loader settings are not compatible with loader settings. */
    A2_E_CABS_START_FAILED = 5142, /**<   */
    A2_E_CABS_READ_ERROR = 5143, /**<   */
    A2_E_CABS_WRITE_ERROR = 5144, /**<   */
    A2_E_ELF_DECODE_GENERAL_ERROR = 5145, /**<   */
    A2_E_ELF_INVALID_PARAMETER = 5146, /**<   */
    A2_E_PARTMAN_INIT_FAILED = 5147, /**<   */
    A2_E_PARTMAN_WRITE_FAILED = 5148, /**<   */
    A2_E_PARTMAN_READ_FAILED = 5149, /**<   */
    A2_E_PARTMAN_ERASE_ERROR = 5150, /**<   */
    A2_E_MBBS_CONFIG_ERROR = 5151, /**<   */
    A2_E_MBBS_READ_ERROR = 5152, /**<   */
    A2_E_MBBS_WRITE_ERROR = 5153, /**<   */
    A2_E_MBBS_ERASE_ERROR = 5154, /**<   */
    A2_E_FLASH_HW_CONFIG = 5155, /**<   */
    A2_E_VSP_IS_NOT_ALLOWED = 5156, /**<  Virtual security partition is not allowed in this configuration. */
    A2_E_VSP_WRITE_STATIC_DATA = 5157, /**<  Failed to write static data to virtual security partition. */
    A2_E_VSP_READ_STATIC_DATA = 5158, /**<  Failed to read static data from virtual security partition. */
    A2_E_VSP_NOT_FORMATED = 5159, /**<  Virtual security partition is not initialized. */
    A2_E_VSP_UNSUPPORTED_UNIT_TYPE = 5160, /**<  Unit type is out of range in virtual security partition. */
    A2_E_GET_DYNAMIC_DATA_FAILED = 5161, /**<  Could not get the dynamic data from the security library. */
    A2_E_FLASH_RESULT_PARTITION_TABLE_FLASHED = 5162, /**<  Partition table repaired!! Please load file again. */
    A2_E_FLASH_RESULT_PARTITION_TABLE_NOT_FLASHED = 5163, /**<  Partition table is damaged! */
    A2_E_FLASH_RESULT_READ_FROM_PARTITION_TABLE = 5164, /**<  Partition table is damaged. */
    A2_E_BOOTIMAGE_PARTITION_RECORD_NOT_FOUND = 5165, /**<  Partition table record is not found in the boot containers. */
    A2_E_CORRUPT_STATIC_DATA_IN_BOOT_BLOCK = 5166, /**<  Static data in boot block is corrupt. */
    A2_E_MBBS_BBM_TYPE_CONFLICT = 5167, /**<  MBBS and BBM type conflict. */
    A2_E_EXECUTION_NOT_PERMITTED = 5168, /**<  Execution of this command is not allowed in the current domain with current authentication state. */
    A2_E_EXECUTION_NOT_PERMITTED_WARNING = 5169, /**<  Execution of this command is not allowed in the current domain with current authentication state. */
    A2_E_UNABLE_TO_INITIALIZE_LCD = 5171, /**<  Error during initialization of LCD drivers. */
    A2_E_LCD_NOT_INITIALIZED = 5172, /**<  LCD is not initialized. */
    A2_E_UNABLE_TO_DISPLAY_DATA_ON_LCD = 5173, /**<  Unrecoverable error during BMP image is processed. */
    A2_E_NETWORK_BUFFER_CORRUPTED = 5174, /**<  The network buffer area is corrupted. */
    A2_E_TOO_MENY_FRAGMENTS = 5200, /**<   */
    A2_E_FAILED_TO_GET_APPLICATION = 5201, /**<   */
    A2_E_SECURITY_LIBRARY_ERROR_BEGIN = 9096, /**<   */
    A2_E_SECURITY_LIBRARY_ERROR_END = 9300, /**<   */
    A2_E_UNSUPPORTED_PROPERTY = 5301, /**<  Unsupported file system property. */
    A2_E_READ_ONLY = 5302, /**<  The property is read only. */
    A2_E_INVALLID_PROPERTY_SPECIFIED = 5303, /**<  The specified property value was invalid. */
    A2_E_ACCESS_DENIED = 5304, /**<  The access permission attributes do not allow operation. */
    A2_E_FORMATTING_FS = 5305, /**<  The file system is formatting. */
    A2_E_PATH_NOT_EXISTS = 5306, /**<  The path does not exist. */
    A2_E_PATH_ALREADY_EXISTS = 5307, /**<  The path already exists. */
    A2_E_PATH_READ_ONLY = 5308, /**<  The path is read only. */
    A2_E_INSUFFICENT_SPACE = 5309, /**<  Insufficient space. */
    A2_E_DIRECTORY_NOT_EMPTY = 5310, /**<  The directory is not empty. */
    A2_E_INVALID_RESTRICTION_SPECIFIED = 5311, /**<  Invalid access restrictions specified. */
    A2_E_NO_FILESYSTEM_PROPERTY = 5312, /**<  No file system property. */
    A2_E_FILE_NOT_EXISTS = 5313, /**<  The file does not exist. */
    A2_E_CHANGE_DIR = 5314, /**<  Failed to change directory. */
    A2_E_CHMOD = 5315, /**<  Failed to set new CH mod. */
    A2_E_GWD = 5316, /**<  Failed to get the current directory. */
    A2_E_OPEN_FILE = 5317, /**<  Failed to open a file in the file system. */
    A2_E_ITEM_STAT = 5318, /**<  Failed to read the stat from file. */
    A2_E_LIST_VOL = 5319, /**<  Failed to list volumes. */
    A2_E_FREE_SPACE = 5320, /**<  No free space left. */
    A2_E_CLOSE_FILE = 5321, /**<  Failed to close a file in the filesystem. */
    A2_E_CLOSE_DIRECTORY = 5322, /**<  Failed to close the directory. */
    A2_E_REMOVE_FILE = 5323, /**<  Failed to remove a file from the filesystem. */
    A2_E_RENAME_FILE = 5324, /**<  Failed to rename a file in the filesystem. */
    A2_E_CREATE_DIR = 5325, /**<  Failed to create a new directory in the filesystem. */
    A2_E_REMOVE_DIRECTORY = 5326, /**<  Failed to remove a directory from the file system. */
    A2_E_WRITE_FILE = 5327, /**<  Failed to write a file to the file system. */
    A2_E_INVALID_SFA_STATE = 5328, /**<  The state of the SFA programming is incorrect. */
    A2_E_READ_FILE = 5329, /**<  Failed to read a file from the file system. */
    A2_E_CORE_SUPERV = 61797, /**<   */
    A2_E_MEM_ALLOC = 66166, /**<   */
    A2_E_GDFS_UNSUPPORTED_PROPERTY = 5401, /**<  Unsupported GDFS property. */
    A2_E_GDFS_READ_ONLY = 5402, /**<  The GDFS property is read only. */
    A2_E_GDFS_PROPERTY_VALUE_INVALID = 5403, /**<  The property value is invalid. */
    A2_E_GDFS_INVALID_START_PROPERTY = 5404, /**<  Invalid start property. */
    A2_E_GDFS_NOT_STARTED = 5405, /**<  GDFS has not been started yet. */
    A2_E_GDFS_FAILED_TO_FORMAT = 5406, /**<  Failed to format the GDFS area. */
    A2_E_GDFS_NOT_FORMATTED = 5407, /**<  GDFS is not formatted, as it should be. */
    A2_E_GDFS_INVALID_BLOCK_UNIT_SPECIFIED = 5408, /**<  Invalid Block unit number. */
    A2_E_GDFS_NO_CONFIGURATION_FOUND = 5409, /**<  Could not find any GDFS configuration from the platform software in flash. */
    A2_E_GDFS_CLOSE = 5410, /**<  Failed to close the GDFS area. */
    A2_E_GDFS_OPEN = 5411, /**<  Failed to open the GDFS area. */
    A2_E_GDFS_INVALID_UNIT_SIZE = 5412, /**<  Invalid GDFS UNIT size(size=0). */
    A2_E_GDFS_WRITE_TO_UNIT_FAILED = 5413, /**<  Failed to write to the specified unit. */
    A2_E_GDFS_FAILED_TO_READ_FROM_UNIT = 5414, /**<  Failed to read from the specified unit. */
    A2_E_GDFS_EMPTY = 5415, /**<  Failed read access on an empty GDFS. */
    A2_E_GENERAL_FATAL_ERROR = 70535, /**<   */
    E_DUMMY_CODE = 80009  /**<  */
} ErrorCode_e;

#endif /* _ERRORCODE_H */
