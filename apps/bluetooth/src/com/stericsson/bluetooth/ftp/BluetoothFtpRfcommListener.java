/*
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

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.IOException;

/** Handles RFCOMM listener for incoming connections */
public class BluetoothFtpRfcommListener {
    private static final String TAG = "BluetoothFtpRfcommListener";
    private static final boolean D = Constants.DEBUG;
    private static final boolean V = Constants.VERBOSE;

    private static final int CREATE_RETRY_TIME = 10;
    private static final int DEFAULT_FTP_CHANNEL = 21;

    private final BluetoothAdapter mAdapter;

    private final int mBtFtpRfcommChannel;

    private BluetoothServerSocket mBtServerSocket = null;

    private Thread mSocketAcceptThread;

    private Handler mCallback;

    private volatile boolean mInterrupted;

    public BluetoothFtpRfcommListener(BluetoothAdapter adapter) {
        this(adapter, DEFAULT_FTP_CHANNEL);
    }

    public BluetoothFtpRfcommListener(BluetoothAdapter adapter, int channel) {
        mAdapter = adapter;
        mBtFtpRfcommChannel = channel;
    }

    public synchronized boolean start(Handler callback) {
        if (mSocketAcceptThread == null) {
            mCallback = callback;

            mSocketAcceptThread = new Thread(TAG) {
                public void run() {
                    boolean listenOk = false;

                    for (int i = 0; i < CREATE_RETRY_TIME && !mInterrupted && !listenOk; i++) {
                        try {
                            mBtServerSocket = mAdapter.listenUsingEncryptedRfcommOn(mBtFtpRfcommChannel);
                            listenOk = true;
                        } catch (IOException e) {
                            Log.e(TAG, "Exception when trying to listen on RFCOMM: " + e);
                        }

                        if (!listenOk) {
                            synchronized (this) {
                                try {
                                    if (V) Log.v(TAG, "Wait 3 seconds before retry");
                                    Thread.sleep(3000);
                                } catch (InterruptedException e) {
                                    Log.e(TAG, "Interrupted when waiting to retry listening");
                                    mInterrupted = true;
                                }
                            }
                        }
                    }

                    if (!listenOk) {
                        Log.e(TAG, "Cannot start listening after " + CREATE_RETRY_TIME + " attempts");
                    } else {
                        Log.i(TAG, "Accept thread started on channel " + mBtFtpRfcommChannel);

                        while (!mInterrupted) {
                            try {
                                BluetoothSocket clientSocket = mBtServerSocket.accept();
                                Log.i(TAG, "Accepted connection from "
                                        + clientSocket.getRemoteDevice());

                                Message msg = Message.obtain();
                                msg.setTarget(mCallback);
                                msg.what = BluetoothFtpService.MSG_INCOMING_BTFTP_CONNECTION;
                                msg.obj = clientSocket;
                                msg.sendToTarget();
                            } catch (IOException e) {
                                Log.e(TAG, "Exception when accepting connection: " + e);
                            }
                        }
                    }

                    Log.i(TAG, "Accept thread finished");
                }
            };

            mInterrupted = false;
            mSocketAcceptThread.start();
        }
        return true;
    }

    public synchronized void stop() {
        if (mSocketAcceptThread != null) {
            mInterrupted = true;

            if (mBtServerSocket != null) {
                try {
                    mBtServerSocket.close();
                } catch (IOException e) {
                    Log.e(TAG, "Exception when closing socket: " + e);
                }
            }

            try {
                mSocketAcceptThread.interrupt();
                if (V) Log.v(TAG, "Waiting for thread to terminate...");
                mSocketAcceptThread.join();
                mSocketAcceptThread = null;
                mCallback = null;
            } catch (InterruptedException e) {
                Log.e(TAG, "Interrupted when waiting for thread to terminate");
            }
        }
    }
}
