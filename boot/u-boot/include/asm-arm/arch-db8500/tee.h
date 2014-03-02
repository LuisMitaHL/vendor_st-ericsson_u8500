/*
 * Code ported from tee in ST-Ericsson Linux kernel.
 *
 * Original Linux authors:
 * Copyright (C) ST-Ericsson SA 2010
 * Authors: Shujuan Chen <shujuan.chen@stericsson.com>
 *          Martin Hovang <martin.xm.hovang@stericsson.com>
 *
 * Ported to U-boot by:
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Paer-Olof Haakansson <par-olof.hakansson@stericsson.com>
 *          for ST-Ericsson.
 *
 * License terms: GNU General Public License (GPL) version 2
 */

#ifndef _TEE_H
#define _TEE_H

#include <common.h>

/**
 * Product id numbers, copied from bass_app.h
 * Note: Needs to be replicated from isswapi_types.h
 */
enum tee_product_id {
    PRODUCT_ID_UNKNOWN = 0,
    PRODUCT_ID_8400    = 1, /* Obsolete */
    PRODUCT_ID_8500B   = 2, /* 1080p/1GHz/400MHz */
    PRODUCT_ID_9500    = 3,
    PRODUCT_ID_5500    = 4,
    PRODUCT_ID_7400    = 5,
    PRODUCT_ID_8500C   = 6, /* 720p/1GHz/400MHz */
    PRODUCT_ID_8500A   = 7, /* 720p/800MHz/320MHz */
    PRODUCT_ID_8500E   = 8, /* 1080p/1.15GHz/533MHz */
    PRODUCT_ID_8520F   = 9, /* 720p/1.15GHz/533MHz */
    PRODUCT_ID_8520H   = 10, /* 720p/1GHz/200MHz */
    PRODUCT_ID_9540    = 11,
    PRODUCT_ID_9500C   = 12, /* 1080p/1.15GHz/533MHz */
};

int get_productid(u32 *const product_id);

#endif
