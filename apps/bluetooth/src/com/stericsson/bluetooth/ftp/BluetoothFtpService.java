/*
 *
 * Copyright (c) 2008-2009, Motorola, Inc.
 * Copyright (C) ST-Ericsson SA 2010
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
 * - Neither the name of the Motorola, Inc. nor the names of its contributors
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

package com.stericsson.bluetooth.ftp;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.MediaScannerConnection;
import android.media.MediaScannerConnection.MediaScannerConnectionClient;
import android.net.Uri;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import java.io.IOException;

import javax.obex.ObexTransport;

/** Handles incoming connection and messages from other classes */
 public class BluetoothFtpService extends Service {
    private static final String TAG = "BluetoothFtpService";
    private static final boolean D = Constants.DEBUG;
    private static final boolean V = Constants.VERBOSE;

    private BluetoothAdapter mAdapter;

    private BluetoothFtpRfcommListener mSocketListener;

    private boolean mListenerStarted = false;

    private ObexTransport mPendingConnection = null;

    private BluetoothDevice mRemoteDevice = null;

    private BluetoothFtpObexServerSession mServerSession = null;

    static final int MSG_START_LISTENER = 5000;
    static final int MSG_SERVERSESSION_CLOSED = 5001;
    static final int MSG_USER_TIMEOUT = 5002;
    static final int MSG_INCOMING_BTFTP_CONNECTION = 5003;

    @Override
    public IBinder onBind(Intent arg0) {
        throw new UnsupportedOperationException("Cannot bind to Bluetooth FTP Service");
    }

    @Override
    public void onCreate() {
        super.onCreate();

        if (V) Log.v(TAG, "onCreate called");

        mAdapter = BluetoothAdapter.getDefaultAdapter();
        mSocketListener = new BluetoothFtpRfcommListener(mAdapter);

        registerReceiver(mBluetoothReceiver, new IntentFilter(Constants.ACTION_AUTHORIZE_ALLOWED));
        registerReceiver(mBluetoothReceiver, new IntentFilter(Constants.ACTION_AUTHORIZE_DISALLOWED));
        registerReceiver(mBluetoothReceiver, new IntentFilter(Constants.ACTION_SCAN_REQUEST));
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (V) Log.v(TAG, "onStartCommand called");

        int ret = super.onStartCommand(intent, flags, startId);
        if (ret == START_STICKY) {
            mHandler.sendMessageDelayed(mHandler.obtainMessage(MSG_START_LISTENER), 1000);
        }
        return ret;
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (V) Log.v(TAG, "Received message: " + msg.what);

            switch (msg.what) {
            case MSG_START_LISTENER:
                if (mAdapter != null && mAdapter.isEnabled() && !mListenerStarted) {
                    mListenerStarted = true;
                    mSocketListener.start(mHandler);
                }
                break;

            case MSG_SERVERSESSION_CLOSED:
                Log.i(TAG, "Session closed");
                mServerSession = null;
                break;

            case MSG_USER_TIMEOUT:
                sendBroadcast(new Intent(Constants.ACTION_AUTHORIZE_TIMEOUT));
                if (mPendingConnection != null) {
                    try {
                        mPendingConnection.close();
                    } catch (IOException e) {
                        Log.e(TAG, "Exception when closing transport: " + e);
                    }
                    mPendingConnection = null;
                }
                break;

            case MSG_INCOMING_BTFTP_CONNECTION:
                BluetoothSocket socket = (BluetoothSocket)msg.obj;
                BluetoothFtpRfcommTransport transport = new BluetoothFtpRfcommTransport(socket);

                // Reject connection immediately if another connection already active
                // or waiting for authorization.
                if (mServerSession != null || mPendingConnection != null) {
                    if (D) Log.d(TAG, "Incomming connection rejected, other connection in progress");

                    try {
                        transport.close();
                    } catch (IOException e) {
                        Log.e(TAG, "Exception when closing transport: " + e);
                    }
                } else {
                    mRemoteDevice = socket.getRemoteDevice();

                    // Auto-accept if device is trusted
                    if (mRemoteDevice.getTrustState()) {
                        if (D) Log.d(TAG, "Incomming connection from \"" + mRemoteDevice.getName() +
                                        "\" accepted automatically (trusted device)");

                        createServerSession(transport);
                    } else {
                        mPendingConnection = transport;

                        // Setup authorization dialog
                        Intent intent = new Intent();
                        intent.setClass(getApplicationContext(), BluetoothFtpActivity.class);
                        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                        intent.setAction(Constants.ACTION_AUTHORIZE_REQUEST);
                        intent.putExtra(Constants.EXTRA_REMOTE_NAME, mRemoteDevice.getName());
                        getApplicationContext().startActivity(intent);

                        Message lmsg = Message.obtain(mHandler);
                        lmsg.what = MSG_USER_TIMEOUT;
                        mHandler.sendMessageDelayed(lmsg, Constants.AUTHORIZE_TIMEOUT_VALUE);

                        if (D) Log.d(TAG, "Incomming connection from \"" + mRemoteDevice.getName() +
                                        "\" pending authorization");
                    }
                }
                break;
            }
        }
    };

    @Override
    public void onDestroy() {
        if (V) Log.v(TAG, "onDestroy called");

        super.onDestroy();
        unregisterReceiver(mBluetoothReceiver);
        mSocketListener.stop();
    }

    private void createServerSession(ObexTransport transport) {
        mServerSession = new BluetoothFtpObexServerSession(mHandler, this, transport);
        mServerSession.start();

        if (V) Log.v(TAG, "Created new session for connection");
    }

    private final BroadcastReceiver mBluetoothReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (V) Log.v(TAG, "Received intent: " + action);

            if (action.equals(Constants.ACTION_AUTHORIZE_ALLOWED)) {
                boolean alwaysAllowed = intent.getBooleanExtra(Constants.EXTRA_ALWAYS_ALLOWED, false);

                if (D) Log.d(TAG, "Authorization allowed (alwaysAllowed=" + alwaysAllowed + ")");

                mHandler.removeMessages(MSG_USER_TIMEOUT);

                if (mPendingConnection != null) {
                    if (alwaysAllowed && mRemoteDevice != null) {
                        mRemoteDevice.setTrust(true);
                    }
                    createServerSession(mPendingConnection);
                    mPendingConnection = null;
                }
            } else if (action.equals(Constants.ACTION_AUTHORIZE_DISALLOWED)) {
                if (D) Log.d(TAG, "Authorization disallowed");

                mHandler.removeMessages(MSG_USER_TIMEOUT);
                if (mPendingConnection != null) {
                    try {
                        mPendingConnection.close();
                    } catch (IOException e) {
                        Log.e(TAG, "Exception when closing transport: " + e);
                    }
                    mPendingConnection = null;
                }
            } else if (action.equals(Constants.ACTION_SCAN_REQUEST)) {
                int scanType = intent.getIntExtra(Constants.EXTRA_SCANTYPE, Constants.SCAN_TYPE_NEW);
                String scanPath = intent.getStringExtra(Constants.EXTRA_SCANPATH);
                String mimeType = intent.getStringExtra(Constants.EXTRA_MIMETYPE);

                if (V) Log.v(TAG, "Requested to scan file " + scanPath + "(" +
                                        "scanType=" + scanType + ", " +
                                        "mimeType=" + mimeType + ")");

                if (scanPath != null && mimeType != null) {
                    new MediaScannerNotifier(context, scanType, scanPath, mimeType);
                }
            }
        }
    };

    private static class MediaScannerNotifier implements MediaScannerConnectionClient {
        private MediaScannerConnection mConnection;
        private Context mContext;

        private int mScanType;
        private String mScanPath;
        private String mMimeType;

        public MediaScannerNotifier(Context context, int scanType, String scanPath, String mimeType) {
            mContext = context;
            mScanType = scanType;
            mScanPath = scanPath;
            mMimeType = mimeType;

            mConnection = new MediaScannerConnection(mContext, this);
            mConnection.connect();
        }

        public void onMediaScannerConnected() {
            mConnection.scanFile(mScanPath, mMimeType);
        }

        public void onScanCompleted(String path, Uri uri) {
            if (V) Log.v(TAG, "Scan complated for " + path + " (uri=" + uri + ")");

            if (mScanType == Constants.SCAN_TYPE_DELETED && uri != null) {
                mContext.getContentResolver().delete(uri,  null, null);
            }

            mConnection.disconnect();
        }
    }
}
