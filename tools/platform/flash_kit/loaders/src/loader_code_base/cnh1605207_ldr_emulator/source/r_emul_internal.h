#ifndef _INCLUSION_GUARD_R_EMUL_INTERNAL_H_
#define _INCLUSION_GUARD_R_EMUL_INTERNAL_H_
/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/

/**
 * \addtogroup ldr_emulator
 * @{
 */
#include "error_codes.h"
#include "t_emulator.h"

/**
 *  This function initializes the default values for Manufacturer ID and Device ID
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e EMUL_ConfigInit(void);

/**
 *  This function configures the nand memory according to the configuration
 *  file provided as input parameter.
 *  @param[in]  FileLine_p is a pointer to a structure containing info about
 *              config file.
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e EMUL_ConfigNand(Line_t *FileLine_p);
/*@}*/
#endif /*_INCLUSION_GUARD_R_EMUL_INTERNAL_H_*/
