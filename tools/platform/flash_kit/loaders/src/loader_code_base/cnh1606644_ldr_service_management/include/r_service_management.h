/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_SERVICE_MANAGEMENT_H_
#define _R_SERVICE_MANAGEMENT_H_

/**
 * @file  r_service_management.h
 * @brief Function declarations for service management.
 *
 * @addtogroup ldr_service_management Service Management
 * @{
 *
 * Responsible for managing of all supported services.
 *
 * It starts and stops all services completely automatically. The only function
 * that should be used outside of this module is @ref Do_Stop_Service. The
 * service manager constantly monitors number of service users, so if somebody
 * is using the service, it will not be stopped.
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_service_management.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/***************************************************************************//**
* @brief Initializes services.
*
* Initializes all services, function pointers and configuration settings
*
* @remark Must be called before using any service
*
*******************************************************************************/
void Do_Init_Services(void);

/***************************************************************************//**
* @brief Reads and saves services configurations.
*
* Tries to find boot records with services configuration and saves them in
* global variables. Most of services must call this function before startup, to
* update their configuration setting if new platform software is flashed.
* @retval E_INVALID_SIZE_IN_MEMCONF if MEMCONF configuration is nt apropriate for
* flash device instaled on ME
*
*******************************************************************************/
ErrorCode_e Do_Get_Configurations(void);

/***************************************************************************//**
* @brief Registers usage of specified service.
*
* Registers that somebody is using specified service. If servise is not started,
* it tries to start it.
*
* @param[in]  Service   Service that should be registered.
*
* @retval E_SERVICE_NOT_SUPPORTED if service is unknown.
* @retval E_INCONSISTENCY_IN_SERVICES if some unexpected behavior is detected.
* @retval E_SUCCESS if service was successfuly registered.
*
* @remark This function should be called only from Service management module.
*
*******************************************************************************/
ErrorCode_e Do_Register_Service(Service_t Service);


/***************************************************************************//**
* @brief Unregisters usage of specified service.
*
* Marks that some user is no longer using specified service.
*
* @param[in]  Service   Service that should be unregistered.
*
* @retval E_INCONSISTENCY_IN_SERVICES if some unexpected behavior is detected.
* @retval E_SUCCESS if service was successfuly unregistered.
*
* @remark This function should be called only from Service management module.
*
*******************************************************************************/
ErrorCode_e Do_UnRegister_Service(Service_t Service);


/***************************************************************************//**
* @brief Stops specified service.
*
* If nobody is using specified service, it will try to stop it.
*
* @param[in]  Service   Service that should be stopped.
*
* @retval E_SERVICE_NOT_SUPPORTED if service is unknown.
* @retval E_SERVICE_IN_USE if service is in use.
* @retval E_SUCCESS if service was successfuly stopped.
*
* @remark Call this function with NUMBER_OF_SERVICES as input parameter to
* shutdown all supported services.
*******************************************************************************/
ErrorCode_e Do_Stop_Service(Service_t Service);


/***************************************************************************//**
* @brief Checks if the service is started.
*
* @param[in]  Service   Service that should be checked.
*
* @retval FALSE if service is not started.
* @retval TRUE if service is started.
*
*******************************************************************************/
boolean IsStarted(Service_t Service);


/***************************************************************************//**
* @brief Returns current number of service users.
*
* @param[in]  Service   Service that should be checked.
*
* @return Number of users.
*
*******************************************************************************/
int GetReferenceCount(Service_t Service);

/** @} */
#endif //_SERVICE_MANAGEMENT_H_
