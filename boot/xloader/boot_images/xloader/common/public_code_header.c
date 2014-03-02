/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*------------------------------------------------------------------------   */
/*                             Wireless Multimedia                           */
/*                           ROM code and Boot Image Team                    */
/*------------------------------------------------------------------------   */
/* This module provides template xloader.c file implementation               */
/* Specification release related to this implementation:                     */
/*------------------------------------------------------------------------   */
/*                                                                           */
/*****************************************************************************/
#include "bip_version.h"
#include "xloader_header.h"
#include "public_code_header.h"

#include <stddef.h>

/* code entry point defined outside */
extern t_uint8 _c_int00[];


#ifdef __GNUC__
__attribute__((section(".xloader_header")))
#endif /*__GNUC__*/
volatile const t_xloader_header header = {
    PUBLIC_CODE_HEADER_MAGIC,   /* magic                */

    0,              /* accelFlags           */
    {   0,              /* sectionId  uint32 Cut Id  */
        {0, 0, 0, 0, 0, 0},     /* sectionId  uint8 profile[6] */
        {0, 0, 0},          /* sectionId  uint8 bibversion[3] */
        0,              /* sectionId  uint8 bipversion */
        0,              /* sectionId  uint8 MajorVersion */
        0},             /* sectionId  uint8 MinorVersion */
    0,              /* size of signature    */
    0,              /* size of section      */
    0,              /* size of PA section   */
    0,              /* hashType             */
    0,              /* signHashType         */
    0,              /* signType             */
    {               /* applicationHash      */
        {0},
        {{0, 0}}        /* limits start, end    */
    },

    offsetof(t_public_code_header, spare[0])+32,    /* signedHeaderSize  */
    (t_uint32)_c_int00,     /* startup           */
    0,              /* wdogParam         */
    0,              /* integrityCheckIntr*/
    0,              /* integrityCheckFlag*/
    NULL,               /* p_locationCheck      */
    {0}             /* spare[8] => 32 bytes */
};


