/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_LDR_INIT_H_
#define _INCLUSION_GUARD_LDR_INIT_H_

/**
 *  @addtogroup ldr_main
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"

#ifndef CFG_ENABLE_PROLOGUE

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 *  Hardware initialization.
 *
 *  @retval E_SUCCESS After successful execution.
 */
ErrorCode_e Do_Loader_Hardware_Init(void);

/**
 *  Communication protocol initialization.
 *
 *  @retval E_SUCCESS After successful execution.
 */
ErrorCode_e Do_Loader_CommunicationProtocolInit(void);

#else
/**
 *  Prologue Hardware initialization.
 *
 *  @retval E_SUCCESS After successful execution.
 */
ErrorCode_e Do_Prologue_Hardware_Init(void);

#endif //#ifndef CFG_ENABLE_PROLOGUE

/** @} */
#endif /*_INCLUSION_GUARD_LDR_INIT_H_*/
