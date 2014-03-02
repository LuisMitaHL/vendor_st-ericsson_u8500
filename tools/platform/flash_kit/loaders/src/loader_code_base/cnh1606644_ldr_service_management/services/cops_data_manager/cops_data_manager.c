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
#include <string.h>
#include "t_system.h"
#include "r_service_management.h"
#include "r_debug_macro.h"
#include "cops_extint.h"
#include "cops_protect.h"
#include "cops_data.h"
#include "cops_data_manager.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/* Offset of COPS error codes in loader local error code table. */
#define COPS_ERROR_OFFSET 1650

extern const COPS_ExtInt_FlexStorage_t COPS_ExtInt_FlexStorage;
extern const COPS_ExtInt_Security_t COPS_ExtInt_Security;
extern const COPS_ExtInt_PMC_t COPS_ExtInt_PMC;

static COPS_Protect_Root_t *COPS_Protect_Root_p  = NULL;
static COPS_Data_Root_t    *COPS_Data_Root_p     = NULL;

/*******************************************************************************
 * Definition of file internal functions
 ******************************************************************************/
static COPS_ReturnCode_t COPS_WriteData
(
    uint8                    *const Buffer_p,
    const uint32                          Length
);

static COPS_RC_t COPS_Core_LoadVerifyAllData
(
    COPS_Data_Condition_t       DataCondition,
    boolean                     RequireDataOK
);


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
 * @remark This function is caled only from Service management
 */
