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

import android.content.Intent;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Registrant;
import android.util.Log;

import java.util.Arrays;

import com.stericsson.ril.oem.service.STErilOemService;

/**
  * A thin wrapper class for STErilOemService that runs isolated from the telephony
  * framework. i.e it mocks the Phone interface that STErilOemService uses.
  */
public class IsolatedSTErilOemService extends STErilOemService {

    private static final String TAG = "IsolatedSTErilOemService";

    private MockPhone mMockPhone;

    @Override
    public void onCreate() {
        Log.i(TAG, "onCreate()");
        super.onCreate();
    }


    @Override
    public void onDestroy() {
        Log.i(TAG, "onDestroy()");
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(TAG, "onBind()");
        return super.onBind(intent);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "onStartCommand() flags: " + flags + " startId: " + startId);
        if ("com.stericsson.ril.oem.service.TRIGGER_USOL_RESPONSE".equals(intent.getAction())) {
            Log.i(TAG, "Trigger UnsolOemHookRaw request received");
            if (mMockPhone != null) {
                mMockPhone.triggerUnsolOemHookRaw(intent.getByteArrayExtra("data"));
            } else {
                Log.w(TAG, "MockPhone was null.  Can't trigger unsol response!");
            }
        }
        stopSelf(startId);
        return START_NOT_STICKY;
    }

    @Override
    protected void initPhone() {
        mMockPhone = new MockPhone();
        mPhone = mMockPhone;
    }

    public static class MockPhone extends MockPhoneBase {

        Registrant mUnsolRegistrant;

        public MockPhone() {}

        public void setOnUnsolOemHookRaw(Handler h, int what, Object obj) {
            Log.i(TAG, "MockPhone:setOnUnsolOemHookRaw");
            mUnsolRegistrant = new Registrant(h, what, obj);
        }

        public void unSetOnUnsolOemHookRaw(Handler h) {
            Log.i(TAG, "MockPhone:unSetOnUnsolOemHookRaw");
            mUnsolRegistrant.clear();
            mUnsolRegistrant = null;
        }


        public void invokeOemRilRequestRaw(byte[] data, Message response) {
            Log.i(TAG, "MockPhone:invokeOemRilRequestRaw");
            // Just send the data back directly
            // But strip the OEM TAG
            AsyncResult.forMessage(response, Arrays.copyOfRange(data, 1, data.length - 1), null);
            response.sendToTarget();
        }

        public void triggerUnsolOemHookRaw(byte[] data) {
            if (mUnsolRegistrant == null) {
                Log.w(TAG, "MockPhone:triggerUnsolOemHookRaw - Unsol recevier not registered!");
                return;
            }
            mUnsolRegistrant.notifyRegistrant(new AsyncResult(null, data, null));
        }
    }

}
