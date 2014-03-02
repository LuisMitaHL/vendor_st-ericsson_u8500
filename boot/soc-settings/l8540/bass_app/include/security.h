/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*------------------------------------------------------------------------
 *                               TPA / MPU
 *                       MultiMedia Platform Unit
 *------------------------------------------------------------------------
 * This module stands for the API interface file of the secure ROM code HCL.
 * It includes the types and functions used by every call to secure mode.
 *------------------------------------------------------------------------
 *
 ****************************************************************************/

#ifndef _SECURE_CODE_H
#define _SECURE_CODE_H

#ifdef  __cplusplus
extern "C" {
#endif      // __cplusplus

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"

/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
/* Secure mode environment setting bit masks. Note that if you modify
 * these definitions, you have to change them also in sec_conf.inc */
#define SEC_ROM_IRQ_ENABLE_MASK                     0x0004			/**< IRQ enabling/disabling*/
#define SEC_ROM_FIQ_ENABLE_MASK                     0x0008			/**< FIQ enabling/disabling*/

// Temporary flag value before flags cleaning
#define SEC_ROM_FORCE_CLEAN_MASK                    0x0020              /**< Force cache clean during sec mode exit*/


/*--------------------------------------------------------------------------*
 * Exported types                                                           *
 * Used by each call to the secure ROM code.
 *--------------------------------------------------------------------------*/

/* Secure environment configuration */
typedef t_uint32 t_sec_rom_response;

/* Secure environment configuration */
typedef t_uint32 t_sec_rom_service_id;

/* Secure environment configuration */
typedef t_uint32 t_sec_rom_exec_config;

/* Secure environment configuration */
typedef t_uint32 t_sec_rom_cut_id;

/*--------------------------------------------------------------------------*
 * Exported functions                                                       *
 *--------------------------------------------------------------------------*/
PUBLIC void SECURITY_Init( void );
PUBLIC void SECURITY_GetVersion( t_version * );
PUBLIC void SECURITY_GetRomVersion( t_uint32 * );
PUBLIC t_sec_rom_response SECURITY_CallSecureService( t_sec_rom_service_id, t_sec_rom_exec_config, ... );
PUBLIC t_physical_address SECURITY_GetPhysicalAddress( t_logical_address );

#ifdef __cplusplus
}           // allow C++ to use these headers
#endif      // __cplusplus

#endif      // _SECURE_CODE_H
