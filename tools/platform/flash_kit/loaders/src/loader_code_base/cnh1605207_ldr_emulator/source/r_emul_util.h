#ifndef INCLUSION_GUARD_R_EMUL_UTIL_H_
#define INCLUSION_GUARD_R_EMUL_UTIL_H_
/***********************************************************************
 * $Copyright Ericsson AB 2008 $
 **********************************************************************/

/**
 * \addtogroup ldr_emulator
 * @{
 */
#include "error_codes.h"
#include "t_emul_util.h"

/**
 *  This function converts from string to Device ID
 *  @param[in]  Arg_p pointer to an argument.
 *  @param[out]  DevID_p points to a variable where the Device ID shall be placed
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e EMUL_Util_ArgToDevID(sint8 *Arg_p, uint8 *DevID_p);

/**
 *  This function converts from string to Manufacturers ID
 *  @param[in]  Arg_p pointer to an argument.
 *  @param[out]  ManID_p points to a variable where the Manufacturer ID shall be placed
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e EMUL_Util_ArgToManID(sint8 *Arg_p, uint8 *ManID_p);

/**
 *  This function opens A01 file, parses its content and places it in the provided buffer.
 *  @param[in]  Name_p Name of a file to be opened.
 *  @param[out]  Buffer_p Pointer to buffer, where the data will be placed.
 *  @param[in,out]  BufSize_p as input parameter conatins the maximum size of the buffer,
 *               as output parameter it conatins the size of the data placed in the buffer.
 *  @param[out]  Address_p returns the start adress where the data should be placed.
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
ErrorCode_e EMUL_Util_A01ToBuffer(sint8 *Name_p, sint8 *Buffer_p, uint32 *BufSize_p, uint32 *Address_p);
/*@}*/
#endif /*INCLUSION_GUARD_R_EMUL_UTIL_H_*/
