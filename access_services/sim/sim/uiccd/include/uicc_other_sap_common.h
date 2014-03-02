/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <stdio.h>

// sap extensions
// substate used to differentiate actions on responses
typedef enum {
    UICCD_SAP_SUBSTATE_NONE,
    UICCD_SAP_SUBSTATE_CONNECTING,
    UICCD_SAP_SUBSTATE_CONNECTED,
    UICCD_SAP_SUBSTATE_DISCONNECTING
} uiccd_sap_substate_t;
