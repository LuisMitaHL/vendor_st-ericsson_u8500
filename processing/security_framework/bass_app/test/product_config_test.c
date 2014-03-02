/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/
#include <stdio.h>

#include <bass_app.h>
#include <bass_app_test.h>
#include <debug.h>
#include <tee_client_api.h>

/**
 * Retrieve the product config and do evaluation.
 */
uint32_t bass_app_test_get_product_config(void)
{
    bass_return_code result = BASS_RC_ERROR_UNKNOWN;
    struct tee_product_config product_config;

    result = get_product_config(&product_config);

    if (BASS_RC_SUCCESS != result)  {
        dprintf(ERROR, "Unexpected result! Error code = %d\n", result);
        goto error;
    } else {
        dprintf(INFO, "Successfully got product config\n");
    }

    dprintf(INFO, "Product id =\n");

    switch (product_config.product_id) {
    case PRODUCT_ID_8400:
        dprintf(INFO, "PRODUCT_ID_8400\n");
        break;
    case PRODUCT_ID_8500B:
        dprintf(INFO, "PRODUCT_ID_8500B\n");
        break;
    case PRODUCT_ID_9500:
        dprintf(INFO, "PRODUCT_ID_9500\n");
        break;
    case PRODUCT_ID_7400:
        dprintf(INFO, "PRODUCT_ID_7400\n");
        break;
    case PRODUCT_ID_8500C:
        dprintf(INFO, "PRODUCT_ID_8500C\n");
        break;
    case PRODUCT_ID_8500A:
        dprintf(INFO, "PRODUCT_ID_8500A\n");
        break;
    case PRODUCT_ID_8500E:
        dprintf(INFO, "PRODUCT_ID_8500E\n");
        break;
    case PRODUCT_ID_8520F:
        dprintf(INFO, "PRODUCT_ID_8500F\n");
        break;
    case PRODUCT_ID_8520H:
        dprintf(INFO, "PRODUCT_ID_8500H\n");
        break;
    case PRODUCT_ID_9500C:
        dprintf(INFO, "PRODUCT_ID_9500C\n");
        break;
    case PRODUCT_ID_8500F:
        dprintf(INFO, "PRODUCT_ID_8500F\n");
        break;
    case PRODUCT_ID_8520E:
        dprintf(INFO, "PRODUCT_ID_8520E\n");
        break;
    case PRODUCT_ID_8520J:
        dprintf(INFO, "PRODUCT_ID_8520J\n");
        break;
    default:
        dprintf(ERROR, "PRODUCT_ID_UNKNOWN: 0x%x\n", product_config.product_id);
        result = BASS_RC_FAILURE;
        break;
    }

    dprintf(INFO, "rt_flags = 0x%X\n", product_config.rt_flags);
    dprintf(INFO, "fuse_flags = 0x%X\n", product_config.fuse_flags);
error:
    return result;
}
