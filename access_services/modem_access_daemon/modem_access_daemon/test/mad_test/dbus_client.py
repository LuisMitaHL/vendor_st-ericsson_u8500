"""
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
"""

import dbus

STE_MAD_DBUS_CONN_NAME = 'com.stericsson.mad'
STE_MAD_DBUS_OBJECT_NAME = '/com/stericsson/mad'
STE_MAD_DBUS_IF_BASE_NAME = 'com.stericsson.mad.'

STE_MAD_DBUS_TXBO_IF_NAME = STE_MAD_DBUS_IF_BASE_NAME + 'txbo'

STE_MAD_TXBO_WIFI_AP = 'WifiAP'
STE_MAD_TXBO_THERMAL_MANAGER = 'ThermalManager'

STE_MAD_FEATURE_OFF = 'Off'
STE_MAD_FEATURE_ON = 'On'

class DbusClient:

    def __init__(self):
        self.bus = dbus.SessionBus()
        self.mad_object = self.bus.get_object(STE_MAD_DBUS_CONN_NAME,
                                            STE_MAD_DBUS_OBJECT_NAME, False)
        self.txbo = dbus.Interface(self.mad_object,
                                   dbus_interface=STE_MAD_DBUS_TXBO_IF_NAME)



    def send_wifi_hotspot_signal(self, state):
        if state:
            self.txbo.WifiAP(STE_MAD_FEATURE_ON)
        else:
            self.txbo.WifiAP(STE_MAD_FEATURE_OFF)

    def send_thermal_signal(self, state):
        if state:
            self.txbo.ThermalManager(STE_MAD_FEATURE_ON)
        else:
            self.txbo.ThermalManager(STE_MAD_FEATURE_OFF)


