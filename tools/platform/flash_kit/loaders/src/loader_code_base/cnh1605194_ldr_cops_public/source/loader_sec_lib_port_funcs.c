/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>

#include "t_basicdefinitions.h"
#include "r_loader_sec_lib.h"
#include "header_definitions.h"
#include "e_loader_sec_lib.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "cops_data_manager.h"
#include "loader_sec_lib_internals.h"
#include "r_cops.h"
#include "otp_map_macro.h"
#include "r_otp_ld_functions.h"
#include "t_otp_pd_functions.h"
#include "r_serialization.h"
#include "r_boot_records_handler.h"
#include "r_bootparam.h"
#include "cops_data_manager.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * This routine frees the allocated memory region. Loader Security Library
 * external.
 *
 * @param[in] Buf_pp  Pointer to the allocated memory region to free.
 *
 * @return see \c LoaderSecLib_Output_t for detailed description of the error
 *                codes.
 */
LoaderSecLib_Output_t LoaderSecLib_Free(uint8 **const Buf_pp)
{
    if (NULL != *Buf_pp) {
        free(*Buf_pp);
        *Buf_pp = NULL;
        return LOADER_SEC_LIB_SUCCESS;
    }

    A_(printf("loader_sec_lib_port_funcs.c (%d): ** ERR: Failed to free memory! **\n", __LINE__);)
    return LOADER_SEC_LIB_FAILURE;
}
/*
 * This routine allocates a memory region. Loader Security Library external.
 *
 * @param[in] BufLength   The length of the buffer to allocate.
 *
 * @return the allocated buffer, the function will return NULL if the allocation
 *                               failed.
 */
void *LoaderSecLib_Allocate(const uint32 BufLength)
{
    return (void *)malloc(BufLength);
}


