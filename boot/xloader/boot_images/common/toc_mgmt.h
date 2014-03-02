/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file toc_mgmt.h
 * \author STMicroelectronics
 *
 * \addtogroup SCENARIO
 *
 * This header file provides function prototypes common to all Boot scenarios.
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#ifndef _TOC_MGMT_H
#define _TOC_MGMT_H

#include "boot_types.h"
#include "pub_code_address.h"

#define ASCII_XLOADER     "X-LOADER"
#define ASCII_PWR_MGT     "PWR_MGT"
#define ASCII_MEM_INIT    "MEM_INIT"
#define ASCII_IPL         "IPL"
#define ASCII_MODEM       "MODEM"
#define ASCII_NORMAL      "NORMAL"
#define ASCII_PRODUCTION  "PRODUCTION"
#define ASCII_ADL         "ADL"
#define ASCII_DNT_CERT    "DNT-CERT"
#define ASCII_SUBTOC      "SUBTOC"
#define ASCII_TEE         "TEE"


/* Token for I/F Boot */
#define XLOADER_TOKEN         0xA0400000
#define MEM_INIT_TOKEN        0xA0300000
#define NORMAL_TOKEN          0xA0300001
#define PWR_MGT_TOKEN         0xA0300002
#define IPL_TOKEN             0xA0300003
#define MODEM_TOKEN           0xA0300004
#define DNT_CERT_TOKEN        0xA0300005
#define TEE_TOKEN             0xA030000B
#define MEM_INIT_PASSED_TOKEN 0xD0700000
#define MEM_INIT_FAILED_TOKEN 0xD0700001

#define BOOT_TOC_FLAGS_DMA_MASK  0x40000000

typedef enum {
    UNDEF_ITEM_ID   = 0,
    XLOADER_ITEM_ID = 1,
    IPL_ITEM_ID     = 2,
    PWR_MGT_ITEM_ID = 3,
    DNT_CERT_ITEM_ID = 4,
} t_public_item;


/*------------------------------------------------------------------------
 * Public functions
 *----------------------------------------------------------------------*/

PUBLIC t_bool TOC_GetItemInfo(t_uint32, t_uint32, t_uint32, char*,
                    t_size*, t_address*, t_bool*, t_uint32*);
extern  t_bool TOC_SearchItemInToc(const t_uint32 tocAddress,
                    const t_uint32 subtocAddress,
                    const t_uint32 subtocSize,
                    const char *p_stringItem,
                    t_boot_toc_item *p_tocItem,
                    t_bool *p_in_subtoc);
extern t_bool toc_find_subtoc(t_uint32 toc_address, t_uint32 *p_subtoc_size);

#endif /* _TOC_MGMT_H*/
/* end of toc_mgmt.h */
/** @} */
