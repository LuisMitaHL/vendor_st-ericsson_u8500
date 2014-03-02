/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef MAD_H
#define MAD_H

/* The MAD Service path */
#define STE_MAD_DBUS_SERVICE "com.stericsson.mad"

/* The DBus object path of MAD */
#define STE_MAD_DBUS_OBJECT_NAME "/com/stericsson/mad"

/* Common prefix of MAD's DBus interfaces */
#define STE_MAD_DBUS_IF_BASE_NAME "com.stericsson.mad."

/* TX Backoff */
#define STE_MAD_DBUS_TXBO_IF_NAME STE_MAD_DBUS_IF_BASE_NAME"txbo"

/* Dbus interface for receiving USB Tether events */
#define STE_MAD_DBUS_TETHER_NAME STE_MAD_DBUS_IF_BASE_NAME"fd_handler"

/* TX Backoff events (each can have a value STE_MAD_FEATURE_OFF or STE_MAD_FEATURE_ON) */
#define STE_MAD_TXBO_WIFI_AP "WifiAP"
#define STE_MAD_TXBO_THERMAL_MANAGER "ThermalManager"

/* Dbus Method for receiving USB Tethering events */
#define STE_MAD_USB_TETHER "UsbTethering"

#define STE_MAD_FEATURE_OFF "Off"
#define STE_MAD_FEATURE_ON  "On"


#endif        /* #ifndef MAD_H */

