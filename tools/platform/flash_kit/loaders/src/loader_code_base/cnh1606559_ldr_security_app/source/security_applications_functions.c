/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/

/*
 *  @addtogroup ldr_security_app
 *  @{
 */
#include "c_system.h"
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "error_codes.h"
#include "commands.h"
#include "command_ids.h"
#include "commands_impl.h"
#include "r_serialization.h"
#ifndef MACH_TYPE_STN8500
#include "cops_protect.h"
#include "t_copsrom.h"
#include "r_bootparam.h"
#include "r_otp_pd_functions.h"
#include "r_otp_ld_functions.h"
#include "r_boot_records_handler.h"
#include "cops_data_manager.h"
#endif

#include "r_command_exec_service.h"
#include "r_io_subsystem.h"
#include "t_communication_service.h"
#include "r_path_utilities.h"
#include "t_loader_sec_lib.h"
#include "r_loader_sec_lib.h"
#include "r_loader_sec_lib_port_funcs.h"
#include "ces_commands.h"
#ifndef MACH_TYPE_STN8500
#include "r_copsrom.h"
#include "t_service_management.h"
#include "jmptable.h"
#include "r_otp_pd_functions.h"
#include "r_pd_esb.h"
#endif

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define PARAMETER_ID_IMEI   0x01

#ifndef MACH_TYPE_STN8500
/*
 * XVLANAU: TEST string used as batch secret for cops.
 */
