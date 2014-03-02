/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*
 *
 * \file public_code_header.h
 * \brief header definition for public code
 *
 * The file defines types for public code header definition.
 */
/*---------------------------------------------------------------------------*/
#ifndef PUBLIC_CODE_HEADER_H
#define PUBLIC_CODE_HEADER_H

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "boot_secure_types.h"
#include "issw_types.h"
#include "bip_version.h"

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define PUBLIC_CODE_HEADER_MAGIC                0x50434852

/*-----------------------------------------------------------------------------
 * Types definition
 *---------------------------------------------------------------------------*/
typedef struct {
    t_uint32 code;
    t_uint32 rw_data;
    t_uint32 zi_data;
} t_issw_sect_lengths;

typedef struct 
{
  t_uint32 start;
  t_uint32 end;
} t_issw_hash_limits;

typedef struct sec_rom_hash_descriptor {
  t_issw_hash_limits   limits;
  union {
    t_uint8 digest[MAX_HASH_SIZE];
    struct {
      t_uint32 type;
      t_uint32 size;
    } attribute;
  } data; 
} t_issw_hash_descriptor; 
 
typedef struct {
    t_uint32 magic;
    t_uint32 accelFlags;                        /* To control DMA and PKA acceleration resp on bit 0 and 1 */
    t_section_id sectionId;                     /* section descriptor */
    t_uint32 sectionSize;                       /* size of the section (code + data) */
    t_uint32 paSectionSize;                     /* size of PA section if relevant. 0 if no PA */
    t_uint32 signatureSize;                     /* size of signature */
    t_uint32 hashType;                          /* algo used to hash public code and data */
    t_uint32 signHashType;                      /* algo used to hash header */
    t_uint32 signType;                          /* algo used to sign this header */
    t_issw_hash_descriptor applicationHash;     /* descirption of section hash */
    t_uint32 signedHeaderSize;                  /* size of header which is signed */
    t_uint32 startup;                           /* offset of startup address */
    t_uint32 wdogParam;
    t_uint32 integrityCheckInterval;
    t_uint32 integrityCheckFlag;
    t_uint8  *p_locationCheck;
    t_uint32 spare[8];
} t_public_code_header;


#endif
