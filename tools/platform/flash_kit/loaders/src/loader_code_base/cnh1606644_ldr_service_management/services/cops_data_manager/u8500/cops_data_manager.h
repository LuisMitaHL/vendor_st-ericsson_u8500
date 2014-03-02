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
#include "cops.h"
#include "cops_types.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

#define N_SIMLOCK_CONTROL_KEYS  5

#define SIMLOCK_INPUT_DATA_LEN (COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH * N_SIMLOCK_CONTROL_KEYS)


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
 * @remark This function is called only from Service management
 */
ErrorCode_e Do_COPS_Start(void);


/**
 * Used to stop COPS module.
 *
 * @retval Error code forwarded from COPS module
 *
 * @remark This function is called only from Service management
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
 *                                        boolean information about existence
 *                                        of parameter will be returned.
 * @param [out] Data_pp                   Pointer to the Parameter inside the
 *                                        RAM copy of the data
 * @param [out] Length_p                  Length of the Parameter
 *                                        (excluding Padding)
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GetParameter
(
    uint32                           ParameterID,
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
 * @param [in]  Length          Length of the Parameter (excluding Padding)
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_SetParameter
(
    uint32                           ParameterID,
    const uint8                     *const Data_p,
    uint32                           Length);


/**
 * Write cached properties to secure storage.
 *
 * @retval Error code
 **/
ErrorCode_e COPS_BindProperties(void);


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
    uint32                           Type,
    uint32                           Length,
    const uint8                     *const Data_p);

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
                              uint32                       AuthDataLen);


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
ErrorCode_e COPS_Deauthenticate(boolean permanently);


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
                                   size_t                      *Blen);

/*
 * Read OTP data.
 * This function returns the OTP of the ME.
 *
 * @param [in]  buffer_p      Pointer to the first data element
 *                            in reserved buffer
 * @param [in]  size          Number of bytes in the reserved buffer.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_ReadOTP(uint32_t *buffer_p, size_t size);


/*
 * Write data to OTP.
 * This function writes the OTP of the ME.
 *
 * @param [in]  buffer_p      Pointer to first meta data element
 *                            in write data buffer
 * @param [in]  size          Number of bytes in the reserved buffer.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_WriteOTP(uint32_t *buffer_p, size_t size);


/*
 * Get Authentication Device State.
 *
 * @param [out]  DeviceState_p      Auth Device state.
 *
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_GetDeviceState(cops_device_state_t *DeviceState_p);


/*
 * Invokes programming of RPMB authentication key.
 *
 * @param [in]  dev_id        Represents on which device will be the rpmb
 *                            key written.
 * @param [in]  commercial    Indicates if a commercial key or a development
 *                            key will be used for RPMB.
 * @retval Error code forwarded from COPS module
 */
ErrorCode_e COPS_WriteRPMBKey(uint32 dev_id, uint32 commercial);

/** @} */
/** @} */
#endif
