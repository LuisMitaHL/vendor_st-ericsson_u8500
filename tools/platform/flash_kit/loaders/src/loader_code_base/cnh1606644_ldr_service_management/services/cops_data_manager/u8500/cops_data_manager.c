/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file  cops_data_manager.c
 * @brief Function definitions for COPS data manager
 *
 * @addtogroup ldr_service_cops
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "c_system.h"
#include "r_memory_utils.h"
#include "t_loader_sec_lib.h"
#include "r_service_management.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "cops.h"
#include "cops_types.h"
#include "cops_data_manager.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#ifndef DISABLE_COPS
// COPS context
cops_context_id_t  *Ctx_p;

// Local storage of property data
static cops_bind_properties_arg_t Properties = {
    NULL,  // const cops_imei_t       *imei;
    0,     // size_t                   num_new_auth_data;
    NULL,  // const cops_auth_data_t *auth_data;
    0,     // size_t                   cops_data_length;
    NULL,  // const uint8_t           *cops_data;
    FALSE  // bool                     merge_cops_data;
};

/*******************************************************************************
 * Definition of file internal functions
 ******************************************************************************/

static cops_auth_data_t *CreateSimLockAuthData(uint8 *Data_p, uint32 Length);
static cops_auth_data_t *ParseSimLockAuthData(uint8 *Data_p, uint32 Length);
static cops_auth_data_t *DeleteSimLockAuthData(cops_auth_data_t *Auth);
static cops_auth_data_t *CreateAuthData(uint8 *Data_p, uint32 Length, cops_auth_type_t auth_type);
static cops_auth_data_t *DeleteAuthData(cops_auth_data_t *Auth);
#endif

C_(
    static void PrintChallenge(uint8_t *data, size_t length);
    static void PrintResponse(uint8_t *data, size_t length);)


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
void Do_COPS_Init(void)
{
    return;
}

/*
 * Used to start COPS module.
 *
 * @retval Error code forwarded from COPS module
 *
 * @remark This function is called only from Service management
 */
ErrorCode_e Do_COPS_Start(void)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_return_code_t cops_ret;

    cops_ret = cops_context_create(&Ctx_p, NULL, NULL);
    VERIFY(COPS_RC_OK == cops_ret, E_GENERAL_FATAL_ERROR);

    ReturnValue = E_SUCCESS;

ErrorExit:
#endif
    return ReturnValue;
}

/*
 * Used to stop COPS module.
 *
 * @retval Error code forwarded from COPS module
 *
 * @remark This function is called only from Service management
 */
ErrorCode_e Do_COPS_Stop(void)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    cops_context_destroy(&Ctx_p);

    ReturnValue = E_SUCCESS;
#endif

    return ReturnValue;
}


/*
 * COPS_GetParameter
 *
 * This function returns the requested parameter.
 * It allocates memory for *Data_pp. It is the responsibility of
 * the caller to free the memory.
 *
 * @param [in]  ParameterID               Requested parameter id
 * @param [out] Exist_p                   if Exist == NULL
 *                                        an error will be returned if parameter
 *                                        does not exist.
 *                                        if Exist != NULL
 *                                        boolean information about existence
 *                                        of parameter will be returned.
 * @param [out] Data_pp                   Pointer to the Parameter inside the
 *                                        RAM copy of the data
 * @param [out] Length_p                  Length of the Parameter
 *                                        (Excluding Padding)
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GetParameter
(
    uint32         ParameterID,
    boolean *const Exist_p,
    uint8 **const Data_pp,
    uint32 *const Length_p)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e         ReturnValue;
    cops_return_code_t  CopsRet;
    cops_imei_t         ImeiInMe;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    if (Exist_p) {
        *Exist_p = FALSE;
    }

    *Data_pp = NULL;
    *Length_p = 0;

    switch (ParameterID) {
    case PROP_IMEI:
        A_(printf("cops_data_manager.c (%d): Get PROP_IMEI\n", __LINE__);)

        // Return IMEI stored in ME
        CopsRet = cops_read_imei(Ctx_p, &ImeiInMe);

        if (COPS_RC_OK == CopsRet) {
            // Checksum byte of no interest, skip it
            *Data_pp = (uint8 *)malloc(COPS_UNPACKED_IMEI_LENGTH - 1);
            VERIFY(NULL != *Data_pp, E_ALLOCATE_FAILED);

            memcpy(*Data_pp, ImeiInMe.digits, COPS_UNPACKED_IMEI_LENGTH - 1);

            if (Exist_p) {
                *Exist_p = TRUE;
            }

            *Length_p = COPS_UNPACKED_IMEI_LENGTH - 1;

        } else if (NULL == Exist_p) {
            ReturnValue = E_INVALID_INPUT_PARAMETERS;
        } else {
            A_(printf("cops_data_manager.c(%d): cops read imei failed!\n", __LINE__);)
            ReturnValue = E_COPS_UNDEFINED_ERROR;
        }

        break;

    case DEFAULT_DATA:
        /* Fall through */
    case PROP_SIMLOCK_SET:
        /* Fall through */
    default:
        A_(printf("cops_data_manager.c (%d): Unsupported Unit_id %d\n", __LINE__, ParameterID);)
        ReturnValue = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

