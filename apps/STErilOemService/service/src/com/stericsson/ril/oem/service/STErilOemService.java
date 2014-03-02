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


package com.stericsson.ril.oem.service;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Bundle;
import android.os.Messenger;
import android.os.Process;
import android.os.RemoteException;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import android.os.AsyncResult;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import com.stericsson.ril.oem.STErilOemHookConstants;

import java.util.Arrays;

/**
 * Implements a service allowing applications running outside com.android.phone
 * process to access invokeOemRilRequestRaw and setOnUnsolOemHookRaw.
 */
public class STErilOemService extends Service {

    private static final int OEM_RESPONSE_MSG = 0xF0;
    private static final int OEM_UNSOLICITED_MSG = 0xF1;


    private static final String TAG = "STErilOemService";
    private static final String mPermission = "com.stericsson.permission.SEND_RIL_OEM_HOOK";


    // protected to allow for mocking
    protected Phone mPhone = null;

    @Override
    public void onCreate() {
        super.onCreate();

        initPhone();

        // Register for unsolicited oem hook raw events
        mPhone.setOnUnsolOemHookRaw(mHandler, OEM_UNSOLICITED_MSG, null);

        Log.i(TAG, "service started");
    }

    @Override
    public void onDestroy() {

        Log.i(TAG, "service stopped");
        super.onDestroy();
    }

    /* Protected to allow for mocking during test */
    protected void initPhone() {

        // Initialize phones
        PhoneFactory.makeDefaultPhones(this);

        // Get reference to the default phone
        mPhone = PhoneFactory.getDefaultPhone();
    }

    /**
     * Handler for OEM request responses
     */
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            /* We got a OEM Hook message from a client.
             * Forward it onwards to the RIL.
             */
            case STErilOemHookConstants.SEND_OEM_HOOK_MSG:

                if (msg.replyTo == null) {
                    Log.w(TAG, "Received OEM_HOOK_MSG without replyTo. Ignoring..");
                    break;
                }

                if (msg.obj == null || !Bundle.class.isInstance(msg.obj)) {
                    Log.w(TAG, "Received OEM_HOOK_MSG with invalid data. Ignoring..");
                    break;
                }

                Message msgRequest = mHandler.obtainMessage(OEM_RESPONSE_MSG);
                msgRequest.arg1 = msg.arg1;
                msgRequest.obj = msg.replyTo;
                byte[] payload = ((Bundle)msg.obj).getByteArray("data");
                byte[] payloadWithTag = new byte[payload.length + 1];

                /* Add the OEM tag to the data */
                payloadWithTag[0] = (byte) (msg.arg2 & 0xFF);
                System.arraycopy(payload, 0, payloadWithTag, 1, payload.length);

                Log.d(TAG, "invoking OEM Hook Raw..");
                mPhone.invokeOemRilRequestRaw(payloadWithTag, msgRequest);

                break;

            case OEM_RESPONSE_MSG:
                AsyncResult ar = (AsyncResult) msg.obj;

                try {
                    Message msgResponse = Message.obtain(null, STErilOemHookConstants.RESPONSE_MSG);
                    Messenger client = (Messenger)ar.userObj;

                    msgResponse.arg1 = msg.arg1; /* client tag */
                    Bundle b = new Bundle();
                    if (ar.exception != null) {
                        Log.e(TAG, "OEM Hook RAW caused exception: " + ar.exception.getMessage());
                    } else {
                        Log.d(TAG, "Got OEM Hook Raw response");
                        b.putByteArray("data", (byte[])ar.result);
                    }
                    msgResponse.obj = b;

                    client.send(msgResponse);
                } catch (RemoteException e) {
                    Log.e(TAG, "Could not deliver Response Message", e);
                }

                break;

            // Unsolicited oem raw
            case OEM_UNSOLICITED_MSG:
                Log.i(TAG, "Received unsolicited oem raw, sending broadcast");

                // Grab unsolicited data from result
                AsyncResult unsolResult = (AsyncResult) msg.obj;

                // Intent for broadcast
                Intent intent;

                byte oemTag = ((byte[])unsolResult.result)[0];

                Log.d(TAG, "Got OEM Tag: " + oemTag);
                if (oemTag == STErilOemHookConstants.STE_TAG) {
                    Log.i(TAG, "Sending STE HOOK RAW intent");
                    intent = new Intent(STErilOemHookConstants.ACTION_UNSOL_RESPONSE_STE_HOOK_RAW);
                } else {
                    Log.i(TAG, "Sending OEM HOOK RAW intent");
                    intent = new Intent(STErilOemHookConstants.ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW);
                }

                byte[] data = Arrays.copyOfRange((byte[]) unsolResult.result, 1,
                                                            ((byte[]) unsolResult.result).length);
                // Add unsolicited data to intent
                intent.putExtra("data", data);

                // Broadcast intent
                sendBroadcast(intent, mPermission);

                break;

            default:
                Log.w(TAG, "Received unknown message, msg.what: " + msg.what);
                super.handleMessage(msg);
                break;
            }
        }
    };

    private final Messenger mMessenger = new Messenger(mHandler);

    @Override
    public IBinder onBind(Intent intent) {
        // Return service interface
        if (STErilOemService.class.getName().equals(intent.getAction())) {
            Log.i(TAG, "Attached new client");
            return mMessenger.getBinder();
        }

        return null;
    }

}
