/*
 * Copyright (C) ST-Ericsson SA 2011
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.stericsson.ril.oem;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Messenger;
import android.os.Parcel;
import android.os.RemoteException;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import java.util.concurrent.TimeoutException;

/**
 * Wrapper for the STrilOemService interface. Hides service binding and call
 * complexity.
 */
public class STErilOemHook {

    // Redefined here for backwards compatibility
    public static final String ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW
                    = STErilOemHookConstants.ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW;

    // Connection callback interface
    public interface ConnectionCallback {

        void onConnected();

        void onDisconnected();

        void onResult(int requestId, byte[] data);

    };

    /* private variables */
    private static final String TAG = "STErilOemHook";

    private ServiceHandler mServiceHandler = null;
    private Context mContext = null;

    // Timeout for blocking requests, in milliseconds
    private int mBlockingTimeout = 1000;

    // Application callback registered in the constructor to the oem hook
    private ConnectionCallback mConnectionCallback;

    // Separate thread that handles asynchronous callbacks
    private ServiceHandlerThread mAsyncHandlerThread;

    // is the hook message STE specific
    private final boolean mIsSTEHook;

    public STErilOemHook(Context context, ConnectionCallback connectionCallback) {
        this(context, connectionCallback, false);
    }

    public STErilOemHook(Context context,
                                ConnectionCallback connectionCallback, boolean isSTEhook) {
        mContext = context;
        mConnectionCallback = connectionCallback;
        mIsSTEHook = isSTEhook;

        if (context == null
                || !context.bindService(
                    new Intent("com.stericsson.ril.oem.service.STErilOemService"),
                                                        mConnection, Context.BIND_AUTO_CREATE)) {
            Log.e(TAG, "Context was null or unable to bind to service!");
            cleanup();
        }


    }

    /**
     * Sends an OEM RIL message.
     *
     * @param requestId
     *            OEM specific identifier for request.
     * @param data
     *            Raw OEM request as byte array
     * @returns void
     * @throws RemoteException
     */
    public void sendRilOemHookMsgAsync(final int requestId, final byte[] data)
                                                                        throws RemoteException {
        // Only send if the service is connected and using a valid request id
        if (mServiceHandler == null || requestId == ServiceHandler.RESERVED_REQUEST_ID)
            return;

        mServiceHandler.sendOemHookMessage(requestId, data);
    }

    /**
     * Sends an OEM RIL message. Blocking.
     *
     * @param data Raw OEM request as byte array
     *
     * @return Null if the service is not connected or if there are async calls
     *         waiting on a response. byte[] otherwise.
     * @throws TimeoutException (Default timeout is 1 second but can be modified by
     *              {@link setTimeout(int)}), RemoteException
     */
    public byte[] sendRilOemHookMsg(final byte[] data) throws TimeoutException, RemoteException {
        // Don't try to send if the service is not connected
        if (mServiceHandler == null)
            return null;

        return mServiceHandler.sendOemHookMessageWithResponse(data);
    }

    /**
     * Sets the timeout for blocking requests
     *
     * @param timeout Timeout in milliseconds
     */
    public void setTimeout(int timeout) {
        if (timeout > 1000)
            mBlockingTimeout = timeout;
    }

    protected void finalize() {
        disconnect();
    }

    /**
     * Disconnects from service
     */
    public void disconnect() {
        if (mServiceHandler != null) {

            // Detach our existing connection.
            if (mContext != null)
                mContext.unbindService(mConnection);

        } else {
            Log.i(TAG, "disconnect() Service already disconnected");
        }

        // Do cleanup
        cleanup();
    }

    /**
     * Cleanup resources and send relevant callbacks.
     */
    private void cleanup() {
        // Notify application on disconnection
        if (mConnectionCallback != null) {
            mConnectionCallback.onDisconnected();
            mConnectionCallback = null;
        }

        if (mServiceHandler != null) {
            mServiceHandler.dispose();
            mServiceHandler = null;
        }

        // Kill the async thread
        if (mAsyncHandlerThread != null) {
            mAsyncHandlerThread.quit();
            mAsyncHandlerThread = null;
        }
    }

    /**
     * Class for interacting with the main interface of the service
     */
    private ServiceConnection mConnection = new ServiceConnection() {

        public void onServiceConnected(ComponentName className, IBinder service) {

            Log.d(TAG, "Connected to service!");
            /* Start the AsyncHandlerThread that receives incoming messages */
            mAsyncHandlerThread = new ServiceHandlerThread(new Messenger(service));
            mAsyncHandlerThread.start();
            mServiceHandler = mAsyncHandlerThread.getServiceHandler();

            // Notify application on connection
            if (mConnectionCallback != null)
                mConnectionCallback.onConnected();
        }

        public void onServiceDisconnected(ComponentName className) {
            cleanup();  // pack up and leave
        }
    };