ErrorExit:

    /* Unregister of service is done differently from registering of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif

    return ReturnValue;
}


/*
 * COPS_SetParameter
 *
 * This function injects the given parameter to the RAM copy of the data.
 *
 * @param [in]  ParameterID     parameter id
 * @param [in]  Data_p          Pointer to the Parameter to be set.
 * @param [in]  Length          Length of the Parameter (excluding Padding)
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_SetParameter
(
    uint32       ParameterID,
    const uint8 *const Data_p,
    uint32       Length)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e ReturnValue;

    if (NULL == Data_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    switch (ParameterID) {
    case PROP_IMEI:
        A_(printf("cops_data_manager.c (%d): PROP_IMEI len 0x%08x\n", __LINE__, Length);)

        // Sanity check
        VERIFY((COPS_UNPACKED_IMEI_LENGTH - 1) == Length, E_INVALID_INPUT_PARAMETERS);

        // Overwrite any old data
        if (!Properties.imei) {
            // Cache new data
            Properties.imei = (cops_imei_t *)malloc(sizeof(cops_imei_t));
            VERIFY(NULL != Properties.imei, E_ALLOCATE_FAILED);
        }

        memset((uint8 *)Properties.imei->digits, 0xff, COPS_UNPACKED_IMEI_LENGTH);
        memcpy((uint8 *)Properties.imei->digits, (uint8 *)Data_p, COPS_UNPACKED_IMEI_LENGTH - 1);

        B_( {
            int ii;
            printf("IMEI : ");

            for (ii = 0; ii < COPS_UNPACKED_IMEI_LENGTH; ii++)
                printf("0x%02x ", Properties.imei->digits[ii]);
            printf("\n");
        }
          )
        break;

    case PROP_SIMLOCK_SET:
        A_(printf("cops_data_manager.c (%d): PROP_SIMLOCK_SET len 0x%08x\n", __LINE__, Length);)

        // Sanity check
        VERIFY(SIMLOCK_INPUT_DATA_LEN >= Length, E_INVALID_INPUT_PARAMETERS);

        // Overwrite any old data
        Properties.auth_data = DeleteSimLockAuthData((cops_auth_data_t *)Properties.auth_data);

        // Cache new data
        Properties.auth_data = CreateSimLockAuthData((uint8 *)Data_p, Length);
        VERIFY(NULL != Properties.auth_data, E_ALLOCATE_FAILED);

        Properties.num_new_auth_data = 1;
        break;

    case DEFAULT_DATA:
        A_(printf("cops_data_manager.c (%d): DEFAULT_DATA len 0x%08x\n", __LINE__, Length);)

        // Sanity check
        VERIFY(0 != Length, E_INVALID_INPUT_PARAMETERS);

        // Overwrite any old data
        if (Properties.cops_data) {
            free((uint8 *)Properties.cops_data);
        }

        // Cache new data
        Properties.cops_data_length = Length;
        Properties.cops_data = (uint8 *)malloc(Length);
        VERIFY(NULL != Properties.cops_data, E_ALLOCATE_FAILED);

        memcpy((uint8 *)Properties.cops_data, Data_p, Length);
        break;

    default:
        A_(printf("cops_data_manager.c (%d): Unsupported Unit_id %d\n", __LINE__, ParameterID);)
        ReturnValue = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }


ErrorExit:

    /* Unregister of service is done differently from registering of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif

    return ReturnValue;
}


/**
 * COPS_InitARBTable
 *
 * This function initializes the SW version table for ARB functionality.
 *
 *
 * @param [in]  Type            arb type
 * @param [in]  Length          Length of the arb data.
 * @param [in]  Data_p          Pointer to the arb data to be set.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_InitARBTable
(
    uint32        Type,
    uint32        Length,
    const uint8   *const Data_p)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else
    ErrorCode_e         ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_return_code_t  CopsRet;
    cops_init_arb_table_args_t   *Args_p = NULL;
    uint8   *TmpData_p = NULL;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    Args_p = malloc(sizeof(cops_init_arb_table_args_t));
    VERIFY(NULL != Args_p, E_ALLOCATE_FAILED);
    memset((uint8 *) Args_p, 0, sizeof(cops_init_arb_table_args_t));

    Args_p->arb_data_type = Type;

    // Allocate buffer and adjust data for cops
    if (Length == 1) {

        Length++;
        TmpData_p = malloc(Length);
        VERIFY(NULL != TmpData_p, E_ALLOCATE_FAILED);

        TmpData_p[0] = Data_p[0];
        TmpData_p[1] = 0x00;
    } else if (Length == 2) {

        TmpData_p = malloc(Length);
        VERIFY(NULL != TmpData_p, E_ALLOCATE_FAILED);

        TmpData_p[0] = Data_p[1];
        TmpData_p[1] = Data_p[0];
    }

    Args_p->data_length = Length;
    Args_p->data = (uint8 *)TmpData_p;

    C_(printf("cops_data_manager.c: (%d): COPS_InitARBTable\n", __LINE__);)
    C_(printf("cops_data_manager.c: Length 0x%08x, Type (%x)\n", Length, Type);)
    C_(printf("cops_data_manager.c: Data1= %x, Data2= %x\n", TmpData_p[0], TmpData_p[1]);)

    CopsRet = cops_init_arb_table(Ctx_p, Args_p);

    switch (CopsRet) {
    case COPS_RC_OK:
        ReturnValue = E_SUCCESS;
        break;

    case COPS_RC_ARGUMENT_ERROR:
        ReturnValue = E_COPS_ARGUMENT_ERROR;
        break;

        // Should be authenticated to call init_arb_table
    case COPS_RC_NOT_AUTHENTICATED_ERROR:
        ReturnValue = E_COPS_AUTHENTICATION_FAILED;
        break;

    case COPS_RC_INTERNAL_ERROR:
    default:
        ReturnValue = E_COPS_UNDEFINED_ERROR;
        break;
    }

ErrorExit:

    BUFFER_FREE(TmpData_p);
    BUFFER_FREE(Args_p);

    /* Unregister of service is done differently from registering of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif

    return ReturnValue;
}


/**
 * Write cached properties to secure storage.
 *
 * @retval Error code
 **/
