/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _COPS_DATA_MANAGER_H_
#define _COPS_DATA_MANAGER_H_

/**
 * @file  cops_data_manager.h
 * @brief Function and type declarations for COPS data manager
 *
 * @addtogroup ldr_service_management
 * @{
 *    @addtogroup ldr_service_cops COPS Data Manager
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "cops_data.h"
#include "cops_protect.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Sets valid pointer functions
 */
void Do_COPS_Init(void);


/**
 * Used to start COPS module.
 *
 * @retval Error code forwarded from COPS module
 *
 * @remark This function is caled only from Service management
 */
ErrorCode_e Do_COPS_Start(void);


/**
 * Used to stop COPS module.
 *
 * @retval Error code forwarded from COPS module
 *
 * @remark This function is caled only from Service management
 */
ErrorCode_e Do_COPS_Stop(void);


/**
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
    uint32                    *const Length_p);


/**
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
    uint32                           Length);


/**
 * Used by COPS to store or exchange authentication data.
 *
 * NOTE: Not all data will be accepted. Not allowed for are e.g.:
 * RSA challange or response
 *
 * @param [in]  AuthenticationData_p  Pointer to COPS_Data function pointers to access data
 * @param [in]  SessionType           Defines origin and lifetime of new data.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_NewAuthenticationData
(
    const COPS_AuthenticationData_t    *const AuthenticationData_p,
    COPS_Protect_SessionType_t          SessionType);


/**
 * Used to authenticate to perfom dedicated actions
 *
 * The authentication data may also be used for calculating new protection
 *
 * @param [in]  AuthenticationData_p  Authentication data
 * @param [in]  SessionType           Sessiontype which defines origin and lifetime of the data
 * @param [out] VerifyStatus_p        Result of verification
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_Authenticate
(
    const COPS_AuthenticationData_t    *const AuthenticationData_p,
    COPS_Protect_SessionType_t          SessionType,
    COPS_Protect_VerifyStatus_t  *const VerifyStatus_p);


/**
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
ErrorCode_e COPS_Read(COPS_Data_Condition_t Condition);


/**
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
ErrorCode_e COPS_Write(COPS_Data_Condition_t Condition);


/**
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
ErrorCode_e COPS_CalculateProtection(void);


/**
 * This function verifies all data protected by COPS Protect
 *
 * This function is typically called at the beginning of every cops interface function.
 *
 * @param [out] VerifyStatus_p     Result of verification
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_VerifyData(COPS_Protect_VerifyStatus_t *const VerifyStatus_p);

/**
 * Used by COPS to query which operations are allowed.
 *
 * @param [out] AllowedOP_p     Sturct indication allowed operations
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GetAllowedOperation(COPS_Protect_AllowedOP_t *const AllowedOP_p);

/**
 * This function is used by Loader in order to write default data.
 *
 * @param [in] Buffer_p   Pointer to buffer that contain default data.
 * @param [in] Length     Length od default data buffer.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_WriteDefaultData(uint8 *const Buffer_p, const uint32 Length);

/**
 * This function read the modem lock key from COPS_Data_0.csd
 * and if exist will be used to create new footprints for the modem lock key.
 * If the modem lock key do not exist in the COPS file are do not exist the
 * COPS file, will be created default modem lock key ("1111") with minimal
 * key size(4 digits).
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_Lock_SetDefaultModemLockKey(void);

/**
 * Generate challenge data block.
 *
 * @param [in]  ChallengeType Challenge data type.
 * @param [in]  Challenge_p   Pointer to the challenge data block.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GenerateChallenge(const COPS_Authentication_t ChallengeType, COPS_Challenge_t *const Challenge_p);

/*
 * COPS_LoadAndVerifyAllData
 *
 * This function loads the data into RAM and verifies the protection.
 * Also integrity check is done on loaded parameters.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_LoadAndVerifyAllData(void);

/** @} */
/** @} */
#endif
