/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 */

#ifndef UTEE_TYPES_H
#define UTEE_TYPES_H

enum utee_property {
    UTEE_PROP_TEE_API_VERSION = 0,
    UTEE_PROP_TEE_DESCR,
    UTEE_PROP_TEE_DEV_ID,
    UTEE_PROP_TEE_DATE_PROT_LEVEL,
    UTEE_PROP_TEE_TIME_PROT_LEVEL,
    UTEE_PROP_TEE_ARITH_MAX_BIG_INT_SIZE,
    UTEE_PROP_CLIENT_ID,
    UTEE_PROP_TA_APP_ID,
};

enum utee_time_category {
    UTEE_TIME_CAT_SYSTEM = 0,
    UTEE_TIME_CAT_TA_PERSISTENT,
    UTEE_TIME_CAT_REE
};


#endif /*UTEE_TYPES_H*/