ErrorCode_e COPS_BindProperties(void)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e         ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_return_code_t  CopsRet;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    Properties.merge_cops_data = (Properties.imei      == NULL) ||
                                 (Properties.auth_data == NULL) ||
                                 (Properties.cops_data == NULL);

    A_(printf("COPS_BindProperties merge %d\n", Properties.merge_cops_data);)

    CopsRet = cops_bind_properties(Ctx_p, &Properties);

    if (COPS_RC_OK != CopsRet) {
        ReturnValue = E_COPS_UNDEFINED_ERROR;
        /* TODO: cops_bind_properties() currently only returns two error codes:
        **       COPS_RC_UNSPECIFIC_ERROR and
        **       COPS_RC_INTERNAL_ERROR
        **       Update ReturnValue when more detailed error codes are implemented.
        */
    }

    // Throw away all cached variables
    if (NULL != Properties.imei) {
        free((void *)Properties.imei);
        Properties.imei = NULL;
    }

    if (NULL != Properties.auth_data) {
        Properties.num_new_auth_data = 0;
        Properties.auth_data = DeleteSimLockAuthData((cops_auth_data_t *)Properties.auth_data);
    }

    if (NULL != Properties.cops_data) {
        Properties.cops_data_length = 0;
        free((uint8 *)Properties.cops_data);
        Properties.cops_data = NULL;
    }

    /* Unregister of service is done differently from registering of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif

    return ReturnValue;
}


/*
 * Used to authenticate to perform dedicated actions
 *
 * The authentication data may also be used for calculating new protection
 *
 * @param [in]  AuthType      Challenge data type.
 * @param [in]  AuthData_p    Pointer to the challenge data block.
 * @param [in]  AuthDataLen   Length of the challenge data block.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_Authenticate(cops_auth_type_t             AuthType,
                              boolean                      PersistentAuth,
                              uint8_t                     *AuthData_p,
                              uint32                       AuthDataLen)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e         ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_auth_data_t   *Auth_p = NULL;
    cops_return_code_t  CopsRet;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    switch (AuthType) {
    case COPS_AUTH_TYPE_SIMLOCK_KEYS:
        Auth_p = ParseSimLockAuthData(AuthData_p, AuthDataLen);
        VERIFY(Auth_p != NULL, E_ALLOCATE_FAILED);
        break;

    case COPS_AUTH_TYPE_RSA_CHALLENGE:
        Auth_p = CreateAuthData(AuthData_p, AuthDataLen, COPS_AUTH_TYPE_RSA_CHALLENGE);
        VERIFY(Auth_p != NULL, E_ALLOCATE_FAILED);
        break;

    case COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION:
        Auth_p = CreateAuthData(AuthData_p, AuthDataLen, COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION);
        break;

    default:
        goto ErrorExit;
    }

    VERIFY(Auth_p != NULL, E_ALLOCATE_FAILED);

    C_(PrintResponse(Auth_p->data, Auth_p->length);)

    CopsRet = cops_authenticate(Ctx_p, PersistentAuth, Auth_p);


    if (AuthType == COPS_AUTH_TYPE_SIMLOCK_KEYS) {
        (void)DeleteSimLockAuthData(Auth_p);
    } else if ((AuthType == COPS_AUTH_TYPE_RSA_CHALLENGE) || (AuthType == COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION)) {
        (void)DeleteAuthData(Auth_p);
    }

    switch (CopsRet) {
    case COPS_RC_OK:
        ReturnValue = E_SUCCESS;
        break;

    case COPS_RC_ARGUMENT_ERROR:
    case COPS_RC_NOT_AUTHENTICATED_ERROR:
    case COPS_RC_CHALLENGE_MISSING_ERROR:
        // case COPS_RC_SIGNATURE_VERIFICATION_FAILED:  TODO: This value is not declared in cops_types.h
        ReturnValue = E_COPS_AUTHENTICATION_FAILED;
        break;
    case COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT:
        ReturnValue = E_LOADER_SEC_LIB_EXCEEDED_NUMBER_OF_AUTHENTICATION;
        break;
    case COPS_RC_UNSPECIFIC_ERROR:
    case COPS_RC_INTERNAL_ERROR:
    default:
        ReturnValue = E_COPS_UNDEFINED_ERROR;
        break;
    }

ErrorExit:

    /* Unregister of service is done differently from registering of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif
    return ReturnValue;
}

/*
 * Deauthenticate from prior authentications.
 *
 * This function deauthenticates the ME from prior
 * authentications to prevent unauthorized access.
 *
 * @param [in]  permanently If true permanently deauthenticate.
 *                          If false deauthenticate until next
 *                          reboot of the ME..
 *
 * @retval E_SUCCESS The function completed successfully
 * @retval E_COPS_DEAUTHENTICATION_FAILED Error has occured during deauthentication.
 */
