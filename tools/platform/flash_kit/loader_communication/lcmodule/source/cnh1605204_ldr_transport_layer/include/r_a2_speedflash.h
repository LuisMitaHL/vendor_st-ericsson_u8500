/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_A2_SPEEDFLASH_H_
#define _R_A2_SPEEDFLASH_H_
/**
 *  @addtogroup ldr_communication_serv
 *  @{
 *    @addtogroup a2_family
 *    @{
 *      @addtogroup a2_speedflash
 *      @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_a2_protocol.h"
#include "t_a2_network.h"
#include "error_codes.h"
#include "t_communication_service.h"

#if (defined(WIN32) || defined(__CYGWIN__))

#ifdef LCM_EXPORTS
#define LCM_API __declspec(dllexport) // export DLL information
#else //LCM_EXPORTS
#define LCM_API __declspec(dllimport) // import DLL information
#endif // LCM_EXPORTS

#elif (defined(__linux__) || defined(__APPLE__))

#ifdef LCM_EXPORTS
#define LCM_API __attribute__((visibility("default")))
#else //LCM_EXPORTS
#define LCM_API
#endif // LCM_EXPORTS

#elif defined(CFG_ENABLE_LOADER_TYPE)

#define LCM_API

#endif  // WIN32

/***********************************************************************
 * Declaration of functions
 **********************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif // #ifdef __cplusplus
    /**
     * Set the LCM in A2 Speedflash Mode.
     *
     * @param [in] Communication_p  Communication module context.
     *
     */
    LCM_API void Do_A2_Speedflash_Start(Communication_t *Communication_p);

    LCM_API void Do_A2_Speedflash_SetLastBlock(Communication_t *Communication_p);

    /**
     * Writes A2 speedflash sub-block on the comm device.
     *
     * @param [in] Communication_p  Communication module context.
     * @param [in] Buffer           Buffer containing the data of the sub-block.
     * @param [in] BufferSize       Size of the speedflash sub-blocks.
     *
     * @retval  E_SUCCESS                  After successful execution.
     */
    LCM_API ErrorCode_e Do_A2_Speedflash_WriteBlock(Communication_t *Communication_p, const void *Buffer, const uint32 BufferSize);
#ifdef __cplusplus
};
#endif // #ifdef __cplusplus

ErrorCode_e A2_Speedflash_Poll(Communication_t *Communication_p);

/** @} */
/** @} */
/** @} */
#endif /* _R_A2_SPEEDFLASH_H_ */
