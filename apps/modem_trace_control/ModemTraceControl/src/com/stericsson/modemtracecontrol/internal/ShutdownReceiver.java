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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.stericsson.modemtracecontrol.test.CommandServerHelper;

/**
 * Listens for the ACTION_SHUTDOWN event which the Android system sends when powering off.
 * When such an event is received, this class either sends a command to the server (MLR)
 * to generate a trigger report or does nothing.
 */
public class ShutdownReceiver extends BroadcastReceiver {

    // Socket Client to connect to MLR
    CommandSender mCommandSender = null;

    // CommandServer for testing standalone
    CommandServerHelper mCommandServer = null;

    private static boolean doTriggerOnPoweroff = true;
    private static boolean isTraceOngoing = false;
    private boolean isTriggerReportCreated = false;

    /**
     * This callback is the one being called when an ACTION_SHUTDOWN event is received.
     */
    @Override
    public void onReceive(Context context, Intent intent) {
        if (isTraceOngoing && doTriggerOnPoweroff) {
            TriggerReport();
        }
    }

    /**
     * Sets whether a trigger should be generated on power off or not.
     * @param enable true if a trigger report should be generated, false otherwise.
     */
    public static void setTriggerOnPoweroff(boolean enable) {
        doTriggerOnPoweroff = enable;
    }

    /**
     * Sets whether trace is ongoing or not.
     * @param isOngoing true if trace to sdcard is ongoing, false otherwise.
     */
    public static void setTraceOngoing(boolean isOngoing) {
        isTraceOngoing = isOngoing;
    }

    /**
     * Returns whether a trigger report has been created or not.
     * Used for test only.
     * @return true if a trigger report has been created, false otherwise.
     */
    public boolean getIsTriggerReportCreated() {
        return this.isTriggerReportCreated;
    }

    /**
     * Initializes connection to the server (MLR).
     *
     * @return SUCCESS on successful connection, FAILURE on error
     */
    private int Initialize() {
        int returnCode;

        if (mCommandSender == null) {
            mCommandSender = new CommandSender();
        }

        // TODO: This is used for testing on emulator
        if (Utility.DEBUG_PROGRAM) {
            if (mCommandServer == null) {
                mCommandServer = new CommandServerHelper();
            }
            mCommandServer.startServer();
            try {
                Thread.sleep(2000);
            } catch (InterruptedException ex) {
                Log.d(Utility.APP_NAME, "Sleep()");
            }
        }

        // Connect to the MLR in the device
        returnCode = mCommandSender.Connect();
        if (returnCode == Utility.FAILURE) {
            Log.e(Utility.APP_NAME,
                    "Failed to initialize client socket during trigger on power off!");
            Deinitialize();
            return returnCode;
        }
        return Utility.SUCCESS;
    }

    /**
     * Deinitializes connection to the server (MLR).
     *
     * @return SUCCESS on successful disconnection, FAILURE on error
     */
    private int Deinitialize() {
        if (mCommandSender != null && mCommandSender.Disconnect() == Utility.FAILURE) {
            Log.d(Utility.APP_NAME,
                    "Failed to disconnect from command socket during trigger on power off.");
            return Utility.FAILURE;
        }
        if (Utility.DEBUG_PROGRAM) {
            if (mCommandServer != null) {
                mCommandServer.setClientConnected(false);
                mCommandServer.stopServer();
            }
        }
        return Utility.SUCCESS;
    }

    /**
     * Triggers a modem trace report
     */
    private void TriggerReport() {
        int returnCode;

        // Connect to command socket
        returnCode = Initialize();
        if (returnCode == Utility.FAILURE) {
            return;
        }

        // Trigger report command
        returnCode = mCommandSender.SendCommand(Utility.TRACE_TRIGGER_COMMAND);
        if (returnCode == Utility.FAILURE) {
            Deinitialize();
            return;
        }

        // Disconnect from command socket
        returnCode = Deinitialize();
        if (returnCode == Utility.FAILURE) {
            return;
        }

        this.isTriggerReportCreated = true;
    }

}
