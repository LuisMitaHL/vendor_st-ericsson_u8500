#!/usr/bin/env python
"""
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
"""

"""
Modem Access Daemon Test app

Acts as a AT server to listen to AT commands sent from MAD.
Sends DBus signals to the MAD
"""

from mad_test import at_server, dbus_client
from mad_test.at_server import TxBoEvents
import unittest
import threading


class MadTestApp(unittest.TestCase):

    singleton_setup = False
    server = None
    server_thread = None
    mad_connected = False

    def setupOnce(self):
        print "Starting AT Server"
        server = at_server.AtServer()
        server_thread = threading.Thread(target=server.serve_forever)
        server_thread.setDaemon(True)
        server_thread.start()
        self.__class__.server = server
        self.__class__.server_thread = server_thread
        print "Waiting to MAD to connect to AT"
        self.waitForHandshake()

    def waitForHandshake(self):
        self.server.set_next_response(at_server.AT_OK)
        cmd = self.server.wait_for_next_at_command()
        if at_server.AT_HANDSHAKE == cmd:
            self.__class__.mad_connected = True
        else:
            cmd_hex = map(hex, map(ord, cmd))
            expected_hex = map(hex, map(ord, at_server.AT_HANDSHAKE))
            print "Handshake got: %s" % (cmd_hex)
            print "expected: %s" % (expected_hex)

    def setUp(self):
        if self.__class__.server is None:
            self.setupOnce();
        self.dbus_client = dbus_client.DbusClient()


    def testWifi(self):
        self.assertTrue(self.mad_connected)
        # Send WifiAP On
        self.server.set_next_response(at_server.AT_OK)
        self.dbus_client.send_wifi_hotspot_signal(True)
        # Wait for and parse the response
        cmd = self.server.wait_for_next_at_command()
        self.assertEqual(cmd, at_server.AT_TXBO + str(TxBoEvents.WLAN_HOTSPOT_ACTIVE))

        # Send WifiAP Off
        self.server.set_next_response(at_server.AT_OK)
        self.dbus_client.send_wifi_hotspot_signal(False)
        # Wait for and parse the response
        cmd = self.server.wait_for_next_at_command()
        self.assertEqual(cmd, at_server.AT_TXBO + str(TxBoEvents.WLAN_HOTSPOT_INACTIVE))

        # Send WifiAP Off again
        self.dbus_client.send_wifi_hotspot_signal(False)
        # This time we shouldn't get anything
        cmd = self.server.wait_for_next_at_command_with_timeout(5)
        self.assertTrue(cmd is None)

    def testThermal(self):
        self.assertTrue(self.mad_connected)
        self.server.set_next_response(at_server.AT_OK)
        # Send ThermalManager On
        self.dbus_client.send_thermal_signal(True)
        # Wait for and parse the response
        cmd = self.server.wait_for_next_at_command()
        self.assertEqual(cmd, at_server.AT_TXBO + str(TxBoEvents.THERMAL_MANAGEMENT_ACTIVE))

        # Send ThermalManager Off
        self.server.set_next_response(at_server.AT_OK)
        self.dbus_client.send_thermal_signal(False)
        # Wait for and parse the response
        cmd = self.server.wait_for_next_at_command()
        self.assertEqual(cmd, at_server.AT_TXBO + str(TxBoEvents.THERMAL_MANAGEMENT_INACTIVE))

        # Send WifiAP Off again
        self.dbus_client.send_thermal_signal(False)
        # This time we shouldn't get anything
        cmd = self.server.wait_for_next_at_command_with_timeout(5)
        self.assertTrue(cmd is None)

    def testErrorResponse(self):
        self.assertTrue(self.mad_connected)
        self.server.set_next_response(at_server.AT_ERROR)
        self.dbus_client.send_thermal_signal(True)
        # Wait for and parse the response
        cmd = self.server.wait_for_next_at_command()

        self.server.set_next_response(at_server.AT_ERROR)
        self.dbus_client.send_thermal_signal(False)
        # Wait for and parse the response
        cmd = self.server.wait_for_next_at_command()




if __name__ == '__main__':
    unittest.main()