/*
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
LoaderSecLib_Output_t Do_LoaderSecLib_ControlKey_Authenticaion(uint8 const *const Data_p, const uint32 Length)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    COPS_Protect_VerifyStatus_t VerifyStatus = COPS_PROTECT_VERIFYSTATUS_NOT_VERIFIED;
    COPS_AuthenticationData_t AuthenticationData;

    VERIFY((LOADER_SEC_LIB_NO_OF_CONTROL_KEYS *(LOADER_SEC_LIB_CONTROL_KEY_LENGTH + 1)) == Length, LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);
    AuthenticationData.AuthType = COPS_AUTHENTICATION_SIMLOCK_KEYS;
    AuthenticationData.Length   = Length;
    AuthenticationData.Data_p   = (uint8 *)Data_p;

    Result = COPS_Authenticate(&AuthenticationData, COPS_PROTECT_SESSIONTYPE_COPS, &VerifyStatus);
    VERIFY(E_SUCCESS == Result, LOADER_SEC_LIB_CONTROL_KEY_VERIFICATION_FAILURE);
    VERIFY(COPS_PROTECT_VERIFYSTATUS_OK == VerifyStatus, LOADER_SEC_LIB_CONTROL_KEY_VERIFICATION_FAILURE);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}


/*
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
LoaderSecLib_Output_t Do_LoaderSecLib_CA_Authenticaion(uint8 *Data_p, uint32 Length, uint32 *AuthenticationPurpose_p)
{
    LoaderSecLib_Output_t   ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    COPS_RC_t COPSResult = COPS_RC_OK;
    COPS_Protect_VerifyStatus_t VerifyRestult = COPS_PROTECT_VERIFYSTATUS_NOT_VERIFIED;
    COPS_AuthenticationData_t AuthenticationData;
    COPS_Protect_AllowedOP_t    AllowedOP = {0};

    /* Authenticate */
    AuthenticationData.AuthType = COPS_AUTHENTICATION_RSA_CHALLENGE;
    AuthenticationData.Data_p = Data_p;
    AuthenticationData.Length = Length;
    COPSResult = COPS_Authenticate(&AuthenticationData, COPS_PROTECT_SESSIONTYPE_SWBP, &VerifyRestult);
    C_(printf("loader_sec_lib_port_funcs.c (%d): COPSResult(%d)\n", __LINE__, COPSResult);)
    C_(printf("loader_sec_lib_port_funcs.c (%d): VerifyRestult(%d)\n", __LINE__, VerifyRestult);)
    VERIFY((COPSResult == COPS_RC_OK), LOADER_SEC_LIB_CA_CERTIFICATE_VERIFICATION_FAILURE);
    VERIFY((VerifyRestult == COPS_PROTECT_VERIFYSTATUS_OK), LOADER_SEC_LIB_CA_CERTIFICATE_VERIFICATION_FAILURE);
    ReturnValue = (LoaderSecLib_Output_t)COPS_GetAllowedOperation(&AllowedOP);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    *AuthenticationPurpose_p = AllowedOP.AuthenticationPurpose;

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    B_(printf("loader_sec_lib_port_funcs.c (%d): return... (%d)\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/*
 * Pack IMEI data
 *
 * @param[in]  IMEI_Data_p    Buffer that conatins unpacked IMEI data
 * @param[out] Packed_IMEI_p  Buffer that conatins packed IMEI data
 *
 * @retval LOADER_SEC_LIB_SUCCESS                            Successful packing.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC        Invalid input params.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Pack_IMEI(uint8 *IMEI_Data_p, uint8 *Packed_IMEI_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    COPS_IMEI_t COPS_IMEI_Data = {0};

    VERIFY(IMEI_Data_p   != NULL, LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);
    VERIFY(Packed_IMEI_p != NULL, LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    memcpy(COPS_IMEI_Data.Digits, IMEI_Data_p, UNPACKED_IMEI_SIZE);
    Util_COPS_PackIMEI(&COPS_IMEI_Data, Packed_IMEI_p);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:

    return ReturnValue;
}


/*
 * Unpack IMEI data
 *
 * @param[in]  Packed_IMEI_p  Buffer that conatins packed IMEI data
 * @param[out] IMEI_Data_p    Buffer that conatins unpacked IMEI data
 *
 * @retval LOADER_SEC_LIB_SUCCESS                          Successful unpacking.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC        Invalid input params.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Unpack_IMEI(uint8 *Packed_IMEI_p, uint8 *IMEI_Data_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    COPS_IMEI_t COPS_IMEI_Data = {0};

    VERIFY(IMEI_Data_p   != NULL, LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);
    VERIFY(Packed_IMEI_p != NULL, LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    Util_COPS_UnpackIMEI(Packed_IMEI_p, &COPS_IMEI_Data);
    memcpy(IMEI_Data_p, COPS_IMEI_Data.Digits, UNPACKED_IMEI_SIZE);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:

    return ReturnValue;
}

/*
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
LoaderSecLib_Output_t Do_LoaderSecLib_Write_BootRecord(uint32 BootRecordId, uint32 *BootRecordData_p, uint32 BootRecordSize)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    VERIFY((NULL != BootRecordData_p) && (BootRecordSize > 0), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    ReturnValue = (LoaderSecLib_Output_t)Do_Write_BootRecord(BootRecordId, BootRecordData_p, BootRecordSize);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}


/*
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
LoaderSecLib_Output_t Do_LoaderSecLib_GetSecuritySettings(COPSROM_Security_Settings_t **SecuritySettings_pp)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    BOOTPARAM_Status_t BOOTPARAM_RetVal = BOOTPARAM_STATUS_NOT_FOUND;
    uint32 BP_Size = 0;

    VERIFY(NULL != SecuritySettings_pp, LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    BOOTPARAM_RetVal = R_Do_BOOTPARAM_GetPtr(BOOTPARAM_IDENTIFIER_SECURITY_SETTINGS, (uint32 **)SecuritySettings_pp, &BP_Size, TRUE, 0);
    VERIFY(sizeof(COPSROM_Security_Settings_t) == BP_Size, LOADER_SEC_LIB_FAILURE);
    VERIFY(BOOTPARAM_STATUS_OK == BOOTPARAM_RetVal, LOADER_SEC_LIB_FAILURE);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * Get the random generated data using challenge data generator.
 *
 * @param [out] RandomData_p Buffer with random generated data with size 32 bytes.
 *
 * @retval LOADER_SEC_LIB_SUCCESS successfuly generated random data.
 * @retval E_INVALID_INPUT_PARAMETERS If there is no valid input
 *                                    parameters sent.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_GenerateRandomData(uint8 *RandomData_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    COPS_Challenge_t ChallengeData;

    VERIFY((NULL != RandomData_p), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    ReturnValue = (LoaderSecLib_Output_t)COPS_GenerateChallenge(COPS_AUTHENTICATION_RSA_CHALLENGE, &ChallengeData);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    memcpy(RandomData_p, (uint8 *)&ChallengeData.Data[LOADER_SEC_LIB_RANDOM_DATA_OFFSET], LOADER_SEC_LIB_RANDOM_DATA_LENGTH);
    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}
