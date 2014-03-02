/*******************************************************************************
 * $Copyright ST-Ericsson 2010 $
 ******************************************************************************/
#ifndef _CSPSA_HANDLER_H_
#define _CSPSA_HANDLER_H_

#include "cspsa.h"

ErrorCode_e InitializeCSPSA(const uint32 Size, CSPSA_Handle_t *const Handle_p);

ErrorCode_e WriteCSPSA(const CSPSA_Handle_t Handle, const uint32 Index, const uint32 DataSize, uint8 *const Data_p);

ErrorCode_e FlushCSPSA(const CSPSA_Handle_t Handle);

ErrorCode_e ReadRawCSPSA(const CSPSA_Handle_t Handle, const uint32 Offset, uint8 *const Data_p, const uint32 Size);

void DestroyCSPSA(CSPSA_Handle_t *const Handle_p);

#endif
