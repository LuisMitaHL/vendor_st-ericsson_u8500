/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include "omxilosaldatetimeimpl.h"
#include "omxilosal_debug.h"
#define LOG_NDEBUG 0
#define DBGT_LAYER 0
#define LOG_TAG "OmxILOsalProductId"
#include "linux_utils.h"

#ifndef ANDROID

OMXIL_OSAL_PRODUCT_ID OmxILOsalProductId::GetProductId()
{
    DBGT_ERROR("PRODUCT lib not available, returning : EProductIdUnknown");
    return EProductIdUnknown;
}

#else  // #ifndef ANDROID

extern "C" {
#include "bass_app.h"
}

OMXIL_OSAL_PRODUCT_ID OmxILOsalProductId::GetProductId()
{
    bass_return_code result;
    uint32_t product_id = PRODUCT_ID_UNKNOWN;
    OMXIL_OSAL_PRODUCT_ID productId = EProductIdUnknown;

    result = get_productid(&product_id);

    if (BASS_RC_SUCCESS != result) {
        DBGT_ERROR("get_productid returned error code = %d", result);
        return productId;
    } else {
       DBGT_PTRACE("BASS_RC_SUCCESS");
    }

    switch(product_id) {
    case PRODUCT_ID_8400:
        DBGT_PTRACE("PRODUCT_ID_8400");
        productId = EProductId8400;
        break;
    case PRODUCT_ID_8500:
        DBGT_PTRACE("PRODUCT_ID_8500");
        productId = EProductId8500;
        break;
    case PRODUCT_ID_9500:
        DBGT_PTRACE("PRODUCT_ID_9500");
        productId = EProductId9500;
        break;
#ifndef __STN_5500 // FIXME: below enum are not yet known to u5500
    case PRODUCT_ID_5500:
        DBGT_PTRACE("PRODUCT_ID_5500");
        productId = EProductId5500;
        break;
    case PRODUCT_ID_7400:
        DBGT_PTRACE("PRODUCT_ID_7400");
        productId = EProductId7400;
        break;
    case PRODUCT_ID_8500C:
        DBGT_PTRACE("PRODUCT_ID_8500C");
        productId = EProductId8500C;
        break;
#endif
    case PRODUCT_ID_8500A:
        DBGT_PTRACE("PRODUCT_ID_8500A");
        productId = EProductId8500A;
        break;
    case PRODUCT_ID_8520F:
        DBGT_PTRACE("PRODUCT_ID_8520F");
        productId = EProductId8520F;
        break;
    case PRODUCT_ID_8520H:
        DBGT_PTRACE("PRODUCT_ID_8520H");
        productId = EProductId8520H;
        break;
    default:
        DBGT_ERROR("PRODUCT_ID_UNKNOWN: 0x%x", product_id);
        break;
    }

    return productId;
}

#endif  // #ifndef ANDROID
