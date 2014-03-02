#ifndef _INCLUSION_GUARD_R_EMULATOR_H_
#define _INCLUSION_GUARD_R_EMULATOR_H_
/***********************************************************************
 * $Copyright Ericsson AB 2008 $
 **********************************************************************/

/**
 * \addtogroup ldr_emulator
 * @{
 * @todo A lot of improvement can be done in this module. Becuse of the lack of time
 *       this is left for latter improvement. Also this module is not used in the real
 *       loaders and it doesn't impact the security of the platform.
 */

#include "error_codes.h"
#include "t_emulator.h"

/**
 *  This function configures the emulator. It configures NAND device,
 *  UART device, and some other parameters used by the emulator.
 *  @param[in]  Argc this is the number of command line arguments.
 *              Received from the main function.
 *  @param[in]  Argv_p this is pointer to list of arguments received from
 *              the command line in the main function.
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e EMUL_Config(uint32 Argc, char *Argv_p[]);

/**
 *  This function initializes some variables used by the emulator, such as:
 *  Events, Critical Sections etc.
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e EMUL_Init();
/*@}*/
#endif //_INCLUSION_GUARD_R_EMULATOR_H_