    /**
     * Simple container representing a result from a synchronous call
     * to {@link sendRilOemHookMsg(byte[])}.
     */
    private class SyncResult {
        public byte[] data;
        public boolean isDataReady;
    }

    /**
     * Handles messages posted from the service callback functions. Passes
     * responses to application.
     */
    private class ServiceHandlerThread extends HandlerThread {

        private ServiceHandler mHandler;
        private final Object mStartupLock = new Object();
        private boolean mIsStarted = false;
        private Messenger mTmpMessenger;

        public ServiceHandlerThread(Messenger serviceMessenger) {
            super("STErilOemHook$AsyncHandlerThread");
            mTmpMessenger = serviceMessenger;
        }

        public ServiceHandler getServiceHandler() {
            synchronized (mStartupLock) {
                try {
                    if (!mIsStarted)
                        mStartupLock.wait();
                } catch (InterruptedException ie) {
                    Log.e(TAG, "Got interrupted while waiting for handler thread to start: " +
                                                                                  ie.getMessage());
                    mHandler = null;
                }
            }
            return mHandler;
        }

        @Override
        protected void onLooperPrepared() {

            synchronized (mStartupLock) {
                mHandler = new ServiceHandler(mTmpMessenger);
                mTmpMessenger = null;
                mIsStarted = true;
                mStartupLock.notifyAll();
            }
        }
    }

    private class ServiceHandler extends Handler {
        public static final int RESERVED_REQUEST_ID = -1; /* Reserved for internal use */
        private Messenger mServiceMessenger;
        private Messenger mMessenger;
        private final SyncResult mOngoingRequest = new SyncResult();

        public ServiceHandler(Messenger serviceMessenger) {
            mServiceMessenger = serviceMessenger;
            mMessenger = new Messenger(this);
        }


        public void sendOemHookMessage(final int requestId, final byte[] data)
                                                                    throws RemoteException {
            Message msg = Message.obtain(null, STErilOemHookConstants.SEND_OEM_HOOK_MSG);
            msg.replyTo = mMessenger;

            msg.arg1 = requestId;
            msg.arg2 = mIsSTEHook ? STErilOemHookConstants.STE_TAG : STErilOemHookConstants.OEM_TAG;
            Bundle b = new Bundle();
            b.putByteArray("data", data);
            msg.obj = b;

            try {
                mServiceMessenger.send(msg);
            } catch (RemoteException e) {
                Log.e(TAG, "Failed to send message to service! - " + e.getMessage());
                throw e;
            }
        }

        public byte[] sendOemHookMessageWithResponse(final byte[] data)
                                                    throws TimeoutException, RemoteException {

            synchronized (mOngoingRequest) {

                mOngoingRequest.data = null;
                mOngoingRequest.isDataReady = false;

                sendOemHookMessage(RESERVED_REQUEST_ID, data);

                if (!mOngoingRequest.isDataReady) {
                    try {
                        mOngoingRequest.wait(mBlockingTimeout);
                    } catch (InterruptedException ie) {
                        Log.w(TAG, "Got interrupted while waiting for OEM RIL response");
                        // Nothing to do here, treat it as a timeout below
                    }
                }

                if (mOngoingRequest.isDataReady)
                    return mOngoingRequest.data;
            }

            // No result received, throw timeout
            throw new TimeoutException("No result returned from OEM RIL");
        }

        public void dispose() {
            mServiceMessenger = null;
            mMessenger = null;
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {

            case STErilOemHookConstants.RESPONSE_MSG:

                if (msg.obj == null || !Bundle.class.isInstance(msg.obj)) {
                    Log.w(TAG, "Received RESPONSE_MSG with invalid data! Ignoring..");
                    break;
                }

                if (msg.arg1 == RESERVED_REQUEST_ID) { /* synchronous request */
                   synchronized (mOngoingRequest) {
                       mOngoingRequest.data = ((Bundle)msg.obj).getByteArray("data");
                       mOngoingRequest.isDataReady = true;
                       mOngoingRequest.notify();
                   }
                } else if (mConnectionCallback != null) {
                    // Pass result back to application
                    mConnectionCallback.onResult(msg.arg1, ((Bundle)msg.obj).getByteArray("data"));
                } else {
                    Log.e(TAG, "ConnectionCallback was null.  Could not deliver async result.");
                }

                break;
            default:
                super.handleMessage(msg);
            }
        }
    }
}