ErrorCode_e Do_COPS_Start(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    COPS_Result = COPS_Data_Init(&COPS_Data_Root_p, &COPS_ExtInt_FlexStorage);
    A_(printf("cops_data_manager.c (%d): ReturnValue= %d **\n", __LINE__, COPS_Result);)

    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    COPS_Result = COPS_Data_Read(COPS_Data_Root_p, COPS_DATA_CONDITION_FORCE);
    A_(printf("cops_data_manager.c (%d): ReturnValue= %d **\n", __LINE__, COPS_Result);)
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    COPS_Result = COPS_Protect_Init(&COPS_Protect_Root_p, COPS_Data_Root_p, &COPS_ExtInt_Security, &COPS_ExtInt_PMC);
    A_(printf("cops_data_manager.c (%d): ReturnValue= %d **\n", __LINE__, COPS_Result);)
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * Used to stop COPS module.
 *
 * @retval Error code forwarded from COPS module
 *
 * @remark This function is caled only from Service management
 */
ErrorCode_e Do_COPS_Stop(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    COPS_Result = COPS_Protect_DeInit(&COPS_Protect_Root_p);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    COPS_Result = COPS_Data_DeInit(&COPS_Data_Root_p);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    return ReturnValue;
}


/*
 * COPS_GetParameter
 *
 * This function returns a pointer to the requested parameter.
 * It directly points the ram copy of the data.
 * When the size is not extended a directly modification of the data
 * is possible.
 * But still a call to COPS_SetParameter is necessary.
 *
 * @param [in]  ParameterID               Requested parameter id
 * @param [out] Exist_p                   if Exist == NULL
 *                                        an error will be returned if parameter
 *                                        does not exist.
 *                                        if Exist != NULL
 *                                        boolean information about existance
 *                                        of parameter will be returend.
 * @param [out] Data_pp                   Pointer to the Parameter inside the
 *                                        RAM copy of the data
 * @param [out] Length_p                  Length of the Parameter
 *                                        (exluding Padding)
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GetParameter
(
    COPS_Storage_ParameterID_t       ParameterID,
    boolean                   *const Exist_p,
    uint8                    **const Data_pp,
    uint32                    *const Length_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Data_General_GetParameter(&(COPS_Data_Root_p->General), ParameterID, Exist_p, Data_pp, Length_p);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}


/*
 * COPS_SetParameter
 *
 * This function injects the given parameter to the RAM copy of the data.
 *
 * When the pointer of Data_p is identical with the RAM copy it only will update
 * the Length field and set the modified flags correctly.
 * Otherwise it will alloc memory and copy the data.
 *
 * @param [in]  ParameterID     parameter id
 * @param [in]  Data_p          Pointer to the Parameter to be set.
 * @param [in]  Length          Length of the Parameter (exluding Padding)
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_SetParameter
(
    COPS_Storage_ParameterID_t       ParameterID,
    const uint8                     *const Data_p,
    uint32                           Length)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Data_General_SetParameter(&(COPS_Data_Root_p->General), ParameterID, Data_p, Length);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * Used by COPS to store or exchange authentication data.
 *
 * NOTE: Not all data will be accepted. Not allowed for are e.g.:
 * RSA challange or response
 *
 * @param [IN]  AuthenticationData_p  Pointer to COPS_Data function pointers to access data
 * @param [IN]  SessionType           Defines origin and lifetime of new data.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_NewAuthenticationData
(
    const COPS_AuthenticationData_t    *const AuthenticationData_p,
    COPS_Protect_SessionType_t          SessionType)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Protect_NewAuthenticationData(COPS_Protect_Root_p, AuthenticationData_p, SessionType);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * Used to authenticate to perfom dedicated actions
 *
 * The authentication data may also be used for calculating new protection
 *
 * @param [IN]  AuthenticationData_p  Authentication data
 * @param [IN]  SessionType           Sessiontype which defines origin and lifetime of the data
 * @param [OUT] VerifyStatus_p        Result of verification
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_Authenticate
(
    const COPS_AuthenticationData_t    *const AuthenticationData_p,
    COPS_Protect_SessionType_t          SessionType,
    COPS_Protect_VerifyStatus_t  *const VerifyStatus_p)
{
    ErrorCode_e                 ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t                   COPS_Result = COPS_RC_UNDEFINED_ERROR;
    COPS_SubDomain_t            SubDomain = COPS_SUBDOMAIN_THREE;
    uint32                      COPS_SubDomain_Length = 1;
    uint8                       *COPS_SubDomain_p = NULL;
    boolean                     SubDomain_Exist = FALSE;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Data_General_GetParameter(&(COPS_Data_Root_p->General), COPS_STORAGE_PARAMETER_ID_SUBDOMAIN, &SubDomain_Exist, &COPS_SubDomain_p, &COPS_SubDomain_Length);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));
    SubDomain = (COPS_SubDomain_t) * COPS_SubDomain_p;

    COPS_Result = COPS_Protect_Authenticate(COPS_Protect_Root_p, AuthenticationData_p, SessionType, SubDomain, VerifyStatus_p);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * COPS_Read
 *
 * Read COPS_Data files from flash into RAM.
 *
 * When Contition is set to COPS_DATA_CONDITION_FORCE the data are read in
 * any case
 *
 * When Contition is set to COPS_DATA_CONDITION_AUTO the data are only read
 * when the data in memory are not valid. (e.g not loaded)
 *
 * @param [in] Condition       Conditions when data should be loaded
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_Read
(
    COPS_Data_Condition_t     Condition)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Data_Read(COPS_Data_Root_p, Condition);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * COPS_Write
 *
 * Commits changed to the data done in RAM to flash
 *
 * When Contition is set to COPS_DATA_CONDITION_FORCE the data are written in
 * any case
 *
 * When Contition is set to COPS_DATA_CONDITION_AUTO the data are only written
 * if they where modified.
 *
 * @param [in] Condition       Conditions when data should be loaded
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_Write
(
    COPS_Data_Condition_t     Condition)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Data_Write(COPS_Data_Root_p, Condition);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * Recalculates integrity protection of protected data.
 *
 * This function will fail if there are currently not all sufficient
 * rights/data avaiable to perform the recalculation on all modiefied or
 * not correctly signed data.
 *
 * The rights are got from previous executed COPS_Authenticate and
 * CPOP_Protect_VerifyControlKey calles.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_CalculateProtection(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Protect_CalculateProtetion(COPS_Protect_Root_p);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * This function verifies all data protected by COPS Protect
 *
 * This function is typically called at the beginning of every cops interface function.
 *
 * @param [OUT] VerifyStatus_p     Result of verification
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_VerifyData
(
    COPS_Protect_VerifyStatus_t *const VerifyStatus_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Protect_VerifyData(COPS_Protect_Root_p, VerifyStatus_p);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * Used by COPS to query which operations are allowed.
 *
 * @param [IN]  Protect_Root_p  Pointer to Protect conext.
 * @param [OUT] AllowedOP_p     Sturct indication allowed operations
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GetAllowedOperation(COPS_Protect_AllowedOP_t *const AllowedOP_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Protect_GetAllowedOP(COPS_Protect_Root_p, AllowedOP_p);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}


/*
 * This function is used by Loader in order to write default data.
 *
 * @param [IN] Buffer_p   Pointer to buffer that contain default data.
 * @param [IN] Length     Length od default data buffer.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_WriteDefaultData(uint8 *const Buffer_p, const uint32 Length)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_WriteData(Buffer_p, Length);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * This function read the modem lock key from COPS_Data_0.csd
 * and if exist will be used to create new footprints for the modem lock key.
 * If the modem lock key do not exist in the COPS file are do not exist the
 * COPS file, will be created default modem lock key ("1111") with minimal
 * key size(4 digits).
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_Lock_SetDefaultModemLockKey(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result        = COPS_RC_OK;
    ASCII_t   ModemLockDefaultKey[COPS_MODEMLOCK_CONTROLKEY_MIN_LENGTH] = {0,};
    uint32    ModemLockDefaultKeySize;
    COPS_AuthenticationData_t ModemLockAuthenticationData =
    { COPS_AUTHENTICATION_NONE, NULL, 0 };

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    /* Set default modem lock key */
    ModemLockDefaultKeySize = COPS_MODEMLOCK_CONTROLKEY_MIN_LENGTH;
    //ModemLockDefaultKey buffer should have default values '0' ('0' == 0x30)
    memset(ModemLockDefaultKey, 0x30, COPS_MODEMLOCK_CONTROLKEY_MIN_LENGTH);


    /* Wrap control key into Authentication data */
    COPS_Result = COPS_Protect_WrapControlKey(&ModemLockDefaultKey[0], ModemLockDefaultKeySize,
    COPS_STORAGE_LOCKTYPE_MODEMLOCK,
    COPS_PROTECT_KEYTYPE_UNDEFINED,
    &ModemLockAuthenticationData);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    /* Set new Control Key */
    COPS_Result = COPS_Protect_NewAuthenticationData(COPS_Protect_Root_p,
    &ModemLockAuthenticationData,
    COPS_PROTECT_SESSIONTYPE_COPS);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:
    COPS_Protect_FreeWrapedControlKey(&ModemLockAuthenticationData);

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * Generate challenge data block.
 *
 * @param [in]  ChallengeType Challenge data type.
 * @param [in]  Challenge_p   Pointer to the challenge data block.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GenerateChallenge(const COPS_Authentication_t ChallengeType, COPS_Challenge_t *const Challenge_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    COPS_Result = COPS_Protect_GenerateChallenge(COPS_Protect_Root_p, ChallengeType, Challenge_p);
    VERIFY(COPS_RC_UNDEFINED_ERROR != COPS_Result, E_COPS_UNDEFINED_ERROR);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*
 * COPS_LoadAndVerifyAllData
 *
 * This function is wraped around COPS_Core_LoadVerifyAllData function and
 * export functionality as API to loader modules.
 */
ErrorCode_e COPS_LoadAndVerifyAllData(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    COPS_RC_t COPS_Result = COPS_RC_OK;

    ReturnValue = Do_Register_Service(COPS_SERVICE);

    if (E_SUCCESS != ReturnValue) {
        return ReturnValue;
    }

    /*
     * Read data into memory and verify data (integrity check included).
     */
    COPS_Result = COPS_Core_LoadVerifyAllData(COPS_DATA_CONDITION_AUTO, TRUE);
    VERIFY(COPS_RC_OK == COPS_Result, (ErrorCode_e)(COPS_Result + COPS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;

ErrorExit:

    /* Unregister of service is done differently from registring of service
       because it is done in ErrorExit part while still allowing COPS error set
       in VERIFY macro to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(COPS_SERVICE)) {
        ReturnValue = E_UNREGISTER_COPS_SERVICE_FAILED;
    }

    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/*
 * COPS_WriteData
 *
 * This function is used by Loader in order to write default data.
 *
 * @param [in]  Buffer_p    Pointer to buffer that contain default data.
 * @param [in]  Length      Length od default data buffer.
 *
 */
static COPS_ReturnCode_t COPS_WriteData
(
    uint8                    *const Buffer_p,
    const uint32                          Length
)
{
    COPS_RC_t                ReturnValue = COPS_RC_OK;
    COPS_Protect_AllowedOP_t AllowedOP;

    /*
     * Read data into memory and verify data but do not require
     * that the integrity is intact.
     */
    ReturnValue = COPS_Core_LoadVerifyAllData(COPS_DATA_CONDITION_AUTO, FALSE);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

    /* Check if operation is allowed */
    ReturnValue = COPS_Protect_GetAllowedOP(COPS_Protect_Root_p, &AllowedOP);

    if (!AllowedOP.WriteErase_COPS_DATA) {
        B_(printf("cops_data_manager.c %d: Authentication insufficient.\n", __LINE__);)
        ReturnValue = COPS_RC_AUTHENTICATION_FAILED;
        goto ErrorExit;
    }

    /* Merge supplied data with existing secure data */
    ReturnValue = COPS_Data_Merge(COPS_Data_Root_p, Buffer_p, Length);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

    /* Commit data to flash */
    ReturnValue = COPS_Data_Write(COPS_Data_Root_p, COPS_DATA_CONDITION_FORCE);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

    /* Read data once again */
    ReturnValue = COPS_Data_Read(COPS_Data_Root_p, COPS_DATA_CONDITION_FORCE);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

ErrorExit:
    B_(printf("cops_data_manager.c %d: COPS_WriteData return %d.\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/*
 * COPS_Core_LoadVerifyAllData
 *
 * This function loads the data into RAM and verifies the protection.
 * If RequireDataOK is TRUE the function will return an error if
 * the integrity of the data cannot be verified.
 *
 * NOTE: This function does NOT check the LockSetting protections
 *
 * @param [in]  DataCondition    Defines how data are loaded.
 * @param [in]  RequireDataOK    Determines if an error should be returned if
 *                               data are not OK
 *
 */
static COPS_RC_t COPS_Core_LoadVerifyAllData
(
    COPS_Data_Condition_t       DataCondition,
    boolean                     RequireDataOK
)
{
    COPS_RC_t ReturnValue = COPS_RC_OK;
    COPS_Protect_VerifyStatus_t VerifyStatus  = COPS_PROTECT_VERIFYSTATUS_NOT_VERIFIED;

    ReturnValue = COPS_Data_Read(COPS_Data_Root_p, DataCondition);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

    ReturnValue = COPS_Protect_VerifyData(COPS_Protect_Root_p, &VerifyStatus);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

    if (RequireDataOK && VerifyStatus != COPS_PROTECT_VERIFYSTATUS_OK) {
        B_(printf("cops_data_manager.c %d: Data Tampered.\n", __LINE__);)
        ReturnValue = COPS_RC_DATA_TAMPERED;
        goto ErrorExit;
    }

ErrorExit:
    B_(printf("cops_data_manager.c %d: COPS_Core_LoadVerifyAllData return %d.\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/** @} */
