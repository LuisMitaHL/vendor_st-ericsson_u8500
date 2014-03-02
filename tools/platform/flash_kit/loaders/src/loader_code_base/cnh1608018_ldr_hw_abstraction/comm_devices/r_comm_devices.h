/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_COMM_DEVICES_H_
#define _R_COMM_DEVICES_H_

/**
 * @addtogroup ldr_hw_abstraction
 * @{
 *   @addtogroup comm_devices
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_comm_devices.h"
#include "t_communication_abstraction.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * @brief Get external communication devices parameters.
 *
 * @param [in] Device   Device that should be checked.
 *
 * @return    The appropriate parameters.
 */
void Do_GetDeviceSettings(CommDevId_t Device, DeviceParam_t *DevParam);

/**
 * @brief Set external communication devices parameters.
 *
 * @param [in] DevParam       Parameter to be set.
 * @return     E_SUCCESS   Parameter set successfully.
 *
 * @return     E_FAILED_TO_SET_COMM_DEVICES   Failed to set parameters of
 *                                                  communications devices.
 */
ErrorCode_e Do_SetDeviceSettings(DeviceParam_t DevParam);

/** @}*/
/** @}*/
#endif /*_R_COMM_DEVICES_H_*/
