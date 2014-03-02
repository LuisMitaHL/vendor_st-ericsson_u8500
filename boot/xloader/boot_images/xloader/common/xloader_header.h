/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file xloader_header.h
 * \author ST Ericsson
 *
 * \addtogroup SCENARIO
 *
 * This header file provides function prototypes common to all Boot scenarios.
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#ifndef _XLOADER_HEADER_H
#define _XLOADER_HEADER_H

#include "boot_secure_types.h"
#include "issw_types.h"
#include "public_code_header.h"
#include "bip_version.h"


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
} t_xloader_header;


#endif /* _XLOADER_header_H*/
/* end of xloader_version.h */
/** @} */
