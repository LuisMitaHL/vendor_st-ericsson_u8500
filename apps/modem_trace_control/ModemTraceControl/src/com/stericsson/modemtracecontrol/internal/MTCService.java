/*
 * Copyright (C) ST-Ericsson SA 2010
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

package com.stericsson.modemtracecontrol.internal;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.StrictMode;
import android.util.Log;

import com.stericsson.modemtracecontrol.test.CommandServerHelper;

/**
 * Starts a service to display a notification if modem trace is on going.
 */
public class MTCService extends Service {

    // Socket Client to connect to MLR
    private CommandSender mCommandSender = null;

    // CommandServer for testing stand-alone
    private CommandServerHelper mCommandServer = null;

    private MTCNotification mNotification;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(Utility.APP_NAME, "MTC Service created");
        mNotification = MTCNotification.getInstance();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(Utility.APP_NAME, "MTC Service destroyed");
    }

    @Override
    public void onStart(Intent intent, int startid) {
        super.onStart(intent, startid);

        // Allow network access
        if (android.os.Build.VERSION.SDK_INT > 9) {
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll()
                    .build();
            StrictMode.setThreadPolicy(policy);
        }

        // Check for the SDCard logging, if modem trace is ongoing
        // display a notification
        if (isLoggingInProgress() == Utility.SUCCESS) {
            mNotification.CreateMTCNotification(this.getApplicationContext(),
                    "Select to stop modem tracing.", false);
            Log.d(Utility.APP_NAME, "MTCService : SDCard logging is ongoing");
        } else {
            Log.d(Utility.APP_NAME, "MTCService : SDCard logging is not ongoing");
        }

        Log.d(Utility.APP_NAME, "MTCService started");

        stopSelf();
    }

    /**
     * Checks if MLR is running on target
     *
     * @return SUCCESS on successfully connected, FAILURE on error
     */
    private int isLoggingInProgress() {

        int retCode = -1;

        if (mCommandSender == null) {
            mCommandSender = new CommandSender();
        }

        // It is used for testing on emulator
        if (Utility.DEBUG_PROGRAM) {
            if (mCommandServer == null) {
                mCommandServer = new CommandServerHelper();
            }
            mCommandServer.startServer();
        }

        if (mCommandSender != null) {

            // Connect to the MLR in the device
            retCode = mCommandSender.Connect();
            if (retCode == Utility.FAILURE) {
                Log.e(Utility.APP_NAME,
                        "MTCService : Failed to initialize client socket !");
                return Utility.FAILURE;
            }

            // Send query command
            retCode = mCommandSender.SendCommand(Utility.TRACE_SDCARD_LOGGING_COMMAND);

            // Disconnect
            if (mCommandSender.Disconnect() == Utility.FAILURE) {
                return Utility.FAILURE;
            }
        }

        if (Utility.DEBUG_PROGRAM) {
            if (mCommandServer != null) {
                mCommandServer.stopServer();
            }
        }

        return retCode;
    }
}
