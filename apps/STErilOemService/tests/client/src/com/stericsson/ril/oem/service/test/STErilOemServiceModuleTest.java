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

package com.stericsson.ril.oem.service.test;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.RemoteException;
import android.test.ActivityInstrumentationTestCase2;
import android.test.suitebuilder.annotation.MediumTest;

import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.util.Arrays;
import java.util.concurrent.TimeoutException;

import com.stericsson.ril.oem.STErilOemHook;
import com.stericsson.ril.oem.STErilOemHookConstants;
import com.stericsson.ril.oem.service.test.client.TestActivity;


/**
 * Modules tests for STErilOemService and STErilOemHook
 */
public class STErilOemServiceModuleTest
            extends ActivityInstrumentationTestCase2<TestActivity>
            implements STErilOemHook.ConnectionCallback {
    public static final String TAG = "STErilOemServiceModuleTest";

    protected Context mContext;
    protected STErilOemHook mHook;
    protected Object mConnectionWaitLock = new Object();
    protected boolean mIsConnected = false;
    protected OemHookResult mResult = new OemHookResult();
    protected int mDefaultTimeout = 5000;  // Five second default timeout.

    public STErilOemServiceModuleTest() {
        super(TestActivity.class);
    }

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mContext = getInstrumentation().getContext();
        mResult.clear();
        mHook = new STErilOemHook(mContext, this);
    }


    @Override
    protected void tearDown() throws Exception {
        mHook.disconnect();
        waitForDisconnect();
        mHook = null;
        super.tearDown();
    }

    /* ConnectionCallback */
    public void onConnected() {
        if (mConnectionWaitLock == null)
            return;

        synchronized(mConnectionWaitLock) {
            mIsConnected = true;
            mConnectionWaitLock.notifyAll();
        }
    }
    public void onDisconnected() {
        if (mConnectionWaitLock == null)
            return;

        synchronized(mConnectionWaitLock) {
            mIsConnected = false;
            mConnectionWaitLock.notifyAll();
        }
    }
    public void onResult(int requestId, byte[] data) {
        synchronized(mResult) {
            mResult.setResult(requestId, data);
            mResult.notifyAll();
        }
    }

    /* Utility methods and classes*/
    public class OemHookResult {
        public int mRequestId;
        public byte[] mData;
        public boolean mIsReady;

        public OemHookResult() {
            clear();
        }

        public void setResult(int requestId, byte[] data) {
            mRequestId = requestId;
            mData = data;
            mIsReady = true;
        }

        public void clear() {
            mRequestId = 0;
            mData = null;
            mIsReady = false;
        }
    }

    /* Broadcast Receiver for receiving unsolicited OEM hook responses */
    public class UnsolResponseReceiver extends BroadcastReceiver {
        private String mLastIntentAction;
        private OemHookResult mLastResult;

        public UnsolResponseReceiver(OemHookResult resultStorage) {
            mLastResult = resultStorage;
        }

        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "Received broadcast");

            synchronized(mLastResult) {
                mLastIntentAction = intent.getAction();
                mLastResult.setResult(0, intent.getByteArrayExtra("data"));
                mLastResult.notifyAll();
            }
        }

        public String getLastIntentAction() {
            synchronized (mLastResult) {
                Log.i(TAG, "getLastIntentAction() returning: " + mLastResult);
                return mLastIntentAction;
            }
        }
    }

    public boolean waitForConnect() {
        synchronized(mConnectionWaitLock) {
            try {
                if (!mIsConnected) {
                    mConnectionWaitLock.wait(mDefaultTimeout); // Wait max five seconds
                }

                if (!mIsConnected) {
                    return false;
                }
            } catch (InterruptedException e) {
                return false;
            }
        }
        return true; // already connected
    }

    public boolean waitForDisconnect() {
        synchronized(mConnectionWaitLock) {
            try {
                if (mIsConnected) {
                    mConnectionWaitLock.wait(mDefaultTimeout); // Wait max five seconds
                }

                if (mIsConnected) {
                    return false;
                }
            } catch (InterruptedException e) {
                return false;
            }
        }
        return true; // already disconnected
    }

    public byte[] createTestData() {
        byte[] testData = null;
        try {
            testData = "2BADC0FFEE".getBytes("US-ASCII");

        } catch (UnsupportedEncodingException e) {
            // won't happen for ascii
        }
        return testData;
    }

    public byte[] prependOemTagToData(byte oemTag, byte[] testPayload) {
        byte[] testData = new byte[testPayload.length + 1];
        testData[0] = oemTag;
        System.arraycopy(testPayload, 0, testData, 1, testPayload.length);
        return testData;
    }

    /* Tests */
    @MediumTest
    public void testConnect() {
        assertTrue(waitForConnect());
    }

    @MediumTest
    public void testDisconnect() {
        assertTrue(waitForConnect());
        mHook.disconnect();
        assertTrue(waitForDisconnect());
    }

    @MediumTest
    public void testSendRilOemHookAsync() throws RemoteException, InterruptedException {
        assertTrue(waitForConnect());
        final byte[] data = createTestData();
        final int requestId = 10;
        Log.d(TAG, "sending test data: " + (data != null ? new String(data) : "null"));
        mResult.clear();
        mHook.sendRilOemHookMsgAsync(requestId, data);

        if (!mResult.mIsReady) {
            synchronized (mResult) {
                mResult.wait(mDefaultTimeout);
            }
            assertTrue(mResult.mIsReady);
        }

        assertEquals("Wrong request ID for response", requestId, mResult.mRequestId);
        assertTrue("Wrong data in async response", Arrays.equals(data, mResult.mData));

    }

    @MediumTest
    public void testSendRilOemHookSync() throws RemoteException, TimeoutException {
        assertTrue(waitForConnect());
        final byte[] data = createTestData();
        mHook.setTimeout(mDefaultTimeout);
        byte[] result = mHook.sendRilOemHookMsg(data);
        assertNotNull(result);
        assertTrue("Wrong data in sync response", Arrays.equals(data, result));
    }

    @MediumTest
    public void testUnsolRilOemResponse() throws InterruptedException {
        assertTrue(waitForConnect());

        final String oemAction = STErilOemHookConstants.ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW;
        final String steAction = STErilOemHookConstants.ACTION_UNSOL_RESPONSE_STE_HOOK_RAW;
        IntentFilter filter = new IntentFilter(oemAction);
        filter.addAction(steAction);


        // send a request to the mock phone to trigger an unsolicited response
        final byte[] testData = createTestData();
        final byte[] oemData = prependOemTagToData(STErilOemHookConstants.OEM_TAG, testData);
        final byte[] steData = prependOemTagToData(STErilOemHookConstants.STE_TAG, testData);

        Intent oemIntent = new Intent("com.stericsson.ril.oem.service.TRIGGER_USOL_RESPONSE");
        oemIntent.putExtra("data", oemData);

        Intent steIntent = new Intent("com.stericsson.ril.oem.service.TRIGGER_USOL_RESPONSE");
        steIntent.putExtra("data", steData);

        String oemRespAction = null;
        byte[] oemRespData = null;

        String steRespAction = null;
        byte[] steRespData = null;

        OemHookResult response = new OemHookResult();
        // setup receiver
        UnsolResponseReceiver receiver = new UnsolResponseReceiver(response);

        mContext.registerReceiver(receiver, filter);
        mContext.startService(oemIntent);

        // wait for a response
        synchronized(response) {
            if (!response.mIsReady) {
                response.wait(mDefaultTimeout);
            }
            if (response.mIsReady) {
                oemRespAction = receiver.getLastIntentAction();
                oemRespData = response.mData;
            }

            response.mIsReady = false; /* reset data */
            response.mData = null;
        }

        mContext.startService(steIntent);

        // wait for a response
        synchronized(response) {
            if (!response.mIsReady) {
                response.wait(mDefaultTimeout);
            }
            if (response.mIsReady) {
                steRespAction = receiver.getLastIntentAction();
                steRespData = response.mData;
            }
        }

        // careful not to leak the receiver
        mContext.unregisterReceiver(receiver);


        assertNotNull("Timeout waiting for OEM response", oemRespData);
        assertNotNull("Timeout waiting for STE response", steRespData);

        assertEquals("Did not receive expected OEM intent", oemAction, oemRespAction);
        assertEquals("Did not receive expected STE intent", steAction, steRespAction);

        assertTrue("Wrong data in unsolicited OEM response.\n" +
                "Expected: " + String.valueOf(testData) + " Got: " + String.valueOf(oemRespData),
                                      Arrays.equals(testData, oemRespData));
        assertTrue("Wrong data in unsolicited STE response.\n" +
                "Expected: " + String.valueOf(testData) + " Got: " + String.valueOf(steRespData),
                                      Arrays.equals(testData, steRespData));

    }


}