ErrorCode_e COPS_Deauthenticate(boolean permanently)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else
    ErrorCode_e         ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_return_code_t  CopsRet;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    CopsRet = cops_deauthenticate(Ctx_p, permanently);
    VERIFY(COPS_RC_OK == CopsRet, E_COPS_DEAUTHENTICATION_FAILED);

ErrorExit:

    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif
    return ReturnValue;
}


/*
 * Generate challenge data block.
 *
 * @param [in]  AuthType      Challenge data type.
 * @param [in]  Buf           Pointer to the challenge data block.
 * @param [in]  Blen          Length of the challenge data block.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GenerateChallenge(cops_auth_type_t             AuthType,
                                   uint8_t                    **Buf,
                                   size_t                      *Blen)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e         ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_return_code_t  CopsRet;
    uint8              *Tmp_p;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    // Determine challenge block length
    CopsRet = cops_get_challenge(Ctx_p, AuthType, NULL, Blen);

    VERIFY(COPS_RC_OK == CopsRet, E_COPS_UNDEFINED_ERROR);

    // Allocate buffer
    Tmp_p = malloc(*Blen);
    VERIFY(NULL != Tmp_p, E_ALLOCATE_FAILED);

    // Get challenge
    CopsRet = cops_get_challenge(Ctx_p, AuthType, Tmp_p, Blen);

    if (COPS_RC_OK != CopsRet) {
        BUFFER_FREE(Tmp_p);
        ReturnValue = E_COPS_UNDEFINED_ERROR;
        goto ErrorExit;
    }

    // Set buffer to data
    *Buf = Tmp_p;

    C_(PrintChallenge(*Buf, *Blen);)

ErrorExit:

    /* Unregister of service is done differently from registering of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif

    return ReturnValue;
}

/*
 * Read OTP data.
 *
 * @param [in]  buffer_p      Pointer to the first data element
 *                            in reserved buffer
 * @param [in]  size          Number of bytes in the reserved buffer.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_ReadOTP(uint32_t *buffer_p, size_t size)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e         ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_return_code_t  CopsRet;

    ReturnValue = Do_Register_Service(COPS_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    CopsRet = cops_read_otp(Ctx_p, buffer_p, size);
    VERIFY(COPS_RC_OK == CopsRet, E_COPS_UNDEFINED_ERROR);

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif

    return ReturnValue;
}

/*
 * Get Authentication Device State.
 *
 * @param [out]  DeviceState_p      Auth Device state.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GetDeviceState(cops_device_state_t *DeviceState_p)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e         ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_return_code_t  CopsRet;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    // Determine challenge block length
    CopsRet = cops_get_device_state(Ctx_p, DeviceState_p);

    VERIFY(COPS_RC_OK == CopsRet, E_COPS_UNDEFINED_ERROR);

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif

    return ReturnValue;
}

/*
 * Write data to OTP.
 *
 * @param [in]  buffer_p      Pointer to first meta data element
 *                            in write data buffer
 * @param [in]  size          Number of bytes in the reserved buffer.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_WriteOTP(uint32_t *buffer_p, size_t size)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e         ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_return_code_t  CopsRet;

    ReturnValue = Do_Register_Service(COPS_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    CopsRet = cops_write_otp(Ctx_p, buffer_p, size);
    VERIFY(COPS_RC_OK == CopsRet, E_COPS_UNDEFINED_ERROR);

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif

    return ReturnValue;
}

/*
 * Invokes programming of RPMB authentication key.
 *
 * @param [in]  dev_id        Represents on which device will be the rpmb
 *                            key written.
 * @param [in]  commercial    Indicates if a commercial key or a development
 *                            key will be used for RPMB.
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_WriteRPMBKey(uint32 dev_id, uint32 commercial)
{
#ifdef DISABLE_COPS
    ErrorCode_e ReturnValue = E_SUCCESS;
#else

    ErrorCode_e        ReturnValue = E_GENERAL_FATAL_ERROR;
    cops_return_code_t CopsRet     = E_COPS_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    A_(printf("cops_data_manager.c (%d): COPS_WriteRPMBKey dev_id = 0x%X, commercial = 0x%X \n", __LINE__, dev_id, commercial);)
    CopsRet = cops_write_rpmb_key(Ctx_p, (uint16)dev_id, (boolean)commercial);

    if (COPS_RC_OK != CopsRet) {
        ReturnValue = E_COPS_UNDEFINED_ERROR;
    } else {
        ReturnValue = E_SUCCESS;
    }

    /* Unregister of service is done differently from registering of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

#endif

    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
#ifndef DISABLE_COPS
static cops_auth_data_t *CreateSimLockAuthData(uint8 *Data_p, uint32 Length)
{
    cops_auth_data_t             *auth;
    cops_simlock_control_keys_t  *keys;
    uint32                        ii;

    if (Length < SIMLOCK_INPUT_DATA_LEN) {
        return NULL;
    }

    auth = (cops_auth_data_t *)(malloc(sizeof(cops_auth_data_t)));

    if (!auth) {
        return NULL;
    }

    keys = (cops_simlock_control_keys_t *)malloc(sizeof(cops_simlock_control_keys_t));

    if (!keys) {
        free(auth);
        return NULL;
    }

    auth->auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
    auth->length = sizeof(cops_simlock_control_keys_t);
    auth->data = (uint8 *)keys;

    ii = 0;
    /** @brief Network lock control key */
    memcpy(keys->nl_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->nl_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;
    ii += COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH;

    /** @brief Network subset lock control key */
    memcpy(keys->nsl_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->nsl_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;
    ii += COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH;

    /** @brief Service provider lock control key */
    memcpy(keys->spl_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->spl_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;
    ii += COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH;

    /** @brief Corporate lock control key */
    memcpy(keys->cl_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->cl_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;
    ii += COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH;

    /** @brief Flexible ESL lock control key */
    memcpy(keys->esll_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->esll_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;

    B_( {

        int ii;
        printf("nl_key  :");

        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->nl_key.value[ii]);
        printf("\nnsl_key :");
        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->nsl_key.value[ii]);
        printf("\nspl_key :");
        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->spl_key.value[ii]);
        printf("\ncl_key  :");
        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->cl_key.value[ii]);
        printf("\nesll_key:");
        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->esll_key.value[ii]);
        printf("\n");
    }
      )
    return auth;
}

