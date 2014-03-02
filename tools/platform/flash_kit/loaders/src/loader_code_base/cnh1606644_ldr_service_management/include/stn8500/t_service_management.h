/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_SERVICE_MANAGEMENT_H_
#define _T_SERVICE_MANAGEMENT_H_

/**
 *  @file  t_service_management.h
 *  @brief Type declarations for service management.
 *
 *  @addtogroup ldr_service_management
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_bitset.h"

#include "flash_manager.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 * Supported services
 */
typedef enum {
    COPS_SERVICE,
    GD_SERVICE,         // Global Data MAnager
    BDM_SERVICE,        // Block Device Manager, CABS for now
    BAM_SERVICE,        // Boot Area Manager
    FPD_SERVICE,        // Flash Physical Driver

    NUMBER_OF_SERVICES
} Service_t;

/**
 * Service parameters requested for proper service manager functionality.
 */
typedef struct {
    Service_t ServiceID;
    BitSet32_t Incompatibles;
    BitSet32_t Dependencies;
    BitSet32_t Dependent;
    boolean Started;
    int ReferenceCount;
    ErrorCode_e(* Service_Start_Function)();
    ErrorCode_e(* Service_Stop_Function)();
} ServiceInfo_t;

/** @} */
#endif //_SERVICE_MANAGEMENT_H_
