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

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import java.io.IOException;
import java.util.concurrent.Semaphore;
import javax.obex.ObexTransport;

public class BluetoothMapService extends Service {

    public BluetoothMapService() {
    }
    /* Constants pertaining to email */
    private static final boolean USE_EMAIL = true;
    private static final int EMAIL_RFCOMM_CHANNEL = 25;

    /* Variables pertaining to email */
    private BluetoothMapRfcommListener mEmailSocketListener;
    private static BluetoothDevice mEmailRemoteDevice = null;
    private static BluetoothSocket mEmailConnSocket = null;
    private BluetoothMapObexServerSession mEmailServerSession;

    /* Variables pertaining to Sms session */
    private BluetoothMapRfcommListener mSmsSocketListener;
    private static BluetoothDevice mSmsRemoteDevice = null;
    private static BluetoothSocket mSmsConnSocket = null;
    private BluetoothMapObexServerSession mSmsServerSession;

    /* Constants pertaining to sms */
    private static final int SMS_RFCOMM_CHANNEL = 23;

    private static final String TAG = "BtMapServer";
    private static final boolean D = Constants.DEBUG;
    private static final boolean V = Constants.VERBOSE;

    public static final int MSG_OBEX_SESSION_DISCONNECTED = 2001;
    public static final int MSG_OBEX_SESSION_CONNECTED = 2002;
    public static final int MSG_OBEX_TRANSPORT_CLOSED = 2003;

    /* States of Bluetooth MAP */
    public static final int MAP_STATE_EMAIL_TRANSPORT_CONNECTED = 4001;
    public static final int MAP_STATE_SMS_TRANSPORT_CONNECTED = 4002;
    public static final int MAP_STATE_NONE = 4003;
    public static final int MAP_STATE_EMAIL_OBEX_CONNECTED = 4004;
    public static final int MAP_STATE_SMS_OBEX_CONNECTED = 4005;
    public static final int MAP_STATE_BOTH_TRANSPORT_CONNECTED = 4006;
    public static final int MAP_STATE_BOTH_OBEX_CONNECTED = 4007;

    private static int mMapState;

    private BluetoothAdapter mAdapter;

    private boolean mListenStarted = false;

    public static Semaphore semEvent;

    private boolean smsRequestRejected;

    /* Single Message Notification Channel */
    public static BluetoothMapMessageNotificationClient mMessageNotifier = null;