static cops_auth_data_t *ParseSimLockAuthData(uint8 *Data_p, uint32 Length)
{
    cops_auth_data_t             *auth;
    cops_simlock_control_keys_t  *keys;
    uint32                        ii;

    if (Length < SIMLOCK_INPUT_DATA_LEN) {
        return NULL;
    }

    auth = (cops_auth_data_t *)(malloc(sizeof(*auth)));

    if (!auth) {
        return NULL;
    }

    keys = (cops_simlock_control_keys_t *)malloc(sizeof(cops_simlock_control_keys_t));

    if (!keys) {
        free(auth);
        return NULL;
    }

    auth->auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
    auth->length = sizeof(cops_simlock_control_keys_t);
    auth->data = (uint8 *)keys;

    ii = 0;
    /** @brief Network lock control key */
    memcpy(keys->nl_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->nl_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;
    ii += (COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH + 1);

    /** @brief Network subset lock control key */
    memcpy(keys->nsl_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->nsl_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;
    ii += (COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH + 1);

    /** @brief Service provider lock control key */
    memcpy(keys->spl_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->spl_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;
    ii += (COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH + 1);

    /** @brief Corporate lock control key */
    memcpy(keys->cl_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->cl_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;
    ii += (COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH + 1);

    /** @brief Flexible ESL lock control key */
    memcpy(keys->esll_key.value, &Data_p[ii], COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH);
    keys->esll_key.value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH] = 0;

    B_( {
        int ii;
        printf("nl_key  :");

        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->nl_key.value[ii]);
        printf("\nnsl_key :");
        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->nsl_key.value[ii]);
        printf("\nspl_key :");
        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->spl_key.value[ii]);
        printf("\ncl_key  :");
        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->cl_key.value[ii]);
        printf("\nesll_key:");
        for (ii = 0; ii < COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH; ii++)
            printf("0x%02x ", keys->esll_key.value[ii]);
        printf("\n");
    }
      )
    return auth;
}

