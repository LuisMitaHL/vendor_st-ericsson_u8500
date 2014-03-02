/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: WenHai Fang <wenhai.h.fang at stericsson.com>
 *  for ST-Ericsson.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST-Ericsson.
 */

#include <types.h>

/* Version number, upgraded when format changes */
/* Sync with ISSW before upgrading the number */
__attribute__((section(".meminit_version")))
const u32 version = 0x00000002;
