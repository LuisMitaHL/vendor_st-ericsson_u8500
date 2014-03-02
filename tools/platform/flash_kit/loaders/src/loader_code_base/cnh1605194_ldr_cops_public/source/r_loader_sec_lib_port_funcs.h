/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_LOADER_SEC_LIB_PORT_FUNCS_H
#define _R_LOADER_SEC_LIB_PORT_FUNCS_H
/**
 * @addtogroup ldr_security_library
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_loader_sec_lib.h"
#include "e_loader_sec_lib.h"
#ifndef MACH_TYPE_STN8500
#include "cops_protect.h"
#endif

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * This routine frees the allocated memory region. Loader Security Library
 * external.
 *
 * @param[in] Buf_pp  Pointer to the allocated memory region to free.
 *
 * @return see \c LoaderSecLib_Output_t for detailed description of the error
 *                codes.
 */
LoaderSecLib_Output_t LoaderSecLib_Free(uint8 **const Buf_pp);

/**
 * This routine allocates a memory region. Loader Security Library external.
 *
 * @param[in] BufLength   The length of the buffer to allocate.
 *
 * @return the allocated buffer, the function will return NULL if the allocation
 *                               failed.
 */
void *LoaderSecLib_Allocate(const uint32 BufLength);

/**
 * Authentication with Control keys.
 *
 * @param[in] Data_p Buffer with control keys.All 12 keys are in ASCII format.
 * @param[in] Length Length of the buffer Data_p.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                          Successful
 *                                                         authentication.
 * @retval LOADER_SEC_LIB_CONTROL_KEY_VERIFICATION_FAILURE Control Key
 *                                                         verification failed.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_ControlKey_Authenticaion(uint8 const *const Data_p, const uint32 Length);

/**
 * Authentication with CA certificate.
 *
 * @param[in] Data_p Buffer with updated authentication chalenge data block,
 *                   signed with correct certificate.
 * @param[in] Length Length of the buffer Data_p.
 * @param[out] AuthenticationPurpose_p Authentication purpose value from CA
 *                                    certificate.
 * @retval LOADER_SEC_LIB_SUCCESS                             Successful
 *                                                            authentication.
 * @retval LOADER_SEC_LIB_CA_CERTIFICATE_VERIFICATION_FAILURE CA verification
 *                                                            failed.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_CA_Authenticaion(uint8 *Data_p, uint32 Length, uint32 *AuthenticationPurpose_p);

/**
 * Pack IMEI data
 *
 * @param[in]  IMEI_Data_p    Buffer that conatins unpacked IMEI data
 * @param[out] Packed_IMEI_p  Buffer that conatins packed IMEI data
 *
 * @retval LOADER_SEC_LIB_SUCCESS                          Successful unpacking.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC        Invalid input params.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Pack_IMEI(uint8 *IMEI_Data_p, uint8 *Packed_IMEI_p);

/**
 * Unpack IMEI data
 *
 * @param[in]  Packed_IMEI_p  Buffer that conatins packed IMEI data
 * @param[out] IMEI_Data_p    Buffer that conatins unpacked IMEI data
 *
 * @retval LOADER_SEC_LIB_SUCCESS                          Successful unpacking.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC        Invalid input params.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Unpack_IMEI(uint8 *Packed_IMEI_p, uint8 *IMEI_Data_p);

/**
 * This is the wrap function which write single boot record that is
 * identified by BootRecordId and BootPartitionIndex.
 *
 * @param [in]     BootRecordId - Record Id of specified boot record.
 * @param [out]    BootRecordData_p - Pointer to boot record data.
 * @param [in]     BootRecordSize - Size of data that should be written(pointed
 *                                  by BootRecordData_p pointer).
 *
 * @retval E_SUCCESS - Success ending.
 * @retval E_INVALID_INPUT_PARAMETERS If there is no valid input
 *                                    parameters sent.
 * @retval E_BOOTRECORD_FULL          If containers cannot support storage for
 *                                    more boot records. This is software
 *                                    limitation that should be easily fixed.
 * @retval E_BOOT_RECORD_EMPTY        If no valid boot records exist on
 *                                    flash memory. User should wait untill
 *                                    valid boot record image is flashed.
 *
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Write_BootRecord(uint32 BootRecordId, uint32 *BootRecordData_p, uint32 BootRecordSize);

#ifdef MACH_TYPE_DB5700
/**
 * Get the random generated data using challenge data generator.
 *
 * @param [out] RandomData_p Buffer with random generated data with size 32 bytes.
 *
 * @retval LOADER_SEC_LIB_SUCCESS successfuly generated random data.
 * @retval E_INVALID_INPUT_PARAMETERS If there is no valid input
 *                                    parameters sent.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_GenerateRandomData(uint8 *RandomData_p);

/**
 * This is the wrap function which returns a pointer to the start of the contents of a SECURITY
 * SETTINGS boot stage parameter.
 *
 * @param [in,out]     SecuritySettings_pp - Pointer to pointer to SECURITY_SETTINGS
 *                                           parameter data.
 *
 * @retval E_SUCCESS - Success ending.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC - If there is no valid input
 *                                                    parameters sent.
 * @retval LOADER_SEC_LIB_FAILURE - If unexpected error occurs.
 *
 */
LoaderSecLib_Output_t Do_LoaderSecLib_GetSecuritySettings(COPSROM_Security_Settings_t **SecuritySettings_pp);
#endif //MACH_TYPE_DB5700

/** @} */
#endif /*_R_LOADER_SEC_LIB_PORT_FUNCS_H*/
