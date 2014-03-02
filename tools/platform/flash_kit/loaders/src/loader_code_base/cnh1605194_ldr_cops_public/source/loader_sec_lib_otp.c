/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 *  @addtogroup ldr_security_library
 *  @{
 *      @addtogroup command_protocol
 *      @{
 *
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_basicdefinitions.h"
#include "string.h"
#include "e_loader_sec_lib.h"
#include "loader_sec_lib_internals.h"
#include "r_loader_sec_lib_port_funcs.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Reads OTP data.
 *
 * @param[out]  OTP_Data_p Buffer that conatins OTP data.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                    Successful reading.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Invalid input params.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Read_OTP(LOADER_SEC_LIB_OTP_t *OTP_Data_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    COPSROM_Security_Settings_t *SecuritySettings_p;

    VERIFY(OTP_Data_p != NULL, LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    memset(OTP_Data_p, 0x00, sizeof(LOADER_SEC_LIB_OTP_t));

    ReturnValue = Do_LoaderSecLib_GetSecuritySettings(&SecuritySettings_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    OTP_Data_p->PAF = SecuritySettings_p->SecuredSettings.OTPValues.PAF;
    OTP_Data_p->RewritableIMEI = SecuritySettings_p->SecuredSettings.OTPValues.IMEI_Changeable;
    OTP_Data_p->CID = SecuritySettings_p->SecuredSettings.OTPValues.CID;

    ReturnValue = Do_LoaderSecLib_Unpack_IMEI(SecuritySettings_p->SecuredSettings.OTPValues.IMEI,
                  (uint8 *)(OTP_Data_p->IMEI));
    VERIFY(ReturnValue == LOADER_SEC_LIB_SUCCESS, ReturnValue);

    if (SecuritySettings_p->SecuredSettings.OTPValues.CIDLock) {
        OTP_Data_p->OTP_Lock_Status |= OTP_PACKED_CID_LOCK_MASK;
    }

    if (SecuritySettings_p->SecuredSettings.OTPValues.IMEI_ChangeableLock) {
        OTP_Data_p->OTP_Lock_Status |= OTP_PACKED_IMEI_CHANGEABLE_FLAG_LOCK_MASK;
    }

    if (SecuritySettings_p->SecuredSettings.OTPValues.PAFLock) {
        OTP_Data_p->OTP_Lock_Status |= OTP_PACKED_PAF_LOCK_MASK;
    }

    if (SecuritySettings_p->SecuredSettings.OTPValues.IMEILock) {
        OTP_Data_p->OTP_Lock_Status |= OTP_PACKED_IMEI_LOCK_MASK;
    }

ErrorExit:

    return ReturnValue;
}

/** @} */
