/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file bip_version.h
 * \author ST Ericsson
 *
 * \addtogroup SCENARIO
 *
 * This header file provides function prototypes common to all Boot scenarios.
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#ifndef _BIP_VERSION_H
#define _BIP_VERSION_H

#include "boot_types.h"

/* List of Cut ID values */
#define CUT_ID_8500B0   0x008500B0
#define CUT_ID_9540A0   0x009540A0
#define CUT_ID_8540A0   0x008540A0

/* Address in public boot rom where the CUT id is located */
#ifdef MACH_L8540
#define CUT_ID_ADDR 0xFFFFDBF4
#define CUT_ID      CUT_ID_8540A0
#elif (__STN_9540 == 10)
#define CUT_ID_ADDR 0xFFFFDBF4
#define CUT_ID      CUT_ID_9540A0
#elif (__STN_8500 == 20)
#define CUT_ID_ADDR 0x9001dbf4
#define CUT_ID      CUT_ID_8500B0
#else
/* We should not be here ! */
#error "No CHIP value defined !"
#endif

typedef struct {
   t_uint32                  CutId;
   t_uint8                   Profile[6];
   t_uint8                   BIBVersion[3];
   t_uint8                   BIPVersion;
   t_uint8                   MajorVersion;
   t_uint8                   MinorVersion;
} t_section_id;

#endif /* _BIP_VERSION_H*/
/* end of bip_version.h */
/** @} */