const COPS_BatchSecret_t COPS_Test_BatchSecret = {"<BatachSectret----------------->"};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 * @brief Set the ME domain.
 *
 * This command set ME domain.
 *
 * Call source: PC
 *
 * @param[in]      TargetDomain     - domain that should be set.
 * @return         @ref E_SUCCESS - Success ending
 *
 * @return         @ref E_INVALID_INPUT_PARAMETERS - If invalid input parameter
 *                                                   is passed for new domain.
 *
 * @return      Function forwards internal Loader SecLib error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_Security_SetDomainImpl(uint16 Session,
                                      const uint32 TargetDomain)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *Data_p = NULL;
    CommandData_t CmmData = {0};
    CmmData.Payload.Data_p = NULL;

    CmmData.CommandNr = COMMAND_SECURITY_SETDOMAIN;
    CmmData.ApplicationNr = GROUP_SECURITY;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    if (!(LOADER_SEC_LIB_DOMAIN_FACTORY == TargetDomain ||
            LOADER_SEC_LIB_DOMAIN_RnD     == TargetDomain ||
            LOADER_SEC_LIB_DOMAIN_PRODUCT == TargetDomain ||
            LOADER_SEC_LIB_DOMAIN_SERVICE == TargetDomain)) {
        B_(printf("Invalid domain passed.\n");)
        ReturnValue = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

    CmmData.Payload.Size = sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, TargetDomain);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("Do_Security_SetDomainImpl..........(%d) \n", ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*
 * @brief Get current ME domain.
 *
 * This command get current ME domain (domain that is read from BOOTROM).
 *
 * Call source: PC
 *
 * @param[in]      none
 * @return         @ref E_SUCCESS - Success ending
 *
 * @return      Function forwards internal Loader SecLib error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_Security_GetDomainImpl(uint16 Session)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_SECURITY_GETDOMAIN;
    CmmData.ApplicationNr = GROUP_SECURITY;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = 0;
    CmmData.Payload.Data_p = NULL;

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("Do_Security_GetDomainImpl..........(%d) \n", ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}
#endif //#ifndef MACH_TYPE_STN8500

/*
 * @brief Writes a security data unit.
 *
 * Call source: PC
 *
 * @param[in]      Unit_id        - Unit id to read.
 * @param[in]      DataBuffLength - Length of the Data buffer.
 * @param[in]      DataBuff_p     - Pointer to DataLength bytes of "Unit data".
 * @return         @ref E_SUCCESS - Success ending
 *
 * @return      @ref E_INVALID_INPUT_PARAMETERS - If invalid argument is
 *                                                passed to function.
 *
 * @return      Function forwards internal COPS error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_Security_SetPropertiesImpl(uint16 Session,
        const uint32 Unit_id,
        const uint32 DataBuffLength,
        void *DataBuff_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *Data_p = NULL;
    CommandData_t CmmData = {0};
    CmmData.Payload.Data_p = NULL;

    CmmData.CommandNr = COMMAND_SECURITY_SETPROPERTIES;
    CmmData.ApplicationNr = GROUP_SECURITY;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY(NULL != DataBuff_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(0 != DataBuffLength, E_INVALID_INPUT_PARAMETERS);

    CmmData.Payload.Size = sizeof(uint32) + sizeof(uint32) + DataBuffLength;
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Unit_id);
    put_uint32_le((void **)&Data_p, DataBuffLength);
    put_block((void **)&Data_p, DataBuff_p, DataBuffLength);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("Do_Security_SetPropertiesImpl..........(%d) \n", ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*
 * @brief Reads a security data unit.
 *
 * Call source: PC
 *
 * @param[in]   Unit_id     - Reads a security data unit.
 * @return      @ref E_SUCCESS - Success ending
 *
 * @return      @ref E_PROPERTY_NOT_FOUND - If specified property cannot be
 *                                          found by COPS module.
 *
 * @return      Function forwards internal COPS error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 *
 */
ErrorCode_e Do_Security_GetPropertiesImpl(uint16 Session,
        const uint32 Unit_id)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *Data_p = NULL;
    CommandData_t CmmData = {0};
    CmmData.Payload.Data_p = NULL;

    CmmData.CommandNr = COMMAND_SECURITY_GETPROPERTIES;
    CmmData.ApplicationNr = GROUP_SECURITY;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Unit_id);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("Do_Security_GetPropertiesImpl..........(%d) \n", ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*
 * @brief Associates all security data units with the current ME.
 *
 * Call source: PC
 *
 * @param[in]   none
 * @return      @ref E_SUCCESS - Success ending
 *
 * @return      @ref E_GENERAL_FATAL_ERROR - If something unexpected happened.
 * @return      @ref E_SEC_APP_OPERATION_DENIED -  If some security operation
 *                                                 cannot be accomplished due to
 *                                                 various reasons.
 * @return      @ref E_SEC_APP_IMEI_NOT_CHANGABLE -  If user try to change IMEI
 *                                                   in COPS while IMEI is set
 *                                                   as non - changeable in OTP
 *                                                   area.
 *
 * @return      Function forwards internal COPS error codes.
 *
 * @return      Function forwards internal Loader SecLib error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_Security_BindPropertiesImpl(uint16 Session)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_SECURITY_BINDPROPERTIES;
    CmmData.ApplicationNr = GROUP_SECURITY;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = 0;
    CmmData.Payload.Data_p = NULL;

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("Do_Security_BindPropertiesImpl..........(%d) \n", ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*
 * @brief Initialize a SW version table, intended for checking the ARB functionality.
 *
 * Call source: PC
 *
 * @param[in]      Type           - Type of the arb data.
 * @param[in]      DataLength     - Length of the Data buffer.
 * @param[in]      DataBuff_p     - Pointer to DataLength bytes of arb data.
 * @return         @ref E_SUCCESS - Success ending
 *
 * @return         @ref E_GENERAL_FATAL_ERROR - If something unexpected happened.
 * @return         @ref E_SEC_APP_OPERATION_DENIED -  If some security operation
 *                                                 cannot be accomplished due to
 *                                                 various reasons.
 *
 * @return      Function forwards internal COPS error codes.
 *
 * @return      Function forwards internal Loader SecLib error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_Security_InitARBTableImpl(uint16 Session, const uint32 Type, const uint32 DataLength,
        void *DataBuff_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};
    uint8 *Data_p = NULL;

    CmmData.CommandNr = COMMAND_SECURITY_INITARBTABLE;
    CmmData.ApplicationNr = GROUP_SECURITY;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY(NULL != DataBuff_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(0 != DataLength, E_INVALID_INPUT_PARAMETERS);

    CmmData.Payload.Size = sizeof(uint32) + sizeof(uint32) + DataLength;
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;

    put_uint32_le((void **)&Data_p, Type);
    put_uint32_le((void **)&Data_p, DataLength);
    put_block((void **)&Data_p, DataBuff_p, DataLength);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("Do_Security_InitARBTableImpl..........(%d) \n", ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*
 * @brief Invoke programming of RPMB authentication key.
 *
 * Call source: PC
 *
 * @return         @ref E_SUCCESS - Success ending
 *
 * @return         @ref E_GENERAL_FATAL_ERROR - If something unexpected happened.
 * @return         @ref E_SEC_APP_OPERATION_DENIED -  If some security operation
 *                                                 cannot be accomplished due to
 *                                                 various reasons.
 *
 * @return      Function forwards internal COPS error codes.
 *
 * @return      Function forwards internal Loader SecLib error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_Security_WriteRPMBKeyImpl(uint16 Session, const uint32 Dev_Id, const uint32 Commercial)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};
    uint8 *Data_p = NULL;

    CmmData.CommandNr = COMMAND_SECURITY_WRITERPMBKEY;
    CmmData.ApplicationNr = GROUP_SECURITY;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    CmmData.Payload.Size = sizeof(uint32) + sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Dev_Id);
    put_uint32_le((void **)&Data_p, Commercial);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    return ReturnValue;

ErrorExit:
    B_(printf("Do_Security_WriteRPMBKeyImpl..........(%d) \n", ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*******************************************************************************
 *  Long running commands
 ******************************************************************************/
#ifndef MACH_TYPE_STN8500
ErrorCode_e Security_SetDomainRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    void *Data_p = NULL;
    uint32 TargetDomain = 0;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    TargetDomain = get_uint32_le((void **)&Data_p);

    ReturnValue = (ErrorCode_e)Do_LoaderSecLib_WriteDomain(TargetDomain);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    ExeContext_p->Running = FALSE;

    B_(printf("Do_Security_SetDomainImpl..........(%d) \n", ReturnValue);)
    ReturnValue = Done_Security_SetDomain(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Security_GetDomainRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint32 EffectiveDomain = 0;
    uint32 WrittenDomain = 0;

    ReturnValue = (ErrorCode_e)Do_LoaderSecLib_ReadDomain(&EffectiveDomain, &WrittenDomain);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    IDENTIFIER_NOT_USED(EffectiveDomain);
    ExeContext_p->Running = FALSE;

    B_(printf("Do_Security_GetDomainImpl..........(%d) \n", ReturnValue);)
    ReturnValue = Done_Security_GetDomain(ExeContext_p->Received.SessionNr, ReturnValue, WrittenDomain);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Security_SetPropertiesRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint32 Unit_id = 0;
    uint32 DataLength = 0;
    void *TempData_p = NULL;
    uint8 *Data_p = NULL;

    TempData_p = ExeContext_p->Received.Payload.Data_p;
    Unit_id = get_uint32_le((void **)&TempData_p);
    DataLength = get_uint32_le((void **)&TempData_p);
    Data_p = (uint8 *)malloc(DataLength);
    ASSERT(NULL != Data_p);
    memcpy(Data_p, TempData_p, DataLength);

    switch (Unit_id) {
    case PROP_IMEI: {
        ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Pack_IMEI(TempData_p, Data_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        DataLength = PACKED_IMEI_SIZE;
        ReturnValue = COPS_SetParameter(COPS_STORAGE_PARAMETER_ID_IMEI, Data_p, DataLength);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }
    break;

    case DEFAULT_DATA: {
        COPS_Protect_VerifyStatus_t VerifyStatus = COPS_PROTECT_VERIFYSTATUS_FAILED;
        COPS_Protect_SessionType_t SessionType = COPS_PROTECT_SESSIONTYPE_COPS;
        COPS_AuthenticationData_t AuthenticationData;
        COPS_Protect_AllowedOP_t AllowedOP = {0};

        /* Check which operations are allowed.*/
        ReturnValue = COPS_GetAllowedOperation(&AllowedOP);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        if (FALSE == AllowedOP.WriteErase_COPS_DATA) {
            AuthenticationData.AuthType = COPS_AUTHENTICATION_DOMAIN;
            AuthenticationData.Length = 0;
            AuthenticationData.Data_p = NULL;

            ReturnValue = COPS_Authenticate(&AuthenticationData, SessionType, &VerifyStatus);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
            VERIFY(COPS_PROTECT_VERIFYSTATUS_OK == VerifyStatus, E_COPS_AUTHENTICATION_FAILED);
        }

        ReturnValue = COPS_WriteDefaultData(Data_p, DataLength);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }
    break;

    case PROP_SIMLOCK_SET: {
        COPS_AuthenticationData_t NewAuthData[2];
        uint8 *SIMLock_Keys_p = NULL;
        uint32 OffsetInOldBuffer = 0;
        uint32 OffsetInNewBuffer = 0;
        uint32 Key = 0;
        uint32 Offset = 0;

        VERIFY(LOADER_SEC_LIB_CONTROL_KEYS_LENGTH == DataLength, E_INVALID_INPUT_PARAMETERS);

        SIMLock_Keys_p = (uint8 *)malloc(sizeof(COPS_SIMLock_Keys_t));
        ASSERT(NULL != SIMLock_Keys_p);

        memset(SIMLock_Keys_p, 0x00, sizeof(COPS_SIMLock_Keys_t));

        // Loop until all keys are transfered in new buffer
        for (Key = 0; Key < LOADER_SEC_LIB_NO_OF_CONTROL_KEYS; Key++) {
            Offset = 0;

            while ((Offset < LOADER_SEC_LIB_CONTROL_KEY_LENGTH) && (*(Data_p + OffsetInOldBuffer + Offset) != 0xFF)) {
                *(SIMLock_Keys_p + OffsetInNewBuffer + Offset) = *(Data_p + OffsetInOldBuffer + Offset);
                Offset++;
            }

            SIMLock_Keys_p[Offset + OffsetInNewBuffer] = '\0';
            OffsetInOldBuffer += LOADER_SEC_LIB_CONTROL_KEY_LENGTH;
            OffsetInNewBuffer += LOADER_SEC_LIB_CONTROL_KEY_LENGTH;
            OffsetInNewBuffer++;
        }

        /*
         * TODO: This part is for testing purposes. After COPS is fully implemented
         *       usage of this part should be investigate and probably removed.
         */
        NewAuthData[0].AuthType = COPS_AUTHENTICATION_BATCH_SECRET;
        NewAuthData[0].Data_p   = (uint8 *)&COPS_Test_BatchSecret;
        NewAuthData[0].Length   = sizeof(COPS_Test_BatchSecret);

        NewAuthData[1].AuthType = COPS_AUTHENTICATION_SIMLOCK_KEYS;
        NewAuthData[1].Length   = sizeof(COPS_SIMLock_Keys_t);
        NewAuthData[1].Data_p   = SIMLock_Keys_p;

        ReturnValue = COPS_NewAuthenticationData(&NewAuthData[0], COPS_PROTECT_SESSIONTYPE_COPS);

        if (E_SUCCESS != ReturnValue) {
            A_(printf("security_applications_functions.c (%d): COPS_NewAuthenticationData failed!\n", __LINE__);)
            BUFFER_FREE(SIMLock_Keys_p);
            goto ErrorExit;
        }

        ReturnValue = COPS_NewAuthenticationData(&NewAuthData[1], COPS_PROTECT_SESSIONTYPE_COPS);

        if (E_SUCCESS != ReturnValue) {
            A_(printf("security_applications_functions.c (%d): COPS_NewAuthenticationData failed!\n", __LINE__);)
            BUFFER_FREE(SIMLock_Keys_p);
            goto ErrorExit;
        }

        ReturnValue = COPS_Lock_SetDefaultModemLockKey();

        BUFFER_FREE(SIMLock_Keys_p);
    }
    break;

    default:
        A_(printf("security_applications_functions.c (%d): Invalid Id passed in function.\n", __LINE__);)
        /*
         * NOTE: Temporarily ignore invalid input parameters cause testing purposes.
         *       After that these should be treated as error and appropriate error
         *       code should be returned.
         */
        ReturnValue = E_SUCCESS;
        goto ErrorExit;
    }

ErrorExit:
    ExeContext_p->Running = FALSE;
    BUFFER_FREE(Data_p);

    B_(printf("Do_Security_SetPropertiesImpl..........(%d) \n", ReturnValue);)
    ReturnValue = Done_Security_SetProperties(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Security_GetPropertiesRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint32 Unit_id = 0;
    boolean Exist = FALSE;
    void *TempData_p = NULL;
    uint8 *Data_p = NULL;
    uint32 DataLength = 0;
    uint8 IMEI_Data[LOADER_SEC_LIB_IMEI_LENGTH] = {0,};
    LOADER_SEC_LIB_OTP_t OTPInfo = {0};

    TempData_p = ExeContext_p->Received.Payload.Data_p;
    Unit_id = get_uint32_le((void **)&TempData_p);

    if (Unit_id == PROP_IMEI) {
        /* Get parameter data for specified ID.*/
        ReturnValue = COPS_GetParameter(COPS_STORAGE_PARAMETER_ID_IMEI, &Exist, &Data_p, &DataLength);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        /* Check integrity of COPS data*/
        ReturnValue = COPS_LoadAndVerifyAllData();

        if ((E_SUCCESS == ReturnValue) && (TRUE == Exist)) {
            ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Unpack_IMEI(Data_p, IMEI_Data);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            DataLength = LOADER_SEC_LIB_IMEI_LENGTH;
            C_(printf("security_application_functions.c(%d): IMEI read from FLASH\n", __LINE__);)
        } else {
            /* Read OTP data from security library.*/
            ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Read_OTP(&OTPInfo);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            memcpy(IMEI_Data, OTPInfo.IMEI, LOADER_SEC_LIB_IMEI_LENGTH);
            DataLength = LOADER_SEC_LIB_IMEI_LENGTH;
            C_(printf("security_application_functions.c(%d): IMEI read from OTP\n", __LINE__);)
        }

        Data_p = IMEI_Data;
    }

    if (Unit_id == PROP_SUBDOMAIN) {
        DataLength = PROP_SUBDOMAIN_LENGTH;
        /* Get parameter data for specified ID.*/
        ReturnValue = COPS_GetParameter(COPS_STORAGE_PARAMETER_ID_SUBDOMAIN, &Exist, &Data_p, &DataLength);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        if (TRUE != Exist) {
            DataLength = PROP_SUBDOMAIN_LENGTH;
            *Data_p = 0xFF;
        }
    }

    if (Unit_id == PROP_TPID) {
        DataLength = PROP_TPID_LENGTH;
        /* Get parameter data for specified ID.*/
        ReturnValue = COPS_GetParameter(COPS_STORAGE_PARAMETER_ID_TPID, &Exist, &Data_p, &DataLength);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        if (TRUE != Exist) {
            DataLength = PROP_SUBDOMAIN_LENGTH;
            *Data_p = 0xFF;
        }
    }

ErrorExit:
    ExeContext_p->Running = FALSE;

    B_(printf("Do_Security_GetPropertiesImpl..........(%d) \n", ReturnValue);)

    if (E_SUCCESS != ReturnValue) {
        C_(printf(" Reset DataLength due to security reason.");)
        DataLength = 0;
    }

    ReturnValue = Done_Security_GetProperties(ExeContext_p->Received.SessionNr, ReturnValue, DataLength, Data_p);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Security_BindPropertiesRepeat(void *ExecutionContext_p)
{
    ErrorCode_e                 ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t          *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    COPS_Protect_AllowedOP_t    AllowedOP = {0};
    LOADER_SEC_LIB_OTP_t        OTPInfo = {0};
    uint8                       *COPS_Packed_IMEI_p = NULL;
    uint32                      COPS_Packed_IMEI_Length = 0;
    boolean                     IMEI_Exist = FALSE;

    /* Check which operations are allowed.*/
    ReturnValue = COPS_GetAllowedOperation(&AllowedOP);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    if (FALSE == AllowedOP.Call_CBP) {
        C_(printf("LINE %d: Authentication insufficient. Loader try to authenticate with domain.\n", __LINE__);)

        COPS_Protect_VerifyStatus_t VerifyStatus = COPS_PROTECT_VERIFYSTATUS_FAILED;
        COPS_Protect_SessionType_t SessionType = COPS_PROTECT_SESSIONTYPE_COPS;
        COPS_AuthenticationData_t AuthenticationData;

        AuthenticationData.AuthType = COPS_AUTHENTICATION_DOMAIN;
        AuthenticationData.Length = 0;
        AuthenticationData.Data_p = NULL;

        ReturnValue = COPS_Authenticate(&AuthenticationData, SessionType, &VerifyStatus);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        VERIFY(COPS_PROTECT_VERIFYSTATUS_OK == VerifyStatus, E_COPS_AUTHENTICATION_FAILED);

        ReturnValue = COPS_GetAllowedOperation(&AllowedOP);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    /* Read OTP data from security library.*/
    ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Read_OTP(&OTPInfo);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    /*
     * IMEI logic.
     */
    ReturnValue = COPS_GetParameter(PARAMETER_ID_IMEI, &IMEI_Exist, &COPS_Packed_IMEI_p, &COPS_Packed_IMEI_Length);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    if ((FALSE == IMEI_Exist) || (FALSE == OTPInfo.RewritableIMEI)) {
        uint8 COPS_Packed_IMEI[PACKED_IMEI_SIZE] = {0};

        VERIFY(TRUE == AllowedOP.Change_IMEI, E_SEC_APP_OPERATION_DENIED);

        ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Pack_IMEI(OTPInfo.IMEI, COPS_Packed_IMEI);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = COPS_SetParameter(PARAMETER_ID_IMEI, COPS_Packed_IMEI, PACKED_IMEI_SIZE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    /* Write domain in boot records.*/
    ReturnValue = (ErrorCode_e)LoaderSecLib_Calculate_And_MAC_DomainData();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    /* Recalculates integrity protection of protected data.*/
    ReturnValue = COPS_CalculateProtection();

    if (E_SUCCESS != ReturnValue) {
        uint32 EffectiveDomain = 0;
        uint32 WrittenDomain = 0;

        ReturnValue = (ErrorCode_e)Do_LoaderSecLib_ReadDomain(&EffectiveDomain, &WrittenDomain);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        if (LOADER_SEC_LIB_DOMAIN_SERVICE == (LOADER_SEC_LIB_Domain_t)EffectiveDomain) {
            ReturnValue = E_SUCCESS;
        }

        goto ErrorExit;
    }

    /* Commits changed to the data done in RAM to flash.*/
    ReturnValue = COPS_Write(COPS_DATA_CONDITION_FORCE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    ExeContext_p->Running = FALSE;

    B_(printf("Do_Security_BindPropertiesImpl..........(%d) \n", ReturnValue);)
    ReturnValue = Done_Security_BindProperties(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

#endif // #ifndef MACH_TYPE_STN8500

ErrorCode_e Security_ErrorHandlerRepeat(void *ExecutionContext_p)
{
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint8 *Data_p = NULL;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 CommandNr = 0;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    ReturnValue = (ErrorCode_e)get_uint32_le((void **)&Data_p);
    CommandNr = get_uint16_le((void **)&Data_p);

    switch (CommandNr) {

#ifndef MACH_TYPE_STN8500
    case COMMAND_SECURITY_SETDOMAIN:
        ReturnValue = Done_Security_SetDomain(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_SECURITY_GETDOMAIN:
        ReturnValue = Done_Security_GetDomain(ExeContext_p->Received.SessionNr, ReturnValue, 0);
        ASSERT(E_SUCCESS == ReturnValue);
        break;
#endif

    case COMMAND_SECURITY_SETPROPERTIES:
        ReturnValue = Done_Security_SetProperties(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_SECURITY_GETPROPERTIES:
        ReturnValue = Done_Security_GetProperties(ExeContext_p->Received.SessionNr, ReturnValue, 0, NULL);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_SECURITY_BINDPROPERTIES:
        ReturnValue = Done_Security_BindProperties(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_SECURITY_INITARBTABLE:
        ReturnValue = Done_Security_InitARBTable(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_SECURITY_WRITERPMBKEY:
        ReturnValue = Done_Security_WriteRPMBKey(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    default:
        break;
    }

    if (0 != ExeContext_p->Received.Payload.Size) {
        BUFFER_FREE(ExeContext_p->Received.Payload.Data_p);
    }

    ExeContext_p->Running = FALSE;
    ReturnValue = E_SUCCESS;
    return ReturnValue;
}

/* @}*/

