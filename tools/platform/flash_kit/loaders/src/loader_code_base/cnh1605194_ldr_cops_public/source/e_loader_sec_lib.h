/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSIONGUARD_E_LOADER_SEC_LIB_H
#define _INCLUSIONGUARD_E_LOADER_SEC_LIB_H
/**
 * @addtogroup ldr_security_library
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"         // Needed for such as uint16

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/**
 *  This enum contain the possible error codes from Loader Security Library routines.
 *  Possible Range 0x0FFF - 0xFFFF
 */
typedef enum {
    LOADER_SEC_LIB_SUCCESS                               = 0   ,/**< Successful evaluation.*/
    LOADER_SEC_LIB_CHIP_ID_INVALID                       = 4000,/**< The verification of ChipID list failed. */
    LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC             = 4096,/**< The function that was called did not have correct inparamaters. */
    LOADER_SEC_LIB_FAILURE                               = 4120,/**< Failure.*/
    LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE                = 4121,/**< The verification of hash list in Loader Security Library failed.*/
    LOADER_SEC_LIB_HEADER_VERIFICATION_FAILURE           = 4122,/**< The verification of the header has failed.*/
    LOADER_SEC_LIB_HEADER_VERIFIED                       = 4123,/**< Successful header verification. */
    LOADER_SEC_LIB_VERIFY_FAILURE                        = 4124,/**< Unsuccessful verification. */
    LOADER_SEC_LIB_INIT_CALLED_TWICE                     = 4150,/**< The sec lib init function has been called 2 times*/
    LOADER_SEC_LIB_MEMORY_RELEASE_FAILED                 = 4180,/**< Memory release failed.*/
    LOADER_SEC_LIB_MEMORY_ALLOCATION_FAILED              = 4181,/**< Memory allocation failed.*/
    LOADER_SEC_LIB_DATA_BLOCK_EXIST                      = 4182,/**< Data block exist in the linked list.*/
    LOADER_SEC_LIB_DATA_BLOCK_DO_NOT_EXIST               = 4183,/**< Data block do not exist in the linked list.*/
    LOADER_SEC_LIB_INVALID_AUTHENTICATION_TYPE           = 4184,/**< Invalid authentication type.*/
    LOADER_SEC_LIB_EXCEEDED_NUMBER_OF_AUTHENTICATION     = 4185,/**< Exceeded number of authentication. */
    LOADER_SEC_LIB_ESB_MAC_INIT_FAILED                   = 4186,/**< Initialization of ESB block for MAC calculation failed.*/
    LOADER_SEC_LIB_ESB_MAC_UPDATE_FAILED                 = 4187,/**< MAC update with ESB block failed.*/
    LOADER_SEC_LIB_ESB_MAC_FINAL_FAILED                  = 4188,/**< MAC finalize with ESB block failed.*/
    LOADER_SEC_LIB_ESB_MAC_NOT_VERIFIED                  = 4189,/**< MAC verification with ESB block failed.*/
    LOADER_SEC_LIB_ESB_DOWNLOCK_FAILED                   = 4190,/**< ESB downlock failed. */
    LOADER_SEC_LIB_CONTROL_KEY_VERIFICATION_FAILURE      = 4191,/**< Control key verification failed.*/
    LOADER_SEC_LIB_CA_CERTIFICATE_VERIFICATION_FAILURE   = 4192,/**< CA certificate verification failed. */
    LOADER_SEC_LIB_X509_ERROR_IN_CERTIFICATE             = 4193,/**< X509 certificate error.*/
    LOADER_SEC_LIB_COPS_INIT_FAILED                      = 4194,/**< COPS initialization failed. */
    LOADER_SEC_LIB_COPS_PROTECT_DATA_INIT_FAILED         = 4195,/**< COPS protect data initialization failed. */
    LOADER_SEC_LIB_COPS_DATA_READ_FAILED                 = 4196,/**< COPS data read failed. */
    LOADER_SEC_LIB_COPS_DATA_WRITE_FAILED                = 4197,/**< COPS data write failed. */
    LOADER_SEC_LIB_READ_OTP_FAILED                       = 4198,/**< Reading OTP data failed.*/
    LOADER_SEC_LIB_WRITE_OTP_FAILED                      = 4199,/**< Writting OTP data failed.*/
    LOADER_SEC_LIB_LOCK_OTP_FAILED                       = 4200,/**< Locking OTP data failed.*/
    LOADER_SEC_LIB_UNPACKING_IMEI_FAILED                 = 4201,/**< Unpacking IMEI data failed.*/
    LOADER_SEC_LIB_PACKING_IMEI_FAILED                   = 4202,/**< Packing IMEI data failed.*/
    LOADER_SEC_LIB_OTP_ALREADY_LOCKED                    = 4203,/**< OTP is already locked.*/
    LOADER_SEC_LIB_INVALID_CID_VALUE                     = 4204,/**< CID value is out of range.*/
    LOADER_SEC_LIB_OTP_LOCKBITS_MISSMATCH                = 4205,/**< OTP lock bits have different values.*/
    LOADER_SEC_LIB_WRITING_BOOTRECORD_FAILED             = 4206,/**< Writing in boot record failed. */
    LOADER_SEC_LIB_UNSUPPORTED_NO_DEBUG_HW               = 4207,/**< No debug hardware detected.*/
    LOADER_SEC_LIB_CHANGE_OPERATION_NOT_SUPPORTED        = 4208,/**< Requested change operation is not supported or not allowed. */
    LOADER_SEC_LIB_INVALID_CHANGE_OPERATION              = 4209,/**< Invalid change operation. */
    LOADER_SEC_LIB_RWIMEI_NOT_ALLOWED                    = 4210,/**< Rewritible IMEI is not allowed to change.*/
    LOADER_SEC_LIB_REQUEST_DENIED                        = 4211,/**< Request for change operation is denied.*/
    LOADER_SEC_LIB_BOOT_BLOCK_DO_NOT_EXIST               = 4212,/**< Boot record do not exist. */
    LOADER_SEC_LIB_CORRUPTED_DOMAIN_DATA                 = 4213,/**< Corrupted domain data in boot block.*/
    LOADER_SEC_LIB_INVALID_DOMAIN                        = 4214,/**< Invalid domain.*/
    LOADER_SEC_LIB_INVALID_CHALLENGE_DATA_BLOCK          = 4215,/**< Invalid challenge data block.*/

} LoaderSecLib_Output_t;

/** @} */
#endif /*_INCLUSIONGUARD_E_LOADER_SEC_LIB_H*/


