/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "mad/mad.h"
#include "mad_dbus.h"

#include "backend/tx_bo.h"
#include "backend/fd_handler.h"

const mad_dbus_msg_handler_element_t mad_dbus_msg_handlers[] = {
    {
        METHOD_CALL,
        STE_MAD_DBUS_TXBO_IF_NAME,
        "type='method_call', interface='"STE_MAD_DBUS_TXBO_IF_NAME"'",
        txbo_dbus_msg_handler
    },
    {
        METHOD_CALL,
        STE_MAD_DBUS_TETHER_NAME,
        "type='method_call', interface='"STE_MAD_DBUS_TETHER_NAME"'",
        tether_dbus_msg_handler
    },
    {INVALID, NULL, NULL, NULL} /* Must be last */
};

const size_t mad_dbus_msg_handlers_size =
            sizeof(mad_dbus_msg_handlers) / sizeof(mad_dbus_msg_handler_element_t) - 1;