static cops_auth_data_t *DeleteSimLockAuthData(cops_auth_data_t *Auth)
{
    if (Auth) {
        if (Auth->data != NULL) {
            free(Auth->data);
        }

        free(Auth);
    }

    return NULL;
}

static cops_auth_data_t *CreateAuthData(uint8 *Data_p, uint32 Length, cops_auth_type_t auth_type)
{
    cops_auth_data_t             *auth;

    auth = (cops_auth_data_t *)(malloc(sizeof(cops_auth_data_t)));

    if (!auth) {
        return NULL;
    }

    auth->auth_type = auth_type;
    auth->length = Length;
    auth->data = Data_p;

    return auth;
}

static cops_auth_data_t *DeleteAuthData(cops_auth_data_t *Auth)
{
    if (Auth) {
        free(Auth);
    }

    return NULL;
}
#endif

C_(
    static void PrintChallenge(uint8_t *data, size_t length)
{
    printf("cops_data_manager.c: PrintChallenge (Length %d):\n", length);

    for (int i = 1; i < (length + 1); i++, data++) {
        printf("0x%02x ", *data);

        if ((i % 16 == 0) && i != 0) {
            printf("\n");
        }
    }

    printf("\n\n");
}

static void PrintResponse(uint8_t *data, size_t length)
{
    printf("cops_data_manager.c: PrintResponse: (Length %d)\n", length);

    for (int i = 1; i < (length + 1); i++, data++) {
        printf("0x%02x ", *data);

        if (i % 17 == 0 && i != 0) {
            printf("\n");
        }
    }

    printf("\n\n");
}
)

/** @} */
