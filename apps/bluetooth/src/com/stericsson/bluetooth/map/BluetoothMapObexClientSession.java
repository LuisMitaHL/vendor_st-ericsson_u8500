/*
 * Copyright (C) 2010, ST-Ericsson SA
 * Author: Pradeep Kumar <pradeep-mmc.kumar@stericsson.com> for ST-Ericsson
 * Licence terms: 3-clause BSD
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of the ST-Ericsson SA nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
package com.stericsson.bluetooth.map;

import android.content.Context;
import android.os.Handler;
import android.util.Log;
import java.io.IOException;
import java.io.OutputStream;
import javax.obex.ApplicationParameter;
import javax.obex.ClientOperation;
import javax.obex.ClientSession;
import javax.obex.HeaderSet;
import javax.obex.ObexTransport;
import javax.obex.ResponseCodes;

public class BluetoothMapObexClientSession {

    private static final String TAG = "BluetoothMapObexClientSession";
    private static final boolean D = Constants.DEBUG;
    private static final boolean V = Constants.VERBOSE;

    private static final byte[] MNS_TARGET_UUID = {(byte)0xbb,
        (byte)0x58,
        (byte)0x2b,
        (byte)0x41,
        (byte)0x42,
        (byte)0x0c,
        (byte)0x11,
        (byte)0xdb,
        (byte)0xb0,
        (byte)0xde,
        (byte)0x08,
        (byte)0x00,
        (byte)0x20,
        (byte)0x0c,
        (byte)0x9a,
        (byte)0x66};

    private ObexTransport mTransport;

    private ClientSession mCs;

    private HeaderSet mLastConnectHeader;

    public BluetoothMapObexClientSession(Context context, ObexTransport transport) {
        if (transport == null) {
            Log.e("BluetoothMapObexClientSession", "transport is null");
            throw new NullPointerException("transport is null");
        }
        mTransport = transport;
        mLastConnectHeader = null;
    }

    public void start(Handler sessionHandler) {

    }

    public void stop() {

    }

    public void connect() {
        try {
            Log.i(TAG, "creating new ClientSession");
            mCs = new ClientSession(mTransport);
        } catch (IOException e1) {
            Log.e(TAG, "OBEX session create error");
        }

        HeaderSet hs = new HeaderSet();
        try {
            Log.i(TAG, "Setting header");
            hs.setHeader(HeaderSet.TARGET, MNS_TARGET_UUID);
            mLastConnectHeader = mCs.connect(hs);

            final int response = hs.getResponseCode();
            if (ResponseCodes.OBEX_HTTP_OK == response) {
                Log.i(TAG, "Connection ID obtained " + hs.mConnectionID[0]+
                            " " + hs.mConnectionID[1] +
                            " " + hs.mConnectionID[2] +
                            " " + hs.mConnectionID[3]);
            } else {
                Log.d(TAG, "OBEX session failed");
            }

        } catch (IOException e) {
            Log.e(TAG, "OBEX session connect error " + e);
        }
    }

    public void disconnect() {
        try {
            if (mCs != null) {
                Log.i(TAG, "Disconnecting the client session: calling mCs.disconnect");
                mCs.disconnect(mLastConnectHeader);
            }
            mCs = null;
            mLastConnectHeader = null;
            Log.i(TAG, "OBEX session disconnected");
        } catch (IOException e) {
            Log.e(TAG, "OBEX session disconnect error" + e);
        }
        try {
            if (mCs != null) {
                mCs.close();
                Log.i(TAG, "OBEX session closed");
            }
        } catch (IOException e) {
            Log.e(TAG, "OBEX session close error" + e);
        }
        if (mTransport != null) {
            try {
                mTransport.close();
            } catch (IOException e) {
                Log.e(TAG, "mTransport.close error");
            }

        }
    }

    /**
     * This method sends the MAP event report object to
     * the map notification server.
     *
     * @param MasInstanceID
     * @param object
     * @return
     */
    public boolean sendData(int MasInstanceID, String object) {
        HeaderSet request;
        boolean error = false;
        request = new HeaderSet();
        ApplicationParameter ap = new ApplicationParameter();
        byte [] appParam;
        /* Synchronised this prevent multiple access occuring for receipt of multiple Message Notifications simultaneously  */
        synchronized(this) {
            /* Create the request header */
            request.setHeader(HeaderSet.TYPE, Constants.OBEX_HEADER_TYPE_SEND_EVENT);
            appParam = new byte[1];
            appParam[0] = (byte) (MasInstanceID & 0xff);
            ap.addAPPHeader(Constants.APPLI_PARAM_ID_MASINSTANCE_ID, (byte)1, appParam);
            request.setHeader(HeaderSet.APPLICATION_PARAMETER, ap.getAPPparam());

            request.mConnectionID = new byte[4];
            try {
                request.setHeader(HeaderSet.CONNECTION_ID, mLastConnectHeader.mConnectionID);
            } catch (Exception e) {
                Log.e(TAG, "Exception occurred" + e);
            }

            ClientOperation putOperation = null;
            OutputStream outputStream = null;

            /* Create put request */
            try {
                putOperation = (ClientOperation)mCs.put(request);
            } catch (IOException e) {
                Log.e(TAG, "IOException occurred" + e);
                error = true;
            }

            /* Open Output streams */
            if (!error) {
                try {
                    if (V) Log.v(TAG, "openOutputStream ");
                    outputStream = putOperation.openDataOutputStream();
                } catch (IOException e) {
                    Log.e(TAG, "Error when openOutputStream");
                    error = true;
                }
            }

            if (error != true) {
                /* Send the body header */
                try {
                    byte [] buffer = object.getBytes();
                    outputStream.write(buffer, 0, buffer.length);
                } catch (IOException e) {
                    Log.e(TAG, "IOException " + e);
                    error = true;
                }
            }

            /* Close the output stream */
            try {
                outputStream.close();
            } catch (IOException e) {
                Log.e(TAG, "IOException " + e);
                error = true;
            } catch (Exception e) {
                Log.e(TAG, "Exception " + e.toString());
                error = true;
            }

            if (putOperation != null) {
                try {
                    putOperation.close();
                } catch (IOException e) {
                    Log.e(TAG, "IOException" + e);
                    error = true;
                }
            }

            return (!error);
        }
    }

};
