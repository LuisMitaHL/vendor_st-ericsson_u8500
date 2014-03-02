/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef AT_HANDLER_H
#define AT_HANDLER_H

typedef enum {
    NO_EVENT                        = 0,
    FOLD_SLIDE_MECHANISM_CLOSED     = 1,
    FOLD_SLIDE_MECHANISM_OPEN       = 2,
    DVBH_INACTIVE                   = 3,
    DVBH_ACTIVE                     = 4,
    PROXIMITY_SENSOR_FAR            = 5,
    PROXIMITY_SENSOR_NEAR           = 6,
    GPS_INACTIVE                    = 7,
    GPS_ACTIVE                      = 8,
    ANTENNA_INACTIVE                = 9,
    ANTENNA_ACTIVE                  = 10,
    THERMAL_MANAGEMENT_INACTIVE     = 11,
    THERMAL_MANAGEMENT_ACTIVE       = 12,
    WLAN_HOTSPOT_INACTIVE           = 13,
    WLAN_HOTSPOT_ACTIVE             = 14,
    SPURIOUS_BATTERY_STATE_INACTIVE = 15,
    SPURIOUS_BATTERY_STATE_ACTIVE   = 16,
    LOW_BATTERY_STATE_INACTIVE      = 17,
    LOW_BATTERY_STATE_ACTIVE        = 18
} mad_epwrred_event_values_t;


int mad_at_handler_init();
int mad_at_handler_shutdown();
int mad_at_handler_send_FD(int event);
int mad_at_handler_send_txbo(const mad_epwrred_event_values_t event);

#endif        /* #ifndef AT_HANDLER_H */

