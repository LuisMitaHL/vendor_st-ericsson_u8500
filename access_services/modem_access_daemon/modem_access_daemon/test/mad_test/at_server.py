"""
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
"""

import SocketServer
import Queue
import socket
import os
import sys
from Queue import Empty


AT_SOCKET_PATH  = "/tmp/mad_test_at_socket"

AT_OK = "OK\r"
AT_ERROR = "ERROR\r"

AT_HANDSHAKE = "ATE0Q0V1"
AT_TXBO = "AT*EPWRRED="

class TxBoEvents:
    NO_EVENT                        = 0
    FOLD_SLIDE_MECHANISM_CLOSED     = 1
    FOLD_SLIDE_MECHANISM_OPEN       = 2
    DVBH_INACTIVE                   = 3
    DVBH_ACTIVE                     = 4
    PROXIMITY_SENSOR_FAR            = 5
    PROXIMITY_SENSOR_NEAR           = 6
    GPS_INACTIVE                    = 7
    GPS_ACTIVE                      = 8
    ANTENNA_INACTIVE                = 9
    ANTENNA_ACTIVE                  = 10
    THERMAL_MANAGEMENT_INACTIVE     = 11
    THERMAL_MANAGEMENT_ACTIVE       = 12
    WLAN_HOTSPOT_INACTIVE           = 13
    WLAN_HOTSPOT_ACTIVE             = 14
    SPURIOUS_BATTERY_STATE_INACTIVE = 15
    SPURIOUS_BATTERY_STATE_ACTIVE   = 16
    LOW_BATTERY_STATE_INACTIVE      = 17
    LOW_BATTERY_STATE_ACTIVE        = 18


class AtServer(SocketServer.UnixStreamServer):

    socket_type = socket.SOCK_SEQPACKET
    allow_reuse_address = True

    def __init__(self):
        os.remove(AT_SOCKET_PATH)
        SocketServer.UnixStreamServer.__init__(self, AT_SOCKET_PATH, AtRequestHandler)
        self.req_queue = Queue.Queue()
        self.resp_queue = Queue.Queue()
        self.response = AT_OK


    def get_request_queue(self):
        return self.req_queue

    def get_response_queue(self):
        return self.resp_queue

    def wait_for_next_at_command(self):
        cmd = None
        while cmd is None:
            cmd = self.wait_for_next_at_command_with_timeout(10)
        return cmd

    def wait_for_next_at_command_with_timeout(self, timeout):
        cmd = None
        try:
            cmd = self.req_queue.get(True, timeout)
            if cmd.__class__ is socket.error:
                raise cmd

        except Empty:
            pass #Timeout
        except(KeyboardInterrupt, SystemExit):
            print "Got keyboard interrupt"
            sys.exit()
        return cmd

    def set_next_response(self, response):
        self.resp_queue.put(response)



class AtRequestHandler(SocketServer.BaseRequestHandler):

    def setup(self):
        self.req_queue = self.server.get_request_queue()
        self.resp_queue = self.server.get_response_queue()


    def handle(self):
        try:
            while True:
                self.read_request()
                self.enqueue_request()
                self.write_response()
        except socket.error, msg:
            self.req_queue.put(socket.error(msg))


    def read_request(self):
        self.data = self.request.recv(8 * 1024).strip()
        print "AT Socket Read: %s - Len: %d" % (self.data, len(self.data))
        if len(self.data) is 0:
            raise socket.error("Client Disconnected")

    def write_response(self):
        resp = AT_ERROR
        try:
            # Wait max five seconds for the main thread to give us a response
            resp = self.resp_queue.get(True, 5)
        except Empy:
            pass

        print "AT Socket Send: %s" % (resp.strip())
        self.request.send(resp)

    def enqueue_request(self):
        self.req_queue.put(self.data)





