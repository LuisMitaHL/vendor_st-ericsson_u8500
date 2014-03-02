/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*------------------------------------------------------------------------   */
/* 	                             Wireless Multimedia                     	 */
/*                           ROM code and Boot Image Team                    */
/*------------------------------------------------------------------------   */
/* This module provides template xloader.c file implementation               */
/* Specification release related to this implementation:                     */
/*------------------------------------------------------------------------   */
/*                                                                           */
/*****************************************************************************/

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "xloader.h"

extern t_uint32 XL_GetLastReset(t_boot_last_reset *p_lastReset);
extern  t_uint8 XL_GetAB8500CutId(void);
extern void XL_WriteSSP(t_uint8 block, t_uint32 address, t_uint8 data);
extern t_uint8 XL_ReadSSP(t_uint8 block, t_uint32 address);
extern void XL_WriteI2C(t_uint8 block, t_uint8 address, t_uint8 data);
extern t_uint8 XL_ReadI2C(t_uint8 block, t_uint8 address);
extern void XL_SendToken(t_pub_rom_loader_info   *p_loaderInfo,
                      t_uint32              token);

extern void XL_DisableCachesMmu(void);
extern void XL_CleanInvalidateL2(void);
/* end of xl-drivers.h */
