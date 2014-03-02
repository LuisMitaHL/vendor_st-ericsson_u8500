/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef TEEC_APP_HCL_H
#define TEEC_APP_HCL_H

#include <tee_client_api.h>

typedef t_uint32 t_bass_app_logical_address;
typedef t_uint32 t_bass_app_physical_address;
typedef t_uint32 t_bass_app_sec_rom_exec_config;
typedef t_uint32 t_bass_app_sec_rom_service_id;
/* Secure environment configuration */
typedef t_uint32 t_bass_app_sec_rom_response;

#define PUBLIC        /* Extern by default */
#define PRIVATE      static


typedef struct {
    t_uint32 ChecksumAddr;
    t_uint32 ChecksumValue;
    t_uint32 ConversionFunc;
    t_uint32 BridgeFunc;
    t_uint32 CutName;
} t_bass_app_sec_rom_cut_desc;


/** Secure services identification numbers
 *
 * Here is given the description of the functions that are called by the dispatcher \n
 * when user requests a service number */

typedef enum {
    // "ISSWAPI_ISSW_EXECUTE_SERV" is linked to ROM code ("SSAPI_ISSW_EXECUTE_SERV")
#if (__STN_8500 == 01 )
    BASS_APP_ISSWAPI_ISSW_EXECUTE_SERV       = 5,
#elif (__STN_8500 == 10 ) || (__STN_8500 == 11 )
    BASS_APP_ISSWAPI_ISSW_EXECUTE_SERV       = 6,
#endif
    BASS_APP_ISSWAPI_PROT_APPL_MSG_SEND      = 0x10000000,
    BASS_APP_ISSWAPI_EXTERNAL_CODE_CHECK     = 0x10000001,
    BASS_APP_ISSWAPI_SECURE_LOAD             = 0x10000002,
    BASS_APP_ISSWAPI_ISSW_REIMPORT_PUB_KEYS  = 0x10000003,

    BASS_APP_ISSWAPI_EXECUTE_TA              = 0x11000001,
    BASS_APP_ISSWAPI_CLOSE_TA                = 0x11000002,
    BASS_APP_ISSWAPI_FLUSH_BOOT_CODE         = 0x11000003
} t_bass_app_service_id;


#define SEC_ROM_FORCE_CLEAN_MASK  0x0020  /**< Force cache clean during sec mode exit*/
#define BOOT_PHYSICAL_BASE_ADDR         0x90000000
#define ROM_BASE_ADDR                   0x90000000

PUBLIC void TEEC_Init(void);
PUBLIC t_bass_app_sec_rom_response SECURITY_CallSecureService(t_bass_app_sec_rom_service_id, t_bass_app_sec_rom_exec_config, ...);

PUBLIC t_bass_app_physical_address SECURITY_GetPhysicalAddress(t_bass_app_logical_address);

#endif