    @Override
    public IBinder onBind(Intent arg0) {
        throw new UnsupportedOperationException("Cannot bind to Bluetooth MAP Service");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mAdapter = BluetoothAdapter.getDefaultAdapter();
        mSmsSocketListener = new BluetoothMapRfcommListener(mAdapter, SMS_RFCOMM_CHANNEL);

        if (USE_EMAIL) {
            mEmailSocketListener = new BluetoothMapRfcommListener(mAdapter, EMAIL_RFCOMM_CHANNEL);
        }

        mMessageNotifier = new BluetoothMapMessageNotificationClient(this);

        synchronized (BluetoothMapService.this) {
            if (mAdapter != null) {
                startListenerDelayed();
            } else {
                Log.d(TAG, "Local BT device is not enabled");
            }
        }

        registerReceiver(mBluetoothReceiver, new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED));
        registerReceiver(mBluetoothReceiver, new IntentFilter(Constants.AUTHORIZE_ALLOWED_ACTION));
        registerReceiver(mBluetoothReceiver, new IntentFilter(Constants.AUTHORIZE_DISALLOWED_ACTION));
        semEvent = new Semaphore(0);
        smsRequestRejected = true;
        mMapState = MAP_STATE_NONE;
    }

    private static final int START_LISTENER = 1;

    private Handler mSmsHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (V) Log.v(TAG, "mSmsHandler -> Message received is " + msg.what);
            switch (msg.what) {
            case START_LISTENER:
                /**
                 * Start socket listener corresponding to Sms
                 * socket
                 */
                mSmsSocketListener.start(mSmsHandler);
                mMapState = MAP_STATE_NONE;
                break;
            case Constants.MSG_USER_TIMEOUT:
                Intent authIntent = new Intent(Constants.USER_CONFIRM_TIMEOUT_ACTION);
                sendBroadcast(authIntent);
                cleanupSmsConnSession();
                smsRequestRejected = true;
                break;
            case Constants.MSG_AUTHORIZE_INCOMING_MAP_CONNECTION:
                synchronized(this) {
                    mSmsConnSocket = (BluetoothSocket)msg.obj;
                    Log.i(TAG, "mSmsHandler evt :  MSG_AUTHORIZE_INCOMING_MAP_CONNECTION mMapState " + mMapState);
                    if (mSmsConnSocket != null) {
                        /**
                         * Check if sms transport/obex or both
                         * the transports (email and obex)are
                         * already connected
                         */
                        if ((mMapState == MAP_STATE_SMS_TRANSPORT_CONNECTED) ||
                                (mMapState == MAP_STATE_SMS_OBEX_CONNECTED) ||
                                (mMapState == MAP_STATE_BOTH_TRANSPORT_CONNECTED) ||
                                (mMapState == MAP_STATE_BOTH_OBEX_CONNECTED)) {
                            try {
                                mSmsConnSocket.close();
                            } catch (IOException e) {
                                Log.e(TAG, "IOException occured while closing Sms socket " + e);
                            }
                            smsRequestRejected = true;
                            break;
                        }

                        /**
                         * check if email has already been connected or not, if yes
                         * then check if it has been connected with the same device which is
                         * trying to connect the sms transport
                         */
                        if (mMapState == MAP_STATE_EMAIL_TRANSPORT_CONNECTED) {
                            if (mEmailRemoteDevice != mSmsConnSocket.getRemoteDevice()) {
                                try {
                                    mSmsConnSocket.close();
                                } catch (IOException e) {
                                    Log.e(TAG, "IOException occured while closing Sms socket " + e);
                                }
                                smsRequestRejected = true;
                                break;
                            }
                        }

                        mSmsRemoteDevice = mSmsConnSocket.getRemoteDevice();

                        /**
                         * Upon accepting this transport update the
                         * current status either to MAP_STATE_EMAIL_TRANSPORT_CONNECTED
                         * or MAP_STATE_BOTH_TRANSPORT_CONNECTED
                         */
                        if (mMapState == MAP_STATE_EMAIL_TRANSPORT_CONNECTED) {
                            mMapState = MAP_STATE_BOTH_TRANSPORT_CONNECTED;
                        } else if (mMapState == MAP_STATE_NONE){
                            mMapState = MAP_STATE_SMS_TRANSPORT_CONNECTED;
                        }
                    }
                    if (D) Log.d(TAG, "Authorize incoming connection...");
                    if (mSmsRemoteDevice != null) {

                        /**
                         * mRemoteDevice.getTrustState() is always returning true and
                         * not present in the current documentation for BluetoothDevice class
                         * hence probably deprecated or unpredictable.
                         */
                        boolean trust = mSmsRemoteDevice.getTrustState();
                        if (trust && mSmsConnSocket != null) {
                            if (V) Log.v(TAG, "incomming connection accepted from: "
                                    + getRemoteDeviceName() + " automatically as trusted device");
                            setupConnMessage(mSmsConnSocket);
                            smsRequestRejected = false;
                        } else {
                            startMapActivity(Constants.AUTHORIZE_REQUEST_ACTION);

                            Message lmsg = Message.obtain(mSmsHandler);
                            lmsg.what = Constants.MSG_USER_TIMEOUT;
                            mSmsHandler.sendMessageDelayed(lmsg, Constants.USER_CONFIRM_TIMEOUT_VALUE_IN_MS);
                            smsRequestRejected = true;
                        }
                    }
                }
                break;
            case Constants.MSG_INCOMING_BTMAP_CONNECTION:
                if (V) Log.v(TAG, "listened one incoming connection");
                BluetoothMapRfcommTransport transport = (BluetoothMapRfcommTransport) msg.obj;

                /**
                 * Create strategy to take care of incoming connections
                 * when MAP server is already connected to MCE for Sms Instance
                 */
                createSmsServerSession(transport);
                break;

            case MSG_OBEX_SESSION_DISCONNECTED:
                /**
                 * Obex session corresponding to Sms Instance
                 * has been disconnected, update the current
                 * state
                 */
                synchronized(this) {
                    if (mMapState == MAP_STATE_SMS_OBEX_CONNECTED) {
                        mMapState = MAP_STATE_NONE;
                    } else if (mMapState == MAP_STATE_BOTH_OBEX_CONNECTED) {
                        mMapState = MAP_STATE_EMAIL_OBEX_CONNECTED;
                    }
                    Log.i(TAG, "mSmsHandler evt :  MSG_OBEX_SESSION_DISCONNECTED mMapState " + mMapState);
                }
                break;

            case MSG_OBEX_SESSION_CONNECTED:
                /**
                 * Obex session corresponding to sms Instance
                 * has been connected, update the current
                 * state
                 */
                synchronized(this) {
                    if (mMapState == MAP_STATE_EMAIL_OBEX_CONNECTED) {
                        mMapState = MAP_STATE_BOTH_OBEX_CONNECTED;
                    } else {
                        mMapState = MAP_STATE_SMS_OBEX_CONNECTED;
                    }
                    Log.i(TAG, "mSmsHandler evt :  MSG_OBEX_SESSION_CONNECTED mMapState " + mMapState);
                }
                break;

            case MSG_OBEX_TRANSPORT_CLOSED:
                /**
                 * Transport session corresponding to
                 * sms Instance has been disconnected,
                 * update the current state
                 */
                synchronized(this) {
                    if (mMapState == MAP_STATE_BOTH_OBEX_CONNECTED) {
                        mMapState = MAP_STATE_EMAIL_OBEX_CONNECTED;
                    }
                    if (mMapState == MAP_STATE_BOTH_TRANSPORT_CONNECTED) {
                        mMapState = MAP_STATE_EMAIL_TRANSPORT_CONNECTED;
                    } else {
                        mMapState = MAP_STATE_NONE;
                    }
                }
                break;
            }
        }
    };

    private Handler mEmailHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (V) Log.v(TAG, "mEmailHandler -> Message received is " + msg.what);
            switch (msg.what) {
            case START_LISTENER:
                if (USE_EMAIL) {
                    mEmailSocketListener.start(mEmailHandler);
                    mMapState = MAP_STATE_NONE;
                }
                break;
            case Constants.MSG_USER_TIMEOUT:
                Intent authIntent = new Intent(Constants.USER_CONFIRM_TIMEOUT_ACTION);
                sendBroadcast(authIntent);
                cleanupEmailConnSession();
                break;
            case Constants.MSG_AUTHORIZE_INCOMING_MAP_CONNECTION:
                synchronized(this) {
                    mEmailConnSocket = (BluetoothSocket)msg.obj;
                    Log.i(TAG, "mEmailHandler evt :  MSG_AUTHORIZE_INCOMING_MAP_CONNECTION mMapState " + mMapState);
                    if (mEmailConnSocket != null) {
                        /**
                         * Check if email transport/obex or both
                         * the transports are already connected
                         */
                        if ((mMapState == MAP_STATE_EMAIL_TRANSPORT_CONNECTED) ||
                                (mMapState == MAP_STATE_EMAIL_OBEX_CONNECTED) ||
                                (mMapState == MAP_STATE_BOTH_TRANSPORT_CONNECTED)||
                                (mMapState == MAP_STATE_BOTH_OBEX_CONNECTED)) {
                            try {
                                mEmailConnSocket.close();
                            } catch (IOException e) {
                                Log.e(TAG, "IOException occured while closing Email socket " + e);
                            }
                            break;
                        }

                        /**
                         * check if sms has already been connected or not, if yes
                         * then check if it has been connected with the same device which is
                         * trying to connect the email transport
                         */
                        if (mMapState == MAP_STATE_SMS_TRANSPORT_CONNECTED) {
                            if (mSmsRemoteDevice != mEmailConnSocket.getRemoteDevice()) {
                                try {
                                    mEmailConnSocket.close();
                                } catch (IOException e) {
                                    Log.e(TAG, "IOException occured while closing Email socket " + e);
                                }
                                break;
                            }
                        }

                        mEmailRemoteDevice = mEmailConnSocket.getRemoteDevice();

                        /**
                         * upon accepting this transport update the
                         * current status either to MAP_STATE_EMAIL_TRANSPORT_CONNECTED
                         * or MAP_STATE_BOTH_TRANSPORT_CONNECTED
                         */
                        if (mMapState == MAP_STATE_SMS_TRANSPORT_CONNECTED) {
                            mMapState = MAP_STATE_BOTH_TRANSPORT_CONNECTED;
                        } else if (mMapState == MAP_STATE_NONE) {
                            mMapState = MAP_STATE_EMAIL_TRANSPORT_CONNECTED;
                        }
                    }
                    if (D) Log.d(TAG, "Authorize incoming connection...");
                    if (mEmailRemoteDevice != null) {
                        boolean trust = true;

                        /* Sms request has been rejected recently, so reject email as well */
                        if (smsRequestRejected == true) {
                            trust = false;
                            cleanupEmailConnSession();
                        } else if (trust && mEmailConnSocket != null) {
                        if (V) Log.v(TAG, "incomming connection accepted from: "
                                    + getRemoteDeviceName() + " automatically as trusted device");
                            setupEmailConnMessage(mEmailConnSocket);
                        } else {
                            setupEmailConnMessage(mEmailConnSocket);
                        }
                    }
                }
                break;
            case Constants.MSG_INCOMING_BTMAP_CONNECTION:
                if (V) Log.v(TAG, "listened one incoming connection");
                BluetoothMapRfcommTransport transport = (BluetoothMapRfcommTransport) msg.obj;

                /* Create strategy to take care of incoming connections when MAP server is already connected to MCE for EMAIL instance */
                createEmailServerSession(transport);
                break;

            case MSG_OBEX_SESSION_DISCONNECTED:
                /**
                 * Obex session corresponding to email Instance
                 * has been disconnected, update the current
                 * state
                 */
                synchronized(this) {
                    if (mMapState == MAP_STATE_EMAIL_OBEX_CONNECTED) {
                        mMapState = MAP_STATE_NONE;
                    } else if (mMapState == MAP_STATE_BOTH_OBEX_CONNECTED) {
                        mMapState = MAP_STATE_SMS_OBEX_CONNECTED;
                    }
                    Log.i(TAG, "evt :  MSG_OBEX_SESSION_CONNECTED mMapState " + mMapState);
                }
                break;

            case MSG_OBEX_SESSION_CONNECTED:
                /**
                 * Obex session corresponding to email Instance
                 * has been connected, update the current
                 * state
                 */
                synchronized(this) {
                    Log.i(TAG, "BluetoothMapService.MSG_OBEX_SESSION_CONNECTED recvd ");
                    if (mMapState == MAP_STATE_SMS_OBEX_CONNECTED) {
                        mMapState = MAP_STATE_BOTH_OBEX_CONNECTED;
                    } else {
                        mMapState = MAP_STATE_EMAIL_OBEX_CONNECTED;
                    }
                    Log.i(TAG, "BluetoothMapService.MSG_OBEX_SESSION_CONNECTED recvd ");
                    Log.i(TAG, "evt :  MSG_OBEX_SESSION_CONNECTED mMapState " + mMapState);
                }
                break;

            case MSG_OBEX_TRANSPORT_CLOSED:
                /**
                 * Transport session corresponding to
                 * email Instance has been disconnected,
                 * update the current state
                 */
                synchronized(this) {
                    if (mMapState == MAP_STATE_BOTH_OBEX_CONNECTED) {
                        mMapState = MAP_STATE_SMS_OBEX_CONNECTED;
                    }
                    if (mMapState == MAP_STATE_BOTH_TRANSPORT_CONNECTED) {
                        mMapState = MAP_STATE_SMS_TRANSPORT_CONNECTED;
                    } else {
                        mMapState = MAP_STATE_NONE;
                    }
                }
                break;
            }
        }
    };

    public int onStartCommand(Intent intent, int flags, int startId) {
        int retCode = super.onStartCommand(intent, flags, startId);
        if (retCode == START_STICKY) {
            if (mAdapter == null) {
                Log.w(TAG, "Local BT device is not enabled");
            }
        }
        return retCode;
    }

    private void startListenerDelayed() {
        if (!mListenStarted) {
            if (V) Log.v(TAG, "Starting RfcommListener in 9 seconds");
            mSmsHandler.sendMessageDelayed(mSmsHandler.obtainMessage(START_LISTENER), 9000);
            mListenStarted = true;

            if (USE_EMAIL) {
                mEmailHandler.sendMessageDelayed(mEmailHandler.obtainMessage(START_LISTENER), 9000);
            }
        }
    }

    /* Create and initialize ObexServerSession object for Sms Instance */
    private void createSmsServerSession(ObexTransport transport) {
        mSmsServerSession = new BluetoothMapObexServerSession(this,
                                    transport,
                                    Constants.SMS_MASINSTANCE,
                                    mSmsHandler);
        mSmsServerSession.preStart();
        if (D) Log.d(TAG, "Get ServerSession " + mSmsServerSession.toString()
                + " for incoming connection" + transport.toString());
    }

    /* Create and initialize ObexServerSession object for Email Instance */
    private void createEmailServerSession(ObexTransport transport) {
        mEmailServerSession = new BluetoothMapObexServerSession(this,
                                    transport,
                                    Constants.EMAIL_MASINSTANCE,
                                    mEmailHandler);
        mEmailServerSession.preStart();
        if (D) Log.d(TAG, "Get ServerSession " + mEmailServerSession.toString()
                + " for incoming connection" + transport.toString());
    }

    private final BroadcastReceiver mBluetoothReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            boolean bRemoveTimeoutMsg = true;

            if (V) Log.v(TAG, "Intent received is " + action);

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                switch (intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR)) {
                case BluetoothAdapter.STATE_ON:
                    if (V) Log.v(TAG, "Bluetooth switched ON");
                    startSocketListener();
                    break;
                case BluetoothAdapter.STATE_TURNING_OFF:
                    if (V) Log.v(TAG, "Bluetooth switched OFF");
                    mSmsSocketListener.stop();

                    /* Also stop email socketListener */
                    if (USE_EMAIL) {
                        mEmailSocketListener.stop();
                    }

                    /* Reset this state as well */
                    mMapState = MAP_STATE_NONE;

                    synchronized(BluetoothMapService.this) {
                        stopSelf();
                    }
                    break;
                }
            } else if (action.equals(Constants.AUTHORIZE_ALLOWED_ACTION)) {
                boolean bAlwaysAllowed = intent.getBooleanExtra(Constants.EXTRA_AUTHORIZE_ALWAYS_ALLOWED, false);
                if (D) Log.d(TAG, "AlwaysAllowed = " + bAlwaysAllowed);

                if (mSmsRemoteDevice != null && mSmsConnSocket != null) {
                    if (bAlwaysAllowed) {
                        mSmsRemoteDevice.setTrust(true);
                    }
                    setupConnMessage(mSmsConnSocket);
                    smsRequestRejected = false;
                }
            } else if (action.equals(Constants.AUTHORIZE_DISALLOWED_ACTION)) {
                cleanupSmsConnSession();
                smsRequestRejected = true;
            } else {
                bRemoveTimeoutMsg = false;
            }
            if (bRemoveTimeoutMsg) {
                mSmsHandler.removeMessages(Constants.MSG_USER_TIMEOUT);
            }
        }
    };

    private void startMapActivity(String action) {
        if (mSmsRemoteDevice != null) {
            Intent authIntent = new Intent();
            authIntent.setClass(getApplicationContext(), BluetoothMapActivity.class);
            authIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            authIntent.setAction(action);
            getApplicationContext().startActivity(authIntent);
        } else {
            Log.i(TAG, "mRemoteDevice found null : activity cannot be started");
        }
    }

    private void startSocketListener() {
        mSmsSocketListener.start(mSmsHandler);

        if (USE_EMAIL) {
            mEmailSocketListener.start(mEmailHandler);
        }
    }

    public static String getRemoteDeviceName() {
        if (mSmsRemoteDevice != null) {
            return mSmsRemoteDevice.getName();
        } else {
            return null;
        }
    }

    private void setupConnMessage(BluetoothSocket sBTSocket) {
        BluetoothMapRfcommTransport transport = new BluetoothMapRfcommTransport(sBTSocket);
        Message lmsg = Message.obtain(mSmsHandler);
        lmsg.what = Constants.MSG_INCOMING_BTMAP_CONNECTION;
        lmsg.obj = transport;
        mSmsHandler.sendMessage(lmsg);
        if (D) Log.d(TAG, "MSG_INCOMING_BTMAP_CONNECTION sent.");
    }

    private void setupEmailConnMessage(BluetoothSocket sBTSocket) {
        BluetoothMapRfcommTransport transport = new BluetoothMapRfcommTransport(sBTSocket);
        Message lmsg = Message.obtain(mEmailHandler);
        lmsg.what = Constants.MSG_INCOMING_BTMAP_CONNECTION;
        lmsg.obj = transport;
        mEmailHandler.sendMessage(lmsg);
        if (D) Log.d(TAG, "MSG_INCOMING_BTMAP_CONNECTION sent.");
    }

    private synchronized static void cleanupSmsConnSession() {
        try {
            if (mSmsConnSocket != null) mSmsConnSocket.close();
        } catch (IOException ex) {
            Log.e(TAG, "CloseSocket error: " + ex);
        }
        mSmsRemoteDevice = null;
        mSmsConnSocket = null;

        /* Cleanup of sms is asked,
         * change the current state */
        if (mMapState == MAP_STATE_BOTH_TRANSPORT_CONNECTED) {
            mMapState = MAP_STATE_EMAIL_TRANSPORT_CONNECTED;
        }
        if (mMapState == MAP_STATE_BOTH_OBEX_CONNECTED) {
            mMapState = MAP_STATE_EMAIL_OBEX_CONNECTED;
        } else {
            mMapState = MAP_STATE_NONE;
        }
    }

    private synchronized static void cleanupEmailConnSession() {
        try {
            if (mEmailConnSocket != null) mEmailConnSocket.close();
        } catch (IOException ex) {
            Log.e(TAG, "CloseSocket error: " + ex);
        }
        mEmailRemoteDevice = null;
        mEmailConnSocket = null;

        /**
         * Cleanup of email is asked,
         * change the current state
         */
        if (mMapState == MAP_STATE_BOTH_TRANSPORT_CONNECTED) {
            mMapState = MAP_STATE_SMS_TRANSPORT_CONNECTED;
        }
        if (mMapState == MAP_STATE_BOTH_OBEX_CONNECTED) {
            mMapState = MAP_STATE_SMS_OBEX_CONNECTED;
        } else {
            mMapState = MAP_STATE_NONE;
        }
    }

    /* This method returns connection sockets for the specefied type of MAS Instance */
    public BluetoothSocket GetServerSocket(int currentMasInstance) {
        if (currentMasInstance == Constants.EMAIL_MASINSTANCE) {
            return mEmailConnSocket;
        } else {
            return mSmsConnSocket;
        }
    }

    @Override
    public void onDestroy() {
        if (D) Log.d(TAG, "Service onDestroy");
        super.onDestroy();
        unregisterReceiver(mBluetoothReceiver);
        try {
            mSmsSocketListener.stop();
        } catch (Exception e) {
            Log.e(TAG,e.toString());
        }

        /* Stop email socket listener as well */
        try {
            if (USE_EMAIL) {
                mEmailSocketListener.stop();
            }
        } catch (Exception e) {
            Log.e(TAG,e.toString());
        }

        /* Deinitialize the notification client as well */
        try {
            mMessageNotifier.stop();
        } catch (Exception e) {
            Log.e(TAG,e.toString());
        }

        /* Close the ObexServerSessions as well */
        try {
            mSmsServerSession.stop();
        } catch (Exception e) {
            Log.e(TAG,e.toString());
        }
        try {
            mEmailServerSession.stop();
        } catch (Exception e) {
            Log.e(TAG,e.toString());
        }
    }
}
